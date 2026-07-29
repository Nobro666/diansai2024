#ifndef __MOTOR_H__
#define __MOTOR_H__


#include "ti_msp_dl_config.h" 
#include "headfile.h"
#include "pid.h"
#include "math.h"

// #define MOTOR_FILTER 0.3
// #define POLL_INTERVAL_MS 50      // 轮询间隔
// #define MOTOR_SPEED_RERATIO 20u  // 电机减速比
// #define PULSE_PRE_ROUND 13       // 一圈多少个脉冲
// #define RADIUS_OF_TYRE 24.5f        // 轮胎半径，单位毫米
// #define MULTIPLE_ENCODER 1       // 编码器倍频,GPIO中断方式仅捕获单边沿(1x)
// #define LINE_SPEED_C RADIUS_OF_TYRE * 2 * 3.1415926535897

#define MOTOR_FILTER 0.3
#define POLL_INTERVAL_MS 50      // 轮询间隔
#define MOTOR_SPEED_RERATIO 28u  // 电机减速比
#define PULSE_PRE_ROUND 13       // 一圈多少个脉冲
#define RADIUS_OF_TYRE 32.5f        // 轮胎半径，单位毫米
#define MULTIPLE_ENCODER 1       // 编码器倍频,GPIO中断方式仅捕获单边沿(1x)
#define LINE_SPEED_C RADIUS_OF_TYRE * 2 * 3.1415926535897

typedef struct _ENCODER {
  int32_t lastCount;       // 上一次计数值
  int32_t currentCount;    // 当前计数值
  int32_t deltaCount;      // 两次计数值之差
  float speed;             // 电机转速
  float lineSpeed;         // 线速度
  uint8_t direct;          // 旋转方向
  uint32_t totalCount;     // 连续的总位置值
  int16_t last_deltaCount; // 上一次差值
  int32_t max_normal_delta;// 最大正常差值，超过这个值认为是异常
} Encoder;

typedef struct _MOTOR {
  // 硬件底层指针
  void *timer_encoder;       // 编码器定时器寄存器地址 (如 TIMER_0_INST)
  void *timer_pwm;           // PWM定时器寄存器地址 (如 TIMER_1_INST)
  
  DL_TIMER_CC_INDEX pwm_channel;    // 【修正】PWM通道索引 (如 DL_TIMER_CC_0_INDEX) 大写INDEX
  
  GPIO_Regs *port1;                 // 【修正】方向引脚1 端口 (如 GPIOA)
  uint32_t pin1;                    // 方向引脚1 引脚 (如 DL_GPIO_PIN_21=0x00200000)
  GPIO_Regs *port2;                 // 【修正】方向引脚2 端口
  uint32_t pin2;                    // 方向引脚2 引脚

  Encoder encoder;
  float last_speed;
  float speed;
  float speed_filter;
  float speed_set;
  PID pid;

  // 函数指针
  void (*PidInit)(struct _MOTOR *motor, uint8_t mode, float maxout, float max_iout, float kp, float ki, float kd);
  void (*EncoderUpdate)(struct _MOTOR *motor);
  void (*SpeedGet)(struct _MOTOR *motor);
  void (*Calc)(struct _MOTOR *motor);
  void (*Driver)(struct _MOTOR *motor, int16_t pwm);
} Motor;

// 声明初始化函数 (参数类型也做了同步修正)
void Motor_Init(Motor *motor, void *timer_encoder, void *timer_pwm, DL_TIMER_CC_INDEX channel,
                GPIO_Regs *port1, uint32_t pin1, GPIO_Regs *port2, uint32_t pin2);

#endif /* __MOTOR_H__ */



// #include "ti_msp_dl_config.h"

// void Motor_On(void);
// void Motor_Off(void);
// void Set_Speed(uint8_t side,int8_t duty);


// #ifndef __MOTOR_H__
// #define __MOTOR_H__

// #include "ti_msp_dl_config.h"
// #include "pid.h"  // 引入PID头文件

// void Motor_On(void);
// void Motor_Off(void);
// void Set_Speed(uint8_t side, int8_t speed);

// // 初始化电机PID参数
// void Motor_PID_Init(float kp, float ki, float kd, float max_out);
// // 计算PID并设置电机速度 (返回计算出的控制量)
// int16_t Motor_PID_Calc_And_Set(float target_speed, float current_speed);

// #endif