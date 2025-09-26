/**
 * @file debug_comms.h
 * @brief Deklarationen der öffentlichen Schnittstelle für Debug-Kommunikation.
 *
 * Diese Header-Datei definiert die Funktionen, die von anderen Modulen verwendet werden,
 * um Debug-Nachrichten und Daten über eine serielle Schnittstelle (UART) zu senden.
 * Sie abstrahiert die zugrunde liegenden UART-Operationen.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 */
#ifndef FUNKTIONSSCHNITTSTELLEN_DEBUG_COMMS_H_
#define FUNKTIONSSCHNITTSTELLEN_DEBUG_COMMS_H_

#include <stdint.h>

/**
 * @brief Sendet einen Null-terminierten String über die Debug-Schnittstelle.
 *
 * Diese Funktion ermöglicht das einfache Senden von Textnachrichten
 * zur Fehlerbehebung oder Statusausgabe über die serielle Schnittstelle.
 *
 * @param str Zeiger auf den zu sendenden Null-terminierten String.
 */
void Debug_Comms_SendString(const char* str);

/**
 * @brief Sendet einen Puffer von Daten (Bytes) über die Debug-Schnittstelle.
 *
 * Diese Funktion wird verwendet, um Rohdaten oder binäre Informationen
 * für Debugging-Zwecke über die serielle Schnittstelle zu übertragen.
 *
 * @param data Zeiger auf den Datenpuffer, der gesendet werden soll.
 * @param length Die Anzahl der Bytes, die aus dem Puffer gesendet werden sollen.
 */
void Debug_Comms_SendBuffer(const uint8_t* data, size_t length);

#endif /* FUNKTIONSSCHNITTSTELLEN_DEBUG_COMMS_H_ */
