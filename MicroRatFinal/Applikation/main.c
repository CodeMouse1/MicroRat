/*
 * main.c
 *
 * Einstiegspunkt der Anwendung.
 * Initialisiert DAVE und die Hardware.
 * Startet die Hauptschleife und steuert die Roboter-Zustandsmaschine.
 * Kapselt DAVE-Aufrufe zentral am Start.
 */
#include "DAVE.h"
#include "Applikation/state_machine.h"
#include "Funktionsschnittstellen/sensors.h"

int main(void)
{
	DAVE_Init();           		   	/* Initialization of DAVE APPs */
	SensorsInit();					/* Sensoren initialisiert */

	while(true){
		RatStateMachine_Update();
	}
}
