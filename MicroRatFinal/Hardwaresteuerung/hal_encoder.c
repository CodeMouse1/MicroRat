/*
 * hal_encoder.c
 *
 *  Created on: 23 Apr 2025
 *      Author: marcu
 */
#include "DAVE.h"
#include "Funktionsschnittstellen/sensors.h"

#define PERIODIC_READ 50000U

uint32_t Timer_50ms;
uint32_t Capture_t;

void Encoder_Start_Left(void) {
    // Encoder starten für das linke Rad
    COUNTER_Start(&COUNTER_L);
}

void Encoder_Start_Right(void) {
    // Encoder starten für das rechte Rad
    COUNTER_Start(&COUNTER_R);
}

int Encoder_Read_Left(void) {
    return COUNTER_GetCurrentCount(&COUNTER_L);  // Gibt den aktuellen Wert des linken Encoders zurück
}

int Encoder_Read_Right(void) {
    return COUNTER_GetCurrentCount(&COUNTER_R);  // Gibt den aktuellen Wert des linken Encoders zurück
}

void EncoderReset(){
	COUNTER_ResetCounter(&COUNTER_L);
	COUNTER_ResetCounter(&COUNTER_R);
}

void StartSensorTimer(){
	Timer_50ms = SYSTIMER_CreateTimer(PERIODIC_READ, SYSTIMER_MODE_PERIODIC, (void*)SensorsRead, NULL);
	SYSTIMER_StartTimer(Timer_50ms);
}
