/**
 * @file main.c
 * @brief Einstiegspunkt und Hauptschleifensteuerung der Microrat-Anwendung.
 *
 * Dieses Modul bildet das Herzstück des Starts der Microrat-Anwendung.
 * Es ist verantwortlich für die Initialisierung aller notwendigen Hardware- und
 * Softwarekomponenten durch die Nutzung der DAVE APPs.
 * Nach erfolgreicher Initialisierung wird die unendliche Hauptschleife gestartet,
 * welche die kontinuierliche Ausführung der Zustandsmaschine
 * ('RatStateMachine_Update()') sicherstellt.
 *
 * Alle systemweiten Initialisierungsaufrufe, insbesondere die von DAVE generierten,
 * sind hier zentral am Anfang der Anwendung gekapselt, um einen klaren Startpunkt
 * zu gewährleisten.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 */
#include "DAVE.h"
#include "Applikation/state_machine.h"
#include "Funktionsschnittstellen/sensors.h"

int main(void) {

	DAVE_Init();           		   	///- Initialisierung der DAVE APPs
	Sensors_Init();					///- Sensoren initialisiert

	// Hauptschleife der Anwendung
	while(true) {
		RatStateMachine_Update();	///- Aktualisiert den aktuellen Zustand der MicroRat und führt entsprechende Aktionen aus
	}
}
