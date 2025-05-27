
#include "DAVE.h"
#include "stdio.h"
#include "Applikation/pathfinding.h"
#include "Applikation/maze.h"
#include "Applikation/state_machine.h"
#include "Funktionsschnittstellen/movement.h"
#include "Funktionsschnittstellen/pd_regler.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Hardwaresteuerung/hal_encoder.h"
#include "Hardwaresteuerung/hal_motor.h"

volatile RatState currentState = STATE_IDLE;

// Diese Variablen MÜSSEN global (nicht static) sein, damit andere Dateien darauf zugreifen können.
int currentX = 0;
int currentY = 0;
Orientation currentOrientation = EAST; // Startausrichtung des Roboters

// Zielkoordinaten für das Labyrinth (können auch woanders definiert sein, z.B. maze.h)
int targetX = 3; // Beispielziel x-Koordinate
int targetY = 1; // Beispielziel y-Koordinate

static bool targetReachedDone = false;
static bool reportSent = false;

void RatStateMachine_Update(void) {
    switch (currentState) {
        case STATE_IDLE:
            // Warte auf den Startknopf
            if (IsStartButtonPressed()) {
                for (volatile int i = 0; i < 10000000; i++) {}
                currentState = STATE_EXPLORE;
                 // Zurücksetzen der Position für neue Erkundung
                currentX = 0;
                currentY = 0;
                currentOrientation = EAST;
                MazeMap_Init(); // Karte zurücksetzen
                updateMazeMap(currentX, currentY, currentOrientation);
            }
            break;
        case STATE_EXPLORE:
        	//updateTurn(WALLFOLLOW_LEFT);
        	// Wandverfolgung
        	wallfollower(WALLFOLLOW_LEFT);

            if (currentX == targetX && currentY == targetY) {
				currentState = STATE_WAIT_REPORT;
            }
            break;
        case STATE_WAIT_REPORT:
			// Ziel erreicht, melde und warte
			if (!targetReachedDone) {
				TargetReached();
				targetReachedDone = true;
			}

			if (!reportSent && IsStartButtonPressed()) {
                calculateDistanceMap(targetX, targetY);
				sendReportViaUART();
				reportSent = true;
			}

			if (reportSent && IsStartButtonPressed()) {
                for (volatile int i = 0; i < 10000000; i++) {}
				currentState = STATE_SHORTEST_PATH;
				currentX = 0;
				currentY = 0;
				currentOrientation = EAST;
				targetReachedDone = false; // Zurücksetzen für den nächsten Durchlauf
				reportSent = false;
			}
			break;
        case STATE_SHORTEST_PATH:
        	// Prüfe, ob das Ziel erreicht ist (Distanz ist 0)
			if (distanceMap[currentY][currentX] == 0) {
				MotorsSetSpeed(0, 0); // Roboter anhalten
				TargetReached();      // Ziel erreicht signalisieren
				currentState = STATE_IDLE; // Wechsel zurück in den IDLE-Zustand
				break; // Zustand verlassen
			}

			// Finde den nächsten besten Zug basierend auf der Distanzkarte
			ShortestPathMove next_step = getNextShortestPathMove(currentX, currentY, currentOrientation);

			// Überprüfe, ob sich die Position oder Ausrichtung tatsächlich ändern würde.
			// Dies dient als grundlegende Fehlerbehandlung, falls der Roboter
			// feststeckt oder kein gültiger nächster Schritt gefunden wird (was bei
			// einem funktionierenden Flood Fill nicht passieren sollte, solange Distanz > 0).
			if (next_step.nextX == currentX && next_step.nextY == currentY && next_step.nextOrientation == currentOrientation) {
				MotorsSetSpeed(0, 0); // Roboter anhalten
				// Optional: Eine Fehlermeldung über UART senden
				currentState = STATE_IDLE; // Wechsel in den IDLE-Zustand
				break; // Zustand verlassen
			}

			// Drehung ausführen, falls die Ausrichtung geändert werden muss
			if (next_step.nextOrientation != currentOrientation) {
				// Bestimme, ob eine Links- oder Rechtsdrehung nötig ist
				if (getNewOrientation(currentOrientation, TURN_LEFT) == next_step.nextOrientation) {
					Turn(left);
				} else if (getNewOrientation(currentOrientation, TURN_RIGHT) == next_step.nextOrientation) {
					Turn(right);
				}
				currentOrientation = next_step.nextOrientation; // Aktuelle Ausrichtung aktualisieren
				}

			// Vorwärts zur nächsten Zelle bewegen
			MoveOneCell();

			// Aktuelle Position des Roboters aktualisieren
			currentX = next_step.nextX;
			currentY = next_step.nextY;
			break; // Zustand verlassen, nächste Iteration der State Machine

		default:
			// Optional: Fehlerbehandlung oder nichts tun für andere Zustände
			break;
	}
}

// Funktion für die UART-Berichterstattung
void sendReportViaUART() {
    printMazeMap();
}

// Überprüft, ob der Startknopf gedrückt wurde
bool IsStartButtonPressed() {
    return DIGITAL_IO_GetInput(&DIGITAL_IO_BUTTON);
}

void TargetReached() {
    MotorsSetSpeed(1000, 1000);
    for (volatile int i = 0; i < 500000; i++) {}
    MotorsSetSpeed(0, 0);
    for (volatile int i = 0; i < 500000; i++) {}
    MotorsSetSpeed(1000, 1000);
    for (volatile int i = 0; i < 500000; i++) {}
    MotorsSetSpeed(0, 0);
    DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_1);
    DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_2);
}
