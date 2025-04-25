/*
 * sensors.h
 *
 *  Created on: 23 Apr 2025
 *      Author: marcu
 */

#ifndef FUNKTIONSSCHNITTSTELLEN_SENSORS_H_
#define FUNKTIONSSCHNITTSTELLEN_SENSORS_H_

void SensorsInit(void);
void DebugPrint(void);

bool IsWallLeft(void);
bool IsWallRight(void);
bool IsWallFront(void);

int GetIRRight(void);
int GetIRLeft(void);
int GetEncoderLeft(void);
int GetEncoderRight(void);

float GetUltraschall(void);
float GetDistanceTravelled(void);

#endif /* FUNKTIONSSCHNITTSTELLEN_SENSORS_H_ */
