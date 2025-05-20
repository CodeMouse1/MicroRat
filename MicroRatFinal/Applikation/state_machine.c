/*
 * state_machine.c
 *
 *  Created on: 20 May 2025
 *      Author: marcu
 */

#include "DAVE.h"
#include "stdio.h"
#include "Applikation/pathfinding.h"
#include "Applikation/maze.h"
#include "Applikation/state_machine.h"
#include "Funktionsschnittstellen/start_condition.h"
#include "Funktionsschnittstellen/movement.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Hardwaresteuerung/hal_encoder.h"
#include "Hardwaresteuerung/hal_motor.h"

volatile RatState currentState = STATE_IDLE;

static bool targetReachedDone = false;
static bool reportSent = false;

/*void RobotStateMachine_Init(void) {
    // Initialisierungslogik, falls nötig
}*/

void RatStateMachine_Update(void) {
    switch (currentState) {
        case STATE_IDLE:
            // Warte auf den Startknopf
            if (IsStartButtonPressed()) {
                for (volatile int i = 0; i < 5000000; i++) {}
                currentState = STATE_EXPLORE;
                 // Zurücksetzen der Position für neue Erkundung
                currentX = 0;
                currentY = 0;
                currentOrientation = EAST;
                MazeMap_Init(); // Karte zurücksetzen
                updateMazeMap(currentX, currentY, currentOrientation);
                //printMazeMap();
            }
            break;
        case STATE_EXPLORE:
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
                for (volatile int i = 0; i < 5000000; i++) {}
				currentState = STATE_EXPLORE;
				currentX = 0;
				currentY = 0;
				currentOrientation = EAST;
				MazeMap_Init();
				updateMazeMap(currentX, currentY, currentOrientation);
				//printMazeMap(); // Entfernt, um es optional zu machen
				targetReachedDone = false; // Zurücksetzen für den nächsten Durchlauf
				reportSent = false;
			}
			break;
    }
}

// Funktion für die UART-Berichterstattung
void sendReportViaUART() {
    uint8_t reportString[200];
    sprintf((char*)reportString, "Ziel erreicht! Position: X=%d, Y=%d, Orientierung: %d\r\n", currentX, currentY, currentOrientation);
    UART_Transmit(&UART_COM, reportString, strlen((char*)reportString));
    printMazeMap();
}

// Überprüft, ob der Startknopf gedrückt wurde (muss noch implementiert werden)
bool IsStartButtonPressed() {
    // Hier muss die Logik für die Abfrage des Startknopfs implementiert werden.
    // Das hängt von der spezifischen Hardware ab (z.B. digitaler Eingang).
    //  Rückgabe: true, wenn der Knopf gedrückt ist, false sonst.
    return DIGITAL_IO_GetInput(&DIGITAL_IO_BUTTON);  //Anpassen an deinen Start Button
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
