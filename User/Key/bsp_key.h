#ifndef __KEY_H
#define	__KEY_H

#include "stm32f10x.h"

//外部中断检测  KEY1->PA1 、 KEY2->PA4 、 KEY3->PA5
/*KEY1宏定义*/
#define    KEY1_GPIO_CLK            RCC_APB2Periph_GPIOA
#define    KEY1_GPIO_PORT           GPIOA			   
#define    KEY1_GPIO_PIN            GPIO_Pin_0
#define    KEY1_EXTI_PORTSOURCE     GPIO_PortSourceGPIOA
#define    KEY1_EXTI_PINSOURCE      GPIO_PinSource0
#define    KEY1_EXTI_LINE           EXTI_Line0

/*KEY2宏定义*/
#define    KEY2_GPIO_CLK            RCC_APB2Periph_GPIOA
#define    KEY2_GPIO_PORT           GPIOA		   
#define    KEY2_GPIO_PIN            GPIO_Pin_1
#define    KEY2_EXTI_PORTSOURCE     GPIO_PortSourceGPIOA
#define    KEY2_EXTI_PINSOURCE      GPIO_PinSource1
#define    KEY2_EXTI_LINE           EXTI_Line1

/*KEY3宏定义*/
#define    KEY3_GPIO_CLK            RCC_APB2Periph_GPIOA
#define    KEY3_GPIO_PORT           GPIOA		   
#define    KEY3_GPIO_PIN            GPIO_Pin_4
#define    KEY3_EXTI_PORTSOURCE     GPIO_PortSourceGPIOA
#define    KEY3_EXTI_PINSOURCE      GPIO_PinSource4
#define    KEY3_EXTI_LINE           EXTI_Line4

/*KEY4宏定义*/
#define    KEY4_GPIO_CLK            RCC_APB2Periph_GPIOA
#define    KEY4_GPIO_PORT           GPIOA		   
#define    KEY4_GPIO_PIN            GPIO_Pin_5
#define    KEY4_EXTI_PORTSOURCE     GPIO_PortSourceGPIOA
#define    KEY4_EXTI_PINSOURCE      GPIO_PinSource5
#define    KEY4_EXTI_LINE           EXTI_Line5

/*KEY中断源宏定义*/
#define    KEY1_EXTI_IRQ            EXTI0_IRQn
#define    KEY1_IRQHandler          EXTI0_IRQHandler

#define    KEY2_EXTI_IRQ            EXTI1_IRQn
#define    KEY2_IRQHandler          EXTI1_IRQHandler

#define    KEY3_EXTI_IRQ            EXTI4_IRQn
#define    KEY3_IRQHandler          EXTI4_IRQHandler

#define    KEY4_EXTI_IRQ            EXTI9_5_IRQn
#define    KEY4_IRQHandler          EXTI9_5_IRQHandler

extern uint8_t key1_val,key2_val,key3_val,key4_val;


void Key_GPIO_Config(void);

#endif /* __KEY_H */

