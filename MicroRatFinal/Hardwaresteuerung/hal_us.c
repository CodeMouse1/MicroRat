/*
 * hal_us.c
 *
 *  Created on: 17 Apr 2025
 *      Author: marcu
 */
#include "DAVE.h"
#include "hal_us.h"

float FrontRead() {
    uint32_t capture_time;
    CAPTURE_GetCapturedTime(&CAPTURE_ULTRA, &capture_time);
    float time_in_us = ((float)capture_time * 333.33)/1000;
    return time_in_us / 58.0f;
}

void UltrasoundStart(){
	PWM_Start(&PWM_TRIGGER);
}
