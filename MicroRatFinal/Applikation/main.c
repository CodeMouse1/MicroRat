#include "DAVE.h"

#include "Funktionsschnittstellen/start_condition.h"
#include "Funktionsschnittstellen/movement.h"
#include "Funktionsschnittstellen/sensors.h"
//#include "Motor.h"
//#include "Sensoren.h"
#include "UART.h"
#include "PID.h"

int main(void)
{
	DAVE_Init();           		   	/* Initialization of DAVE APPs */
	MovementInit();		   			/* Motoren auf Vorwärts eingestellt */
	SensorsInit();	  			   	/* Sensoren kalibriert und initialisiert */
	WaitForStart();

	while(true){
		//wallfollower();
		UART_Send();
	}
}



