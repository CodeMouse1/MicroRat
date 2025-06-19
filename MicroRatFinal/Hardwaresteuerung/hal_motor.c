/*
 * hal_motor.c
 *
 *  Created on: 17 Apr 2025
 *      Author: marcu
 */
#include <stdlib.h>
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

volatile MotorDirection currentMotorDirectionL = MOTOR_STOPPED;
volatile MotorDirection currentMotorDirectionR = MOTOR_STOPPED;

void MotorsSetSpeed(int speedL, int speedR) {
    // --- Linker Motor ---
    if (speedL > 0) {
        // Motor soll vorwärts drehen (IN1 High, IN2 Low)
        DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_INPUT_1);
        DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_2);
        // PWM-Duty-Cycle für linken Motor setzen
        PWM_SetDutyCycle(&PWM_L, speedL);
        currentMotorDirectionL = MOTOR_FORWARD;
    } else if (speedL < 0) {
        // Motor soll rückwärts drehen (IN1 Low, IN2 High)
        DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_1);
        DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_INPUT_2);
        // PWM-Duty-Cycle für linken Motor setzen (mit Betrag der Geschwindigkeit)
        PWM_SetDutyCycle(&PWM_L, abs(speedL));
        currentMotorDirectionL = MOTOR_BACKWARD;
    } else {
        // Motor stoppen (Freilauf: IN1 Low, IN2 Low, PWM auf 0)
        DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_1);
        DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_2);
        PWM_SetDutyCycle(&PWM_L, 0);
        currentMotorDirectionL = MOTOR_STOPPED;
    }
    // --- Rechter Motor ---
    if (speedR > 0) {
        // Motor soll vorwärts drehen (IN3 Low, IN4 High)
        DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_3);
        DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_INPUT_4);
        // PWM-Duty-Cycle für rechten Motor setzen
        PWM_SetDutyCycle(&PWM_R, speedR);
        currentMotorDirectionR = MOTOR_FORWARD;
    } else if (speedR < 0) {
        // Motor soll rückwärts drehen (IN3 High, IN4 Low)
        DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_INPUT_3);
        DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_4);
        // PWM-Duty-Cycle für rechten Motor setzen (mit Betrag der Geschwindigkeit)
        PWM_SetDutyCycle(&PWM_R, abs(speedR));
        currentMotorDirectionR = MOTOR_BACKWARD;
    } else {
        // Motor stoppen (Freilauf: IN3 Low, IN4 Low, PWM auf 0)
        DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_3);
        DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_4);
        PWM_SetDutyCycle(&PWM_R, 0);
        currentMotorDirectionR = MOTOR_STOPPED;
    }
}

void StopAndSignal(){
	MotorsSetSpeed(0,0);
	TIMER_Stop(&TIMER_REGLER);
}
