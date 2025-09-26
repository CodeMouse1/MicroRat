/**
 * @file hal_timer.c
 * @brief Implementierung der Hardware-Abstraktionsschicht (HAL) für Timer-Funktionen.
 *
 * Dieses Modul stellt grundlegende timerbasierte Funktionen für den MicroRat bereit,
 * insbesondere für blockierende Verzögerungen und die Steuerung eines Timers,
 * für Regelungsaufgaben verwendet wird.
 * Es kapselt die spezifischen HAL-Funktionen des DAVE-Frameworks für die Timer-Peripherie.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 *
 * @dependencies
 * - "Dave.h": Das Haupt-Header-File des DAVE-Frameworks, das die notwendigen
 * Definitionen und Funktionsprototypen für die Peripheriesteuerung (TIMER_ APIs) bereitstellt
 * - "Hardwaresteuerung/hal_timer.h": Deklarationen der öffentlichen Schnittstellen dieses Moduls
 */
#include "Dave.h"
#include "Hardwaresteuerung/hal_timer.h"

volatile bool s_delay_isr_flag = false;

/**
 * @brief Interrupt-Service-Routine (ISR) für den Verzögerungstimer.
 *
 * Diese Funktion wird aufgerufen, wenn der 'TIMER_DELAY' seinen konfigurierten
 * Zeitintervall abgeschlossen hat. Sie löscht das Timer-Ereignis und setzt das
 * 's_delay_isr_fla', um die blockierende Verzögerung zu beenden.
 * @see TIMER_ClearEvent
 * @see TIMER_DELAY
 * @see s_delay_isr_flag
 */
void TIMER_DELAY_ISR() {
    TIMER_ClearEvent(&TIMER_DELAY);
    s_delay_isr_flag = true;
}

/**
 * @brief Erzeugt eine blockierende Verzögerung für die angegebene Dauer in Millisekunden.
 *
 * Diese Funktion nutzt den 'TIMER_DELAY', um eine präzise blockierende Wartezeit
 * zu implementieren. Sie konfiguriert den Timer für das angegebene Intervall,
 * startet ihn und wartet in einer Schleife, bis die zugehörige ISR das Ende
 * der Verzögerung signalisiert.
 *
 * @param milliseconds Die Dauer der Verzögerung in Millisekunden [ms].
 * @note Diese Funktion ist **blockierend**, d.h., der Mikrocontroller führt
 * während der Verzögerung keine anderen Aufgaben aus. Für nicht-blockierende
 * Verzögerungen sollten andere Ansätze (z.B. Zustandsmaschinen mit Timern) verwendet werden.
 * @see TIMER_DELAY
 * @see TIMER_DELAY_ISR
 * @see TIMER_Stop
 * @see TIMER_Clear
 * @see TIMER_SetTimeInterval
 * @see TIMER_ClearEvent
 * @see TIMER_Start
 * @see s_delay_isr_flag
 */
void Delay_Blocking_ms(uint32_t milliseconds) {
    TIMER_Stop(&TIMER_DELAY);

    TIMER_Clear(&TIMER_DELAY);

    uint32_t interval_units = milliseconds * 1000 * 100;
    TIMER_SetTimeInterval(&TIMER_DELAY, interval_units);

    s_delay_isr_flag = false;
    TIMER_ClearEvent(&TIMER_DELAY);

    TIMER_Start(&TIMER_DELAY);

    while (!s_delay_isr_flag) {
    }

    TIMER_Stop(&TIMER_DELAY);
    TIMER_Clear(&TIMER_DELAY);
}

/**
 * @brief Startet den Timer für den PD-Regler.
 *
 * Diese Funktion startet den Timer ('TIMER_REGLER'), der
 * in einem festen Intervall eine Reglerfunktion ausführt.
 * @see TIMER_REGLER
 * @see TIMER_Start
 */
void PDTimer_Start() {
	TIMER_Start(&TIMER_REGLER);
}

/**
 * @brief Stoppt den Timer für den PD-Regler.
 *
 * Diese Funktion stoppt den Timer ('TIMER_REGLER'), der für die
 * periodische Ausführung des PD-Reglers verwendet wird.
 * @see TIMER_REGLER
 * @see TIMER_Stop
 */
void PDTimer_Stop() {
	TIMER_Stop(&TIMER_REGLER);
}



