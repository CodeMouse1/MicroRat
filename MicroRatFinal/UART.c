/*
 * UART.c
 *
 *  Created on: 31 Mar 2025
 *      Author: marcu
 */
#include "Dave.h"
#include <stdio.h>
#include "Funktionsschnittstellen/sensors.h"

uint8_t UART_String[100];

void UART_Send(){
	float distanz_ultra = GetUltraschall();
	int IR_L = GetIRLeft();
	int IR_R = GetIRRight();
	int count_L = GetEncoderLeft();
	int count_R = GetEncoderRight();
	sprintf((char*)UART_String,	" Ultraschall: %.2fcm IR_R: %d IR_L: %d  L: %d  R: %d\n\r", distanz_ultra, IR_R, IR_L, count_L, count_R);
	UART_Transmit(&UART_COM, UART_String, sizeof(UART_String));
}

