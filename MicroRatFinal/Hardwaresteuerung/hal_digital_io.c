/*
 * hal_digital_io.c
 *
 *  Created on: 18 Jun 2025
 *      Author: marcu
 */
#include "Dave.h"
#include "Hardwaresteuerung/hal_digital_io.h"


void SetLEDs(LEDState left, LEDState right){
	if(left == on && right == on){
		DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_1);
		DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_2);
	}else if (left == off && right == on){
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_AUGE_1);
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_AUGE_2);
	}else if (left == on && right == off){
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_AUGE_1);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_AUGE_2);
	}else if(left == on && right == on){
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_AUGE_1);
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_AUGE_2);
	}else if(left == off && right == off){
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_AUGE_1);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_AUGE_2);
	}else if(left == toggle && right == toggle){
		DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_1);
		DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_2);
	}
}

int GetButtonInput(){
	return DIGITAL_IO_GetInput(&DIGITAL_IO_BUTTON);
}
