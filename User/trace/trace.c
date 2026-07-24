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
/*
 * =======================================================
 * 硬件引脚映射 (Hardware Pinout Map)
 * =======================================================
 * 
 * ----------------------------
 * LED控制引脚 (LED Control Pins)
 * ----------------------------
 * PA18  -> LED1        // 指示灯1
 * PA17  -> LED2        // 指示灯2
 * PA16  -> LED3        // 指示灯3
 * PA10  -> LED4        // 指示灯4
 * PA9   -> LED5        // 指示灯5
 * PA8   -> LED6        // 指示灯6
 * PA7   -> LED7        // 指示灯7
 * PA6   -> LED8        // 指示灯8
 * PA11  -> ERR_LED     // 错误状态指示灯
 * PA12  -> KEY_LED     // 按键状态指示灯
 * 
 * ----------------------------
 * 用户输入引脚 (User Input Pin)
 * ----------------------------
 * PA22  -> KEY         // 用户按键输入
 * 
 * ----------------------------
 * 无MCU传感器接口
 * (Dedicated Sensor Interface)
 * ----------------------------
 * PA15  -> AD0         // 地址输入通道0
 * PA14  -> AD1         // 地址输入通道1
 * PA13  -> AD2         // 地址输入通道2
 * PA26  -> ERR         // 传感器错误信号输入
 * PA27  -> OUT         // 传感器模拟量输出信号(接入ADC)
 * 
 * =======================================================
 */



/*                  模拟量转数字量的滞回比较器(施密特触发器)示意图               
 *          /\
 *   Digital |     
 *           |
 *        0  |                 +----------------+--------------
 *           |                 |                |
 *           |                 |                |
 *           |                 |                |
 *           |                 |                |
 *           |                 |                |
 *           |                 |                |
 *           |                 |                |
 *           |                 |                |     
 *         1 |    -------------+----------------+
 *      -----+----------------------------------------------------------> analog
 *           |   0             1/3              2/3            1 
 *           |   黑            灰黑             灰白            白
 *               Calibrated    Gray             Gray           Calibrated
 *               black         black            white          white
 */


// 全局变量定义
unsigned short Anolog[8] = {0};    // 存储当前模拟量值的数组
unsigned short white[8] = {0};     // 存储白色校准值的数组 
unsigned short black[8] = {0};     // 存储黑色校准值的数组
unsigned short Normal[8];          // 归一化值数组

No_MCU_Sensor sensor;              // 传感器数据结构体
unsigned char Digtal;              // 数字输出值


PID tracking_pid;
PID yaw_pid;
Motor motor_l;
Motor motor_r;

float base_target_speed = 25;
#define FindSpeed 18
uint8_t drive_mode = 0;   // 0=循迹, 1=航向转向, 2=锁定直行, 3=慢速找线
float target_angle = 0;   // 转向目标角度
bool  heading_relock = false;  // RunStraight 重新锁定航向

void Motor_Ctrl(float err)
{
    // 纠偏 PID → 差速值
    float turn_correction = PID_Calc(&tracking_pid, err, 0.0f);

    // 左右轮目标速度 = 基础速度 ± 差速修正
    float left_target  = base_target_speed + turn_correction;
    float right_target = base_target_speed - turn_correction;

    // 速度限幅
    float max_speed = 200;  //3000
    if (left_target > max_speed)   left_target = max_speed;
    if (left_target < -max_speed)  left_target = -max_speed;
    if (right_target > max_speed)  right_target = max_speed;
    if (right_target < -max_speed) right_target = -max_speed;

    // // 死区
    // if (fabs(left_target) < 20.0f)  left_target = 0.0f;
    // if (fabs(right_target) < 20.0f) right_target = 0.0f;

    // 下发给电机 PID
    motor_l.speed_set = left_target;
    motor_r.speed_set = right_target;
}




/**
 * @brief 根据8位数字量计算位置误差
 * @param digtal 8位数字状态（1代表黑线，0代表白底）
 * @return 误差值。正数表示偏右，负数表示偏左
 */
float Calculate_Position_Error(unsigned char digtal)
{
    float weighted_sum = 0;
    float total_weight = 0;
    
    for (int i = 0; i < 8; i++) {
        if ((digtal >> i) & 0x01) {
            float position = (float)i - 3.5; 
            weighted_sum += position;
            total_weight += 1;
        }
    }
    
    if (total_weight == 0) return 0.0f;
    
    // 返回纯正的误差位置
    return (weighted_sum / total_weight); 
}






void Trace_init(void)
{
    // 初始化纠偏 PID (位置式/增量式均可，这里推位置式，纠偏更平滑)
    PID_Init(&tracking_pid, DELTA, 40.0f, 5.0f, 21, 0.0f, 0.0f);//DELTA, 40.0f, 5.0f, 10, 0.0f, 0.0f
    PID_Init(&yaw_pid, DELTA, 35.0f, 0.0f, 0.6, 0.0f, 0.0f);
    // 给电机初始化目标速度 (初始为0，防止一上电猛冲)
    motor_l.speed_set = 0;
    motor_r.speed_set = 0;
    //电机初始化
    Motor_Init(&motor_r, TIMER_Encoder_INST, PWM_MOTOR_INST, DL_TIMER_CC_1_INDEX, GPIOB ,DL_GPIO_PIN_8, GPIOB, DL_GPIO_PIN_9);
    Motor_Init(&motor_l, TIMER_Encoder_INST, PWM_MOTOR_INST, DL_TIMER_CC_0_INDEX, GPIOA ,DL_GPIO_PIN_22, GPIOA, DL_GPIO_PIN_21);
    //使能电机驱动芯片 (拉高STBY引脚)
    DL_GPIO_setPins(GPIO_MOTOR_PIN_STBY_PORT, GPIO_MOTOR_PIN_STBY_PIN);
    //编码器初始化(GPIO中断:相位A计数+相位B判方向,用Tick计时)
    Encoder_Init(&encL, GPIOA, DL_GPIO_PIN_7,  GPIOA, DL_GPIO_PIN_26, PULSE_PRE_ROUND);
    Encoder_Init(&encR, GPIOA, DL_GPIO_PIN_28, GPIOB, DL_GPIO_PIN_6,  PULSE_PRE_ROUND);
    //电机PID参数初始化
    motor_l.PidInit(&motor_l, DELTA, 2000.0f, 1000.0f, 0.2, 0, 0);//DELTA, 2000.0f, 1000.0f, 0.2, 0, 0
    motor_r.PidInit(&motor_r, DELTA, 2000.0f, 1000.0f, 0.2, 0, 0);//DELTA, 2000.0f, 1000.0f, 0.2, 0, 0
}








void Control(void)
{   
    
    if (state.value == KEY_IDLE||state.value == KEY_DISABLE||state.value == KEY_WAIT_LOSS ) {
            // 正常操作模式(非校准状态)
            
            // 执行无时基依赖的传感器任务
            No_Mcu_Ganv_Sensor_Task_Without_tick(&sensor);
            
            // 从传感器获取当前模拟量值
            Get_Anolog_Value(&sensor, Anolog);
            
            // 将模拟量转换为数字输出
            Digtal = Get_Digtal_For_User(&sensor);
					
        } else {
            Digtal = 0;
        }

    float error = Calculate_Position_Error(Digtal);

        // 3. 必须用固定时间间隔调用，保证速度计算准确 (建议用 Tick 做非阻塞延时)
        // 每隔 10ms 执行一次循迹
        static uint32_t last_loop_tick = 0;
        if (Tick - last_loop_tick >= 10)
         {
            last_loop_tick = Tick;
            // 从编码器驱动获取delta (GPIO中断累加, 替代TIMA1定时器读数)
            {
                int32_t dL = Encoder_GetDelta(&encL);
                int32_t dR = Encoder_GetDelta(&encR);
                motor_l.encoder.deltaCount = (int16_t)dL;
                motor_r.encoder.deltaCount = (int16_t)dR;
                motor_l.encoder.totalCount += dL;
                motor_r.encoder.totalCount += dR;
            }
            // 读取编码器速度反馈 (更新 speed_filter)
            motor_l.SpeedGet(&motor_l);
            motor_r.SpeedGet(&motor_r);
            // 根据模式设置 speed_set
            switch (drive_mode) {
                case 0: Motor_Ctrl(error);        break;  // 循迹
                case 1: Turn_angel(target_angle); break;  // 航向转向
                case 2: RunStraight();            break;  // 锁定直行
                case 3: SlowForward();            break;  // 慢速找线
            }

            // PID 闭环 → PWM 输出
            motor_l.Calc(&motor_l);
            motor_r.Calc(&motor_r);
            motor_l.Driver(&motor_l, (int32_t)motor_l.pid.out);
            motor_r.Driver(&motor_r, (int32_t)motor_r.pid.out);

            // 调试输出（每10ms一次，手动格式化避免sprintf卡死）
            uart0_send_string("D:");
            for (int i = 7; i >= 0; i--) {
                uart0_send_char(((Digtal >> i) & 0x01) ? '1' : '0');
            }
            uart0_send_string(" A:");
            for (int i = 0; i < 8; i++) {
                uint16_t v = Anolog[i];
                uart0_send_char('0' + v / 1000 % 10);
                uart0_send_char('0' + v / 100  % 10);
                uart0_send_char('0' + v / 10   % 10);
                uart0_send_char('0' + v        % 10);
                if (i < 7) uart0_send_char(',');
            }
            uart0_send_string("\r\n");
        }
				
        // 处理按键输入
        Key_Process();

        // 更新KEY和ERR LED的状态
        LED_KEY_Blink_Update();

        // 保持 Tick 时序推进（每次 Control 调用递增一次）
        Tick++;
}

/**
 * @brief 外部中断处理函数（CLK和按键）
 * @note  检测到有效按键后设置key_pressed标志
 */


 
void GROUP1_IRQHandler(void)
{
	    // 读取Group1的中断寄存器并清除中断标志位
    uint32_t pending = DL_GPIO_getPendingInterrupt(GPIOA);
	
    if(pending == GRAY_IN_IN_KEY_IIDX){
            /* 防抖处理 */
            if ((Tick - last_key_time) < DEBOUNCE_TIME_MS) {
                DL_GPIO_clearInterruptStatus(GPIOA, GRAY_IN_IN_KEY_PIN);
                return;
            }
            /* 确认按键按下 */
            if (DL_GPIO_readPins(GRAY_IN_PORT, GRAY_IN_IN_KEY_PIN) == 0) {
                key_pressed = 1;
				long_pressed_key_time=Tick;
                last_key_time = Tick;
            }
            DL_GPIO_clearInterruptStatus(GPIOA, GRAY_IN_IN_KEY_PIN);
        return;
    }

    /* encoder PA7/PA28 */
    if (pending == DL_GPIO_IIDX_DIO7 || pending == DL_GPIO_IIDX_DIO28) {
        Encoder_HandleGPIOA(pending);
        return;
    }
    /* unknown GPIOA: IIDX to pin mask, clear */
    if (pending > 0 && pending < 32) {
        DL_GPIO_clearInterruptStatus(GPIOA, (1UL << pending));
    }
    /* GPIOB safety net */
    uint32_t pendingB = DL_GPIO_getPendingInterrupt(GPIOB);
    Encoder_HandleGPIOB(pendingB);
}






float current_yaw=0;

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



void Turn_angel(float angel)
{
    static float target_heading = -999;
    static bool  target_set   = false;

    /* 首次调用：记录目标航向 */
  if (!target_set) {
      Read_Quad();
      current_yaw = yaw;
      target_heading = current_yaw + angel;
      target_set    = true;
      PID_clear(&yaw_pid);
  }

    // /* 每 5ms 读一次陀螺仪 */
    // static uint32_t last_gyro_tick = 0;
    // if (Tick - last_gyro_tick >= 5) {
    //     last_gyro_tick = Tick;
        Read_Quad();
        current_yaw = yaw;
    // }

    float yaw_error = Calculate_Heading_Error(target_heading, current_yaw);

    // /* 到位：转直走，重置目标 */
    // if (fabs(yaw_error) < 3.0f) {
    //     motor_l.speed_set = base_target_speed;
    //     motor_r.speed_set = base_target_speed;
    //     target_set = false;
    //     target_heading = -999;
    //     drive_mode = 0;   // 切回循迹模式
    //     return;
    // }

    static uint8_t ok_cnt=0;
    if(fabs(yaw_error)<3)
    {
        ok_cnt++;
        if(ok_cnt>=5)      // 连续50ms满足
        {
            ok_cnt=0;
            motor_l.speed_set = base_target_speed;
            motor_r.speed_set = base_target_speed;
            target_set = false;
            target_heading = -999;
            drive_mode=0;   // 切回循迹模式
            return;
        }
    }
    else
    {
        ok_cnt=0;
    }

        /* 边走边转 */
        float corr = PID_Calc(&yaw_pid, yaw_error, 0.0f);
        motor_l.speed_set = base_target_speed + corr;
        motor_r.speed_set = base_target_speed - corr;
    }




void RunStraight(void)
{
    static float  target_yaw;
    static bool   first = true;

    if (first || heading_relock) {
        Read_Quad();
        target_yaw = yaw;
        PID_clear(&yaw_pid);
        first       = false;
        heading_relock = false;
    }

    Read_Quad();
    float err  = Calculate_Heading_Error(target_yaw, yaw);
    float corr = PID_Calc(&yaw_pid, err, 0.0f);

    motor_l.speed_set = base_target_speed + corr;
    motor_r.speed_set = base_target_speed - corr;
}


void SlowForward(void)
{
    motor_l.speed_set = FindSpeed;
    motor_r.speed_set = FindSpeed;
}
