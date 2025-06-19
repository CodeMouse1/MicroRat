
#ifndef FUNKTIONSSCHNITTSTELLEN_PD_REGLER_H_
#define FUNKTIONSSCHNITTSTELLEN_PD_REGLER_H_

#include <stdbool.h>

#define PWM_L_FORWARD_FACTOR    1.0f//1.003f
#define PWM_L_BACKWARD_FACTOR   1.0f//1.05f
#define PWM_R_FORWARD_FACTOR    1.0f//0.975f
#define PWM_R_BACKWARD_FACTOR   1.0f//0.95f

#define PWM_MAX 3300
#define CYCLES_THRESHOLD 50
#define INTEGRAL_LIMIT 3000

#define KP_GLEICHLAUF 500.0f
#define KD_GLEICHLAUF 0.0f

#define KP_TURN 85.0f//78.0f
#define KD_TURN 10.0f//50.0f

extern float KP_STRAIGHT;
#define KD_STRAIGHT 25.0f//50.0f

extern volatile bool isTurning;

void UpdatePID(void);
void setPIDGoalD(float distance_R, float distance_L);
void ResetPID(void);

int PIDdone(void);




//HILFESTELLUNG//
// --- PID-Logging-Puffer ---
#define PID_LOG_BUFFER_SIZE 300 // Anzahl der gespeicherten Einträge
#define PID_LOG_INTERVAL 2      // Jede 2. PID-Iteration speichern
typedef struct {
    float error_L;
    float error_R;
    float correction_L;
    float correction_R;
    float current_pos_L;
    float current_pos_R;
    int is_stable_condition_met; // 1, wenn Bedingung erfüllt; 0 sonst
} PidLogEntry;

void LogPIDEntry(float error_L, float error_R, float correction_L, float correction_R, float current_pos_L, float current_pos_R, int is_stable_condition_met);
void DumpPIDLog(void);
void ClearPIDLog(void);

#endif /* FUNKTIONSSCHNITTSTELLEN_PD_REGLER_H_ */
