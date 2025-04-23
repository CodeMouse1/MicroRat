/*
 * sensors.h
 *
 *  Created on: 23 Apr 2025
 *      Author: marcu
 */

#ifndef FUNKTIONSSCHNITTSTELLEN_SENSORS_H_
#define FUNKTIONSSCHNITTSTELLEN_SENSORS_H_

void SensorsInit(void);
void SensorsRead(void);

bool IsWallLeft(void);
bool IsWallRight(void);
bool IsWallFront(void);
int GetIRRight();
int GetIRLeft();
float GetUltraschall();
int GetEncoderLeft(void);
int GetEncoderRight(void);

#endif /* FUNKTIONSSCHNITTSTELLEN_SENSORS_H_ */
