
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Dave.h"
#include "Funktionsschnittstellen/pd_regler.h"
#include "Funktionsschnittstellen/movement.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Funktionsschnittstellen/timer_utils.h"
#include "Hardwaresteuerung/hal_motor.h"
#include "Hardwaresteuerung/hal_encoder.h"

volatile float distanceGoal_R = 0.0f;
volatile float distanceGoal_L = 0.0f;
volatile float integral_error_R = 0.0f;
volatile float integral_error_L = 0.0f;
volatile float last_error_L = 0.0f;
volatile float last_error_R = 0.0f;

volatile float signed_current_pos_L = 0.0f;
volatile float signed_current_pos_R = 0.0f;
volatile float last_abs_encoder_L = 0.0f;
volatile float last_abs_encoder_R = 0.0f;
volatile float last_pwmL_calculated = 0.0f;
volatile float last_pwmR_calculated = 0.0f;

volatile float last_gleichlaufError = 0.0f;

//Hilfestellung
uint8_t UART_PID[500];
static PidLogEntry pid_log_buffer[PID_LOG_BUFFER_SIZE];
static volatile int pid_log_index = 0;
static volatile int pid_log_count = 0;
static volatile int pid_update_counter = 0;

void ControllerHandler(){
	UpdatePID();
}

void UpdatePID() {
	float kp_current, kd_current;
	if (isTurning) {
		kp_current = KP_TURN;
		kd_current = KD_TURN;
	} else {
		kp_current = KP_STRAIGHT;
		kd_current = KD_STRAIGHT;
	}

	// 1. Encoder-Werte in Millimeter abrufen (sie liefern ABSOLUTE Werte)
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
		last_gleichlaufError = gleichlaufError;

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

	if (fabsf(error_L) <= 5.0f && fabsf(error_R) <= 5.0f) {
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
	Delay_ms(500);
	isTurning = 0;

	EncoderReset();

	distanceGoal_R = 0.0f;
	distanceGoal_L = 0.0f;
	integral_error_R = 0.0f;
	integral_error_L = 0.0f;
	last_error_R = 0.0f;
	last_error_L = 0.0f;

	signed_current_pos_L = 0.0f;
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
