#ifndef __SGP30_H
#define __SGP30_H

#include <stdint.h>

void SGP30_Init(void);				  
void SGP30_Write(uint8_t a, uint8_t b);
uint32_t SGP30_Read(void);

#endif

