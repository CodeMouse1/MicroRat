/*
 * pathfinding.c
 *
 *  Created on: 17 Apr 2025
 *      Author: marcu
 */
#include "Dave.h"
#include "Applikation/maze.h"
#include "Applikation/pathfinding.h"
#include "Applikation/state_machine.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Funktionsschnittstellen/movement.h"

/* Deklarationen der globalen Variablen für Position und Ausrichtung (aus main.c)*/

// aktuelle Zellen Koordinaten
extern int currentX;
extern int currentY;

// Ziel Zellen Koordinaten
extern int targetX;
extern int targetY;

// aktuelle Ausrichtung
extern Orientation currentOrientation;

// Interne Hilfsfunktion zur Aktualisierung der Ausrichtung & Map
static void updateOrientation(TurnDirection direction);
static void updatePositionAndMap(void);

void wallfollower(WallfollowMode mode) {
    TurnDirection turnDirection = none;
    while (1) {
    	if (currentX == targetX && currentY == targetY) {
			break;
		}
        if (mode == WALLFOLLOW_RIGHT) {
            if (!IsWallRight()) {
                turnDirection = right;
            }
            else if (IsWallFront() && IsWallRight() && !IsWallLeft()) {
                turnDirection = left;
            } else if (IsWallFront() && IsWallRight() && IsWallLeft()) {
                turnDirection = around;
            }
        } else if (mode == WALLFOLLOW_LEFT) {
            if (!IsWallLeft()) {
                turnDirection = left;
            }
            else if (IsWallFront() && IsWallLeft() && !IsWallRight()) {
                turnDirection = right;
            } else if (IsWallFront() && IsWallRight() && IsWallLeft()) {
                turnDirection = around;
            }
        }
        // Falls Drehung, ausführen & Ausrichtung aktualisieren
        if (turnDirection != none) {
            Turn(turnDirection);
            updateOrientation(turnDirection);
		// Nach Drehung fahre eine Zelle weiter, aktualisiere Position & Map
			MoveOneCell();
			updatePositionAndMap();
        } else {
		// Falls keine Drehung erforderlich, fahre eine Zelle & aktualisiere Position & Map
        	MoveOneCell();
            updatePositionAndMap();
        }
        // Drehungsrichung zurücksetzen
        turnDirection = none;
    }
}

static void updateOrientation(TurnDirection direction) {
    switch (direction) {
        case right:
            if (currentOrientation == NORTH) currentOrientation = EAST;
            else if (currentOrientation == EAST) currentOrientation = SOUTH;
            else if (currentOrientation == SOUTH) currentOrientation = WEST;
            else if (currentOrientation == WEST) currentOrientation = NORTH;
            break;
        case left:
            if (currentOrientation == NORTH) currentOrientation = WEST;
            else if (currentOrientation == WEST) currentOrientation = SOUTH;
            else if (currentOrientation == SOUTH) currentOrientation = EAST;
            else if (currentOrientation == EAST) currentOrientation = NORTH;
            break;
        case around:
            if (currentOrientation == NORTH) currentOrientation = SOUTH;
            else if (currentOrientation == EAST) currentOrientation = WEST;
            else if (currentOrientation == SOUTH) currentOrientation = NORTH;
            else if (currentOrientation == WEST) currentOrientation = EAST;
            break;
        default:
            break;
    }
}

static void updatePositionAndMap() {
    if (currentOrientation == NORTH) currentY++;
    else if (currentOrientation == EAST) currentX++;
    else if (currentOrientation == SOUTH) currentY--;
    else if (currentOrientation == WEST) currentX--;
    updateMazeMap(currentX, currentY, currentOrientation);
}

void floodfill(){
	// WIP
}


