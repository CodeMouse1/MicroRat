/*
 * state_machine.h
 *
 *  Created on: 20 May 2025
 *      Author: marcu
 */

#ifndef APPLIKATION_STATE_MACHINE_H_
#define APPLIKATION_STATE_MACHINE_H_

/*typedef enum {
	MOUSE_NORTH = 0, // Von NORTH zu MOUSE_NORTH
	MOUSE_EAST,      // Von EAST zu MOUSE_EAST
	MOUSE_SOUTH,     // Von SOUTH zu MOUSE_SOUTH
	MOUSE_WEST       // Von WEST zu MOUSE_WEST
} MouseOrientation;*/

typedef enum {
    TURN_STRAIGHT = 0,
    TURN_LEFT,
    TURN_RIGHT
} TurnMouse;

// **WICHTIG: DIE DEFINITION DER STRUKTUR `ShortestPathMove`**
typedef struct {
    int nextX;
    int nextY;
    Orientation nextOrientation;
} ShortestPathMove;

// Deklarationen der Funktionen, die in maze.c implementiert sind
// (Diese müssen in maze.c implementiert sein, damit sie hier deklariert werden können)
Orientation getNewOrientation(Orientation current_orientation, TurnMouse turn);
ShortestPathMove getNextShortestPathMove(int currentX, int currentY, Orientation currentOrientation);




typedef enum {
	STATE_IDLE,
	STATE_EXPLORE,
	STATE_WAIT_REPORT,
	STATE_SHORTEST_PATH
} RatState;


//extern float KP_TURN;

extern volatile RatState currentState; // Deklaration, Definition in .c

void RatStateMachine_Update(void);
void sendReportViaUART(void);
void TargetReached(void);

bool IsStartButtonPressed();

void updateTurn(WallfollowMode mode);

#endif /* APPLIKATION_STATE_MACHINE_H_ */
