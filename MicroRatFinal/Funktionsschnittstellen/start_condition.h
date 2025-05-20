/*
 * start_condition.h
 *
 *  Created on: 23 Apr 2025
 *      Author: marcu
 */

#ifndef FUNKTIONSSCHNITTSTELLEN_START_CONDITION_H_
#define FUNKTIONSSCHNITTSTELLEN_START_CONDITION_H_

//void WaitForStart(void);
//void StartButton(void);

void Delay(int delay_ms);

bool IsStartButtonPressed(void);
void StartButton(void);
void WaitForStart(void);

#endif /* FUNKTIONSSCHNITTSTELLEN_START_CONDITION_H_ */
