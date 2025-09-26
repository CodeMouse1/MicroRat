/**
 * @file Applikation/state_machine.h
 * @brief Deklarationen und Typen für die Zustandsmaschine der MicroRat.
 *
 * Diese Header-Datei definiert die verschiedenen Betriebszustände der MicroRat
 * sowie Enumerationen für Bewegungsbefehle. Sie stellt auch die globale Variable
 * für den aktuellen Zustand und die Hauptfunktion zur Zustandsaktualisierung bereit.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 */
#ifndef APPLIKATION_STATE_MACHINE_H_
#define APPLIKATION_STATE_MACHINE_H_

/**
 * @def NO_WALL_TIMEOUT_THRESHOLD
 * @brief Schwellenwert für das "keine-Wand"-Timeout in der Erkundungsphase.
 *
 * Wenn der MicroRat diesen Schwellenwert an aufeinanderfolgenden Zellen erreicht,
 * in denen keine Wände in alle drei Richtungen erkannt werden, wird ein
 * "Out-of-Bounds"-Fehler signalisiert.
 */
#define NO_WALL_TIMEOUT_THRESHOLD 2

/**
 * @brief Enumeration der möglichen Betriebszustände der MicroRat
 */
typedef enum {
	STATE_IDLE,
	STATE_EXPLORE,
	STATE_WAIT_REPORT,
	STATE_SHORTEST_PATH
} RatState;

/**
 * @brief Enumeration der relativen Drehbewegungen, die die MicroRat ausführen kann
 */
typedef enum {
    TURN_STRAIGHT = 0,
    TURN_LEFT,
    TURN_RIGHT
} TurnMouse;

// Externe Deklaration der globalen Zustandsvariablen für die MicroRat
extern volatile RatState currentState;

/**
 * @brief Aktualisiert den Zustand der MicroRat-Zustandsmaschine.
 *
 * Diese Funktion wird in regelmäßigen Abständen aufgerufen,
 * um die Logik der Zustandsmaschine voranzutreiben und die entsprechenden
 * Aktionen basierend auf dem 'currentState' auszuführen.
 */
void RatStateMachine_Update(void);


#endif /* APPLIKATION_STATE_MACHINE_H_ */
