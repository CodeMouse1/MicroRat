#include <Funktionsschnittstellen/pd_regler.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "Dave.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Funktionsschnittstellen/movement.h"
#include "Hardwaresteuerung/hal_ir.h"
#include "Hardwaresteuerung/hal_us.h"
#include "Hardwaresteuerung/hal_encoder.h"

uint8_t UART_String[100];

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

void DebugPrint(){
	float distanz_ultra = GetDistanceFront_mm();
	int IR_L = GetDistanceLeft_mm();
	int IR_R = GetDistanceRight_mm();
	float count_L = GetEncoderLeft_mm();
	float count_R = GetEncoderRight_mm();
	sprintf((char*)UART_String,	" Ultraschall: %.2fmm IR_R: %dmm IR_L: %dmm  L: %.1fmm  R: %.1fmm \n\r", distanz_ultra, IR_R, IR_L, count_L, count_R);
	UART_Transmit(&UART_COM, UART_String, sizeof(UART_String));	//Ausnahme weil wohin zu HAL?
}

bool IsWallLeft(void) {
    // Rückgabewert ist true, wenn der IR-Wert den Schwellenwert unterschreitet
    return GetDistanceLeft_mm() < WALL_DETECTION_THRESHOLD+30;
}

bool IsWallRight(void) {
    // Rückgabewert ist true, wenn der IR-Wert den Schwellenwert unterschreitet
    return GetDistanceRight_mm() < WALL_DETECTION_THRESHOLD+30;
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


