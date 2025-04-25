#include "DAVE.h"
#include "Applikation/pathfinding.h"
#include "Funktionsschnittstellen/start_condition.h"
#include "Funktionsschnittstellen/movement.h"
#include "Funktionsschnittstellen/sensors.h"

int main(void)
{
	DAVE_Init();           		   	/* Initialization of DAVE APPs */
	MovementInit();		   			/* Motoren auf Vorwärts eingestellt */
	SensorsInit();	  			   	/* Sensoren kalibriert und initialisiert */
	WaitForStart();


	while(true){
		DebugPrint();
		DrivePD();
		if(GetDistanceTravelled() >= 30.0){
				Stop();
				break;
			}
		//DrivePD();
		//wallfollower();

	}
}



