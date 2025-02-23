#ifndef __BSP_PWMCAPTURE_PULSEWIDTH_H
#define __BSP_PWMCAPTURE_PULSEWIDTH_H


#include "stm32f10x.h"

//使能哪些通道 ,定义1使能，定义0失能//
#define  CHANNEL_1_ENABLE     1
#define  CHANNEL_2_ENABLE     1
#define  CHANNEL_3_ENABLE     1 
#define  CHANNEL_4_ENABLE     1

/************通用定时器TIM参数定义，只限TIM2、3、4、5************/
// 当使用不同的定时器的时候，对应的GPIO是不一样的，这点要注意
// 我们这里默认使用TIM4
/*通道1、2、3、4公共宏定义*/
//时基宏定义
#define            PWMCAPTURE_PULSEWIDTH_TIM                   TIM4
#define            PWMCAPTURE_PULSEWIDTH_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            PWMCAPTURE_PULSEWIDTH_TIM_CLK               RCC_APB1Periph_TIM4
#define            PWMCAPTURE_PULSEWIDTH_TIM_PERIOD            (40000-1) //测量的最大脉冲宽度为40ms
#define            PWMCAPTURE_PULSEWIDTH_TIM_PSC               (72-1)
//中断宏定义
#define            PWMCAPTURE_PULSEWIDTH_TIM_IRQ               TIM4_IRQn
#define            PWMCAPTURE_PULSEWIDTH_TIM_INT_FUN           TIM4_IRQHandler

// 测量的起始边沿
#define            PWMCAPTURE_PULSEWIDTH_TIM_STRAT_ICPolarity  TIM_ICPolarity_Rising
// 测量的结束边沿
#define            PWMCAPTURE_PULSEWIDTH_TIM_END_ICPolarity    TIM_ICPolarity_Falling


/*通道1相关宏定义*/
#if  CHANNEL_1_ENABLE
#define            PWMCAPTURE_PULSEWIDTH1_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOB
#define            PWMCAPTURE_PULSEWIDTH1_TIM_CH1_PORT          GPIOB
#define            PWMCAPTURE_PULSEWIDTH1_TIM_CH1_PIN           GPIO_Pin_6

//PWM输入捕获通道1
#define            PWMCAPTURE_PULSEWIDTH1_TIM_CHANNEL_1         TIM_Channel_1
#define            PWMCAPTURE_PULSEWIDTH1_TIM_IT_CC1            TIM_IT_CC1

// 获取捕获寄存器值函数宏定义
#define            PWMCAPTURE_PULSEWIDTH1_TIM_GetCapture1_FUN                 TIM_GetCapture1
// 捕获信号极性函数宏定义
#define            PWMCAPTURE_PULSEWIDTH1_TIM_OC1PolarityConfig_FUN           TIM_OC1PolarityConfig
#endif


/*通道2相关宏定义*/
#if  CHANNEL_2_ENABLE
//GPIO相关宏定义PWM输入捕获通道2
#define            PWMCAPTURE_PULSEWIDTH2_TIM_CH2_GPIO_CLK      RCC_APB2Periph_GPIOB
#define            PWMCAPTURE_PULSEWIDTH2_TIM_CH2_PORT          GPIOB
#define            PWMCAPTURE_PULSEWIDTH2_TIM_CH2_PIN           GPIO_Pin_7

//PWM输入捕获通道2
#define            PWMCAPTURE_PULSEWIDTH2_TIM_CHANNEL_2         TIM_Channel_2
#define            PWMCAPTURE_PULSEWIDTH2_TIM_IT_CC2            TIM_IT_CC2

// 获取捕获寄存器值函数宏定义
#define            PWMCAPTURE_PULSEWIDTH2_TIM_GetCapture2_FUN                 TIM_GetCapture2
// 捕获信号极性函数宏定义
#define            PWMCAPTURE_PULSEWIDTH2_TIM_OC2PolarityConfig_FUN           TIM_OC2PolarityConfig
#endif

/*通道3相关宏定义*/
#if  CHANNEL_3_ENABLE
//GPIO相关宏定义PWM输入捕获通道2
#define            PWMCAPTURE_PULSEWIDTH3_TIM_CH3_GPIO_CLK      RCC_APB2Periph_GPIOB
#define            PWMCAPTURE_PULSEWIDTH3_TIM_CH3_PORT          GPIOB
#define            PWMCAPTURE_PULSEWIDTH3_TIM_CH3_PIN           GPIO_Pin_8

//PWM输入捕获通道2
#define            PWMCAPTURE_PULSEWIDTH3_TIM_CHANNEL_3         TIM_Channel_3
#define            PWMCAPTURE_PULSEWIDTH3_TIM_IT_CC3            TIM_IT_CC3

// 获取捕获寄存器值函数宏定义
#define            PWMCAPTURE_PULSEWIDTH3_TIM_GetCapture3_FUN                 TIM_GetCapture3
// 捕获信号极性函数宏定义
#define            PWMCAPTURE_PULSEWIDTH3_TIM_OC3PolarityConfig_FUN           TIM_OC3PolarityConfig
#endif

/*通道4相关宏定义*/
#if  CHANNEL_4_ENABLE
//GPIO相关宏定义PWM输入捕获通道2
#define            PWMCAPTURE_PULSEWIDTH4_TIM_CH4_GPIO_CLK      RCC_APB2Periph_GPIOB
#define            PWMCAPTURE_PULSEWIDTH4_TIM_CH4_PORT          GPIOB
#define            PWMCAPTURE_PULSEWIDTH4_TIM_CH4_PIN           GPIO_Pin_9

//PWM输入捕获通道2
#define            PWMCAPTURE_PULSEWIDTH4_TIM_CHANNEL_4         TIM_Channel_4
#define            PWMCAPTURE_PULSEWIDTH4_TIM_IT_CC4            TIM_IT_CC4

// 获取捕获寄存器值函数宏定义
#define            PWMCAPTURE_PULSEWIDTH4_TIM_GetCapture4_FUN                 TIM_GetCapture4
// 捕获信号极性函数宏定义
#define            PWMCAPTURE_PULSEWIDTH4_TIM_OC4PolarityConfig_FUN           TIM_OC4PolarityConfig
#endif

// 定时器输入捕获用户自定义变量结构体声明
typedef struct
{   
	uint8_t   Capture_FinishFlag;   // 捕获结束标志位
	uint8_t   Capture_StartFlag;    // 捕获开始标志位
	uint16_t  Capture_CcrValue;     // 捕获寄存器的值
	uint16_t  Capture_Period;       // 自动重装载寄存器更新标志 
}TIM_ICUserValueTypeDef;

#if  CHANNEL_1_ENABLE
extern uint16_t pulse_value1;
#endif

#if  CHANNEL_2_ENABLE
extern uint16_t pulse_value2;
#endif

#if  CHANNEL_3_ENABLE
extern uint16_t pulse_value3;
#endif

#if  CHANNEL_4_ENABLE
extern uint16_t pulse_value4;
#endif

/**************************函数声明********************************/
void PWMCAPTURE_PULSEWIDTH_TIM_Init(void);


#endif	/* __BSP_PWMCAPTURE_PULSEWIDTH_H */
