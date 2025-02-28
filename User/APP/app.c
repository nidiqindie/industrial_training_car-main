#include "app.h"
#include "stm32f10x.h"
#include "bsp_GpioRemap.h"
#include "delay.h"
#include "bsp_led.h"
#include "bsp_beep.h" 
#include "bsp_key.h"  
#include "bsp_usart.h"
#include "communication_protocol.h"
#include "bsp_motor.h"
#include "bsp_pwm.h"
#include "Emm_V5.h"
#include "board.h"
#include "servo.h"
#include "Serial_k210.h"
#include "Serial.h"
#include "jy61p.h"
#include "OLED.h"


//此函数需要两秒的时间来执行
void car_init(void)
{
//初始化一些必要的东西，张大头板子和串口并且延时2秒等待其初始化
	board_init();
	delay_ms(2000);
    Usart2Init(115200);
    USART1_Config();
	SERVO_TIM_Init();
	USART3_Config();
	UART4_init();
    UART5_init();
    // 初始化陀螺仪
    jy61p_init();
    OLED_Init();
    OLED_Clear();
    // 使能四个步进电机。
    SERVO_TIM_Init();
    LED_GPIO_Config();
    Emm_V5_En_Control(1, 1, 0);
    delay_ms(1);
    // Emm_V5_En_Control(5,1,0);
    // delay_ms(1);

    // Emm_V5_En_Control(2,1,0);
    // delay_ms(1);

    // Emm_V5_En_Control(3,1,0);
    // delay_ms(1);

    // Emm_V5_En_Control(4,1,0);
    // delay_ms(1);

}

