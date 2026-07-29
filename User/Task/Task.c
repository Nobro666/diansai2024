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
#include "BUZZER.h"


extern unsigned char Digtal;
extern uint8_t drive_mode;
extern float target_angle;
extern bool  heading_relock;
extern Motor motor_l;
extern Motor motor_r;
extern PID tracking_pid;


static bool FindLine(void)
{
    static uint16_t line_cnt = 0;

    if (Digtal != 0xFF) {
        if (line_cnt < 2) line_cnt++;
    } else {
        line_cnt = 0;
    }
    if (line_cnt >= 2) {
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


static bool StopLineDetect(void)
{
    static uint8_t stop_cnt = 0;

    uint8_t black_num = 0;

    /* 统计黑色探头数量 */
    for(uint8_t i = 0; i < 8; i++)
    {
        if(((Digtal >> i) & 0x01) == 0)     // 黑色=0
        {
            black_num++;
        }
    }

    /* 至少6个探头同时检测到黑线 */
    if(black_num >= 6)
    {
        if(stop_cnt < 5)
            stop_cnt++;
    }
    else
    {
        stop_cnt = 0;
    }

    /* 连续20帧认为是真正停车线 */
    if(stop_cnt >= 5)
    {
        stop_cnt = 0;
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


/* ==================== 状态机辅助函数 ==================== */

void Trace_Follow(void) {
    drive_mode = 0;
    Control();
}

void Trace_GoStraight(bool relock) {
    drive_mode     = 2;
    heading_relock = relock;
    Control();
}

void Trace_TurnTo(float angle) {
    drive_mode   = 1;
    target_angle = angle;
    Control();
}

void Trace_Search(void) {
    drive_mode = 3;
    Control();
}

bool Trace_TurnDone(void) {
    return (drive_mode == 0);
}

void Trace_Stop(void) {
    motor_l.Driver(&motor_l, 0);
    motor_r.Driver(&motor_r, 0);
}


void task1(void)
{
    static bool finish=false;

    if(finish)
    {
        Trace_Stop();
        return;
    }

    Trace_Follow();

    if(GetDistance()>388.0f)
    {
        finish=true;
    }
}


/* K230 视觉模块 UART 接收 */
static char   k230_buf[32];
static uint8_t k230_idx = 0;
void UART_k230_INST_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(UART_k230_INST)) {
        case DL_UART_MAIN_IIDX_RX:
            // DL_GPIO_togglePins(GPIO_LEDS_PORT,
            //     GPIO_LEDS_USER_LED_1_PIN | GPIO_LEDS_USER_TEST_PIN);
            // gEchoData = DL_UART_Main_receiveData(UART_k230_INST);
            // DL_UART_Main_transmitData(UART_k230_INST, gEchoData);
            break;
        default:
            break;
    }
}



// /******************************************************************************
//  * @brief  task3 — 沿矩形轨迹行驶 A→C→B→D→A
//  ******************************************************************************/
// #define AC_ANGLE     -39
// #define AC_DISTANCE  125
// #define BD_ANGLE     47
// #define BD_DISTANCE  130

// typedef enum
// {
//     TURN_START,
//     GO_AC,
//     FIND_RIGHT_ARC,
//     TRACE_CB,
//     GO_BD,
//     FIND_LEFT_ARC,
//     TRACE_DA,
//     FINISH
// } TASK_STATE;


// void task3(void)
// {
//     static TASK_STATE state = TURN_START;

//     switch (state) {

//     /*************** 初始原地转向 ***************/
//     case TURN_START:
//         Trace_TurnTo(AC_ANGLE);
//         if (Trace_TurnDone()) {
//             ResetDistance();
//             state = GO_AC;
//         }
//         break;

//     /*************** A→C: 锁定直行 ***************/
//     case GO_AC:
//         Trace_GoStraight(GetDistance() == 0);
//         if (GetDistance() > AC_DISTANCE) 
//         state = FIND_RIGHT_ARC;
//         break;

//     /*************** 找右半圆(慢速前进等线) ***************/
//     case FIND_RIGHT_ARC:
//         Trace_Search();
//         if (FindLine()) 
//         {   
//             ALARM();
//             ResetDistance(); 
//             state = TRACE_CB; 
//         }
//         break;

//     /*************** C→B: 循迹 + 脱线转 BD_ANGLE ***************/
//     case TRACE_CB: {
//         static bool turning = false;

//         if (!turning) 
//         {
//             Trace_Follow();
//             if (LostLine()) 
//             {   
//                 ALARM();
//                 ResetDistance(); turning = true; 
//             }
//         } 
//         else 
//         {
//             Trace_TurnTo(BD_ANGLE);
//             if (Trace_TurnDone())
//             { 
//                 turning = false; state = GO_BD; 
//             }
//         }
//         break;
//     }

//     /*************** B→D: 锁定直行 ***************/
//     case GO_BD:
//         Trace_GoStraight(true);
//         if (GetDistance() > BD_DISTANCE) 
//         state = FIND_LEFT_ARC;
//         break;

//     /*************** 找左半圆(慢速前进等线) ***************/
//     // case FIND_LEFT_ARC:
//         // Trace_Search();
//         // if (FindLine()) 
//         // {   
//         //     // PID_clear(&tracking_pid); 
//         //     ALARM();
//         //     ResetDistance(); 
//         //     Trace_TurnTo(-45);
//         //     if (Trace_TurnDone())
//         //     { 
//         //         Trace_Follow();
//         //         state = TRACE_DA; 
//         //     }
            
//         //     // Trace_Search();
//         //     // if (GetDistance() > 30)
//         //     // state = TRACE_DA; 
//         // }
//         // break;
//     case FIND_LEFT_ARC: {
//       static bool found   = false;
//       static bool turning = false;

//       if (!found) {
//           Trace_Search();
//           if (FindLine()) {
//               ALARM();
//               ResetDistance();
//               found   = true;
//               turning = true;
//           }
//       } else if (turning) {
//           Trace_TurnTo(-45);
//           if (Trace_TurnDone()) {
//               turning = false;
//               state = TRACE_DA;
//           }
//       }
//       break;
//   }




//     /*************** D→A: 循迹 + 脱线停车 ***************/
//     case TRACE_DA:
//         Trace_Follow();
//         if (LostLine()) 
//         {   
//             ALARM();
//             Trace_Stop(); 
//             state = FINISH; 
//         }
//         break;

//     case FINISH:
//         break;
//     }
// }
