/*
 * @file Applikation/maze.h
 * @brief Deklarationen und öffentliche Schnittstellen für das Labyrinthmanagement-Modul.
 *
 * Diese Header-Datei definiert die zentralen Datenstrukturen und Funktionsprototypen
 * für die Verwaltung der Labyrinthkarte. Sie stellt die globalen Variablen 'mazeMap'
 * und 'distanceMap' sowie Funktionen zur Initialisierung, Aktualisierung und Abfrage
 * der Labyrinthinformationen bereit.
 *
 * Das Modul ist ein Kernbestandteil der Applikationsebene und ermöglicht
 * dem MicroRat, seine Umgebung zu kartieren und Entscheidungen basierend
 * auf den erfassten Labyrinthdaten zu treffen.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-05-19
 * @version 1.0
 */
#ifndef APPLIKATION_MAZE_H_
#define APPLIKATION_MAZE_H_

#include <stdbool.h>

// Definitionen der Labyrinth-Dimensionen
#define MAZE_HEIGHT 4
#define MAZE_WIDTH 7

// Konstante zur Kennzeichnung unbesuchter oder unerreichbarer Zellen in der Distanzkarte
#define UNVISITED_DISTANCE 9999

/*
 * @brief Enumeration der Himmelsrichtungen.
 * Definiert die möglichen Ausrichtungen des Rat im Labyrinth.
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

/*
 * @brief Initialisiert die Labyrinthkarte.
 * Setzt alle Zellen in der 'mazeMap' auf den Zustand "keine Wände bekannt".
 */
void MazeMap_Init(void);

/*
 * @brief Aktualisiert die Labyrinthkarte basierend auf aktuellen Sensordaten und Roboterorientierung.
 * @param currentX Die aktuelle X-Koordinate der Rat.
 * @param currentY Die aktuelle Y-Koordinate der Rat.
 * @param currentOrientation Die aktuelle Ausrichtung der Rat (NORTH, EAST, SOUTH, WEST).
 */
void updateMazeMap(int currentX, int currentY, int currentOrientation);

/*
 * @brief Berechnet die Distanzkarte (kürzester Pfad) zu einem Zielpunkt im Labyrinth.
 * Nutzt einen Flood-Fill-Algorithmus, um die minimale Anzahl an Schritten von jeder Zelle zum Ziel zu ermitteln.
 * @param targetX Die X-Koordinate der Zielzelle.
 * @param targetY Die Y-Koordinate der Zielzelle.
 */
void calculateDistanceMap(int targetX, int targetY);

/*
 * @brief Gibt die aktuelle Labyrinth- und Distanzkarte über die Debug-Kommunikationsschicht aus.
 * Nützlich zur Visualisierung und Fehlersuche.
 */
void printMazeMap(void);

/*
 * @brief Prüft, ob eine gegebene Zelle innerhalb der definierten Labyrinthgrenzen liegt.
 * @param x Die X-Koordinate der zu prüfenden Zelle.
 * @param y Die Y-Koordinate der zu prüfenden Zelle.
 * @return true, wenn die Zelle gültig ist; false sonst.
 */
bool isValidCell(int x, int y);

/*
 * @brief Prüft, ob eine bekannte Wand zwischen zwei benachbarten Zellen existiert.
 * Diese Funktion ist entscheidend für die Pfadfindung, um begehbare Wege zu identifizieren.
 * @param x1 Die X-Koordinate der ersten Zelle.
 * @param y1 Die Y-Koordinate der ersten Zelle.
 * @param x2 Die X-Koordinate der zweiten (benachbarten) Zelle.
 * @param y2 Die Y-Koordinate der zweiten (benachbarten) Zelle.
 * @return true, wenn eine Wand existiert oder die Zellen ungültig/nicht benachbart sind; false sonst.
 */
bool hasWallBetween(int x1, int y1, int x2, int y2);

#endif /* APPLIKATION_MAZE_H_ */
