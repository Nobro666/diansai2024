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
extern Motor motor_l;
extern Motor motor_r;

static void StartTurn(float angle)
{
    drive_mode = 1;
    target_angle = angle;
}

static bool TurnFinished(void)
{
    return (drive_mode == 0);
}

static bool FindLine(void)
{
    static uint16_t line_cnt = 0;

    if(Digtal != 0xFF)          // 有黑线
    {
        if(line_cnt < 80)
            line_cnt++;
    }
    else
    {
        line_cnt = 0;
    }

    if(line_cnt >= 80)
    {
        line_cnt = 0;           // 防止重复进入
        return true;
    }

    return false;
}

static bool LostLine(void)
{
    static uint16_t lost_cnt = 0;

    if(Digtal == 0xFF)          // 全白
    {
        if(lost_cnt < 80)
            lost_cnt++;
    }
    else
    {
        lost_cnt = 0;
    }

    if(lost_cnt >= 80)
    {
        lost_cnt = 0;           // 防止重复进入
        return true;
    }

    return false;
}


static float GetDistance(void)
{
    return (Encoder_GetDistance_cm(&encL)
          + Encoder_GetDistance_cm(&encR))*0.5f;
}

static void ResetDistance(void)
{
    Encoder_ResetDistance(&encL);
    Encoder_ResetDistance(&encR);
}



/******************************************************************************
 * @brief  慢速寻找黑线
 ******************************************************************************/
#define FindSpeed 18

void SlowForward(void)
{
    motor_l.Driver(&motor_l, (int32_t)FindSpeed);
    motor_r.Driver(&motor_r, (int32_t)FindSpeed);
}





  void task3(void)
  {
      static bool   done       = false;
      static bool   turning    = false;
      static int    loop_cnt   = 0;
      static int    lost_cnt   = 0;

      Control();
      loop_cnt++;

      /* 上电前 200 轮不触发脱线（约 2 秒） */
      if (loop_cnt < 200) return;

      if (!turning && !done) {
          if (Digtal == 0xFF) {
              lost_cnt++;
              if (lost_cnt >= 80) {
                  turning      = true;
                  drive_mode   = 1;
                  target_angle = 60;
                  lost_cnt     = 0;
              }
          } else {
              lost_cnt = 0;
          }
      }

      if (turning && drive_mode == 0) {
          turning = false;
          done    = true;
      }
  }