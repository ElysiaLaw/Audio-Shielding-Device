#ifndef __configUI_H
#define __configUI_H

#include "main.h"



void configUI_In(void);
void configUI_Update(void);


void ADUI_In(void);
void ADUI_Update(void);
void ADSHOWUI_Init(void);
void ADSHOWUI_In(void);
void ADSHOWUI_Update(void);
void ADSHOWUI_Showx(uint16_t y ,uint16_t num);

void ExitconfigMenu(void);
void MENU_ADUI(void);

#endif
