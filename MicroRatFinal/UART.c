/*
 * UART.c
 *
 *  Created on: 31 Mar 2025
 *      Author: marcu
 */
#include "Dave.h"
#include <stdio.h>

uint8_t UART_String[100];

extern float distanz_ultra;
extern int IR_L, IR_R;
extern int count_L, count_R;

void UART_Send(){
	sprintf((char*)UART_String,	" Ultraschall: %.2fcm IR_R: %d IR_L: %d  L: %d  R: %d\n\r", distanz_ultra, IR_R, IR_L, count_L, count_R);
	UART_Transmit(&UART_COM, UART_String, sizeof(UART_String));
}

