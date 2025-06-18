
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Dave.h"
#include "Funktionsschnittstellen/pd_regler.h"
#include "Funktionsschnittstellen/movement.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Hardwaresteuerung/hal_motor.h"
#include "Hardwaresteuerung/hal_encoder.h"

volatile float distanceGoal_R = 0.0f;
volatile float distanceGoal_L = 0.0f;
volatile float integral_error_R = 0.0f;
volatile float integral_error_L = 0.0f;
volatile float last_error_L = 0.0f;
volatile float last_error_R = 0.0f;

volatile float signed_current_pos_L = 0.0f; // Die interne, vorzeichenbehaftete Position für PID
volatile float signed_current_pos_R = 0.0f;
volatile float last_abs_encoder_L = 0.0f; // Speichert den letzten ABSOLUT positiven Encoder-Wert
volatile float last_abs_encoder_R = 0.0f;
volatile float last_pwmL_calculated = 0.0f;
volatile float last_pwmR_calculated = 0.0f;

volatile float last_gleichlaufError = 0.0f;

//Hilfestellung
uint8_t UART_PID[500]; // Globaler UART-Puffer
static PidLogEntry pid_log_buffer[PID_LOG_BUFFER_SIZE];
static volatile int pid_log_index = 0;
static volatile int pid_log_count = 0;
static volatile int pid_update_counter = 0; // Zähler für das Intervall

void ControllerHandler(){
	UpdatePID();
}

/*void UpdatePID() {
	float kp_current, ki_current , kd_current;
	if (isTurning) {
		kp_current = KP_TURN;
		ki_current = KI_TURN;
		kd_current = KD_TURN;
	} else {
		kp_current = KP_STRAIGHT;
		ki_current = KI_STRAIGHT;
		kd_current = KD_STRAIGHT;
	}
	// Encoder-Werte in Millimeter abrufen
	float current_pos_L_mm = GetEncoderLeft_mm();
	float current_pos_R_mm = GetEncoderRight_mm();

	// Fehlerberechnung für jedes Rad
	float error_L = distanceGoal_L - current_pos_L_mm;
	float error_R = distanceGoal_R - current_pos_R_mm;

	// Integralanteile aktualisieren
	integral_error_L += error_L;
	integral_error_R += error_R;

	// Integralbegrenzung
	if (integral_error_L > INTEGRAL_LIMIT) integral_error_L = INTEGRAL_LIMIT;
	if (integral_error_L < -INTEGRAL_LIMIT) integral_error_L = -INTEGRAL_LIMIT;
	if (integral_error_R > INTEGRAL_LIMIT) integral_error_R = INTEGRAL_LIMIT;
	if (integral_error_R < -INTEGRAL_LIMIT) integral_error_R = -INTEGRAL_LIMIT;

	// P, I, D Anteile
	float proportionalCorrection_L = kp_current * error_L;
	float integralCorrection_L = ki_current * integral_error_L;
	float derivativeCorrection_L = kd_current * (error_L - last_error_L);

	float proportionalCorrection_R = kp_current * error_R;
	float integralCorrection_R = ki_current * integral_error_R;
	float derivativeCorrection_R = kd_current * (error_R - last_error_R);

	float currentEncoderDifference = current_pos_R_mm - current_pos_L_mm;
	float gleichlaufError = currentEncoderDifference;
	float gleichlaufKorrektur = KP_GLEICHLAUF * gleichlaufError;

	// Gesamte PWM-Werte
	int pwmL = (int)(proportionalCorrection_L + integralCorrection_L + derivativeCorrection_L + gleichlaufKorrektur);
	int pwmR = (int)(proportionalCorrection_R + integralCorrection_R + derivativeCorrection_R - gleichlaufKorrektur);

	// PWM-Werte begrenzen und sicherstellen, dass sie positiv sind fuer MotorsSetSpeed
	pwmL = (pwmL > PWM_MAX) ? PWM_MAX : pwmL;
	pwmR = (pwmR > PWM_MAX) ? PWM_MAX : pwmR;
	pwmL = (pwmL < -PWM_MAX) ? -PWM_MAX : pwmL;
	pwmR = (pwmR < -PWM_MAX) ? -PWM_MAX : pwmR;

	MotorsSetSpeed(abs(pwmL), abs(pwmR)); // Bleibt unverändert. Deine MotorsSetX() Funktionen müssen die Richtung setzen.

	last_error_L = error_L;
	last_error_R = error_R;
}*/

void UpdatePID() {
	float kp_current, ki_current , kd_current;
	if (isTurning) {
		kp_current = KP_TURN;
		ki_current = KI_TURN;
		kd_current = KD_TURN;
	} else {
		kp_current = KP_STRAIGHT;
		ki_current = KI_STRAIGHT;
		kd_current = KD_STRAIGHT;
	}

	// 1. Encoder-Werte in Millimeter abrufen (sie liefern ABSOLUTE Werte)
	float raw_current_pos_L_mm = GetEncoderLeft_mm();
	float raw_current_pos_R_mm = GetEncoderRight_mm();

	// 2. Delta (Veränderung seit der letzten PID-Iteration) berechnen
	// Hier muss delta_L/R das korrekte Vorzeichen bekommen, da raw_current_pos_X_mm immer positiv ist.
	float delta_L = raw_current_pos_L_mm - last_abs_encoder_L;
	float delta_R = raw_current_pos_R_mm - last_abs_encoder_R;

    if (distanceGoal_L < 0) { // Wenn das Ziel rückwärts ist
        signed_current_pos_L -= delta_L; // Subtrahiere das positive Delta, um signed_current_pos_L negativ zu machen
    } else { // Wenn das Ziel vorwärts ist (oder 0)
        signed_current_pos_L += delta_L; // Addiere das positive Delta
    }

    if (distanceGoal_R < 0) { // Wenn das Ziel rückwärts ist
        signed_current_pos_R -= delta_R;
    } else { // Wenn das Ziel vorwärts ist (oder 0)
        signed_current_pos_R += delta_R;
    }

	// Fehlerberechnung für jedes Rad
	float error_L = distanceGoal_L - signed_current_pos_L;
	float error_R = distanceGoal_R - signed_current_pos_R;

	// Integralanteile aktualisieren
	integral_error_L += error_L;
	integral_error_R += error_R;

	// Integralbegrenzung
	if (integral_error_L > INTEGRAL_LIMIT) integral_error_L = INTEGRAL_LIMIT;
	if (integral_error_L < -INTEGRAL_LIMIT) integral_error_L = -INTEGRAL_LIMIT;
	if (integral_error_R > INTEGRAL_LIMIT) integral_error_R = INTEGRAL_LIMIT;
	if (integral_error_R < -INTEGRAL_LIMIT) integral_error_R = -INTEGRAL_LIMIT;

	// P, I, D Anteile
	float proportionalCorrection_L = kp_current * error_L;
	float integralCorrection_L = ki_current * integral_error_L;
	float derivativeCorrection_L = kd_current * (error_L - last_error_L);

	float proportionalCorrection_R = kp_current * error_R;
	float integralCorrection_R = ki_current * integral_error_R;
	float derivativeCorrection_R = kd_current * (error_R - last_error_R);

	//TEST
	float gleichlaufKorrektur = 0.0f; // Dies ist die Korrektur, die auf die PWMs angewendet wird
	if (!isTurning) {
		// Logik für Geradeausfahrt (wie bisher)
		float gleichlaufError = signed_current_pos_R - signed_current_pos_L; // Vorzeichenbehafteter Fehler

		float p_gleichlauf = KP_GLEICHLAUF * gleichlaufError;
		float d_gleichlauf = KD_GLEICHLAUF * (gleichlaufError - last_gleichlaufError);

		gleichlaufKorrektur = p_gleichlauf + d_gleichlauf;
		last_gleichlaufError = gleichlaufError;

	} else { // Wir sind in einer Drehung (isTurning == true)
		// NEU: Nur P-Regler für Dreh-Gleichlauf
		// Fehler ist die Differenz der ABSOLUTEN Wege
		float drehGleichlaufError = fabsf(signed_current_pos_R) - fabsf(signed_current_pos_L);

		float p_dreh_gleichlauf = 750 * drehGleichlaufError;
		gleichlaufKorrektur = p_dreh_gleichlauf; // Nur P-Anteil
	}


	/*float gleichlaufError = signed_current_pos_R - signed_current_pos_L;

	float gleichlaufKorrektur = 0.0f;
	if (!isTurning) {
		float p_gleichlauf = KP_GLEICHLAUF * gleichlaufError;
		float d_gleichlauf = KD_GLEICHLAUF * (gleichlaufError - last_gleichlaufError);

		gleichlaufKorrektur = p_gleichlauf + d_gleichlauf;
	}*/

	float pwmL = proportionalCorrection_L + integralCorrection_L + derivativeCorrection_L;
	float pwmR = proportionalCorrection_R + integralCorrection_R + derivativeCorrection_R;

	// ----- ANWENDUNG DER RICHTUNGSABHÄNGIGEN KALIBRIERUNGSFAKTOREN -----
	if (distanceGoal_L >= 0) { // Ziel ist vorwärts
		pwmL *= PWM_L_FORWARD_FACTOR;
	} else { // Ziel ist rückwärts
		pwmL *= PWM_L_BACKWARD_FACTOR;
	}
	if (distanceGoal_R >= 0) { // Ziel ist vorwärts
		pwmR *= PWM_R_FORWARD_FACTOR;
	} else { // Ziel ist rückwärts
		pwmR *= PWM_R_BACKWARD_FACTOR;
	}
	// ----- ENDE ANWENDUNG RICHTUNGSABHÄNGIGER FAKTOREN -----

	// PWM-Werte begrenzen und sicherstellen, dass sie positiv sind fuer MotorsSetSpeed
	pwmL = (pwmL > PWM_MAX) ? PWM_MAX : pwmL;
	pwmR = (pwmR > PWM_MAX) ? PWM_MAX : pwmR;
	pwmL = (pwmL < -PWM_MAX) ? -PWM_MAX : pwmL;
	pwmR = (pwmR < -PWM_MAX) ? -PWM_MAX : pwmR;

	/*float pwm_L = pwmL + gleichlaufKorrektur;
	float pwm_R = pwmR - gleichlaufKorrektur;
	*/
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
	int pwm_Lf = (int)(pwm_L);
	int pwm_Rf = (int)(pwm_R);

	MotorsSetSpeed(pwm_Lf, pwm_Rf); // Hier werden die korrekten, signierten PWMs übergeben

	//Nur für LOGGEN
	last_pwmL_calculated = pwm_Lf;
	last_pwmR_calculated = pwm_Rf;

	last_error_L = error_L;
	last_error_R = error_R;

	// Speichere den aktuellen ABSOLUTEN Encoder-Wert für die nächste Delta-Berechnung
	last_abs_encoder_L = raw_current_pos_L_mm;
	last_abs_encoder_R = raw_current_pos_R_mm;

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

void setPIDGoalD(float distance_R, float distance_L) {
	distanceGoal_L = distance_L;
	distanceGoal_R = distance_R;
}

int PIDdone() {
	static int stableCycleCount = 0;
	float current_pos_L_for_done = signed_current_pos_L;
	float current_pos_R_for_done = signed_current_pos_R;

	// Fehlerberechnung für jedes Rad unter Verwendung der vorzeichenbehafteten Positionen
	float error_L = distanceGoal_L - current_pos_L_for_done;
	float error_R = distanceGoal_R - current_pos_R_for_done;

	// Gleichlauffehler für die PIDdone-Bedingung (ebenfalls mit vorzeichenbehafteter Position)
	//float gleichlauferror_mm = current_pos_R_for_done - current_pos_L_for_done;

	if (fabsf(error_L) < 2.0f && fabsf(error_R) < 2.0f) {
		stableCycleCount++;
		//LogPIDEntry(error_L, error_R, last_pwmL_calculated, last_pwmR_calculated, signed_current_pos_L, signed_current_pos_R, 1);
		if (stableCycleCount >= CYCLES_THRESHOLD){
			StopAndSignal();
			return 1;
		}
	} else {
		stableCycleCount = 0;
	}

	return 0;
}

void ResetPID(){
    //DumpPIDLog();
	for (volatile int i = 0; i < 1000000; i++) {}
	isTurning = 0;

	EncoderReset();

	distanceGoal_R = 0.0f;
	distanceGoal_L = 0.0f;
	integral_error_R = 0.0f;
	integral_error_L = 0.0f;
	last_error_R = 0.0f;
	last_error_L = 0.0f;

	signed_current_pos_L = 0.0f; // Wichtig: Die interne PID-Position auf 0 setzen
	signed_current_pos_R = 0.0f;
	last_abs_encoder_L = 0.0f;
	last_abs_encoder_R = 0.0f;

	last_pwmL_calculated = 0.0f;
	last_pwmR_calculated = 0.0f;
	last_gleichlaufError = 0.0f;
    ClearPIDLog();

}

void LogPIDEntry(float error_L, float error_R, float correction_L, float correction_R,
                 float current_pos_L, float current_pos_R, int is_stable_condition_met) {
    pid_log_buffer[pid_log_index].error_L = error_L;
    pid_log_buffer[pid_log_index].error_R = error_R;
    pid_log_buffer[pid_log_index].correction_L = correction_L;
    pid_log_buffer[pid_log_index].correction_R = correction_R;
    pid_log_buffer[pid_log_index].current_pos_L = current_pos_L;
    pid_log_buffer[pid_log_index].current_pos_R = current_pos_R;
    pid_log_buffer[pid_log_index].is_stable_condition_met = is_stable_condition_met;

    pid_log_index++;
    if (pid_log_index >= PID_LOG_BUFFER_SIZE) {
        pid_log_index = 0; // Ringpuffer-Verhalten
    }
    if (pid_log_count < PID_LOG_BUFFER_SIZE) {
        pid_log_count++; // Zähler erhöhen, bis der Puffer voll ist
    }
}

void DumpPIDLog(void) {
    int offset = 0;
    offset += sprintf((char*)UART_PID + offset, "PID_LOG: Letzte %d Eintraege vor Reset:\n\r", pid_log_count);
    offset += sprintf((char*)UART_PID + offset, "Idx | ErrL | ErrR | CorrL | CorrR | PosL | PosR | S\n\r");
    offset += sprintf((char*)UART_PID + offset, "-----------------------------------------------------\n\r");

    int start_idx = (pid_log_count < PID_LOG_BUFFER_SIZE) ? 0 : pid_log_index;

    for (int i = 0; i < pid_log_count; i++) {
        int current_idx = (start_idx + i) % PID_LOG_BUFFER_SIZE;
        PidLogEntry *entry = &pid_log_buffer[current_idx];

        offset += sprintf((char*)UART_PID + offset, "%3d | %6.2f | %6.2f | %7.2f | %7.2f | %6.2f | %6.2f | %c\n\r",
            current_idx,
            entry->error_L,
            entry->error_R,
            entry->correction_L,
            entry->correction_R,
            entry->current_pos_L,
            entry->current_pos_R,
            entry->is_stable_condition_met ? '*' : ' '); // Markierung: '*' wenn stabil, ' ' sonst

        if (offset > (sizeof(UART_PID) - 100)) {
            UART_Transmit(&UART_COM, UART_PID, strlen((char*)UART_PID));
            offset = 0;
        }
    }

    if (offset > 0) {
        UART_Transmit(&UART_COM, UART_PID, strlen((char*)UART_PID));
    }
}

void ClearPIDLog(void) {
    pid_log_index = 0;
    pid_log_count = 0;
    pid_update_counter = 0;
    // Der Inhalt des Puffers muss nicht explizit gelöscht werden, er wird überschrieben.
}
