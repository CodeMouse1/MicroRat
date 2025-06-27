/**
 * @file state_machine.c
 * @brief Implementiert die zentrale Zustandsmaschine der MicroRat.
 *
 * Dieses Modul steuert den gesamten Anwendungsablauf der MicroRat,
 * von der Initialisierung über die Labyrintherkundung und Berichterstattung
 * bis hin zur Ausführung des kürzesten Pfades. Es verwaltet den globalen
 * Zustand des Roboters, dessen aktuelle Position und Ausrichtung
 * und koordiniert die Aufrufe an alle anderen Applikations- und
 * Funktionsschnittstellen-Module.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 *
 * @dependencies
 * - Applikation/pathfinding.h: Für Pfadfindungsalgorithmen
 * - Applikation/maze.h: Für Labyrinth-Kartenmanagement
 * - Applikation/state_machine.h: Deklarationen für dieses Modul, insbesondere die 'RatState' Enumeration.
 * - Funktionsschnittstellen/movement.h: Für die MicroRatbewegung.
 * - Funktionsschnittstellen/sensors.h: Für Sensorabfragen
 * - Funktionsschnittstellen/user_interface.h: Für Benutzerrückmeldungen
 * - Funktionsschnittstellen/system_interface.h: Für Verzögerungen ('Delay_ms')
 * - <stdbool.h>: Für den 'bool' Datentyp
 */
#include <stdbool.h>
#include "Applikation/pathfinding.h"
#include "Applikation/maze.h"
#include "Applikation/state_machine.h"
#include "Funktionsschnittstellen/movement.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Funktionsschnittstellen/user_interface.h"
#include "Funktionsschnittstellen/system_interface.h"

/**
 * @brief Die aktuelle Zustandsvariable der MicroRat-Zustandsmaschine.
 *
 * Diese `volatile` Variable steuert das übergeordnete Verhalten des Roboters
 */
volatile RatState currentState = STATE_IDLE;

int currentX = 0;	///- Die aktuelle X-Koordinate der MicroRat im Labyrinth.
int currentY = 0;	///- Die aktuelle Y-Koordinate der MicroRat im Labyrinth.
int targetX = 6;	///- Die X-Koordinate der aktuellen Zielzelle.
int targetY = 3;	///- Die Y-Koordinate der aktuellen Zielzelle.

Orientation currentOrientation = EAST;	///- Die aktuelle Ausrichtung der MicroRat.

// Flags für Aktionen
static bool targetReachedDone = false;
static bool reportSent = false;
static bool diagnosticsPerformed = false;

// Zähler Wanderkennung, für Timeout
static int noWall = 0;

/**
 * @brief Überprüft, ob die MicroRat sich in einem Bereich ohne Wände befindet.
 * Wenn eine bestimmte Anzahl von aufeinanderfolgenden Zellen ohne Wände erkannt wird,
 * signalisiert dies einen Fehlerzustand.
 */
static void checkNoWallsAndTimeout(void);

/**
 * @brief Aktualisiert den Zustand der MicroRat-Zustandsmaschine.
 * Diese Funktion wird in der Hauptschleife des Systems aufgerufen werden.
 * Sie implementiert die Logik für den Übergang zwischen den verschiedenen Zuständen
 * des Roboters und ruft die entsprechenden Funktionen der anderen Module auf.
 */
void RatStateMachine_Update(void) {
    switch (currentState) {
        case STATE_IDLE:
            // Warte auf den Startknopf
            if (IsStartButtonPressed()) {
            	// Wartet 5sek
            	Delay_ms(5000);
            	// Ausführen der Diagnose
				if (!diagnosticsPerformed) {
					PerformDiagnosticCheck();
					diagnosticsPerformed = true;
				}
				// Wechsle in den Erkundungszustand
                currentState = STATE_EXPLORE;
                // MicroRat-Zustand für den Start der Erkundung initialisieren:
                currentX = 0;
                currentY = 0;
                currentOrientation = EAST;
                MazeMap_Init();
                // Die Karte an der Startposition mit den ersten Sensordaten aktualisieren.
                MazeMap_Update(currentX, currentY, currentOrientation);
            }
            break;
        case STATE_EXPLORE:
        	// Erkundung mittels Wallfollower
        	Pathfinding_Wallfollower(WALLFOLLOW_LEFT);
        	checkNoWallsAndTimeout();
            if (currentX == targetX && currentY == targetY) {
				currentState = STATE_WAIT_REPORT;
            }
            break;
        case STATE_WAIT_REPORT:
			// Ziel erreicht, melde und warte
			if (!targetReachedDone) {
				SignalTargetReached();
				targetReachedDone = true;
			}
			// Bei Knopfdruck schick Labyrinthmap über UART
			if (!reportSent && IsStartButtonPressed()) {
                Pathfinding_CalculateDistanceMap(targetX, targetY);
                MazeMap_Print();
				reportSent = true;
			}
			// Bei nächsten Knopfdruck Ausführung des optimierten Pfades
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
        	//Ausführung des optimalen Pfades bis Ziel erreicht wurde
        	if (!Pathfinding_ExecuteShortestPath()) {
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
