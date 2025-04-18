#include "DAVE.h"
#include "Motor.h"
#include "Sensoren.h"
#include "UART.h"
#include "PID.h"
//test
int already_turned =0;
int start = 0;
int button_status = 0;
bool button_pressed = false;

extern int IR_L, IR_R;
extern int count_L, count_R;

void Start_Button(){
	Motor_Set_Speed(3000, 3000);
	button_pressed = true;
	TIMER_ClearEvent(&TIMER_DELAY);
	TIMER_Stop(&TIMER_DELAY);
	start = 1;
		//DIGITAL_IO_ToggleOutput(&DIGITAL_IO_AUGE_1);	Erprobung neuer Timer Interval
		//TIMER_SetTimeInterval(&TIMER_0, 100000000U);
		//TIMER_Start(&TIMER_0);
	TIMER_Start(&TIMER_REGELUNG);
}


int main(void)
{
	DAVE_Init();           /* Initialization of DAVE APPs */
	Motor_Init();		   /* Motoren auf Vorwärts eingestellt */
	Sensoren_Init();	   /* Sensoren kalibriert und initialisiert */

	while(true){
	button_status = DIGITAL_IO_GetInput(&DIGITAL_IO_BUTTON);
		if(button_status == 1 && !button_pressed){
			TIMER_Start(&TIMER_DELAY);
		}
		if(start == 1){
			break;
		}
	}
	while(true){
		UART_Send();
		if(count_L + count_R == 1000){
			TIMER_Stop(&TIMER_REGELUNG);
			Motor_Stop();
			if (!already_turned) {
				while(1){
					Motor_Turn(2);
				}
			}
		}


		/*if(IR_L >= 2000 || IR_R >= 2000){		Erprobung Reaktion auf IR Werte
			Motor_Stop();
		}else{
			Motor_Init();
		}*/
	}
}



