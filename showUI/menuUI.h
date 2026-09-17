#ifndef __MENUUI_H
#define __MENUUI_H

#include "main.h"

/********************************************
    菜单界面位置全局存储
        ************************************/
extern uint16_t UI_now;
extern uint16_t UI_last;

typedef struct
{
    char *name;          // 显示名字
	uint8_t self;			//自身ID
    uint8_t parent;      // 父菜单ID
    uint8_t child;       // 第一个子菜单ID
    uint8_t next;        // 同级下一个菜单ID
    uint8_t prev;        // 同级上一个菜单ID

	uint16_t* value;		//绑定的变量
    void (*func)(void);          // 绑定的功能（设置项）
}Menu;

#define NONE 255
extern Menu configmenu[];
extern Menu setmenu[];


void UI_Init(void);
void menuUI_In(void);
void menuUI_Update(void);
void menuUI_inUp(void);


uint16_t reduct(uint16_t a,uint16_t b);//求二者的差值

#endif
