#include "Dave.h"
#include <stdio.h>
/*
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

uint8_t UART_String[100];
*/
// Funktion für die PID-Regelung (PD-Regler)
/*void PID_SR() {
    counter_R = COUNTER_GetCurrentCount(&COUNTER_R);  // Rechtes Rad
    counter_L = COUNTER_GetCurrentCount(&COUNTER_L);  // Linkes Rad

    // Fehler berechnen
    float error_speed = counter_L - counter_R;

    // D-Anteil berechnen (Differenz zum letzten Fehler)
    float d_error_speed = error_speed - last_error_speed;
    last_error_speed = error_speed;

    // ⚠ Kein I-Anteil berechnet:
    // → Da wir kontinuierlich geradeaus fahren und keine feste Zielposition anstreben,
    //    wäre ein I-Anteil kontraproduktiv.
    // → Er würde sich über die Zeit aufintegrieren, auch bei kleinen, systematischen Abweichungen,
    //    und könnte dadurch zu "drift" oder Überschwingen führen.
    // → Ein PD-Regler reagiert schnell und stabil genug auf Unterschiede zwischen linker und rechter Seite.
    // → Besonders bei einem Intervall von 50ms kann der I-Anteil träge und instabil werden,
    //    wenn er nicht sehr vorsichtig implementiert wird (z.B. mit Anti-Windup).

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
    PWM_SetDutyCycle(&PWM_L, pwmL);
    PWM_SetDutyCycle(&PWM_R, pwmR);

    // Debug-Ausgabe über UART
    //sprintf((char*)UART_String, " PWM_L: %d PWM_R: %d || count_L: %d count_R: %d\n\r", pwmL, pwmR, counter_L, counter_R);
    //UART_Transmit(&UART_COM, UART_String, sizeof(UART_String));

    // Encoder-Zähler zurücksetzen
    counter_L = 0;
    counter_R = 0;
}
*/

