/*
 * Motor.h
 *
 *  Created on: 31 Mar 2025
 *      Author: marcu
 */

#ifndef MOTOR_H_
#define MOTOR_H_

void Motor_Init(void);

void Motor_Set_Speed(int duty_cycle_R, int duty_cycle_L);

// Stoppt den Motor
void Motor_Stop(void);

void Motor_Turn(int rotations);



#endif /* MOTOR_H_ */
