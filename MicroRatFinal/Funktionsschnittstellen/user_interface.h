/*
 * user_feedback.h
 *
 *  Created on: 18 Jun 2025
 *      Author: marcu
 */

#ifndef FUNKTIONSSCHNITTSTELLEN_USER_INTERFACE_H_
#define FUNKTIONSSCHNITTSTELLEN_USER_INTERFACE_H_

#include <stdbool.h>

void SignalTargetReached(void);
void SignalOptimisationComplete(void);
void SignalDiagnosticsResult(bool result);
void SignalOutOfBounds(void);

bool IsStartButtonPressed();

#endif /* FUNKTIONSSCHNITTSTELLEN_USER_INTERFACE_H_ */
