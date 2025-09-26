/**
 * @file pd_controller.h
 * @brief Deklarationen der PD-Regler-Funktionen und relevanter Konstanten für die MicroRat.
 *
 * Diese Header-Datei definiert die öffentliche Schnittstelle für den Proportional-Differential-Regler (PD-Regler),
 * der die Motoren der MicroRat steuert. Sie enthält Funktionsprototypen für die Reglerlogik,
 * die Zielsetzung und das Zurücksetzen des Reglers. Darüber hinaus werden wichtige Konstanten
 * für die PD-Parameter (KP, KD), PWM-Begrenzungen und die Toleranzen für die Zielerfassung definiert.
 * Eine integrierte Logging-Funktionalität für Debugging-Zwecke ist ebenfalls deklariert.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 *
 * @dependencies
 * - <stdbool.h>: Für den `bool`-Datentyp.
 */
#ifndef FUNKTIONSSCHNITTSTELLEN_PD_CONTROLLER_H_
#define FUNKTIONSSCHNITTSTELLEN_PD_CONTROLLER_H_

#include <stdbool.h>

/**
 * @brief Maximaler PWM-Wert für die Motorsteuerung.
 *
 * Definiert den maximalen absoluten Wert, den ein PWM-Signal für die Ansteuerung der Motoren
 * erreichen kann (im Bereich von -PWM_MAX bis +PWM_MAX). Dies dient als obere und untere
 * Begrenzung für die vom Regler berechneten Stellgrößen.
 */
#define PWM_MAX 3300

/**
 * @brief Anzahl der stabilen Zyklen, die erfüllt sein müssen, damit der Roboter als "am Ziel" gilt.
 *
 * Damit die 'PDdone()'-Funktion signalisiert, dass das Regelziel erreicht wurde, müssen
 * die Positionsfehler beider Räder für diese Anzahl von aufeinanderfolgenden Reglerzyklen
 * innerhalb der definierten Toleranz liegen. Dies hilft,
 * Oszillationen am Zielpunkt zu ignorieren und eine tatsächliche Stabilität zu gewährleisten.
 */
#define CYCLES_THRESHOLD 1

/**
 * @brief Proportionaler Verstärkungsfaktor für den Gleichlaufregler.
 *
 * Dieser Parameter bestimmt, wie stark der Gleichlaufregler auf Abweichungen zwischen
 * den beiden Radpositionen reagiert. Ein höherer Wert führt zu einer aggressiveren
 * Korrektur, um die Spur zu halten oder die Drehgenauigkeit zu verbessern.
 */
#define KP_GLEICHLAUF 1000.0f

/**
 * @brief Proportionaler Verstärkungsfaktor für Drehbewegungen.
 *
 * Definiert den Proportionalanteil des PD-Reglers, wenn sich der Roboter in einem
 * Drehmodus befindet ('isTurning' ist wahr). Er beeinflusst die Stärke der Korrektur
 * basierend auf dem aktuellen Positionsfehler während Drehungen.
 */
#define KP_TURN 90.0f

/**
 * @brief Differentialer Verstärkungsfaktor für Drehbewegungen.
 *
 * Definiert den Differentialanteil des PD-Reglers für Drehbewegungen. Dieser Wert
 * reagiert auf die Änderungsrate des Fehlers und hilft, ein Überschwingen des Roboters
 * während Drehungen zu reduzieren und die Stabilität zu verbessern.
 */
#define KD_TURN 50.0f

/**
 * @brief Proportionaler Verstärkungsfaktor für Geradeausfahrt.
 *
 * Dieser Wert wird als 'extern' deklariert da es in movement.c definiert ist und hier nur bekannt gemacht wird.
 * Er steuert den Proportionalanteil des PD-Reglers bei Geradeausfahrt.
 */
extern float KP_STRAIGHT;

/**
 * @brief Differentialer Verstärkungsfaktor für Geradeausfahrt.
 *
 * Definiert den Differentialanteil des PD-Reglers für die Geradeausfahrt. Er reagiert
 * auf die Änderungsrate des Fehlers, um ein gleichmäßiges Fahrverhalten zu erzielen
 * und Oszillationen bei Geradeausfahrt zu minimieren.
 */
#define KD_STRAIGHT 10.0f

extern volatile bool isTurning;

/**
 * @brief Aktualisiert den Zustand des PD-Reglers und steuert die Motoren.
 *
 * Diese Funktion ist der Kern des PD-Regelkreises. Sie liest die aktuellen Encoder-Werte,
 * berechnet Positionsfehler, wendet proportionale und differentielle Korrekturen an
 * und passt die Motor-PWM-Werte an, um die Sollpositionen zu erreichen. Sie wird in festen
 * Intervallen von 1ms aufgerufen.
 */
void UpdatePD(void);

/**
 * @brief Setzt die Ziel-Distanzen für den PD-Regler.
 *
 * Diese Funktion wird vom Modul 'movement.c' aufgerufen,
 * um dem PD-Regler die neuen Sollpositionen für das rechte und linke Rad in Millimetern mitzuteilen.
 * Die Regelung beginnt, sobald neue Ziele gesetzt werden.
 *
 * @param distance_R Die Soll-Distanz für das rechte Rad in Millimetern [mm].
 * @param distance_L Die Soll-Distanz für das linke Rad in Millimetern [mm].
 */
void setPDGoalD(float distance_R, float distance_L);

/**
 * @brief Setzt alle internen Zustandsvariablen des PD-Reglers zurück.
 *
 * Diese Funktion initialisiert den Regler für eine neue Bewegungssequenz. Sie setzt alle
 * akkumulierten Fehler, Sollwerte, Encoder-Positionen und internen Zustandsflags auf ihre
 * Anfangswerte (typischerweise Null), um "Restfehler" aus vorherigen Bewegungen zu eliminieren
 * und einen sauberen Start zu gewährleisten. Optional wird der Log-Puffer geleert und ausgegeben.
 */
void ResetPD(void);

/**
 * @brief Überprüft, ob das PD-Regelziel erreicht und der Roboter stabil am Ziel ist.
 *
 * Die Funktion evaluiert die aktuellen Positionsfehler beider Räder. Sie gibt '1' (true) zurück,
 * wenn die Fehler für eine bestimmte Anzahl von aufeinanderfolgenden Zyklen ('CYCLES_THRESHOLD')
 * innerhalb der definierten Fehlertoleranz liegen, was bedeutet,
 * dass der Roboter seine Zielposition erreicht hat und stabil ist. Andernfalls gibt sie '0' (false) zurück.
 *
 * @return '1' wenn das Ziel erreicht und die MicroRat stabil ist; '0' sonst.
 */
int PDdone(void);

/*
---
 Debugging- und Logging-Funktionalität
---
*/
#define PD_LOG_BUFFER_SIZE 300 // Anzahl der gespeicherten Einträge
#define PD_LOG_INTERVAL 2      // Jede 2. PD-Iteration speichern

/**
 * @brief Struktur für einen einzelnen Log-Eintrag des PD-Reglers.
 *
 * Diese Struktur kapselt die wesentlichen Parameter des PD-Reglers zu einem spezifischen
 * Zeitpunkt. Sie dient der nachträglichen Analyse des Reglerverhaltens, der Fehlerbehebung
 * und dem Tuning der PID-Parameter.
 */
typedef struct {
    float error_L;
    float error_R;
    float correction_L;
    float correction_R;
    float current_pos_L;
    float current_pos_R;
    int is_stable_condition_met; // 1, wenn Bedingung erfüllt; 0 sonst
} PdLogEntry;

/**
 * @brief Speichert einen einzelnen Eintrag der PD-Regler-Daten im internen Ringpuffer.
 *
 * Diese Hilfsfunktion wird von 'UpdatePD()' aufgerufen, um aktuelle Regler-Parameter
 * (Fehler, Korrekturen, aktuelle Positionen, Stabilitätsstatus) für das Debugging
 * und die nachträgliche Analyse zu protokollieren. Der Puffer funktioniert als Ringpuffer.
 *
 * @param error_L Der aktuelle Fehler des linken Rades (Soll - Ist) in [mm].
 * @param error_R Der aktuelle Fehler des rechten Rades (Soll - Ist) in [mm].
 * @param correction_L Die vom PD-Regler berechnete Korrektur (PWM-Wert) für den linken Motor.
 * @param correction_R Die vom PD-Regler berechnete Korrektur (PWM-Wert) für den rechten Motor.
 * @param current_pos_L Die aktuelle vorzeichenbehaftete Position des linken Rades in [mm].
 * @param current_pos_R Die aktuelle vorzeichenbehaftete Position des rechten Rades in [mm].
 * @param is_stable_condition_met Flag, das anzeigt, ob die Stabilitätsbedingung im aktuellen Zyklus erfüllt ist (1) oder nicht (0).
 */
void LogPDEntry(float error_L, float error_R, float correction_L, float correction_R, float current_pos_L, float current_pos_R, int is_stable_condition_met);

/**
 * @brief Gibt den Inhalt des PD-Regler-Log-Puffers über die Debug-Kommunikationsschnittstelle aus.
 *
 * Diese Funktion formatiert alle im 'pd_log_buffer' gespeicherten Log-Einträge in lesbare
 * Strings und sendet sie in Blöcken über die 'debug_comms'-Schnittstelle (UART).
 * Die Ausgabe erfolgt als Ringpuffer.
 */
void DumpPDLog(void);

/**
 * @brief Leert den internen PD-Regler-Log-Puffer und setzt die zugehörigen Zähler zurück.
 *
 * Diese Funktion wird typischerweise nach dem Ausgeben des Logs ('DumpPDLog()') oder
 * vor dem Start einer neuen Bewegungssequenz aufgerufen, um den Log-Puffer
 * für neue Einträge vorzubereiten und sicherzustellen, dass keine alten Daten verbleiben.
 */
void ClearPDLog(void);

#endif /* FUNKTIONSSCHNITTSTELLEN_PD_CONTROLLER_H_ */
