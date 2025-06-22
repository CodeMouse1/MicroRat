/*
 * @file main.c
 * @brief Einstiegspunkt der Microrat-Anwendung und zentrale Steuerung der Hauptschleife.
 *
 * Dieses Modul bildet das Herzstück des Starts. Es ist verantwortlich für
 * die Initialisierung aller notwendigen Hardware- und Softwarekomponenten über DAVE APPs.
 * Anschließend startet es die unendliche Hauptschleife, die die kontinuierliche
 * Ausführung der Roboter-Zustandsmaschine ('RatStateMachine_Update').
 *
 * Alle systemweiten Initialisierungsaufrufe, insbesondere die von DAVE generierten,
 * sind hier zentral am Anfang der Anwendung gekapselt.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-22
 * @version 1.0
 *
 * @dependencies
 * - DAVE.h (Für die Initialisierung der DAVE APPs)
 * - Applikation/state_machine.h (Für die Steuerung der Rat-Zustandsmaschine)
 * - Funktionsschnittstellen/sensors.h (Für die Initialisierung der Sensoren)
 */
#include "DAVE.h"
#include "Applikation/state_machine.h"
#include "Funktionsschnittstellen/sensors.h"

int main(void)
{
	DAVE_Init();           		   	// Initialization of DAVE APPs
	SensorsInit();					// Sensoren initialisiert


	while(true){
		RatStateMachine_Update();	// Aktualisiert den aktuellen Zustand der Rat und führt entsprechende Aktionen aus
		SensorsPrint();
	}
}
