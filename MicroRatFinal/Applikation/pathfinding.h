/**
 * @file Applikation/pathfinding.h
 * @brief Deklarationen und öffentliche Schnittstellen für das Pfadfindungs- und Navigationsmodul.
 *
 * Diese Header-Datei stellt die notwendigen Datenstrukturen
 * und Funktionsprototypen bereit, die für die Implementierung der Labyrinth-Navigationsalgorithmen
 * wie Wall Follower und den kürzesten Pfad benötigt werden.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 *
 * @dependencies
 * - Applikation/maze.h: Verwendet Labyrinth-Dimensionen und Orientierungs-Enumerationen.
 * - Applikation/state_machine.h: Benötigt Zugriff auf globale Roboterzustandsvariablen (Position, Orientierung).
 * - <stdbool.h>: Für den 'bool' Datentyp.
 */
#ifndef APPLIKATION_PATHFINDING_H_
#define APPLIKATION_PATHFINDING_H_

#include "Applikation/maze.h"
#include "Applikation/state_machine.h"
#include <stdbool.h>

/**
 * @brief Definiert die Größe der internen Warteschlange (Queue) für den Flood-Fill-Algorithmus.
 * Die Größe ist so gewählt, dass sie alle Zellen des Labyrinths aufnehmen kann.
 */
#define QUEUE_SIZE (MAZE_WIDTH * MAZE_HEIGHT)

/**
 * @brief Enumeration der Betriebsmodi für den Wall Follower Algorithmus.
 */
typedef enum {
    WALLFOLLOW_RIGHT,
    WALLFOLLOW_LEFT
} WallfollowMode;

/**
 * @brief Struktur zur Darstellung einer Zelle im Labyrinth.
 * Wird hauptsächlich für die Queue im Flood Fill Algorithmus verwendet.
 */
typedef struct {
    int x;
    int y;
} Cell;

/**
 * @brief Struktur zur Beschreibung des nächsten Schritts im kürzesten Pfad.
 * Enthält die Zielkoordinaten und die resultierende Orientierung nach dem Schritt.
 */
typedef struct {
    int nextX;
    int nextY;
    Orientation nextOrientation;
} ShortestPathMove;

// Externe Deklarationen für globale Ratzustandsvariablen.
extern int currentX;
extern int currentY;
extern int targetX;
extern int targetY;
extern Orientation currentOrientation;

/**
 * @brief Führt einen Schritt entlang des kürzesten Pfades aus.
 * Diese Funktion leitet die MicroRat durch eine Sequenz von Drehungen und Bewegungen,
 * um dem optimalen Pfad zur Zielzelle zu folgen. Sie kann auch mehrere Zellen in
 * einer geraden Linie fahren, um die Bewegung zu optimieren.
 * @return true, wenn ein Schritt erfolgreich ausgeführt wurde und das Ziel noch nicht erreicht ist;
 * false, wenn das Ziel erreicht ist oder kein weiterer Schritt möglich ist (Fehler/Stuck).
 */
bool Pathfinding_ExecuteShortestPath(void);

/**
 * @brief Führt den Wall Follower Algorithmus aus.
 * Die MicroRat versucht, einer Wand auf der spezifizierten Seite zu folgen, um das Labyrinth zu erkunden.
 * Diese Funktion führt eine einzelne Bewegungsentscheidung (Drehung + Vorwärts) aus.
 * @param mode Der Modus des Wall Followers (WALLFOLLOW_RIGHT oder WALLFOLLOW_LEFT).
 */
void Pathfinding_Wallfollower(WallfollowMode mode);

/**
 * @brief Berechnet die Distanzkarte zum Ziel mit dem Flood Fill Algorithmus.
 * Füllt die 'distanceMap' in 'maze.c' mit der minimalen Anzahl von Schritten
 * von jeder erreichbaren Zelle zum angegebenen Ziel.
 * @param targetX Die X-Koordinate der Zielzelle.
 * @param targetY Die Y-Koordinate der Zielzelle.
 */
void Pathfinding_CalculateDistanceMap(int targetX, int targetY);

/**
 * @brief Bestimmt die neue Orientierung der MicroRat nach einer relativen Drehung.
 * @param current_orientation Die aktuelle Orientierung der MicroRat.
 * @param turn Die Art der relativen Drehung (TURN_LEFT, TURN_RIGHT, TURN_STRAIGHT).
 * @return Die resultierende Orientierung nach der Drehung.
 */
Orientation Pathfinding_GetNewOrientation(Orientation current_orientation, TurnMouse turn);

/**
 * @brief Ermittelt den nächsten optimalen Schritt im kürzesten Pfad basierend auf der Distanzkarte.
 * Die Funktion prüft die Nachbarzellen (geradeaus, links, rechts) und wählt diejenige mit der
 * niedrigsten Distanz zum Ziel, die erreichbar ist. Präferenzen (geradeaus vor Drehung) werden berücksichtigt.
 * @param currentX Die aktuelle X-Koordinate der MicroRat.
 * @param currentY Die aktuelle Y-Koordinate der MicroRat.
 * @param currentOrientation Die aktuelle Ausrichtung der MicroRat.
 * @return Eine 'ShortestPathMove'-Struktur, die die Koordinaten und Orientierung der nächsten Zelle im Pfad enthält.
 */
ShortestPathMove Pathfinding_GetNextShortestPathMove(int currentX, int currentY, Orientation currentOrientation);

#endif /* APPLIKATION_PATHFINDING_H_ */
