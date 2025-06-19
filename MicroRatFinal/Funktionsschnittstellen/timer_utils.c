/*
 * timer_utils.c
 *
 *  Created on: 19 Jun 2025
 *      Author: marcu
 */
#include "Dave.h"
#include "Funktionsschnittstellen/timer_utils.h"

volatile bool s_delay_isr_flag = false;

void TIMER_DELAY_ISR(void) {
    TIMER_ClearEvent(&TIMER_DELAY);
    s_delay_isr_flag = true;
}

void Delay_ms(uint32_t milliseconds) {
    TIMER_Stop(&TIMER_DELAY);

    TIMER_Clear(&TIMER_DELAY);

    uint32_t interval_units = milliseconds * 1000 * 100;
    TIMER_SetTimeInterval(&TIMER_DELAY, interval_units);

    s_delay_isr_flag = false;
    TIMER_ClearEvent(&TIMER_DELAY);

    TIMER_Start(&TIMER_DELAY);

    while (!s_delay_isr_flag) {
    }

    TIMER_Stop(&TIMER_DELAY);
    TIMER_Clear(&TIMER_DELAY);
}

