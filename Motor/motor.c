#include "motor.h"

float motor_max_duty=100;

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