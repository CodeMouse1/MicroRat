/*
 * state_machine.h
 *
 *  Created on: 20 May 2025
 *      Author: marcu
 */

#ifndef APPLIKATION_STATE_MACHINE_H_
#define APPLIKATION_STATE_MACHINE_H_

#define NO_WALL_TIMEOUT_THRESHOLD 2

typedef enum {
	STATE_IDLE,
	STATE_EXPLORE,
	STATE_WAIT_REPORT,
	STATE_SHORTEST_PATH
} RatState;

typedef enum {
    TURN_STRAIGHT = 0,
    TURN_LEFT,
    TURN_RIGHT
} TurnMouse;

extern volatile RatState currentState;

void RatStateMachine_Update(void);


#endif /* APPLIKATION_STATE_MACHINE_H_ */
