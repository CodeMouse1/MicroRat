
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "Dave.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Funktionsschnittstellen/movement.h"
#include "Funktionsschnittstellen/debug_comms.h"
#include <Funktionsschnittstellen/pd_regler.h>
#include <Funktionsschnittstellen/user_interface.h>
#include "Funktionsschnittstellen/timer_utils.h"
#include "Hardwaresteuerung/hal_ir.h"
#include "Hardwaresteuerung/hal_us.h"
#include "Hardwaresteuerung/hal_encoder.h"
#include "Hardwaresteuerung/hal_motor.h"
#include "Hardwaresteuerung/hal_digital_io.h"


uint8_t UART_String[500];
uint8_t DiagnosticSummaryString[100];

typedef struct {
    int raw_adc_value;
    int distance_mm;
} IrCalibrationPoint;

const IrCalibrationPoint ir_distance_lut[] = {
    // {ADC_Rohwert, Distanz_mm}
    {1815, 20},
    {1554, 25},
    {1317, 30},
    {1158, 35},
    {1032, 40},
    {957, 45},
    {880, 50},
    {801, 55},
    {735, 60},
    {700, 65},
    {645, 70},
    {581, 75},
    {538, 80},
    {451, 90},
    {395, 100}
};
const int IR_LUT_SIZE = sizeof(ir_distance_lut) / sizeof(ir_distance_lut[0]);

static int _convertIrRawToMm(int raw_value) {
    // Fall: Wert liegt unterhalb des Bereichs der Tabelle (Objekt sehr weit weg oder nicht vorhanden)
    // -> Gib den maximalen Distanzwert der LUT zurück.
    if (raw_value <= ir_distance_lut[IR_LUT_SIZE - 1].raw_adc_value) {
        return ir_distance_lut[IR_LUT_SIZE - 1].distance_mm;
    }
    // Fall: Wert liegt oberhalb des Bereichs der Tabelle (Objekt extrem nah)
    // -> Gib den minimalen Distanzwert der LUT zurück.
    if (raw_value >= ir_distance_lut[0].raw_adc_value) {
        return ir_distance_lut[0].distance_mm;
    }

    // Finde die zwei Punkte in der LUT, zwischen denen der raw_value liegt.
    // Die Tabelle ist absteigend nach ADC-Rohwert sortiert.
    for (int i = 0; i < IR_LUT_SIZE - 1; i++) {
        if (raw_value <= ir_distance_lut[i].raw_adc_value && raw_value >= ir_distance_lut[i+1].raw_adc_value) {
            // Lineare Interpolation zwischen Punkt i und Punkt i+1
            float raw_diff = (float)ir_distance_lut[i].raw_adc_value - ir_distance_lut[i+1].raw_adc_value;
            float dist_diff = (float)ir_distance_lut[i+1].distance_mm - ir_distance_lut[i].distance_mm;

            if (raw_diff == 0) { // Sollte nicht passieren, wenn LUT-Punkte einzigartig sind
                return ir_distance_lut[i].distance_mm;
            }

            // Interpolationsformel: dist = dist1 + (raw_value - raw1) * (dist2 - dist1) / (raw2 - raw1)
            // Beachte, dass wir raw_diff und dist_diff entsprechend gesetzt haben.
            float interpolated_distance = ir_distance_lut[i].distance_mm +
                                          ((float)raw_value - ir_distance_lut[i].raw_adc_value) * (dist_diff / raw_diff);

            // Zusätzliche Begrenzung auf den physikalischen Sensorbereich
            // Da die LUT selbst die Grenzen setzt, ist dies eher eine Redundanz oder für Extrapolationen.
            if (interpolated_distance < 20) interpolated_distance = 20;
            // Wenn deine LUT nur bis 100mm geht und du nicht möchtest, dass interpolierte Werte darüber liegen:
            if (interpolated_distance > ir_distance_lut[IR_LUT_SIZE-1].distance_mm) {
                interpolated_distance = ir_distance_lut[IR_LUT_SIZE-1].distance_mm;
            }

            return (int)round(interpolated_distance); // Runde auf den nächsten ganzen Millimeter
        }
    }
    return 0; // Dieser Fall sollte bei korrekter LUT und Range-Handling normalerweise nicht erreicht werden.
}

void SensorsInit(){
    EncoderStartLeft();
    EncoderStartRight();
    UltrasoundStart();
}

void SensorsPrint(){
	float distanz_ultra = GetDistanceFront_mm();
	int IR_L = GetDistanceLeft_mm();
	int IR_R = GetDistanceRight_mm();
	float count_L = GetEncoderLeft_mm();
	float count_R = GetEncoderRight_mm();
	int len = sprintf((char*)UART_String, " Ultraschall: %.2fmm IR_R: %dmm IR_L: %dmm  L: %.1fmm  R: %.1fmm \n\r", distanz_ultra, IR_R, IR_L, count_L, count_R);
	Debug_Comms_SendBuffer(UART_String, len);
}

bool IsWallLeft(void) {
    // Rückgabewert ist true, wenn der IR-Wert den Schwellenwert unterschreitet
    return GetDistanceLeft_mm() < WALL_DETECTION_THRESHOLD;
}

bool IsWallRight(void) {
    // Rückgabewert ist true, wenn der IR-Wert den Schwellenwert unterschreitet
    return GetDistanceRight_mm() < WALL_DETECTION_THRESHOLD;
}

bool IsWallFront(void) {
	// Rückgabewert ist true, wenn der US-Wert den Schwellenwert unterschreitet
    float distance = GetDistanceFront_mm();
    return (distance <= WALL_DETECTION_THRESHOLD);
}

float GetDistanceFront_mm(void) {
    return FrontRead()*10;
}

int GetDistanceLeft_mm(void) {
	int raw = ReadLeft(); // Lies den Roh-ADC-Wert über die HAL-Schicht
	return _convertIrRawToMm(raw) - IR_SENSOR_OFFSET_LEFT_TO_WHEEL_MM;
}

int GetDistanceRight_mm(void) {
	int raw = ReadRight(); // Lies den Roh-ADC-Wert über die HAL-Schicht
	return _convertIrRawToMm(raw) - IR_SENSOR_OFFSET_RIGHT_TO_WHEEL_MM;
}

float GetEncoderLeft_mm(void) {
    int raw_ticks = EncoderReadLeft();
    // Umrechnung von Ticks in Millimeter mit MM_PER_TICK (aus sensors.h)
    float distance_float = (float)raw_ticks * TICK_PER_MM;
    return (distance_float); // Auf ganzen Millimeter runden und zurückgeben
}

float GetEncoderRight_mm(void) {
    int raw_ticks = EncoderReadRight();
    // Umrechnung von Ticks in Millimeter mit MM_PER_TICK (aus sensors.h)
    float distance_float = (float)raw_ticks * TICK_PER_MM;
    return (distance_float); // Auf ganzen Millimeter runden und zurückgeben
}

bool PerformDiagnosticCheck(void) {
	const float MIN_ENCODER_DISTANCE_FOR_TEST = 10.0f;
	const int DIAG_TEST_PWM = 3500;
    bool all_ok = true;
    int offset = 0;
    offset += sprintf((char*)UART_String + offset, "\033[H\033[2J");

    // Lies die Distanzsensor-Werte ab
    float dist_front = GetDistanceFront_mm();
    int dist_left = ReadLeft();
    int dist_right = ReadRight();

    offset += sprintf((char*)UART_String + offset, "Initialer Sensor-Check:\n\r");

    // Distanzsensoren Check: Prüfen, ob die Werte im erwarteten Bereich liegen
    if (dist_front == 0) {
    	offset += sprintf((char*)UART_String + offset, "  FEHLER: Frontsensor kann nicht 0 sein, Versorgung prüfen (%.1fmm)\n\r", dist_front);
        all_ok = false;
    } else {
    	offset += sprintf((char*)UART_String + offset, "  OK: Frontsensor (%.1fmm)\n\r", dist_front);
    }

    if (dist_left > 2000) {
    	offset += sprintf((char*)UART_String + offset, "  FEHLER: Linker Sensor außer Bereich, Versorgung prüfen (%d)\n\r", dist_left);
        all_ok = false;
    } else {
    	offset += sprintf((char*)UART_String + offset, "  OK: Linker Sensor (%d)\n\r", dist_left);
    }

    if (dist_right > 2000) {
    	offset += sprintf((char*)UART_String + offset, "  FEHLER: Rechter Sensor außer Bereich, Versorgung prüfen (%d)\n\r", dist_right);
        all_ok = false;
    } else {
    	offset += sprintf((char*)UART_String + offset, "  OK: Rechter Sensor (%d)\n\r", dist_right);
    }
    // --- 2. Motor- und Encoder-Reaktionstest ---
    offset += sprintf((char*)UART_String + offset, "Motor-/Encoder-Reaktionstest:\n\r");

    EncoderReset();
    MotorsSetSpeed(DIAG_TEST_PWM, DIAG_TEST_PWM);
	Delay_ms(100);
    MotorsSetSpeed(0, 0);
	MotorsSetSpeed(-DIAG_TEST_PWM, -DIAG_TEST_PWM);
	Delay_ms(100);
	MotorsSetSpeed(0, 0);

    // Nach dem Test Encoder-Werte ablesen (jetzt sollten sie sich bewegt haben)
    float enc_L_end = GetDistanceLeft_mm();
    float enc_R_end = GetDistanceRight_mm();

    // Prüfen, ob Encoder überhaupt hochgezählt haben und die Mindestdistanz erreicht wurde
    if (fabs(enc_L_end) < MIN_ENCODER_DISTANCE_FOR_TEST) {
    	offset += sprintf((char*)UART_String + offset, "  FEHLER: Linker Encoder zu wenig Distanz (%.1fmm)\n\r", enc_L_end);
        all_ok = false;
    } else {
    	offset += sprintf((char*)UART_String + offset, "  OK: Linker Encoder (%.1fmm)\n\r", enc_L_end);
    }

    if (fabs(enc_R_end) < MIN_ENCODER_DISTANCE_FOR_TEST) {
    	offset += sprintf((char*)UART_String + offset, "  FEHLER: Rechter Encoder zu wenig Distanz (%.1fmm)\n\r", enc_R_end);
        all_ok = false;
    } else {
    	offset += sprintf((char*)UART_String + offset, "  OK: Rechter Encoder (%.1fmm)\n\r", enc_R_end);
    }
    // --- Abschließendes Ergebnis ---
    if (all_ok) {
    	SignalDiagnosticsResult(all_ok);
    } else {
        // Bei Fehler: Finalen Fehlerbericht in den Haupt-Log-String schreiben
        offset += sprintf((char*)UART_String + offset, "DIAGNOSE: Einige Checks FEHLGESCHLAGEN. Anschlüsse untersuchen.\n\r");
    }
    Debug_Comms_SendString((const char*)UART_String);
    if (!all_ok) {
            static unsigned int print_iteration_counter = 0;
            const unsigned int PRINT_UPDATE_INTERVAL = 10; // Beispiel: Alle 5 Schleifendurchläufe aktualisieren
    		while (1) {
    			SignalDiagnosticsResult(all_ok);
                if (print_iteration_counter >= PRINT_UPDATE_INTERVAL) {
					Debug_Comms_SendString((const char*)UART_String);
                    //for (volatile int i = 0; i < 5000000; i++) {} // Erhöhter Delay, z.B. 1.5 Millionen Zyklen
                    print_iteration_counter = 0; // Zähler zurücksetzen, um den nächsten Zählzyklus zu starten
                } else {
                    for (volatile int int_delay_loop = 0; int_delay_loop < 50000; int_delay_loop++) {}
                }

                print_iteration_counter++; // Zähler für jeden Schleifendurchlauf erhöhen
    		}
        }
    return all_ok;
}




