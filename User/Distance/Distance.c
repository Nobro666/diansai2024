  #include "Distance.h"
  #include "motor.h"

  float Encoder_GetDistance_mm(EncoderUnit *enc)
  {
      return (float)enc->count / PULSES_PER_WHEEL * WHEEL_CIRCUM_MM;
  }

  float Encoder_GetDistance_cm(EncoderUnit *enc)
  {
      return Encoder_GetDistance_mm(enc) / 10.0f;
  }

  void Encoder_ResetDistance(EncoderUnit *enc)
  {
      enc->count = 0;
  }

