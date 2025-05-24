
#include "DAVE.h"
#include "stdio.h"
#include "Applikation/pathfinding.h"
#include "Applikation/maze.h"
#include "Applikation/state_machine.h"
#include "Funktionsschnittstellen/movement.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Hardwaresteuerung/hal_encoder.h"
#include "Hardwaresteuerung/hal_motor.h"

volatile RatState currentState = STATE_IDLE;

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
        	/*Turn(left);
        	while(1){}*/
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
				sendReportViaUART();
				reportSent = true;
			}

			if (reportSent && IsStartButtonPressed()) {
                for (volatile int i = 0; i < 10000000; i++) {}
				currentState = STATE_EXPLORE;
				currentX = 0;
				currentY = 0;
				currentOrientation = EAST;
				MazeMap_Init();
				updateMazeMap(currentX, currentY, currentOrientation);
				targetReachedDone = false; // Zurücksetzen für den nächsten Durchlauf
				reportSent = false;
			}
			break;
    }
}

// Funktion für die UART-Berichterstattung
void sendReportViaUART() {
    printMazeMap();
}
//------------MUSS NOCH NACH SCHICHTEN SEIN---------------------------------------------------------
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
