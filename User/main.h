#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f10x.h"                  // Device header

#include "key.h"
#include "OLED.h"
#include "WIFI.h"
#include "AD.h"
#include "MYI2C.h"
#include "MYRTC.h"
#include "Serial.h"
#include "OLED_Data.h"
#include "menuUI.h"
#include "Delay.h"
#include "configUI.h"
#include "showUI.h"
#include "setUI.h"
#include "tran.h"
#include "Timer.h"


extern uint8_t UI_inFlag;
/********************************************
    全局菜单变量,从1开始计数，
            如需修改，直接改数就行了，然后把menu改一下就行
                注意：为防止出现bug，请将数值按照菜单顺序进行修改
                未出现在菜单上的页面可以随便填
        1、默认界面
        2、设置界面
        3、调试界面
        4、
        ************************************/
extern uint8_t menu_state;//main.c

#define showUI_UInum 1              //默认界面
#define menuUI_UInum 255            //菜单界面
#define setUI_UInum 2               //设置界面
#define configUI_UInum 3            //调试界面
#define ADSHOWUI_UInum 254			//ADC波形显示
#define ADUI_UInum 253				//ADC波形显示


#endif
