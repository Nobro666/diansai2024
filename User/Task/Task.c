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


extern unsigned char Digtal;
extern uint8_t drive_mode;
extern float target_angle;

//   void task3(void)
//   {
//       static bool turning = false;

//       /* 脱线检测 */
//       if (!turning && Digtal == 0xFF) {
//           turning      = true;
//           drive_mode   = 1;
//           target_angle = 60;
//       }

//       /* 等 Turn_angel 完成 */
//       if (turning && drive_mode == 0) {
//           turning = false;
//       }

//       Control();
//   }


//     void task3(void)
//   {
//       static bool done = false;

//       /* 脱线时触发一次转向 */
//       if (!done && Digtal == 0xFF) {
//           drive_mode   = 1;
//           target_angle = 60;
//           done         = true;   // 只转一次，之后直走
//       }

//       Control();
//   }


//       void task3(void)
//   {
//       static bool   done    = false;
//       static bool   turning = false;
//       static uint32_t start_tick = 0;

//       /* 记录首次调用时刻 */
//       if (start_tick == 0) start_tick = Tick;

//       /* 上电前 2 秒不管黑白，直走 */
//       if (Tick - start_tick < 2000) {
//           Control();
//           return;
//       }
       
//        Control();
//       /* 脱线检测 */
//       if (!turning && !done && Digtal == 0xFF) {
//           turning      = true;
//           drive_mode   = 1;
//           target_angle = 60;
//       }

//       /* 等 Turn_angel 完成 */
//       if (turning && drive_mode == 0) {
//           turning = false;
//           done    = true;
//       }

//       Control();
//   }


//  void task3(void)
//   {
//       static bool   done       = false;
//       static bool   turning    = false;
//       static uint32_t start_tick = 0;

//       if (start_tick == 0) start_tick = Tick;

//       Control();

//       /* 2 秒后才启用脱线检测 */
//       if (Tick - start_tick >= 2000 && !turning && !done && Digtal == 0xFF) {
//           turning      = true;
//           drive_mode   = 1;
//           target_angle = 60;
//       }

//       if (turning && drive_mode == 0) {
//           turning = false;
//           done    = true;
//       }
//   }


//   void task3(void)
//   {
//       static bool   done        = false;
//       static bool   turning     = false;
//       static uint32_t start_tick = 0;
//       static int    lost_count  = 0;

//       if (start_tick == 0) start_tick = Tick;

//       Control();

//       if (Tick - start_tick >= 2000 && !turning && !done) {
//           if (Digtal == 0xFF) {
//               lost_count++;
//               if (lost_count >= 5) {   // 连续 5 帧全白才判脱线
//                   turning      = true;
//                   drive_mode   = 1;
//                   target_angle = 60;
//                   lost_count   = 0;
//               }
//           } else {
//               lost_count = 0;   // 看到线就清零
//           }
//       }

//       if (turning && drive_mode == 0) {
//           turning = false;
//           done    = true;
//       }
//   }


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
                  target_angle = 58;
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