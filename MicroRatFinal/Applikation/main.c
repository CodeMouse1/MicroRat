#include "DAVE.h"
#include "stdio.h"
#include "Applikation/maze.h"
#include "Applikation/pathfinding.h"
#include "Applikation/state_machine.h"
#include "Funktionsschnittstellen/movement.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Funktionsschnittstellen/pid_regler.h"


// Globale Variablen für Mausposition und -ausrichtung
int currentX = 0;
int currentY = 0;
int targetX = 6;
int targetY = 3;
Orientation currentOrientation = EAST;

int main(void)
{
	DAVE_Init();           		   	/* Initialization of DAVE APPs */
	MovementInit();		   			/* Motoren auf Vorwärts eingestellt */
	SensorsInit();					/* Sensoren initialisiert */

	/*while(1){
		DebugPrint();

	}*/

	while(true){
		RatStateMachine_Update();
	}
}
