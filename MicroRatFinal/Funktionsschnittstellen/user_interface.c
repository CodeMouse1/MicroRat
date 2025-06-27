/**
 * @file user_interface.c
 * @brief Implementierung von Funktionen zur Benutzer-Rückmeldung (User Feedback).
 *
 * Dieses Modul stellt Funktionen zur Verfügung, um den Benutzer über
 * den Status des MicroRats zu informieren. Dies geschieht durch
 * akustische Signale (Motorsingen), visuelle Signale (LEDs)
 * und das Auslesen von Benutzereingaben (Taster).
 *
 * @author Marcus Stake Alvarado
 * @date 2025-06-26
 * @version 1.0
 *
 * @dependencies
 * - <stdbool.h>: Für den 'bool'-Datentyp
 * - "Funktionsschnittstellen/user_interface.h": Deklarationen der öffentlichen Schnittstellen dieses Moduls
 * - "Funktionsschnittstellen/system_interface.h": Für systemnahe Funktionen wie Verzögerungen
 * - "Hardwaresteuerung/hal_motor.h": Hardware-Abstraktionsschicht zur Steuerung der Motoren (für akustisches Feedback).
 * - "Hardwaresteuerung/hal_digital_io.h": Hardware-Abstraktionsschicht zur Steuerung von LEDs und zum Auslesen von Tastern.
 */
#include <stdbool.h>
#include "Funktionsschnittstellen/user_interface.h"
#include "Funktionsschnittstellen/system_interface.h"
#include "Hardwaresteuerung/hal_motor.h"
#include "Hardwaresteuerung/hal_digital_io.h"

/**
 * @brief Signalisiert das Erreichen eines Ziels durch eine kurze Motorpiepsen.
 *
 * Diese Funktion erzeugt eine kurze, zweimal wiederholte Vorwärtsbewegung
 * der Motoren, gefolgt von einer Pause, um dem Benutzer zu signalisieren,
 * dass der Roboter sein Ziel erreicht hat. Zusätzlich werden die LEDs getoggelt.
 * Die Verzögerungen sind hier als blockierende, leere Schleifen implementiert.
 */
void SignalTargetReached(){
	MotorsSetSpeed(1000, 1000);
	for (volatile int i = 0; i < 500000; i++) {}
	MotorsSetSpeed(0, 0);
	for (volatile int i = 0; i < 500000; i++) {}
	MotorsSetSpeed(1000, 1000);
	for (volatile int i = 0; i < 500000; i++) {}
	MotorsSetSpeed(0, 0);
	SetLEDs(toggle, toggle);
}

/**
 * @brief Signalisiert den Abschluss eines Optimierungsvorgangs.
 *
 * Diese Funktion gibt ein längeres, dreifaches "Bestätigungssignal"
 * über die Motoren aus, um anzuzeigen, dass ein Optimierungsprozess
 * abgeschlossen wurde. Abschließend wird eine längere Pause verwendet.
 *
 */
void SignalOptimisationComplete(){
	MotorsSetSpeed(1000, 1000);
	for (volatile int i = 0; i < 200000; i++) {}
	MotorsSetSpeed(0, 0);
	for (volatile int i = 0; i < 200000; i++) {}
	MotorsSetSpeed(1000, 1000);
	for (volatile int i = 0; i < 200000; i++) {}
	MotorsSetSpeed(0, 0);
	for (volatile int i = 0; i < 200000; i++) {}
	MotorsSetSpeed(1000, 1000);
	for (volatile int i = 0; i < 200000; i++) {}
	MotorsSetSpeed(0, 0);
	Delay_ms(2000);
}

/**
 * @brief Signalisiert das Ergebnis eines Diagnosetests an den Benutzer.
 *
 * Je nach Testergebnis gibt diese Funktion
 * ein spezifisches Feedback aus. Bei Erfolg gibt es eine kurzes piepsen.
 * Bei einem Fehler signalisieren die LEDs wechselnd `on` und `off` in einer
 * sehr langen, blockierenden Schleife.
 *
 * @param result Das Ergebnis des Diagnosetests ('true' für erfolgreich, 'false' für fehlgeschlagen).
 */
void SignalDiagnosticsResult(bool result){
	for (volatile int i = 0; i < 1000000; i++) {}
	if(result == true){
		MotorsSetSpeed(1000, 1000);
		for (volatile int i = 0; i < 1000000; i++) {}
		MotorsSetSpeed(0, 0);

	}else if(result == false){
		SetLEDs(on,off);
		for (volatile int i = 0; i < 7500000; i++) {}
		SetLEDs(off,on);
		for (volatile int i = 0; i < 7500000; i++) {}
	}
}

/**
 * @brief Signalisiert einen "Außerhalb des Bereichs"-Zustand durch blinkende LEDs.
 *
 * Diese Funktion geht in eine Endlosschleife und lässt die LEDs abwechselnd
 * an- und ausgehen, um einen kritischen Zustand zu signalisieren.
 *
 * @note Dies ist eine **Endlosschleife** ('while(1)'), die das Programm an dieser
 * Stelle dauerhaft blockiert.
 */
void SignalOutOfBounds(){
	while(1){
		SetLEDs(on,off);
		for (volatile int i = 0; i < 2000000; i++) {}
		SetLEDs(off,on);
		for (volatile int i = 0; i < 2000000; i++) {}
	}
}

/**
 * @brief Prüft, ob der Start-Taster gedrückt wurde.
 *
 * Diese Funktion liest den aktuellen Status des Start-Tasters über die
 * Hardware-Abstraktionsschicht aus.
 *
 * @return 'true', wenn der Start-Taster gedrückt ist; 'false' sonst.
 * @see GetButtonInput
 */
bool IsStartButtonPressed(){
	return GetButtonInput();
}




