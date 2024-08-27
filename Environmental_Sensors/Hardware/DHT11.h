#ifndef __DHT11_H
#define __DHT11_H

#include <stdint.h>

extern uint8_t U8T_data_H, U8T_data_L, U8RH_data_H, U8RH_data_L, U8checkdata;

void DHT11_Init(void);
void DHT11_TRH(void);

#endif
