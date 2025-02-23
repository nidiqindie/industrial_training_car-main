#include "bsp_ppm.h"

//开启8个PWM通道
uint16_t Channel_x[9]={1000,1000,500,1000,1000,1000,1000,1000,1000};

void PPM_TIME_Init(void)
{
    GPIO_InitTypeDef            GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef     TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef            NVIC_InitStructure;

    PPM_RCC_Periph_COM(PPM_RCC_Periph, ENABLE); //使能PPM IO口时钟
    PPM_TIM_APBxClock_FUN(PPM_TIM_CLK,ENABLE);  //使能TIM6时钟
    
    //GPIO结构体初始化
    GPIO_InitStructure.GPIO_Pin = PPM_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PPM_GPIO_PORT , &GPIO_InitStructure);
    GPIO_SetBits(PPM_GPIO_PORT,PPM_PIN);

    /* ----------------   PWM信号 周期和占空比的计算--------------- */
    // ARR ：自动重装载寄存器的值
    // CLK_cnt：计数器的时钟，等于 Fck_int / (psc+1) = 72M/(psc+1)
    // PWM 信号的周期 T = (ARR+1) * (1/CLK_cnt) = (ARR+1)*(PSC+1) / 72M
    // 占空比P=CCR/(ARR+1)

    //定时器2结构体初始化
    TIM_TimeBaseInitStructure.TIM_Period = PPM_TIM_Period-1; 	//自动重装载值
    TIM_TimeBaseInitStructure.TIM_Prescaler=PPM_TIM_Prescaler-1;  //定时器分频
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInit(PPM_TIM ,&TIM_TimeBaseInitStructure);//初始化TIM6

    TIM_ITConfig(PPM_TIM ,TIM_IT_Update,ENABLE); //允许定时器6更新中断
    TIM_Cmd(PPM_TIM ,ENABLE); //使能定时器6

    NVIC_InitStructure.NVIC_IRQChannel=PPM_TIM_IRQn; //定时器6中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2; //抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0; //子优先级0
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

u8 ppm_count = 1;
u32 time;

//定时器6中断服务程序
void PPM_TIME_IRQHandler(void)
{
    TIM_ClearITPendingBit(PPM_TIM, TIM_IT_Update);
    switch(ppm_count)
    {
    case 1:
        PPM_Out_H;
        time=Channel_x[1];
        TIM_SetAutoreload(PPM_TIM,Channel_x[1]);
        ppm_count++;
        break;
    case 2:
        PPM_Out_L;
        TIM_SetAutoreload(PPM_TIM,499);
        ppm_count++;
        break;
    case 3:
        PPM_Out_H;
        time+=Channel_x[2];
        TIM_SetAutoreload(PPM_TIM,Channel_x[2]);
        ppm_count++;
        break;
    case 4:
        PPM_Out_L;
        TIM_SetAutoreload(PPM_TIM,499);
        ppm_count++;
        break;
    case 5:
        PPM_Out_H;
        time+=Channel_x[3];
        TIM_SetAutoreload(PPM_TIM,Channel_x[3]);
        ppm_count++;
        break;
    case 6:
        PPM_Out_L;
        TIM_SetAutoreload(PPM_TIM,499);
        ppm_count++;
        break;
    case 7:
        PPM_Out_H;
        time+=Channel_x[4];
        TIM_SetAutoreload(PPM_TIM,Channel_x[4]);
        ppm_count++;
        break;
    case 8:
        PPM_Out_L;
        TIM_SetAutoreload(PPM_TIM,499);
        ppm_count++;
        break;
    case 9:
        PPM_Out_H;
        time+=Channel_x[5];
        TIM_SetAutoreload(PPM_TIM,Channel_x[5]);
        ppm_count++;
        break;
    case 10:
        PPM_Out_L;
        TIM_SetAutoreload(PPM_TIM,499);
        ppm_count++;
        break;
    case 11:
        PPM_Out_H;
        time+=Channel_x[6];
        TIM_SetAutoreload(PPM_TIM,Channel_x[6]);
        ppm_count++;
        break;
    case 12:
        PPM_Out_L;
        TIM_SetAutoreload(PPM_TIM,499);
        ppm_count++;
        break;
    case 13:
        PPM_Out_H;
        time+=Channel_x[7];
        TIM_SetAutoreload(PPM_TIM,Channel_x[7]);
        ppm_count++;
        break;
    case 14:
        PPM_Out_L;
        TIM_SetAutoreload(PPM_TIM,499);
        ppm_count++;
        break;
    case 15:
        PPM_Out_H;
        time+=Channel_x[8];
        TIM_SetAutoreload(PPM_TIM,Channel_x[8]);
        ppm_count++;
        break;
    case 16:
        PPM_Out_L;
        TIM_SetAutoreload(PPM_TIM,499);
        ppm_count++;
        break;		
    case 17:
        PPM_Out_H;
        TIM_SetAutoreload(PPM_TIM,15500-time);
        ppm_count++;
        break;
    case 18:
        PPM_Out_L;
        TIM_SetAutoreload(PPM_TIM,499);
        time=0;
        ppm_count=1;
    }

    TIM_ClearITPendingBit(PPM_TIM,TIM_IT_Update);  //清除中断标志位
}
