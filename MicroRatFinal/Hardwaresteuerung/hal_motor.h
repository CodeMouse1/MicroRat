/*
 * hal_motor.h
 *
 *  Created on: 17 Apr 2025
 *      Author: marcu
 */

#ifndef HARDWARESTEUERUNG_HAL_MOTOR_H_
#define HARDWARESTEUERUNG_HAL_MOTOR_H_

typedef enum {
    MOTOR_STOPPED,
    MOTOR_FORWARD,
    MOTOR_BACKWARD
} MotorDirection;

extern volatile MotorDirection currentMotorDirectionL;
extern volatile MotorDirection currentMotorDirectionR;

void MotorsSetForward(void);
void MotorsSetRight(void);
void MotorsSetLeft(void);
void MotorsSetReverse(void);
void MotorsSetSpeed(int left, int right);
void MotorsStop(void);
void MotorsDrive(void);
void StopAndSignal(void);

#endif /* HARDWARESTEUERUNG_HAL_MOTOR_H_ */
