#include "adc.h"
#include "string.h"

uint16_t ADC_VALUE[40];

// 读取ADC的数据
unsigned int adc_getValue(unsigned int number)
{
    // 1. 由于后面是轮询，没必要用 memset 清零，用标志位更安全
    // 也可以保留 memset 但确保 DMA 没有在跑
    // memset((uint16_t*)ADC_VALUE, 0, sizeof(ADC_VALUE)); 
    
    // 2. 启动 ADC 采样 (取决于您的库函数怎么写，这里假设已经有触发动作)
    // DL_ADC12_startConversion(ADC_VOLTAGE_INST);

    // 3. 加入超时机制，防止死循环卡死（这是防止卡死的关键！）
    uint32_t timeout = 100000; // 设置一个超时计数器
    while(ADC_VALUE[number-1] == 0) {
        // 即使CPU全速跑循环，也不会陷入休眠（去掉了 __WFI()）
        
        timeout--;
        if(timeout == 0) {
            // 如果超时了还没数据，说明ADC或DMA配置有误，直接返回 0 报错
            return 0; 
        }
    }

    // 4. 此时 ADC_VALUE 已更新，开始求和
    unsigned int gAdcResult = 0;
    unsigned char i = 0;
    for( i = 0; i < number; i++ ) {
        gAdcResult += ADC_VALUE[i];
    }
    
    // 均值滤波
    gAdcResult /= number;

    return gAdcResult;
}
