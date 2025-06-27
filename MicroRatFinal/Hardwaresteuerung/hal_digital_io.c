/**
 * @file hal_digital_io.c
 * @brief Implementierung der Hardware-Abstraktionsschicht (HAL) für digitale Ein- und Ausgänge.
 *
 * Dieses Modul stellt die Schnittstelle zur direkten Steuerung von digitalen
 * Ausgängen (wie LEDs) und zum Auslesen von digitalen Eingängen (wie Tastern)
 * auf dem MicroRat bereit. Es kapselt die spezifischen HAL-Funktionen,
 * die vom DAVE-Framework zur Verfügung gestellt werden.
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 *
 * @dependencies
 * - "Dave.h": Das Haupt-Header-File des DAVE-Frameworks, das die notwendigen
 * Definitionen und Funktionsprototypen für die Peripheriesteuerung bereitstellt
 * - "Hardwaresteuerung/hal_digital_io.h": Deklarationen der öffentlichen Schnittstellen dieses Moduls
 */
#include "Dave.h"
#include "Hardwaresteuerung/hal_digital_io.h"

/**
 * @brief Steuert den Zustand der beiden LEDs der MicroRat.
 *
 * Diese Funktion setzt die Ausgabe-Pins für die LEDs basierend auf dem
 * gewünschten Zustand für die linke und rechte LED. Die Zustände werden
 * durch den 'LEDState'-Enum definiert.
 *
 * @param left Der gewünschte Zustand für die linke LED ('on', 'off', 'toggle').
 * @param right Der gewünschte Zustand für die rechte LED ('on', 'off', 'toggle').
 */
void SetLEDs(LEDState left, LEDState right){
	if(left == on && right == on){
		DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_1);
		DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_2);
	}else if (left == off && right == on){
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_AUGE_1);
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_AUGE_2);
	}else if (left == on && right == off){
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_AUGE_1);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_AUGE_2);
	}else if(left == on && right == on){
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_AUGE_1);
		DIGITAL_IO_SetOutputHigh(&DIGITAL_IO_AUGE_2);
	}else if(left == off && right == off){
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_AUGE_1);
		DIGITAL_IO_SetOutputLow(&DIGITAL_IO_AUGE_2);
	}else if(left == toggle && right == toggle){
		DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_1);
		DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_2);
	}
}

/**
 * @brief Liest den aktuellen Zustand des Start-Tasters aus.
 *
 * Diese Funktion greift auf die Hardware-Abstraktionsschicht des
 * digitalen I/Os zu, um den Zustand des Taster-Pins zu ermitteln.
 *
 * @return Der logische Zustand des Tasters:
 * Ein nicht-Null-Wert (typischerweise 1), wenn der Taster gedrückt ist,
 * und 0, wenn er nicht gedrückt ist (oder umgekehrt, je nach Hardware-Pull-Up/-Down).
 * Der genaue Rückgabewert hängt von der 'DIGITAL_IO_GetInput'-Implementierung ab.
 * @see DIGITAL_IO_BUTTON
 * @see DIGITAL_IO_GetInput
 */
int GetButtonInput(){
	return DIGITAL_IO_GetInput(&DIGITAL_IO_BUTTON);
}
