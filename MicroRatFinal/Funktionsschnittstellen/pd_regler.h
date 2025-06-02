
#ifndef FUNKTIONSSCHNITTSTELLEN_PD_REGLER_H_
#define FUNKTIONSSCHNITTSTELLEN_PD_REGLER_H_

#define PWM_MAX 3250
#define CYCLES_THRESHOLD 50
#define INTEGRAL_LIMIT 3000

#define KP_GLEICHLAUF 2000.0f
#define KP_TURN 37.5f//36.5f
#define KI_TURN 0.35f
#define KD_TURN 80.0f

extern float KP_STRAIGHT;
#define KI_STRAIGHT 0.26f
#define KD_STRAIGHT 400.0f//150.0f


extern volatile int isTurning;

void UpdatePID(void);
void setPIDGoalD(float distance_R, float distance_L);
void ResetPID(void);

int PIDdone(void);


#endif /* FUNKTIONSSCHNITTSTELLEN_PD_REGLER_H_ */
