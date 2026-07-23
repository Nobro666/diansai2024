  #ifndef ENCODER_DISTANCE_H
  #define ENCODER_DISTANCE_H
  #include "encoder.h"

  #define WHEEL_DIAMETER_MM  65.0f            // 轮子直径 (RADIUS_OF_TYRE * 2)
  #define WHEEL_CIRCUM_MM    (3.1415926535897f * WHEEL_DIAMETER_MM)  // 周长 ≈ 204.2mm
  #define PULSES_PER_WHEEL   (PULSE_PRE_ROUND * MOTOR_SPEED_RERATIO) // 330

  /**
   * @brief 获取编码器累计行驶距离 (毫米)
   * @param enc  编码器实例 (&encL 或 &encR)
   * @return 距离, 正转前进为正, 反转后退为负
   */
  float Encoder_GetDistance_mm(EncoderUnit *enc);

  /**
   * @brief 获取编码器累计行驶距离 (厘米)
   */
  float Encoder_GetDistance_cm(EncoderUnit *enc);

  /**
   * @brief 清零编码器累计脉冲 (重新计距)
   */
  void Encoder_ResetDistance(EncoderUnit *enc);

  #endif