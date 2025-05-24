/*
 * movement.h
 *
 *  Created on: 17 Apr 2025
 *      Author: marcu
 */

#ifndef FUNKTIONSSCHNITTSTELLEN_MOVEMENT_H_
#define FUNKTIONSSCHNITTSTELLEN_MOVEMENT_H_

#define ROBOT_WHEEL_BASE_MM 91.68f//89.12f
#define MM_PER_CELL_REFERENCE 170.0f // 16cm
#define DISTANCE_PER_90_DEGREE_MM (M_PI * ROBOT_WHEEL_BASE_MM / 4.0f)

//#define ENCODER_DIFF_90_DEGREE 251 //140.183 mm

#define MIN_WALL_DISTANCE 20.0f
#define MAX_MEASUREMENT_DISTANCE 1000.0f

typedef enum {
	none,
	left,
	right,
	around
} TurnDirection;

void MovementInit(void);
void MoveOneCell(void);
void Turn(TurnDirection direction);
void RecalibrateAndMoveForward(void);
float EstimateCellSize(void);

#endif /* FUNKTIONSSCHNITTSTELLEN_MOVEMENT_H_ */
