/**
 * @file hal_digital_io.h
 * @brief Deklarationen der Hardware-Abstraktionsschicht (HAL) für digitale Ein- und Ausgänge.
 *
 * Dieses Header-File definiert die öffentliche Schnittstelle für die Steuerung
 * von digitalen Ausgängen (wie LEDs) und das Auslesen von digitalen Eingängen
 * (wie Tastern). Es stellt die notwendigen Typdefinitionen und Funktionsprototypen
 * bereit, um eine hardwareunabhängige Interaktion mit diesen Peripheriegeräten zu ermöglichen.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 */
#ifndef HARDWARESTEUERUNG_HAL_DIGITAL_IO_H_
#define HARDWARESTEUERUNG_HAL_DIGITAL_IO_H_

/**
 * @brief Enumeration zur Definition der möglichen Zustände einer LED.
 *
 * Diese Enumeration wird verwendet, um den gewünschten Zustand für eine LED
 * anzugeben: Aus, Ein oder Umschalten des aktuellen Zustands.
 */
typedef enum {
    off,
    on,
    toggle
} LEDState;

/**
 * @brief Steuert den Zustand der beiden LEDs der MicroRat.
 *
 * Diese Funktion setzt die digitalen Ausgänge, die mit den LEDs verbunden sind,
 * entsprechend den übergebenen Zuständen für die linke und rechte LED.
 *
 * @param left Der gewünschte Zustand für die linke LED ('off', 'on', oder 'toggle').
 * @param right Der gewünschte Zustand für die rechte LED ('off', 'on', oder 'toggle').
 */
void SetLEDs(LEDState left, LEDState right);

/**
 * @brief Liest den aktuellen Zustand eines digitalen Eingangstasters.
 *
 * Diese Funktion fragt den Status des Buttons ab, der für Benutzereingaben
 * vorgesehen ist. Der Rückgabewert hängt von der hardwarenahen Implementierung
 * und der Verdrahtung des Tasters ab (z.B. ob ein Pull-Up oder Pull-Down verwendet wird).
 *
 * @return Der logische Zustand des Tasters: 0 (LOW) oder 1 (HIGH).
 * Typischerweise bedeutet 1, dass der Taster gedrückt ist, kann aber je nach Schaltung variieren.
 */
int GetButtonInput();


#endif /* HARDWARESTEUERUNG_HAL_DIGITAL_IO_H_ */
