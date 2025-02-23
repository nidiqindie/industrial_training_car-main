#include "bsp_pwm.h" 

/*****************************PWM1**********************/
static void PWM1_TIM_GPIO_Config(uint16_t CCR1_Val , uint16_t CCR2_Val , uint16_t CCR3_Val , uint16_t CCR4_Val) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // 输出比较通道1 GPIO 初始化
  if(CCR1_Val>0)
  {
  RCC_APB2PeriphClockCmd(PWM1_TIM_CH1_GPIO_CLK, ENABLE);	
  GPIO_InitStructure.GPIO_Pin =  PWM1_TIM_CH1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(PWM1_TIM_CH1_PORT, &GPIO_InitStructure);
  }
	
	// 输出比较通道2 GPIO 初始化
  if(CCR2_Val>0)
  {
  RCC_APB2PeriphClockCmd(PWM1_TIM_CH2_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  PWM1_TIM_CH2_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(PWM1_TIM_CH2_PORT, &GPIO_InitStructure);
  }

	// 输出比较通道3 GPIO 初始化
	if(CCR3_Val>0)
	{
  RCC_APB2PeriphClockCmd(PWM1_TIM_CH3_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  PWM1_TIM_CH3_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(PWM1_TIM_CH3_PORT, &GPIO_InitStructure);
	}

	// 输出比较通道4 GPIO 初始化
	if(CCR4_Val>0)
	{
  RCC_APB2PeriphClockCmd(PWM1_TIM_CH4_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  PWM1_TIM_CH4_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(PWM1_TIM_CH4_PORT, &GPIO_InitStructure);
	}
}


///*
// * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
// * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
// * 另外三个成员是通用定时器和高级定时器才有.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            都有
// *	TIM_CounterMode			     TIMx,x[6,7]没有，其他都有
// *  TIM_Period               都有
// *  TIM_ClockDivision        TIMx,x[6,7]没有，其他都有
// *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]才有
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */

/* ----------------   PWM信号 周期和占空比的计算--------------- */
// ARR ：自动重装载寄存器的值
// CLK_cnt：计数器的时钟，等于 Fck_int / (psc+1) = 72M/(psc+1)
// PWM 信号的周期 T = (ARR+1) * (1/CLK_cnt) = (ARR+1)*(PSC+1) / 72M
// 占空比P=CCR/(ARR+1)

static void PWM1_TIM_Mode_Config(uint16_t CCR1_Val , uint16_t CCR2_Val , uint16_t CCR3_Val , uint16_t CCR4_Val)
{
  // 开启定时器时钟,即内部时钟CK_INT=72M
	PWM1_TIM_APBxClock_FUN(PWM1_TIM_CLK,ENABLE);

/*--------------------时基结构体初始化-------------------------*/
	// 配置周期，这里配置为50Hz
	
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
	TIM_TimeBaseStructure.TIM_Period=PWM1_TIM_Period-1;	
	// 驱动CNT计数器的时钟 = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler= PWM1_TIM_Prescaler-1;	
	// 时钟分频因子 ，配置死区时间时需要用到
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// 计数器计数模式，设置为向上计数
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// 重复计数器的值，没用到不用管
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// 初始化定时器
	TIM_TimeBaseInit(PWM1_TIM, &TIM_TimeBaseStructure);

	TIM_OCInitTypeDef  TIM_OCInitStructure;
	// 配置为PWM模式1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	// 输出使能
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// 输出通道电平极性配置	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	// 输出比较通道 1
	if(CCR1_Val>0)
	{
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
	TIM_OC1Init(PWM1_TIM, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(PWM1_TIM, TIM_OCPreload_Enable);
	}

	// 输出比较通道 2
	if(CCR2_Val>0)
	{
	TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
	TIM_OC2Init(PWM1_TIM, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(PWM1_TIM, TIM_OCPreload_Enable);
	}	

	// 输出比较通道 3
	if(CCR3_Val>0)
	{
	TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
	TIM_OC3Init(PWM1_TIM, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(PWM1_TIM, TIM_OCPreload_Enable);
	}

	// 输出比较通道 4
	if(CCR4_Val>0)
	{
	TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
	TIM_OC4Init(PWM1_TIM, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(PWM1_TIM, TIM_OCPreload_Enable);
	}

	// 使能计数器
	TIM_Cmd(PWM1_TIM, ENABLE);
}


/**
  * @brief  PWM初始化，输出4个PWM，周期均为20ms.
  * @param  CCRx_Val: 第几个PWM的脉宽，周期宽度为2000，配置该参数可以调节占空比. 
  *   This parameter can be one of the following values:
  *   0：      不开启PWMx，同时不占用IO口
  *   1-2000
  * @retval None
  */
void PWM1_TIM_Init(uint16_t CCR1_Val , uint16_t CCR2_Val , uint16_t CCR3_Val , uint16_t CCR4_Val)
{
	PWM1_TIM_GPIO_Config(CCR1_Val , CCR2_Val , CCR3_Val , CCR4_Val);
	PWM1_TIM_Mode_Config(CCR1_Val , CCR2_Val , CCR3_Val , CCR4_Val);		
}
/*********************************END OF PWM1**********************/



// /*****************************PWM1**********************/
// static void PWM2_TIM_GPIO_Config(uint16_t CCR1_Val , uint16_t CCR2_Val , uint16_t CCR3_Val , uint16_t CCR4_Val) 
// {
//   GPIO_InitTypeDef GPIO_InitStructure;

//   // 输出比较通道1 GPIO 初始化
//   if(CCR1_Val>0)
//   {
//   RCC_APB2PeriphClockCmd(PWM2_TIM_CH1_GPIO_CLK, ENABLE);	
//   GPIO_InitStructure.GPIO_Pin =  PWM2_TIM_CH1_PIN;
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//   GPIO_Init(PWM2_TIM_CH1_PORT, &GPIO_InitStructure);
//   }
	
// 	// 输出比较通道2 GPIO 初始化
//   if(CCR2_Val>0)
//   {
//   RCC_APB2PeriphClockCmd(PWM2_TIM_CH2_GPIO_CLK, ENABLE);
//   GPIO_InitStructure.GPIO_Pin =  PWM2_TIM_CH2_PIN;
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//   GPIO_Init(PWM2_TIM_CH2_PORT, &GPIO_InitStructure);
//   }

// 	// 输出比较通道3 GPIO 初始化
// 	if(CCR3_Val>0)
// 	{
//   RCC_APB2PeriphClockCmd(PWM2_TIM_CH3_GPIO_CLK, ENABLE);
//   GPIO_InitStructure.GPIO_Pin =  PWM2_TIM_CH3_PIN;
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//   GPIO_Init(PWM2_TIM_CH3_PORT, &GPIO_InitStructure);
// 	}

// 	// 输出比较通道4 GPIO 初始化
// 	if(CCR4_Val>0)
// 	{
//   RCC_APB2PeriphClockCmd(PWM2_TIM_CH4_GPIO_CLK, ENABLE);
//   GPIO_InitStructure.GPIO_Pin =  PWM2_TIM_CH4_PIN;
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//   GPIO_Init(PWM2_TIM_CH4_PORT, &GPIO_InitStructure);
// 	}
// }


// ///*
// // * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
// // * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
// // * 另外三个成员是通用定时器和高级定时器才有.
// // *-----------------------------------------------------------------------------
// // *typedef struct
// // *{ TIM_Prescaler            都有
// // *	TIM_CounterMode			     TIMx,x[6,7]没有，其他都有
// // *  TIM_Period               都有
// // *  TIM_ClockDivision        TIMx,x[6,7]没有，其他都有
// // *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]才有
// // *}TIM_TimeBaseInitTypeDef; 
// // *-----------------------------------------------------------------------------
// // */

// /* ----------------   PWM信号 周期和占空比的计算--------------- */
// // ARR ：自动重装载寄存器的值
// // CLK_cnt：计数器的时钟，等于 Fck_int / (psc+1) = 72M/(psc+1)
// // PWM 信号的周期 T = (ARR+1) * (1/CLK_cnt) = (ARR+1)*(PSC+1) / 72M
// // 占空比P=CCR/(ARR+1)

// static void PWM2_TIM_Mode_Config(uint16_t CCR1_Val , uint16_t CCR2_Val , uint16_t CCR3_Val , uint16_t CCR4_Val)
// {
//   // 开启定时器时钟,即内部时钟CK_INT=72M
// 	PWM2_TIM_APBxClock_FUN(PWM2_TIM_CLK,ENABLE);

// /*--------------------时基结构体初始化-------------------------*/
// 	// 配置周期，这里配置为50Hz
	
//     TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
// 	// 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
// 	TIM_TimeBaseStructure.TIM_Period=PWM2_TIM_Period-1;	
// 	// 驱动CNT计数器的时钟 = Fck_int/(psc+1)
// 	TIM_TimeBaseStructure.TIM_Prescaler= PWM2_TIM_Prescaler-1;	
// 	// 时钟分频因子 ，配置死区时间时需要用到
// 	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
// 	// 计数器计数模式，设置为向上计数
// 	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
// 	// 重复计数器的值，没用到不用管
// 	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
// 	// 初始化定时器
// 	TIM_TimeBaseInit(PWM2_TIM, &TIM_TimeBaseStructure);

// 	TIM_OCInitTypeDef  TIM_OCInitStructure;
// 	// 配置为PWM模式1
// 	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
// 	// 输出使能
// 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;

// 	// 输出通道电平极性配置	
// 	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
// 	// 输出比较通道 1
// 	if(CCR1_Val>0)
// 	{
// 	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
// 	TIM_OC1Init(PWM2_TIM, &TIM_OCInitStructure);
// 	TIM_OC1PreloadConfig(PWM2_TIM, TIM_OCPreload_Enable);
// 	}

// 	// 输出比较通道 2
// 	if(CCR2_Val>0)
// 	{
// 	TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
// 	TIM_OC2Init(PWM2_TIM, &TIM_OCInitStructure);
// 	TIM_OC2PreloadConfig(PWM2_TIM, TIM_OCPreload_Enable);
// 	}	

// 	// 输出比较通道 3
// 	if(CCR3_Val>0)
// 	{
// 	TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
// 	TIM_OC3Init(PWM2_TIM, &TIM_OCInitStructure);
// 	TIM_OC3PreloadConfig(PWM2_TIM, TIM_OCPreload_Enable);
// 	}

// 	// 输出比较通道 4
// 	if(CCR4_Val>0)
// 	{
// 	TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
// 	TIM_OC4Init(PWM2_TIM, &TIM_OCInitStructure);
// 	TIM_OC4PreloadConfig(PWM2_TIM, TIM_OCPreload_Enable);
// 	}

// 	// 使能计数器
// 	TIM_Cmd(PWM2_TIM, ENABLE);
// 	//主输出使能，当使用通用定时器时，此句不需要
// 	TIM_CtrlPWMOutputs(PWM2_TIM,ENABLE);
// }


// /**
//   * @brief  PWM初始化，输出4个PWM，周期均为20ms.
//   * @param  CCRx_Val: 第几个PWM的脉宽，周期宽度为2000，配置该参数可以调节占空比. 
//   *   This parameter can be one of the following values:
//   *   0：      不开启PWMx，同时不占用IO口
//   *   1-2000
//   * @retval None
//   */
// void PWM2_TIM_Init(uint16_t CCR1_Val , uint16_t CCR2_Val , uint16_t CCR3_Val , uint16_t CCR4_Val)
// {
// 	PWM2_TIM_GPIO_Config(CCR1_Val , CCR2_Val , CCR3_Val , CCR4_Val);
// 	PWM2_TIM_Mode_Config(CCR1_Val , CCR2_Val , CCR3_Val , CCR4_Val);		
// }
// /*********************************END OF PWM2**********************/
/*********************************************END OF FILE**********************/
