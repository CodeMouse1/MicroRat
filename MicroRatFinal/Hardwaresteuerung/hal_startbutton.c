
#include "DAVE.h"
#include "Hardwaresteuerung/hal_startbutton.h"

bool HAL_IsStartButtonPressed(void) {
    return DIGITAL_IO_GetInput(&DIGITAL_IO_BUTTON);
}

void HAL_StartStartDelay(void) {
    TIMER_Start(&TIMER_DELAY);
}

void HAL_StopStartDelay(void) {
    TIMER_Stop(&TIMER_DELAY);
}

void HAL_ClearStartDelayEvent(void) {
    TIMER_ClearEvent(&TIMER_DELAY);
}

void HAL_StartReglerTimer(void) {
    TIMER_Start(&TIMER_REGELUNG);
}

