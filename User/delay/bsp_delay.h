#ifndef __BSP_DELAY_H
#define __BSP_DELAY_H 			   

#include "stm32f10x.h"



void Delay_init(void);
void Delay_ms(u16 nms);
void Delay_us(u32 nus);

#endif
