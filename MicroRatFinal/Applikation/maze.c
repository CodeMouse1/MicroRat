/*
 * maze.c
 *
 *  Created on: 19 May 2025
 *      Author: marcu
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

int mazeMap[MAZE_HEIGHT][MAZE_WIDTH];
int distanceMap[MAZE_HEIGHT][MAZE_WIDTH];

void MazeMap_Init(void) {
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            mazeMap[y][x] = 0; // Initialisiere alle Zellen als "keine Wände bekannt"
        }
    }
}

void updateMazeMap(int currentX, int currentY, int currentOrientation) {
    int cellInfo = mazeMap[currentY][currentX];
    bool frontWall = IsWallFront();
    bool leftWall = IsWallLeft();
    bool rightWall = IsWallRight();

    // Die entsprechenden Bits in 'cellInfo' setzen oder löschen basierend auf der Ausrichtung
    switch (currentOrientation) {
        case NORTH:
            if (frontWall) cellInfo |= (1 << 0); // Setze Bit 0 (Norden)
           //else cellInfo &= ~(1 << 0);
            if (leftWall) cellInfo |= (1 << 3);  // Setze Bit 3 (Westen)
            //else cellInfo &= ~(1 << 3);
            if (rightWall) cellInfo |= (1 << 1); // Setze Bit 1 (Osten)
            //else cellInfo &= ~(1 << 1);
            break;
        case EAST:
            if (frontWall) cellInfo |= (1 << 1); // Setze Bit 1 (Osten)
            //else cellInfo &= ~(1 << 1);
            if (leftWall) cellInfo |= (1 << 0);  // Setze Bit 0 (Norden)
            //else cellInfo &= ~(1 << 0);
            if (rightWall) cellInfo |= (1 << 2); // Setze Bit 2 (Süden)
            //else cellInfo &= ~(1 << 2);
            break;
        case SOUTH:
            if (frontWall) cellInfo |= (1 << 2); // Setze Bit 2 (Süden)
            //else cellInfo &= ~(1 << 2);
            if (leftWall) cellInfo |= (1 << 1);  // Setze Bit 1 (Osten)
            //else cellInfo &= ~(1 << 1);
            if (rightWall) cellInfo |= (1 << 3); // Setze Bit 3 (Westen)
            //else cellInfo &= ~(1 << 3);
            break;
        case WEST:
            if (frontWall) cellInfo |= (1 << 3); // Setze Bit 3 (Westen)
            //else cellInfo &= ~(1 << 3);
            if (leftWall) cellInfo |= (1 << 2);  // Setze Bit 2 (Süden)
            //else cellInfo &= ~(1 << 2);
            if (rightWall) cellInfo |= (1 << 0); // Setze Bit 0 (Norden)
            //else cellInfo &= ~(1 << 0);
            break;
    }
    mazeMap[currentY][currentX] = cellInfo; // Speichern der aktualisierten Infos
}

/*void printMazeMap(void) {
	UART_Transmit(&UART_COM, (uint8_t*)"Labyrinth Karte:\n\r", sizeof("Labyrinth Karte:\n\r") - 1);
	// Gehe Zellenweise durch das Labyrinth (von oben nach unten, wie in deinem Skript)
	for (int y = MAZE_HEIGHT - 1; y >= 0; y--) { // Y-Achse umgekehrt für Top-Down-Darstellung
		for (int x = 0; x < MAZE_WIDTH; x++) {
			// Format: [Y][X]:MAZE_MAP_VALUE:DISTANCE_MAP_VALUE,
			// Beispiel: [3][6]:10:0, (Zelle 6,3 hat MazeMap Wert 10 und Distanz 0)
			// Wenn eine Zelle unerreichbar ist, nutzen wir UNVISITED_DISTANCE
			sprintf((char*)UART_MapString, "[%d][%d]:%d:%d,",
					y,
					x,
					mazeMap[y][x],
					distanceMap[y][x]); // NEU: distanceMap Wert hinzugefügt
			UART_Transmit(&UART_COM, UART_MapString, strlen((char*)UART_MapString));
		}
		// Zeilenumbruch nach jeder Labyrinth-Reihe
		UART_Transmit(&UART_COM, (uint8_t*)"\n\r", sizeof("\n\r") - 1);
	}
	// Eine zusätzliche Leerzeile signalisiert das Ende des Datenblocks für dein Python-Skript
	UART_Transmit(&UART_COM, (uint8_t*)"\n\r", sizeof("\n\r") - 1);
}*/
void printMazeMap(void) {
    // Nun nutzen wir die abstrahierte Kommunikationsschicht
    Debug_Comms_SendString("Labyrinth Karte:\n\r"); // <-- Geändert!

    for (int y = MAZE_HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            sprintf((char*)UART_MapString, "[%d][%d]:%d:%d,",
                    y,
                    x,
                    mazeMap[y][x],
                    distanceMap[y][x]);
            Debug_Comms_SendString((char*)UART_MapString); // <-- Geändert!
        }
        Debug_Comms_SendString("\n\r"); // <-- Geändert!
    }
    Debug_Comms_SendString("\n\r"); // <-- Geändert!
}

// Hilfsfunktion, um zu prüfen, ob eine Zelle innerhalb des Labyrinths liegt
bool isValidCell(int x, int y) {
    return x >= 0 && x < MAZE_WIDTH && y >= 0 && y < MAZE_HEIGHT;
}

bool hasWallBetween(int x1, int y1, int x2, int y2) {
    // 1. Zuerst prüfen, ob die Zellen überhaupt gültig sind
    if (!isValidCell(x1, y1) || !isValidCell(x2, y2)) {
        // Wenn eine der Zellen ungültig ist, behandeln wir es als Wand
        return true;
    }
    // 2. Wenn die Zellen identisch sind, gibt es keine "Wand dazwischen"
    if (x1 == x2 && y1 == y2) {
        return false;
    }
    // 3. Bestimme die Richtung von (x1,y1) zu (x2,y2) und prüfe SYMMETRISCH
    //    d.h., die entsprechende Wand in mazeMap[y1][x1] ODER die Gegenwand in mazeMap[y2][x2]

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
