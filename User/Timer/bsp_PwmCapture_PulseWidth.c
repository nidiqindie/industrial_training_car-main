#include "bsp_PwmCapture_PulseWidth.h"


// 定时器输入捕获用户自定义变量结构体定义
#if  CHANNEL_1_ENABLE
uint16_t pulse_value1;
TIM_ICUserValueTypeDef TIM_ICUserValue_1_Structure = {0,0,0,0};
#endif

#if  CHANNEL_2_ENABLE
uint16_t pulse_value2;
TIM_ICUserValueTypeDef TIM_ICUserValue_2_Structure = {0,0,0,0};
#endif

#if  CHANNEL_3_ENABLE
uint16_t pulse_value3;
TIM_ICUserValueTypeDef TIM_ICUserValue_3_Structure = {0,0,0,0};
#endif

#if  CHANNEL_4_ENABLE
uint16_t pulse_value4;
TIM_ICUserValueTypeDef TIM_ICUserValue_4_Structure = {0,0,0,0};
#endif

// TIM 计数器的驱动时钟，用72中断中算出的time单位是us
uint32_t TIM_PscCLK = 72/(PWMCAPTURE_PULSEWIDTH_TIM_PSC+1);	

// 中断优先级配置
static void PWMCAPTURE_PULSEWIDTH_TIM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
		// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = PWMCAPTURE_PULSEWIDTH_TIM_IRQ;	
		// 设置主优先级为 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // 设置抢占优先级为0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void PWMCAPTURE_PULSEWIDTH_TIM_GPIO_Config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;

	#if  CHANNEL_1_ENABLE
    // 输入捕获通道1 GPIO 初始化
	RCC_APB2PeriphClockCmd(PWMCAPTURE_PULSEWIDTH1_TIM_CH1_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  PWMCAPTURE_PULSEWIDTH1_TIM_CH1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(PWMCAPTURE_PULSEWIDTH1_TIM_CH1_PORT, &GPIO_InitStructure);	
	#endif

	#if  CHANNEL_2_ENABLE
	// 输入捕获通道2 GPIO 初始化
	RCC_APB2PeriphClockCmd(PWMCAPTURE_PULSEWIDTH2_TIM_CH2_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  PWMCAPTURE_PULSEWIDTH2_TIM_CH2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(PWMCAPTURE_PULSEWIDTH2_TIM_CH2_PORT, &GPIO_InitStructure);
	#endif

	#if  CHANNEL_3_ENABLE
	// 输入捕获通道3GPIO 初始化
	RCC_APB2PeriphClockCmd(PWMCAPTURE_PULSEWIDTH3_TIM_CH3_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  PWMCAPTURE_PULSEWIDTH3_TIM_CH3_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(PWMCAPTURE_PULSEWIDTH3_TIM_CH3_PORT, &GPIO_InitStructure);
	#endif	

	#if  CHANNEL_4_ENABLE
	// 输入捕获通道4 GPIO 初始化
	RCC_APB2PeriphClockCmd(PWMCAPTURE_PULSEWIDTH4_TIM_CH4_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  PWMCAPTURE_PULSEWIDTH4_TIM_CH4_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(PWMCAPTURE_PULSEWIDTH4_TIM_CH4_PORT, &GPIO_InitStructure);
	#endif		
}

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


/* ----------------   PWM信号 周期和占空比的计算--------------- */
// ARR ：自动重装载寄存器的值
// CLK_cnt：计数器的时钟，等于 Fck_int / (psc+1) = 72M/(psc+1)
// PWM 信号的周期 T = ARR * (1/CLK_cnt) = ARR*(PSC+1) / 72M
// 占空比P=CCR/(ARR+1)

static void PWMCAPTURE_PULSEWIDTH_TIM_Mode_Config(void)
{
  // 开启定时器时钟,即内部时钟CK_INT=72M
	PWMCAPTURE_PULSEWIDTH_TIM_APBxClock_FUN(PWMCAPTURE_PULSEWIDTH_TIM_CLK,ENABLE);

/*--------------------时基结构体初始化-------------------------*/	
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
	TIM_TimeBaseStructure.TIM_Period=PWMCAPTURE_PULSEWIDTH_TIM_PERIOD;	
	// 驱动CNT计数器的时钟 = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler= PWMCAPTURE_PULSEWIDTH_TIM_PSC;	
	// 时钟分频因子 ，配置死区时间时需要用到
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// 计数器计数模式，设置为向上计数
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// 重复计数器的值，没用到不用管
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// 初始化定时器
	TIM_TimeBaseInit(PWMCAPTURE_PULSEWIDTH_TIM, &TIM_TimeBaseStructure);


	/*--------------------输入捕获结构体初始化-------------------*/	
	TIM_ICInitTypeDef TIM_ICInitStructure;

	#if  CHANNEL_1_ENABLE
	//CHANNEL__1
	// 配置输入捕获的通道，需要根据具体的GPIO来配置
	TIM_ICInitStructure.TIM_Channel = PWMCAPTURE_PULSEWIDTH1_TIM_CHANNEL_1;
	// 输入捕获信号的极性配置
	TIM_ICInitStructure.TIM_ICPolarity = PWMCAPTURE_PULSEWIDTH_TIM_STRAT_ICPolarity;
	// 输入通道和捕获通道的映射关系，有直连和非直连两种
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	// 输入的需要被捕获的信号的分频系数
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	// 输入的需要被捕获的信号的滤波系数
	TIM_ICInitStructure.TIM_ICFilter = 0;
	// 定时器输入捕获初始化
	TIM_ICInit(PWMCAPTURE_PULSEWIDTH_TIM, &TIM_ICInitStructure);
    // 清除更新和捕获中断标志位
    TIM_ClearFlag(PWMCAPTURE_PULSEWIDTH_TIM, TIM_FLAG_Update|PWMCAPTURE_PULSEWIDTH1_TIM_IT_CC1);	
    // 开启捕获中断  
	TIM_ITConfig (PWMCAPTURE_PULSEWIDTH_TIM, PWMCAPTURE_PULSEWIDTH1_TIM_IT_CC1, ENABLE );
	#endif

	#if  CHANNEL_2_ENABLE	
	//CHANNEL__2
	// 配置输入捕获的通道，需要根据具体的GPIO来配置
	TIM_ICInitStructure.TIM_Channel = PWMCAPTURE_PULSEWIDTH2_TIM_CHANNEL_2;
	// 输入捕获信号的极性配置
	TIM_ICInitStructure.TIM_ICPolarity = PWMCAPTURE_PULSEWIDTH_TIM_STRAT_ICPolarity;
	// 输入通道和捕获通道的映射关系，有直连和非直连两种
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	// 输入的需要被捕获的信号的分频系数
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	// 输入的需要被捕获的信号的滤波系数
	TIM_ICInitStructure.TIM_ICFilter = 0;
	// 定时器输入捕获初始化
	TIM_ICInit(PWMCAPTURE_PULSEWIDTH_TIM, &TIM_ICInitStructure);

	// 清除更新和捕获中断标志位
    TIM_ClearFlag(PWMCAPTURE_PULSEWIDTH_TIM, TIM_FLAG_Update|PWMCAPTURE_PULSEWIDTH2_TIM_IT_CC2);	
    // 开启捕获中断  
	TIM_ITConfig (PWMCAPTURE_PULSEWIDTH_TIM, PWMCAPTURE_PULSEWIDTH2_TIM_IT_CC2, ENABLE );
	#endif

	#if  CHANNEL_3_ENABLE	
	//CHANNEL__3
	// 配置输入捕获的通道，需要根据具体的GPIO来配置
	TIM_ICInitStructure.TIM_Channel = PWMCAPTURE_PULSEWIDTH3_TIM_CHANNEL_3;
	// 输入捕获信号的极性配置
	TIM_ICInitStructure.TIM_ICPolarity = PWMCAPTURE_PULSEWIDTH_TIM_STRAT_ICPolarity;
	// 输入通道和捕获通道的映射关系，有直连和非直连两种
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	// 输入的需要被捕获的信号的分频系数
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	// 输入的需要被捕获的信号的滤波系数
	TIM_ICInitStructure.TIM_ICFilter = 0;
	// 定时器输入捕获初始化
	TIM_ICInit(PWMCAPTURE_PULSEWIDTH_TIM, &TIM_ICInitStructure);

	// 清除更新和捕获中断标志位
    TIM_ClearFlag(PWMCAPTURE_PULSEWIDTH_TIM, TIM_FLAG_Update|PWMCAPTURE_PULSEWIDTH3_TIM_IT_CC3);	
    // 开启捕获中断  
	TIM_ITConfig (PWMCAPTURE_PULSEWIDTH_TIM, PWMCAPTURE_PULSEWIDTH3_TIM_IT_CC3, ENABLE );
	#endif

	#if  CHANNEL_4_ENABLE	
	//CHANNEL__4
	// 配置输入捕获的通道，需要根据具体的GPIO来配置
	TIM_ICInitStructure.TIM_Channel = PWMCAPTURE_PULSEWIDTH4_TIM_CHANNEL_4;
	// 输入捕获信号的极性配置
	TIM_ICInitStructure.TIM_ICPolarity = PWMCAPTURE_PULSEWIDTH_TIM_STRAT_ICPolarity;
	// 输入通道和捕获通道的映射关系，有直连和非直连两种
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	// 输入的需要被捕获的信号的分频系数
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	// 输入的需要被捕获的信号的滤波系数
	TIM_ICInitStructure.TIM_ICFilter = 0;
	// 定时器输入捕获初始化
	TIM_ICInit(PWMCAPTURE_PULSEWIDTH_TIM, &TIM_ICInitStructure);

	// 清除更新和捕获中断标志位
    TIM_ClearFlag(PWMCAPTURE_PULSEWIDTH_TIM, TIM_FLAG_Update|PWMCAPTURE_PULSEWIDTH4_TIM_IT_CC4);	
    // 开启捕获中断  
	TIM_ITConfig (PWMCAPTURE_PULSEWIDTH_TIM, PWMCAPTURE_PULSEWIDTH4_TIM_IT_CC4, ENABLE );
	#endif

	// 使能计数器
	TIM_Cmd(PWMCAPTURE_PULSEWIDTH_TIM, ENABLE);
}

void PWMCAPTURE_PULSEWIDTH_TIM_Init(void)
{
	PWMCAPTURE_PULSEWIDTH_TIM_GPIO_Config();
	PWMCAPTURE_PULSEWIDTH_TIM_NVIC_Config();
	PWMCAPTURE_PULSEWIDTH_TIM_Mode_Config();		
}


//中断函数
void PWMCAPTURE_PULSEWIDTH_TIM_INT_FUN(void)
{

#if  CHANNEL_1_ENABLE
	/*CHANNEL__1*/
	// 上升沿捕获中断
	if ( TIM_GetITStatus (PWMCAPTURE_PULSEWIDTH_TIM, PWMCAPTURE_PULSEWIDTH1_TIM_IT_CC1) != RESET)
	{
		// 第一次捕获
		if (TIM_ICUserValue_1_Structure.Capture_StartFlag == 0 )
		{
			// 计数器清0
			TIM_SetCounter ( PWMCAPTURE_PULSEWIDTH_TIM, 0 );
			// 自动重装载寄存器更新标志清0
			TIM_ICUserValue_1_Structure.Capture_Period = 0;
      		// 存捕获比较寄存器的值的变量的值清0			
			TIM_ICUserValue_1_Structure.Capture_CcrValue = 0;

			// 当第一次捕获到上升沿之后，就把捕获边沿配置为下降沿
			PWMCAPTURE_PULSEWIDTH1_TIM_OC1PolarityConfig_FUN(PWMCAPTURE_PULSEWIDTH_TIM, TIM_ICPolarity_Falling);
      		// 开始捕获标准置1			
			TIM_ICUserValue_1_Structure.Capture_StartFlag = 1;			
		}
		// 下降沿捕获中断
		else // 第二次捕获
		{
			// 获取捕获比较寄存器的值，这个值就是捕获到的高电平的时间的值
			TIM_ICUserValue_1_Structure.Capture_CcrValue = 
			PWMCAPTURE_PULSEWIDTH1_TIM_GetCapture1_FUN (PWMCAPTURE_PULSEWIDTH_TIM);

			// 当第二次捕获到下降沿之后，就把捕获边沿配置为上升沿，好开启新的一轮捕获
			PWMCAPTURE_PULSEWIDTH1_TIM_OC1PolarityConfig_FUN(PWMCAPTURE_PULSEWIDTH_TIM, TIM_ICPolarity_Rising);
      		// 开始捕获标志清0		
			TIM_ICUserValue_1_Structure.Capture_StartFlag = 0;
      		// 捕获完成标志置1			
			TIM_ICUserValue_1_Structure.Capture_FinishFlag = 1;		
		}			
		
		if(TIM_ICUserValue_1_Structure.Capture_FinishFlag == 1)
		{
			// 计算高电平时间的计数器的值
			pulse_value1 = (TIM_ICUserValue_1_Structure.Capture_Period * (PWMCAPTURE_PULSEWIDTH_TIM_PERIOD+1) + 
			       (TIM_ICUserValue_1_Structure.Capture_CcrValue+1))/TIM_PscCLK;		
			// 打印高电平脉宽时间
			//printf ( "\r\nCHANNEL__1：%d us\r\n",pulse_value1);			
			TIM_ICUserValue_1_Structure.Capture_FinishFlag = 0;			 	
	    }		
			TIM_ClearITPendingBit (PWMCAPTURE_PULSEWIDTH_TIM,PWMCAPTURE_PULSEWIDTH1_TIM_IT_CC1);	
	}		
#endif

#if  CHANNEL_2_ENABLE
	/*CHANNEL__2*/
	// 上升沿捕获中断
	if ( TIM_GetITStatus (PWMCAPTURE_PULSEWIDTH_TIM, PWMCAPTURE_PULSEWIDTH2_TIM_IT_CC2) != RESET)
	{
		// 第一次捕获
		if ( TIM_ICUserValue_2_Structure.Capture_StartFlag == 0 )   
		{
			// 计数器清0
			TIM_SetCounter ( PWMCAPTURE_PULSEWIDTH_TIM, 0 );
			// 自动重装载寄存器更新标志清0
			TIM_ICUserValue_2_Structure.Capture_Period = 0;
      		// 存捕获比较寄存器的值的变量的值清0			
			TIM_ICUserValue_2_Structure.Capture_CcrValue = 0;

			// 当第一次捕获到上升沿之后，就把捕获边沿配置为下降沿
			PWMCAPTURE_PULSEWIDTH2_TIM_OC2PolarityConfig_FUN(PWMCAPTURE_PULSEWIDTH_TIM, TIM_ICPolarity_Falling);
      		// 开始捕获标准置1			
			TIM_ICUserValue_2_Structure.Capture_StartFlag = 1;			
		}
		// 下降沿捕获中断
		else // 第二次捕获
		{
			// 获取捕获比较寄存器的值，这个值就是捕获到的高电平的时间的值
			TIM_ICUserValue_2_Structure.Capture_CcrValue = 
			PWMCAPTURE_PULSEWIDTH2_TIM_GetCapture2_FUN (PWMCAPTURE_PULSEWIDTH_TIM);

			// 当第二次捕获到下降沿之后，就把捕获边沿配置为上升沿，好开启新的一轮捕获
			PWMCAPTURE_PULSEWIDTH2_TIM_OC2PolarityConfig_FUN(PWMCAPTURE_PULSEWIDTH_TIM, TIM_ICPolarity_Rising);
      		// 开始捕获标志清0		
			TIM_ICUserValue_2_Structure.Capture_StartFlag = 0;
      		// 捕获完成标志置1			
			TIM_ICUserValue_2_Structure.Capture_FinishFlag = 1;		
		}			
			
		
		if(TIM_ICUserValue_2_Structure.Capture_FinishFlag == 1)
		{
			// 计算高电平时间的计数器的值
			pulse_value2 = (TIM_ICUserValue_2_Structure.Capture_Period * (PWMCAPTURE_PULSEWIDTH_TIM_PERIOD+1) + 
			       (TIM_ICUserValue_2_Structure.Capture_CcrValue+1))/TIM_PscCLK;		                         
			// 打印高电平脉宽时间
			// printf ( "\r\nCHANNEL_2：%d us\r\n",pulse_value2);			
			TIM_ICUserValue_2_Structure.Capture_FinishFlag = 0;		
	    }		
			TIM_ClearITPendingBit (PWMCAPTURE_PULSEWIDTH_TIM,PWMCAPTURE_PULSEWIDTH2_TIM_IT_CC2);
	}		
#endif

#if  CHANNEL_3_ENABLE
		/*CHANNEL__3*/
	// 上升沿捕获中断
	if ( TIM_GetITStatus (PWMCAPTURE_PULSEWIDTH_TIM, PWMCAPTURE_PULSEWIDTH3_TIM_IT_CC3) != RESET)
	{
		// 第一次捕获
		if ( TIM_ICUserValue_3_Structure.Capture_StartFlag == 0 )   
		{
			// 计数器清0
			TIM_SetCounter ( PWMCAPTURE_PULSEWIDTH_TIM, 0 );
			// 自动重装载寄存器更新标志清0
			TIM_ICUserValue_3_Structure.Capture_Period = 0;
      		// 存捕获比较寄存器的值的变量的值清0			
			TIM_ICUserValue_3_Structure.Capture_CcrValue = 0;

			// 当第一次捕获到上升沿之后，就把捕获边沿配置为下降沿
			PWMCAPTURE_PULSEWIDTH3_TIM_OC3PolarityConfig_FUN(PWMCAPTURE_PULSEWIDTH_TIM, TIM_ICPolarity_Falling);
      		// 开始捕获标准置1			
			TIM_ICUserValue_3_Structure.Capture_StartFlag = 1;			
		}
		// 下降沿捕获中断
		else // 第二次捕获
		{
			// 获取捕获比较寄存器的值，这个值就是捕获到的高电平的时间的值
			TIM_ICUserValue_3_Structure.Capture_CcrValue = 
			PWMCAPTURE_PULSEWIDTH3_TIM_GetCapture3_FUN (PWMCAPTURE_PULSEWIDTH_TIM);

			// 当第二次捕获到下降沿之后，就把捕获边沿配置为上升沿，好开启新的一轮捕获
			PWMCAPTURE_PULSEWIDTH3_TIM_OC3PolarityConfig_FUN(PWMCAPTURE_PULSEWIDTH_TIM, TIM_ICPolarity_Rising);
      		// 开始捕获标志清0		
			TIM_ICUserValue_3_Structure.Capture_StartFlag = 0;
      		// 捕获完成标志置1			
			TIM_ICUserValue_3_Structure.Capture_FinishFlag = 1;		
		}			
			
		
		if(TIM_ICUserValue_3_Structure.Capture_FinishFlag == 1)
		{
			// 计算高电平时间的计数器的值
			pulse_value3 = (TIM_ICUserValue_3_Structure.Capture_Period * (PWMCAPTURE_PULSEWIDTH_TIM_PERIOD+1) + 
			       (TIM_ICUserValue_3_Structure.Capture_CcrValue+1))/TIM_PscCLK;		                         
			// 打印高电平脉宽时间
			// printf ( "\r\nCHANNEL_3：%d us\r\n",pulse_value3);		
			TIM_ICUserValue_3_Structure.Capture_FinishFlag = 0;			
	    }		
			TIM_ClearITPendingBit (PWMCAPTURE_PULSEWIDTH_TIM,PWMCAPTURE_PULSEWIDTH3_TIM_IT_CC3);
	}		
#endif

#if  CHANNEL_4_ENABLE
	/*CHANNEL__4*/
	// 上升沿捕获中断
	if ( TIM_GetITStatus (PWMCAPTURE_PULSEWIDTH_TIM, PWMCAPTURE_PULSEWIDTH4_TIM_IT_CC4) != RESET)
	{
		// 第一次捕获
		if ( TIM_ICUserValue_4_Structure.Capture_StartFlag == 0 )   
		{
			// 计数器清0
			TIM_SetCounter ( PWMCAPTURE_PULSEWIDTH_TIM, 0 );
			// 自动重装载寄存器更新标志清0
			TIM_ICUserValue_4_Structure.Capture_Period = 0;
      		// 存捕获比较寄存器的值的变量的值清0			
			TIM_ICUserValue_4_Structure.Capture_CcrValue = 0;

			// 当第一次捕获到上升沿之后，就把捕获边沿配置为下降沿
			PWMCAPTURE_PULSEWIDTH4_TIM_OC4PolarityConfig_FUN(PWMCAPTURE_PULSEWIDTH_TIM, TIM_ICPolarity_Falling);
      		// 开始捕获标准置1			
			TIM_ICUserValue_4_Structure.Capture_StartFlag = 1;			
		}
		// 下降沿捕获中断
		else // 第二次捕获
		{
			// 获取捕获比较寄存器的值，这个值就是捕获到的高电平的时间的值
			TIM_ICUserValue_4_Structure.Capture_CcrValue = 
			PWMCAPTURE_PULSEWIDTH4_TIM_GetCapture4_FUN (PWMCAPTURE_PULSEWIDTH_TIM);

			// 当第二次捕获到下降沿之后，就把捕获边沿配置为上升沿，好开启新的一轮捕获
			PWMCAPTURE_PULSEWIDTH4_TIM_OC4PolarityConfig_FUN(PWMCAPTURE_PULSEWIDTH_TIM, TIM_ICPolarity_Rising);
      		// 开始捕获标志清0		
			TIM_ICUserValue_4_Structure.Capture_StartFlag = 0;
      		// 捕获完成标志置1			
			TIM_ICUserValue_4_Structure.Capture_FinishFlag = 1;		
		}			
			
		
		if(TIM_ICUserValue_4_Structure.Capture_FinishFlag == 1)
		{
			// 计算高电平时间的计数器的值
			pulse_value4 = (TIM_ICUserValue_4_Structure.Capture_Period * (PWMCAPTURE_PULSEWIDTH_TIM_PERIOD+1) + 
			       (TIM_ICUserValue_4_Structure.Capture_CcrValue+1))/TIM_PscCLK;		                         
			// 打印高电平脉宽时间
			// printf ( "\r\nCHANNEL_4：%d us\r\n",pulse_value4);			
			TIM_ICUserValue_4_Structure.Capture_FinishFlag = 0;				
	    }		
			TIM_ClearITPendingBit (PWMCAPTURE_PULSEWIDTH_TIM,PWMCAPTURE_PULSEWIDTH4_TIM_IT_CC4);	
	}	
#endif	
}

/*********************************************END OF FILE**********************/
