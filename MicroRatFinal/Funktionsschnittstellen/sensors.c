
#include "Dave.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Funktionsschnittstellen/movement.h"
#include "Hardwaresteuerung/hal_ir.h"
#include "Hardwaresteuerung/hal_us.h"
#include "Hardwaresteuerung/hal_encoder.h"
#include <stdbool.h>
#include <stdio.h>

#define IR_THRESHOLD_LEFT 400  // Schwellenwert ca. 5cm
#define IR_THRESHOLD_RIGHT 400  // Schwellenwert ca. 5cm
#define US_THRESHOLD 5 // Beispiel-Schwellenwert für den Ultraschallsensor (in cm)

#define WHEEL_RADIUS 1.6 // cm
#define ENCODER_TICKS 180  // 30:1 Getriebe × 12 Zählungen = 360 Ticks/Umdrehung
#define WHEEL_CIRCUMFERENCE (2 * 3.14159 * WHEEL_RADIUS) // ≈ 10.05 cm
#define DISTANCE_PER_TICK (WHEEL_CIRCUMFERENCE / ENCODER_TICKS) // ≈ 0.0279 cm

    static volatile float lastFrontDistance = 100.0; // Statische Variable behält ihren Wert

uint8_t UART_String[100];

void SensorsInit(){
    EncoderStartLeft();
    EncoderStartRight();
    UltrasoundStart();
}

void DebugPrint(){
	float distanz_ultra = GetUltraschall();
	//float distance_travelled = GetDistanceTravelled();
	int IR_L = GetIRLeft();
	int IR_R = GetIRRight();
	int count_L = GetEncoderLeft();
	int count_R = GetEncoderRight();
	sprintf((char*)UART_String,	" Ultraschall: %.2fcm IR_R: %d IR_L: %d  L: %d  R: %d  d: %.1f \n\r", distanz_ultra, IR_R, IR_L, count_L, count_R);//distance_travelled);
	UART_Transmit(&UART_COM, UART_String, sizeof(UART_String));	//Ausnahme weil wohin zu HAL?
}

bool IsWallLeft(void) {
    // Rückgabewert ist true, wenn der IR-Wert den Schwellenwert überschreitet
    return ReadLeft() > IR_THRESHOLD_LEFT;
}

bool IsWallRight(void) {
    // Rückgabewert ist true, wenn der IR-Wert den Schwellenwert überschreitet
    return ReadRight() > IR_THRESHOLD_RIGHT;
}

bool IsWallFront(void) {
    float distance = FrontRead();
    return (distance <= US_THRESHOLD);
}

float GetUltraschall(void) {
    return FrontRead();
}

int GetIRLeft(void) {
    return ReadLeft();
}

int GetIRRight(void) {
    return ReadRight();
}

int GetEncoderLeft(void) {
    return EncoderReadLeft();
}

int GetEncoderRight(void) {
    return EncoderReadRight();
}


