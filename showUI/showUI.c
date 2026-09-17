#include "showUI.h"


//时间界面引入以及交互函数
void showUI_In(void)
{
    //进入页面时的初始化
    
    //设置进入页面时的按键状态
    
    //设置循环，完成页面内交互
    while(Key_con == 0)
    {
        //限制按键状态，防止溢出
        Delay_ms(5);
        if (Key_state < 1 )
            Key_state = 2;
        if (Key_state > 2 )
            Key_state = 1;
        
        //页面内显示刷新
        showUI_Update();
        
    }
    
    //页面交互完毕后的命令执行（例如退出页面，设置参数等等）
    if (Key_state == 1)
    {
        menu_state = menuUI_UInum;
        Key_state = showUI_UInum;
        UI_now = showUI_UInum * 48;
    }
    if (Key_state == 2)
    {
		if (PlayerSet_Flag == OPEN)
			PlayerSet_Flag = CLOSE;
        else
			PlayerSet_Flag = OPEN;
    }
    
}

void showUI_Update(void)
{
	AHT20_Task();
    MyRTC_ReadTime();
	all_round();
	
	char* showtemp = "未知";
	if (PlayerSet_Flag == OPEN)
		showtemp = "开启";
	else if (PlayerSet_Flag == CLOSE)
		showtemp = "关闭";
	
	OLED_Printf(2,2,OLED_6X12,"运行时间：%02d:%02d:%02d",MyRTC_Time[3]-8,MyRTC_Time[4],MyRTC_Time[5]);
	
	OLED_Printf(2,18,OLED_6X12,"当前状态：");
	OLED_ShowString(2+70,18,showtemp,OLED_6X12);
	
	OLED_ShowString(0,34,"温度",OLED_6X12);
	OLED_Printf(0+26,34,OLED_6X12,"%.1f℃",MYtemp);
	
	OLED_ShowString(66,34,"湿度",OLED_6X12);
	OLED_Printf(66+26,34,OLED_6X12,"%.1f％",MYhumi);
	
	OLED_ShowString(4,52,"菜单",OLED_6X12);
	OLED_ShowString(100,52,"控制",OLED_6X12);
    if(Key_state == 1)
        OLED_ReverseArea(4,52,24,12);
    if (Key_state == 2)
        OLED_ReverseArea(100,52,24,12);
	
    OLED_Update();
		
		// ========== 新增：显示阈值、灵敏度、背景模式 ==========
    uint16_t RealThreshold;
    if(BackSoundControl_Flag == SOUNDIN_Auto)
        RealThreshold = AutoBackSound + LingMinDu * 300;
		
    else
			
        RealThreshold = SetBackSound + LingMinDu * 300;

//    OLED_Printf(0,46,OLED_6X12,"Th:%d",RealThreshold);
//    OLED_Printf(60,46,OLED_6X12,"Sen:%d",LingMinDu);
//		OLED_ShowString(0, 50, "Adjust:Sen", OLED_6X12);

//    if(BackSoundControl_Flag == SOUNDIN_Auto)
//        OLED_ShowString(0,58,"Bg:Auto",OLED_6X12);
//    else
//        OLED_ShowString(0,58,"Bg:Manu",OLED_6X12);
}


