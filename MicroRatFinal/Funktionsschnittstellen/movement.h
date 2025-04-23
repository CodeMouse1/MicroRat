/*
 * movement.h
 *
 *  Created on: 17 Apr 2025
 *      Author: marcu
 */

#ifndef FUNKTIONSSCHNITTSTELLEN_MOVEMENT_H_
#define FUNKTIONSSCHNITTSTELLEN_MOVEMENT_H_

typedef enum {
	left,
	right,
	around
} TurnDirection;

void MovementInit();
void Turn(TurnDirection dir);
void Stop();

#endif /* FUNKTIONSSCHNITTSTELLEN_MOVEMENT_H_ */
