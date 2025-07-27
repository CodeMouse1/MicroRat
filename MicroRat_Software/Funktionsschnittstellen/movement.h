/**
 * @file movement.h
 * @brief Deklarationen der Bewegungsfunktionen und relevanter Konstanten.
 *
 * Diese Header-Datei definiert die Schnittstelle für Funktionen, die die
 * physische Bewegung der MicroRat steuern, wie das Fahren von Zellen oder
 * das Ausführen von Drehungen. Sie enthält auch wichtige Konstanten für
 * die Robotergeometrie und Sensorabstände.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 */
#ifndef FUNKTIONSSCHNITTSTELLEN_MOVEMENT_H_
#define FUNKTIONSSCHNITTSTELLEN_MOVEMENT_H_

/**
 * @brief Der Abstand zwischen den Rädern (Radbasis) der MicroRat in Millimetern.
 *
 * Dieser Wert ist entscheidend für die Berechnung von Drehungen.
 */
#define ROBOT_WHEEL_BASE_MM 88.0f

/**
 * @brief Referenzwert für die Länge einer Labyrinthzelle in Millimetern.
 *
 * Dieser Wert wird als Standard für die Berechnung von Fahrdistanzen verwendet,
 * kann aber durch Sensorik dynamisch angepasst werden (siehe EstimateCellSize).
 */
#define MM_PER_CELL_REFERENCE 160.0f // 16cm

/**
 * @brief Die Distanz, die ein Rad fahren muss, um eine 90-Grad-Drehung des Roboters zu bewirken.
 *
 * Berechnet basierend auf dem Radstand: (PI * ROBOT_WHEEL_BASE_MM) / 4.0f.
 */
#define DISTANCE_PER_90_DEGREE_MM (M_PI * ROBOT_WHEEL_BASE_MM / 4.0f)

/**
 * @brief Der minimale sichere Abstand zu einer Wand in Millimetern.
 *
 * Wird bei der Zellengrößen-Schätzung verwendet, um den befahrbaren Bereich zu bestimmen.
 */
#define MIN_WALL_DISTANCE 27.5f

/**
 * @brief Der maximale Messabstand der Infrarotsensoren in Millimetern.
 *
 * Werte über diesem Schwellenwert werden als "keine Wand" oder "zu weit entfernt" interpretiert.
 */
#define MAX_MEASUREMENT_DISTANCE 1000.0f

/**
 * @brief Enumeration der möglichen Drehrichtungen für die MicroRat.
 */
typedef enum {
	none,
	left,
	right,
	around
} TurnDirection;

/**
 * @brief Bewegt die MicroRat logisch eine Labyrinthzelle vorwärts.
 *
 * Die genaue Distanz wird dynamisch mittels EstimateCellSize() bestimmt.
 */
void MoveOneCell(void);

/**
 * @brief Bewegt die MicroRat eine spezifizierte Anzahl von Labyrinthzellen vorwärts.
 * @param numCells Die Anzahl der Zellen, die gefahren werden sollen.
 *
 * @note Dies ist eine Work-In-Progress (WIP) Funktion für den Schnelllauf.
 */
void MoveMultipleCells(int numCells);

/**
 * @brief Stoppt die aktuelle Bewegung der MicroRat.
 *
 * Setzt die Motor-PWM-Werte auf Null, um die MicroRat anzuhalten.
 */
void Stop(void);

/**
 * @brief Führt eine Drehung der MicroRat in eine spezifizierte Richtung aus.
 * @param direction Die gewünschte Drehrichtung.
 */
void Turn(TurnDirection direction);

/**
 * @brief Führt eine Rekalibrierung der Position durch und fährt ein kurzes Stück vorwärts.
 *
 * Dient der präzisen Ausrichtung an einer Wand und dem Zurücksetzen der Encoder.
 */
void Recalibrate(void);

/**
 * @brief Schätzt die tatsächliche Länge einer Labyrinthzelle basierend auf Sensordaten.
 *
 * Verwendet den Frontsensor, um die Distanz zur nächsten Wand zu messen und daraus
 * die Zellengröße abzuleiten.
 * @return Die geschätzte Zellengröße in Millimetern.
 */
float EstimateCellSize(void);

#endif /* FUNKTIONSSCHNITTSTELLEN_MOVEMENT_H_ */
