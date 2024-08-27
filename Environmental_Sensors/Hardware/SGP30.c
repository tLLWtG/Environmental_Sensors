#include "stm32f10x.h"                  // Device header
#include "SGP30.h"
#include "Delay.h"

#define  SGP30_SCL_GPIO_CLK        RCC_APB2Periph_GPIOB
#define  SGP30_SCL_GPIO_PORT       GPIOB
#define  SGP30_SCL_GPIO_PIN        GPIO_Pin_10

#define  SGP30_SDA_GPIO_CLK        RCC_APB2Periph_GPIOB
#define  SGP30_SDA_GPIO_PORT       GPIOB
#define  SGP30_SDA_GPIO_PIN        GPIO_Pin_11

#define  SGP30_SDA_READ()           GPIO_ReadInputDataBit(SGP30_SDA_GPIO_PORT, SGP30_SDA_GPIO_PIN)

#define SGP30_read  0xb1
#define SGP30_write 0xb0

void SCL_H(void)
{
	GPIO_SetBits(SGP30_SCL_GPIO_PORT, SGP30_SCL_GPIO_PIN);
}

void SCL_L(void)
{
	GPIO_ResetBits(SGP30_SCL_GPIO_PORT, SGP30_SCL_GPIO_PIN);
}

void SDA_H(void)
{
	GPIO_SetBits(SGP30_SDA_GPIO_PORT, SGP30_SDA_GPIO_PIN);
}

void SDA_L(void)
{
	GPIO_ResetBits(SGP30_SDA_GPIO_PORT, SGP30_SDA_GPIO_PIN);
}

void SGP30_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(SGP30_SCL_GPIO_CLK | SGP30_SDA_GPIO_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = SGP30_SCL_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SGP30_SCL_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SGP30_SDA_GPIO_PIN;
	GPIO_Init(SGP30_SDA_GPIO_PORT, &GPIO_InitStructure);
}


void SDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = SGP30_SDA_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SGP30_SDA_GPIO_PORT, &GPIO_InitStructure);
}

void SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = SGP30_SDA_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SGP30_SDA_GPIO_PORT, &GPIO_InitStructure);
}

// Generate I2C start signal
// Start condition: when CLK is high, DATA changes from high to low
void SGP30_IIC_Start(void)
{
	SDA_OUT();
	SDA_H();
	SCL_H();
	Delay_us(20);

	SDA_L();
	Delay_us(20);
	
// Pull I2C bus low to prepare for data transmission
	SCL_L();
}

// Generate I2C stop signal
// Stop condition: when CLK is high, DATA changes from low to high
void SGP30_IIC_Stop(void)
{
	SDA_OUT();
	SCL_L();
	SDA_L();
	Delay_us(20);

	SCL_H();
	SDA_H();
	Delay_us(20);
}

// Wait for an acknowledgment (ACK) signal
// Returns: 1 if no ACK received (failure)
//          0 if ACK received (success)
uint8_t SGP30_IIC_Wait_Ack(void)
{
	uint8_t ucErrTime = 0;
	SDA_IN();
	SDA_H();
	Delay_us(10);
	SCL_H();
	Delay_us(10);
	while (SGP30_SDA_READ())
	{
		ucErrTime++;
		if (ucErrTime > 250)
		{
			SGP30_IIC_Stop();
			return 1;
		}
	}
	SCL_L();
	return 0;
}

void SGP30_IIC_Ack(void)
{
	SCL_L();
	SDA_OUT();
	SDA_L();
	Delay_us(20);
	SCL_H();
	Delay_us(20);
	SCL_L();
}

void SGP30_IIC_NAck(void)
{
	SCL_L();
	SDA_OUT();
	SDA_H();
	Delay_us(20);
	SCL_H();
	Delay_us(20);
	SCL_L();
}

// Send a byte of data via I2C
// Returns: 1 if ACK received, 0 if no ACK received
void SGP30_IIC_Send_Byte(uint8_t txd)
{
	uint8_t t;
	SDA_OUT();
// Pull clock low to start transmission
	SCL_L();
	for( t = 0; t < 8; t++)
	{
		if ((txd & 0x80) >> 7)
			SDA_H();
		else
			SDA_L();
		txd <<= 1;
		Delay_us(20);
		SCL_H();
		Delay_us(20);
		SCL_L();
		Delay_us(20);
	}
	Delay_us(20);
}

// Read a byte of data via I2C
// ack = 1 to send ACK after receiving, 0 to send NACK
uint16_t SGP30_IIC_Read_Byte(uint8_t ack)
{
	uint8_t i;
	uint16_t receive = 0;
	SDA_IN();
	for(i = 0; i < 8; i++)
	{
		SCL_L();
		Delay_us(20);
		SCL_H();
		receive <<= 1;
		if(SGP30_SDA_READ())
			receive++;
		Delay_us(20);
	}
	if (!ack)
		SGP30_IIC_NAck();
	else
		SGP30_IIC_Ack();
	return receive;
}

void SGP30_Init(void)
{
	SGP30_GPIO_Init();
// Send initialization command
	SGP30_Write(0x20, 0x03);
}

void SGP30_Write(uint8_t a, uint8_t b)
{
	SGP30_IIC_Start();
// Send device address + write instruction
	SGP30_IIC_Send_Byte(SGP30_write);
	SGP30_IIC_Wait_Ack();
// Send first data byte
	SGP30_IIC_Send_Byte(a);
	SGP30_IIC_Wait_Ack();
// Send second data byte
	SGP30_IIC_Send_Byte(b);
	SGP30_IIC_Wait_Ack();
	SGP30_IIC_Stop();
	Delay_ms(100);
}

uint32_t SGP30_Read(void)
{
	uint32_t dat;
	uint8_t crc;
	SGP30_IIC_Start();
	SGP30_IIC_Send_Byte(SGP30_read);
	SGP30_IIC_Wait_Ack();
	dat = SGP30_IIC_Read_Byte(1);
	dat <<= 8;
	dat += SGP30_IIC_Read_Byte(1);
// Read CRC byte (ignored)
	crc = SGP30_IIC_Read_Byte(1);
	crc = crc;
	dat <<= 8;
	dat += SGP30_IIC_Read_Byte(1);
	dat <<= 8;
	dat += SGP30_IIC_Read_Byte(0);
	SGP30_IIC_Stop();
	return(dat);
}
