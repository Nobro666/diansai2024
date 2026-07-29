#include "BUZZER.h"
#include "Task.h"
int notify_tick;
int notify_flag;


void BEE_OFF(void)
{
    DL_GPIO_setPins(BUZZER_PORT,BUZZER_PIN_BUZZER_PIN);
}

void BEE_ON(void)
{
    DL_GPIO_clearPins(BUZZER_PORT,BUZZER_PIN_BUZZER_PIN);
}


void LED_ON(void)
{
    DL_GPIO_setPins(LED_PORT,LED_KEY_PIN);
}

void LED_OFF(void)
{
    DL_GPIO_clearPins(LED_PORT,LED_KEY_PIN);
}

void ALARM(void)
{   
    Trace_Stop();
    BEE_ON();
    LED_ON();
    mspm0_delay_ms(150);
    BEE_OFF();
    LED_OFF();
}


