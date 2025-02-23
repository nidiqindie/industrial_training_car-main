#ifndef __BSP_PWMCAPTURE_FREQUENCYANDPROPORTION_H
#define __BSP_PWMCAPTURE_FREQUENCYANDPROPORTION_H


#include "stm32f10x.h"


/************高级定时器TIM参数定义，只限TIM1和TIM8************/
// 当使用不同的定时器的时候，对应的GPIO是不一样的，这点要注意
// 一个定时器智能测量一路PWM信号，虽然有4个通道，但是只有前两个通道可以用来测PWM
//且一个通道测频率，则另一个通道测占空比
/************使用TIME1来测量一路PWM信号，PWM_IN->PA8************/
#define            Pwm1Capture_TIM                   TIM1
#define            Pwm1Capture_TIM_APBxClock_FUN     RCC_APB2PeriphClockCmd
#define            Pwm1Capture_TIM_CLK               RCC_APB2Periph_TIM1

// 输入捕获能捕获到的最小的频率为 72M/{ (ARR+1)*(PSC+1) }
#define            Pwm1Capture_TIM_PERIOD            (10000-1)
#define            Pwm1Capture_TIM_PSC               (144-1)

// 中断相关宏定义
#define            Pwm1Capture_TIM_IRQ               TIM1_CC_IRQn
#define            Pwm1Capture_TIM_IRQHandler        TIM1_CC_IRQHandler

// TIM1 输入捕获通道1
#define            Pwm1Capture_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            Pwm1Capture_TIM_CH1_PORT          GPIOA
#define            Pwm1Capture_TIM_CH1_PIN           GPIO_Pin_8

#define            Pwm1Capture_TIM_IC1PWM_CHANNEL    TIM_Channel_1



/**************************函数声明********************************/
void Pwm1Capture_TIM_Init(void);


#endif	/* __BSP_Pwm1CaptureTIME_H */


