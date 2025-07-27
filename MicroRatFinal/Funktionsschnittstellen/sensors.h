/**
 * @file sensors.h
 * @brief Deklarationen der Sensor-Schnittstellen und zugehörige Konstanten für den MicroRat.
 *
 * Dieses Header-File definiert die öffentlichen Schnittstellen (Funktionsprototypen)
 * und wichtigen Konstanten für das Sensor-Modul. Es ermöglicht anderen Modulen den Zugriff
 * auf Funktionen zum Initialisieren, Auslesen und Verarbeiten von Daten der
 * Infrarot- (IR), Ultraschall- (US) und Encoder-Sensoren.
 *
 * Es enthält auch essenzielle Makros für die Kalibrierung, Offsets und Schwellenwerte,
 * die für die korrekte Funktion der Sensorik der MicroRat notwendig sind.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 */
#ifndef FUNKTIONSSCHNITTSTELLEN_SENSORS_H_
#define FUNKTIONSSCHNITTSTELLEN_SENSORS_H_

#include <stdbool.h>

#define WALL_DETECTION_THRESHOLD 60	///- Schwellenwert in Millimetern zur Detektion einer Wand

#define WHEEL_RADIUS 16 ///- Radius des Roboter-Rades in Millimetern.
#define ENCODER_TICKS_PER_REV 180  ///- Anzahl der Encoder-Ticks pro vollständiger Radumdrehung.
#define WHEEL_CIRCUMFERENCE (2 * 3.14159 * WHEEL_RADIUS) ///- Umfang des Roboter-Rades in Millimetern.
#define TICK_PER_MM (WHEEL_CIRCUMFERENCE / ENCODER_TICKS_PER_REV) ///- Umrechnungsfaktor von Encoder-Ticks in Millimeter.

#define ADC_MAX_VALUE   4095    // 12-Bit-ADC (2^12 - 1)
#define ADC_REF_VOLTAGE 3.3    // 3.3V Referenzspannung des XMC

#define IR_SENSOR_OFFSET_LEFT_TO_WHEEL_MM 14	///- Offset für den linken IR-Sensor
#define IR_SENSOR_OFFSET_RIGHT_TO_WHEEL_MM 18	///- Offset für den rechten IR-Sensor

/**
 * @brief Initialisiert alle Sensoren des MicroRats.
 *
 * Diese Funktion muss einmalig zu Beginn des Programms aufgerufen werden,
 * um die Encoder und den Ultraschallsensor in Betrieb zu nehmen.
 */
void Sensors_Init(void);

/**
 * @brief Gibt die aktuellen Sensorwerte über die Debug-Kommunikationsschnittstelle aus.
 *
 * Formatiert die Distanzwerte der Ultraschall- und IR-Sensoren sowie die Encoder-Werte
 * in einen lesbaren String und sendet diesen über UART zur Anzeige.
 */
void SensorsPrint(void);

/**
 * @brief Prüft, ob der linke IR-Sensor eine Wand detektiert.
 *
 * Der Rückgabewert ist 'true', wenn die vom linken IR-Sensor gemessene Distanz
 * unter einem vordefinierten Schwellenwert ('WALL_DETECTION_THRESHOLD') liegt.
 * Dieser Schwellenwert definiert, ab welcher Nähe ein Objekt als Wand interpretiert wird.
 *
 * @return 'true' wenn eine Wand links detektiert wird; 'false' sonst.
 * @see WALL_DETECTION_THRESHOLD
 */
bool IsWallLeft(void);

/**
 * @brief Prüft, ob der rechte IR-Sensor eine Wand detektiert.
 *
 * Der Rückgabewert ist 'true', wenn die vom rechten IR-Sensor gemessene Distanz
 * unter einem vordefinierten Schwellenwert ('WALL_DETECTION_THRESHOLD') liegt.
 *
 * @return 'true' wenn eine Wand rechts detektiert wird; 'false' sonst.
 * @see WALL_DETECTION_THRESHOLD
 */
bool IsWallRight(void);

/**
 * @brief Prüft, ob der vordere Ultraschallsensor eine Wand detektiert.
 *
 * Der Rückgabewert ist 'true', wenn die vom Ultraschallsensor gemessene Distanz
 * kleiner oder gleich einem vordefinierten Schwellenwert ('WALL_DETECTION_THRESHOLD') ist.
 *
 * @return 'true' wenn eine Wand vorne detektiert wird; 'false' sonst.
 * @see WALL_DETECTION_THRESHOLD
 */
bool IsWallFront(void);

/**
 * @brief Führt eine umfassende Diagnoseprüfung aller kritischen Sensoren und Aktuatoren durch.
 *
 * Diese Funktion überprüft die Funktionsfähigkeit des Front-Ultraschallsensors,
 * der linken und rechten IR-Sensoren sowie der Motor-Encoder-Systeme.
 * Sie gibt detaillierte Meldungen über die Debug-Kommunikationsschnittstelle aus
 * und signalisiert das Gesamtergebnis über die User-Interface-Schnittstelle.
 * Bei erkannten Fehlern wird die Funktion in einer Endlosschleife verbleiben
 * und kontinuierlich Fehlermeldungen ausgeben, bis das Problem behoben ist.
 *
 * @return 'true' wenn alle Diagnosetests erfolgreich waren; 'false' wenn mindestens ein Test fehlschlägt.
 * @note Im Fehlerfall geht die Funktion in eine Endlosschleife und beendet sich nicht.
 */
bool PerformDiagnosticCheck(void);

/**
 * @brief Liest die inkrementelle Distanz des linken Encoders in Millimetern.
 *
 * Ruft die Hardware-Abstraktionsschicht des linken Encoders auf ('EncoderReadLeft()'),
 * um die seit dem letzten Reset akkumulierten Ticks zu erhalten. Diese Ticks werden dann
 * in Millimeter umgerechnet, basierend auf dem Kalibrierungsfaktor 'TICK_PER_MM'.
 *
 * @return Die akkumulierte Distanz des linken Rades in Millimetern [mm].
 * @see TICK_PER_MM
 */
float GetEncoderLeft_mm(void);

/**
 * @brief Liest die inkrementelle Distanz des rechten Encoders in Millimetern.
 *
 * Ruft die Hardware-Abstraktionsschicht des rechten Encoders auf ('EncoderReadRight()'),
 * um die seit dem letzten Reset akkumulierten Ticks zu erhalten. Diese Ticks werden dann
 * in Millimeter umgerechnet, basierend auf dem Kalibrierungsfaktor 'TICK_PER_MM'.
 *
 * @return Die akkumulierte Distanz des rechten Rades in Millimetern [mm].
 * @see TICK_PER_MM
 */
float GetEncoderRight_mm(void);

/**
 * @brief Liest die Distanz des vorderen Ultraschallsensors in Millimetern.
 *
 * Ruft die Hardware-Abstraktionsschicht des Ultraschallsensors auf ('FrontRead()')
 * und konvertiert den rohen Messwert in Millimeter.
 *
 * @return Die gemessene Distanz in Millimetern [mm].
 */
float GetDistanceFront_mm(void);

/**
 * @brief Liest die Distanz des linken IR-Sensors in Millimetern.
 *
 * Ruft die Hardware-Abstraktionsschicht des linken IR-Sensors auf ('ReadLeft()')
 * und konvertiert den rohen ADC-Wert mittels der internen LUT ('_convertIrRawToMm()')
 * in Millimeter. Abschließend wird ein Offset ('IR_SENSOR_OFFSET_LEFT_TO_WHEEL_MM')
 * subtrahiert, um die Distanz relativ zur Radachse oder einer anderen Referenz zu erhalten.
 *
 * @return Die berechnete Distanz in Millimetern [mm], korrigiert um den Offset.
 * @see _convertIrRawToMm
 * @see IR_SENSOR_OFFSET_LEFT_TO_WHEEL_MM
 */
int GetDistanceLeft_mm(void);

/**
 * @brief Liest die Distanz des rechten IR-Sensors in Millimetern.
 *
 * Ruft die Hardware-Abstraktionsschicht des rechten IR-Sensors auf ('ReadRight()')
 * und konvertiert den rohen ADC-Wert mittels der internen LUT ('_convertIrRawToMm()')
 * in Millimeter. Abschließend wird ein Offset ('IR_SENSOR_OFFSET_RIGHT_TO_WHEEL_MM')
 * subtrahiert, um die Distanz relativ zur Radachse oder einer anderen Referenz zu erhalten.
 *
 * @return Die berechnete Distanz in Millimetern [mm], korrigiert um den Offset.
 * @see _convertIrRawToMm
 * @see IR_SENSOR_OFFSET_RIGHT_TO_WHEEL_MM
 */
int GetDistanceRight_mm(void);


#endif /* FUNKTIONSSCHNITTSTELLEN_SENSORS_H_ */
