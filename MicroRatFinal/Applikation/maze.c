/*
 * maze.c
 *
 *  Created on: 19 May 2025
 *      Author: marcu
 */
#include "DAVE.h"
#include <stdio.h>
#include "Applikation/maze.h"
#include "Applikation/pathfinding.h"
#include "Applikation/state_machine.h"
#include "Funktionsschnittstellen/sensors.h"

// Puffer für die Ausgabe von Strings über UART
uint8_t UART_MapString[100];

// Die 2D-Array-Repräsentation der Labyrinthkarte
int mazeMap[MAZE_HEIGHT][MAZE_WIDTH];	// Speichert die Labyrinthinformationen: 0 = unbekannt, Bits für Wände

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
            else cellInfo &= ~(1 << 0);
            if (leftWall) cellInfo |= (1 << 3);  // Setze Bit 3 (Westen)
            else cellInfo &= ~(1 << 3);
            if (rightWall) cellInfo |= (1 << 1); // Setze Bit 1 (Osten)
            else cellInfo &= ~(1 << 1);
            break;
        case EAST:
            if (frontWall) cellInfo |= (1 << 1); // Setze Bit 1 (Osten)
            else cellInfo &= ~(1 << 1);
            if (leftWall) cellInfo |= (1 << 0);  // Setze Bit 0 (Norden)
            else cellInfo &= ~(1 << 0);
            if (rightWall) cellInfo |= (1 << 2); // Setze Bit 2 (Süden)
            else cellInfo &= ~(1 << 2);
            break;
        case SOUTH:
            if (frontWall) cellInfo |= (1 << 2); // Setze Bit 2 (Süden)
            else cellInfo &= ~(1 << 2);
            if (leftWall) cellInfo |= (1 << 1);  // Setze Bit 1 (Osten)
            else cellInfo &= ~(1 << 1);
            if (rightWall) cellInfo |= (1 << 3); // Setze Bit 3 (Westen)
            else cellInfo &= ~(1 << 3);
            break;
        case WEST:
            if (frontWall) cellInfo |= (1 << 3); // Setze Bit 3 (Westen)
            else cellInfo &= ~(1 << 3);
            if (leftWall) cellInfo |= (1 << 2);  // Setze Bit 2 (Süden)
            else cellInfo &= ~(1 << 2);
            if (rightWall) cellInfo |= (1 << 0); // Setze Bit 0 (Norden)
            else cellInfo &= ~(1 << 0);
            break;
    }
    mazeMap[currentY][currentX] = cellInfo; // Speichern der aktualisierten Infos
}

void printMazeMap(void) {
	UART_Transmit(&UART_COM, (uint8_t*)"Labyrinth Karte:\n\r", sizeof("Labyrinth Karte:\n\r") - 1);
	for (int y = MAZE_HEIGHT - 1; y >= 0; y--) {
		for (int x = 0; x < MAZE_WIDTH; x++) {
			sprintf((char*)UART_MapString, "[%d][%d]:%d,", y, x, mazeMap[y][x]);
			UART_Transmit(&UART_COM, UART_MapString, strlen((char*)UART_MapString));
		}
		UART_Transmit(&UART_COM, (uint8_t*)"\n\r", sizeof("\n\r") - 1);
	}
}
