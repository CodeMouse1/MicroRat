/**
 * @file hal_motor.c
 * @brief Implementierung der Hardware-Abstraktionsschicht (HAL) für die Ansteuerung der Motor-H-Brücken.
 *
 * Dieses Modul stellt die direkte Schnittstelle zur Steuerung der **H-Brücken** dar,
 * die wiederum die Gleichstrommotoren des MicroRats antreiben.
 * Es kapselt die spezifischen HAL-Funktionen für digitale Ein-/Ausgänge (GPIO)
 * zur Richtungskontrolle und Pulsweitenmodulation (PWM) zur Geschwindigkeitskontrolle,
 * die vom DAVE-Framework bereitgestellt werden.
 * Es ermöglicht die unabhängige Einstellung von Fahrtrichtung und Geschwindigkeit für jeden Motor.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 *
 * @dependencies
 * - <stdlib.h>: Für die 'abs()'-Funktion
 * - "DAVE.h": Das Haupt-Header-File des DAVE-Frameworks, das die notwendigen
 * Definitionen und Funktionsprototypen für die Peripheriesteuerung (DIGITAL_IO_, PWM_) bereitstellt
 * - "Hardwaresteuerung/hal_motor.h": Deklarationen der öffentlichen Schnittstellen dieses Moduls
 */
#include <stdlib.h>
#include "Dave.h"
#include "Hardwaresteuerung/hal_motor.h"

volatile MotorDirection currentMotorDirectionL = MOTOR_STOPPED;	///- Speichert die aktuelle Fahrtrichtung des linken Motors.
volatile MotorDirection currentMotorDirectionR = MOTOR_STOPPED;	///- Speichert die aktuelle Fahrtrichtung des rechten Motors.

/**
 * @brief Stellt die Geschwindigkeit und Richtung beider Motoren ein.
 *
 * Diese zentrale Funktion zur Motorsteuerung setzt sowohl die Fahrtrichtung
 * als auch die PWM-Geschwindigkeit für den linken und rechten Motor.
 * Positive Werte bedeuten Vorwärtsfahrt, negative Werte Rückwärtsfahrt,
 * und 0 stoppt den jeweiligen Motor.
 *
 * @param speedL Die gewünschte Geschwindigkeit für den linken Motor.
 * Positiv für Vorwärts, negativ für Rückwärts, 0 für Stopp.
 * Der Betrag entspricht dem PWM-Duty-Cycle-Wert.
 * @param speedR Die gewünschte Geschwindigkeit für den rechten Motor.
 * Positiv für Vorwärts, negativ für Rückwärts, 0 für Stopp.
 * Der Betrag entspricht dem PWM-Duty-Cycle-Wert.
 * @note Die PWM-Werte sind plattformspezifisch und hängen von der Konfiguration
 * des PWM-Moduls ab
 * @see DIGITAL_IO_INPUT_1
 * @see DIGITAL_IO_INPUT_2
 * @see DIGITAL_IO_INPUT_3
 * @see DIGITAL_IO_INPUT_4
 * @see PWM_L
 * @see PWM_R
 * @see MotorDirection
 */
void MotorsSetSpeed(int speedL, int speedR) {
    // --- Linker Motor ---
    if (speedL > 0) {
        // Motor soll vorwärts drehen (IN1 High, IN2 Low)
        DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_INPUT_1);
        DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_2);
        // PWM-Duty-Cycle für linken Motor setzen
        PWM_SetDutyCycle(&PWM_L, speedL);
        currentMotorDirectionL = MOTOR_FORWARD;
    } else if (speedL < 0) {
        // Motor soll rückwärts drehen (IN1 Low, IN2 High)
        DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_1);
        DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_INPUT_2);
        // PWM-Duty-Cycle für linken Motor setzen (mit Betrag der Geschwindigkeit)
        PWM_SetDutyCycle(&PWM_L, abs(speedL));
        currentMotorDirectionL = MOTOR_BACKWARD;
    } else {
        // Motor stoppen (Freilauf: IN1 Low, IN2 Low, PWM auf 0)
        DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_1);
        DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_2);
        PWM_SetDutyCycle(&PWM_L, 0);
        currentMotorDirectionL = MOTOR_STOPPED;
    }
    // --- Rechter Motor ---
    if (speedR > 0) {
        // Motor soll vorwärts drehen (IN3 Low, IN4 High)
        DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_3);
        DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_INPUT_4);
        // PWM-Duty-Cycle für rechten Motor setzen
        PWM_SetDutyCycle(&PWM_R, speedR);
        currentMotorDirectionR = MOTOR_FORWARD;
    } else if (speedR < 0) {
        // Motor soll rückwärts drehen (IN3 High, IN4 Low)
        DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_INPUT_3);
        DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_4);
        // PWM-Duty-Cycle für rechten Motor setzen (mit Betrag der Geschwindigkeit)
        PWM_SetDutyCycle(&PWM_R, abs(speedR));
        currentMotorDirectionR = MOTOR_BACKWARD;
    } else {
        // Motor stoppen (Freilauf: IN3 Low, IN4 Low, PWM auf 0)
        DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_3);
        DIGITAL_IO_SetOutputLow(&DIGITAL_IO_INPUT_4);
        PWM_SetDutyCycle(&PWM_R, 0);
        currentMotorDirectionR = MOTOR_STOPPED;
    }
}

/**
 * @brief Stoppt beide Motoren und signalisiert das Ende der Regleraktivität.
 *
 * Diese Funktion setzt die Geschwindigkeit beider Motoren sofort auf Null und
 * stoppt typischerweise auch den Timer, der den übergeordneten PD-Regler periodisch aufruft.
 */
void StopAndSignal(){
	MotorsSetSpeed(0,0);
	TIMER_Stop(&TIMER_REGLER);
}
