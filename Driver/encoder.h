#ifndef ENCODER_H
#define ENCODER_H
#include "ti_msp_dl_config.h"

/**
 * @brief 编码器实例结构体 (GPIO外部中断 + 软件方向判断)
 */
typedef struct {
    GPIO_Regs *portA;       // 相位A(计数引脚)端口
    uint16_t pinA;          // 相位A引脚掩码
    GPIO_Regs *portB;       // 相位B(方向引脚)端口
    uint16_t pinB;          // 相位B引脚掩码
    volatile int32_t count; // 累计脉冲计数(正转++, 反转--)
    int32_t lastCount;      // 上次速度计算时的计数值
    float speed_rpm;        // 转速 (RPM)
    uint16_t ppr;           // 每转脉冲数
    uint32_t lastTick;      // 上次速度计算时刻(Tick)
} EncoderUnit;

/**
 * @brief 初始化编码器
 * @param enc  编码器实例
 * @param portA 相位A端口
 * @param pinA  相位A引脚
 * @param portB 相位B端口
 * @param pinB  相位B引脚
 * @param ppr   每转脉冲数
 * @note  初始化后需要在 GROUP1_IRQHandler 中调用 Encoder_HandleGPIOA/B
 */
void Encoder_Init(EncoderUnit *enc,
                  GPIO_Regs *portA, uint16_t pinA,
                  GPIO_Regs *portB, uint16_t pinB,
                  uint16_t ppr);

/**
 * @brief 获取编码器累计脉冲计数
 */
int32_t Encoder_GetCount(EncoderUnit *enc);

/**
 * @brief 计算转速并返回 (RPM)
 * @note  周期性调用(如每10ms)，内部用 Tick 计算时间间隔
 */
float Encoder_CalcSpeed(EncoderUnit *enc);

/**
 * @brief 获取用于电机PID的 deltaCount (本次与上次读数之差)
 * @note  调用后将 lastCount 更新为当前 count
 */
int32_t Encoder_GetDelta(EncoderUnit *enc);

/* GPIOA编码器中断处理(PA7=encL, PA28=encR相位A) */
void Encoder_HandleGPIOA(uint32_t pending);

/* GPIOB编码器中断处理(PB6=encR相位B, 仅清除) */
void Encoder_HandleGPIOB(uint32_t pending);

/* 全局编码器实例 */
extern EncoderUnit encL, encR;

#endif /* ENCODER_H */
