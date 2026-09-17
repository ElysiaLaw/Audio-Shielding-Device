#ifndef __MYI2C_H
#define __MYI2C_H

#include "main.h"

extern float MYtemp;
extern float MYhumi;
extern uint8_t AHT20_GetmessageFlag;
extern uint8_t AHT20_GetEndFlag;

/********************************************
    全局最大超时时间定义(单位：秒)
        ************************************/
#define MYI2C_MAXDELAY 2

void MyI2C_Init(void);
void MyI2C_Start(void);
void MyI2C_Stop(void);
void MyI2C_SendByte(uint8_t Byte);
uint8_t MyI2C_WaitAck(void);
uint8_t MyI2C_ReadByte(uint8_t Ack);

void AHT20_Task(void);
uint8_t AHT20_Init(void);
uint8_t CHECK_GetTempHumiStart(void);
uint8_t CHECK_GetTempHumiAfter(void);

#endif
