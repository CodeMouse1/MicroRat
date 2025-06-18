/*
 * maze.h
 *
 *  Created on: 19 May 2025
 *      Author: marcu
 */

#ifndef APPLIKATION_MAZE_H_
#define APPLIKATION_MAZE_H_

#include <stdbool.h>

#define MAZE_HEIGHT 4
#define MAZE_WIDTH 7

#define UNVISITED_DISTANCE 9999

typedef enum {
    NORTH,
    EAST,
    SOUTH,
    WEST
} Orientation;

extern int mazeMap[MAZE_HEIGHT][MAZE_WIDTH];
extern int distanceMap[MAZE_HEIGHT][MAZE_WIDTH];

void MazeMap_Init(void);
void updateMazeMap(int currentX, int currentY, int currentOrientation);
void calculateDistanceMap(int targetX, int targetY);
void printMazeMap(void);

bool isValidCell(int x, int y);
bool hasWallBetween(int x1, int y1, int x2, int y2);



#endif /* APPLIKATION_MAZE_H_ */
