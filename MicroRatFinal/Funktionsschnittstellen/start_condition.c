/*
 * start_condition.c
 *
 *  Created on: 23 Apr 2025
 *      Author: marcu
 */
#include "DAVE.h"
#include "PID.h"

#include "Hardwaresteuerung/hal_startbutton.h"
#include "Hardwaresteuerung/hal_motor.h"

/*
int start = 0;
int button_status = 0;
bool button_pressed = false;
*/
/*void WaitForStart(){
	while(1){
		button_status = DIGITAL_IO_GetInput(&DIGITAL_IO_BUTTON);
		if(button_status == 1 && !button_pressed){
			TIMER_Start(&TIMER_DELAY);
		}
		if(start == 1){
			break;
		}
	}
}*/
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
	HAL_StartReglerTimer();
	StartDrive();
	/*button_pressed = true;
	TIMER_ClearEvent(&TIMER_DELAY);
	TIMER_Stop(&TIMER_DELAY);
	start = 1;
		//DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_1);	Erprobung neuer Timer Interval
		//TIMER_SetTimeInterval(&TIMER_0, 100000000U);
		//TIMER_Start(&TIMER_0);
	TIMER_Start(&TIMER_REGELUNG);*/
}
