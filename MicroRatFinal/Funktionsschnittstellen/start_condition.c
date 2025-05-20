/*
 * start_condition.c
 *
 *  Created on: 23 Apr 2025
 *      Author: marcu
 */
#include "DAVE.h"
#include "Funktionsschnittstellen/start_condition.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Hardwaresteuerung/hal_startbutton.h"
#include "Hardwaresteuerung/hal_motor.h"

static bool button_pressed = false;
static bool start = false;


void WaitForStart() {
    while (true) {
        if (HAL_IsStartButtonPressed() && !button_pressed) {
            button_pressed = true;
            HAL_StartStartDelay(5000000);
        }
        if (start) {
            break;
        }
    }
}

void StartButton(){
	if(start == false){
		start = true;
		HAL_ClearStartDelayEvent();
		HAL_StopStartDelay();
		MotorsSetForward();

		TIMER_Start(&TIMER_REGLER);
	}

}

void Delay(int delay_ms){
	int delay_us = delay_ms*1000;
	HAL_DelayTimer(delay_us);
}
