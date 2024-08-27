#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "DHT11.h"
#include "SGP30.h"

int main(void)
{
	OLED_Init();
	DHT11_Init();
	SGP30_Init();
	
	uint32_t SGP_DATA;
	uint32_t CO2DATA, TVOCDATA;	
	
	OLED_ShowString(1, 1, "Loading...");
	
	do
	{
		SGP30_Write(0x20, 0x08);
		SGP_DATA = SGP30_Read();
		CO2DATA = (SGP_DATA & 0xffff0000) >> 16;
		TVOCDATA = SGP_DATA & 0x0000ffff;
		Delay_ms(500);
//	} while(CO2DATA == 400 && TVOCDATA == 0);
	} while(0);

	Delay_s(2);
	OLED_ShowString(1, 1, "          ");
	OLED_ShowString(1, 1, "TEMP:");
	OLED_ShowString(2, 1, "RH  :");
	OLED_ShowString(3, 1, "CO2 :");
	OLED_ShowString(4, 1, "TVOC:");
	
	OLED_Show16x16Char(1, 14, 0);
	OLED_Show16x16Char(2, 14, 1);
	OLED_ShowString(3, 14, "ppm");
	OLED_ShowString(4, 14, "ppb");
	
	while(1)
	{
		DHT11_TRH();
		OLED_ShowNum(1, 8, U8T_data_H, 2);
		OLED_ShowChar(1, 10, '.');
		OLED_ShowNum(1, 11, U8T_data_L, 1);
		
		OLED_ShowNum(2, 8, U8RH_data_H, 2);
		OLED_ShowChar(2, 10, '.');
		OLED_ShowNum(2, 11, U8RH_data_L, 1);
		
		
		SGP30_Write(0x20,0x08);
		SGP_DATA = SGP30_Read();
		CO2DATA = (SGP_DATA & 0xffff0000) >> 16;
		TVOCDATA = SGP_DATA & 0x0000ffff;
		OLED_ShowNum(3, 7, CO2DATA, 5);
		OLED_ShowNum(4, 7, TVOCDATA, 5);
		
		Delay_s(3);
	}
}
