#ifndef __HWT101_H
#define __HWT101_H

#include "stm32f10x.h"
#include "REG.h"
#include "wit_c_sdk.h"
#include "bsp_usart.h"
#include "delay.h"
extern float yaw;
void jy61p_init(void);
void Usart2Init(unsigned int uiBaud);
void Yaw_setzero(void);
#endif
