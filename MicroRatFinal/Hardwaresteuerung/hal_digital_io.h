/*
 * hal_digital_io.h
 *
 *  Created on: 18 Jun 2025
 *      Author: marcu
 */

#ifndef HARDWARESTEUERUNG_HAL_DIGITAL_IO_H_
#define HARDWARESTEUERUNG_HAL_DIGITAL_IO_H_

typedef enum {
    off,
    on,
    toggle
} LEDState;

void SetLEDs(LEDState left, LEDState right);

int GetButtonInput();


#endif /* HARDWARESTEUERUNG_HAL_DIGITAL_IO_H_ */
