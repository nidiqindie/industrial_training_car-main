#ifndef _BSP_RANGER_H
#define _BSP_RANGER_H

#include "stm32f10x.h"

//决定使能那个超声波测距
#define  ENABLE_RANGER1  1
#define  ENABLE_RANGER2  1
#define  ENABLE_RANGER3  1
#define  ENABLE_RANGER4  1

#if ENABLE_RANGER1
//TRIG1
#define            RANGER_TRIG1_GPIO_PORT  	          GPIOC
#define            RANGER_TRIG1_PIN                   GPIO_Pin_3
#define            RANGER_TRIG1_RCC_Periph            RCC_APB2Periph_GPIOC
#define            RANGER_TRIG1_RCC_Periph_COM        RCC_APB2PeriphClockCmd
#define            RANGER_TRIG1_Out_H                 GPIO_SetBits(RANGER_TRIG1_GPIO_PORT ,RANGER_TRIG1_PIN)
#define            RANGER_TRIG1_Out_L                 GPIO_ResetBits(RANGER_TRIG1_GPIO_PORT,RANGER_TRIG1_PIN)
extern float distance1;
#endif

#if ENABLE_RANGER2
//TRIG2
#define            RANGER_TRIG2_GPIO_PORT  	          GPIOC
#define            RANGER_TRIG2_PIN                   GPIO_Pin_2
#define            RANGER_TRIG2_RCC_Periph            RCC_APB2Periph_GPIOC
#define            RANGER_TRIG2_RCC_Periph_COM        RCC_APB2PeriphClockCmd
#define            RANGER_TRIG2_Out_H                 GPIO_SetBits(RANGER_TRIG2_GPIO_PORT ,RANGER_TRIG2_PIN)
#define            RANGER_TRIG2_Out_L                 GPIO_ResetBits(RANGER_TRIG2_GPIO_PORT,RANGER_TRIG2_PIN)
extern float distance2;
#endif

#if ENABLE_RANGER3
//TRIG3
#define            RANGER_TRIG3_GPIO_PORT  	          GPIOC
#define            RANGER_TRIG3_PIN                   GPIO_Pin_1
#define            RANGER_TRIG3_RCC_Periph            RCC_APB2Periph_GPIOC
#define            RANGER_TRIG3_RCC_Periph_COM        RCC_APB2PeriphClockCmd
#define            RANGER_TRIG3_Out_H                 GPIO_SetBits(RANGER_TRIG3_GPIO_PORT ,RANGER_TRIG3_PIN)
#define            RANGER_TRIG3_Out_L                 GPIO_ResetBits(RANGER_TRIG3_GPIO_PORT,RANGER_TRIG3_PIN)
extern float distance3;
#endif

#if ENABLE_RANGER4
//TRIG4
#define            RANGER_TRIG4_GPIO_PORT  	          GPIOC
#define            RANGER_TRIG4_PIN                   GPIO_Pin_0
#define            RANGER_TRIG4_RCC_Periph            RCC_APB2Periph_GPIOC
#define            RANGER_TRIG4_RCC_Periph_COM        RCC_APB2PeriphClockCmd
#define            RANGER_TRIG4_Out_H                 GPIO_SetBits(RANGER_TRIG4_GPIO_PORT ,RANGER_TRIG4_PIN)
#define            RANGER_TRIG4_Out_L                 GPIO_ResetBits(RANGER_TRIG4_GPIO_PORT,RANGER_TRIG4_PIN)
extern float distance4;
#endif


void RANGER_TRIG_GPIO_Init(void);
void Distance_update(void);

#endif















