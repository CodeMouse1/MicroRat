/**
 * @file hal_encoder.h
 * @brief Deklarationen der Hardware-Abstraktionsschicht (HAL) für Encoder.
 *
 * Dieses Header-File definiert die öffentliche Schnittstelle für die Interaktion
 * mit den Quadratur-Encodern des MicroRats. Es stellt Funktionsprototypen
 * bereit, um die Encoder zu starten, ihre Zählwerte auszulesen und zurückzusetzen.
 * Dieses Modul dient als hardwarenahe Abstraktion für die Encoder-Peripherie.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 */
#ifndef HARDWARESTEUERUNG_HAL_ENCODER_H_
#define HARDWARESTEUERUNG_HAL_ENCODER_H_

/**
 * @brief Startet den Zähler für den linken Encoder.
 *
 * Initialisiert und aktiviert das entsprechende Hardware-Zählermodul,
 * sodass es beginnt, die Impulse des linken Encoders zu erfassen.
 */
void EncoderStartLeft(void);

/**
 * @brief Startet den Zähler für den rechten Encoder.
 *
 * Initialisiert und aktiviert das entsprechende Hardware-Zählermodul,
 * sodass es beginnt, die Impulse des rechten Encoders zu erfassen.
 */
void EncoderStartRight(void);

/**
 * @brief Setzt die Zählerstände beider Encoder auf Null zurück.
 *
 * Diese Funktion löscht die akkumulierten Tick-Werte sowohl für den
 * linken als auch für den rechten Encoder, um eine neue Messung
 * der zurückgelegten Distanz zu ermöglichen.
 */
void EncoderReset(void);

/**
 * @brief Setzt die Zählerstände beider Encoder auf Null zurück.
 *
 * Diese Funktion löscht die akkumulierten Tick-Werte sowohl für den
 * linken als auch für den rechten Encoder, um eine neue Messung
 * der zurückgelegten Distanz zu ermöglichen.
 */
void StartSensorTimer(void);

/**
 * @brief Liest den aktuellen Zählerwert des linken Encoders aus.
 *
 * @return Der aktuelle akkumulierte Tick-Wert des linken Encoders.
 */
int EncoderReadLeft(void);

/**
 * @brief Liest den aktuellen Zählerwert des rechten Encoders aus.
 *
 * @return Der aktuelle akkumulierte Tick-Wert des rechten Encoders.
 */
int EncoderReadRight(void);


#endif /* HARDWARESTEUERUNG_HAL_ENCODER_H_ */
