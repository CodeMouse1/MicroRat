/*
 * hal_motor.h
 *
 *  Created on: 17 Apr 2025
 *      Author: marcu
 */

#ifndef HARDWARESTEUERUNG_HAL_MOTOR_H_
#define HARDWARESTEUERUNG_HAL_MOTOR_H_

void MotorsSetForward(void);
void MotorsSetRight(void);
void MotorsSetLeft(void);
void MotorsSetReverse(void);
void MotorsSetSpeed(int left, int right);
void MotorsStop(void);
void MotorsDrive(void);

#endif /* HARDWARESTEUERUNG_HAL_MOTOR_H_ */
