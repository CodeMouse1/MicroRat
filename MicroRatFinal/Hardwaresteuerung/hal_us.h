/**
 * @file hal_us.h
 * @brief Deklarationen der Hardware-Abstraktionsschicht (HAL) für den Ultraschallsensor.
 *
 * Dieses Header-File definiert die öffentliche Schnittstelle für die Interaktion
 * mit dem Ultraschallsensor der MicroRat. Es stellt Funktionsprototypen bereit,
 * um Distanzmessungen durchzuführen und den Sensor zu triggern. Dieses Modul
 * kapselt die hardwarenahen Details der Ultraschall-Peripherie.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 */
#ifndef HARDWARESTEUERUNG_HAL_US_H_
#define HARDWARESTEUERUNG_HAL_US_H_

/**
 * @brief Liest die Distanz vom vorderen Ultraschallsensor aus.
 *
 * Diese Funktion initiiert eine Messung und gibt die berechnete Distanz
 * zum nächsten Objekt vor dem Roboter zurück. Die Rückgabe erfolgt als
 * Gleitkommazahl, in cm.
 *
 * @return Die gemessene Distanz in Millimetern [cm] als Fließkommazahl.
 */
float FrontRead(void);

/**
 * @brief Startet den Ultraschall-Trigger-Puls.
 *
 * Diese Funktion aktiviert den notwendigen Hardware-Trigger, um eine Ultraschallmessung
 * einzuleiten. Nach dem Start des Triggers kann die Echo-Dauer erfasst und
 * in eine Distanz umgerechnet werden.
 */
void UltrasoundStart(void);

#endif /* HARDWARESTEUERUNG_HAL_US_H_ */
