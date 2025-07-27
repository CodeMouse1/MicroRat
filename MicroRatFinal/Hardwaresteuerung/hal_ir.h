/**
 * @file hal_ir.h
 * @brief Deklarationen der Hardware-Abstraktionsschicht (HAL) für Infrarotsensoren.
 *
 * Dieses Header-File definiert die öffentliche Schnittstelle für das Auslesen
 * der Rohwerte von Infrarotsensoren (IR). Es stellt die Funktionsprototypen
 * bereit, um die analogen Messwerte der linken und rechten IR-Sensoren
 * über den Analog-Digital-Wandler (ADC) zu erfassen.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 */
#ifndef HARDWARESTEUERUNG_HAL_IR_H_
#define HARDWARESTEUERUNG_HAL_IR_H_

/**
 * @brief Liest den rohen ADC-Wert des linken Infrarotsensors aus.
 *
 * Diese Funktion gibt den direkt vom Analog-Digital-Wandler (ADC)
 * ermittelten Rohwert des linken IR-Sensors zurück. Dieser Wert
 * repräsentiert die gemessene Infrarotintensität und muss später
 * in eine physikalische Distanz umgerechnet werden.
 *
 * @return Der rohe ADC-Wert des linken IR-Sensors.
 */
int ReadLeft(void);

/**
 * @brief Liest den rohen ADC-Wert des rechten Infrarotsensors aus.
 *
 * Diese Funktion gibt den direkt vom Analog-Digital-Wandler (ADC)
 * ermittelten Rohwert des rechten IR-Sensors zurück. Dieser Wert
 * repräsentiert die gemessene Infrarotintensität und muss später
 * in eine physikalische Distanz umgerechnet werden.
 *
 * @return Der rohe ADC-Wert des rechten IR-Sensors.
 */
int ReadRight(void);

#endif /* HARDWARESTEUERUNG_HAL_IR_H_ */
