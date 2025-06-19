/*
 * state_machine.c
 *
 * Implementiert die zentrale Zustandsmaschine des Roboters.
 * Steuert den gesamten Anwendungsablauf (Idle, Erkundung, Berichterstattung, kürzester Pfad).
 * Verwaltet den globalen Zustand, die Position und die Ausrichtung des Roboters.
 * Koordiniert Aufrufe an alle anderen Applikations- und Funktionsschnittstellen-Module.
 */
#include <stdbool.h>
#include <stdio.h>
#include "Applikation/pathfinding.h"
#include "Applikation/maze.h"
#include "Applikation/state_machine.h"
#include "Funktionsschnittstellen/movement.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Funktionsschnittstellen/user_interface.h"
#include "Funktionsschnittstellen/timer_utils.h"


volatile RatState currentState = STATE_IDLE;

int currentX = 0;
int currentY = 0;
int targetX = 6;
int targetY = 3;

Orientation currentOrientation = EAST;

static bool targetReachedDone = false;
static bool reportSent = false;
static bool diagnosticsPerformed = false;

static int noWall = 0;

static void checkNoWallsAndTimeout(void);

void RatStateMachine_Update(void) {
    switch (currentState) {
        case STATE_IDLE:
            // Warte auf den Startknopf
            if (IsStartButtonPressed()) {
				Delay_ms(5000);
            	// Ausführen der Diagnose
				if (!diagnosticsPerformed) {
					PerformDiagnosticCheck();
					diagnosticsPerformed = true;
				}
                currentState = STATE_EXPLORE;
                currentX = 0;
                currentY = 0;
                currentOrientation = EAST;
                MazeMap_Init();
                updateMazeMap(currentX, currentY, currentOrientation);
            }
            break;
        case STATE_EXPLORE:
        	// Erkundung mittels Wallfollower
        	wallfollower(WALLFOLLOW_LEFT);
        	checkNoWallsAndTimeout();
            if (currentX == 6/*targetX*/ && currentY == 3 /*targetY*/) {
				currentState = STATE_WAIT_REPORT;
            }
            break;
        case STATE_WAIT_REPORT:
			// Ziel erreicht, melde und warte
			if (!targetReachedDone) {
				SignalTargetReached();
				targetReachedDone = true;
			}
			if (!reportSent && IsStartButtonPressed()) {
                calculateDistanceMap(targetX, targetY);
                printMazeMap();
				reportSent = true;
			}
			if (reportSent && IsStartButtonPressed()) {
				Delay_ms(5000);
                SignalOptimisationComplete();
				currentState = STATE_SHORTEST_PATH;
				currentX = 0;
				currentY = 0;
				currentOrientation = EAST;
				targetReachedDone = false;
				reportSent = false;
			}
			break;
        case STATE_SHORTEST_PATH:
        	if (!executeShortestPathStep()) {
        		SignalTargetReached();
				currentState = STATE_IDLE;
			}
			break;

		default:
			break;
	}
}

static void checkNoWallsAndTimeout(void) {
    if (!IsWallFront() && !IsWallLeft() && !IsWallRight()) {
        noWall++; // Zähler erhöhen, wenn keine Wände da sind
        if (noWall >= NO_WALL_TIMEOUT_THRESHOLD) { // Schwellenwert in Schleifendurchläufen
            Stop();
            SignalOutOfBounds();
        }
    } else {
        noWall = 0;
    }
}
