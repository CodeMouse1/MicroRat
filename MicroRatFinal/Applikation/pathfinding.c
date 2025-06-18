/*
 * pathfinding.c
 *
 *  Created on: 17 Apr 2025
 *      Author: marcu
 */
#include <stdbool.h>
#include "Applikation/maze.h"
#include "Applikation/pathfinding.h"
#include "Applikation/state_machine.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Funktionsschnittstellen/movement.h"

//#include "Hardwaresteuerung/hal_motor.h"

static Cell queue[QUEUE_SIZE];

static int head = 0; // Zeigt auf das nächste Element zum Entfernen
static int tail = 0; // Zeigt auf die nächste freie Position zum Hinzufügen

static void enqueue(int x, int y) {
    if ((tail + 1) % QUEUE_SIZE != head) {
        queue[tail].x = x;
        queue[tail].y = y;
        tail = (tail + 1) % QUEUE_SIZE;
    }
}

static Cell dequeue(void) {
    Cell cell = queue[head];
    head = (head + 1) % QUEUE_SIZE;
    return cell;
}

static bool isQueueEmpty(void) {
    return head == tail;
}

static const int dx[] = {0, 1, 0, -1}; // delta-x für Nord, Ost, Süd, West
static const int dy[] = {1, 0, -1, 0}; // delta-y für Nord, Ost, Süd, West

static void updateOrientation(TurnDirection direction) {
    switch (direction) {
        case right:
            if (currentOrientation == NORTH) currentOrientation = EAST;
            else if (currentOrientation == EAST) currentOrientation = SOUTH;
            else if (currentOrientation == SOUTH) currentOrientation = WEST;
            else if (currentOrientation == WEST) currentOrientation = NORTH;
            break;
        case left:
            if (currentOrientation == NORTH) currentOrientation = WEST;
            else if (currentOrientation == WEST) currentOrientation = SOUTH;
            else if (currentOrientation == SOUTH) currentOrientation = EAST;
            else if (currentOrientation == EAST) currentOrientation = NORTH;
            break;
        case around: // 180-Grad-Drehung
            if (currentOrientation == NORTH) currentOrientation = SOUTH;
            else if (currentOrientation == EAST) currentOrientation = WEST;
            else if (currentOrientation == SOUTH) currentOrientation = NORTH;
            else if (currentOrientation == WEST) currentOrientation = EAST;
            break;
        default:
            break;
    }
}

static void updatePositionAndMap() {
    if (currentOrientation == NORTH) currentY++;
    else if (currentOrientation == EAST) currentX++;
    else if (currentOrientation == SOUTH) currentY--;
    else if (currentOrientation == WEST) currentX--;
    // Rufe die Funktion von maze.c auf, um die Karte zu aktualisieren
    updateMazeMap(currentX, currentY, currentOrientation);
}

void wallfollower(WallfollowMode mode) {
    TurnDirection turnDirection = none;
	if (mode == WALLFOLLOW_RIGHT) {
		if (!IsWallRight()) {
			turnDirection = right;
		}
		else if (IsWallFront() && IsWallRight() && !IsWallLeft()) {
			turnDirection = left;
		} else if (IsWallFront() && IsWallRight() && IsWallLeft()) {
			turnDirection = around;
		}
	} else if (mode == WALLFOLLOW_LEFT) {
		if (!IsWallLeft()) {
			turnDirection = left;
		}
		else if (IsWallFront() && IsWallLeft() && !IsWallRight()) {
			turnDirection = right;
		} else if (IsWallFront() && IsWallRight() && IsWallLeft()) {
			turnDirection = around;
		}
	}
	// Falls Drehung, ausführen & Ausrichtung aktualisieren
	if (turnDirection != none) {
		Turn(turnDirection);
		updateOrientation(turnDirection);
	// Nach Drehung fahre eine Zelle weiter, aktualisiere Position & Map
		MoveOneCell();
		updatePositionAndMap();
	} else {
	// Falls keine Drehung erforderlich, fahre eine Zelle & aktualisiere Position & Map
		MoveOneCell();
		updatePositionAndMap();
	}
	// Drehungsrichung zurücksetzen
	turnDirection = none;
}

// Berechnet die Distanzkarte zum Ziel (Flood Fill)
void calculateDistanceMap(int targetX_param, int targetY_param) {
    // 1. Distanzkarte initialisieren und Queue leeren
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            distanceMap[y][x] = UNVISITED_DISTANCE; // Alle Zellen als "unerreichbar" markieren
        }
    }
    head = 0; // Queue zurücksetzen (wichtig vor jedem Flood Fill Lauf!)
    tail = 0;

    // 2. Zielzelle initialisieren und zur Queue hinzufügen
    // isValidCell ist jetzt in maze.h deklariert und in maze.c implementiert
    if (isValidCell(targetX_param, targetY_param)) { // Prüfen, ob das Ziel gültig ist
        distanceMap[targetY_param][targetX_param] = 0; // Zielzelle hat Distanz 0
        enqueue(targetX_param, targetY_param);         // Zielzelle zur Queue hinzufügen
    }

    // 3. Flood Fill Algorithmus (Breitensuche - BFS)
    while (!isQueueEmpty()) {
        Cell currentCell = dequeue(); // Aktuelle Zelle aus der Queue nehmen
        int currentDist = distanceMap[currentCell.y][currentCell.x]; // Distanz der aktuellen Zelle

        // Durchlaufe alle 4 Nachbarrichtungen (N, E, S, W)
        for (int i = 0; i < 4; i++) {
            int nextX = currentCell.x + dx[i]; // X-Koordinate des Nachbarn
            int nextY = currentCell.y + dy[i]; // Y-Koordinate des Nachbarn

            // isValidCell und hasWallBetween sind jetzt in maze.h deklariert und in maze.c implementiert
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

bool executeShortestPathStep(void) {
    // 1. Prüfen, ob das Ziel bereits erreicht ist (Distanz 0)
    if (distanceMap[currentY][currentX] == 0) {
        Stop();
        return false; // Ziel erreicht, kein weiterer Schritt nötig
    }

    // 2. Den nächsten besten Zug basierend auf der Distanzkarte finden
    ShortestPathMove next_step = getNextShortestPathMove(currentX, currentY, currentOrientation);

    // 3. Fehlerprüfung: Wenn kein Fortschritt erzielt werden kann (sollte bei funktionierendem Flood Fill nicht passieren)
    if (next_step.nextX == currentX && next_step.nextY == currentY && next_step.nextOrientation == currentOrientation) {
        Stop();
        // Optional: Hier könnte eine Fehlermeldung über UART gesendet werden
        return false; // Fehler: Kein gültiger nächster Schritt gefunden oder festgefahren
    }

    // 4. Drehung ausführen, falls die Ausrichtung geändert werden muss
    if (next_step.nextOrientation != currentOrientation) {
        // Bestimmen, ob eine Links- oder Rechtsdrehung nötig ist
        if (getNewOrientation(currentOrientation, TURN_LEFT) == next_step.nextOrientation) {
            Turn(left);
        } else if (getNewOrientation(currentOrientation, TURN_RIGHT) == next_step.nextOrientation) {
            Turn(right);
        }
        currentOrientation = next_step.nextOrientation; // Aktuelle Ausrichtung nach Drehung aktualisieren
    }

    // 5. Vorwärts zur nächsten Zelle bewegen
    MoveOneCell();

    // 6. Aktuelle Position des Roboters aktualisieren
    currentX = next_step.nextX;
    currentY = next_step.nextY;

    return true; // Schritt erfolgreich ausgeführt, Ziel noch nicht erreicht
}

// Bestimmt die neue Orientierung nach einer Drehung (relative zur aktuellen Orientierung)
Orientation getNewOrientation(Orientation current_orientation, TurnMouse turn) {
    if (turn == TURN_LEFT) {
        return (current_orientation == NORTH) ? WEST : (current_orientation - 1);
    } else if (turn == TURN_RIGHT) {
        return (current_orientation == WEST) ? NORTH : (current_orientation + 1);
    } else { // MOUSE_TURN_STRAIGHT
        return current_orientation;
    }
}

// Ermittelt den nächsten Schritt basierend auf der Distanzkarte für den kürzesten Pfad
ShortestPathMove getNextShortestPathMove(int currentX_param, int currentY_param, Orientation currentOrientation_param) {
    ShortestPathMove next_move = {currentX_param, currentY_param, currentOrientation_param}; // Initialisiere mit aktueller Position
    int min_dist = distanceMap[currentY_param][currentX_param]; // Starte mit der Distanz der aktuellen Zelle
    TurnMouse best_turn = TURN_STRAIGHT; // Bevorzuge geradeaus fahren
    int target_x_found = currentX_param; // temp Variable für die beste Ziel-X
    int target_y_found = currentY_param; // temp Variable für die beste Ziel-Y

    // 1. Geradeaus prüfen
    int forwardX = currentX_param;
    int forwardY = currentY_param;
    if (currentOrientation_param == NORTH) forwardY++;
    else if (currentOrientation_param == EAST) forwardX++;
    else if (currentOrientation_param == SOUTH) forwardY--;
    else if (currentOrientation_param == WEST) forwardX--;

    // Prüfen, ob die Zelle existiert und keine Wand dazwischen ist
    if (isValidCell(forwardX, forwardY) && !hasWallBetween(currentX_param, currentY_param, forwardX, forwardY)) {
        if (distanceMap[forwardY][forwardX] < min_dist) {
            min_dist = distanceMap[forwardY][forwardX];
            target_x_found = forwardX;
            target_y_found = forwardY;
            best_turn = TURN_STRAIGHT;
        }
    }

    // 2. Links prüfen (relativ zur aktuellen Orientierung)
    int leftX = currentX_param;
    int leftY = currentY_param;
    Orientation leftOrientation = getNewOrientation(currentOrientation_param, TURN_LEFT);
    if (leftOrientation == NORTH) leftY++;
    else if (leftOrientation == EAST) leftX++;
    else if (leftOrientation == SOUTH) leftY--;
    else if (leftOrientation == WEST) leftX--;

    if (isValidCell(leftX, leftY) && !hasWallBetween(currentX_param, currentY_param, leftX, leftY)) {
        if (distanceMap[leftY][leftX] < min_dist) {
            min_dist = distanceMap[leftY][leftX];
            target_x_found = leftX;
            target_y_found = leftY;
            best_turn = TURN_LEFT;
        }
    }

    // 3. Rechts prüfen (relativ zur aktuellen Orientierung)
    int rightX = currentX_param;
    int rightY = currentY_param;
    Orientation rightOrientation = getNewOrientation(currentOrientation_param, TURN_RIGHT);
    if (rightOrientation == NORTH) rightY++;
    else if (rightOrientation == EAST) rightX++;
    else if (rightOrientation == SOUTH) rightY--;
    else if (rightOrientation == WEST) rightX--;

    if (isValidCell(rightX, rightY) && !hasWallBetween(currentX_param, currentY_param, rightX, rightY)) {
        if (distanceMap[rightY][rightX] < min_dist) {
            min_dist = distanceMap[rightY][rightX];
            target_x_found = rightX;
            target_y_found = rightY;
            best_turn = TURN_RIGHT;
        }
    }

    // Setze die gefundenen Werte in die Rückgabe-Struktur.
    next_move.nextX = target_x_found;
    next_move.nextY = target_y_found;
    next_move.nextOrientation = getNewOrientation(currentOrientation_param, best_turn);

    return next_move;
}

