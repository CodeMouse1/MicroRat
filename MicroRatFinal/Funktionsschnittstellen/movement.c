/*
 * movement.c
 *
 *  Created on: 17 Apr 2025
 *      Author: marcu
 */
#include "Dave.h"
#include "Funktionsschnittstellen/movement.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Funktionsschnittstellen/start_condition.h"
#include "Hardwaresteuerung/hal_motor.h"
#include "Hardwaresteuerung/hal_encoder.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t UART_String[100];

#define TARGET_PWM 2500             // Ziel PWM-Wert
#define PWM_MAX 3000                // Maximaler PWM-Wert (60%)
#define PWM_MIN 0                   // Minimaler PWM-Wert

// PID-Koeffizienten für die Geschwindigkeitsregelung
#define KP_SPEED 15                 // Proportionaler Anteil
#define KD_SPEED 5                  // Derivativer Anteil

// ⚠ Kein KI_SPEED definiert, da kein I-Anteil genutzt wird

// Volatile Variablen
volatile int counter_R = 0;         // Encoder-Ticks für rechtes Rad
volatile int counter_L = 0;         // Encoder-Ticks für linkes Rad
volatile float last_error_speed = 0; // Für D-Anteil

void DrivePD() {

	MotorsSetForward();

    counter_R = EncoderReadRight();
    counter_L = EncoderReadLeft();

    // Fehler berechnen
    float error_speed = counter_L - counter_R;

    // D-Anteil berechnen (Differenz zum letzten Fehler)
    float d_error_speed = error_speed - last_error_speed;
    last_error_speed = error_speed;

    // PD-Korrektur berechnen
    float correction_speed = KP_SPEED * error_speed + KD_SPEED * d_error_speed;

    // Korrektur begrenzen
    if (correction_speed > 300) correction_speed = 300;
    if (correction_speed < -300) correction_speed = -300;

    // PWM-Werte berechnen
    int pwmL = TARGET_PWM - correction_speed;
    int pwmR = TARGET_PWM + correction_speed;

    // PWM-Werte begrenzen
    pwmL = (pwmL > PWM_MAX) ? PWM_MAX : (pwmL < PWM_MIN) ? PWM_MIN : pwmL;
    pwmR = (pwmR > PWM_MAX) ? PWM_MAX : (pwmR < PWM_MIN) ? PWM_MIN : pwmR;

    // PWM setzen
    MotorsSetSpeed(pwmL, pwmR);

    // Debug-Ausgabe über UART
   /* sprintf((char*)UART_String, " PWM_L: %d PWM_R: %d || count_L: %d count_R: %d\n\r", pwmL, pwmR, counter_L, counter_R);
    UART_Transmit(&UART_COM, UART_String, sizeof(UART_String));*/
}

void MovementInit(){
	MotorsSetForward();
}

void Turn(TurnDirection dir) {
    int duration_ms = 0;

    switch(dir) {
        case left:
            MotorsSetLeft();
            duration_ms = 385;  // Zeit für 90°
            break;
        case right:
            MotorsSetRight();
            duration_ms = 300;  // Zeit für 90°
            break;
        case around:
            MotorsSetRight();
            duration_ms = 625;  // Zeit für 180°
            break;
    }
    MotorsSetSpeed(3000, 3000);  // 30% PWM für konstante Geschwindigkeit
    Delay(duration_ms);  // Alternativ: Einfacher Delay, bis die Zeit vorbei ist
    if(dir == left){
        MotorsSetRight();
    }else{
        MotorsSetLeft();
    }
	Delay(50);
    MotorsStop();
    EncoderReset();	//Unklar ob nötig muss getestet werden
}

void Stop(){
	MotorsSetReverse();
	Delay(100);
	MotorsStop();
}





