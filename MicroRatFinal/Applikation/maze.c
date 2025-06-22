/*
 * @file maze.c
 * @brief Verwaltet die interne Darstellung und Interaktion mit dem Labyrinth.
 *
 * Dieses Modul ist verantwortlich für die Speicherung der Labyrinthstruktur
 * in der 'mazeMap' (welche Wände bekannt sind) und der 'distanceMap'
 * (berechnete Distanzen zum Ziel). Es aktualisiert die Karte basierend
 * auf aktuellen Sensordaten während der Erkundung und stellt Funktionen
 * bereit, um Labyrinthinformationen abzufragen (z.B. 'asWallBetween').
 *
 * Es bildet das zentrale "Gedächtnis" der MicroRat über die erkundete Umgebung
 * und unterstützt die Pfadfindungsalgorithmen, indem es eine präzise
 * und aktuelle Darstellung des Labyrinths liefert. Die 'printMazeMap'-Funktion
 * nutzt die Debug-Kommunikationsschicht zur Visualisierung des aktuellen
 * Kartenzustands.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-22
 * @version 1.0
 *
 * @dependencies
 * - Applikation/maze.h: Deklarationen für dieses Modul
 * - Applikation/pathfinding.h: Für die Nutzung der distanceMap und MazeMap_Init
 * - Applikation/state_machine.h: Für allgemeine Zustandsinformationen wie aktuelle Position und Orientierung
 * - Funktionsschnittstellen/sensors.h: Für die Abfrage der Wandsensoren (IsWallFront, IsWallLeft, IsWallRight)
 * - Funktionsschnittstellen/debug_comms.h: Für die Ausgabe der Karte zu Debugging-Zwecken
 */
#include <stdio.h>
#include <stdint.h>
#include "Applikation/maze.h"
#include "Applikation/pathfinding.h"
#include "Applikation/state_machine.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Funktionsschnittstellen/debug_comms.h"


// Puffer für die Ausgabe von Strings über UART
uint8_t UART_MapString[100];

// Globale 2D-Arrays zur Speicherung der Labyrinthdaten:
int mazeMap[MAZE_HEIGHT][MAZE_WIDTH];
int distanceMap[MAZE_HEIGHT][MAZE_WIDTH];

/*
 * @brief Initialisiert die Labyrinthkarte.
 *
 * Setzt alle Zellen in der 'mazeMap' auf 0, was bedeutet, dass
 * zu Beginn keine Wände bekannt sind.
 */
void MazeMap_Init(void) {
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            mazeMap[y][x] = 0;
        }
    }
}

/*
 * @brief Aktualisiert die Labyrinthkarte mit den aktuellen Sensordaten.
 *
 * Liest die Zustände der Front-, Links- und Rechtssensoren aus und speichert
 * die erkannten Wände in der 'mazeMap' für die aktuelle Zelle,
 * basierend auf der aktuellen Orientierung der Rat.
 *
 * Die Bits in 'mazeMap[y][x]' repräsentieren die Wände der Zelle:
 * Bit 0: Nordwand (N)
 * Bit 1: Ostwand (O)
 * Bit 2: Südwand (S)
 * Bit 3: Westwand (W)
 *
 * @param currentX Die aktuelle X-Koordinate der Rat.
 * @param currentY Die aktuelle Y-Koordinate der Rat.
 * @param currentOrientation Die aktuelle Ausrichtung der Rat (NORTH, EAST, SOUTH, WEST).
 */
void updateMazeMap(int currentX, int currentY, int currentOrientation) {
    // Hole die aktuellen Wandinformationen der Zelle, bevor neue hinzugefügt werden.
    int cellInfo = mazeMap[currentY][currentX];
    bool frontWall = IsWallFront();
    bool leftWall = IsWallLeft();
    bool rightWall = IsWallRight();

    // Die entsprechenden Bits in 'cellInfo' setzen oder löschen basierend auf der Ausrichtung
    switch (currentOrientation) {
        case NORTH:
            if (frontWall) cellInfo |= (1 << 0); // Setze Bit 0 (Norden)
            if (leftWall) cellInfo |= (1 << 3);  // Setze Bit 3 (Westen)
            if (rightWall) cellInfo |= (1 << 1); // Setze Bit 1 (Osten)
            break;
        case EAST:
            if (frontWall) cellInfo |= (1 << 1); // Setze Bit 1 (Osten)
            if (leftWall) cellInfo |= (1 << 0);  // Setze Bit 0 (Norden)
            if (rightWall) cellInfo |= (1 << 2); // Setze Bit 2 (Süden)
            break;
        case SOUTH:
            if (frontWall) cellInfo |= (1 << 2); // Setze Bit 2 (Süden)
            if (leftWall) cellInfo |= (1 << 1);  // Setze Bit 1 (Osten)
            if (rightWall) cellInfo |= (1 << 3); // Setze Bit 3 (Westen)
            break;
        case WEST:
            if (frontWall) cellInfo |= (1 << 3); // Setze Bit 3 (Westen)
            if (leftWall) cellInfo |= (1 << 2);  // Setze Bit 2 (Süden)
            if (rightWall) cellInfo |= (1 << 0); // Setze Bit 0 (Norden)
            break;
    }
    mazeMap[currentY][currentX] = cellInfo; // Aktualisierte Wandinformationen speichern
}

/*
 * @brief Gibt die aktuelle Labyrinth- und Distanzkarte über die Debug-Kommunikationsschicht aus.
 *
 * Formatiert die Informationen jeder Zelle (Koordinaten, bekannte Wände, Distanz zum Ziel)
 * und sendet sie zeilenweise über UART. Dies ist nützlich für das Debugging und die
 * Visualisierung des Erkundungs- und Pfadfindungsfortschritts.
 * @note Eine bessere Visualisierung ist mittels eines externen "Maze Visualiser" Tools möglich
 */
void printMazeMap(void) {
    // Sende einen Header, um die Ausgabe im Terminal zu identifizieren (MazeVisualiser).
    Debug_Comms_SendString("Labyrinth Karte:\n\r");
    // Iteriere von der oberen Reihe des Labyrinths (höchster Y-Wert) zur unteren (Y=0),
	// um eine intuitive top-down Ansicht zu erhalten.
    for (int y = MAZE_HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            // Formatiere die Zellinformationen in einen String: [X,Y]:Wände:Distanz
            sprintf((char*)UART_MapString, "[%d][%d]:%d:%d,",
                    y,						// Y-Koordinate der Zelle
                    x,						// X-Koordinate der Zelle
                    mazeMap[y][x],			// Bitmaske der bekannten Wände in dieser Zelle
                    distanceMap[y][x]);		// Kürzeste Distanz zum Ziel von dieser Zelle aus
            Debug_Comms_SendString((char*)UART_MapString);	// Sende den formatierten String
        }
        Debug_Comms_SendString("\n\r");
    }
    Debug_Comms_SendString("\n\r");
}

/*
 * @brief Prüft, ob eine Zelle innerhalb der definierten Labyrinthusgrenzen liegt.
 * @param x Die X-Koordinate der zu prüfenden Zelle.
 * @param y Die Y-Koordinate der zu prüfenden Zelle.
 * @return true, wenn die Zelle innerhalb der Grenzen liegt; false sonst.
 */
bool isValidCell(int x, int y) {
    return x >= 0 && x < MAZE_WIDTH && y >= 0 && y < MAZE_HEIGHT;
}

/*
 * @brief Prüft, ob eine bekannte Wand zwischen zwei benachbarten Zellen existiert.
 *
 * Diese Funktion ist entscheidend für Pfadfindungsalgorithmen, um zu bestimmen,
 * welche Pfade passierbar sind. Sie prüft symmetrisch: Wenn eine Wand in Zelle 1
 * in Richtung Zelle 2 bekannt ist, ODER eine Wand in Zelle 2 in Richtung Zelle 1
 * bekannt ist, gilt die Passage als blockiert. Ungültige Zellen werden ebenfalls
 * als "Wand" (nicht begehbar) behandelt.
 *
 * @param x1 Die X-Koordinate der ersten Zelle.
 * @param y1 Die Y-Koordinate der ersten Zelle.
 * @param x2 Die X-Koordinate der zweiten (benachbarten) Zelle.
 * @param y2 Die Y-Koordinate der zweiten (benachbarten) Zelle.
 * @return true, wenn eine Wand zwischen den Zellen bekannt ist oder eine Zelle ungültig ist; false sonst.
 */
bool hasWallBetween(int x1, int y1, int x2, int y2) {
    // 1. Zuerst prüfen, ob die Zellen überhaupt gültig sind
    if (!isValidCell(x1, y1) || !isValidCell(x2, y2)) {
        return true;
    }
    // 2. Wenn die Zellen identisch sind, gibt es keine Wand dazwischen
    if (x1 == x2 && y1 == y2) {
        return false;
    }
    // 3. Bestimme die Richtung der Bewegung von (x1,y1) zu (x2,y2) und
	//    prüfe symmetrisch die entsprechende Wand in beiden Zellen.
	//    Dies stellt sicher, dass die Wand erkannt wird, egal von welcher Seite sie kartiert wurde.

    // Bewegung von (x1,y1) nach Norden zu (x1,y1+1)
    if (y2 > y1) {
        // Prüfe Nordwand von (x1,y1) ODER Südwand von (x2,y2)
        return ((mazeMap[y1][x1] & (1 << 0)) != 0) || ((mazeMap[y2][x2] & (1 << 2)) != 0);
    }
    // Bewegung von (x1,y1) nach Süden zu (x1,y1-1)
    else if (y2 < y1) {
        // Prüfe Südwand von (x1,y1) ODER Nordwand von (x2,y2)
        return ((mazeMap[y1][x1] & (1 << 2)) != 0) || ((mazeMap[y2][x2] & (1 << 0)) != 0);
    }
    // Bewegung von (x1,y1) nach Osten zu (x1+1,y1)
    else if (x2 > x1) {
        // Prüfe Ostwand von (x1,y1) ODER Westwand von (x2,y2)
        return ((mazeMap[y1][x1] & (1 << 1)) != 0) || ((mazeMap[y2][x2] & (1 << 3)) != 0);
    }
    // Bewegung von (x1,y1) nach Westen zu (x1-1,y1)
    else if (x2 < x1) {
        // Prüfe Westwand von (x1,y1) ODER Ostwand von (x2,y2)
        return ((mazeMap[y1][x1] & (1 << 3)) != 0) || ((mazeMap[y2][x2] & (1 << 1)) != 0);
    }
    return true;
}
