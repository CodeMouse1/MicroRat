
#include "DAVE.h"
#include "Hardwaresteuerung/hal_startbutton.h"
#include <stdbool.h>

uint32_t DelayTimer;

uint8_t UART_String[100];


bool HAL_IsStartButtonPressed(void) {
    return DIGITAL_IO_GetInput(&DIGITAL_IO_BUTTON);
}

void HAL_StartStartDelay(int delay) {
	TIMER_SetTimeInterval(&TIMER_DELAY, delay *100);
    TIMER_Start(&TIMER_DELAY);
}

void HAL_StopStartDelay(void) {
    TIMER_Stop(&TIMER_DELAY);
}

void HAL_ClearStartDelayEvent(void) {
    TIMER_ClearEvent(&TIMER_DELAY);
}


void HAL_DelayTimer(int delay_us){
	DelayTimer = SYSTIMER_CreateTimer(delay_us, SYSTIMER_MODE_ONE_SHOT, (void*)DelayRoutine, NULL);	//Falls Probleme Delay wieder PERIODIC!
		SYSTIMER_StartTimer(DelayTimer);
		uint32_t start_time = SYSTIMER_GetTime();
		while ((SYSTIMER_GetTime() - start_time) < delay_us)
			{
				// Busy-Wait (blockierend)
			}
}
void DelayRoutine(){

	SYSTIMER_StopTimer(DelayTimer);
	SYSTIMER_DeleteTimer(DelayTimer);
}

