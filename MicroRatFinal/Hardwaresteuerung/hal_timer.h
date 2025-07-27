/**
 * @file hal_timer.h
 * @brief Deklarationen der Hardware-Abstraktionsschicht (HAL) für Timer-Funktionen.
 *
 * Dieses Header-File definiert die öffentliche Schnittstelle für timerbasierte
 * Funktionalitäten, die für zeitkritische Operationen wie blockierende Verzögerungen
 * und die Steuerung von periodischen Aufgaben verwendet werden.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 */
#ifndef HARDWARESTEUERUNG_HAL_TIMER_H_
#define HARDWARESTEUERUNG_HAL_TIMER_H_

#include <stdint.h>
#include <stdbool.h>

extern volatile bool s_delay_isr_flag;
/**
 * @brief Erzeugt eine blockierende Verzögerung für die angegebene Dauer in Millisekunden.
 *
 * Diese Funktion pausiert die Programmausführung für die spezifizierte Anzahl
 * von Millisekunden, indem sie einen Hardware-Timer nutzt und blockierend auf dessen
 * Ablauf wartet.
 *
 * @param milliseconds Die Dauer der Verzögerung in Millisekunden [ms].
 * @note Diese Funktion ist **blockierend**, was bedeutet, dass der Mikrocontroller
 * während ihrer Ausführung keine anderen Aufgaben bearbeiten kann.
 */
void Delay_Blocking_ms(uint32_t milliseconds);

/**
 * @brief Startet den Timer, der für den PD-Regler verwendet wird.
 *
 * Diese Funktion initialisiert und startet einen periodischen Timer, der
 * in festen Intervallen ein Ereignis auslöst.
 */
void PDTimer_Start(void);

/**
 * @brief Stoppt den Timer, der für den PD-Regler verwendet wird.
 *
 * Diese Funktion beendet den periodischen Timer und hält somit die
 * Ausführung von regelmäßigen Aufgaben an.
 */
void PDTimer_Stop(void);


#endif /* HARDWARESTEUERUNG_HAL_TIMER_H_ */
