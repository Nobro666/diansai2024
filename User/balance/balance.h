/* ==================== balance.h ==================== */
#ifndef BALANCE_H
#define BALANCE_H
#include "pid.h"
#include "ti_msp_dl_config.h" 

typedef struct {
    PID   pos_pid;       // 位置环: 球误差→期望倾角
    PID   ang_pid;       // 倾角环: 倾角误差→舵机修正
    float ball_target;   // 目标刻度 (cm)
    float ball_now;      // 当前刻度 (cm, ESP32-CAM)
    float ang_desired;   // 期望倾角 (位置环输出)
    float ang_actual;    // 实际倾角 (pitch + 摆杆基准)
    int32_t servo_pwm;   // 当前舵机 PWM
} BalanceCtrl;

extern BalanceCtrl bal;
void Balance_Init(void);
void Balance_SetTarget(float cm);
void Balance_FeedBallPos(float cm);
void Balance_Control(void);
#endif