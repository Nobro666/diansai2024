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
#include "control.h"

extern unsigned short Anolog[8] ;    // 存储当前模拟量值的数组
extern unsigned short white[8] ;     // 存储白色校准值的数组 
extern unsigned short black[8];     // 存储黑色校准值的数组
extern unsigned short Normal[8];          // 归一化值数组
extern No_MCU_Sensor sensor;


int main(void)
{
    // 初始化系统配置
    SYSCFG_DL_init();
	
    // 初始化LED
    LED_init();
   
    SYSCFG_DL_UART_0_init();
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

    DL_TimerA_startCounter(PWM_MOTOR_INST);
    Motor_On();
		
    /* 主应用程序循环 */
    while (1) {
        Control();
    }
}








// #include "ti_msp_dl_config.h"
// #include "motor.h"

// int16_t rxbuf = 0, cx = 160;
// int16_t basespeed = 30;
// int16_t SpeedL = 0, SpeedR = 0;
// float Kp = 0.5;

// void Motor_Ctrl(void);

// int main(void)
// {
//     SYSCFG_DL_init();
//     DL_TimerA_startCounter(PWM_MOTOR_INST);
//     Set_Speed(0,basespeed);
//     Set_Speed(1,basespeed);
    
//     Motor_On();

//     while (1) 
//     {
//       Motor_Ctrl();
//     }
// }

// void Motor_Ctrl(void)
// {
//     SpeedL = basespeed;
//     SpeedR = basespeed;
    

//     if(SpeedL > 30)
//        SpeedL = 30;
//     else if(SpeedL < 0)
//        SpeedL = 0;

//     if(SpeedR > 30)
//        SpeedR = 30;
//     else if(SpeedR < 0)
//        SpeedR = 0;
// }

