#ifndef FLASH_H
#define FLASH_H

#include "ti_msp_dl_config.h"

/* Flash存储配置 */
#define FLASH_WHITE_DATA_ADDR  (0x0000F000) // 偏移地址
#define FLASH_BLACK_DATA_ADDR  (0x0000F010) // 偏移地址
/* 函数声明 */
void writeToFlash(void);
void readWhiteFromFlash(void);
void readBlackFromFlash(void);
#endif // ADC_H
