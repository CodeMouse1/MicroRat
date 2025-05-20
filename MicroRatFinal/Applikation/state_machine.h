/*
 * state_machine.h
 *
 *  Created on: 20 May 2025
 *      Author: marcu
 */

#ifndef APPLIKATION_STATE_MACHINE_H_
#define APPLIKATION_STATE_MACHINE_H_

typedef enum {
	STATE_IDLE,
	STATE_EXPLORE,
	STATE_WAIT_REPORT
} RatState;

extern int currentX;
extern int currentY;
//extern Orientation currentOrientation;
extern int targetX;
extern int targetY;
extern Orientation currentOrientation;

extern volatile RatState currentState; // Deklaration, Definition in .c

//void RatStateMachine_Init(void);
void RatStateMachine_Update(void);

#endif /* APPLIKATION_STATE_MACHINE_H_ */
