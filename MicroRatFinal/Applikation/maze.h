/**
 * @file Applikation/maze.h
 * @brief Deklarationen und öffentliche Schnittstellen für das Labyrinthmanagement-Modul.
 *
 * Diese Header-Datei definiert die zentralen Datenstrukturen und Funktionsprototypen
 * für die Verwaltung der Labyrinthkarte. Sie stellt die globalen Variablen 'mazeMap'
 * und 'distanceMap' sowie Funktionen zur Initialisierung, Aktualisierung und Abfrage
 * der Labyrinthinformationen bereit.
 *
 * Das Modul ist ein Kernbestandteil der Applikationsebene und ermöglicht
 * der MicroRat, seine Umgebung zu kartieren und Entscheidungen basierend
 * auf den erfassten Labyrinthdaten zu treffen.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 */
#ifndef APPLIKATION_MAZE_H_
#define APPLIKATION_MAZE_H_

#include <stdbool.h>

/**
 * @def MAZE_HEIGHT
 * @brief Definiert die Höhe (Anzahl der Zeilen) des Labyrinths.
 */
#define MAZE_HEIGHT 4

/**
 * @def MAZE_WIDTH
 * @brief Definiert die Breite (Anzahl der Spalten) des Labyrinths.
 */
#define MAZE_WIDTH 7

/**
 * @def UNVISITED_DISTANCE
 * @brief Kennzeichnungswert für unbesuchte oder unerreichbare Zellen in der Distanzkarte.
 */
#define UNVISITED_DISTANCE 9999

/**
 * @brief Enumeration der Himmelsrichtungen.
 * Definiert die möglichen Ausrichtungen der MicroRat im Labyrinth.
 */
typedef enum {
    NORTH,
    EAST,
    SOUTH,
    WEST
} Orientation;

// Externe Deklarationen für die globalen Labyrinthkarten
extern int mazeMap[MAZE_HEIGHT][MAZE_WIDTH];
extern int distanceMap[MAZE_HEIGHT][MAZE_WIDTH];

/**
 * @brief Initialisiert die Labyrinthkarte.
 * Setzt alle Zellen in der 'mazeMap' auf den Zustand "keine Wände bekannt".
 */
void MazeMap_Init(void);

/**
 * @brief Aktualisiert die Labyrinthkarte basierend auf aktuellen Sensordaten und MicroRat Orientierung.
 * @param currentX Die aktuelle X-Koordinate der MicroRat.
 * @param currentY Die aktuelle Y-Koordinate der MicroRat.
 * @param currentOrientation Die aktuelle Ausrichtung der MicroRat (NORTH, EAST, SOUTH, WEST).
 */
void MazeMap_Update(int currentX, int currentY, int currentOrientation);

/**
 * @brief Gibt die aktuelle Labyrinth- und Distanzkarte über die Debug-Kommunikationsschicht aus.
 */
void MazeMap_Print(void);

/**
 * @brief Prüft, ob eine gegebene Zelle innerhalb der definierten Labyrinthgrenzen liegt.
 * @param x Die X-Koordinate der zu prüfenden Zelle.
 * @param y Die Y-Koordinate der zu prüfenden Zelle.
 * @return true, wenn die Zelle gültig ist; false sonst.
 */
bool MazeMap_IsValidCell(int x, int y);

/**
 * @brief Prüft, ob eine bekannte Wand zwischen zwei benachbarten Zellen existiert.
 * Diese Funktion ist entscheidend für die Pfadfindung, um begehbare Wege zu identifizieren.
 * @param x1 Die X-Koordinate der ersten Zelle.
 * @param y1 Die Y-Koordinate der ersten Zelle.
 * @param x2 Die X-Koordinate der zweiten (benachbarten) Zelle.
 * @param y2 Die Y-Koordinate der zweiten (benachbarten) Zelle.
 * @return true, wenn eine Wand existiert oder die Zellen ungültig/nicht benachbart sind; false sonst.
 */
bool MazeMap_HasWallBetween(int x1, int y1, int x2, int y2);

#endif /* APPLIKATION_MAZE_H_ */
