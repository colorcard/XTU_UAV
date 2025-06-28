#include "Menu.h"

//按键变量
KeyInfo keys[KEY_NUM];
const GPIO_TypeDef* KEY_PORT[KEY_NUM] = {KEY0_GPIO_Port, KEY1_GPIO_Port, KEY2_GPIO_Port, KEY3_GPIO_Port};
const uint16_t      KEY_PIN [KEY_NUM] = {KEY0_Pin,       KEY1_Pin,       KEY2_Pin,       KEY3_Pin      };

//外部调用的数据
extern float Yaw;
extern float Pitch;
extern float Roll;
extern int16_t Encoder_Value;

VertexNode cube_vertices[NUM_VERTICES];


//按键扫描函数，初始化代码由HAL库生成
//该函数使用状态机，仅实现短按效果，需要拓展自行修改代码
//按键的检测结果（即按键是否被短按）存储在 keys[i].short_press_flag 变量中,该标志置位需要手动清零
void scan_keys(void) {
    for (uint8_t i = 0; i < KEY_NUM; i++) {
        uint8_t pin_level = (HAL_GPIO_ReadPin((GPIO_TypeDef*)KEY_PORT[i], KEY_PIN[i]) == GPIO_PIN_RESET) ? 1 : 0; // 按下为1

        switch (keys[i].state) {
        case KEY_RELEASED:
            if (pin_level) {
                keys[i].state = KEY_DEBOUNCE;
                keys[i].counter = 0;
            }
            break;
        case KEY_DEBOUNCE:
            if (pin_level) {
                if (++keys[i].counter >= KEY_DEBOUNCE_TIME) {
                    keys[i].state = KEY_PRESSED;
                }
            } else {
                keys[i].state = KEY_RELEASED;
            }
            break;
        case KEY_PRESSED:
            if (!pin_level) {
                keys[i].short_press_flag = 1; // 产生短按事件
                keys[i].state = KEY_RELEASED;
            }
            break;
        default:
            keys[i].state = KEY_RELEASED;
            break;
        }
    }
}

/**
 * @brief 将按键标志位转换为菜单动作
 * @return MenuAction 枚举值（包含WAITING表示无操作）
 */
MenuAction get_menu_action(void) {
    static const MenuAction key_mapping[KEY_NUM] = {
        [KEY_UP_IDX]   = MENU_UP,    // 上键
        [KEY_DOWN_IDX] = MENU_DOWN,  // 下键
        [KEY_OK_IDX]   = MENU_OK,    // 确认键
        [KEY_BACK_IDX] = MENU_BACK   // 返回键
    };

    for (uint8_t i = 0; i < KEY_NUM; i++) {
        if (keys[i].short_press_flag) {
            keys[i].short_press_flag = 0;  // 清除标志位
            return key_mapping[i];         // 返回映射后的动作
        }
    }
    return WAITING;  // 无按键动作
}


// Menu option items
//需要实现菜单选项功能在这里替换回调函数即可
Menu option_4_4 = {"section4", 19, option_func, NULL, NULL, NULL};
Menu option_4_3 = {"section3", 18, option_func, NULL, NULL, &option_4_4};
Menu option_4_2 = {"section2", 17, option_func, NULL, NULL, &option_4_3};
Menu option_4_1 = {"section1", 16, option_func, NULL, NULL, &option_4_2};

Menu option_3_4 = {"section4", 15, option_func, NULL, NULL, NULL};
Menu option_3_3 = {"section3", 14, option_func, NULL, NULL, &option_3_4};
Menu option_3_2 = {"section2", 13, option_func, NULL, NULL, &option_3_3};
Menu option_3_1 = {"section1", 12, option_func, NULL, NULL, &option_3_2};

Menu option_2_4 = {"section4", 11, option_func, NULL, NULL, NULL};
Menu option_2_3 = {"section3", 10, option_func, NULL, NULL, &option_2_4};
Menu option_2_2 = {"section2", 9, option_func, NULL, NULL, &option_2_3};
Menu option_2_1 = {"section1", 8, option_func, NULL, NULL, &option_2_2};

Menu option_1_4 = {"section4", 7, option_func, NULL, NULL, NULL};
Menu option_1_3 = {"section3", 6, option_func, NULL, NULL, &option_1_4};
Menu option_1_2 = {"section2", 5, option_func, NULL, NULL, &option_1_3};
Menu option_1_1 = {"fly", 4, read_to_fly, NULL, NULL, &option_1_2};

// Mode menu items
//Menu mode5 = {"Mode5", 4, mode_func, NULL, &option_5_1, NULL};
Menu mode4 = {"Mode4", 3, mode_func, NULL, &option_4_1, NULL};
Menu mode3 = {"Mode3", 2, mode_func, NULL, &option_3_1, &mode4};
Menu mode2 = {"Mode2", 1, mode_func, NULL, &option_2_1, &mode3};
Menu mode1 = {"Mode1", 0, mode_func, NULL, &option_1_1, &mode2};

// Main menu
Menu main_menu = {"Main Menu", -1, main_menu_func, NULL, &mode1, NULL};

// 初始化菜单的parent关系
// 这里设置每个菜单项的parent指针，便于在菜单系统中进行导航
void init_menu_parents(void) {
    // 模式的parent
    mode1.parent = &main_menu;
    mode2.parent = &main_menu;
    mode3.parent = &main_menu;
    mode4.parent = &main_menu;
    //mode5.parent = &main_menu;

    // 选项的parent
    option_1_1.parent = &mode1; option_1_2.parent = &mode1; option_1_3.parent = &mode1; option_1_4.parent = &mode1;
    option_2_1.parent = &mode2; option_2_2.parent = &mode2; option_2_3.parent = &mode2; option_2_4.parent = &mode2;
    option_3_1.parent = &mode3; option_3_2.parent = &mode3; option_3_3.parent = &mode3; option_3_4.parent = &mode3;
    option_4_1.parent = &mode4; option_4_2.parent = &mode4; option_4_3.parent = &mode4; option_4_4.parent = &mode4;
    //option_5_1.parent = &mode5; option_5_2.parent = &mode5; option_5_3.parent = &mode5; option_5_4.parent = &mode5;

    //在此进行用户所需要的初始化
    generate_cube_graph(cube_vertices, 20); 

}

//菜单切换函数，接受值为0-3的整数，表示确定，上下，返回
// 获取同级菜单的上一个（需要从parent的child链表遍历）
Menu* menu_get_prev_sibling(Menu* current) {
    if (!current || !current->parent || current->parent->child == current)
        return NULL;
    Menu* p = current->parent->child;
    while (p && p->sibling != current) {
        p = p->sibling;
    }
    return p;
}

// 主遍历函数，传入操作变量，返回菜单地址
Menu* menu_navigate(Menu* current, MenuAction action) {
    if (!current) return NULL;

    switch (action) {
        case MENU_UP: {
            Menu* prev = menu_get_prev_sibling(current);
            if (prev) return prev;
            break;
        }
        case MENU_DOWN:
            if (current->sibling) return current->sibling;
            break;
        case MENU_OK:
            if (current->child) {
                if (current->child->func) current->child->func();
                return current->child;
            }
            break;
        case MENU_BACK:
            if (current->parent) return current->parent;
            break;
    }

    //显示当前菜单项
    //OLED_ShowString(0*16,0*16,current->i, OLED_8X16);

    // 若无可跳转项，返回自身
    return current;
}

// 示例回调函数(实例)
void option_func(void) {
    OLED_ShowString(16*4, 16*1, "Hello", OLED_8X16);
    return;
    //printf("You selected an option.\n");
}





void mode_func(void) {
        OLED_ShowString(16*4, 16*1, "Hello", OLED_8X16);

    //printf("You selected a mode.\n");
}

void main_menu_func(void) {
        OLED_ShowString(16*4, 16*1, "Hello", OLED_8X16);

    //printf("This is the main menu.\n");
}

void init_menu(void) {
    // 初始化菜单结构体
    init_menu_parents();
    OLED_Clear();
    OLED_Update();
}

void OLED_DrawMenu(Menu *selected_menu) {
    // 1. 找到同级菜单链表头
    Menu *head = selected_menu;
    while (head->parent && head->parent->child && head->parent->child != head) {
        head = head->parent->child;
    }

    // 2. 遍历同级链表，统计总数和选中项序号
    int total = 0;
    int selected_index = -1;
    Menu *iter = head;
    while (iter) {
        if (iter == selected_menu) selected_index = total;
        total++;
        iter = iter->sibling;
    }

    // 3. 计算分页起点
    uint8_t page_start = 0;
    if (selected_index >= MENU_MAX_ROW)
        page_start = selected_index - (MENU_MAX_ROW - 1);

    // 4. 定位到page_start
    iter = head;
    for (int i = 0; i < page_start && iter; ++i)
        iter = iter->sibling;

    // 5. 清屏并显示4项
    OLED_Clear();
    for (int row=0; row<MENU_MAX_ROW && iter; row++) {
        int y = row * FONT_H;
        if ((page_start + row) == selected_index) {
            OLED_DrawRectangle(0, y, 64, FONT_H, OLED_UNFILLED); // 高亮选中项
        }
        OLED_ShowString(2, y+2, iter->name, OLED_8X16);
        iter = iter->sibling;
    }
}


/*********菜单回调函数区(start)***************/
void read_to_fly(void){
    //进入该选项后，持续扫描按键确定选项，一旦检测到动静，切换page标志位，到飞行菜单
    uint8_t action = get_menu_action();
    //菜单切换动作处理
    if(action == MENU_OK) {
        page = 1; // 切换到飞行菜单
    }else if (action == MENU_BACK)  //为了安全，此处要加一道安全锁，当无人机在油门大小小于10时才允许返回
    {
        //开发阶段测试功能使用
        page = 0; // 返回到主菜单
    }
    
    //功能函数执行
    if(page==1){
        //显示飞行界面
        OLED_ShowString(0, 0, "Flying Mode", OLED_8X16);
        Draw_Cube_By_Graph(-Yaw_copy, Roll_copy, -Pitch_copy,  63, 16, 1, cube_vertices);
        OLED_DrawProgressBar(1*16,1*16+2, Encoder_Value, 0, 100); // 油门进度条
        OLED_ShowChar(0, 1*16+2, 'E', OLED_8X16); // 油门字母
        OLED_ShowChar(0, 2*16+2, 'R', OLED_8X16); // Roll字母
        OLED_ShowChar(0, 3*16+2, 'P', OLED_8X16); // Pitch字母
        OLED_DrawProgressBar(1*16,2*16+2, Roll_copy, -40, 40); // 油门进度条
        OLED_DrawProgressBar(1*16,3*16+2, Pitch_copy, -40, 40); // 油门进度条
    }

}


