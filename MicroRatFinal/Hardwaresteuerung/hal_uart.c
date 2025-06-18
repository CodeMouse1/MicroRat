/*
 * hal_uart.c
 *
 *  Created on: 18 Jun 2025
 *      Author: marcu
 */
#include "DAVE.h"
#include "Hardwaresteuerung/hal_uart.h"


void HAL_UART_SendBuffer(const uint8_t* data, uint32_t length) {
    if (data == NULL || length == 0) {
        return; // Nichts senden, wenn Puffer leer oder Länge Null
    }
    UART_Transmit(&UART_COM, (uint8_t*)data, length);
}
