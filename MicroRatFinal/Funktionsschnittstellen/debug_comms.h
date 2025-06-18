/*
 * debug_comms.h
 *
 *  Created on: 18 Jun 2025
 *      Author: marcu
 */

#ifndef FUNKTIONSSCHNITTSTELLEN_DEBUG_COMMS_H_
#define FUNKTIONSSCHNITTSTELLEN_DEBUG_COMMS_H_

/**
 * @brief Sendet einen Null-terminierten String über die Debug-Schnittstelle.
 * @param str Der zu sendende String.
 */
void Debug_Comms_SendString(const char* str);

/**
 * @brief Sendet einen Puffer von Daten über die Debug-Schnittstelle.
 * @param data Zeiger auf den Datenpuffer.
 * @param length Die Anzahl der zu sendenden Bytes.
 */
void Debug_Comms_SendBuffer(const uint8_t* data, size_t length);

#endif /* FUNKTIONSSCHNITTSTELLEN_DEBUG_COMMS_H_ */
