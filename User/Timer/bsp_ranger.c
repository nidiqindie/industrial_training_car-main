#include "bsp_ranger.h"
#include "bsp_PwmCapture_PulseWidth.h"
#include "bsp_delay.h"

#if ENABLE_RANGER1
float distance1;
#endif
#if ENABLE_RANGER2
float distance2;
#endif
#if ENABLE_RANGER3
float distance3;
#endif
#if ENABLE_RANGER4
float distance4;
#endif

void RANGER_TRIG_GPIO_Init(void)
{
    GPIO_InitTypeDef            GPIO_InitStructure;

#if ENABLE_RANGER1
    RANGER_TRIG1_RCC_Periph_COM(RANGER_TRIG1_RCC_Periph, ENABLE); //使能PPM IO口时钟 
    //GPIO结构体初始化
    GPIO_InitStructure.GPIO_Pin = RANGER_TRIG1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(RANGER_TRIG1_GPIO_PORT , &GPIO_InitStructure);
#endif

#if ENABLE_RANGER2
    RANGER_TRIG2_RCC_Periph_COM(RANGER_TRIG2_RCC_Periph, ENABLE); //使能PPM IO口时钟 
    //GPIO结构体初始化
    GPIO_InitStructure.GPIO_Pin = RANGER_TRIG2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(RANGER_TRIG2_GPIO_PORT , &GPIO_InitStructure);
#endif

#if ENABLE_RANGER3
    RANGER_TRIG3_RCC_Periph_COM(RANGER_TRIG3_RCC_Periph, ENABLE); //使能PPM IO口时钟 
    //GPIO结构体初始化
    GPIO_InitStructure.GPIO_Pin = RANGER_TRIG3_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(RANGER_TRIG3_GPIO_PORT , &GPIO_InitStructure);
#endif

#if ENABLE_RANGER4
    RANGER_TRIG4_RCC_Periph_COM(RANGER_TRIG4_RCC_Periph, ENABLE); //使能PPM IO口时钟 
    //GPIO结构体初始化
    GPIO_InitStructure.GPIO_Pin = RANGER_TRIG4_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(RANGER_TRIG4_GPIO_PORT , &GPIO_InitStructure);
#endif

}

void Distance_update(void)
{
 #if ENABLE_RANGER1 
    RANGER_TRIG1_Out_H;
    Delay_us(10);
    RANGER_TRIG1_Out_L;
    Delay_ms(60);
    distance1 = pulse_value1*170*0.0001;
#endif

 #if ENABLE_RANGER2   
    RANGER_TRIG2_Out_H;
    Delay_us(10);
    RANGER_TRIG2_Out_L;
    Delay_ms(60);
    distance2 = pulse_value2*170*0.0001;
#endif

 #if ENABLE_RANGER3   
    RANGER_TRIG3_Out_H;
    Delay_us(10);
    RANGER_TRIG3_Out_L;
    Delay_ms(60);
    distance3 = pulse_value3*170*0.0001;
#endif

 #if ENABLE_RANGER4   
    RANGER_TRIG4_Out_H;
    Delay_us(10);
    RANGER_TRIG4_Out_L;
    Delay_ms(60);
    distance4 = pulse_value4*170*0.0001;
#endif
}

