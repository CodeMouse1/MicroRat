
#include "Dave.h"
#include "Funktionsschnittstellen/movement.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Funktionsschnittstellen/start_condition.h"
#include "Hardwaresteuerung/hal_motor.h"
#include "Hardwaresteuerung/hal_encoder.h"
#include <stdio.h>
#include <stdlib.h>

#define LOG_SIZE 100

typedef struct {
    int pwmL;
    int pwmR;
    int counter_L;
	int counter_R;
    float distanceError_L;
    float distanceError_R;
    float angleError;
} LogEntry;

LogEntry logBuffer[LOG_SIZE];
uint8_t UART_String[100];
uint8_t logIndex = 0;


#define PWM_MAX 3250//3500

#define TICKS_PER_CELL 290 // 16.25cm
#define ENCODER_DIFFERENCE_90_DEGREE 251

//#define ERROR_THRESHOLD 5  // Schwellenwert für den Fehler, unterhalb dem wir als "nahe genug an null" betrachten
#define CYCLES_THRESHOLD 50//1 // Anzahl der Zyklen, in denen der Fehler klein genug sein muss

#define KP_TURN 42//49.5//50//41.5
//#define KP_STRAIGHT 20.5//22//19.5
int KP_STRAIGHT = 22.75;//22;
#define KP_ANGLE 2500

volatile float distanceError_R = 0;
volatile float distanceError_L = 0;
volatile float angleError = 0;
volatile float distanceGoal = 0;
volatile float angleGoal = 0;


volatile int isTurning = 0;

#define KP_GLEICHLAUF_S 1500//2000// test 50 war für drehen
#define KP_GLEICHLAUF_T 50

void ControllerHandler(){
	UpdatePID();
}

//#define KP_ANGLE_TURN 20.85//20.5 war gut aber noch zu schwach

#define KP_ANGLE_TURN_BASE 18.5//20//.35
#define KP_ANGLE_TURN_BOOST 500
#define MIN_DREHGESCHWINDIGKEIT 0
#define BOOST_VERZOEGERUNG_ZYKLEN 500
int turnStartCounter = 0;



void UpdatePID() {
	int counter_R = EncoderReadRight();
	int counter_L = EncoderReadLeft();
	int angleError = 0;
	static int lastAngleError_Turn = 0;
	//TEST DYNAMIC
	static int last_counter_R = 0;
	static int last_counter_L = 0;
	float dynamicKP_Turn = KP_ANGLE_TURN_BASE;

	distanceError_R = distanceGoal - counter_R;
	distanceError_L = distanceGoal - counter_L;

	int pwmL = 0; // Initialisierung
	int pwmR = 0; // Initialisierung

	if (isTurning) {
		// PID-Regelung für die Drehung
		int currentAngleDifference = counter_R + counter_L;
		angleError = angleGoal - currentAngleDifference;

		if (turnStartCounter < BOOST_VERZOEGERUNG_ZYKLEN) {
			turnStartCounter++;
		} else {
			// Boost-Logik nach der Verzögerung
			int aktuelleDrehgeschwindigkeit = abs((counter_R - last_counter_R)) + abs((counter_L - last_counter_L));
			if (aktuelleDrehgeschwindigkeit <= MIN_DREHGESCHWINDIGKEIT && angleError >= 1) {
				dynamicKP_Turn = KP_ANGLE_TURN_BOOST;
			}
		}
		float kd_turn = 1.5;
		float derivativeCorrection = kd_turn * (angleError - lastAngleError_Turn);
		float turnCorrection = dynamicKP_Turn * angleError + derivativeCorrection;

		// Gleichlaufkorrektur (bleibt wie gehabt)
		int encoderDifference = counter_R - counter_L;
		float gleichlaufKorrektur = KP_GLEICHLAUF_T * encoderDifference;

		// Anwenden der Korrektur auf die PWM-Werte
		pwmL = abs((int)(turnCorrection + gleichlaufKorrektur));
		pwmR = abs((int)(turnCorrection - gleichlaufKorrektur));

		lastAngleError_Turn = angleError;
		last_counter_R = counter_R;
		last_counter_L = counter_L;

	}else {
		// PID-Regelung für die Geradeausfahrt MIT Gleichlauf
		float distanceCorrection_R = KP_STRAIGHT * distanceError_R;
		float distanceCorrection_L = KP_STRAIGHT * distanceError_L;

		int encoderDifferenceStraight = counter_R - counter_L;
		float gleichlaufKorrekturStraight = KP_GLEICHLAUF_S * encoderDifferenceStraight;

		pwmL = abs((int)(distanceCorrection_L + gleichlaufKorrekturStraight));
		pwmR = abs((int)(distanceCorrection_R - gleichlaufKorrekturStraight));
	}

	pwmL = (pwmL > PWM_MAX) ? PWM_MAX : pwmL;
	pwmR = (pwmR > PWM_MAX) ? PWM_MAX : pwmR;

	MotorsSetSpeed(pwmL, pwmR);
}

void ResetPID(){
	MotorsSetSpeed(0, 0);

	for (volatile int i = 0; i < 2000000; i++) {}

	isTurning = 0;

	EncoderReset();


	distanceError_R = 0;
	distanceError_L = 0;
	angleError = 0;
	distanceGoal = 0;
	angleGoal = 0;
	turnStartCounter = 0;

}

void setPIDGoalD(int distance) {
    distanceGoal = distance;
}

void setPIDGoalA(int angle) {
    angleGoal = angle;
}

int PIDdone() {
    static int stableCycleCount = 0;

    // Fehler berechnen
    int counter_R = EncoderReadRight();
    int counter_L = EncoderReadLeft();
    distanceError_R = distanceGoal - counter_R;
    distanceError_L = distanceGoal - counter_L;
    int gleichlauferror = counter_R - counter_L;

    if (abs(distanceError_L) < 5 && abs(distanceError_R) < 5 && abs(gleichlauferror) < 5) {
        stableCycleCount++;
        if (stableCycleCount >= CYCLES_THRESHOLD){
        	TIMER_Stop(&TIMER_REGLER);
        	MotorsSetSpeed(0, 0);
        	DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_1);
			DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_2);
        	return 1;
		}
    } else{
			stableCycleCount = 0;
	}

    return 0;
}

volatile bool hasRecalibrated = false; // Neue Zustandsvariable

void move (int n){
	EncoderReset();
	MotorsSetForward();
	isTurning = 0;

	int goalDistance_base = n * TICKS_PER_CELL;
	int goalDistance_adjusted = goalDistance_base; // Standardwert

	int distanceToWall_cm = GetUltraschall(); // Entfernung in cm
	int schwellwert_cm = 20;
	int sicherheitsabstand_cm = 2; // Gewünschter minimaler Abstand zur Wand in cm

	if(distanceToWall_cm <= schwellwert_cm && distanceToWall_cm > 0) {
		// Berechne die Ziel-Distanz basierend auf dem Ultraschallwert und dem Sicherheitsabstand
		goalDistance_adjusted = (distanceToWall_cm - sicherheitsabstand_cm) * 17.9;//tick per cm laut rechnung
		// Stelle sicher, dass die Ziel-Distanz nicht negativ wird
		if (goalDistance_adjusted < 0) {
			goalDistance_adjusted = 0;
		}
		// Stelle sicher, dass die neue Ziel-Distanz nicht größer ist als die ursprüngliche Basis-Distanz
		if (goalDistance_adjusted > goalDistance_base) {
			goalDistance_adjusted = goalDistance_base;
		}
	}

	setPIDGoalD(goalDistance_adjusted);
	setPIDGoalA(0);
	TIMER_Start(&TIMER_REGLER);

	while (!PIDdone()) {
		/*distanceToWall_cm = GetUltraschall();
		// Rekalibrierung, wenn Wand >= 40cm entfernt UND noch nicht rekalibriert
		if (distanceToWall_cm >= 55 && !hasRecalibrated) {
			TIMER_Stop(&TIMER_REGLER);
			MotorsSetSpeed(0, 0);

			RecalibrateAndMoveForward(); // Führt die Rückwärtsbewegung aus

			hasRecalibrated = true; // Setze die Flagge, um zukünftige Rekalibrierungen in dieser move()-Ausführung zu verhindern

			return;
		}*/
		// Normale PID-Regelung, wenn nicht rekalibriert oder bereits rekalibriert wurde
	}
	ResetPID(); // Setzt alle PID-bezogenen Variablen zurück, NACHDEM die Bewegung abgeschlossen ist (oder vorzeitig beendet wurde)
	hasRecalibrated = false; // Setze die Flagge für die nächste move()-Ausführung zurück
}

void RecalibrateAndMoveForward(){
	// --- Rückwärtsfahren für die Rekalibrierung ---
	MotorsSetReverse();
	MotorsSetSpeed(3000, 3000);
	for (volatile int i = 0; i < 5000000; i++){} // Einfache Zeitverzögerung für Rückwärtsfahrt
	MotorsSetSpeed(0, 0);
	DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_1);
	DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_2);
	EncoderReset(); // Encoder zurückgesetzt nach der Rückwärtsbewegung

	MotorsSetForward();
	setPIDGoalD(36); // Ziel für eine Zelle vorwärts = 2cm
	setPIDGoalA(0);
	KP_STRAIGHT = 100;
	TIMER_Start(&TIMER_REGLER);
	while (!PIDdone()) {} // Warte, bis die Vorwärtsbewegung abgeschlossen ist
	ResetPID(); // Setze PID für die nächste Bewegung zurück
	KP_STRAIGHT = 22.75;
}

void turn (TurnDirection direction){
	EncoderReset();
	int goalAngleError = ENCODER_DIFFERENCE_90_DEGREE;


	if(direction == left){
		MotorsSetLeft();
		setPIDGoalA(goalAngleError);
		setPIDGoalD(0);
	}else if(direction == right){
		MotorsSetRight();
		setPIDGoalA(goalAngleError);
		setPIDGoalD(0);
	}else if(direction == around){
		if (GetIRLeft() < GetIRRight()) {
			MotorsSetLeft();
			setPIDGoalA(goalAngleError*1.8);
			setPIDGoalD(0);
		} else if (GetIRRight() < GetIRLeft()) {
			MotorsSetRight();
			setPIDGoalA(goalAngleError*1.8);
			setPIDGoalD(0);
		}
	}

	/*setPIDGoalA(goalAngleError);
	setPIDGoalD(0);*/

	isTurning = 1;
	TIMER_Start(&TIMER_REGLER);
	while (!PIDdoneTurn()) {

	}

	ResetPID();
	if (direction == around) {
	        // Rekalibrierung direkt nach einer 180-Grad-Drehung
	        TIMER_Stop(&TIMER_REGLER); // Sicherstellen, dass der Timer gestoppt ist
	        MotorsSetSpeed(0, 0);      // Sicherstellen, dass die Motoren stehen
	        RecalibrateAndMoveForward();
	    }
}

void MovementInit(){
	MotorsSetForward();
}

int PIDdoneTurn() {
    int counter_R = EncoderReadRight();
    int counter_L = EncoderReadLeft();
    int currentAngleDifference = counter_R + counter_L;
    int error = abs(angleGoal - currentAngleDifference);

    static int stableCycles = 0;
    if (error <=1) { // Angepasster Fehler-Schwellenwert für den Winkel (experimentell anpassen!)
        stableCycles++;
        if (stableCycles >= CYCLES_THRESHOLD) {
            TIMER_Stop(&TIMER_REGLER);
        	MotorsSetSpeed(0, 0);
            DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_1);
			DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_2);
            return 1;
        }
    } else {
        stableCycles = 0;
    }
    return 0;
}




