#ifndef _TASK_H_
#define _TASK_H_
#include "headfile.h"

void task3(void);
void StartTurn(float angle);
bool TurnFinished(void);
bool LostLine(void);
bool FindLine(void);
float GetDistance(void);
void ResetDistance(void);
void SlowForward(void);

#endif