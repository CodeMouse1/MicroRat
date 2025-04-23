/*
 * hal_encoder.h
 *
 *  Created on: 23 Apr 2025
 *      Author: marcu
 */

#ifndef HARDWARESTEUERUNG_HAL_ENCODER_H_
#define HARDWARESTEUERUNG_HAL_ENCODER_H_

void Encoder_Start_Left(void);
void Encoder_Start_Right(void);
int Encoder_Read_Left(void);
int Encoder_Read_Right(void);
void EncoderReset(void);
void StartSensorTimer(void);

#endif /* HARDWARESTEUERUNG_HAL_ENCODER_H_ */
