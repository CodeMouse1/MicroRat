/*
 * pathfinding.h
 *
 *  Created on: 17 Apr 2025
 *      Author: marcu
 */

#ifndef APPLIKATION_PATHFINDING_H_
#define APPLIKATION_PATHFINDING_H_

#include "Applikation/maze.h"
#include "Applikation/state_machine.h"

#define QUEUE_SIZE (MAZE_WIDTH * MAZE_HEIGHT)

typedef enum {
    WALLFOLLOW_RIGHT,
    WALLFOLLOW_LEFT
} WallfollowMode;

typedef struct {
    int x;
    int y;
} Cell;

typedef struct {
    int nextX;
    int nextY;
    Orientation nextOrientation;
} ShortestPathMove;

extern int currentX;
extern int currentY;
extern int targetX;
extern int targetY;

extern Orientation currentOrientation;

bool executeShortestPathStep(void);

void wallfollower(WallfollowMode mode);
void calculateDistanceMap(int targetX, int targetY);

Orientation getNewOrientation(Orientation current_orientation, TurnMouse turn);
ShortestPathMove getNextShortestPathMove(int currentX, int currentY, Orientation currentOrientation);

#endif /* APPLIKATION_PATHFINDING_H_ */
