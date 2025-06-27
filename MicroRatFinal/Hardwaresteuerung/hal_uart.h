/**
 * @file hal_uart.h
 * @brief Deklarationen der Hardware-Abstraktionsschicht (HAL) für die UART-Kommunikation.
 *
 * Dieses Header-File definiert die öffentliche Schnittstelle für die serielle
 * Kommunikation (UART) der MicroRats. Es stellt Funktionsprototypen bereit,
 * um Daten über die UART-Schnittstelle zu senden. Dieses Modul kapselt die
 * hardwarenahen Details der UART-Peripherie.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 */
#ifndef HARDWARESTEUERUNG_HAL_UART_H_
#define HARDWARESTEUERUNG_HAL_UART_H_

/**
 * @brief Sendet einen Puffer von Daten über die UART-Schnittstelle.
 *
 * Diese Funktion überträgt die angegebenen Daten einer bestimmten Länge
 * über die konfigurierte UART-Schnittstelle. Sie ist Teil der Hardware-
 * Abstraktionsschicht und bietet eine einfache Möglichkeit zum Senden
 * von Rohdaten.
 *
 * @param data Ein Zeiger auf den Puffer, der die zu sendenden Daten enthält.
 * Die Daten werden als Konstante behandelt und nicht modifiziert.
 * @param length Die Anzahl der Bytes, die aus dem Puffer gesendet werden sollen.
 */
void HAL_UART_SendBuffer(const uint8_t* data, uint32_t length);

#endif /* HARDWARESTEUERUNG_HAL_UART_H_ */
