#ifndef __MENU_H
#define __MENU_H

#include "main.h"
#include "OLED.h"
#include "Draw_3D.h"
#include "Global.h"
/*****按键状态变量定义(start)******/
#define KEY_NUM 4
#define KEY_DEBOUNCE_TIME 1 // 10ms消抖

// 按键引脚定义（根据实际修改）
#define KEY0_GPIO_Port GPIOB
#define KEY0_Pin GPIO_PIN_12
#define KEY1_GPIO_Port GPIOB
#define KEY1_Pin GPIO_PIN_13
#define KEY2_GPIO_Port GPIOB
#define KEY2_Pin GPIO_PIN_14
#define KEY3_GPIO_Port GPIOB
#define KEY3_Pin GPIO_PIN_15
//映射按键行为
#define KEY_UP_IDX     0   // 对应GPIO的上键
#define KEY_DOWN_IDX   1   // 对应GPIO的下键
#define KEY_OK_IDX     2   // 对应GPIO的确认键
#define KEY_BACK_IDX   3   // 对应GPIO的返回键

typedef enum {
    KEY_RELEASED = 0,
    KEY_DEBOUNCE,
    KEY_PRESSED
} KeyState;

typedef struct {
    KeyState state;
    uint16_t counter;
    uint8_t  short_press_flag;
} KeyInfo;
/*****按键状态变量定义(end)******/

/****外部调用的菜单功能接口(start)*********/
extern uint8_t page; // 页面状态量，0表示菜单显示，1表示进入功能页面
/****外部调用的菜单功能接口(end)***********/


//菜单结构体
typedef struct Menu {
    char *name;
    int id;
    void (*func)();
    struct Menu *parent;
    struct Menu *child;
    struct Menu *sibling;
} Menu;

/*****菜单可视化相关宏(start)*******/
#define MENU_MAX_ROW 4
#define FONT_H 16


/*****菜单可视化相关宏(end)*******/
//菜单可视化
void OLED_DrawMenu(Menu *selected_menu);


//按键状态变量

//行为枚举
//0表示上
//1表示向下
//2表示确定
//3表示返回
//4表示无任务
typedef enum {
    MENU_UP,
    MENU_DOWN,
    MENU_OK,
    MENU_BACK,
    WAITING
} MenuAction;


// 回调函数
void option_func(void);
void mode_func(void);
void main_menu_func(void);
void read_to_fly(void);
//菜单初始化
void init_menu_parents(void);

//菜单切换函数
Menu* menu_navigate(Menu* current, MenuAction action);
//按键扫描函数
void scan_keys(void);
//获取菜单动作
MenuAction get_menu_action(void);


#endif
