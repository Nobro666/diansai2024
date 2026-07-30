/* ==================== balance.c ==================== */
#include "balance.h"
#include "mpu6050.h"

BalanceCtrl bal;

void Balance_Init(void)
{
    PID_Init(&bal.pos_pid, POSITION, 10.0f, 2.0f,  2.0f, 0.01, 0);
    PID_Init(&bal.ang_pid, POSITION, 30.0f, 5.0f,  3.5f, 0.01, 0);
    bal.ball_target = 0.0f;
    bal.ball_now    = 0.0f;
    bal.servo_pwm   = 1500;
    DL_TimerG_setCaptureCompareValue(PWM_Servo_INST, 1500, DL_TIMER_CC_0_INDEX);
}

void Balance_SetTarget(float cm)   
{ bal.ball_target = cm; }

void Balance_FeedBallPos(float cm) 
{ bal.ball_now = cm; }

void Balance_Control(void)
{
    Read_Quad();
    float body_pitch = pitch;

    /* 位置环: 球误差→期望倾角 */
    bal.ang_desired = PID_Calc(&bal.pos_pid, bal.ball_now, bal.ball_target);

    /* 实际倾角 = 车身pitch + 连杆角度 */
    float rod_angle = (bal.servo_pwm - 1500) / 11.1f;
    bal.ang_actual  = body_pitch + rod_angle;

    /* 倾角环: 倾角误差→舵机PWM */
    float corr = PID_Calc(&bal.ang_pid, bal.ang_actual, bal.ang_desired);
    bal.servo_pwm += (int32_t)(corr * 11.1f);

    if (bal.servo_pwm < 1000)  
        bal.servo_pwm = 1000;
    if (bal.servo_pwm > 2000) 
        bal.servo_pwm = 2000;

    DL_TimerG_setCaptureCompareValue(PWM_Servo_INST, (uint32_t)bal.servo_pwm, DL_TIMER_CC_0_INDEX);
}