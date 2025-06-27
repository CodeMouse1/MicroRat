/**
 * @file hal_motor.h
 * @brief Deklarationen der Hardware-Abstraktionsschicht (HAL) für die Motorsteuerung.
 *
 * Dieses Header-File definiert die öffentliche Schnittstelle und zugehörige
 * Typen für die Ansteuerung der Gleichstrommotoren der MicroRat.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 */
#ifndef HARDWARESTEUERUNG_HAL_MOTOR_H_
#define HARDWARESTEUERUNG_HAL_MOTOR_H_

/**
 * @brief Enumeration für die mögliche Fahrtrichtung eines Motors.
 * Definiert die Zustände: gestoppt, vorwärts, rückwärts.
 */
typedef enum {
    MOTOR_STOPPED,
    MOTOR_FORWARD,
    MOTOR_BACKWARD
} MotorDirection;

extern volatile MotorDirection currentMotorDirectionL;	///- Aktuelle Fahrtrichtung des linken Motors
extern volatile MotorDirection currentMotorDirectionR;	///- Aktuelle Fahrtrichtung des rechten Motors

/**
 * @brief Stellt die Geschwindigkeit und Richtung beider Motoren ein.
 *
 * Dies ist die Hauptfunktion zur variablen Steuerung der Motoren.
 * Positive Werte für 'left' und 'right' bewirken Vorwärtsfahrt, negative Werte Rückwärtsfahrt,
 * und ein Wert von 0 stoppt den jeweiligen Motor.
 *
 * @param left Die gewünschte Geschwindigkeit für den linken Motor. Der Betrag
 * entspricht dem PWM-Duty-Cycle-Wert.
 * @param right Die gewünschte Geschwindigkeit für den rechten Motor. Der Betrag
 * entspricht dem PWM-Duty-Cycle-Wert.
 */
void MotorsSetSpeed(int left, int right);

/**
 * @brief Stoppt beide Motoren und signalisiert das Ende der Regleraktivität.
 *
 * Diese Funktion setzt die Geschwindigkeit beider Motoren sofort auf Null und
 * stoppt typischerweise auch den Timer, der den übergeordneten PD-Regler periodisch aufruft.
 */
void StopAndSignal(void);

#endif /* HARDWARESTEUERUNG_HAL_MOTOR_H_ */
