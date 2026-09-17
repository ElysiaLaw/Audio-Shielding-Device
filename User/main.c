#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "MyRTC.h"
#include "main.h"

//void Update_Sensitivity(void);5.28  9:55注释

uint8_t UI_inFlag = 1;

//全局菜单变量
uint8_t menu_state = 1;

int main(void)
{
//	SysTick_Init(); 
	UI_Init();
	MyRTC_Init();
	freq = 400;
	
	

	SetFreq((float)(freq) / 10.0);
	PWM_Enable(1);
	
	while (1)
	{
		
				all_round(); 
        OLED_Clear();
        Key_con = 0;
        switch (menu_state)
        {
            case showUI_UInum:
                showUI_In();
                break;
            case menuUI_UInum:
                menuUI_In();
                break;
            case configUI_UInum:
                configUI_In();
                break;
            case setUI_UInum:
				PWM_Enable(0);
                setUI_In();
                break;

			case ADSHOWUI_UInum:
                ADSHOWUI_In();
				break;
			
			case ADUI_UInum:
                ADUI_In();
				break;
			
			
            default:
                showUI_In();
                break;
        }


	}
}
