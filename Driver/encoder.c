#include "encoder.h"
#include "delay.h"

/* 全局编码器实例 */
EncoderUnit encL, encR;

/**
 * @brief 初始化编码器
 * @note  启用相位A引脚的中断(单边沿), 相位B仅用于方向判断
 *        配置内部上拉电阻防止浮空
 */
void Encoder_Init(EncoderUnit *enc,
                  GPIO_Regs *portA, uint16_t pinA,
                  GPIO_Regs *portB, uint16_t pinB,
                  uint16_t ppr)
{
    enc->portA  = portA;
    enc->pinA   = pinA;
    enc->portB  = portB;
    enc->pinB   = pinB;
    enc->ppr    = ppr;
    enc->count  = 0;
    enc->lastCount = 0;
    enc->speed_rpm = 0.0f;
    enc->lastTick  = Tick;

    /* 相位B仅用于方向判断，禁用中断 */
    DL_GPIO_disableInterrupt(portB, pinB);

    /* 为相位A引脚启用中断(上升沿), 内部上拉由SysConfig GPIO配置提供 */
    DL_GPIO_clearInterruptStatus(portA, pinA);
    DL_GPIO_enableInterrupt(portA, pinA);
}

/**
 * @brief 获取编码器累计脉冲计数
 */
int32_t Encoder_GetCount(EncoderUnit *enc)
{
    return enc->count;
}

/**
 * @brief 计算转速 (RPM)
 */
float Encoder_CalcSpeed(EncoderUnit *enc)
{
    int32_t delta = enc->count - enc->lastCount;
    uint32_t dt   = Tick - enc->lastTick;
    enc->lastCount = enc->count;
    enc->lastTick  = Tick;

    if (dt > 0 && enc->ppr > 0) {
        enc->speed_rpm = (float)delta / (float)enc->ppr * 60000.0f / (float)dt;
    } else {
        enc->speed_rpm = 0.0f;
    }
    return enc->speed_rpm;
}

/**
 * @brief 获取本次delta计数，用于电机PID速度反馈
 */
int32_t Encoder_GetDelta(EncoderUnit *enc)
{
    int32_t cur  = enc->count;
    int32_t delta = cur - enc->lastCount;
    enc->lastCount = cur;
    return delta;
}

/**
 * @brief GPIOA 编码器中断处理
 * @note  PA7(encL相位A) 和 PA28(encR相位A) 的中断在此处理
 *        通过读取相位B引脚电平判断方向
 *        IIDX = 引脚号 (DL_GPIO_IIDX_DIO7 = 7, DL_GPIO_IIDX_DIO28 = 28)
 */
void Encoder_HandleGPIOA(uint32_t pending)
{
    /* encL: PA7 = 相位A, PA26 = 相位B */
    if (pending & DL_GPIO_IIDX_DIO7) {
        if (DL_GPIO_readPins(encL.portB, encL.pinB)) {
            encL.count++;   /* B为高 → 正转 */
        } else {
            encL.count--;   /* B为低 → 反转 */
        }
        DL_GPIO_clearInterruptStatus(GPIOA, DL_GPIO_PIN_7);
    }

    /* encR: PA28 = 相位A, PB6 = 相位B */
    if (pending & DL_GPIO_IIDX_DIO28) {
        if (DL_GPIO_readPins(encR.portB, encR.pinB)) {
            encR.count++;
        } else {
            encR.count--;
        }
        DL_GPIO_clearInterruptStatus(GPIOA, DL_GPIO_PIN_28);
    }
}

/**
 * @brief GPIOB 编码器中断处理
 * @note  当前配置中 PB6 是 encR 的相位B(仅方向判断, 不产生中断)
 *        但若硬件噪声误触发, 清除中断防止风暴
 */
void Encoder_HandleGPIOB(uint32_t pending)
{
    if (pending) {
        DL_GPIO_clearInterruptStatus(GPIOB, pending);
    }
}
