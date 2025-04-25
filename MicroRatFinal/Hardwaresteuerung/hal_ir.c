/*
 * hal_ir.c
 *
 *  Created on: 17 Apr 2025
 *      Author: marcu
 */
#include "DAVE.h"
#include "Hardwaresteuerung/hal_ir.h"

int ReadLeft(void) {
    XMC_VADC_RESULT_SIZE_t result = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_Channel_B);
    return result;  // Hier wird der ADC-Wert des linken IR-Sensors zurückgegeben
}

int ReadRight(void) {
    XMC_VADC_RESULT_SIZE_t result = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_Channel_A);
    return result;  // Hier wird der ADC-Wert des rechten IR-Sensors zurückgegeben
}
