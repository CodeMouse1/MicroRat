/*
 * pathfinding.h
 *
 *  Created on: 17 Apr 2025
 *      Author: marcu
 */

#ifndef APPLIKATION_PATHFINDING_H_
#define APPLIKATION_PATHFINDING_H_

typedef enum {
    WALLFOLLOW_RIGHT,
    WALLFOLLOW_LEFT
} WallfollowMode;

void wallfollower(WallfollowMode mode);
void floodfill(void);
void TargetReached(void);

#endif /* APPLIKATION_PATHFINDING_H_ */
