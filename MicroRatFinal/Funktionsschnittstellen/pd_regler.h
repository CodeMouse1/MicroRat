
#ifndef FUNKTIONSSCHNITTSTELLEN_PD_REGLER_H_
#define FUNKTIONSSCHNITTSTELLEN_PD_REGLER_H_

#define PWM_MAX 3250
#define CYCLES_THRESHOLD 50
#define INTEGRAL_LIMIT 3000

#define KP_GLEICHLAUF 2000.0f
#define KP_TURN 35.5f//38.5
#define KI_TURN 0.35f
#define KD_TURN 80.0f //statt 10

extern float KP_STRAIGHT;
#define KI_STRAIGHT 0.25f//0.125f//0.09f
#define KD_STRAIGHT 15.0f//17.5;//7.5f//0.01f


extern volatile int isTurning;

void UpdatePID(void);
void setPIDGoalD(float distance_R, float distance_L);
void ResetPID(void);

int PIDdone(void);


#endif /* FUNKTIONSSCHNITTSTELLEN_PD_REGLER_H_ */
