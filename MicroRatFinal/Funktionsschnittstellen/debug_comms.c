/*
 * debug_comms.c
 *
 *  Created on: 18 Jun 2025
 *      Author: marcu
 */
#include "DAVE.h"
#include "Funktionsschnittstellen/debug_comms.h"
#include "Hardwaresteuerung/hal_uart.h"
#include <string.h>





void Debug_Comms_SendString(const char* str) {
    if (str == NULL) {
        return;
    }
    HAL_UART_SendBuffer((const uint8_t*)str, strlen(str));
}

void Debug_Comms_SendBuffer(const uint8_t* data, size_t length) {
    if (data == NULL || length == 0) {
        return;
    }
    HAL_UART_SendBuffer(data, length);
}
