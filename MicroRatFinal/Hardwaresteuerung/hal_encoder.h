/*
 * hal_encoder.h
 *
 *  Created on: 23 Apr 2025
 *      Author: marcu
 */

#ifndef HARDWARESTEUERUNG_HAL_ENCODER_H_
#define HARDWARESTEUERUNG_HAL_ENCODER_H_

void EncoderStartLeft(void);
void EncoderStartRight(void);
void EncoderReset(void);
void StartSensorTimer(void);

int EncoderReadLeft(void);
int EncoderReadRight(void);


#endif /* HARDWARESTEUERUNG_HAL_ENCODER_H_ */
