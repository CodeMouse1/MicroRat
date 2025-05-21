/*
 * PD.h
 *
 *  Created on: 20 May 2025
 *      Author: marcu
 */

#ifndef FUNKTIONSSCHNITTSTELLEN_PD_H_
#define FUNKTIONSSCHNITTSTELLEN_PD_H_

void UpdatePID(void);
void ResetPID(void);
void setPIDGoalD(int distance);
void setPIDGoalA(int angle);
int PIDdone(void);
int PIDdoneTurn(void);

#endif /* FUNKTIONSSCHNITTSTELLEN_PD_H_ */
