/*
 * maze.h
 *
 *  Created on: 19 May 2025
 *      Author: marcu
 */

#ifndef APPLIKATION_MAZE_H_
#define APPLIKATION_MAZE_H_

#define MAZE_HEIGHT 4
#define MAZE_WIDTH 7

typedef enum {
    NORTH,
    EAST,
    SOUTH,
    WEST
} Orientation;

void MazeMap_Init(void);
void updateMazeMap(int currentX, int currentY, int currentOrientation);
void printMazeMap(void);

#endif /* APPLIKATION_MAZE_H_ */
