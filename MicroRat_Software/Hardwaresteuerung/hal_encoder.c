/**
 * @file hal_encoder.c
 * @brief Implementierung der Hardware-Abstraktionsschicht (HAL) für Encoder.
 *
 * Dieses Modul stellt die Schnittstelle zur Interaktion mit den Quadratur-Encodern
 * des MicroRats dar. Es kapselt die spezifischen HAL-Funktionen, die vom DAVE-Framework
 * bereitgestellt werden, um die Encoder zu starten, ihre Zählwerte auszulesen und zurückzusetzen.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 *
 * @dependencies
 * - "DAVE.h": Das Haupt-Header-File des DAVE-Frameworks, das die notwendigen
 * Definitionen und Funktionsprototypen für die Peripheriesteuerung bereitstellt (z.B. COUNTER_ APIs)
 * - "Hardwaresteuerung/hal_encoder.h": Deklarationen der öffentlichen Schnittstellen dieses Moduls
 */
#include "DAVE.h"
#include "Hardwaresteuerung/hal_encoder.h"

/**
 * @brief Startet den linken Encoder-Zähler.
 *
 * Initialisiert und aktiviert das Hardware-Zählermodul für den linken Encoder,
 * sodass es beginnt, die Impulse des Encoders zu erfassen.
 * @see COUNTER_L
 * @see COUNTER_Start
 */
void EncoderStartLeft(void) {
    COUNTER_Start(&COUNTER_L);
}

/**
 * @brief Startet den rechten Encoder-Zähler.
 *
 * Initialisiert und aktiviert das Hardware-Zählermodul für den rechten Encoder,
 * sodass es beginnt, die Impulse des Encoders zu erfassen.
 * @see COUNTER_R
 * @see COUNTER_Start
 */
void EncoderStartRight(void) {
    COUNTER_Start(&COUNTER_R);
}

/**
 * @brief Liest den aktuellen Zählerwert des linken Encoders aus.
 *
 * Gibt die akkumulierte Anzahl der Ticks (Impulse) zurück, die vom
 * linken Encoder seit dem letzten Reset erfasst wurden.
 * @return Der aktuelle Zählerwert des linken Encoders.
 * @see COUNTER_L
 * @see COUNTER_GetCurrentCount
 */
int EncoderReadLeft(void) {
    return COUNTER_GetCurrentCount(&COUNTER_L);  // Gibt den aktuellen Wert des linken Encoders zurück
}

/**
 * @brief Liest den aktuellen Zählerwert des rechten Encoders aus.
 *
 * Gibt die akkumulierte Anzahl der Ticks (Impulse) zurück, die vom
 * rechten Encoder seit dem letzten Reset erfasst wurden.
 * @return Der aktuelle Zählerwert des rechten Encoders.
 * @see COUNTER_R
 * @see COUNTER_GetCurrentCount
 */
int EncoderReadRight(void) {
    return COUNTER_GetCurrentCount(&COUNTER_R);  // Gibt den aktuellen Wert des linken Encoders zurück
}

/**
 * @brief Setzt die Zählerwerte beider Encoder auf Null zurück.
 *
 * Diese Funktion löscht die akkumulierten Tick-Werte sowohl für den
 * linken als auch für den rechten Encoder. Dies ist nützlich, um die
 * zurückgelegte Distanz ab einem bestimmten Punkt neu zu messen.
 * @see COUNTER_L
 * @see COUNTER_R
 * @see COUNTER_ResetCounter
 */
void EncoderReset(){
	COUNTER_ResetCounter(&COUNTER_L);
	COUNTER_ResetCounter(&COUNTER_R);
}


