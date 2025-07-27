/**
 * @file sensors.c
 * @brief Implementierung der Sensor-Auslese-, Umrechnungs- und Diagnosefunktionen.
 *
 * Dieses Modul stellt die Schnittstelle zu den verschiedenen Sensoren des MicroRats dar,
 * einschließlich Infrarotsensoren (IR), Ultraschallsensoren (US) und Encodern.
 * Es kapselt die Hardware-Abstraktionsschicht (HAL) der Sensoren und bietet
 * Funktionen zur Umrechnung von Rohdaten in physikalische Einheiten (mm) sowie
 * zur Erkennung von Wänden.
 *
 * Darüber hinaus enthält es eine Funktion zur Durchführung eines umfassenden
 * Diagnosetests aller wichtigen Sensoren und Aktuatoren, um die Betriebsbereitschaft
 * der MicroRat zu überprüfen und Probleme zu identifizieren.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 *
 * @dependencies
 * - <stdbool.h>: Für den 'bool'-Datentyp
 * - <stdio.h>: Für Standard-E/A-Funktionen
 * - <math.h>: Für mathematische Funktionen
 * - "Funktionsschnittstellen/sensors.h": Deklarationen der öffentlichen Schnittstellen dieses Moduls
 * - "Funktionsschnittstellen/movement.h": Für die Steuerung der Motoren während des Diagnosetests
 * - "Funktionsschnittstellen/debug_comms.h": Für die Ausgabe von Debug-Informationen und Diagnosestatus
 * - "Funktionsschnittstellen/user_interface.h": Für die visuelle Signalisierung des Diagnosestatus
 * - "Hardwaresteuerung/hal_ir.h": Hardware-Abstraktionsschicht für Infrarotsensoren
 * - "Hardwaresteuerung/hal_us.h": Hardware-Abstraktionsschicht für Ultraschallsensoren
 * - "Hardwaresteuerung/hal_encoder.h": Hardware-Abstraktionsschicht für Encoder
 * - "Hardwaresteuerung/hal_motor.h": Hardware-Abstraktionsschicht für Motoren
 * - "Hardwaresteuerung/hal_digital_io.h": Hardware-Abstraktionsschicht für digitale E/A
 */
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "Funktionsschnittstellen/sensors.h"
#include "Funktionsschnittstellen/movement.h"
#include "Funktionsschnittstellen/debug_comms.h"
#include "Funktionsschnittstellen/system_interface.h"
#include "Funktionsschnittstellen/user_interface.h"
#include "Hardwaresteuerung/hal_ir.h"
#include "Hardwaresteuerung/hal_us.h"
#include "Hardwaresteuerung/hal_encoder.h"
#include "Hardwaresteuerung/hal_motor.h"

uint8_t UART_String[500];
uint8_t DiagnosticSummaryString[100];

/**
 * @brief Struktur zur Definition eines Kalibrierungspunktes für Infrarotsensoren.
 *
 * Ein Kalibrierungspunkt besteht aus einem rohen ADC-Wert des IR-Sensors
 * und der zugehörigen physikalischen Distanz in Millimetern.
 * Diese Punkte werden in einer Lookup-Tabelle für die Distanzumrechnung verwendet.
 */
typedef struct {
    int raw_adc_value;
    int distance_mm;
} IrCalibrationPoint;

/**
 * @brief Lookup-Tabelle (LUT) für die Umrechnung von IR-Rohwerten in Distanzen.
 *
 * Diese Konstante Array enthält eine Reihe von Kalibrierungspunkten.
 * Die Tabelle ist nach 'raw_adc_value' **absteigend** sortiert, da mit abnehmendem Abstand (näher am Objekt)
 * der ADC-Rohwert tendenziell zunimmt (IR-Intensität steigt).
 * Die gespeicherten Informationen spiegeln die Messwerte des Sensors von 20mm bis 100mm wider.
 *
 * @note Diese Werte müssen sorgfältig für den verwendeten IR-Sensor kalibriert werden.
 */
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

/**
 * @brief Konvertiert einen rohen IR-ADC-Wert in eine Distanz in Millimetern mittels linearer Interpolation.
 *
 * Diese statische Hilfsfunktion nimmt einen rohen ADC-Wert eines Infrarotsensors entgegen
 * und wandelt ihn unter Verwendung der 'ir_distance_lut' in eine physikalische Distanz
 * in Millimetern um. Es wird eine lineare Interpolation verwendet, wenn der Wert
 * zwischen zwei Kalibrierungspunkten liegt. Werte außerhalb des LUT-Bereichs werden
 * auf die minimalen oder maximalen Distanzen der Tabelle begrenzt.
 *
 * @param raw_value Der Rohwert des IR-Sensors (ADC-Wert).
 * @return Die berechnete Distanz in Millimetern [mm]. Gibt 0 zurück, falls ein unerwarteter Fehler auftritt.
 */
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
            float interpolated_distance = ir_distance_lut[i].distance_mm +((float)raw_value - ir_distance_lut[i].raw_adc_value) * (dist_diff / raw_diff);
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
    return 0;
}

/**
 * @brief Initialisiert alle Sensoren des MicroRats.
 *
 * Startet die Encoder auf beiden Rädern und initialisiert den Ultraschallsensor.
 * Diese Funktion muss vor dem ersten Auslesen der Sensordaten aufgerufen werden.
 */
void Sensors_Init(){
    EncoderStartLeft();
    EncoderStartRight();
    UltrasoundStart();
}

/**
 * @brief Gibt die aktuellen Sensorwerte über die Debug-Kommunikationsschnittstelle aus.
 *
 * Liest die Distanzen der Ultraschall- und IR-Sensoren sowie die Encoder-Werte
 * beider Räder ab und formatiert sie in einen String. Dieser String wird dann
 * über UART gesendet, um eine Echtzeitüberwachung der Sensordaten zu ermöglichen.
 *
 * @note Die Encoder-Werte werden hier als "Count" bezeichnet, obwohl sie bereits
 * in Millimeter umgerechnet sind. Die Ausgabe erfolgt gerundet auf eine Nachkommastelle.
 */
void SensorsPrint(){
	float distanz_ultra = GetDistanceFront_mm();
	int IR_L = GetDistanceLeft_mm();
	int IR_R = GetDistanceRight_mm();
	float count_L = GetEncoderLeft_mm();
	float count_R = GetEncoderRight_mm();
	int len = sprintf((char*)UART_String, " Ultraschall: %.2fmm IR_R: %dmm IR_L: %dmm  L: %.1fmm  R: %.1fmm \n\r", distanz_ultra, IR_R, IR_L, count_L, count_R);
	Debug_Comms_SendBuffer(UART_String, len);
}

/**
 * @brief Prüft, ob der linke IR-Sensor eine Wand detektiert.
 *
 * Der Rückgabewert ist 'true', wenn die vom linken IR-Sensor gemessene Distanz
 * unter einem vordefinierten Schwellenwert ('WALL_DETECTION_THRESHOLD') liegt.
 * Dieser Schwellenwert definiert, ab welcher Nähe ein Objekt als Wand interpretiert wird.
 *
 * @return 'true' wenn eine Wand links detektiert wird; 'false' sonst.
 * @see WALL_DETECTION_THRESHOLD
 */
bool IsWallLeft(void) {
    // Rückgabewert ist true, wenn der IR-Wert den Schwellenwert unterschreitet
    return GetDistanceLeft_mm() < WALL_DETECTION_THRESHOLD;
}

/**
 * @brief Prüft, ob der rechte IR-Sensor eine Wand detektiert.
 *
 * Der Rückgabewert ist 'true', wenn die vom rechten IR-Sensor gemessene Distanz
 * unter einem vordefinierten Schwellenwert ('WALL_DETECTION_THRESHOLD') liegt.
 *
 * @return 'true' wenn eine Wand rechts detektiert wird; 'false' sonst.
 * @see WALL_DETECTION_THRESHOLD
 */
bool IsWallRight(void) {
    // Rückgabewert ist true, wenn der IR-Wert den Schwellenwert unterschreitet
    return GetDistanceRight_mm() < WALL_DETECTION_THRESHOLD;
}

/**
 * @brief Prüft, ob der vordere Ultraschallsensor eine Wand detektiert.
 *
 * Der Rückgabewert ist 'true', wenn die vom Ultraschallsensor gemessene Distanz
 * kleiner oder gleich einem vordefinierten Schwellenwert ('WALL_DETECTION_THRESHOLD') ist.
 *
 * @return 'true' wenn eine Wand vorne detektiert wird; 'false' sonst.
 * @see WALL_DETECTION_THRESHOLD
 */
bool IsWallFront(void) {
	// Rückgabewert ist true, wenn der US-Wert den Schwellenwert unterschreitet
    float distance = GetDistanceFront_mm();
    return (distance <= WALL_DETECTION_THRESHOLD);
}

/**
 * @brief Liest die Distanz des vorderen Ultraschallsensors in Millimetern.
 *
 * Ruft die Hardware-Abstraktionsschicht des Ultraschallsensors auf ('FrontRead()')
 * und konvertiert den rohen Messwert in Millimeter.
 *
 * @return Die gemessene Distanz in Millimetern [mm].
 */
float GetDistanceFront_mm(void) {
    return FrontRead()*10;
}

/**
 * @brief Liest die Distanz des linken IR-Sensors in Millimetern.
 *
 * Ruft die Hardware-Abstraktionsschicht des linken IR-Sensors auf ('ReadLeft()')
 * und konvertiert den rohen ADC-Wert mittels der internen LUT ('_convertIrRawToMm()')
 * in Millimeter. Abschließend wird ein Offset ('IR_SENSOR_OFFSET_LEFT_TO_WHEEL_MM')
 * subtrahiert, um die Distanz relativ zur Radachse oder einer anderen Referenz zu erhalten.
 *
 * @return Die berechnete Distanz in Millimetern [mm], korrigiert um den Offset.
 * @see _convertIrRawToMm
 * @see IR_SENSOR_OFFSET_LEFT_TO_WHEEL_MM
 */
int GetDistanceLeft_mm(void) {
	int raw = ReadLeft(); // Lies den Roh-ADC-Wert über die HAL-Schicht
	return _convertIrRawToMm(raw) - IR_SENSOR_OFFSET_LEFT_TO_WHEEL_MM;
}

/**
 * @brief Liest die Distanz des rechten IR-Sensors in Millimetern.
 *
 * Ruft die Hardware-Abstraktionsschicht des rechten IR-Sensors auf ('ReadRight()')
 * und konvertiert den rohen ADC-Wert mittels der internen LUT ('_convertIrRawToMm()')
 * in Millimeter. Abschließend wird ein Offset ('IR_SENSOR_OFFSET_RIGHT_TO_WHEEL_MM')
 * subtrahiert, um die Distanz relativ zur Radachse oder einer anderen Referenz zu erhalten.
 *
 * @return Die berechnete Distanz in Millimetern [mm], korrigiert um den Offset.
 * @see _convertIrRawToMm
 * @see IR_SENSOR_OFFSET_RIGHT_TO_WHEEL_MM
 */
int GetDistanceRight_mm(void) {
	int raw = ReadRight(); // Lies den Roh-ADC-Wert über die HAL-Schicht
	return _convertIrRawToMm(raw) - IR_SENSOR_OFFSET_RIGHT_TO_WHEEL_MM;
}

/**
 * @brief Liest die inkrementelle Distanz des linken Encoders in Millimetern.
 *
 * Ruft die Hardware-Abstraktionsschicht des linken Encoders auf ('EncoderReadLeft()'),
 * um die seit dem letzten Reset akkumulierten Ticks zu erhalten. Diese Ticks werden dann
 * in Millimeter umgerechnet, basierend auf dem Kalibrierungsfaktor 'TICK_PER_MM'.
 *
 * @return Die akkumulierte Distanz des linken Rades in Millimetern [mm].
 * @see TICK_PER_MM
 */
float GetEncoderLeft_mm(void) {
    int raw_ticks = EncoderReadLeft();
    // Umrechnung von Ticks in Millimeter mit MM_PER_TICK (aus sensors.h)
    float distance_float = (float)raw_ticks * TICK_PER_MM;
    return (distance_float); // Auf ganzen Millimeter runden und zurückgeben
}

/**
 * @brief Liest die inkrementelle Distanz des rechten Encoders in Millimetern.
 *
 * Ruft die Hardware-Abstraktionsschicht des rechten Encoders auf ('EncoderReadRight()'),
 * um die seit dem letzten Reset akkumulierten Ticks zu erhalten. Diese Ticks werden dann
 * in Millimeter umgerechnet, basierend auf dem Kalibrierungsfaktor 'TICK_PER_MM'.
 *
 * @return Die akkumulierte Distanz des rechten Rades in Millimetern [mm].
 * @see TICK_PER_MM
 */
float GetEncoderRight_mm(void) {
    int raw_ticks = EncoderReadRight();
    // Umrechnung von Ticks in Millimeter mit MM_PER_TICK (aus sensors.h)
    float distance_float = (float)raw_ticks * TICK_PER_MM;
    return (distance_float); // Auf ganzen Millimeter runden und zurückgeben
}

/**
 * @brief Führt eine umfassende Diagnoseprüfung aller kritischen Sensoren und Aktuatoren durch.
 *
 * Diese Funktion überprüft die Funktionsfähigkeit des Front-Ultraschallsensors,
 * der linken und rechten IR-Sensoren sowie der Motor-Encoder-Systeme.
 * Sie gibt detaillierte Meldungen über die Debug-Kommunikationsschnittstelle aus
 * und signalisiert das Gesamtergebnis über die User-Interface-Schnittstelle.
 * Bei erkannten Fehlern wird die Funktion in einer Endlosschleife verbleiben
 * und kontinuierlich Fehlermeldungen ausgeben, bis das Problem behoben ist.
 *
 * @return 'true' wenn alle Diagnosetests erfolgreich waren; 'false' wenn mindestens ein Test fehlschlägt.
 * @note Im Fehlerfall geht die Funktion in eine Endlosschleife und beendet sich nicht.
 */
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
                    print_iteration_counter = 0; // Zähler zurücksetzen, um den nächsten Zählzyklus zu starten
                } else {
                    for (volatile int int_delay_loop = 0; int_delay_loop < 50000; int_delay_loop++) {}
                }

                print_iteration_counter++; // Zähler für jeden Schleifendurchlauf erhöhen
    		}
        }
    return all_ok;
}




