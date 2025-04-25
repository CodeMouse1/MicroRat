/*
 * hal_startbutton.h
 *
 *  Created on: 23 Apr 2025
 *      Author: marcu
 */

#ifndef HARDWARESTEUERUNG_HAL_STARTBUTTON_H_
#define HARDWARESTEUERUNG_HAL_STARTBUTTON_H_

bool HAL_IsStartButtonPressed(void);

void HAL_StartStartDelay(void);
void HAL_StopStartDelay(void);
void HAL_ClearStartDelayEvent(void);
void HAL_StartReglerTimer(void);
void HAL_DelayTimer(int delay_us);
void DelayRoutine(void);

#endif /* HARDWARESTEUERUNG_HAL_STARTBUTTON_H_ */
