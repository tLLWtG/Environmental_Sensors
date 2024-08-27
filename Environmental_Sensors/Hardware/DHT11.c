#include "stm32f10x.h"                  // Device header
#include "Delay.h"

uint32_t U32FLAG;
uint8_t U8count, U8temp;
uint8_t U8T_data_H, U8T_data_L, U8RH_data_H, U8RH_data_L, U8checkdata;
uint8_t U8T_data_H_temp, U8T_data_L_temp, U8RH_data_H_temp, U8RH_data_L_temp, U8checkdata_temp;
uint8_t U8comdata;

#define DataPin GPIO_Pin_10

// about 200 ms
#define LIMIT_NUM 1000000

void DHT11_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = DataPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA, DataPin);
}

void DHT11_Read_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = DataPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void DHT11_COM(void)
{
	uint8_t i;        
	for (i = 0; i < 8; ++i)
	{
		U32FLAG = 2;
		while ((GPIO_ReadInputDataBit(GPIOA, DataPin) == RESET) && U32FLAG++ < LIMIT_NUM)
			;
//		U32FLAG = 2;
//		while ((GPIO_ReadInputDataBit(GPIOA, DataPin) == RESET))
//			;
		
		Delay_us(35);
		U8temp=0;
		if (GPIO_ReadInputDataBit(GPIOA, DataPin) == SET)
			U8temp = 1;
		
		U32FLAG=2;
		while ((GPIO_ReadInputDataBit(GPIOA, DataPin) == SET) && U32FLAG++ < LIMIT_NUM)
			;
//		U32FLAG=2;
//		while ((GPIO_ReadInputDataBit(GPIOA, DataPin) == SET))
//			;
		if (U32FLAG >= LIMIT_NUM)
			break; 
		U8comdata <<= 1;
		U8comdata |= U8temp;
	 }

}
 
 
void DHT11_TRH(void)
{
	GPIO_ResetBits(GPIOA, DataPin);
	Delay_ms(20);
	GPIO_SetBits(GPIOA, DataPin);
	Delay_us(30);
	DHT11_Read_Init();
	if (GPIO_ReadInputDataBit(GPIOA, DataPin) == RESET)
	{
		U32FLAG = 2;
		while ((GPIO_ReadInputDataBit(GPIOA, DataPin) == RESET) && U32FLAG++ < LIMIT_NUM)
			;
		U32FLAG = 2;
		while ((GPIO_ReadInputDataBit(GPIOA, DataPin) == SET) && U32FLAG++ < LIMIT_NUM)
			;
//		U32FLAG = 2;
//		while ((GPIO_ReadInputDataBit(GPIOA, DataPin) == RESET))
//			;
//		U32FLAG = 2;
//		while ((GPIO_ReadInputDataBit(GPIOA, DataPin) == SET))
//			;
		
		DHT11_COM();
		U8RH_data_H_temp = U8comdata;
		DHT11_COM();
		U8RH_data_L_temp = U8comdata;
		DHT11_COM();
		U8T_data_H_temp = U8comdata;
		DHT11_COM();
		U8T_data_L_temp = U8comdata;
		DHT11_COM();
		U8checkdata_temp = U8comdata;
 
		DHT11_Init();
 
		U8temp = (U8T_data_H_temp + U8T_data_L_temp + U8RH_data_H_temp + U8RH_data_L_temp);
		if (U8temp==U8checkdata_temp)
		{
			U8RH_data_H = U8RH_data_H_temp;
			U8RH_data_L = U8RH_data_L_temp;
			U8T_data_H = U8T_data_H_temp;
			U8T_data_L = U8T_data_L_temp;
			U8checkdata = U8checkdata_temp;
		}
	}
}