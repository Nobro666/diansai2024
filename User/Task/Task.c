#include "ti_msp_dl_config.h"
#include "No_Mcu_Ganv_Grayscale_Sensor_Config.h"
#include "delay.h"
#include "adc.h"
#include "led.h"
#include "uart.h"
#include "stdio.h"
#include "key.h"
#include "flash.h"
#include "trace.h"

#include "motor.h"
#include "pid.h"
#include "encoder.h"


PID yaw_pid;
//初始化航向pid
PID_Init(&yaw_pid, DELTA, 40.0f, 5.0f, 1.5, 0.0f, 0.0f);

/**
 * @brief 计算航向误差（处理 360° 环绕）
 * @param target  目标航向角 (0~360°)
 * @param current 当前航向角 (来自 MPU6050 yaw)
 * @return 误差 (-180~+180), 正值需左转, 负值需右转
 */
float Calculate_Heading_Error(float target, float current)
{
    float error = target - current;
    while (error > 180.0f)  error -= 360.0f;
    while (error < -180.0f) error += 360.0f;
    return error;
}

// void Turn_angel(float angel)
// {   
//     static float current_heading = yaw; // 记录初始航向
//     static uint32_t last_gyro_tick = 0;
//     if (Tick - last_gyro_tick >= 5)  // 每 5ms 读一次
//     {
//         last_gyro_tick = Tick;
//         Read_Quad(); // 去问陀螺仪要数据
//         current_yaw = yaw; // 把最新的航向存下来
//     }
//     float target_heading = current_heading + angel;//设置目标航向
//     float yaw_erro = Calculate_Heading_Error(target_heading, current_yaw);
//     float heading_correction = PID_Calc(&yaw_pid, yaw_error, 0.0f);
//     float left_target  = base_target_speed + heading_correction;
//     float right_target = base_target_speed - heading_correction;
//     if (fabs(yaw_error) < 3.0f) {   // 误差 < 3° 认为到位
//     left_target = 0;
//     right_target = 0;
//   }
// }

void Turn_angel(float angel)
{
    static float target_heading = -999;  // -999 表示未设置
    static uint32_t last_gyro_tick = 0;

    /* 只在第一次调用时设定目标 */
    if (target_heading < -100) {
        target_heading = yaw + angel;   // 记下目标
        PID_clear(&yaw_pid);
    }

    /* 每 5ms 更新 yaw */
    if (Tick - last_gyro_tick >= 5) {
        last_gyro_tick = Tick;
        Read_Quad();
        current_yaw = yaw;
    }

    float yaw_error = Calculate_Heading_Error(target_heading, current_yaw);

    /* 到位判断 */
    if (fabs(yaw_error) < 3.0f) {
        // 到位，停转
        motor_l.speed_set = base_target_speed;
        motor_r.speed_set = base_target_speed;
        target_heading = -999;   // 重置，下次调用重新设定
        return;
    }

    float corr = PID_Calc(&yaw_pid, yaw_error, 0.0f);
    motor_l.speed_set = base_target_speed + corr;
    motor_r.speed_set = base_target_speed - corr;
}



























/******************************************************************************
 * 函数名称：task1
 * 功    能：题目1：A->B自动循迹，到B点停车并声光提示
 ******************************************************************************/
void task1(void)
{
    /* 清零编码器 */
    Encoder_Clear();

    /* 开始循迹 */
    while(1)
    {
        /* 灰度PID循迹 */
        Track_PID();

        /* 到达B点 */
        if(Encoder_GetDistance() >= AB_DISTANCE)
        {
            break;
        }
    }

    /* 停车 */
    Motor_Stop();

    /* 声光提示 */
    // LED_On();
    // Buzzer_On();
    // Delay_ms(300);
    // Buzzer_Off();
    // LED_Off();
}