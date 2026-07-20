// #include "ti_msp_dl_config.h"

// void Motor_On(void);
// void Motor_Off(void);
// void Set_Speed(uint8_t side,int8_t duty);


#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "ti_msp_dl_config.h"
#include "pid.h"  // 引入PID头文件

void Motor_On(void);
void Motor_Off(void);
void Set_Speed(uint8_t side, int8_t speed);

// 初始化电机PID参数
void Motor_PID_Init(float kp, float ki, float kd, float max_out);
// 计算PID并设置电机速度 (返回计算出的控制量)
int16_t Motor_PID_Calc_And_Set(float target_speed, float current_speed);

#endif