/**
 * @file pd_controller.c
 * @brief Implementierung des PD-Reglers für die präzise Bewegung der MicroRat.
 *
 * Dieses Modul verwaltet die Kernlogik des Proportional-Differential-Reglers (PD-Regler)
 * zur Steuerung der Motoren der MicroRat. Es liest Encoder-Werte, berechnet Fehler
 * relativ zu einem Sollwert, und generiert PWM-Signale für die Motoren.
 * Zusätzlich wird ein Gleichlaufregler implementiert, um Abweichungen zwischen
 * den beiden Rädern zu korrigieren. Das Modul umfasst auch eine grundlegende
 * Logging-Funktionalität zur Analyse des Reglerverhaltens, die über das
 * 'debug_comms'-Modul ausgegeben wird.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 *
 * @dependencies
 * - <stdio.h>: Für Standard-E/A-Funktionen
 * - <stdlib.h>: Für Standardbibliotheksfunktionen
 * - <math.h>: Für mathematische Funktionen wie 'fabsf()' und 'copysignf()'
 * - <string.h>: Für String-Manipulationsfunktionen wie 'strlen()'
 * - "Funktionsschnittstellen/pd_controller.h": Enthält die Deklarationen der öffentlichen Schnittstellen und Strukturen dieses Moduls
 * - "Funktionsschnittstellen/movement.h": Für externe Variablen wie 'isTurning'
 * - "Funktionsschnittstellen/debug_comms.h": Für die Debug-Kommunikationsschnittstelle, insbesondere `Debug_Comms_SendBuffer()` zum Senden von Log-Daten über UART
 * - "Funktionsschnittstellen/sensors.h": Für das Auslesen der Encoder in mm
 * - "Funktionsschnittstellen/system_interface.h": Für plattformspezifische Funktionen wie `Delay_ms()`
 * - "Hardwaresteuerung/hal_motor.h": Für die Hardware-Abstraktionsschicht der Motoren
 * - "Hardwaresteuerung/hal_encoder.h": Für die Hardware-Abstraktionsschicht der Encoder
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "Funktionsschnittstellen/pd_controller.h"
#include "Funktionsschnittstellen/movement.h"
#include "Funktionsschnittstellen/debug_comms.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Funktionsschnittstellen/system_interface.h"
#include "Hardwaresteuerung/hal_motor.h"
#include "Hardwaresteuerung/hal_encoder.h"

volatile float distanceGoal_R = 0.0f;	///- Sollwert für die zu fahrende Distanz des rechten Rades in Millimetern
volatile float distanceGoal_L = 0.0f;	///- Sollwert für die zu fahrende Distanz des linken Rades in Millimetern
volatile float last_error_L = 0.0f;		///- Letzter Fehlerwert des linken Rades für die Berechnung des D-Anteils in Millimetern
volatile float last_error_R = 0.0f;		///- Letzter Fehlerwert des rechten Rades für die Berechnung des D-Anteils in Millimetern

volatile float signed_current_pos_L = 0.0f;	///- aktuell akkumulierte, vorzeichenbehaftete Position des linken Rades in Millimetern
volatile float signed_current_pos_R = 0.0f;	///- aktuell akkumulierte, vorzeichenbehaftete Position des rechten Rades in Millimetern
volatile float last_abs_encoder_L = 0.0f;	///- letzte absolute Encoder-Wert des linken Rades in Millimetern
volatile float last_abs_encoder_R = 0.0f;	///- letzte absolute Encoder-Wert des rechten Rades in Millimetern
volatile float last_pwmL_calculated = 0.0f;	///- letzte berechnete und angewendete PWM-Wert für den linken Motor
volatile float last_pwmR_calculated = 0.0f;	///- letzte berechnete und angewendete PWM-Wert für den rechten Motor

// --- Interne Variablen für die Logging-Funktionalität ---
uint8_t UART_PD[500];
static PdLogEntry pd_log_buffer[PD_LOG_BUFFER_SIZE];
static volatile int pd_log_index = 0;
static volatile int pd_log_count = 0;
static volatile int pd_update_counter = 0;

/**
 * @brief Haupt-Handler für den PD-Regler-Update-Zyklus.
 *
 * Diese Funktion dient typischerweise als Callback für einen Hardware-Timer-Interrupt.
 * Sie stellt sicher, dass die 'UpdatePD()'-Funktion, die die Kernlogik des Reglers enthält,
 * in festen, regelmäßigen Zeitintervallen von 1ms aufgerufen wird.
 */
#include "Dave.h"

void ControllerHandler(){
	UpdatePD();
}
/**
 * @brief Aktualisiert den Zustand des PD-Reglers und steuert die Motoren.
 *
 * Diese Funktion ist das Herzstück des PD-Reglers und wird periodisch aufgerufen
 * . Sie implementiert die gesamte Regellogik in folgenden Schritten:
 * 1.  **Parameter-Auswahl:** Wählt die passenden KP- und KD-Parameter basierend auf dem aktuellen Bewegungsmodus.
 * 2.  **Encoder-Datenabruf:** Ruft die aktuellen absoluten Encoder-Werte für beide Räder ab in mm.
 * 3.  **Delta-Berechnung & Vorzeichenbehaftung:** Berechnet die Bewegungsdistanz seit dem letzten Update ('delta_L', 'delta_R')
 *       und aktualisiert die vorzeichenbehafteten Gesamtpositionen ('signed_current_pos_L', 'signed_current_pos_R')
 *       basierend auf der aktuellen Motorrichtung ('currentMotorDirectionL/R').
 * 4.  **Fehlerberechnung:** Berechnet den Positionsfehler für jedes Rad als Differenz zwischen Soll- und Ist-Position.
 * 5.  **PD-Anteile:** Berechnet die Proportional- und Derivativanteile der Korrektur für jedes Rad.
 * 6.  **Gleichlaufkorrektur:** Berechnet einen Gleichlauffehler zwischen den Rädern und wendet eine Korrektur an,
 *       um Abweichungen (Schieflage bei Geradeausfahrt, ungleichmäßige Drehung) auszugleichen. Die Logik unterscheidet sich für Geradeausfahrt und Drehbewegung.
 * 7.  **PWM-Begrenzung:** Begrenzt die berechneten PWM-Werte auf den maximal zulässigen Bereich.
 * 8.  **Motorsteuerung:** Setzt die Motor-Geschwindigkeiten mit den finalen, angepassten PWM-Werten über die HAL-Schnittstelle.
 * 9.  **Zustandsaktualisierung:** Speichert die aktuellen Fehler und absoluten Encoder-Werte für die Berechnung des D-Anteils und der Deltas in der nächsten Iteration.
 * 10. **Optionales Logging:** Führt intervallbasiertes Logging der Reglerdaten durch, um das Verhalten analysieren zu können.
 */
void UpdatePD() {
	float kp_current, kd_current;
	if (isTurning) {
		kp_current = KP_TURN;
		kd_current = KD_TURN;
	} else {
		kp_current = KP_STRAIGHT;
		kd_current = KD_STRAIGHT;
	}
	// 1. Encoder-Werte in Millimeter abrufen
	float raw_current_pos_L_mm = GetEncoderLeft_mm();
	float raw_current_pos_R_mm = GetEncoderRight_mm();
	// 2. Delta (Veränderung seit der letzten PID-Iteration) berechnen
	// Hier muss delta_L/R das korrekte Vorzeichen bekommen, da raw_current_pos_X_mm immer positiv ist.
	float delta_L = raw_current_pos_L_mm - last_abs_encoder_L;
	float delta_R = raw_current_pos_R_mm - last_abs_encoder_R;

	if (currentMotorDirectionL == MOTOR_BACKWARD) {
		signed_current_pos_L -= delta_L;
	} else if (currentMotorDirectionL == MOTOR_FORWARD) {
		signed_current_pos_L += delta_L;
	}
	if (currentMotorDirectionR == MOTOR_BACKWARD) {
		signed_current_pos_R -= delta_R;
	} else if (currentMotorDirectionR == MOTOR_FORWARD) {
		signed_current_pos_R += delta_R;
	}

	// Fehlerberechnung für jedes Rad
	float error_L = distanceGoal_L - signed_current_pos_L;
	float error_R = distanceGoal_R - signed_current_pos_R;

	// P, D Anteile
	float proportionalCorrection_L = kp_current * error_L;
	float derivativeCorrection_L = kd_current * (error_L - last_error_L);

	float proportionalCorrection_R = kp_current * error_R;
	float derivativeCorrection_R = kd_current * (error_R - last_error_R);

	float gleichlaufKorrektur = 0.0f;
	if (!isTurning) {
		float gleichlaufError = signed_current_pos_R - signed_current_pos_L;
		gleichlaufKorrektur = KP_GLEICHLAUF * gleichlaufError;

	} else {

		float drehGleichlaufError = fabsf(signed_current_pos_R) - fabsf(signed_current_pos_L);

		gleichlaufKorrektur = KP_GLEICHLAUF * drehGleichlaufError;
	}

	float pwmL = proportionalCorrection_L + derivativeCorrection_L;
	float pwmR = proportionalCorrection_R + derivativeCorrection_R;

	// PWM-Werte begrenzen und sicherstellen, dass sie positiv sind fuer MotorsSetSpeed
	pwmL = (pwmL > PWM_MAX) ? PWM_MAX : pwmL;
	pwmR = (pwmR > PWM_MAX) ? PWM_MAX : pwmR;
	pwmL = (pwmL < -PWM_MAX) ? -PWM_MAX : pwmL;
	pwmR = (pwmR < -PWM_MAX) ? -PWM_MAX : pwmR;

	float pwm_L, pwm_R;
	if (!isTurning) {
		// Anwendung für Geradeausfahrt: linkes Rad schneller, rechtes Rad langsamer bei positivem Fehler (R>L)
		pwm_L = pwmL + gleichlaufKorrektur;
		pwm_R = pwmR - gleichlaufKorrektur;
	} else {
		pwm_R = pwmR - gleichlaufKorrektur * copysignf(1.0f, pwmR);
		pwm_L = pwmL + gleichlaufKorrektur * copysignf(1.0f, pwmL);
	}

	// Jetzt die PWM-Werte begrenzen und in int umwandeln
	int PWM_L = (int)(pwm_L);
	int PWM_R = (int)(pwm_R);

	MotorsSetSpeed(PWM_L, PWM_R); // Motoren mit den berechneten PWM-Werten ansteuern

	last_pwmL_calculated = PWM_L;
	last_pwmR_calculated = PWM_R;

	last_error_L = error_L;
	last_error_R = error_R;

	// Speichere den aktuellen ABSOLUTEN Encoder-Wert für die nächste Delta-Berechnung
	last_abs_encoder_L = raw_current_pos_L_mm;
	last_abs_encoder_R = raw_current_pos_R_mm;

	// Optionales Logging für Debugging und Analyse
	/*pid_update_counter++;
	if (pid_update_counter >= PID_LOG_INTERVAL) {
		pid_update_counter = 0; // Zähler zurücksetzen

		// Prüfe die Stabilitätsbedingung direkt hier
		uint8_t current_cycle_stable = 0;
		if (fabsf(error_L) < 2.0f && fabsf(error_R) < 2.0f) { // Dieselbe Bedingung wie in PIDdone()
			current_cycle_stable = 1;
		}

		// Rufe die neue Hilfsfunktion zum Loggen auf
		LogPIDEntry(error_L, error_R, pwm_Lf, pwm_Rf, signed_current_pos_L, signed_current_pos_R, current_cycle_stable);
	}*/
}

/**
 * @brief Setzt die Ziel-Distanzen für den PD-Regler.
 *
 * Diese Funktion wird von übergeordneten Modulen ('movement.c') aufgerufen,
 * um dem PD-Regler mitzuteilen, welche Strecke jedes Rad fahren soll.
 * Die Werte werden in Millimetern angegeben.
 *
 * @param distance_R Die Ziel-Distanz für das rechte Rad in Millimetern [mm].
 * @param distance_L Die Ziel-Distanz für das linke Rad in Millimetern [mm].
 */
void setPDGoalD(float distance_R, float distance_L) {
	distanceGoal_L = distance_L;
	distanceGoal_R = distance_R;
}

/**
 * @brief Überprüft, ob das PD-Regelziel erreicht und die MicroRat stabil ist.
 *
 * Diese Funktion wird verwendet, um das Ende einer Bewegungssequenz zu erkennen.
 * Sie prüft, ob die aktuellen Positionsfehler ('error_L', 'error_R') für eine
 * bestimmte Anzahl von aufeinanderfolgenden Zyklen ('CYCLES_THRESHOLD') innerhalb
 * eines vordefinierten Toleranzbereichs liegen.
 * Wenn die Stabilitätsbedingung erfüllt ist, wird 'StopAndSignal()' aufgerufen.
 *
 * @return '1', wenn das Ziel erreicht und die MicroRat stabil ist; '0' sonst.
 */
int PDdone() {
	static int stableCycleCount = 0;
	float current_pos_L_for_done = signed_current_pos_L;
	float current_pos_R_for_done = signed_current_pos_R;

	// Fehlerberechnung für jedes Rad unter Verwendung der vorzeichenbehafteten Positionen
	float error_L = distanceGoal_L - current_pos_L_for_done;
	float error_R = distanceGoal_R - current_pos_R_for_done;

	if (fabsf(error_L) <= 3.0f && fabsf(error_R) <= 3.0f) {
		stableCycleCount++;
		//LogPDEntry(error_L, error_R, last_pwmL_calculated, last_pwmR_calculated, signed_current_pos_L, signed_current_pos_R, 1);
		if (stableCycleCount >= CYCLES_THRESHOLD){
			StopAndSignal();
			return 1;
		}
	} else {
		stableCycleCount = 0;
	}

	return 0;
}

/**
 * @brief Setzt alle internen Zustandsvariablen des PD-Reglers zurück.
 *
 * Diese Funktion wird nach Abschluss einer Bewegungssequenz aufgerufen,
 * um den Regler für die nächste Bewegungsaufgabe vorzubereiten.
 * Alle Fehlerakkumulationen, Sollwerte, Encoder-Positionen und internen
 * Zustandsvariablen werden auf ihre Initialwerte (typischerweise Null) gesetzt.
 * Dies ist entscheidend, um "Restfehler" aus vorherigen Bewegungen zu eliminieren.
 */
void ResetPD(){
    //DumpPDLog();
	Delay_ms(500);
	isTurning = 0;

	EncoderReset();

	distanceGoal_R = 0.0f;
	distanceGoal_L = 0.0f;
	last_error_R = 0.0f;
	last_error_L = 0.0f;

	signed_current_pos_L = 0.0f;
	signed_current_pos_R = 0.0f;
	last_abs_encoder_L = 0.0f;
	last_abs_encoder_R = 0.0f;

	last_pwmL_calculated = 0.0f;
	last_pwmR_calculated = 0.0f;
    ClearPDLog();

}

/**
 * @brief Speichert einen einzelnen Eintrag der PD-Regler-Daten im internen Ringpuffer.
 *
 * Diese Hilfsfunktion wird von 'UpdatePD()' aufgerufen, um die Regler-Parameter
 * (Fehler, Korrekturen, aktuelle Positionen, Stabilitätsstatus) für das Debugging
 * und die nachträgliche Analyse zu protokollieren. Der Puffer funktioniert als Ringpuffer,
 * d.h., wenn er voll ist, werden die ältesten Einträge überschrieben.
 *
 * @param error_L Der aktuelle Fehler des linken Rades (Soll - Ist) in [mm].
 * @param error_R Der aktuelle Fehler des rechten Rades (Soll - Ist) in [mm].
 * @param correction_L Die vom PD-Regler berechnete Korrektur für den linken Motor (PWM-Wert).
 * @param correction_R Die vom PD-Regler berechnete Korrektur für den rechten Motor (PWM-Wert).
 * @param current_pos_L Die aktuelle vorzeichenbehaftete Position des linken Rades in [mm].
 * @param current_pos_R Die aktuelle vorzeichenbehaftete Position des rechten Rades in [mm].
 * @param is_stable_condition_met Flag, das anzeigt, ob die Stabilitätsbedingung im aktuellen Zyklus erfüllt ist (1) oder nicht (0).
 */
void LogPDEntry(float error_L, float error_R, float correction_L, float correction_R,
                 float current_pos_L, float current_pos_R, int is_stable_condition_met) {
    pd_log_buffer[pd_log_index].error_L = error_L;
    pd_log_buffer[pd_log_index].error_R = error_R;
    pd_log_buffer[pd_log_index].correction_L = correction_L;
    pd_log_buffer[pd_log_index].correction_R = correction_R;
    pd_log_buffer[pd_log_index].current_pos_L = current_pos_L;
    pd_log_buffer[pd_log_index].current_pos_R = current_pos_R;
    pd_log_buffer[pd_log_index].is_stable_condition_met = is_stable_condition_met;

    pd_log_index++;
    if (pd_log_index >= PD_LOG_BUFFER_SIZE) {
        pd_log_index = 0;
    }
    if (pd_log_count < PD_LOG_BUFFER_SIZE) {
        pd_log_count++;
    }
}

/**
 * @brief Gibt den Inhalt des PD-Regler-Log-Puffers über die Debug-Kommunikationsschnittstelle aus.
 *
 * Diese Funktion formatiert alle im 'pd_log_buffer' gespeicherten Log-Einträge
 * in lesbare Strings und sendet sie in Blöcken über die 'debug_comms'-Schnittstelle (UART).
 * Dies ist nützlich für das Post-Mortem-Debugging und die Analyse des Reglerverhaltens
 * nach Abschluss einer Bewegung oder bei Auftreten eines Problems. Die Ausgabe erfolgt als Ringpuffer.
 */
void DumpPDLog(void) {
    int offset = 0;
    offset += sprintf((char*)UART_PD + offset, "PD_LOG: Letzte %d Eintraege vor Reset:\n\r", pd_log_count);
    offset += sprintf((char*)UART_PD + offset, "Idx | ErrL | ErrR | CorrL | CorrR | PosL | PosR | S\n\r");
    offset += sprintf((char*)UART_PD + offset, "-----------------------------------------------------\n\r");

    int start_idx = (pd_log_count < PD_LOG_BUFFER_SIZE) ? 0 : pd_log_index;

    for (int i = 0; i < pd_log_count; i++) {
        int current_idx = (start_idx + i) % PD_LOG_BUFFER_SIZE;
        PdLogEntry *entry = &pd_log_buffer[current_idx];

        offset += sprintf((char*)UART_PD + offset, "%3d | %6.2f | %6.2f | %7.2f | %7.2f | %6.2f | %6.2f | %c\n\r",
            current_idx,
            entry->error_L,
            entry->error_R,
            entry->correction_L,
            entry->correction_R,
            entry->current_pos_L,
            entry->current_pos_R,
            entry->is_stable_condition_met ? '*' : ' '); // Markierung: '*' wenn stabil, ' ' sonst

        if (offset > (sizeof(UART_PD) - 100)) {
            Debug_Comms_SendBuffer(UART_PD, strlen((char*)UART_PD));
            offset = 0;
        }
    }

    if (offset > 0) {
        Debug_Comms_SendBuffer(UART_PD, strlen((char*)UART_PD));
    }
}

/**
 * @brief Leert den internen PD-Regler-Log-Puffer und setzt die zugehörigen Zähler zurück.
 *
 * Diese Funktion wird typischerweise nach dem Ausgeben des Logs ('DumpPDLog()')
 * oder vor dem Start einer neuen Bewegungssequenz aufgerufen, um den Log-Puffer
 * für neue Einträge vorzubereiten.
 */
void ClearPDLog(void) {
    pd_log_index = 0;
    pd_log_count = 0;
    pd_update_counter = 0;
}
