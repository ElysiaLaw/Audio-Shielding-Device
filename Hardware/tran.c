#include "tran.h"

MODETYPE NowMode_Flag = MODE_NO;
TRANTYPE NowState_Flag = OPEN;
TRANTYPE PlayerSet_Flag = CLOSE;
TRANTYPE ALLSoundSet_Flag = CLOSE;
TRANTYPE SPECITYSet_Flag = CLOSE;
SOUNDControl BackSoundControl_Flag = SOUNDIN_Set;
SOUNDIN SoundInputControl_Flag = SOUNDIN_IN;

uint16_t RawSound;
uint16_t SetBackSound;
uint16_t AutoBackSound;

uint16_t LingMinDu = 5;
uint16_t hubushuchu = 0;
uint16_t freq = 0;


uint16_t ThreshLevel = 5; // 阈值等级 1~10，默认中间值




// ======================== 你新方案的固定参数 ========================
#define IGNORE_MIN        1763    // 忽略下限
#define IGNORE_MAX        2400    // 忽略上限
//#define BASE_THRESH       2400    // 有效声音门槛
#define BASE_THRESH_MIN		 2200
#define BASE_THRESH_MAX		 3200
#define VALID_PERCENT     20      // 有效占比 > 20% 输出
// ====================================================================

// 采样统计
#define SAMPLE_MAX_NUM        50
static uint16_t sampleBuf[SAMPLE_MAX_NUM];
static uint16_t idx = 0;
static uint16_t validCnt = 0;   // 超过2450的次数
// ===================== 主逻辑 =====================
void all_round(void)
{
    RawSound = AD_Value[0];

    // 自适应背景（保留）
    if(BackSoundControl_Flag == SOUNDIN_Auto && NowState_Flag == CLOSE)
    {
        AutoBackSound = (AutoBackSound * 9 + RawSound) / 10;
    }

		// ===================== 【新方案：按正常说话2700 + 动态灵敏度窗口】 =====================
		// 1. 阈值：1=最灵敏(2200)，10=最难触发(3200)，正常说话2700在中间
		uint16_t final_thresh = 2000 + (ThreshLevel - 1) * 120;

		// 2. 动态采样窗口（灵敏度控制）
		// 灵敏度1  = 250点（5秒）
		// 灵敏度10 = 25点（50ms）
		uint16_t SAMPLE_NUM = 250 - (LingMinDu - 1) * 25;
		if(SAMPLE_NUM < 25) SAMPLE_NUM = 25;  // 防止越界
		if(SAMPLE_NUM > 250) SAMPLE_NUM = 250;

		// 3. 忽略区间过滤
		uint8_t is_valid = 0;
		if((RawSound < IGNORE_MIN || RawSound > IGNORE_MAX) && RawSound >= final_thresh)
		{
				is_valid = 1;
		}
    // ========== 4. 滑动统计占比 ==========
    if(sampleBuf[idx] == 1) validCnt--;
    sampleBuf[idx] = is_valid;
    if(is_valid) validCnt++;
    idx = (idx + 1) % SAMPLE_NUM;

    uint8_t percent = (validCnt * 100) / SAMPLE_NUM;
    uint8_t need_output = (percent > VALID_PERCENT) ? 1 : 0;

    // 5.总开关+模式
    if(PlayerSet_Flag == CLOSE)
        NowState_Flag = CLOSE;
    else
    {
        switch(NowMode_Flag)
        {
            case MODE_NO: NowState_Flag=OPEN; break;
            case MODE_ALLSOUND: NowState_Flag=need_output?OPEN:CLOSE; break;
            default: NowState_Flag=CLOSE;
        }
    }

    //// 6、最终控制超声波PWM输出
    if(NowState_Flag == OPEN)
    {
        PWM_Enable(1);
    }
    else
    {
        PWM_Enable(0);
    }
}

void SetFreq(float freq_khz)
{
	uint32_t arr;
	
	if(freq_khz < 1.0f)
		freq_khz = 1.0f;
	
	if(freq_khz > 100.0f)
		freq_khz = 100.0f;
	
	arr = (72000000 / (freq_khz * 1000)) - 1;
	
	TIM_SetAutoreload(TIM2,arr);
	
	/*固定50%占空比*/
	TIM_SetCompare3(TIM2,arr / 2);
	
	if(hubushuchu == 1)
		TIM_SetCompare4(TIM2,arr / 2);
	else
		TIM_SetCompare4(TIM2,0);
}


void PWM_Enable(uint8_t state)
{
	if(state)
	{
		/*A2输出PWM*/
		TIM_CCxCmd(TIM2,TIM_Channel_3,TIM_CCx_Enable);
		
		/*根据模式决定A3是否输出*/
		if(hubushuchu == 1)
			TIM_CCxCmd(TIM2,TIM_Channel_4,TIM_CCx_Enable);
		else
			TIM_CCxCmd(TIM2,TIM_Channel_4,TIM_CCx_Disable);
	}
	else
	{
		TIM_CCxCmd(TIM2,TIM_Channel_3,TIM_CCx_Disable);
		TIM_CCxCmd(TIM2,TIM_Channel_4,TIM_CCx_Disable);
	}
}
