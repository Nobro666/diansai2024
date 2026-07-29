/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
#include "pid.h"
#include "Task.h"
#include "oled_software_i2c.h"

extern Motor motor_l;
extern Motor motor_r;
extern PID tracking_pid; 


extern unsigned short Anolog[8] ;    // 存储当前模拟量值的数组
extern unsigned short white[8] ;     // 存储白色校准值的数组 
extern unsigned short black[8];     // 存储黑色校准值的数组
extern unsigned short Normal[8];          // 归一化值数组
extern No_MCU_Sensor sensor;
extern float current_yaw;

uint8_t oled_buffer[32];
void OLED_ShowTimer(void);
uint32_t tick_start = 0;

int main(void)
{
    // 初始化系统配置
    SYSCFG_DL_init();
    DL_TimerG_setCaptureCompareValue(PWM_Servo_INST, 500, DL_TIMER_CC_0_INDEX);

    __enable_irq();
    SysTick_Init();
	
    // 初始化LED
    LED_init();
   
    SYSCFG_DL_UART_k230_init();
    uart0_send_string("Start System OK\r\n");
    /* DMA配置 - 用于ADC数据传输 */
    // 设置DMA源地址(ADC存储器)
    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC0->ULLMEM.MEMRES[0]);
    // 设置DMA目标地址(ADC_VALUE缓冲区)
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC_VALUE[0]);		
    // 使能DMA通道
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);;
    // 启动ADC转换
    DL_ADC12_startConversion(ADC_VOLTAGE_INST);
    
    // 使能按键中断
    NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOA_INT_IRQN);
	
    // 从Flash存储器读取校准值
    readWhiteFromFlash();  // 读取白色校准值
    readBlackFromFlash();  // 读取黑色校准值

    // 使用校准值初始化传感器
    No_MCU_Ganv_Sensor_Init(&sensor, white, black);

    //无MCU灰度传感器硬件起振需要时间
    // Tick_delay(100);
    state.value=KEY_IDLE;
    
    //初始化电机
    Trace_init();
    
    //初始化陀螺仪
    MPU6050_Init();
    OLED_Init();
    Tick_delay(2000); // 原地静止 2 秒校准
    tick_start = tick_ms; 
	// float Yaw_Angle=0;

    Interrupt_Init();

    OLED_ShowString(0,7,(uint8_t *)"MPU6050 Demo",8);

    OLED_ShowString(0,0,(uint8_t *)"Pitch",8);
    OLED_ShowString(0,2,(uint8_t *)" Roll",8);
    OLED_ShowString(0,4,(uint8_t *)"  Yaw",8);

    OLED_ShowString(16*6,3,(uint8_t *)"Accel",8);
    OLED_ShowString(17*6,4,(uint8_t *)"Gyro",8);

    

    /* 主应用程序循环 */
    while (1)
    {
            
        // DL_TimerA_setCaptureCompareValue(PWM_MOTOR_INST,600,DL_TIMER_CC_0_INDEX);
        // DL_GPIO_setPins(GPIO_MOTOR_AIN1_PORT, GPIO_MOTOR_AIN1_PIN);
        // DL_GPIO_clearPins(GPIO_MOTOR_AIN2_PORT, GPIO_MOTOR_AIN2_PIN);
        // Control();
        // ===== 2. 使用轮询方式读取 =====
        // 如果不接 INT 引脚，我们不能一直狂读。每隔 5ms 读一次刚好。

        // static uint32_t last_gyro_tick = 0;
        // if (Tick - last_gyro_tick >= 5)  // 每 5ms 读一次
        // {
        //     last_gyro_tick = Tick;
        //     Read_Quad(); // 强行去问陀螺仪要数据
        //     current_yaw = yaw; // 把最新的航向存下来
        //     // 调试打印
        //     char tx_buff[50];
        //     sprintf(tx_buff, "Yaw: %.2f\r\n", current_yaw);
        //     uart0_send_string(tx_buff);
        // }
        // // Tick_delay(10);

        task1();

        // DL_TimerG_setCaptureCompareValue(PWM_Servo_INST, 1500, DL_TIMER_CC_0_INDEX);

        // Read_Quad();
        // sprintf((char *)oled_buffer, "%-6.1f", pitch);
        // OLED_ShowString(5*8,0,oled_buffer,16);
        // sprintf((char *)oled_buffer, "%-6.1f", roll);
        // OLED_ShowString(5*8,2,oled_buffer,16);
        // sprintf((char *)oled_buffer, "%-6.1f", yaw);
        // OLED_ShowString(5*8,4,oled_buffer,16);

        // sprintf((char *)oled_buffer, "%6d", accel[0]);
        // OLED_ShowString(15*6,0,oled_buffer,8);
        // sprintf((char *)oled_buffer, "%6d", accel[1]);
        // OLED_ShowString(15*6,1,oled_buffer,8);
        // sprintf((char *)oled_buffer, "%6d", accel[2]);
        // OLED_ShowString(15*6,2,oled_buffer,8);

        // sprintf((char *)oled_buffer, "%6d", gyro[0]);
        // OLED_ShowString(15*6,5,oled_buffer,8);
        // sprintf((char *)oled_buffer, "%6d", gyro[1]);
        // OLED_ShowString(15*6,6,oled_buffer,8);
        // sprintf((char *)oled_buffer, "%6d", gyro[2]);
        // OLED_ShowString(15*6,7,oled_buffer,8);
        OLED_ShowTimer();

        

    }
}


  /**
   * @brief OLED 显示运行计时 + 球位置 (每 500ms 刷新)
   * @note  非阻塞, 在 main() 的 while(1) 里调用
   */
  void OLED_ShowTimer(void)
  {   

      static uint32_t last_oled = 0;
      if (Tick - last_oled < 800) return;   // 500ms 刷新一次
      last_oled = Tick;
     
      uint8_t t = (tick_ms - tick_start) / 1000;            // 运行秒数
      uint8_t  mm = t / 60;
      uint8_t  ss = t % 60;

      /* 第一行: 计时器 */
      OLED_ShowString(0, 0, (uint8_t*)"T:", 16);
      OLED_ShowNum(16, 0, mm, 2, 16);
      OLED_ShowChar(32, 0, ':', 16);
      OLED_ShowNum(40, 0, ss, 2, 16);

    //   /* 第二行: 球实际位置 */
    //   OLED_ShowString(0, 2, (uint8_t*)"P:", 16);
    //   if (bal.ball_now < 0) {
    //       OLED_ShowChar(16, 2, '-', 16);
    //       OLED_ShowNum(24, 2, (uint32_t)(-bal.ball_now * 10), 2, 16);
    //   } else {
    //       OLED_ShowNum(24, 2, (uint32_t)(bal.ball_now * 10), 2, 16);
    //   }
  }








