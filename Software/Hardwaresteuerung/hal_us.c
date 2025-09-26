/**
 * @file hal_us.c
 * @brief Implementierung der Hardware-Abstraktionsschicht (HAL) für den Ultraschallsensor.
 *
 * Dieses Modul stellt die Schnittstelle zum Ultraschallsensor (US) der MicroRat dar.
 * Es kapselt die hardwarenahen Funktionen für die Zeitmessung (Capture-Einheit)
 * und die Triggerung (PWM-Einheit), die vom DAVE-Framework bereitgestellt werden, um Distanzen zu messen.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 *
 * @dependencies
 * - "DAVE.h": Das Haupt-Header-File des DAVE-Frameworks, das die notwendigen
 * Definitionen und Funktionsprototypen für die Peripheriesteuerung (CAPTURE_, PWM_) bereitstellt
 * - "hal_us.h": Deklarationen der öffentlichen Schnittstellen dieses Moduls
 */
#include "DAVE.h"
#include "hal_us.h"

/**
 * @brief Liest die Distanz vom vorderen Ultraschallsensor aus.
 *
 * Diese Funktion erfasst die vom CAPTURE-Modul gemessene Zeit (Echo-Dauer)
 * und rechnet diese in eine Distanz in cm um. Die Umrechnung basiert
 * auf der Schallgeschwindigkeit in Luft und spezifischen Umrechnungsfaktoren
 * für den Ultraschallsensor.
 *
 * @return Die gemessene Distanz in Millimetern [cm] als Fließkommazahl.
 * @see CAPTURE_ULTRA
 * @see CAPTURE_GetCapturedTime
 * @see UltrasoundStart
 */
float FrontRead() {
    uint32_t capture_time;
    CAPTURE_GetCapturedTime(&CAPTURE_ULTRA, &capture_time);
    float time_in_us = ((float)capture_time * 333.33)/1000;
    return time_in_us / 58.0f;
}

/**
 * @brief Startet den Ultraschall-Trigger-Puls.
 *
 * Diese Funktion startet den PWM-Kanal ('PWM_TRIGGER'), der den Trigger-Puls
 * für den Ultraschallsensor erzeugt. Dieser Puls initiiert die Schallaussendung
 * des Sensors, woraufhin die Echo-Dauer gemessen werden kann.
 * @see PWM_TRIGGER
 * @see PWM_Start
 */
void UltrasoundStart(){
	PWM_Start(&PWM_TRIGGER);
}
