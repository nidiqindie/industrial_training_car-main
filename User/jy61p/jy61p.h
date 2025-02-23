#ifndef __JY61P_H
#define __JY61P_H

#include "stm32f10x.h"
#include "REG.h"
#include "wit_c_sdk.h"
#include "bsp_usart.h"
#include "delay.h"
void jy61p_init(void);
void Usart2Init(unsigned int uiBaud);

#endif
