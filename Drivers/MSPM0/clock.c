#include "ti_msp_dl_config.h"
#include "clock.h"
#include "delay.h"
 

volatile unsigned long tick_ms;
volatile uint32_t start_time;

int mspm0_delay_ms(unsigned long num_ms)
{
    while (num_ms--) {
        delay_us(1000);
        Tick++;
        tick_ms++;
    }
    return 0;
}

int mspm0_get_clock_ms(unsigned long *count)
{
    if (!count)
        return 1;
    count[0] = Tick;
    return 0;
}

void SysTick_Init(void)
{
    DL_SYSTICK_config(CPUCLK_FREQ/1000);
    NVIC_SetPriority(SysTick_IRQn, 0);
}
