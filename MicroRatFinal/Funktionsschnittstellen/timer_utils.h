/*
 * timer_utils.h
 *
 *  Created on: 19 Jun 2025
 *      Author: marcu
 */

#ifndef FUNKTIONSSCHNITTSTELLEN_TIMER_UTILS_H_
#define FUNKTIONSSCHNITTSTELLEN_TIMER_UTILS_H_

#include <stdint.h>

extern volatile bool s_delay_isr_flag;

void Delay_ms(uint32_t milliseconds);

#endif /* FUNKTIONSSCHNITTSTELLEN_TIMER_UTILS_H_ */
