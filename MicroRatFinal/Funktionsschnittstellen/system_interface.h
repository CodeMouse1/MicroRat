/**
 * @file system_interface.h
 * @brief Deklarationen für grundlegenden Systemfunktionen.
 *
 * Dieses Header-File definiert die öffentliche Schnittstelle für
 * Funktionen, die zeitbasierte Operationen wie blockierende Verzögerungen
 * im System ermöglichen. Es abstrahiert die zugrundeliegende Hardware-Implementierung
 * der Timer und bietet eine einfache, portierbare Schnittstelle.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 */
#ifndef FUNKTIONSSCHNITTSTELLEN_SYSTEM_INTERFACE_H_
#define FUNKTIONSSCHNITTSTELLEN_SYSTEM_INTERFACE_H_

#include <stdint.h>

/**
 * @brief Erzeugt eine blockierende Verzögerung für die angegebene Dauer.
 *
 * Diese Funktion pausiert die Programmausführung für die angegebene Anzahl
 * von Millisekunden.
 *
 * @param milliseconds Die Dauer der Verzögerung in Millisekunden [ms].
 * @note Dies ist eine blockierende Funktion, was bedeutet, dass der Mikrocontroller
 * während der Ausführung dieser Funktion keine anderen Aufgaben erledigen kann.
 */
void Delay_ms(uint32_t milliseconds);

#endif /* FUNKTIONSSCHNITTSTELLEN_SYSTEM_INTERFACE_H_ */
