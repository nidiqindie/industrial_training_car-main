#include "servo.h"
#include <math.h>
#include <stdio.h>
#include "delay.h"
#include "bsp_led.h"
#include "Emm_V5.h"
#include "Serial.h"
#include "stdlib.h"

extern uint8_t Serial_RxFlag, K;

extern uint16_t DistanceX, DistanceY, Size, XX, YY;
extern uint16_t task[2];

/*****************************SERVO**********************/

#define zp_red 50
#define zp_blue 100



static void SERVO_TIM_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //     Ƚ  ?  1 GPIO   ʼ
    RCC_APB2PeriphClockCmd(SERVO_TIM_CH1_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = SERVO_TIM_CH1_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SERVO_TIM_CH1_PORT, &GPIO_InitStructure);

    //     Ƚ  ?  2 GPIO   ʼ
    RCC_APB2PeriphClockCmd(SERVO_TIM_CH2_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = SERVO_TIM_CH2_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SERVO_TIM_CH2_PORT, &GPIO_InitStructure);

    //     Ƚ  ?  3 GPIO   ʼ
    RCC_APB2PeriphClockCmd(SERVO_TIM_CH3_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = SERVO_TIM_CH3_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SERVO_TIM_CH3_PORT, &GPIO_InitStructure);

    //     Ƚ  ?  4 GPIO   ʼ
    RCC_APB2PeriphClockCmd(SERVO_TIM_CH4_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = SERVO_TIM_CH4_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SERVO_TIM_CH4_PORT, &GPIO_InitStructure);
}

/* ----------------   PWM ź     ں ռ ձȵļ   --------------- */
// ARR    Զ   װ ؼĴ     ֵ
// CLK_cnt          ʱ ӣ      Fck_int / (psc+1) = 72M/(psc+1)
// PWM  źŵ      T = (ARR+1) * (1/CLK_cnt) = (ARR+1)*(PSC+1) / 72M
// ռ ձ P=CCR/(ARR+1)

static void SERVO_TIM_Mode_Config(void)
{
    //       ʱ  ʱ  ,   ڲ ʱ  CK_INT=72M
    SERVO_TIM_APBxClock_FUN(SERVO_TIM_CLK, ENABLE);

    /*--------------------ʱ   ṹ   ʼ  ??-------------------------*/
    //        ڣ         Ϊ50Hz

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    //  Զ   װ ؼĴ     ֵ   ۼ TIM_Period+1  Ƶ ʺ    һ     »    ж ??
    TIM_TimeBaseStructure.TIM_Period = SERVO_TIM_Period - 1;
    //     CNT        ʱ   = Fck_int/(psc+1)
    TIM_TimeBaseStructure.TIM_Prescaler = SERVO_TIM_Prescaler - 1;
    // ʱ ӷ Ƶ               ʱ  ʱ  Ҫ õ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    //           ģʽ      Ϊ   ϼ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    //  ظ         ֵ  û õ    ù
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    //   ʼ    ʱ
    TIM_TimeBaseInit(SERVO_TIM, &TIM_TimeBaseStructure);

    TIM_OCInitTypeDef TIM_OCInitStructure;
    //     ΪPWMģʽ1
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    //    ʹ  ??
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    //     ?    ƽ

    TIM_OCInitStructure.TIM_OCIdleState  = TIM_OCIdleState_Set;      // ڿ   ʱ   ??
    TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable;   // PWM   ʹ  ??(    ͨ  )
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; // PWM   ʧ  ??(    ͨ  )
    TIM_OCInitStructure.TIM_OCNPolarity  = TIM_OCNPolarity_High;     //
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;    //
    // TIM_OCInitStructure.TIM_Pulse        = 0;                         //    Ƚϵ      ?
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    TIM_OC1Init(SERVO_TIM, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(SERVO_TIM, TIM_OCPreload_Enable);

    TIM_OC2Init(SERVO_TIM, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(SERVO_TIM, TIM_OCPreload_Enable);

    TIM_OC3Init(SERVO_TIM, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(SERVO_TIM, TIM_OCPreload_Enable);

    TIM_OC4Init(SERVO_TIM, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(SERVO_TIM, TIM_OCPreload_Enable);

    // ʹ ܼ
    TIM_Cmd(SERVO_TIM, ENABLE);
    //     ʹ ܣ   ʹ   ? ö ʱ  ʱ   ˾䲻  Ҫ
}

/**
 * @brief  PWM  ʼ       ??4  PWM     ھ Ϊ20ms.
 * @param  CCRx_Val:  ڼ   PWM           ڿ   Ϊ2000     øò      Ե   ռ ձ .
 *   This parameter can be one of the following values:
 *   0              PWMx  ͬʱ  ռ  IO
 *   1-2000
 * @retval None
 */
void SERVO_TIM_Init(void)
{
    SERVO_TIM_GPIO_Config();
    SERVO_TIM_Mode_Config();
}
/*********************************END OF SERVO**********************/

/**
 * @brief   Ƕ     .
 * @param  channel_n:  ڼ      ??.
 * @param  CCR_Val: Ҫ   õĽǶ ֵ.
 *
 *
 * @retval None
 */
void Servo_Angle_Config(uint8_t channel_n, int16_t angle)
{
    uint16_t compare;
    // Ƕ ת  ΪPWM
    switch (channel_n) {
        case 1:
            // compare = 20 + angle;
            compare = angle;
            TIM_SetCompare1(SERVO_TIM, compare);
            break;
        case 2:
            // compare = 155 - angle;
            compare = angle;

            TIM_SetCompare2(SERVO_TIM, compare);
            break;
        case 3:
            // compare = 50 + angle;
            compare = angle;

            TIM_SetCompare3(SERVO_TIM, compare);
            break;
        case 4:
            compare = angle;
            TIM_SetCompare4(SERVO_TIM, compare);
            break;
        default:
            break;
    }
}

// 将机械臂和转盘调整置初始位置
void start_p(void)
{
    Servo_Angle_Config(1, 60);
    Servo_Angle_Config(3, 130);
    Emm_V5_Pos_Control(1, 00, 300, 250, 0, 01, 00);
    delay_ms(1000);
}
// 车载转盘位置 1为红色 2为绿色 3为蓝色
void zp(uint16_t num)
{

    switch (num) {
        case 1:
            Servo_Angle_Config(2, 49);
            break;
        case 2:
            Servo_Angle_Config(2, 140);
            break;
        case 3:
            Servo_Angle_Config(2, 225);
            break;
        default:
            break;
    }
}

// 从原料区取物块//130142
void catch (uint16_t num)
{
    Servo_Angle_Config(1, 60);
    uint16_t op = 1, x, y;
    op          = 1;
    switch (num) {
        case 1: // h红色

            while (op == 1) {
                x = XX;
                y = YY;
                delay_ms(300);
                if (abs(XX - x) <= 20 && abs(YY - y) <= 20 && K == 'R' && XX >= 100 && XX <= 240 && YY >= 130 && YY <= 230) {
                    Servo_Angle_Config(3, 130);
                    Emm_V5_Pos_Control(1, 1, 400, 250, 3550, 01, 00);
                    delay_ms(500);
                    Servo_Angle_Config(3, 90);
                    delay_ms(1000);
                    Emm_V5_Pos_Control(1, 0, 400, 250, 0, 01, 00);
                    delay_ms(1000);
                    op = 2;
                }
            }
            break;
        case 2: // 绿色

            while (op == 1) {
                x = XX;
                y = YY;
                delay_ms(300);
                if (abs(XX - x) <= 20 && abs(YY - y) <= 20 && K == 'G' && XX >= 100 && XX <= 240 && YY >= 130 && YY <= 230) {
                    Servo_Angle_Config(3, 130);
                    Emm_V5_Pos_Control(1, 1, 400, 250, 3550, 01, 00);
                    delay_ms(500);
                    Servo_Angle_Config(3, 90);
                    delay_ms(1000);
                    Emm_V5_Pos_Control(1, 0, 400, 250, 0, 01, 00);
                    delay_ms(1000);
                    op = 2;
                }
            }
            break;
        case 3: // 蓝色

            while (op == 1) {
                x = XX;
                y = YY;
                delay_ms(300);
                if (abs(XX - x) <= 20 && abs(YY - y) <= 20 && K == 'B' && XX >= 100 && XX <= 240 && YY >= 130 && YY <= 230) {
                    Servo_Angle_Config(3, 130);
                    Emm_V5_Pos_Control(1, 1, 400, 250, 3550, 01, 00);
                    delay_ms(500);
                    Servo_Angle_Config(3, 90);
                    delay_ms(1000);
                    Emm_V5_Pos_Control(1, 0, 400, 250, 0, 01, 00);
                    delay_ms(1000);
                    op = 2;
                }
            }
            break;
        default:
            break;
    }
}
// 将爪子上的物块放到转盘中
void put(uint16_t num)
{

    switch (num) {
        case 1: // 红色
            zp(1);
            Servo_Angle_Config(1, 190);

            delay_ms(1000);
            Emm_V5_Pos_Control(1, 1, 300, 250, 2010, 01, 00); // 脉冲数待定
            delay_ms(1000);
            Servo_Angle_Config(3, 130);
            delay_ms(1000);

            Emm_V5_Pos_Control(1, 0, 300, 250, 0, 01, 00); // 脉冲数待定
            delay_ms(1000);

            break;
        case 2: // 绿
            zp(2);
            Servo_Angle_Config(1, 190);

            delay_ms(1000);
            Emm_V5_Pos_Control(1, 1, 300, 250, 2010, 01, 00); // 脉冲数待定
            delay_ms(1000);
            Servo_Angle_Config(3, 130);
            delay_ms(1000);

            Emm_V5_Pos_Control(1, 0, 300, 250, 0, 01, 00); // 脉冲数待定

            delay_ms(1000);
            break;
        case 3:
            zp(3);
            Servo_Angle_Config(1, 190);

            delay_ms(1000);
            Emm_V5_Pos_Control(1, 1, 300, 250, 2010, 01, 00); // 脉冲数待定
            delay_ms(1000);

            Servo_Angle_Config(3, 130);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 0, 300, 250, 0, 01, 00); // 脉冲数待定

            delay_ms(1000);
            break;
        default:
            break;
    }
    start_p();
}
// 从加工区拿取物块
void catch_huan(void)
{
    start_p();
    Emm_V5_Pos_Control(1, 1, 300, 250, 8900, 01, 00);
    delay_ms(1000);
    Servo_Angle_Config(3, 90);
    delay_ms(1000);
    Emm_V5_Pos_Control(1, 0, 300, 250, 0, 01, 00);
    delay_ms(1000);
}
// 2070
//  将转盘中的物料放到加工区
void put_huan(uint16_t num)
{
    start_p();
    // delay_ms(1000);
    switch (num) {
        case 1:
            zp(1);
            Servo_Angle_Config(1, 195);
            delay_ms(1500);
            Emm_V5_Pos_Control(1, 01, 300, 250, 2070, 01, 00);
            delay_ms(1000);
            Servo_Angle_Config(3, 90);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 0, 300, 250, 0, 01, 00);
            delay_ms(1000);
            Servo_Angle_Config(1, 60);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 01, 300, 250, 8900, 01, 00);
            delay_ms(1500);
            Servo_Angle_Config(3, 130);
            delay_ms(100);
            start_p();
            delay_ms(300);
            break;
        case 2:
            zp(2);
            Servo_Angle_Config(1, 195);
            delay_ms(1500);
            Emm_V5_Pos_Control(1, 01, 300, 250, 2070, 01, 00);
            delay_ms(1000);
            Servo_Angle_Config(3, 90);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 0, 300, 250, 0, 01, 00);
            delay_ms(1000);
            Servo_Angle_Config(1, 60);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 01, 300, 250, 8900, 01, 00);
            delay_ms(1500);
            Servo_Angle_Config(3, 130);
            delay_ms(100);
            start_p();
            delay_ms(300);
            break;
        case 3:
            zp(3);
            Servo_Angle_Config(1, 195);
            delay_ms(1500);
            Emm_V5_Pos_Control(1, 01, 300, 250, 2070, 01, 00);
            delay_ms(1000);
            Servo_Angle_Config(3, 90);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 0, 300, 250, 0, 01, 00);
            delay_ms(1000);
            Servo_Angle_Config(1, 60);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 01, 300, 250, 8900, 01, 00);
            delay_ms(1500);
            Servo_Angle_Config(3, 130);
            delay_ms(100);
            start_p();
            delay_ms(300);
            break;
        default:
            break;
    }
}

// 将转盘中的物料堆叠到成品区域
void maduo(uint16_t num)
{

    switch (num) {
        case 1: // 红色

            Servo_Angle_Config(1, 195);
            Servo_Angle_Config(3, 130);
            zp(1);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 01, 300, 250, 2070, 01, 00); // 脉冲数待定
            delay_ms(1500);
            Servo_Angle_Config(3, 90);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 00, 300, 250, 0, 01, 00); // 脉冲数待定
            delay_ms(2000);
            Servo_Angle_Config(1, 60);
            Emm_V5_Pos_Control(1, 01, 300, 250, 4180, 01, 00); // 脉冲数待定
            delay_ms(3000);
            Servo_Angle_Config(3, 130);
            Emm_V5_Pos_Control(1, 0, 400, 250, 0, 01, 00);
            start_p();
            delay_ms(1000);
            break;
        case 2: // 绿
            Servo_Angle_Config(1, 195);
            Servo_Angle_Config(3, 130);
            zp(2);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 01, 300, 250, 2070, 01, 00); // 脉冲数待定
            delay_ms(1500);
            Servo_Angle_Config(3, 90);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 00, 300, 250, 0, 01, 00); // 脉冲数待定
            delay_ms(1000);
            Servo_Angle_Config(1, 60);
            Emm_V5_Pos_Control(1, 01, 300, 250, 4180, 01, 00); // 脉冲数待定
            delay_ms(3000);
            Servo_Angle_Config(3, 130);
            Emm_V5_Pos_Control(1, 0, 400, 250, 0, 01, 00);
            start_p();
            delay_ms(1000);
            break;
        case 3:
            Servo_Angle_Config(1, 195);
            Servo_Angle_Config(3, 130);
            zp(3);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 01, 300, 250, 2070, 01, 00); // 脉冲数待定
            delay_ms(1500);
            Servo_Angle_Config(3, 90);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 00, 300, 250, 0, 01, 00); // 脉冲数待定
            delay_ms(1000);
            Servo_Angle_Config(1, 60);
            Emm_V5_Pos_Control(1, 01, 300, 250, 4180, 01, 00); // 脉冲数待定
            delay_ms(3000);
            Servo_Angle_Config(3, 130);
            Emm_V5_Pos_Control(1, 0, 400, 250, 0, 01, 00);
            start_p();
            delay_ms(1000);
            break;
        default:
            break;
    }
}