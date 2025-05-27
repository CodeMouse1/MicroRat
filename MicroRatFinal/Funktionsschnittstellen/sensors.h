/*
 * sensors.h
 *
 *  Created on: 23 Apr 2025
 *      Author: marcu
 */

#ifndef FUNKTIONSSCHNITTSTELLEN_SENSORS_H_
#define FUNKTIONSSCHNITTSTELLEN_SENSORS_H_

#define WALL_DETECTION_THRESHOLD 60	// 60mm

#define WHEEL_RADIUS 16 // mm
#define ENCODER_TICKS_PER_REV 180  // 180 Ticks/Umdrehung
#define WHEEL_CIRCUMFERENCE (2 * 3.14159 * WHEEL_RADIUS) // ≈ 100.53 mm
#define TICK_PER_MM (WHEEL_CIRCUMFERENCE / ENCODER_TICKS_PER_REV) // ≈ 0.5585 mm

#define ADC_MAX_VALUE   4095    // 12-Bit-ADC (2^12 - 1)
#define ADC_REF_VOLTAGE 3.3    // 3.3V Referenzspannung des XMC

#define IR_SENSOR_OFFSET_LEFT_TO_WHEEL_MM 14
#define IR_SENSOR_OFFSET_RIGHT_TO_WHEEL_MM 18

void SensorsInit(void);
void DebugPrint(void);

bool IsWallLeft(void);
bool IsWallRight(void);
bool IsWallFront(void);

float GetEncoderLeft_mm(void);
float GetEncoderRight_mm(void);
float GetDistanceFront_mm(void);

int GetDistanceLeft_mm(void);
int GetDistanceRight_mm(void);


#endif /* FUNKTIONSSCHNITTSTELLEN_SENSORS_H_ */
