#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Dave.h"
#include "Funktionsschnittstellen/movement.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Funktionsschnittstellen/pd_regler.h"
#include "Hardwaresteuerung/hal_motor.h"
#include "Hardwaresteuerung/hal_ir.h"
#include "Hardwaresteuerung/hal_encoder.h"

float KP_STRAIGHT = 23.0;

volatile bool isTurning = false;
volatile bool hasRecalibrated = false;

void MovementInit(){
	MotorsSetForward();
}

void MoveOneCell(){
	EncoderReset();
	isTurning = false;

	float distanceToDrive = EstimateCellSize();

	setPIDGoalD(distanceToDrive,distanceToDrive);

	TIMER_Start(&TIMER_REGLER);

	while (!PIDdone()) {}
	ResetPID();
	hasRecalibrated = false;
}

float EstimateCellSize() {
    float measuredDistance = GetDistanceFront_mm();

    if (measuredDistance > MAX_MEASUREMENT_DISTANCE) {
    	measuredDistance = MAX_MEASUREMENT_DISTANCE;
    }

    float usableDistance = measuredDistance - MIN_WALL_DISTANCE;

    if (measuredDistance <= (MM_PER_CELL_REFERENCE + 50.0f)) {

		return usableDistance;
	}
    else {

		float numCellsAhead = roundf(usableDistance / MM_PER_CELL_REFERENCE);

		if (numCellsAhead < 1.0f) {
			numCellsAhead = 1.0f;
		}

		float estimatedCellLength = usableDistance / numCellsAhead;

		return estimatedCellLength;
    }
}
// WIP
void RecalibrateAndMoveForward(){
	// --- Rückwärtsfahren für die Rekalibrierung ---
	//MotorsSetReverse();
	MotorsSetSpeed(-3000, -3000);
	for (volatile int i = 0; i < 3000000; i++){} // Einfache Zeitverzögerung für Rückwärtsfahrt
	MotorsSetSpeed(0, 0);
	EncoderReset(); // Encoder zurückgesetzt nach der Rückwärtsbewegung
	//MotorsSetForward();
	setPIDGoalD(40,40); // Ziel für eine Zelle vorwärts = 2cm
	KP_STRAIGHT = 100;
	TIMER_Start(&TIMER_REGLER);
	while (!PIDdone()) {} // Warte, bis die Vorwärtsbewegung abgeschlossen ist
	ResetPID(); // Setze PID für die nächste Bewegung zurück
	KP_STRAIGHT = 23.0;
}

void Stop(){
	MotorsSetSpeed(0, 0);
}

void Turn (TurnDirection direction){
	EncoderReset();
	float goal_distance_L = 0.0f;
	float goal_distance_R = 0.0f;
	float distance_90_deg = 70.0;//DISTANCE_PER_90_DEGREE_MM;
	if(direction == left){
		goal_distance_L = -distance_90_deg;
		goal_distance_R = distance_90_deg;
	}else if(direction == right){
		goal_distance_L = distance_90_deg;
		goal_distance_R = -distance_90_deg;
	}else if(direction == around){
		if (ReadLeft() < ReadRight()) {
			goal_distance_L = -distance_90_deg*1.85;
			goal_distance_R = distance_90_deg*1.85;
		} else if (ReadRight() < ReadLeft()) {
			goal_distance_L = distance_90_deg*1.85;
			goal_distance_R = -distance_90_deg*1.85;
		}
	}
	setPIDGoalD(goal_distance_R, goal_distance_L);
	isTurning = true;
	TIMER_Start(&TIMER_REGLER);
	while (!PIDdone()) {}
	ResetPID();
	if (direction == around) {
		// Rekalibrierung direkt nach einer 180-Grad-Drehung
		TIMER_Stop(&TIMER_REGLER); // Sicherstellen, dass der Timer gestoppt ist
		MotorsSetSpeed(0, 0);      // Sicherstellen, dass die Motoren stehen
		RecalibrateAndMoveForward();
	}
}








