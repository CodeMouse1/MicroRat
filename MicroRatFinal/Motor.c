/*
 * Motor.c
 *
 *  Created on: 31 Mar 2025
 *      Author: marcu
 */
#include "Dave.h"
#include <stdio.h>
#include <stdlib.h>

//Setzt Motor auf Vorwärtsfahren ein
void Motor_Init(){
	DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_INPUT_1);
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_2);
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_3);
	DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_INPUT_4);
}

//Stoppt den Motor
void Motor_Stop(){
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_1);
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_2);
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_3);
	DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_4);
}

//Geschwindigkeit der Räder
void Motor_Set_Speed(int duty_cycle_R, int duty_cycle_L){
	PWM_SetDutyCycle(&PWM_R, duty_cycle_R);
	PWM_SetDutyCycle(&PWM_L, duty_cycle_L);
	PWM_Start(&PWM_R);
	PWM_Start(&PWM_L);
}

void Motor_Turn(int rotations) {
	extern int count_L, count_R;
    // Berechne das Ziel basierend auf den Ticks für 90 Grad
    int target_count = rotations * 150;  // Berechnet die Anzahl der Encoder-Ticks für 90 Grad * rotations
    int current_count = 0;

    if(rotations == 1 || rotations == 2){
    	DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_INPUT_1);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_2);
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_INPUT_3);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_4);
    }
    // Starte die Motoren in die gewünschte Drehrichtung
    Motor_Set_Speed(3000, 3000);  // Linkes Rad vorwärts, rechtes Rad rückwärts für Drehung

    if(rotations == 1){
		// Warten, bis die Drehung ungefähr abgeschlossen ist
		while (abs(current_count) < abs(target_count)) {
			current_count = count_L + count_R;  // Berechne die Differenz der Encoderwerte
		}
    }else if(rotations == 2){
    	while (abs(current_count) < abs(target_count*1.2)) {
    				current_count = count_L + count_R;  // Berechne die Differenz der Encoderwerte
    			}
    }
    Motor_Stop();
}
