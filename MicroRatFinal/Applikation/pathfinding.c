/*
 * pathfinding.c
 *
 *  Created on: 17 Apr 2025
 *      Author: marcu
 */
#include "Dave.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Funktionsschnittstellen/movement.h"

void wallfollower() {

    if (!IsWallFront()) {
        //MoveForwardPID();
    }

    else if (!IsWallLeft()) {
        Stop();
        Turn(left);
        //MoveForwardPID();
    }

    else if (IsWallLeft() && IsWallFront()) {
        Stop();
        Turn(right);
    }

    else if (IsWallLeft() && IsWallFront() && IsWallRight()){
        Stop();
        Turn(around);
    }
}

