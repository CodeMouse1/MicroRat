/*
 * hal_encoder.c
 *
 *  Created on: 23 Apr 2025
 *      Author: marcu
 */
#include "DAVE.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Hardwaresteuerung/hal_encoder.h"

void EncoderStartLeft(void) {
    COUNTER_Start(&COUNTER_L);
}

void EncoderStartRight(void) {
    COUNTER_Start(&COUNTER_R);
}

int EncoderReadLeft(void) {
    return COUNTER_GetCurrentCount(&COUNTER_L);  // Gibt den aktuellen Wert des linken Encoders zurück
}

int EncoderReadRight(void) {
    return COUNTER_GetCurrentCount(&COUNTER_R);  // Gibt den aktuellen Wert des linken Encoders zurück
}

void EncoderReset(){
	COUNTER_ResetCounter(&COUNTER_L);
	COUNTER_ResetCounter(&COUNTER_R);
}


