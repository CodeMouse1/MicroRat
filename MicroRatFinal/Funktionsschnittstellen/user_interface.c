/*
 * user_feedback.c
 *
 *  Created on: 18 Jun 2025
 *      Author: marcu
 */
#include <stdbool.h>
#include "Funktionsschnittstellen/user_interface.h"
#include "Funktionsschnittstellen/timer_utils.h"
#include "Hardwaresteuerung/hal_motor.h"
#include "Hardwaresteuerung/hal_digital_io.h"


void SignalTargetReached(){
	MotorsSetSpeed(1000, 1000);
	for (volatile int i = 0; i < 500000; i++) {}
	MotorsSetSpeed(0, 0);
	for (volatile int i = 0; i < 500000; i++) {}
	MotorsSetSpeed(1000, 1000);
	for (volatile int i = 0; i < 500000; i++) {}
	MotorsSetSpeed(0, 0);
	SetLEDs(toggle, toggle);
}
void SignalOptimisationComplete(){
	MotorsSetSpeed(1000, 1000);
	for (volatile int i = 0; i < 200000; i++) {}
	MotorsSetSpeed(0, 0);
	for (volatile int i = 0; i < 200000; i++) {}
	MotorsSetSpeed(1000, 1000);
	for (volatile int i = 0; i < 200000; i++) {}
	MotorsSetSpeed(0, 0);
	for (volatile int i = 0; i < 200000; i++) {}
	MotorsSetSpeed(1000, 1000);
	for (volatile int i = 0; i < 200000; i++) {}
	MotorsSetSpeed(0, 0);
	Delay_ms(2000);
}
void SignalDiagnosticsResult(bool result){
	for (volatile int i = 0; i < 1000000; i++) {}
	if(result == true){
		MotorsSetSpeed(1000, 1000);
		for (volatile int i = 0; i < 1000000; i++) {}
		MotorsSetSpeed(0, 0);

	}else if(result == false){
		SetLEDs(on,off);
		for (volatile int i = 0; i < 7500000; i++) {}
		SetLEDs(off,on);
		for (volatile int i = 0; i < 7500000; i++) {}
	}
}

void SignalOutOfBounds(){
	while(1){
		SetLEDs(on,off);
		for (volatile int i = 0; i < 2000000; i++) {}
		SetLEDs(off,on);
		for (volatile int i = 0; i < 2000000; i++) {}
	}
}

bool IsStartButtonPressed(){
	return GetButtonInput();
}




