#include "ti_msp_dl_config.h"
#include "No_Mcu_Ganv_Grayscale_Sensor_Config.h"
#include "delay.h"
#include "adc.h"
#include "led.h"
#include "uart.h"
#include "stdio.h"
#include "key.h"
#include "flash.h"
#include "control.h"

#include "motor.h"
#include "pid.h"
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
Motor motor_l;
Motor motor_r;

float base_target_speed = 600.0f;

void Motor_Ctrl(void)
{
    //电机初始化
    Motor_Init(&motor_l, TIMER_Encoder_INST, PWM_MOTOR_INST, DL_TIMER_CC_0_INDEX, GPIOA ,DL_GPIO_PIN_21, GPIOA, DL_GPIO_PIN_22);
    Motor_Init(&motor_r, TIMER_Encoder_INST, PWM_MOTOR_INST, DL_TIMER_CC_1_INDEX, GPIOB ,DL_GPIO_PIN_9, GPIOB, DL_GPIO_PIN_8);
    //电机PID参数初始化
    motor_l.PidInit(&motor_l, DELTA, 3200.0f, 3200.0f, 10, 0, 0);
    motor_r.PidInit(&motor_r, DELTA, 3200.0f, 3200.0f, 10, 0, 0);
     // ----------------- 1. 读取传感器数据 -----------------
    No_Mcu_Ganv_Sensor_Task_Without_tick(&sensor);
    Get_Anolog_Value(&sensor, Anolog);
    Digtal = Get_Digtal_For_User(&sensor);

    // ----------------- 2. 计算位置误差 -----------------
    // 之前写的计算误差函数返回 -3.5 ~ +3.5，直接用
    float error = Calculate_Position_Error(Digtal); 

    // ----------------- 3. 纠偏 PID 计算 (差速计算) -----------------
    // 这里我们只做一个目标值=0的PID，计算出为了把误差拉回0所需的“差速值”
    // 使用您之前定义好的 tracking_pid
    float turn_correction = PID_Calc(&tracking_pid, error, 0.0f);

    // ----------------- 4. 计算左右轮目标速度 -----------------
    // 核心公式：左轮 = 基础速度 + 修正量，右轮 = 基础速度 - 修正量
    // 如果偏右（正误差），PID输出正值：左轮加速，右轮减速 -> 向左转
    float left_target = base_target_speed + turn_correction;
    float right_target = base_target_speed - turn_correction;

    // ----------------- 5. 速度限幅（非常关键！） -----------------
    // 防止突然反转或超出电机最大能力
    // 如果您的电机最快是 3200 脉冲，就设 3200
    float max_speed = 3200.0f; 
    if (left_target > max_speed) left_target = max_speed;
    if (left_target < -max_speed) left_target = -max_speed;
    if (right_target > max_speed) right_target = max_speed;
    if (right_target < -max_speed) right_target = -max_speed;

    // 增加防微抖死区：如果速度太小（比如低于 200），直接给0，防止电机嗡嗡响不转
    if (fabs(left_target) < 200.0f) left_target = 0.0f;
    if (fabs(right_target) < 200.0f) right_target = 0.0f;

    // ----------------- 6. 下发目标速度给电机驱动 -----------------
    // 这一步是关键！我们只告诉电机“你要跑多快”，电机底层 PID 会自动调控 PWM 去达到这个速度
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












void Control(void)
{   
    // 1. 初始化纠偏 PID (位置式/增量式均可，这里推位置式，纠偏更平滑)
    PID_Init(&tracking_pid, POSITION, 1500.0f, 500.0f, 50.0f, 0.0f, 0.0f);
    // 2. 给电机初始化目标速度 (初始为0，防止一上电猛冲)
    motor_l.speed_set = 0;
    motor_r.speed_set = 0;
    if (state.value == KEY_IDLE||state.value == KEY_DISABLE||state.value == KEY_WAIT_LOSS ) {
            // 正常操作模式(非校准状态)
            
            // 执行无时基依赖的传感器任务
            No_Mcu_Ganv_Sensor_Task_Without_tick(&sensor);
            
            // 从传感器获取当前模拟量值
            Get_Anolog_Value(&sensor, Anolog);
            
            // 将模拟量转换为数字输出
            Digtal = Get_Digtal_For_User(&sensor);
					
        } else {
            // 校准模式 - 将数字输出置0，八路LED灯关闭
            Digtal = 0;
        }

        // 3. 必须用固定时间间隔调用，保证速度计算准确 (建议用 Tick 做非阻塞延时)
        // 每隔 10ms 执行一次循迹
        static uint32_t last_loop_tick = 0;
        if (Tick - last_loop_tick >= 10)
         {
            last_loop_tick = Tick;
            // 更新编码器计数值 
            motor_l.EncoderUpdate(&motor_l);
            motor_r.EncoderUpdate(&motor_r);
            // 读取传感器并执行循迹函数
            Motor_Ctrl();
            
            // 第三步：驱动层闭环 (让 PID 速度环生效)
            // 结合当前的 speed_set (目标) 和 speed_filter (当前实际速度)，算出 PWM 发给电机
            motor_l.Calc(&motor_l);
            motor_r.Calc(&motor_r);
        }
				
        // 处理按键输入
        Key_Process();
        
           // 假设定义
           char tx_buff[128];  // 发送缓冲区，不用太大（每行最多约60字节） 
           // 发送数字量（8位拆分为8个独立位）
           sprintf(tx_buff, "Digtal %d-%d-%d-%d-%d-%d-%d-%d\r\n",(Digtal >> 0) & 0x01,(Digtal >> 1) & 0x01,(Digtal >> 2) & 0x01,(Digtal >> 3) & 0x01,(Digtal >> 4) & 0x01,(Digtal >> 5) & 0x01,(Digtal >> 6) & 0x01,(Digtal >> 7) & 0x01);
           uart0_send_string(tx_buff);

           // 发送模拟量（全部8个通道）
           sprintf(tx_buff, "Anolog %u-%u-%u-%u-%u-%u-%u-%u\r\n",Anolog[0], Anolog[1], Anolog[2], Anolog[3],Anolog[4], Anolog[5], Anolog[6], Anolog[7]);
           uart0_send_string(tx_buff);


        // 更新KEY和ERR LED的状态
        LED_KEY_Blink_Update();
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
                return;
            }
            /* 确认按键按下 */
            if (DL_GPIO_readPins(GRAY_IN_PORT, GRAY_IN_IN_KEY_PIN) == 0) {
                key_pressed = 1;
								long_pressed_key_time=Tick;
                last_key_time = Tick;
            }
    }

}







