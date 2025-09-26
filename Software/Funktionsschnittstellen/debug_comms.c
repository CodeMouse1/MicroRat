/**
 * @file debug_comms.c
 * @brief Implementierung des Debug-Kommunikationsmoduls.
 *
 * Dieses Modul stellt Funktionen zum Senden von Debug-Nachrichten
 * über eine serielle Schnittstelle (UART) bereit. Es dient als Abstraktionsschicht
 * über der Hardware-Abstraktionsschicht (HAL) für UART, um das Senden von
 * Zeichenketten und Byte-Puffern für Debugging-Zwecke zu vereinfachen.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 *
 * @dependencies
 * - <string.h>: Für string-Operationen wie strlen()
 * - <stdint.h>: Für Standard-Integer-Typen
 * - Funktionsschnittstellen/debug_comms.h: Deklarationen der öffentlichen Schnittstellen dieses Moduls
 * - Hardwaresteuerung/hal_uart.h: Hardware-Abstraktionsschicht für UART-Operationen
 */
#include <string.h>
#include <stdint.h>
#include "Funktionsschnittstellen/debug_comms.h"
#include "Hardwaresteuerung/hal_uart.h"

/**
 * @brief Sendet eine Null-terminierte Zeichenkette über die Debug-Kommunikationsschnittstelle.
 *
 * Diese Funktion nimmt eine Zeichenkette entgegen und sendet sie Byte für Byte
 * über die konfigurierte UART-Schnittstelle.
 *
 * @param str Zeiger auf die zu sendende Null-terminierte Zeichenkette.
 * @note Die Funktion prüft auf NULL-Zeiger, um Abstürze zu vermeiden.
 */
void Debug_Comms_SendString(const char* str) {
    if (str == NULL) {
        return;
    }
    HAL_UART_SendBuffer((const uint8_t*)str, strlen(str));
}

/**
 * @brief Sendet einen Puffer von Bytes über die Debug-Kommunikationsschnittstelle.
 *
 * Diese Funktion nimmt einen Zeiger auf Daten und deren Länge entgegen und sendet
 * die angegebenen Bytes über die konfigurierte UART-Schnittstelle. Dies ist nützlich
 * für das Senden von Rohdaten oder binären Informationen.
 *
 * @param data Zeiger auf den zu sendenden Datenpuffer.
 * @param length Die Anzahl der Bytes, die aus dem Puffer gesendet werden sollen.
 * @note Die Funktion prüft auf NULL-Zeiger oder eine Länge von 0, um unnötige Operationen
 * oder Abstürze zu vermeiden.
 */
void Debug_Comms_SendBuffer(const uint8_t* data, size_t length) {
    if (data == NULL || length == 0) {
        return;
    }
    HAL_UART_SendBuffer(data, length);
}
