/**
 * @file system_interface.c
 * @brief Implementierung von grundlegenden Systemfunktionen.
 *
 * Dieses Modul bietet eine Abstraktionsschicht für systemnahe Funktionen,
 * die für den Betrieb des Roboters notwendig sind, wie z.B. blockierende Verzögerungen.
 * Es kapselt die spezifische Hardware-Abstraktionsschicht (HAL) für Timer.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 *
 * @dependencies
 * - "Funktionsschnittstellen/system_interface.h": Deklarationen der öffentlichen Schnittstellen dieses Moduls
 * - "Hardwaresteuerung/hal_timer.h": Hardware-Abstraktionsschicht für Timer-Funktionen
 * @note
 *  Dieses Modul ist als zentraler Sammelpunkt für allgemeine Systemdienste und plattformübergreifende
 *  Schnittstellen vorgesehen.
 */
#include <Funktionsschnittstellen/system_interface.h>
#include "Hardwaresteuerung/hal_timer.h"

/**
 * @brief Erzeugt eine blockierende Verzögerung für die angegebene Dauer.
 *
 * Diese Funktion pausiert die Programmausführung für die angegebene Anzahl
 * von Millisekunden. Sie nutzt eine hardwarenahe Timer-Funktion
 * ('Delay_Blocking_ms') aus der Hardware-Abstraktionsschicht.
 *
 * @param milliseconds Die Dauer der Verzögerung in Millisekunden [ms].
 * @note Dies ist eine blockierende Funktion, d.h., der Mikrocontroller
 * führt während der Verzögerung keine anderen Aufgaben aus.
 * @see Delay_Blocking_ms
 */
void Delay_ms(uint32_t milliseconds) {
	Delay_Blocking_ms(milliseconds);
}

