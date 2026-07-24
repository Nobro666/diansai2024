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
#include "Distance.h"


extern unsigned char Digtal;
extern uint8_t drive_mode;
extern float target_angle;
extern bool  heading_relock;
extern Motor motor_l;
extern Motor motor_r;


static bool FindLine(void)
{
    static uint16_t line_cnt = 0;

    if (Digtal != 0xFF) {
        if (line_cnt < 5) line_cnt++;
    } else {
        line_cnt = 0;
    }
    if (line_cnt >= 5) {
        line_cnt = 0;
        return true;
    }
    return false;
}


static bool LostLine(void)
{
    static uint16_t lost_cnt = 0;

    if (Digtal == 0xFF) {
        if (lost_cnt < 80) lost_cnt++;
    } else {
        lost_cnt = 0;
    }
    if (lost_cnt >= 80) {
        lost_cnt = 0;
        return true;
    }
    return false;
}


static float GetDistance(void)
{
    return (Encoder_GetDistance_cm(&encL)
          + Encoder_GetDistance_cm(&encR)) * 0.5f;
}


static void ResetDistance(void)
{
    Encoder_ResetDistance(&encL);
    Encoder_ResetDistance(&encR);
}




/******************************************************************************
 * @brief  task3 — 沿矩形轨迹行驶 A→C→B→D→A
 ******************************************************************************/
#define AC_ANGLE     39
#define AC_DISTANCE  120
#define BD_ANGLE     -30
#define BD_DISTANCE  120

typedef enum
{
    GO_AC,
    FIND_RIGHT_ARC,
    TRACE_CB,
    GO_BD,
    FIND_LEFT_ARC,
    TRACE_DA,
    FINISH
} TASK_STATE;


void task3(void)
{
    static TASK_STATE state = GO_AC;

    switch (state) {

    /*************** A→C: 锁定直行 ***************/
    case GO_AC:
        drive_mode     = 2;
        heading_relock = (GetDistance() == 0);   // 第一帧锁航向
        Control();
        if (GetDistance() > AC_DISTANCE) 
        state = FIND_RIGHT_ARC;
        break;

    /*************** 找右半圆(慢速前进等线) ***************/
    case FIND_RIGHT_ARC:
        drive_mode = 3;
        Control();
        if (FindLine()) {
            ResetDistance();
            drive_mode = 0;
            Control();
            state = TRACE_CB;
        }
        break;

    /*************** C→B: 循迹 + 脱线转 BD_ANGLE ***************/
    case TRACE_CB: {
        static bool turning = false;

        if (!turning) 
        drive_mode = 0;
        Control();

        if (!turning && LostLine()) {
            ResetDistance();
            drive_mode   = 1;
            target_angle = BD_ANGLE;
            turning      = true;
        }
        if (turning && drive_mode == 0) {   // Turn_angel 到位
            turning = false;
            state   = GO_BD;
        }
        break;
    }

    /*************** B→D: 锁定直行 ***************/
    case GO_BD: {
        static bool first_frame = true;

        drive_mode     = 2;
        heading_relock = first_frame;
        first_frame    = false;
        Control();
        if (GetDistance() > BD_DISTANCE) {
            first_frame = true;
            state = FIND_LEFT_ARC;
        }
        break;
    }

    /*************** 找左半圆(慢速前进等线) ***************/
    case FIND_LEFT_ARC:
        drive_mode = 3;
        Control();
        if (FindLine()) {
            ResetDistance();
            drive_mode = 0;
            Control();
            state = TRACE_DA;
        }
        break;

    /*************** D→A: 循迹 + 脱线停车 ***************/
    case TRACE_DA:
        drive_mode = 0;
        Control();
        if (LostLine()) {
            motor_l.Driver(&motor_l, 0);
            motor_r.Driver(&motor_r, 0);
            state = FINISH;
        }
        break;

    case FINISH:
        break;
    }
}
