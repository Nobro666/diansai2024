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
void Trace_Follow(void);
void Trace_GoStraight(bool relock);
void Trace_TurnTo(float angle);
void Trace_Search(void);
bool Trace_TurnDone(void);
void Trace_Stop(void);


#endif