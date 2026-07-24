#ifndef _CONTROL_H_
#define _CONTROL_H_
#include "headfile.h"

void Trace_init(void);
void Control(void);
void Motor_Ctrl(float err);
void Turn_angel(float angel);
float Calculate_Position_Error(unsigned char digtal);
float Calculate_Heading_Error(float target, float current);
void RunStraight(void);
void SlowForward(void);
#endif
