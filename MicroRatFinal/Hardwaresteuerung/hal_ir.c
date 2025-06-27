/**
 * @file hal_ir.c
 * @brief Implementierung der Hardware-Abstraktionsschicht (HAL) für Infrarotsensoren.
 *
 * Dieses Modul stellt die direkte Schnittstelle zu den Infrarotsensoren (IR)
 * des MicroRats dar. Es kapselt die hardwarenahen Funktionen des Analog-Digital-Wandlers (ADC),
 * die vom DAVE-Framework bereitgestellt werden, um die Rohwerte der IR-Sensoren auszulesen.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 *
 * @dependencies
 * - "DAVE.h": Das Haupt-Header-File des DAVE-Frameworks, das die notwendigen
 * Definitionen und Funktionsprototypen für die Peripheriesteuerung bereitstellt (z.B. ADC_MEASUREMENT_ APIs)
 * - "Hardwaresteuerung/hal_ir.h": Deklarationen der öffentlichen Schnittstellen dieses Moduls
 */
#include "DAVE.h"
#include "Hardwaresteuerung/hal_ir.h"

/**
 * @brief Liest den rohen ADC-Wert des linken Infrarotsensors aus.
 *
 * Diese Funktion greift auf das ADC-Modul des Mikrocontrollers zu,
 * um den aktuellen analogen Wert des linken IR-Sensors zu erfassen
 * und als digitalen Rohwert zurückzugeben. Dieser Wert muss typischerweise
 * später in eine physikalische Distanz umgerechnet werden.
 *
 * @return Der rohe ADC-Wert des linken IR-Sensors.
 * @see ADC_MEASUREMENT_GetResult
 * @see ADC_MEASUREMENT_Channel_B
 */
int ReadLeft(void) {
    XMC_VADC_RESULT_SIZE_t result = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_Channel_B);
    return result;  // Hier wird der ADC-Wert des linken IR-Sensors zurückgegeben
}

/**
 * @brief Liest den rohen ADC-Wert des rechten Infrarotsensors aus.
 *
 * Diese Funktion greift auf das ADC-Modul des Mikrocontrollers zu,
 * um den aktuellen analogen Wert des rechten IR-Sensors zu erfassen
 * und als digitalen Rohwert zurückzugeben. Dieser Wert muss typischerweise
 * später in eine physikalische Distanz umgerechnet werden.
 *
 * @return Der rohe ADC-Wert des rechten IR-Sensors.
 * @see ADC_MEASUREMENT_GetResult
 * @see ADC_MEASUREMENT_Channel_A
 */
int ReadRight(void) {
    XMC_VADC_RESULT_SIZE_t result = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_Channel_A);
    return result;  // Hier wird der ADC-Wert des rechten IR-Sensors zurückgegeben
}
