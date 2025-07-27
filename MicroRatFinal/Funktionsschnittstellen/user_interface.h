/**
 * @file user_interface.h
 * @brief Deklarationen für die Benutzer-Feedback- und Eingabe-Schnittstellen.
 *
 * Dieses Header-File definiert die öffentlichen Schnittstellen
 * für das Benutzer-Feedback-Modul. Es ermöglicht anderen Teilen der Software,
 * den Benutzer über verschiedene Systemzustände zu informieren und Benutzereingaben
 * abzufragen.
 * Die Implementierungen dieser Funktionen nutzen typischerweise Hardware wie
 * Motoren (für akustisches Feedback) und LEDs (für visuelles Feedback)
 * sowie Taster für die Eingabe.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 */
#ifndef FUNKTIONSSCHNITTSTELLEN_USER_INTERFACE_H_
#define FUNKTIONSSCHNITTSTELLEN_USER_INTERFACE_H_

#include <stdbool.h>

/**
 * @brief Signalisiert dem Benutzer, dass ein Ziel erreicht wurde.
 *
 * Diese Funktion gibt ein kurzes, positives Feedback aus, um das erfolgreiche Erreichen eines Zielzustands
 * anzuzeigen.
 */
void SignalTargetReached(void);

/**
 * @brief Signalisiert dem Benutzer den Abschluss eines Optimierungsvorgangs.
 *
 * Diese Funktion gibt ein spezifisches Feedback-Signal aus, um zu bestätigen,
 * dass ein längerer Optimierungsprozess erfolgreich abgeschlossen wurde.
 */
void SignalOptimisationComplete(void);

/**
 * @brief Signalisiert das Ergebnis eines Diagnosetests an den Benutzer.
 *
 * Abhängig vom übergebenen 'result'
 * wird ein entsprechendes visuelles oder akustisches Signal ausgegeben,
 * um den Status der Systemdiagnose mitzuteilen.
 *
 * @param result Der Status des Diagnosetests: 'true' wenn alle Checks bestanden wurden,
 * 'false' wenn mindestens ein Fehler aufgetreten ist.
 */
void SignalDiagnosticsResult(bool result);

/**
 * @brief Signalisiert einen "Außerhalb des Bereichs"-Zustand oder einen kritischen Fehler.
 *
 * Diese Funktion wird aufgerufen, um einen ernsten Zustand zu signalisieren,
 * aus dem die MicroRat möglicherweise nicht selbstständig entkommen kann
 */
void SignalOutOfBounds(void);

/**
 * @brief Überprüft den Zustand des Starttasters.
 *
 * Liest den aktuellen Zustand des physischen Starttasters aus.
 * @return 'true' wenn der Taster gedrückt ist, 'false' wenn er nicht gedrückt ist.
 */
bool IsStartButtonPressed();

#endif /* FUNKTIONSSCHNITTSTELLEN_USER_INTERFACE_H_ */
