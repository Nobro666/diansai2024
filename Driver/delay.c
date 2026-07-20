#include "delay.h"

extern volatile unsigned long tick_ms;
// SysTick�жϷ�������(1ms)
void SysTick_Handler(void) {
    Tick_SysTickCallback();
    tick_ms++;
}
volatile uint32_t Tick = 0;
volatile uint32_t Reg_tick=0;
void delay_us(uint32_t us) {
    // ����ʵ�ʲ��Ե�����ֵ
    // 32MHz�´�Լ��Ҫ (us * 32) ��ѭ������У׼��
    volatile uint32_t count = us * 8;  // ~1µs/loop at 32MHz (volatile, ~4 cycles/iter)
    while(count--);
}
/**
 * @brief ��ʱ(ʹ��SysTick�жϼ�ʱ)
 * @param t ��ʱʱ��(ms)
*/
void Tick_delay(uint32_t t) {
    uint32_t tEnd = Tick + t;
    while (Tick < tEnd);
}

// SysTick�жϻص�(1ms)
void Tick_SysTickCallback(void) {
  Tick++;
}



void TIMER_0_INST_IRQHandler(void){
    volatile uint8_t index1 = 0;
	Reg_tick++;
}