#ifndef _CONTROL_H_
#define _CONTROL_H_
#include "headfile.h"

void Trace_init(void);
void Control(void);
void Motor_Ctrl(void);
float Calculate_Position_Error(unsigned char digtal);
#endif
