#ifndef __tran_H
#define __tran_H

#include "main.h"



typedef enum
{ 
  OPEN = 1,
  CLOSE,
  NOInput,
}TRANTYPE;

typedef enum
{ 
  MODE_NO = 1,
  MODE_ALLSOUND =2, 
  MODE_SPECITY =3,
}MODETYPE;

typedef enum
{ 
  SOUNDIN_IN = 0,
  SOUNDIN_OUT
}SOUNDIN;

typedef enum
{ 
  SOUNDIN_Auto = 0,
  SOUNDIN_Set
}SOUNDControl;

extern MODETYPE NowMode_Flag;
extern TRANTYPE NowState_Flag;
extern TRANTYPE PlayerSet_Flag;
extern TRANTYPE ALLSoundSet_Flag;
extern TRANTYPE SPECITYSet_Flag;
extern SOUNDControl BackSoundControl_Flag;
extern SOUNDIN SoundInputControl_Flag;
extern uint16_t RawSound;
extern uint16_t SetBackSound;
extern uint16_t AutoBackSound;
extern uint16_t LingMinDu;
extern uint16_t hubushuchu;
extern uint16_t freq;
extern uint16_t ThreshLevel;

void all_round(void);
void SetFreq(float freq_khz);
void PWM_Enable(uint8_t state);

#endif
