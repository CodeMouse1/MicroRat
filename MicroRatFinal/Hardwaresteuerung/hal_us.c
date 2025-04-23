/*
 * hal_us.c
 *
 *  Created on: 17 Apr 2025
 *      Author: marcu
 */
#include "hal_us.h"
#include "DAVE.h"

float FrontRead(void) {
    uint32_t capture_time;
    CAPTURE_GetCapturedTime(&CAPTURE_ULTRA, &capture_time);
    // Berechne den Abstand basierend auf der Zeit, die das Ultraschallsignal benötigt hat
    float time_in_us = ((float)capture_time * 333.33)/1000; // Umrechnung in Mikrosekunden
    return time_in_us / 58.0f; // Berechnung der Distanz in cm (Schallgeschwindigkeit)
}
