/*
 * movement.h
 *
 *  Created on: 17 Apr 2025
 *      Author: marcu
 */

#ifndef FUNKTIONSSCHNITTSTELLEN_MOVEMENT_H_
#define FUNKTIONSSCHNITTSTELLEN_MOVEMENT_H_

typedef enum {
	none,
	left,
	right,
	around
} TurnDirection;

void MovementInit(void);
void UpdatePID(void);
void ResetPID(void);
void setPIDGoalD(int distance);
void setPIDGoalA(int angle);
int PIDdone(void);
int PIDdoneTurn(void);

void move(int n);
void turn(TurnDirection direction);
void RecalibrateAndMoveForward(void);

#endif /* FUNKTIONSSCHNITTSTELLEN_MOVEMENT_H_ */
