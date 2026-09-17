#include "configUI.h"

/********************************************
    ADC界面电压值转换
        ************************************/
float ADUI_VValue = 0;

uint16_t AD_Value_Get = 0;
float ADUI_VValue_Get = 0.0;

/********************************************
	ADC波形显示界面时间差值，默认值是1
        ************************************/
#define ADSHOWTIME 1


uint16_t MYNowMode_Flag = 1;
uint16_t MYBackSoundControl_Flag = 0;




//显示界面屏幕长宽定义
#define OLED_WIDTH      128
#define OLED_HEIGHT     64

//显示界面左侧留白空间定义
#define AXIS_WIDTH      30
//显示界面左侧坐标数量定义
#define AXIS_LABEL_MAX  3
//显示界面右侧总示波宽度定义
#define WAVE_WIDTH      (OLED_WIDTH-AXIS_WIDTH)

//示波界面顶部留白
#define TOP_MARGIN      5
//示波界面底部留白
#define BOTTOM_MARGIN   3

//示波界面有效总高度
#define WAVE_HEIGHT     (OLED_HEIGHT-TOP_MARGIN-BOTTOM_MARGIN)

//坐标轴坐标刻度的示数转换
#define AXIS_SHOWVALUE(VALUE) (VALUE)



#define MIN_RANGE       30
#define AXIS_MARGIN     10

/* 波形原始数据 */
uint16_t WaveBuffer[WAVE_WIDTH] = {0};

/* 平均滤波 */
static uint32_t ADSum = 0;
static uint16_t ADSumCount = 0;

/* 平滑Y轴范围 */
static uint32_t DisplayMin = 0;
static uint32_t DisplayMax = 1000;

/* 峰值检测 */
static uint16_t PeakValue = 0;
static uint8_t PeakPos = 0;





/********************************************
    二级菜单存储（置于文件最后）
********************************************/
uint8_t configchoose_state = 1;

void configUI_In(void)
{
    //进入页面时的初始化

    //设置进入页面时的按键状态

    //设置循环，完成页面内交互
    while(Key_con == 0)
    {
        Delay_ms(5);

        if (Key_state == 1)
        {
            if(configmenu[configchoose_state].prev != NONE)
            {
                configchoose_state =
                configmenu[configchoose_state].prev;
            }
        }

        if (Key_state == 3)
        {
            if(configmenu[configchoose_state].next != NONE)
            {
                configchoose_state =
                configmenu[configchoose_state].next;
            }
        }

        Key_state = 2;

        //页面内显示刷新
        configUI_Update();
    }

    /****************************************
        页面交互完毕后的命令执行
    ****************************************/

    //进入子菜单
    if (configmenu[configchoose_state].child != NONE)
    {
        configchoose_state =
        configmenu[configchoose_state].child;
    }
    //执行功能
    else if (configmenu[configchoose_state].func != NULL)
    {
        configmenu[configchoose_state].func();
    }
}


void configUI_Update(void)
{
	all_round();
	
    //父菜单
    if (configmenu[configchoose_state].parent != NONE)
        OLED_ShowString(0,0,configmenu[configmenu[configchoose_state].parent].name,OLED_8X16);

    //上一个菜单
    if (configmenu[configchoose_state].prev != NONE)
    {
		OLED_ShowString(6,17,configmenu[configmenu[configchoose_state].prev].name,OLED_6X12);
    }

    //当前菜单
    OLED_ShowString(6,33,configmenu[configchoose_state].name,OLED_6X12);

    //下一个菜单
    if (configmenu[configchoose_state].next != NONE)
    {
        OLED_ShowString(6,49,configmenu[configmenu[configchoose_state].next].name,OLED_6X12);
    }

    //当前选中框
    OLED_ReverseArea(6,33,122,12);

    OLED_Update();
    OLED_Clear();
}


void ExitconfigMenu(void)
{
    //返回父菜单
    if (configmenu[configchoose_state].parent != 0)
    {
        configchoose_state =
        configmenu[configchoose_state].parent;
    }

    //返回主界面
    else
    {
        menu_state = menuUI_UInum;
        Key_state = configUI_UInum;
        UI_now = configUI_UInum * 48;
    }
}


void MENU_ADUI(void)
{
	menu_state = ADUI_UInum;
	Key_state = 1;
}



void ADSHOWUI_Init(void)
{
    AD_Init();
    AD_Start();
}

void ADUI_In(void)
{
    //进入页面时的初始化（启动ADC）

    //设置进入页面时的按键状态
    
    //设置循环，完成页面内交互
    while(Key_con == 0)
    {
        //限制按键状态，防止溢出
        Delay_ms(20);
        if (Key_state < 1 )
            Key_state = 3;
        if (Key_state > 3 )
            Key_state = 1;
        //页面内显示刷新
        ADUI_Update();
        
    }
    //页面交互完毕后的命令执行（例如退出页面，设置参数等等
    if (Key_state == 1)
    {
		menu_state = configUI_UInum;
    }
	
	else if (Key_state == 2)
    
	{
		menu_state = ADSHOWUI_UInum;
    }
    
	else if (Key_state == 3)
    {
		AD_Value_Get = AD_Value[0];
		ADUI_VValue_Get = ADUI_VValue;
	}
}


void ADUI_Update(void)
{
    
	all_round();
    ADUI_VValue =(float)(((float)AD_Value[0] / 4096) * 3.3);
	
    OLED_ShowString(0,48,"返回",OLED_8X16);
    OLED_ShowString(48,48,"波形",OLED_8X16);
	OLED_ShowString(96,48,"捕获",OLED_8X16);
	OLED_ReverseArea((Key_state - 1) * 48,48,32,16);
	
	
    OLED_Printf(4,4,OLED_8X16,"原值:%04d",AD_Value[0]);
	
    OLED_Printf(4,26,OLED_8X16,"电压:%d",(uint16_t)ADUI_VValue);
    OLED_Printf(4+51,26,OLED_8X16,"%03d",((uint16_t)(ADUI_VValue * 1000) % 1000));
    OLED_DrawRectangle(52,40,2,2,OLED_FILLED);
	
	OLED_Printf(84,4,OLED_8X16,"%04d",AD_Value_Get);
	
    OLED_Printf(84,26,OLED_8X16,"%d",(uint16_t)ADUI_VValue_Get);
    OLED_Printf(84+10,26,OLED_8X16,"%03d",((uint16_t)(ADUI_VValue_Get * 1000) % 1000));
    OLED_DrawRectangle(92,40,2,2,OLED_FILLED);
	
	
	OLED_Update();
	OLED_Clear();
}



void ADSHOWUI_In(void)
{
    //进入页面时的初始化
    memset(WaveBuffer, 0, WAVE_WIDTH*2);
    //设置进入页面时的按键状态
    
    //设置循环，完成页面内交互
    while(Key_con == 0)
    {
        //限制按键状态，防止溢出
        Delay_ms(2);
        Key_state = 1;
        
        //页面内显示刷新
        ADSHOWUI_Update();
        
    }
    
    //页面交互完毕后的命令执行（例如退出页面，设置参数等等）
	OLED_Clear();
	OLED_Update();
	menu_state = ADUI_UInum;
	Key_state = 2;
}

void ADSHOWUI_Update(void)
{
	all_round();
    uint16_t value;
    uint16_t avg;
    uint16_t ymin = 0xFFFF;
    uint16_t ymax = 0;

    value = AD_Value[0];

    OLED_Clear();

    /* ---------- ADSHOWTIME平均 ---------- */

    ADSum += value;
    ADSumCount++;

    if(ADSumCount < ADSHOWTIME)
        return;

    avg = ADSum / ADSumCount;

    ADSum = 0;
    ADSumCount = 0;

    /* ---------- 波形缓冲 ---------- */

    for(int i=0;i<WAVE_WIDTH-1;i++)
        WaveBuffer[i] = WaveBuffer[i+1];

    WaveBuffer[WAVE_WIDTH-1] = avg;

    /* ---------- 计算当前范围 ---------- */

    for(int i=0;i<WAVE_WIDTH;i++)
    {
        if(WaveBuffer[i] > ymax)
            ymax = WaveBuffer[i];

        if(WaveBuffer[i] < ymin)
            ymin = WaveBuffer[i];
    }

    if(ymax <= ymin)
        ymax = ymin + 1;

    /* ---------- 自动量程 ---------- */

    if(ymax > DisplayMax - AXIS_MARGIN)
        DisplayMax = ymax + AXIS_MARGIN;

    if(ymin < DisplayMin + AXIS_MARGIN)
        DisplayMin = ymin - AXIS_MARGIN;

    if(ymax < DisplayMax - AXIS_MARGIN*3)
        DisplayMax -= (DisplayMax - ymax) / 20;

    if(ymin > DisplayMin + AXIS_MARGIN*3)
        DisplayMin += (ymin - DisplayMin) / 20;

    if(DisplayMax - DisplayMin < MIN_RANGE)
        DisplayMax = DisplayMin + MIN_RANGE;

    /* ---------- 坐标轴 ---------- */

    OLED_DrawLine(AXIS_WIDTH-1,0,AXIS_WIDTH-1,63);
    OLED_DrawLine(AXIS_WIDTH-1,63,127,63);

    /* ---------- 网格 ---------- */

    for(int y=0;y<64;y+=16)
    {
        for(int x=AXIS_WIDTH;x<=127;x+=4)
            OLED_DrawPoint(x,y);
    }

    for(int x=AXIS_WIDTH;x<128;x+=16)
    {
        for(int y=0;y<=63;y+=4)
            OLED_DrawPoint(x,y);
    }

    /* ---------- Y轴刻度 ---------- */

    uint16_t step;
    uint16_t valuey;
    uint8_t y;

    uint32_t range = DisplayMax - DisplayMin;

    /* 自动选择刻度间距 */

    step = range / 2;

    if(step == 0)
        step = 1;

    /* 底部刻度 */

    valuey = DisplayMin;
	
	/* 防超量程 */
	{
		if (valuey > 6000)
			valuey = -8;
	}

    y = 63 - BOTTOM_MARGIN;

    ADSHOWUI_Showx(y - 6,AXIS_SHOWVALUE(valuey + 8));

    /* 中间刻度 */

    valuey = DisplayMin + step;

    y = 63 - BOTTOM_MARGIN
        - (valuey - DisplayMin) * WAVE_HEIGHT
        / (DisplayMax - DisplayMin);

    ADSHOWUI_Showx(y - 3,AXIS_SHOWVALUE(valuey + 4));

    /* 顶部刻度 */

    valuey = DisplayMin + step*2;

    if(valuey > DisplayMax)
        valuey = DisplayMax;

    y = 63 - BOTTOM_MARGIN
        - (valuey - DisplayMin) * WAVE_HEIGHT
        / (DisplayMax - DisplayMin);

    ADSHOWUI_Showx(y,AXIS_SHOWVALUE(valuey));

    /* ---------- 绘制波形 ---------- */

    PeakValue = 0;

    for(int i=0;i<WAVE_WIDTH-1;i++)
    {
        uint8_t y0;
        uint8_t y1;

        y0 = 63 - BOTTOM_MARGIN
            - (WaveBuffer[i] - DisplayMin) * WAVE_HEIGHT
            / (DisplayMax - DisplayMin);

        y1 = 63 - BOTTOM_MARGIN
            - (WaveBuffer[i+1] - DisplayMin) * WAVE_HEIGHT
            / (DisplayMax - DisplayMin);

        OLED_DrawLine(
            AXIS_WIDTH + i,
            y0,
            AXIS_WIDTH + i + 1,
            y1
        );

        if(WaveBuffer[i] > PeakValue)
        {
            PeakValue = WaveBuffer[i];
            PeakPos = i;
        }
    }

    OLED_Update();
}


void ADSHOWUI_Showx(uint16_t y ,uint16_t num)
{
	OLED_Printf(0,y,OLED_6X12,"%04d",num);
}



Menu configmenu[] =
{
    /************************************************
     * 0 设置
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
     * 1 声音输入
     ***********************************************/
    {
        "声音输入",

        1,
        0,
        NONE,

        2,
        NONE,

        NULL,
        MENU_ADUI
    },

    /************************************************
     * 2 返回
     ***********************************************/
    {
        "返回",

        2,
        0,
        NONE,

        NONE,
        1,

        NULL,
        ExitconfigMenu
    }
};
