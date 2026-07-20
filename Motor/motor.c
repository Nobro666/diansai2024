#include "motor.h"

float motor_max_duty = 100;

// ===== 定义全局PID结构体 =====
PID motor_pid; 

void Motor_On(void)
{
    DL_GPIO_setPins(GPIO_MOTOR_PIN_STBY_PORT, GPIO_MOTOR_PIN_STBY_PIN);
}

void Motor_Off(void)
{
    DL_GPIO_clearPins(GPIO_MOTOR_PIN_STBY_PORT, GPIO_MOTOR_PIN_STBY_PIN);

    DL_GPIO_clearPins(GPIO_MOTOR_AIN1_PORT, GPIO_MOTOR_AIN1_PIN);
    DL_GPIO_clearPins(GPIO_MOTOR_AIN2_PORT, GPIO_MOTOR_AIN2_PIN);
    DL_GPIO_clearPins(GPIO_MOTOR_BIN1_PORT, GPIO_MOTOR_BIN1_PIN);
    DL_GPIO_clearPins(GPIO_MOTOR_BIN2_PORT, GPIO_MOTOR_BIN2_PIN);
}

void Set_Speed(uint8_t side, int8_t speed)
{
    uint32_t compareValue = 0;
    if(side == 0)
    {
        if(speed < 0)
        {
            // compareValue=3199-3199*(-duty/100.0);
            compareValue = 32000 * (-speed/motor_max_duty);
            DL_TimerA_setCaptureCompareValue(PWM_MOTOR_INST,compareValue,DL_TIMER_CC_0_INDEX);
            DL_GPIO_setPins(GPIO_MOTOR_AIN1_PORT, GPIO_MOTOR_AIN1_PIN);
            DL_GPIO_clearPins(GPIO_MOTOR_AIN2_PORT, GPIO_MOTOR_AIN2_PIN);
        }
        else if(speed > 0)
        {
            compareValue = 32000 * (speed/motor_max_duty);
            DL_TimerA_setCaptureCompareValue(PWM_MOTOR_INST,compareValue,DL_TIMER_CC_0_INDEX);
            DL_GPIO_clearPins(GPIO_MOTOR_AIN1_PORT, GPIO_MOTOR_AIN1_PIN);
            DL_GPIO_setPins(GPIO_MOTOR_AIN2_PORT, GPIO_MOTOR_AIN2_PIN);
        }
        else
        {
            DL_GPIO_clearPins(GPIO_MOTOR_AIN1_PORT, GPIO_MOTOR_AIN1_PIN);
            DL_GPIO_clearPins(GPIO_MOTOR_AIN2_PORT, GPIO_MOTOR_AIN2_PIN);
        }
    }
    else
    {
        if(speed < 0)
        {
            compareValue = 32000 * (-speed/motor_max_duty);
            DL_TimerA_setCaptureCompareValue(PWM_MOTOR_INST,compareValue,DL_TIMER_CC_1_INDEX);
            DL_GPIO_setPins(GPIO_MOTOR_BIN1_PORT, GPIO_MOTOR_BIN1_PIN);
            DL_GPIO_clearPins(GPIO_MOTOR_BIN2_PORT, GPIO_MOTOR_BIN2_PIN);
        }
        else if(speed > 0)
        {
            compareValue = 32000 * (speed/motor_max_duty);
            DL_TimerA_setCaptureCompareValue(PWM_MOTOR_INST,compareValue,DL_TIMER_CC_1_INDEX);
            DL_GPIO_clearPins(GPIO_MOTOR_BIN1_PORT, GPIO_MOTOR_BIN1_PIN);
            DL_GPIO_setPins(GPIO_MOTOR_BIN2_PORT, GPIO_MOTOR_BIN2_PIN);
        }
        else
        {
            DL_GPIO_clearPins(GPIO_MOTOR_BIN1_PORT, GPIO_MOTOR_BIN1_PIN);
            DL_GPIO_clearPins(GPIO_MOTOR_BIN2_PORT, GPIO_MOTOR_BIN2_PIN);
        }
    }
}

// ===== PID 控制接口实现 =====

/**
 * @brief 初始化电机PID参数
 * @param kp 比例系数
 * @param ki 积分系数
 * @param kd 微分系数
 * @param max_out 最大输出限制（对应最大PWM值 100）
 */
void Motor_PID_Init(float kp, float ki, float kd, float max_out)
{
    // 使用增量式PID
    PID_Init(&motor_pid, DELTA, max_out, max_out, kp, ki, kd);
    PID_clear(&motor_pid);
}

/**
 * @brief 计算PID并设置电机速度
 * @param target_speed 目标速度 (单位：和你的反馈速度保持一致，比如 转/秒)
 * @param current_speed 当前实际速度 (需配合编码器获取)
 * @return 实际输出的 PWM 占空比控制量 (-100 ~ 100)
 */
int16_t Motor_PID_Calc_And_Set(float target_speed, float current_speed)
{
    // 1. 调用您 pid.c 中的 PID_Calc 算法
    float output = PID_Calc(&motor_pid, current_speed, target_speed);
    
    // 2. 额外安全限幅（防止PWM负数过大导致溢出）
    if (output > 100.0f) output = 100.0f;
    if (output < -100.0f) output = -100.0f;
    
    // 3. 将PID计算出的占空比输出给两个电机
    // (这里设置左右轮同速。如果是循迹差速，你需要分别对左右轮计算PID)
    Set_Speed(0, (int8_t)output); // 左轮
    Set_Speed(1, (int8_t)output); // 右轮
    
    return (int16_t)output;
}




// #include "motor.h"

// float motor_max_duty=100;

// void Motor_On(void)
// {
//     DL_GPIO_setPins(GPIO_MOTOR_PIN_STBY_PORT, GPIO_MOTOR_PIN_STBY_PIN);
// }

// void Motor_Off(void)
// {
//     DL_GPIO_clearPins(GPIO_MOTOR_PIN_STBY_PORT, GPIO_MOTOR_PIN_STBY_PIN);

//     DL_GPIO_clearPins(GPIO_MOTOR_AIN1_PORT, GPIO_MOTOR_AIN1_PIN);
//     DL_GPIO_clearPins(GPIO_MOTOR_AIN2_PORT, GPIO_MOTOR_AIN2_PIN);
//     DL_GPIO_clearPins(GPIO_MOTOR_BIN1_PORT, GPIO_MOTOR_BIN1_PIN);
//     DL_GPIO_clearPins(GPIO_MOTOR_BIN2_PORT, GPIO_MOTOR_BIN2_PIN);
// }

// void Set_Speed(uint8_t side, int8_t speed)
// {
//     uint32_t compareValue = 0;
//     if(side == 0)
//     {
//         if(speed < 0)
//         {
//             // compareValue=3199-3199*(-duty/100.0);
//             compareValue = 32000 * (-speed/motor_max_duty);
//             DL_TimerA_setCaptureCompareValue(PWM_MOTOR_INST,compareValue,DL_TIMER_CC_0_INDEX);
//             DL_GPIO_setPins(GPIO_MOTOR_AIN1_PORT, GPIO_MOTOR_AIN1_PIN);
//             DL_GPIO_clearPins(GPIO_MOTOR_AIN2_PORT, GPIO_MOTOR_AIN2_PIN);
//         }
//         else if(speed > 0)
//         {
//             compareValue = 32000 * (speed/motor_max_duty);
//             DL_TimerA_setCaptureCompareValue(PWM_MOTOR_INST,compareValue,DL_TIMER_CC_0_INDEX);
//             DL_GPIO_clearPins(GPIO_MOTOR_AIN1_PORT, GPIO_MOTOR_AIN1_PIN);
//             DL_GPIO_setPins(GPIO_MOTOR_AIN2_PORT, GPIO_MOTOR_AIN2_PIN);
//         }
//         else
//         {
//             DL_GPIO_clearPins(GPIO_MOTOR_AIN1_PORT, GPIO_MOTOR_AIN1_PIN);
//             DL_GPIO_clearPins(GPIO_MOTOR_AIN2_PORT, GPIO_MOTOR_AIN2_PIN);
//         }
//     }
//     else
//     {
//         if(speed < 0)
//         {
//             compareValue = 32000 * (-speed/motor_max_duty);
//             DL_TimerA_setCaptureCompareValue(PWM_MOTOR_INST,compareValue,DL_TIMER_CC_1_INDEX);
//             DL_GPIO_setPins(GPIO_MOTOR_BIN1_PORT, GPIO_MOTOR_BIN1_PIN);
//             DL_GPIO_clearPins(GPIO_MOTOR_BIN2_PORT, GPIO_MOTOR_BIN2_PIN);
//         }
//         else if(speed > 0)
//         {
//             compareValue = 32000 * (speed/motor_max_duty);
//             DL_TimerA_setCaptureCompareValue(PWM_MOTOR_INST,compareValue,DL_TIMER_CC_1_INDEX);
//             DL_GPIO_clearPins(GPIO_MOTOR_BIN1_PORT, GPIO_MOTOR_BIN1_PIN);
//             DL_GPIO_setPins(GPIO_MOTOR_BIN2_PORT, GPIO_MOTOR_BIN2_PIN);
//         }
//         else
//         {
//             DL_GPIO_clearPins(GPIO_MOTOR_BIN1_PORT, GPIO_MOTOR_BIN1_PIN);
//             DL_GPIO_clearPins(GPIO_MOTOR_BIN2_PORT, GPIO_MOTOR_BIN2_PIN);
//         }
//     }
// }