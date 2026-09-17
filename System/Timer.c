#include "Timer.h"

uint8_t Timer1_CheckFlag = 0;
/**
  * 函    数：定时中断初始化
  * 参    数：无
  * 返 回 值：无
  */
void Timer_Init(void)
{
	/*TIM2 PWM初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	/*GPIO配置*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	/*TIM2时基配置*/
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
	TIM_TimeBaseInitStructure.TIM_Period = 1799;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	
	/*PWM配置*/
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	/*CH3 正向PWM*/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_Pulse = 900;
	TIM_OC3Init(TIM2,&TIM_OCInitStructure);
	
	/*CH4 反向PWM*/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_Pulse = 900;
	TIM_OC4Init(TIM2,&TIM_OCInitStructure);
	
	/*默认关闭输出*/
	TIM_CCxCmd(TIM2,TIM_Channel_3,TIM_CCx_Disable);
	TIM_CCxCmd(TIM2,TIM_Channel_4,TIM_CCx_Disable);
	
	/*启动TIM2*/
	TIM_Cmd(TIM2,ENABLE);
	
	
	/*TIM3保持原配置*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	TIM_InternalClockConfig(TIM3);
	
	TIM_TimeBaseInitStructure.TIM_Period = 6000 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 960 - 1;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	/* ===================== 【新增：TIM1 灵敏度定时器】 ===================== */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    TIM_InternalClockConfig(TIM1);

    // 时基：72MHz / 7200 = 10kHz → 1 count = 0.1ms
    TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;
    TIM_TimeBaseInitStructure.TIM_Period = 500 - 1; // 默认50ms
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM1, ENABLE);
    /* ====================================================================== */

	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	  // ===================== 【新增：TIM1 中断优先级】 =====================
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}
/* ===================== 【新增：设置TIM1中断时间】 ===================== */
void TIM1_SetInterval(uint16_t ms)
{
    uint16_t arr = ms * 10 - 1;  // 0.1ms 基础
    TIM_SetAutoreload(TIM1, arr);
    TIM_SetCounter(TIM1, 0);
}

void TIM2_IRQHandler(void)
{

}
/* ===================== 【新增：TIM1 中断服务函数】 ===================== */
void TIM1_UP_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
    {
        Timer1_CheckFlag = 1;  // 标志：时间到，去判断
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		if (AHT20_GetmessageFlag == 0)
			AHT20_GetmessageFlag = 1;
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

void TIM_Start(TIM_TypeDef* TIMx)
{
	TIM_Cmd(TIMx, ENABLE);
}
void TIM_End(TIM_TypeDef* TIMx)
{
	TIM_Cmd(TIMx, DISABLE);
}
