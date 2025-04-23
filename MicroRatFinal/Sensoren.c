/*
 * Sensoren.c
 *
 *  Created on: 31 Mar 2025
 *      Author: marcu
 */
#include "Dave.h"
#include <stdio.h>
/*

#define PERIODIC_READ 50000U

uint32_t Timer_50ms;
uint32_t Capture_t;

XMC_VADC_RESULT_SIZE_t ADC_Wert_IR_L;
XMC_VADC_RESULT_SIZE_t ADC_Wert_IR_R;

float captured_time_us;
float distanz_ultra;
int IR_L, IR_R = 0;
int count_L, count_R = 0;

void Sensoren_Auslesen_50ms(){
	//IR
	ADC_Wert_IR_R = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_Channel_A);
	ADC_Wert_IR_L = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_Channel_B);
	IR_R = ADC_Wert_IR_R;
	IR_L = ADC_Wert_IR_L;
	//ULTRASCHALL
	CAPTURE_GetCapturedTime(&CAPTURE_ULTRA, &Capture_t);
	captured_time_us = ((float)Capture_t * 333.33)/1000;
	distanz_ultra = captured_time_us /58;
	//Encoder auslesen
	count_R = COUNTER_GetCurrentCount(&COUNTER_R);
	count_L = COUNTER_GetCurrentCount(&COUNTER_L);
}

void Encoder_Get_Speed(){
	// to do?
}

void Sensoren_Init(){
	//Encoderzähler starten
	COUNTER_Start(&COUNTER_R);
	COUNTER_Start(&COUNTER_L);
	//Timer Init und Start zum Auslesen von Sensordaten alle 50ms
	Timer_50ms = SYSTIMER_CreateTimer(PERIODIC_READ,SYSTIMER_MODE_PERIODIC,(void*)Sensoren_Auslesen_50ms,NULL);
	SYSTIMER_StartTimer(Timer_50ms);
}
*/

