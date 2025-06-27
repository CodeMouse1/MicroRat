/**
 * @file movement.c
 * @brief Implementierung der Bewegungsfunktionen für die MicroRat.
 *
 * Dieses Modul stellt Funktionen bereit, um die MicroRat um definierte Zellenabstände
 * vorwärts zu bewegen oder präzise Drehungen auszuführen. Es integriert den PD-Regler,
 * Encoder-Rückmeldung und Sensor-basierte Kalibrierungen.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 *
 * @dependencies
 * - <stdlib.h>: Für Standardbibliotheksfunktionen
 * - <math.h>: Für mathematische Funktionen wie roundf()
 * - Funktionsschnittstellen/movement.h: Deklarationen der öffentlichen Schnittstellen dieses Moduls
 * - Funktionsschnittstellen/sensors.h: Für Sensorlesungen
 * - Funktionsschnittstellen/pd_controller.h: Für die Steuerung des PD-Reglers
 * - Funktionsschnittstellen/system_interface.h: Für systemnahe Funktionen wie Delay_ms()
 * - Hardwaresteuerung/hal_motor.h: Für die direkte Motorsteuerung
 * - Hardwaresteuerung/hal_encoder.h: Für Encoder-Operationen
 * - Hardwaresteuerung/hal_ir.h: Für Infrarot-Sensor-spezifische Funktionen
 * - Hardwaresteuerung/hal_timer.h: Für Start/Stop Timer des Reglers
 */
#include <stdlib.h>
#include <math.h>
#include "Funktionsschnittstellen/movement.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Funktionsschnittstellen/system_interface.h"
#include "Funktionsschnittstellen/pd_controller.h"
#include "Hardwaresteuerung/hal_motor.h"
#include "Hardwaresteuerung/hal_ir.h"
#include "Hardwaresteuerung/hal_encoder.h"
#include "Hardwaresteuerung/hal_timer.h"

/**
 * @brief Standard-KP-Wert für den PD-Regler bei Geradeausfahrten.
 * Dieser Wert wird temporär bei der Rekalibrierung angepasst.
 */
float KP_STRAIGHT = 44.75;

// Flags für Drehbewegung und Kalibrierung
volatile bool isTurning = false;
volatile bool hasRecalibrated = false;

/*
 * @brief Bewegt die MicroRat genau eine Labyrinthzelle vorwärts.
 *
 * Diese Funktion setzt den Encoder zurück, berechnet die zu fahrende Distanz
 * (mittels EstimateCellSize()), setzt das PD-Ziel und wartet, bis die Bewegung
 * abgeschlossen ist. Danach wird der PD-Regler zurückgesetzt.
 */
void MoveOneCell(){
	EncoderReset();
	isTurning = false;
	float distanceToDrive = EstimateCellSize();
	setPDGoalD(distanceToDrive,distanceToDrive);
	PDTimer_Start();
	while (!PDdone()) {}
	ResetPD();
	hasRecalibrated = false;
}

/**
 * @brief Bewegt MicroRat eine spezifizierte Anzahl von Labyrinthzellen vorwärts.
 * @param numCells Die Anzahl der Zellen, die gefahren werden sollen.
 *
 * @note WIP: Diese Funktion ist eine neue Implementierung für den optimierten
 * Schnelllauf und erfordert noch sorgfältige Kalibrierung
 * von MM_PER_CELL_REFERENCE sowie umfassende Tests im Labyrinth.
 * Fehlerakkumulation über lange Strecken ist zu überwachen.
 */
void MoveMultipleCells(int numCells){
    if (numCells <= 0) {
        return;
    }
    EncoderReset();
    isTurning = false;
    float distanceToDrive = (float)numCells * MM_PER_CELL_REFERENCE;
    setPDGoalD(distanceToDrive, distanceToDrive);
	PDTimer_Start();
    while (!PDdone()) {}
    ResetPD();
    hasRecalibrated = false;
}

/*
 * @brief Schätzt die Länge einer Labyrinthzelle basierend auf Sensordaten.
 *
 * Diese Funktion nutzt den Frontsensor, um die Distanz zur nächsten Wand zu messen.
 * Basierend auf dieser Messung und einer Referenzzellengröße wird die aktuelle
 * Zellengröße geschätzt oder direkt die nutzbare Distanz zurückgegeben, wenn die Wand nah ist.
 *
 * @return Die geschätzte Länge einer Zelle in Millimetern.
 * @details Die Funktion behandelt Fälle, in denen die Wand sehr nah ist anders als Fälle, in denen mehrere Zellen
 * vor dem Roboter liegen. Sie subtrahiert `MIN_WALL_DISTANCE`, um die tatsächlich
 * fahrbare Distanz zu erhalten.
 */
float EstimateCellSize() {
    float measuredDistance = GetDistanceFront_mm();

    if (measuredDistance > MAX_MEASUREMENT_DISTANCE) {
    	measuredDistance = MAX_MEASUREMENT_DISTANCE;
    }

    float usableDistance = measuredDistance - MIN_WALL_DISTANCE;

    if (measuredDistance <= (MM_PER_CELL_REFERENCE + 80.0f)) {

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
/**
 * @brief Führt eine Rekalibrierung der MicroRat-Position durch und bewegt sie anschließend vorwärts.
 *
 * Diese Funktion fährt die MicroRat zuerst kurz rückwärts, um sie an eine bekannte Position
 * zu bringen und die Encoder zurückzusetzen. Danach fährt sie eine
 * feste kurze Distanz vorwärts mit einem temporär erhöhten KP-Wert,
 * um eine präzise Ausgangsposition für weitere Bewegungen zu gewährleisten.
 *
 */
void Recalibrate(){
	MotorsSetSpeed(-3250, -3250);
	Delay_ms(1000);
	MotorsSetSpeed(0, 0);
	EncoderReset();
	setPDGoalD(40,40);
	KP_STRAIGHT = 150;
	PDTimer_Start();
	while (!PDdone()) {}
	ResetPD();
	KP_STRAIGHT = 44.75;
}

/**
 * @brief Stoppt die Bewegung der MicroRat.
 *
 * Setzt die PWM-Werte beider Motoren auf Null, um die MicroRat sofort anzuhalten.
 */

void Stop(){
	MotorsSetSpeed(0, 0);
}

/**
 * @brief Führt eine Drehung der MicroRat um eine spezifizierte Richtung aus.
 * @param direction Die Art der Drehung.
 *
 * Die Funktion setzt die Encoder zurück, berechnet die zielgerichteten Distanzen
 * für jedes Rad basierend auf dem Drehtyp und startet den PD-Regler.
 * Nach einer 180-Grad-Drehung ('around') wird zusätzlich eine Rekalibrierung
 * durchgeführt, um die Position zu verfeinern.
 *
 */
void Turn (TurnDirection direction){
	EncoderReset();
	float goal_distance_L = 0.0f;
	float goal_distance_R = 0.0f;
	float distance_90_deg = DISTANCE_PER_90_DEGREE_MM;
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
	setPDGoalD(goal_distance_R, goal_distance_L);
	isTurning = true;
	PDTimer_Start();
	while (!PDdone()) {}
	ResetPD();
	if (direction == around) {
		// Rekalibrierung direkt nach einer 180-Grad-Drehung
		PDTimer_Stop();
		MotorsSetSpeed(0, 0);
		Recalibrate();
	}
}








