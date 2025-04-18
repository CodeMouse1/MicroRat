/*
 * Sensoren.h
 *
 *  Created on: 31 Mar 2025
 *      Author: marcu
 */

#ifndef SENSOREN_H_
#define SENSOREN_H_

extern float distanz_ultra;
extern int IR_L, IR_R;
extern int count_L, count_R;

//Initialisiert die Sensoren
void Sensoren_Init(void);

void Sensoren_Auslesen_50ms(void);

void Encoder_Get_Speed(void);

#endif /* SENSOREN_H_ */
