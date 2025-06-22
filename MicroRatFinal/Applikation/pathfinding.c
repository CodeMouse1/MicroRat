/*
 * @file pathfinding.c
 * @brief Verwaltet die Labyrinth-Navigationsalgorithmen.
 *
 * Dieses Modul ist verantwortlich für die Berechnung der Distanzkarten,
 * das Planen von Bewegungen entlang des kürzesten Pfades und die Implementierung
 * des Wall Follower-Algorithmus. Es nutzt Sensordaten zur Umgebungserkennung und
 * steuert die Aktuatoren des Roboters (Bewegungsmodul) zur Ausführung der geplanten Aktionen.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-22
 * @version 1.0
 *
 * @dependencies
 * - Applikation/maze.h: Für Labyrinth-Karten ('mazeMap', 'distanceMap') und Wandabfragen ('isValidCell', 'hasWallBetween').
 * - Applikation/pathfinding.h: Deklarationen für dieses Modul, insbesondere die 'Cell' Struktur und 'ShortestPathMove'.
 * - Applikation/state_machine.h: Für die globale Position ('currentX', 'currentY') und Orientierung ('currentOrientation') der Rat.
 * - Funktionsschnittstellen/sensors.h: Für die Abfrage der Wandsensoren ('IsWallFront', 'IsWallLeft', 'IsWallRight').
 * - Funktionsschnittstellen/movement.h: Für die Steuerung der Roboterbewegung ('Turn', 'MoveOneCell', 'MoveMultipleCells', 'Stop').
 */
#include <stdbool.h>
#include "Applikation/maze.h"
#include "Applikation/pathfinding.h"
#include "Applikation/state_machine.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Funktionsschnittstellen/movement.h"

/*
 * @brief Eine statische Queue, die für den Flood Fill Algorithmus verwendet wird.
 * Speichert Zellen, die noch verarbeitet werden müssen.
 */
static Cell queue[QUEUE_SIZE];

static int head = 0; // Zeigt auf das nächste Element zum Entfernen
static int tail = 0; // Zeigt auf die nächste freie Position zum Hinzufügen

/*
 * @brief Fügt eine Zelle zur Queue hinzu (Enqueue-Operation).
 * Implementiert eine zirkuläre Queue.
 * @param x Die X-Koordinate der hinzuzufügenden Zelle.
 * @param y Die Y-Koordinate der hinzuzufügenden Zelle.
 */
static void enqueue(int x, int y) {
    // Prüfen, ob die Queue nicht voll ist, bevor ein Element hinzugefügt wird.
    if ((tail + 1) % QUEUE_SIZE != head) {
        queue[tail].x = x;
        queue[tail].y = y;
        tail = (tail + 1) % QUEUE_SIZE;
    }
}

/*
 * @brief Entfernt eine Zelle vom Kopf der Queue (Dequeue-Operation).
 * Implementiert eine zirkuläre Queue.
 * @return Die vom Kopf der Queue entfernte Zelle.
 */
static Cell dequeue(void) {
    Cell cell = queue[head];
    head = (head + 1) % QUEUE_SIZE;
    return cell;
}

/*
 * @brief Prüft, ob die Queue leer ist.
 * @return true, wenn die Queue leer ist; false sonst.
 */
static bool isQueueEmpty(void) {
    return head == tail;
}

static const int dx[] = {0, 1, 0, -1}; // delta-x für Nord, Ost, Süd, West
static const int dy[] = {1, 0, -1, 0}; // delta-y für Nord, Ost, Süd, West

/*
 * @brief Aktualisiert die globale Orientierung der Rat nach einer Drehung.
 * Passt 'currentOrientation' entsprechend der Drehrichtung an.
 * @param direction Die Richtung der Drehung (right, left, around).
 */
static void updateOrientation(TurnDirection direction) {
    switch (direction) {
        case right:	// Drehung um 90 Grad im Uhrzeigersinn
            if (currentOrientation == NORTH) currentOrientation = EAST;
            else if (currentOrientation == EAST) currentOrientation = SOUTH;
            else if (currentOrientation == SOUTH) currentOrientation = WEST;
            else if (currentOrientation == WEST) currentOrientation = NORTH;
            break;
        case left:	// Drehung um 90 Grad gegen den Uhrzeigersinn
            if (currentOrientation == NORTH) currentOrientation = WEST;
            else if (currentOrientation == WEST) currentOrientation = SOUTH;
            else if (currentOrientation == SOUTH) currentOrientation = EAST;
            else if (currentOrientation == EAST) currentOrientation = NORTH;
            break;
        case around: // Drehung um 180 Grad
            if (currentOrientation == NORTH) currentOrientation = SOUTH;
            else if (currentOrientation == EAST) currentOrientation = WEST;
            else if (currentOrientation == SOUTH) currentOrientation = NORTH;
            else if (currentOrientation == WEST) currentOrientation = EAST;
            break;
        default:
            break;
    }
}

/*
 * @brief Aktualisiert die globale Position der Rat und die Labyrinthkarte.
 * Bewegt die Rat logisch eine Zelle vorwärts basierend auf seiner aktuellen Orientierung
 * und ruft dann 'updateMazeMap' auf, um die Karte mit neuen Sensordaten zu aktualisieren.
 */
static void updatePositionAndMap() {
    // Aktuelle Roboterposition basierend auf der Orientierung anpassen.
    if (currentOrientation == NORTH) currentY++;
    else if (currentOrientation == EAST) currentX++;
    else if (currentOrientation == SOUTH) currentY--;
    else if (currentOrientation == WEST) currentX--;
    // Rufe die Funktion von maze.c auf, um die Karte zu aktualisieren
    updateMazeMap(currentX, currentY, currentOrientation);
}

/*
 * @brief Implementiert den Wall Follower Algorithmus (Rechte-Hand- oder Linke-Hand-Regel).
 * Die Rat versucht, eine Wand auf einer bestimmten Seite zu folgen, um das Labyrinth zu erkunden.
 * Die Funktion führt eine Bewegung und ggf. eine Drehung aus und aktualisiert die Roboterposition und Karte.
 * @param mode Der Wallfollow-Modus (WALLFOLLOW_RIGHT für Rechte-Hand-Regel, WALLFOLLOW_LEFT für Linke-Hand-Regel).
 */
void wallfollower(WallfollowMode mode) {
    TurnDirection turnDirection = none;	// Standardmäßig keine Drehung
	if (mode == WALLFOLLOW_RIGHT) {	// Rechte-Hand-Regel
		if (!IsWallRight()) {
			turnDirection = right;	// Wenn rechts frei, nach rechts drehen
		}
		else if (IsWallFront() && IsWallRight() && !IsWallLeft()) {	// Wenn rechts und vorne blockiert, links abbiegen
			turnDirection = left;
		} else if (IsWallFront() && IsWallRight() && IsWallLeft()) {	// Wenn alle drei Seiten blockiert, 180-Grad-Drehung
			turnDirection = around;
		}
	} else if (mode == WALLFOLLOW_LEFT) {	// Linke-Hand-Regel
		if (!IsWallLeft()) {
			turnDirection = left;	// Wenn links frei, nach links drehen
		}
		else if (IsWallFront() && IsWallLeft() && !IsWallRight()) {	// Wenn links und vorne blockiert, rechts abbiegen
			turnDirection = right;
		} else if (IsWallFront() && IsWallRight() && IsWallLeft()) {	// Wenn alle drei Seiten blockiert, 180-Grad-Drehung
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

/*
 * @brief Berechnet die Distanzkarte zum Ziel mit dem Flood Fill Algorithmus.
 * Füllt die 'distanceMap' mit der minimalen Anzahl von Schritten von jeder erreichbaren Zelle
 * zum angegebenen Ziel. Zellen, die nicht erreichbar sind, behalten den Wert 'UNVISITED_DISTANCE'.
 * @param targetX_param Die X-Koordinate der Zielzelle.
 * @param targetY_param Die Y-Koordinate der Zielzelle.
 */
void calculateDistanceMap(int targetX_param, int targetY_param) {
    // 1. Distanzkarte initialisieren und Queue leeren
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            distanceMap[y][x] = UNVISITED_DISTANCE; // Alle Zellen als "unerreichbar" markieren
        }
    }
    head = 0;	// Queue-Kopf zurücksetzen
    tail = 0;	// Queue-Ende zurücksetzen

    // 2. Zielzelle initialisieren und zur Queue hinzufügen
    if (isValidCell(targetX_param, targetY_param)) { // Prüfen, ob das Ziel gültig ist
        distanceMap[targetY_param][targetX_param] = 0; // Zielzelle hat Distanz 0
        enqueue(targetX_param, targetY_param);         // // Zielzelle als Startpunkt der Breitensuche zur Queue hinzufügen
    }

    // 3. Flood Fill Algorithmus (Breitensuche - BFS)
    while (!isQueueEmpty()) {	// Solange noch Zellen in der Queue sind, die verarbeitet werden müssen
        Cell currentCell = dequeue(); // Aktuelle Zelle aus der Queue nehmen
        int currentDist = distanceMap[currentCell.y][currentCell.x]; // Die Distanz dieser Zelle zum Ziel
        // Durchlaufe alle 4 Nachbarrichtungen (N, E, S, W)
        for (int i = 0; i < 4; i++) {
            int nextX = currentCell.x + dx[i]; // X-Koordinate des Nachbarn
            int nextY = currentCell.y + dy[i]; // Y-Koordinate des Nachbarn
            // Prüfen, ob der Nachbar gültig ist (innerhalb des Labyrinths)
			// UND ob keine bekannte Wand zwischen der aktuellen Zelle und dem Nachbarn ist
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

/*
 * @brief Führt einen einzelnen Schritt auf dem kürzesten Pfad aus.
 * Diese Funktion ermittelt den nächsten optimalen Schritt (Zelle + Orientierung),
 * führt die notwendige Drehung und eine einzelne Vorwärtsbewegung aus.
 *
 * @return true, wenn ein Schritt erfolgreich ausgeführt wurde und das Ziel noch nicht erreicht ist;
 * false, wenn das Ziel erreicht ist oder kein gültiger nächster Schritt gefunden wurde.
 */
bool executeShortestPathStep(void) {
    // 1. Prüfen, ob das Ziel bereits erreicht ist (Distanz 0)
    if (distanceMap[currentY][currentX] == 0) {
        Stop();
        return false; // Ziel erreicht, kein weiterer Schritt nötig
    }
    // 2. Den nächsten besten Zug basierend auf der Distanzkarte finden
    ShortestPathMove next_step = getNextShortestPathMove(currentX, currentY, currentOrientation);
    // 3. Fehlerprüfung: Wenn kein Fortschritt erzielt werden kann
    if (next_step.nextX == currentX && next_step.nextY == currentY && next_step.nextOrientation == currentOrientation) {
        Stop();
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

/*
 * @brief Führt einen optimierten Schritt entlang des kürzesten Pfades aus (Work In Progress).
 * Diese erweiterte Funktion dreht den Roboter bei Bedarf und fährt dann die längstmögliche
 * gerade Strecke, die noch Teil des kürzesten Pfades ist, um die Effizienz zu steigern.
 *
 * @return true, wenn ein Schritt (mindestens eine Zelle) erfolgreich ausgeführt wurde und das Ziel
 * noch nicht erreicht ist; false, wenn das Ziel erreicht ist oder kein weiterer Schritt
 * möglich ist (z.B. Roboter ist festgefahren).
 */
/*bool executeShortestPathStep(void) {
    // 1. Prüfen, ob das Ziel bereits erreicht ist (Distanz 0)
    if (distanceMap[currentY][currentX] == 0) {
        Stop(); // Roboter anhalten
        return false; // Ziel erreicht, kein weiterer Schritt nötig
    }

    // 2. Den optimalen ersten Schritt (erste Zelle im Pfad) ermitteln.
    // Diese Funktion liefert die Koordinaten und Orientierung der ERSTEN Zelle im optimalen Pfad.
    ShortestPathMove next_initial_step = getNextShortestPathMove(currentX, currentY, currentOrientation);

    // Fehlerprüfung: Falls kein gültiger Schritt gefunden wurde (z.B. Roboter ist stecken geblieben, obwohl Distanz > 0).
    if (next_initial_step.nextX == currentX && next_initial_step.nextY == currentY && next_initial_step.nextOrientation == currentOrientation) {
        Stop(); // Roboter anhalten
        // Optional: Hier könnte eine Debug-Meldung gesendet werden, um den Fehler zu protokollieren.
        return false; // Fehler: Kein gültiger nächster Schritt gefunden oder festgefahren.
    }

    // 3. Bestimmen, welche Drehung benötigt wird, um sich auf den ersten Schritt auszurichten.
    TurnDirection turn_to_make = none;
    // Differenz zwischen gewünschter und aktueller Orientierung berechnen (Modulo 4 für Richtungen).
    int delta_orientation = (next_initial_step.nextOrientation - currentOrientation + 4) % 4;

    if (delta_orientation == 1) { // delta_orientation 1 bedeutet 90 Grad rechts (z.B. N->O)
        turn_to_make = right;
    } else if (delta_orientation == 3) { // delta_orientation 3 bedeutet 90 Grad links (z.B. N->W)
        turn_to_make = left;
    } else if (delta_orientation == 2) { // delta_orientation 2 bedeutet 180 Grad Drehung (z.B. N->S)
        turn_to_make = around;
    }
    // Wenn delta_orientation 0 ist, bleibt turn_to_make 'none' (keine Drehung nötig).

    // 4. Die benötigte Drehung ausführen.
    if (turn_to_make != none) {
        Turn(turn_to_make); // Die Drehung wird physisch ausgeführt.
        currentOrientation = next_initial_step.nextOrientation; // Globale Orientierung des Roboters aktualisieren.
    }

    // 5. Jetzt, da der Roboter richtig ausgerichtet ist, die längstmögliche gerade Strecke finden.
    // Dies optimiert die Bewegung, indem mehrere Zellen in einem Zug gefahren werden.
    int cellsToDriveStraight = 0;
    int tempX = currentX; // Temporäre X-Koordinate zum "Vorausgucken" entlang des Pfades.
    int tempY = currentY; // Temporäre Y-Koordinate zum "Vorausgucken" entlang des Pfades.
    // Die erwartete Distanz zur Zielzelle der Zelle, in der wir uns GERADE befinden (beginnend mit der aktuellen Zelle).
    int expected_dist_in_path = distanceMap[currentY][currentX];

    // Schleife, um jede nachfolgende Zelle in der geraden Linie zu prüfen, die Teil des kürzesten Pfades ist.
    while (true) {
        int nextX_in_chain = tempX;
        int nextY_in_chain = tempY;

        // Berechne die Koordinaten der nächsten Zelle, wenn wir geradeaus fahren würden.
        if (currentOrientation == NORTH) nextY_in_chain++;
        else if (currentOrientation == EAST) nextX_in_chain++;
        else if (currentOrientation == SOUTH) nextY_in_chain--;
        else if (currentOrientation == WEST) nextX_in_chain--;

        // Prüfe drei Bedingungen, um zu entscheiden, ob die nächste Zelle zur geraden Strecke gehört:
        // a) Ist die Zelle innerhalb des Labyrinths gültig?
        // b) Gibt es KEINE Wand zwischen der aktuellen temporären Zelle und der nächsten Zelle in der Kette?
        // c) Ist die Distanz der nächsten Zelle zum Ziel genau 'expected_dist_in_path - 1' (d.h. sie ist Teil des optimalen Pfades)?
        if (isValidCell(nextX_in_chain, nextY_in_chain) &&
            !hasWallBetween(tempX, tempY, nextX_in_chain, nextY_in_chain) &&
            (distanceMap[nextY_in_chain][nextX_in_chain] == expected_dist_in_path - 1)) {
            // Ja, diese Zelle gehört zur geraden Strecke!
            cellsToDriveStraight++; // Zähler für geradeaus zu fahrende Zellen erhöhen.
            tempX = nextX_in_chain; // Temporäre Position zur nächsten Zelle verschieben.
            tempY = nextY_in_chain;
            expected_dist_in_path--; // Die erwartete Distanz zum Ziel verringert sich um 1.
        } else {
            // Die gerade Strecke ist zu Ende (entweder durch eine Wand, Labyrinthrand oder der Pfad biegt ab).
            break; // Schleife beenden.
        }
    }

    // 6. Die ermittelte gerade Strecke fahren.
    if (cellsToDriveStraight > 0) {
        MoveMultipleCells(cellsToDriveStraight); // Rufe die Bewegung-Funktion auf, um mehrere Zellen zu fahren.
        // Die tatsächliche globale Position des Roboters nach der Fahrt aktualisieren.
        currentX = tempX;
        currentY = tempY;
    } else {
        // Dieser Fall sollte nur eintreten, wenn 'getNextShortestPathMove' keinen Fortschritt signalisiert,
        // aber 'distanceMap[currentY][currentX]' immer noch > 0 ist, was auf einen Logikfehler oder eine Sackgasse hindeutet.
        Stop();
        return false;
    }

    return true; // Ein Schritt wurde erfolgreich ausgeführt, das Ziel ist noch nicht erreicht.
}*/

/*
 * @brief Berechnet die neue Orientierung basierend auf der aktuellen Orientierung und einer Drehanweisung.
 * @param current_orientation Die aktuelle Orientierung der Rat.
 * @param turn Die relative Drehanweisung (TURN_LEFT, TURN_RIGHT, TURN_STRAIGHT).
 * @return Die neue Orientierung nach der Drehung.
 */
Orientation getNewOrientation(Orientation current_orientation, TurnMouse turn) {
    if (turn == TURN_LEFT) {
        return (current_orientation == NORTH) ? WEST : (current_orientation - 1);
    } else if (turn == TURN_RIGHT) {
        return (current_orientation == WEST) ? NORTH : (current_orientation + 1);
    } else { // MOUSE_TURN_STRAIGHT
        return current_orientation;
    }
}

 /*
  * @brief Ermittelt den nächsten optimalen Schritt im kürzesten Pfad basierend auf der Distanzkarte.
  * Die Funktion prüft die Nachbarzellen (geradeaus, links, rechts) und wählt diejenige mit der
  * niedrigsten Distanz zum Ziel, die erreichbar ist. Präferiert wird geradeaus, dann links, dann rechts.
  * @param currentX_param Die aktuelle X-Koordinate des Roboters.
  * @param currentY_param Die aktuelle Y-Koordinate des Roboters.
  * @param currentOrientation_param Die aktuelle Ausrichtung des Roboters.
  * @return Eine ShortestPathMove-Struktur, die die Koordinaten und Orientierung der nächsten Zelle im Pfad enthält.
  */
ShortestPathMove getNextShortestPathMove(int currentX_param, int currentY_param, Orientation currentOrientation_param) {
    // Initialisiere den nächsten Schritt mit der aktuellen Position, falls kein besserer gefunden wird (Fehlerfall/Ziel erreicht).
    ShortestPathMove next_move = {currentX_param, currentY_param, currentOrientation_param};
    // Die minimale Distanz ist zunächst die Distanz der aktuellen Zelle. Jeder gültige Nachbar muss eine kleinere Distanz haben.
    int min_dist = distanceMap[currentY_param][currentX_param];
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

