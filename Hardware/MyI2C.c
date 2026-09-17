#include "MyI2C.h"                  // Device header

/*引脚配置层*/
float MYtemp;
float MYhumi;
uint8_t AHT20_GetmessageFlag = 0;
uint8_t AHT20_GetEndFlag = 1;

/**
  * 函    数：I2C写SCL引脚电平
  * 参    数：BitValue 协议层传入的当前需要写入SCL的电平，范围0~1
  * 返 回 值：无
  * 注意事项：此函数需要用户实现内容，当BitValue为0时，需要置SCL为低电平，当BitValue为1时，需要置SCL为高电平
  */
void MyI2C_W_SCL(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_12, (BitAction)BitValue);		//根据BitValue，设置SCL引脚的电平
	Delay_us(10);												//延时10us，防止时序频率超过要求
}

/**
  * 函    数：I2C写SDA引脚电平
  * 参    数：BitValue 协议层传入的当前需要写入SDA的电平，范围0~1
  * 返 回 值：无
  * 注意事项：此函数需要用户实现内容，当BitValue为0时，需要置SDA为低电平，当BitValue为1时，需要置SDA为高电平
  */
void MyI2C_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_11, (BitAction)BitValue);		//根据BitValue，设置SDA引脚的电平，BitValue要实现非0即1的特性
	Delay_us(10);												//延时10us，防止时序频率超过要求
}

/**
  * 函    数：I2C读SDA引脚电平
  * 参    数：无
  * 返 回 值：协议层需要得到的当前SDA的电平，范围0~1
  * 注意事项：此函数需要用户实现内容，当前SDA为低电平时，返回0，当前SDA为高电平时，返回1
  */
uint8_t MyI2C_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11);		//读取SDA电平
	Delay_us(10);												//延时10us，防止时序频率超过要求
	return BitValue;											//返回SDA电平
}

/**
  * 函    数：I2C初始化
  * 参    数：无
  * 返 回 值：无
  * 注意事项：此函数需要用户实现内容，实现SCL和SDA引脚的初始化
  */
void MyI2C_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOB的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将PB10和PB11引脚初始化为开漏输出
	
	/*设置默认电平*/
	GPIO_SetBits(GPIOA, GPIO_Pin_12 | GPIO_Pin_11);			//设置PB10和PB11引脚初始化后默认为高电平（释放总线状态）
}

/*协议层*/

/**
  * 函    数：I2C起始
  * 参    数：无
  * 返 回 值：无
  */
void MyI2C_Start(void)
{
	MyI2C_W_SDA(1);							//释放SDA，确保SDA为高电平
	MyI2C_W_SCL(1);							//释放SCL，确保SCL为高电平
	MyI2C_W_SDA(0);							//在SCL高电平期间，拉低SDA，产生起始信号
	MyI2C_W_SCL(0);							//起始后把SCL也拉低，即为了占用总线，也为了方便总线时序的拼接
}

/**
  * 函    数：I2C终止
  * 参    数：无
  * 返 回 值：无
  */
void MyI2C_Stop(void)
{
	MyI2C_W_SDA(0);							//拉低SDA，确保SDA为低电平
	MyI2C_W_SCL(1);							//释放SCL，使SCL呈现高电平
	MyI2C_W_SDA(1);							//在SCL高电平期间，释放SDA，产生终止信号
}

/**
  * 函    数：I2C发送一个字节
  * 参    数：Byte 要发送的一个字节数据，范围：0x00~0xFF
  * 返 回 值：无
  */
void MyI2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i ++)				//循环8次，主机依次发送数据的每一位
	{
		/*两个!可以对数据进行两次逻辑取反，作用是把非0值统一转换为1，即：!!(0) = 0，!!(非0) = 1*/
		MyI2C_W_SDA(!!(Byte & (0x80 >> i)));//使用掩码的方式取出Byte的指定一位数据并写入到SDA线
		MyI2C_W_SCL(1);						//释放SCL，从机在SCL高电平期间读取SDA
		MyI2C_W_SCL(0);						//拉低SCL，主机开始发送下一位数据
	}
}

uint8_t MyI2C_ReadByte(uint8_t Ack)
{
    uint8_t i, Byte = 0;

    MyI2C_W_SDA(1);

    for(i = 0; i < 8; i++)
    {
        Byte <<= 1;
        MyI2C_W_SCL(1);

        if(MyI2C_R_SDA())
            Byte |= 0x01;

        MyI2C_W_SCL(0);
    }

    MyI2C_W_SDA(!Ack);
    MyI2C_W_SCL(1);
    MyI2C_W_SCL(0);
    MyI2C_W_SDA(1);

    return Byte;
}

uint8_t MyI2C_WaitAck(void)
{
    uint32_t start_time;

    MyI2C_W_SDA(1);      // 释放SDA
    MyI2C_W_SCL(1);

    start_time = RTC_GetCounter();

    while(MyI2C_R_SDA())
    {
        if((RTC_GetCounter() - start_time) > MYI2C_MAXDELAY)
        {
            MyI2C_Stop();
            return 0;
        }
    }

    MyI2C_W_SCL(0);
    return 1;
}


uint8_t CHECK_GetTempHumiStart(void)
{
	if (AHT20_GetEndFlag == 1)
	{
		AHT20_GetEndFlag = 0;
		/* 触发测量 */
		MyI2C_Start();

		MyI2C_SendByte(0x70);//发送初始命令
		
		if(!MyI2C_WaitAck())//等待应答
		{
			MyI2C_Stop();
			return 0;
		}

		MyI2C_SendByte(0xAC);//发送测量命令
		
		if(!MyI2C_WaitAck())//等待应答
		{
			MyI2C_Stop();
			return 0;
		}

		MyI2C_SendByte(0x33);//发送测量命令的两个参数
		if(!MyI2C_WaitAck())
		{
			MyI2C_Stop();
			return 0;
		}

		MyI2C_SendByte(0x00);
		if(!MyI2C_WaitAck())
		{
			MyI2C_Stop();
			return 0;
		}
		
		MyI2C_Stop();
		TIM_Start(TIM3);
		return 1;
	}
	return 2;
}

uint8_t CHECK_GetTempHumiAfter(void)
{
	if (AHT20_GetmessageFlag == 1)
	{
		uint8_t buf[6];
		uint32_t humi_raw;
		uint32_t temp_raw;

		/* 读取6字节数据 */
		TIM_End(TIM3);
		AHT20_GetmessageFlag = 0;
		MyI2C_Start();

		MyI2C_SendByte(0x71);
		if(!MyI2C_WaitAck())
		{
			MyI2C_Stop();
			return 0;
		}

		buf[0] = MyI2C_ReadByte(1);
		buf[1] = MyI2C_ReadByte(1);
		buf[2] = MyI2C_ReadByte(1);
		buf[3] = MyI2C_ReadByte(1);
		buf[4] = MyI2C_ReadByte(1);
		buf[5] = MyI2C_ReadByte(0);

		MyI2C_Stop();

		/* 检查Busy位 */
		if(buf[0] & 0x80)
			return 0;

		/* 数据转换 */
		humi_raw = ((uint32_t)buf[1] << 12) |
				   ((uint32_t)buf[2] << 4) |
				   (buf[3] >> 4);

		temp_raw = ((uint32_t)(buf[3] & 0x0F) << 16) |
				   ((uint32_t)buf[4] << 8) |
				   buf[5];

		MYhumi = (float)humi_raw * 100 / 1048576;
		MYtemp = (float)temp_raw * 200 / 1048576 - 50;
		AHT20_GetEndFlag = 1;
		return 1;
	}
	return 2;
}







uint8_t AHT20_Init(void)
{
	
	MyI2C_Init();
	Timer_Init();
	
    MyI2C_Start();

    MyI2C_SendByte(0x70);
    if(!MyI2C_WaitAck())
    {
        MyI2C_Stop();
        return 0;
    }

    MyI2C_SendByte(0xBE);
    if(!MyI2C_WaitAck())
    {
        MyI2C_Stop();
        return 0;
    }

    MyI2C_SendByte(0x08);
    if(!MyI2C_WaitAck())
    {
        MyI2C_Stop();
        return 0;
    }

    MyI2C_SendByte(0x00);
    if(!MyI2C_WaitAck())
    {
        MyI2C_Stop();
        return 0;
    }

    MyI2C_Stop();

    Delay_ms(10);

    return 1;
}

void AHT20_Task(void)
{
	uint8_t i = 3;
	
	if (AHT20_GetEndFlag == 1)
		i = CHECK_GetTempHumiStart();
	else if (AHT20_GetmessageFlag == 1)
		i = CHECK_GetTempHumiAfter();
	
	if (i == 0 || i == 2)
	{
		AHT20_GetEndFlag = 1;
		TIM_End(TIM3);
	}
}

