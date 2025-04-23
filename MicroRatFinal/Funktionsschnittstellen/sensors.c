/*
 * sensors.c
 *
 *  Created on: 23 Apr 2025
 *      Author: marcu
 */
#include "Dave.h"
#include <stdbool.h>
#include "Hardwaresteuerung/hal_ir.h"
#include "Hardwaresteuerung/hal_us.h"
#include "Hardwaresteuerung/hal_encoder.h"

#define IR_THRESHOLD 500  // Beispiel-Schwellenwert für den IR-Sensor
#define US_THRESHOLD 30.0 // Beispiel-Schwellenwert für den Ultraschallsensor (in cm)

static int ir_left = 0;
static int ir_right = 0;
static float distanz_ultra = 0.0;
static int count_L = 0, count_R = 0;


void SensorsInit(){
    Encoder_Start_Left();
    Encoder_Start_Right();
    StartSensorTimer();
}

void SensorsRead(){
    // IR-Sensoren
    ir_left = Read_Left();
    ir_right = Read_Right();

    // Ultraschallsensor
    distanz_ultra = FrontRead();

    // Encoderwerte
    count_L = Encoder_Read_Left();
    count_R = Encoder_Read_Right();
}

// Funktion für den linken IR-Sensor
bool IsWallLeft(void) {
    // Rückgabewert ist true, wenn der IR-Wert den Schwellenwert überschreitet
    return ir_left > IR_THRESHOLD;
}

// Funktion für den rechten IR-Sensor
bool IsWallRight(void) {
    // Rückgabewert ist true, wenn der IR-Wert den Schwellenwert überschreitet
    return ir_right > IR_THRESHOLD;
}

// Funktion für den Ultraschallsensor (Front)
bool IsWallFront(void) {
    // Rückgabewert ist true, wenn der Abstand kleiner als der Schwellenwert ist
    return distanz_ultra < US_THRESHOLD;
}

float GetUltraschall(void) {
    return distanz_ultra;
}

int GetIRLeft(void) {
    return ir_left;
}

int GetIRRight(void) {
    return ir_right;
}

int GetEncoderLeft(void) {
    return count_L;
}

int GetEncoderRight(void) {
    return count_R;
}

