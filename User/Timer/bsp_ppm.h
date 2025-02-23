#ifndef _PPM_H
#define _PPM_H

#include "stm32f10x.h"
//IO口宏定义
#define            PPM_GPIO_PORT  	         GPIOB
#define            PPM_PIN                   GPIO_Pin_5
#define            PPM_RCC_Periph            RCC_APB2Periph_GPIOB
#define            PPM_RCC_Periph_COM        RCC_APB2PeriphClockCmd
#define            PPM_Out_H                 GPIO_SetBits(PPM_GPIO_PORT ,PPM_PIN)
#define            PPM_Out_L                 GPIO_ResetBits(PPM_GPIO_PORT,PPM_PIN)
//定时器2宏定义
#define            PPM_TIM                   TIM6
#define            PPM_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            PPM_TIM_CLK               RCC_APB1Periph_TIM6
#define            PPM_TIM_Period            1000
#define            PPM_TIM_Prescaler         72
//中断源宏定义
#define            PPM_TIM_IRQn              TIM6_IRQn
#define            PPM_TIME_IRQHandler       TIM6_IRQHandler

extern uint16_t Channel_x[];

void PPM_TIME_Init(void);

#endif















