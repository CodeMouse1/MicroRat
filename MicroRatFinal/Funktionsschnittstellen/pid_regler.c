
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Dave.h"
#include "Funktionsschnittstellen/pid_regler.h"
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

void ControllerHandler(){
	UpdatePID();
}

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
}

void setPIDGoalD(float distance_R, float distance_L) {
	distanceGoal_L = distance_R;
	distanceGoal_R = distance_L;
}

int PIDdone() {
	static int stableCycleCount = 0;
	float current_pos_L_mm = GetEncoderLeft_mm();
	float current_pos_R_mm = GetEncoderRight_mm();
	// Fehlerberechnung für jedes Rad
	float error_L = distanceGoal_L - current_pos_L_mm;
	float error_R = distanceGoal_R - current_pos_R_mm;
	float gleichlauferror_mm = current_pos_R_mm - current_pos_L_mm;
	if (fabsf(error_L) < 5.0f && fabsf(error_R) < 5.0f && fabsf(gleichlauferror_mm) < 2.0f) {
		stableCycleCount++;
		if (stableCycleCount >= CYCLES_THRESHOLD){
			StopAndSignal();
			return 1;
		}
	} else if (error_L < -5.0f && error_R < -5.0){
	//---FUNKTION WENN ER ÜBERCHIESST UM NICHT DURCHZUDREHEN-------------------------
		MotorsStop();
		while (1) {
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_AUGE_1);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_AUGE_2);
	    for (volatile int i = 0; i < 500000; i++) {}
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_AUGE_1);
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_AUGE_2);
	    for (volatile int i = 0; i < 500000; i++) {}
	}


	} else {
		stableCycleCount = 0;
	}

	return 0;
}

void ResetPID(){
	for (volatile int i = 0; i < 2000000; i++) {}

	isTurning = 0;

	EncoderReset();

	distanceGoal_R = 0.0f;
	distanceGoal_L = 0.0f;
	integral_error_R = 0.0f;
	integral_error_L = 0.0f;
	last_error_R = 0.0f;
	last_error_L = 0.0f;
}

