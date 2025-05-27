/*
 * pathfinding.h
 *
 *  Created on: 17 Apr 2025
 *      Author: marcu
 */

#ifndef APPLIKATION_PATHFINDING_H_
#define APPLIKATION_PATHFINDING_H_

#include "Applikation/maze.h"

typedef enum {
    WALLFOLLOW_RIGHT,
    WALLFOLLOW_LEFT
} WallfollowMode;

// aktuelle Zellen Koordinaten
extern int currentX;
extern int currentY;

// Ziel Zellen Koordinaten
extern int targetX;
extern int targetY;

// aktuelle Ausrichtung
extern Orientation currentOrientation;

void wallfollower(WallfollowMode mode);
void floodfill(void);

#endif /* APPLIKATION_PATHFINDING_H_ */
