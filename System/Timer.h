#ifndef __TIMER_H
#define __TIMER_H

#include "main.h"
/********************************************
    注意：
		TIM1空闲
		TIM2已调用：PWM噪声波
		TIM3被AHT20占用，在命令启动时做中断延迟，间隔80毫秒
        ************************************/

extern uint8_t Timer1_CheckFlag;

void Timer_Init(void);
void TIM_Start(TIM_TypeDef* TIMx);
void TIM_End(TIM_TypeDef* TIMx);

void TIM1_SetInterval(uint16_t ms);


#endif
