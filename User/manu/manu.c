#include "manu.h"
#include "oled_software_i2c.h"
#include "Task.h"
#include "key.h"
#include "clock.h"

/* 任务函数, 需要在其他 .c 里定义好 */
extern void task1(void);
extern void task2(void);
extern void task3(void);
extern void task4(void);
extern void task5(void);

/* 计时起始 Tick */
extern uint32_t tick_start;

Menu menu;

void Menu_Init(void)
{
    menu.state      = MENU_IDLE;
    menu.cursor     = 0;
    menu.last_click = 0;
    menu.click_cnt  = 0;
}

void Menu_Update(void)
{
    if (!key_pressed) return;
    key_pressed = 0;

    uint32_t now = tick_ms;

    /* ---- 超时复位 ---- */
    if (now - menu.last_click > MENU_DBCLK) {
        /* 如果在 IDLE 状态下只点了一次, 超时后执行单击(切光标) */
        if (menu.state == MENU_IDLE && menu.click_cnt == 1) {
            menu.cursor = (menu.cursor + 1) % MENU_COUNT;
        }
        menu.click_cnt = 0;
    }

    menu.click_cnt++;
    menu.last_click = now;

    /* ---- 双击处理 ---- */
    if (menu.click_cnt >= 2) {
        menu.click_cnt = 0;

        if (menu.state == MENU_RUNNING) {
            /* 任务执行中双击 → 退回菜单 */
            menu.state = MENU_IDLE;
        } else {
            /* 菜单界面双击 → 选中并执行 */
            menu.state = MENU_RUNNING;
            tick_start = tick_ms;
        }
    }
}

void Menu_ShowOLED(void)
{
    static uint8_t   last_cursor = 0xFF;
    static MenuState last_state  = 0xFF;
    static uint32_t  last_sec    = 0xFFFF;

    /* 运行中: 每秒刷新一次计时 */
    if (menu.state == MENU_RUNNING) {
        uint32_t sec = (tick_ms - tick_start) / 1000;
        if (sec == last_sec) return;   // 秒没变, 不刷
        last_sec    = sec;
        last_cursor = 0xFF;            // 强制菜单重绘, 下次切回菜单时刷新
        last_state  = 0xFF;
    }
    /* 菜单: 光标没变不刷 */
    else if (menu.cursor == last_cursor) {
        return;
    }

    last_cursor = menu.cursor;
    last_state  = menu.state;
    OLED_Clear();

    if (menu.state == MENU_IDLE) {
        OLED_ShowString(0, 0, (uint8_t*)"MENU", 16);
        for (int i = 0; i < MENU_COUNT; i++) {
            char line[8];
            line[0] = (i == menu.cursor) ? '>' : ' ';
            line[1] = 'T'; line[2] = '0' + i + 1; line[3] = 0;
            OLED_ShowString(0, 2 + i, (uint8_t*)line, 8);
        }
    } 
}
