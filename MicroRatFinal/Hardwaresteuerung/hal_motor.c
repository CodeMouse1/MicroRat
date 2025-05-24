/*
 * hal_motor.c
 *
 *  Created on: 17 Apr 2025
 *      Author: marcu
 */
#include "Dave.h"
#include "Hardwaresteuerung/hal_motor.h"

void MotorsSetForward(){
	DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_INPUT_1);
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_2);
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_3);
	DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_INPUT_4);
}
void MotorsSetLeft(){
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_1);
	DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_INPUT_2);
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_3);
	DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_INPUT_4);
}
void MotorsSetRight(){
	DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_INPUT_1);
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_2);
	DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_INPUT_3);
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_4);
}
void MotorsSetReverse(){
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_1);
	DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_INPUT_2);
	DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_INPUT_3);
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_4);
}

void MotorsStop(){
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_1);
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_2);
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_3);
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_4);
}

void MotorsDrive(){
	PWM_SetDutyCycle(&PWM_R, 3000);
	PWM_SetDutyCycle(&PWM_L, 3000);
	PWM_Start(&PWM_R);
	PWM_Start(&PWM_L);
}

void MotorsSetSpeed(int left, int right){
	PWM_SetDutyCycle(&PWM_L, left);
	PWM_SetDutyCycle(&PWM_R, right);
}

void StopAndSignal(){
	MotorsSetSpeed(0,0);
	TIMER_Stop(&TIMER_REGLER);
	DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_1);
	DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_2);
}
