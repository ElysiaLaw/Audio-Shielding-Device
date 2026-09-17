#include "setUI.h"

extern void Update_Sensitivity(void);

/********************************************
	二级菜单存储（置于文件最后）
        ************************************/
uint8_t setchoose_state = 1;
uint8_t set_state = 0;

void setUI_In(void)
{
    //进入页面时的初始化
	
    //设置进入页面时的按键状态
    
    //设置循环，完成页面内交互
    while(Key_con == 0)
    {
        Delay_ms(5);
        if (Key_state == 1)
		{
			if((setmenu[setchoose_state].prev != NONE) && (set_state == 0))
				setchoose_state = setmenu[setchoose_state].prev;
			else if ((setmenu[setchoose_state].value != NULL) && (set_state == 1))
				(*setmenu[setchoose_state].value) -= 1;
		}
        if (Key_state == 3)
		{
			if((setmenu[setchoose_state].next != NONE) && (set_state == 0))
				setchoose_state = setmenu[setchoose_state].next;
			else if ((setmenu[setchoose_state].value != NULL) && (set_state == 1))
				(*setmenu[setchoose_state].value) += 1;
		}
		Key_state = 2;
        //页面内显示刷新
        setUI_Update();
        

    }
    
    //页面交互完毕后的命令执行（例如退出页面，设置参数等等）

	if (setmenu[setchoose_state].child != NONE)
		setchoose_state = setmenu[setchoose_state].child;
	else if (setmenu[setchoose_state].value != NULL)
	{
		if (set_state == 1)
			set_state = 0;
		else if (set_state == 0)
			set_state = 1;
	}
	else if (setmenu[setchoose_state].func != NULL)
		setmenu[setchoose_state].func();
}

void setUI_Update(void)
{
	all_round();
	if (setmenu[setchoose_state].parent != NONE)
	{
		OLED_ShowString(0,0,setmenu[setmenu[setchoose_state].parent].name,OLED_8X16);
	}
	if (setmenu[setchoose_state].prev != NONE)
	{
		OLED_ShowString(6,17,setmenu[setmenu[setchoose_state].prev].name,OLED_6X12);
		if (setmenu[setmenu[setchoose_state].prev].value != NULL)
			OLED_Printf(64,17,OLED_6X12,"%d",*setmenu[setmenu[setchoose_state].prev].value);
	}
	OLED_ShowString(6,33,setmenu[setchoose_state].name,OLED_6X12);
	if (setmenu[setchoose_state].value != NULL)
		OLED_Printf(64,33,OLED_6X12,"%d",*setmenu[setchoose_state].value);
	if (setmenu[setchoose_state].next != NONE)
	{
		OLED_ShowString(6,49,setmenu[setmenu[setchoose_state].next].name,OLED_6X12);
		if (setmenu[setmenu[setchoose_state].next].value != NULL)
			OLED_Printf(64,49,OLED_6X12,"%d",*setmenu[setmenu[setchoose_state].next].value);
	}
	setMenucheck();
	
	if (set_state == 1)
		OLED_DrawRectangle(0,39,4,4,OLED_FILLED);
	if (set_state == 0)
		OLED_ReverseArea(6,33,122,12);
	
	
	
	OLED_Update();
	OLED_Clear();
}

void setMenucheck(void)
{
    /****************************************
     * 1 模式选择（1~2）
     ****************************************/
    if(*setmenu[1].value < 1)
    {
        *setmenu[1].value = 2;
    }

    if(*setmenu[1].value > 2)
    {
        *setmenu[1].value = 1;
    }


    /****************************************
     * 2 互补输出（0~1）
     ****************************************/
    if(*setmenu[2].value > 1)
    {
        *setmenu[2].value = 0;
    }


    /****************************************
     * 3 频率控制（360~440）
     ****************************************/
    if(*setmenu[3].value < 360)
    {
        *setmenu[3].value = 440;
    }

    if(*setmenu[3].value > 440)
    {
        *setmenu[3].value = 360;
    }


    /****************************************
     * 6 自动设置（1~2）
     ****************************************/
    if(*setmenu[6].value < 1)
    {
        *setmenu[6].value = 2;
    }

    if(*setmenu[6].value > 2)
    {
        *setmenu[6].value = 1;
    }


    /****************************************
     * 7 阈值设置（1~10）
     ****************************************/
    if(*setmenu[7].value < 1)
    {
        *setmenu[7].value = 10;
    }

    if(*setmenu[7].value > 10)
    {
        *setmenu[7].value = 1;
    }


    /****************************************
     * 8 灵敏度（1~2）
     ****************************************/
    if(*setmenu[8].value < 1)
    {
        *setmenu[8].value = 10;
    }

    if(*setmenu[8].value > 10)
    {
        *setmenu[8].value = 1;
    }
		
    NowMode_Flag       = (MODETYPE)MYNowMode_Flag;
    BackSoundControl_Flag = (SOUNDControl)MYBackSoundControl_Flag;
}

void ExitsetMenu(void)
{
	if (setmenu[setchoose_state].parent != 0)
		setchoose_state = setmenu[setchoose_state].parent;
	else 
	{
		PWM_Enable(0);
		menu_state = menuUI_UInum;
        Key_state = setUI_UInum;
        UI_now = setUI_UInum * 48;
		SetFreq((float)(freq) / 10.0);
		PWM_Enable(1);
	}
}



Menu setmenu[] = 
{
    /************************************************
     * 0 设置（主菜单）
     ***********************************************/
    {
        "设置",

        0,
        NONE,
        1,

        NONE,
        NONE,

        NULL,
        NULL
    },

    /************************************************
     * 1 模式选择
     ***********************************************/
    {
        "模式选择",

        1,
        0,
        NONE,

        2,
        NONE,

        &MYNowMode_Flag,
        NULL
    },

    /************************************************
     * 2 互补输出
     ***********************************************/
    {
        "互补输出",

        2,
        0,
        NONE,

        3,
        1,

        &hubushuchu,
        NULL
    },

    /************************************************
     * 3 频率控制
     ***********************************************/
    {
        "频率控制",

        3,
        0,
        NONE,

        4,
        2,

        &freq,
        NULL
    },

    /************************************************
     * 4 背景音
     ***********************************************/
    {
        "背景音",

        4,
        0,
        6,

        5,
        3,

        NULL,
        NULL
    },

    /************************************************
     * 5 返回（返回上一级）
     ***********************************************/
    {
        "返回",

        5,
        0,
        NONE,

        NONE,
        4,

        NULL,
        ExitsetMenu
    },

    /************************************************
     * 6 自动设置
     ***********************************************/
    {
        "自动设置",

        6,
        4,
        NONE,

        7,
        NONE,

        &MYBackSoundControl_Flag,
        NULL
    },

    /************************************************
     * 7 阈值设置
     ***********************************************/
    {
        "阈值设置",

        7,
        4,
        NONE,

        8,
        6,

//        &SetBackSound,
				&ThreshLevel,
        NULL
    },

    /************************************************
     * 8 灵敏度
     ***********************************************/
    {
        "灵敏度",

        8,
        4,
        NONE,

        9,
        7,

        &LingMinDu,
        NULL
    },

    /************************************************
     * 9 返回（背景音菜单返回）
     ***********************************************/
    {
        "返回",

        9,
        4,
        NONE,

        NONE,
        8,

        NULL,
        ExitsetMenu
    }
};
