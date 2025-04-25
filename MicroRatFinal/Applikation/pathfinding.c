/*
 * pathfinding.c
 *
 *  Created on: 17 Apr 2025
 *      Author: marcu
 */
#include "Dave.h"
#include "Applikation/pathfinding.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Funktionsschnittstellen/movement.h"
#include "Hardwaresteuerung/hal_startbutton.h"
#include "Hardwaresteuerung/hal_motor.h"
#include <stdio.h>

void wallfollower() {

    if (IsWallFront()) {
		Stop();
		Delay(500);
		Turn(left);
		Delay(500);
    }else{
    	DrivePD();

	}

}/*

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
    }*/
    //MotorsDrive();


