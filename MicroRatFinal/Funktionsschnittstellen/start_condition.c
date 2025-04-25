/*
 * start_condition.c
 *
 *  Created on: 23 Apr 2025
 *      Author: marcu
 */
#include "DAVE.h"
#include "Funktionsschnittstellen/start_condition.h"
#include "Hardwaresteuerung/hal_startbutton.h"
#include "Hardwaresteuerung/hal_motor.h"

static bool button_pressed = false;
static bool start = false;

void WaitForStart() {
    while (true) {
        if (HAL_IsStartButtonPressed() && !button_pressed) {
            button_pressed = true;
            HAL_StartStartDelay();
        }
        if (start) {
            break;
        }
    }
}

void StartButton(){
	start = true;
	HAL_ClearStartDelayEvent();
	HAL_StopStartDelay();
	MotorsSetForward();
	MotorsDrive();
}

void Delay(int delay_ms){
	int delay_us = delay_ms*1000;
	HAL_DelayTimer(delay_us);
}
