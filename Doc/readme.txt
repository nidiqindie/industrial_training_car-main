功能：
3个串口     	UART1_RX->PA10   UART1_TX->PA9
                	UART2_RX->PA3   UART2_TX->PA2  
		UART3_RX/I2C2_SCL->PB11   UART2_TX/I2C2_SDA->PB10

4个PWM1(TIME3通用)     PWM1_1-> PA6  
					PWM1_2-> PA7
					PWM1_3-> PB0
					PWM1_4-> PB1

4个PWM2(TIME8高级)     PWM2_1-> PC6    (PWM2与TB6612相连) 
					PWM2_2-> PC7
					PWM2_3-> PC8
					PWM2_4-> PC9
  (TIME4通用)          encoderA_A相->B6、encoder_B相->B7

  (TIME6基本)   PID系统时基


3个LED（RGB）	R->PB14 
		G->PB13
		B->PB12

1个蜂鸣器                 BEEP->PA15

4个按键		KEY1->PWM1_Capture
		KEY2->PA1
		KEY3->PA4
		KEY4->PA5

4个直流减速电机通道 
		motor1->PB4、PB3
		motor2->PA12、PA11
		motor3->PA8、PB15
		motor4->PC14、PC15
4个超声波接口 
		Echo1->PWM1_Capture
		Echo2->PWM2_Capture
		Echo3->PWM3_Capture
		Echo4->PWM4_Capture
		Trig1->PC3
		Trig2->PC2
		Trig3->PC1
		Trig4->PC0

1个4针OLED
		SDA->PC11
		SCL->PC12

4个寻迹接口
       trace1->PC5
	   trace2->PC4
	   trace3->PC10
	   trace4->PD2

				


