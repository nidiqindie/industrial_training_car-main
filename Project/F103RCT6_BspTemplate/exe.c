#include "bsp_delay.h"
#include "bsp_led.h"
#include "bsp_beep.h" 
#include "bsp_key.h"  
#include "bsp_usart.h"
#include "communication_protocol.h"
#include "bsp_motor.h"
#include "bsp_pwm.h"
void go_init(void)
{
		Motor_gpio_init();
	Encoder1_init();
	Encoder2_init();
	Encoder3_init();
	Encoder4_init();

	PID_timebase_init();

}
