/**
 * @file state_machine.c
 * @brief Implementiert die zentrale Zustandsmaschine der Rat.
 *
 * Dieses Modul steuert den gesamten Anwendungsablauf des MicroRat,
 * von der Initialisierung über die Labyrintherkundung und Berichterstattung
 * bis hin zur Ausführung des kürzesten Pfades. Es verwaltet den globalen
 * Zustand des Roboters, dessen aktuelle Position und Ausrichtung
 * und koordiniert die Aufrufe an alle anderen Applikations- und
 * Funktionsschnittstellen-Module.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-22
 * @version 1.0
 *
 * @dependencies
 * - Applikation/pathfinding.h: Für Pfadfindungsalgorithmen (`wallfollower`, `calculateDistanceMap`, `executeShortestPathStep`).
 * - Applikation/maze.h: Für Labyrinth-Kartenmanagement (`MazeMap_Init`, `updateMazeMap`, `printMazeMap`).
 * - Applikation/state_machine.h: Deklarationen für dieses Modul, insbesondere die 'RatState' Enumeration.
 * - Funktionsschnittstellen/movement.h: Für die Roboterbewegung (`Stop`).
 * - Funktionsschnittstellen/sensors.h: Für Sensorabfragen (`IsStartButtonPressed`, `IsWallFront`, `IsWallLeft`, `IsWallRight`).
 * - Funktionsschnittstellen/user_interface.h: Für Benutzerrückmeldungen (`SignalTargetReached`, `SignalOptimisationComplete`, `SignalOutOfBounds`, `PerformDiagnosticCheck`).
 * - Funktionsschnittstellen/timer_utils.h: Für Verzögerungen (`Delay_ms`).
 * - <stdbool.h>: Für den 'bool' Datentyp.
 */
#include <stdbool.h>
#include "Applikation/pathfinding.h"
#include "Applikation/maze.h"
#include "Applikation/state_machine.h"
#include "Funktionsschnittstellen/movement.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Funktionsschnittstellen/user_interface.h"
#include "Funktionsschnittstellen/timer_utils.h"

//#include "Hardwaresteuerung/hal_digital_io.h"
#include "Hardwaresteuerung/hal_motor.h"



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

/*
 * @brief Überprüft, ob der Roboter sich in einem Bereich ohne Wände befindet oder ein Timeout eintritt.
 * Wenn eine bestimmte Anzahl von aufeinanderfolgenden Zellen ohne Wände erkannt wird,
 * signalisiert dies einen Fehlerzustand (z.B. Roboter ist aus dem Labyrinth gefallen).
 */
static void checkNoWallsAndTimeout(void);

/*
 * @brief Aktualisiert den Zustand der Roboter-Zustandsmaschine.
 * Diese Funktion sollte regelmäßig in der Hauptschleife des Systems aufgerufen werden.
 * Sie implementiert die Logik für den Übergang zwischen den verschiedenen Zuständen
 * des Roboters und ruft die entsprechenden Funktionen der anderen Module auf.
 */
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
                currentState = STATE_EXPLORE;	// Wechsle in den Erkundungszustand
                // Roboter-Zustand für den Start der Erkundung initialisieren:
                currentX = 0;
                currentY = 0;
                currentOrientation = EAST;
                MazeMap_Init();	// Labyrinthkarte initialisieren (alle Wände unbekannt).
                // Die Karte an der Startposition mit den ersten Sensordaten aktualisieren.
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
