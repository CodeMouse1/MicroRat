/**
 * @file hal_uart.c
 * @brief Implementierung der Hardware-Abstraktionsschicht (HAL) für die UART-Kommunikation.
 *
 * Dieses Modul stellt die Schnittstelle für die serielle Kommunikation (UART)
 * des MicroRats bereit. Es kapselt die spezifischen HAL-Funktionen,
 * die vom DAVE-Framework zur Verfügung gestellt werden,
 * um Daten über die UART-Schnittstelle zu senden.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 *
 * @dependencies
 * - "DAVE.h": Das Haupt-Header-File des DAVE-Frameworks, das die notwendigen
 * Definitionen und Funktionsprototypen für die Peripheriesteuerung (UART_ APIs) bereitstellt
 * - "Hardwaresteuerung/hal_uart.h": Deklarationen der öffentlichen Schnittstellen dieses Moduls
 */
#include "DAVE.h"
#include "Hardwaresteuerung/hal_uart.h"

/**
 * @brief Sendet einen Puffer von Daten über die UART-Schnittstelle.
 *
 * Diese Funktion überträgt die angegebenen Daten ('data') der gegebenen
 * Länge ('length') über die konfigurierte UART-Kommunikationsschnittstelle.
 * Sie prüft vor dem Senden, ob der Datenpuffer gültig und die Länge größer als Null ist.
 *
 * @param data Ein Zeiger auf den Puffer, der die zu sendenden Daten enthält.
 * @param length Die Anzahl der Bytes, die aus dem Puffer gesendet werden sollen.
 * @note Die Funktion 'UART_Transmit' blockiert möglicherweise, bis alle Daten gesendet wurden.
 * @see UART_COM
 * @see UART_Transmit
 */
void HAL_UART_SendBuffer(const uint8_t* data, uint32_t length) {
    if (data == NULL || length == 0) {
        return; // Nichts senden, wenn Puffer leer oder Länge Null
    }
    UART_Transmit(&UART_COM, (uint8_t*)data, length);
}
