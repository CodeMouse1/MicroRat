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

#define QUEUE_SIZE (MAZE_WIDTH * MAZE_HEIGHT)

typedef struct {
    int x;
    int y;
} Cell;


// Das Array, das die Queue-Elemente speichert
static Cell queue[QUEUE_SIZE];
// Indizes für den Kopf (head) und das Ende (tail) der Queue
static int head = 0; // Zeigt auf das nächste Element zum Entfernen
static int tail = 0; // Zeigt auf die nächste freie Position zum Hinzufügen

// Funktion zum Hinzufügen eines Elements zur Queue (Enqueue)
static void enqueue(int x, int y) {
    // Prüfen, ob die Queue voll ist, bevor ein Element hinzugefügt wird
    // Eine volle Queue würde bedeuten, dass (tail + 1) % QUEUE_SIZE == head ist
    if ((tail + 1) % QUEUE_SIZE != head) {
        queue[tail].x = x;
        queue[tail].y = y;
        tail = (tail + 1) % QUEUE_SIZE; // Bewege das Ende der Queue weiter
    }
    // Optional: Fehlerbehandlung, falls die Queue voll ist
}

// Funktion zum Entfernen eines Elements aus der Queue (Dequeue)
static Cell dequeue(void) {
    // Annahme: Diese Funktion wird nur aufgerufen, wenn die Queue nicht leer ist.
    // Eine Überprüfung auf leere Queue sollte vor dem Aufruf erfolgen (z.B. mit isQueueEmpty()).
    Cell cell = queue[head];
    head = (head + 1) % QUEUE_SIZE; // Bewege den Kopf der Queue weiter
    return cell;
}

// Funktion zur Überprüfung, ob die Queue leer ist
static bool isQueueEmpty(void) {
    return head == tail;
}
// --- ENDE NEUE QUEUE-IMPLEMENTIERUNG ---

// --- NEU: Koordinaten-Verschiebung für die 4 Himmelsrichtungen (N, E, S, W) ---
// Diese Arrays helfen uns, die Nachbarzellen zu finden, passend zu deinen Bits: 0=N, 1=E, 2=S, 3=W
static const int dx[] = {0, 1, 0, -1}; // delta-x für Nord, Ost, Süd, West
static const int dy[] = {1, 0, -1, 0}; // delta-y für Nord, Ost, Süd, West
// --- ENDE NEUE KOORDINATEN-VERSCHIEBUNG ---



// Puffer für die Ausgabe von Strings über UART
uint8_t UART_MapString[100];

// Die 2D-Array-Repräsentation der Labyrinthkarte
int mazeMap[MAZE_HEIGHT][MAZE_WIDTH];	// Speichert die Labyrinthinformationen: 0 = unbekannt, Bits für Wände

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

void printMazeMap(void) {
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
}


// Hilfsfunktion, um zu prüfen, ob eine Zelle innerhalb des Labyrinths liegt
static bool isValidCell(int x, int y) {
    return x >= 0 && x < MAZE_WIDTH && y >= 0 && y < MAZE_HEIGHT;
}

// NEU: Funktion zur Prüfung, ob eine Wand zwischen zwei benachbarten Zellen liegt
/*bool hasWallBetween(int x1, int y1, int x2, int y2) {
    // Zuerst prüfen, ob die Zellen überhaupt gültig sind
    if (!isValidCell(x1, y1) || !isValidCell(x2, y2)) {
        // Wenn eine der Zellen ungültig ist, behandeln wir es als Wand
        // (oder einen Fehlerzustand, je nach Anwendungsfall)
        return true;
    }

    // Wenn die Zellen identisch sind, gibt es keine "Wand dazwischen"
    if (x1 == x2 && y1 == y2) {
        return false;
    }

    // Bestimme die Richtung von (x1,y1) zu (x2,y2)
    // und prüfe das entsprechende Bit in mazeMap[y1][x1]
    if (y2 > y1) { // Bewegung von (x1,y1) nach Norden zu (x1,y1+1)
        return (mazeMap[y1][x1] & (1 << 0)) != 0; // Bit 0 ist Nordwand von (x1,y1)
    } else if (y2 < y1) { // Bewegung von (x1,y1) nach Süden zu (x1,y1-1)
        return (mazeMap[y1][x1] & (1 << 2)) != 0; // Bit 2 ist Südwand von (x1,y1)
    } else if (x2 > x1) { // Bewegung von (x1,y1) nach Osten zu (x1+1,y1)
        return (mazeMap[y1][x1] & (1 << 1)) != 0; // Bit 1 ist Ostwand von (x1,y1)
    } else if (x2 < x1) { // Bewegung von (x1,y1) nach Westen zu (x1-1,y1)
        return (mazeMap[y1][x1] & (1 << 3)) != 0; // Bit 3 ist Westwand von (x1,y1)
    }

    // Dieser Fall sollte nur eintreten, wenn Zellen nicht benachbart sind
    // oder ein Logikfehler vorliegt. Wir behandeln es als Wand zur Sicherheit.
    return true;
}*/

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

    // Dieser Fall sollte nicht erreicht werden, wenn Zellen benachbart sind.
    // Sicherheitshalber als Wand behandeln.
    return true;
}

void calculateDistanceMap(int targetX, int targetY) {
    // 1. Distanzkarte initialisieren und Queue leeren
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            distanceMap[y][x] = UNVISITED_DISTANCE; // Alle Zellen als "unerreichbar" markieren
        }
    }
    head = 0; // Queue zurücksetzen (wichtig vor jedem Flood Fill Lauf!)
    tail = 0;

    // 2. Zielzelle initialisieren und zur Queue hinzufügen
    if (isValidCell(targetX, targetY)) { // Prüfen, ob das Ziel gültig ist
        distanceMap[targetY][targetX] = 0; // Zielzelle hat Distanz 0
        enqueue(targetX, targetY);         // Zielzelle zur Queue hinzufügen
    }

    // 3. Flood Fill Algorithmus (Breitensuche - BFS)
    while (!isQueueEmpty()) {
        Cell currentCell = dequeue(); // Aktuelle Zelle aus der Queue nehmen
        int currentDist = distanceMap[currentCell.y][currentCell.x]; // Distanz der aktuellen Zelle

        // Durchlaufe alle 4 Nachbarrichtungen (N, E, S, W)
        for (int i = 0; i < 4; i++) {
            int nextX = currentCell.x + dx[i]; // X-Koordinate des Nachbarn
            int nextY = currentCell.y + dy[i]; // Y-Koordinate des Nachbarn

            // Prüfen, ob der Nachbar gültig ist und keine Wand dazwischen liegt
            if (isValidCell(nextX, nextY) && !hasWallBetween(currentCell.x, currentCell.y, nextX, nextY)) {
                // Wenn wir einen kürzeren Weg zu dieser Nachbarzelle gefunden haben
                if (distanceMap[nextY][nextX] > currentDist + 1) {
                    distanceMap[nextY][nextX] = currentDist + 1; // Distanz aktualisieren
                    enqueue(nextX, nextY);                       // Nachbarn zur Queue hinzufügen
                }
            }
        }
    }
}

Orientation getNewOrientation(Orientation current_orientation, TurnMouse turn) {
    if (turn == TURN_LEFT) {
        // Wenn aktuell NORTH ist und wir links drehen, wird es WEST.
        // Sonst einfach die Ausrichtung dekrementieren.
        return (current_orientation == NORTH) ? WEST : (current_orientation - 1);
    } else if (turn == TURN_RIGHT) {
        // Wenn aktuell WEST ist und wir rechts drehen, wird es NORTH.
        // Sonst einfach die Ausrichtung inkrementieren.
        return (current_orientation == WEST) ? NORTH : (current_orientation + 1);
    } else { // MOUSE_TURN_STRAIGHT
        return current_orientation;
    }
}

// --- AKTUALISIERTE getNextShortestPathMove Funktion ---
ShortestPathMove getNextShortestPathMove(int currentX, int currentY, Orientation currentOrientation) {
    ShortestPathMove next_move = {currentX, currentY, currentOrientation}; // Initialisiere mit aktueller Position
    int min_dist = distanceMap[currentY][currentX]; // Starte mit der Distanz der aktuellen Zelle
    TurnMouse best_turn = TURN_STRAIGHT; // Bevorzuge geradeaus fahren
    int target_x = currentX;
    int target_y = currentY;

    // 1. Geradeaus prüfen
    int forwardX = currentX;
    int forwardY = currentY;
    if (currentOrientation == NORTH) forwardY++;
    else if (currentOrientation == EAST) forwardX++;
    else if (currentOrientation == SOUTH) forwardY--;
    else if (currentOrientation == WEST) forwardX--;

    // Prüfen, ob die Zelle existiert und keine Wand dazwischen ist
    if (forwardX >= 0 && forwardX < MAZE_WIDTH && forwardY >= 0 && forwardY < MAZE_HEIGHT &&
        !hasWallBetween(currentX, currentY, forwardX, forwardY)) {
        if (distanceMap[forwardY][forwardX] < min_dist) {
            min_dist = distanceMap[forwardY][forwardX];
            target_x = forwardX;
            target_y = forwardY;
            best_turn = TURN_STRAIGHT;
        }
    }

    // 2. Links prüfen (relativ zur aktuellen Orientierung)
    int leftX = currentX;
    int leftY = currentY;
    Orientation leftOrientation = getNewOrientation(currentOrientation, TURN_LEFT);
    if (leftOrientation == NORTH) leftY++;
    else if (leftOrientation == EAST) leftX++;
    else if (leftOrientation == SOUTH) leftY--;
    else if (leftOrientation == WEST) leftX--;

    if (leftX >= 0 && leftX < MAZE_WIDTH && leftY >= 0 && leftY < MAZE_HEIGHT &&
        !hasWallBetween(currentX, currentY, leftX, leftY)) {
        if (distanceMap[leftY][leftX] < min_dist) {
            min_dist = distanceMap[leftY][leftX];
            target_x = leftX;
            target_y = leftY;
            best_turn = TURN_LEFT;
        }
    }

    // 3. Rechts prüfen (relativ zur aktuellen Orientierung)
    int rightX = currentX;
    int rightY = currentY;
    Orientation rightOrientation = getNewOrientation(currentOrientation, TURN_RIGHT);
    if (rightOrientation == NORTH) rightY++;
    else if (rightOrientation == EAST) rightX++;
    else if (rightOrientation == SOUTH) rightY--;
    else if (rightOrientation == WEST) rightX--;

    if (rightX >= 0 && rightX < MAZE_WIDTH && rightY >= 0 && rightY < MAZE_HEIGHT &&
        !hasWallBetween(currentX, currentY, rightX, rightY)) {
        if (distanceMap[rightY][rightX] < min_dist) {
            min_dist = distanceMap[rightY][rightX];
            target_x = rightX;
            target_y = rightY;
            best_turn = TURN_RIGHT;
        }
    }

    // Setze die gefundenen Werte in die Rückgabe-Struktur.
    // Die nextOrientation wird basierend auf der gefundenen best_turn berechnet.
    next_move.nextX = target_x;
    next_move.nextY = target_y;
    next_move.nextOrientation = getNewOrientation(currentOrientation, best_turn);

    return next_move;
}
