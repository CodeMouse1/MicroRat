#include "DAVE.h"
#include "stdio.h"
#include "Applikation/maze.h"
#include "Applikation/pathfinding.h"
#include "Applikation/state_machine.h"
#include "Funktionsschnittstellen/start_condition.h"
#include "Funktionsschnittstellen/movement.h"
#include "Funktionsschnittstellen/sensors.h"
#include "Hardwaresteuerung/hal_encoder.h"
#include "Hardwaresteuerung/hal_motor.h"

// Globale Variablen für Mausposition und -ausrichtung
int currentX = 0;
int currentY = 0;
int targetX = 6;
int targetY = 3;
Orientation currentOrientation = EAST;

int main(void)
{
	DAVE_Init();           		   	/* Initialization of DAVE APPs */
	MovementInit();		   			/* Motoren auf Vorwärts eingestellt */
	SensorsInit();					/* Sensoren initialisiert */

	/*while(1){
		DebugPrint();

	}*/

	//WaitForStart();

	//MazeMap_Init();

	//updateMazeMap(currentX, currentY, currentOrientation); // Initialisiere die Karte mit der Startposition
	//printMazeMap();	-- kein muss

	while(true){
		//MotorsDrive();
		//wallfollower(WALLFOLLOW_RIGHT);
		RatStateMachine_Update();
	}
}

/*void TestMotorDifference() {

    EncoderReset();
    MotorsSetForward();

    // Beide Motoren mit GLEICHER PWM
    MotorsSetSpeed(3000, 3000);

    // 3 Sekunden fahren lassen
    for (volatile int i = 0; i < 15000000; i++) {}

    MotorsSetSpeed(0, 0);

    // Encoder auslesen
    int encoder_L = EncoderReadLeft();
    int encoder_R = EncoderReadRight();

    // Berechne Kalibrierungsfaktor
    float calibration_factor = (float)encoder_L / encoder_R;

    DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_2); // Signal: Test beendet

    sprintf((char*)UART_String,	"Calibration Factor: %.3f\r\n", calibration_factor);
	UART_Transmit(&UART_COM, UART_String, sizeof(UART_String));

}*/




