#ifndef MENU_H
#define MENU_H
#include "headfile.h"
#include <stdbool.h>

#define MENU_COUNT  5
#define MENU_DBCLK  600    // 双击窗口 600ms
#define MENU_IDLE_TO  600  // 单击确认超时

typedef enum {
    MENU_IDLE,        // 菜单浏览
    MENU_RUNNING      // 任务执行中
} MenuState;

typedef struct {
    MenuState state;
    uint8_t   cursor;             // 当前选中 0~4
    uint32_t  last_click;         // 上次按键时刻(Tick)
    uint8_t   click_cnt;          // 有效单击次数(600ms内)
} Menu;

extern Menu menu;

void Menu_Init(void);
void Menu_Update(void);
void Menu_ShowOLED(void);
#endif
