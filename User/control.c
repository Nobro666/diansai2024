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
            // 校准模式 - 将数字输出置0，八路LED灯关闭
            Digtal = 0;
        }

        Motor_Ctrl();
				
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



int16_t rxbuf = 0, cx = 160;
int16_t basespeed = 5;
int16_t SpeedL = 0, SpeedR = 0;
float Kp = 0.5;

void Motor_Ctrl(void)
{
    // ---------- 2. 核心循迹算法 (加上这行) ----------
            // 根据 8 位数字量计算偏离误差
            // 越向左偏，误差为负数；越向右偏，误差为正数
            int error = Calculate_Position_Error(Digtal); 
            
            // 基础电机控制 (比如是 PID 控制，或者简单的差速)
            if (error == 0) 
            {
                // 传感器在正中间，或者全白/全黑，直行
                Set_Speed(0, basespeed);
                Set_Speed(1, basespeed);
            } 
            else if (error > 0) 
            {
                // 偏右了，需要向左转 (右侧减速，左侧保持或微减)
                Set_Speed(0, basespeed - (error * 200));
                Set_Speed(1, basespeed);
            } 
            else if (error < 0) 
            {
                // 偏左了，需要向右转 (左侧减速，右侧保持或微减)
                Set_Speed(0, basespeed);
                Set_Speed(1, basespeed - (abs(error) * 200));
            }

        else 
        {
            // 校准模式 - 电机停止，数字输出置0，八路LED灯关闭
            Digtal = 0;
            Set_Speed(0, 0);
            Set_Speed(1, 0); // 校准时不走动
        }
}



/**
 * @brief 根据8位数字量计算位置误差
 * @param digtal 8位数字状态（1代表黑线，0代表白底）
 * @return 误差值。正数表示偏右，负数表示偏左
 */
int Calculate_Position_Error(unsigned char digtal)
{
    // 简化方法：计算重心位置 (左权重高为负，右权重高为正)
    // 假设 8 个传感器索引 0(最左) 到 7(最右)
    
    float weighted_sum = 0;
    int total_weight = 0;
    
    // 遍历 8 个位
    for (int i = 0; i < 8; i++) {
        if ((digtal >> i) & 0x01) {
            // 如果将传感器索引映射为 -3.5 到 +3.5 的步进 (中心为0)
            float position = (float)i - 3.5; 
            weighted_sum += position;
            total_weight += 1;
        }
    }
    
    if (total_weight == 0) {
        return 0; // 没看到黑线
    }
    
    // 返回平均误差（简单归一化，乘以一个系数可以让纠偏更猛）
    return (int)((weighted_sum / total_weight) * 50);
}
