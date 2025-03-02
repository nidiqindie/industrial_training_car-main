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
//servo1为机械臂舵机，servo2为转盘舵机，servo3为爪子舵机
//转盘舵机PWM值
#define servo2_red 53       //红色转盘
#define servo2_green 143    //绿色转盘
#define servo2_blue 233     //蓝色转盘

//机械臂舵机PWM
#define servo1_out 60     //机械臂向外
#define servo1_in 184      //机械臂向内

//爪子舵机PWM值
#define servo3_close 65     //爪子抓取//57
#define servo3_open 80      //爪子展开

//机械臂步进电机脉冲数
#define start_position 0    //爪子起始位置，最高处
#define catch_rew 2800      //从原料区抓取物料
#define put_turntable 2010  //把物料放在车载转盘
#define catch_turntable 2070//从车载转盘中拿去物料
#define put_process 8900    //将物料放在加工区
#define catch_process 8900  //从加工区拿去物料
#define put_store 8900      //将物料放在暂存区
#define put_maduo 4180      //将物料进行码垛
#define y_center        150       // 源180

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

// 将机械臂调整至抓取状态
void catch_p(void)
{
    Servo_Angle_Config(1, servo1_out);
    delay_ms(100);
    Servo_Angle_Config(3, servo3_open);
    delay_ms(100);
    Emm_V5_Pos_Control(1, 00, 300, 250, start_position, 01, 00);
    delay_ms(1);
}

//机械臂默认状态，爪子向内
void standby_p(void)
{
    Servo_Angle_Config(1,servo1_in);
    delay_ms(100);
    Emm_V5_Pos_Control(1, 00, 300, 250, start_position, 01, 00);
    delay_ms(1);
}


// 车载转盘位置 1为红色 2为绿色 3为蓝色
void zp(uint16_t num)
{

    switch (num) {
        case 1:
            Servo_Angle_Config(2, servo2_red);
            break;
        case 2:
            Servo_Angle_Config(2, servo2_green);
            break;
        case 3:
            Servo_Angle_Config(2, servo2_blue);
            break;
        default:
            break;
    }
}

// 从原料区取物块//130142

void catch (uint16_t num)
{
    uint16_t op = 1, x, y;
    op          = 1;
    switch (num) {
        case 1: // h红色

            while (op == 1) {
                x = XX;
                y = YY;
                delay_ms(300);
                if (abs(XX - x) <= 20 && abs(YY - y) <= 20 && K == 'R' && XX >= 100 && XX <= 240 && YY >= y_center-50 && YY <= y_center+50) {
                    Emm_V5_Pos_Control(1, 1, 400, 250, catch_rew, 01, 00);
                    delay_ms(500);
                    Servo_Angle_Config(3, servo3_close);
                    delay_ms(500);
                    Emm_V5_Pos_Control(1, 0, 400, 250, start_position, 01, 00);
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
                if (abs(XX - x) <= 20 && abs(YY - y) <= 20 && K == 'G' && XX >= 100 && XX <= 240 && YY >= y_center - 50 && YY <= y_center + 50) {
                    Emm_V5_Pos_Control(1, 1, 400, 250, catch_rew, 01, 00);
                    delay_ms(500);
                    Servo_Angle_Config(3, servo3_close);
                    delay_ms(500);
                    Emm_V5_Pos_Control(1, 0, 400, 250, start_position, 01, 00);
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
                if (abs(XX - x) <= 20 && abs(YY - y) <= 20 && K == 'B' && XX >= 100 && XX <= 240 && YY >= y_center - 50 && YY <= y_center + 50) {
                    Emm_V5_Pos_Control(1, 1, 400, 250, catch_rew, 01, 00);
                    delay_ms(500);
                    Servo_Angle_Config(3, servo3_close);
                    delay_ms(500);
                    Emm_V5_Pos_Control(1, 0, 400, 250, start_position, 01, 00);
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
            Servo_Angle_Config(1, servo1_in);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 1, 300, 250, put_turntable, 01, 00); // 脉冲数待定
            delay_ms(800);
            Servo_Angle_Config(3, servo3_open);
            delay_ms(500);
            Emm_V5_Pos_Control(1, 0, 300, 250, start_position, 01, 00); // 脉冲数待定
            delay_ms(1000);

            break;
        case 2: // 绿
            zp(2);
            Servo_Angle_Config(1, servo1_in);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 1, 300, 250, put_turntable, 01, 00); // 脉冲数待定
            delay_ms(800);
            Servo_Angle_Config(3, servo3_open);
            delay_ms(500);
            Emm_V5_Pos_Control(1, 0, 300, 250, start_position, 01, 00); // 脉冲数待定
            delay_ms(1000);

            break;
        case 3:
            zp(3);
            Servo_Angle_Config(1, servo1_in);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 1, 300, 250, put_turntable, 01, 00); // 脉冲数待定
            delay_ms(800);
            Servo_Angle_Config(3, servo3_open);
            delay_ms(500);
            Emm_V5_Pos_Control(1, 0, 300, 250, start_position, 01, 00); // 脉冲数待定
            delay_ms(1000);
            break;
        default:
            break;
    }
}
// 从加工区拿取物块
void catch_huan(void)
{
    Emm_V5_Pos_Control(1, 1, 300, 250, catch_process, 01, 00);
    delay_ms(800);
    Servo_Angle_Config(3, servo3_close);
    delay_ms(500);
    Emm_V5_Pos_Control(1, 0, 300, 250, start_position, 01, 00);
    delay_ms(1000);
}
// 2070
//  将转盘中的物料放到加工区
void put_huan(uint16_t num)
{
    switch (num) {
        case 1:
            zp(1);
            Servo_Angle_Config(1, servo1_in);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 01, 300, 250, catch_turntable, 01, 00);
            delay_ms(600);
            Servo_Angle_Config(3, servo3_close);
            delay_ms(500);
            Emm_V5_Pos_Control(1, 0, 300, 250, start_position, 01, 00);
            delay_ms(500);
            Servo_Angle_Config(1, servo1_out);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 01, 300, 250, put_process, 01, 00);
            delay_ms(800);
            Servo_Angle_Config(3, servo3_open);
            delay_ms(100);
            break;
        case 2:
            zp(2);
            Servo_Angle_Config(1, servo1_in);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 01, 300, 250, catch_turntable, 01, 00);
            delay_ms(600);
            Servo_Angle_Config(3, servo3_close);
            delay_ms(500);
            Emm_V5_Pos_Control(1, 0, 300, 250, start_position, 01, 00);
            delay_ms(500);
            Servo_Angle_Config(1, servo1_out);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 01, 300, 250, put_process, 01, 00);
            delay_ms(800);
            Servo_Angle_Config(3, servo3_open);
            delay_ms(100);
            break;
        case 3:
            zp(3);
            Servo_Angle_Config(1, servo1_in);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 01, 300, 250, catch_turntable, 01, 00);
            delay_ms(600);
            Servo_Angle_Config(3, servo3_close);
            delay_ms(500);
            Emm_V5_Pos_Control(1, 0, 300, 250, start_position, 01, 00);
            delay_ms(500);
            Servo_Angle_Config(1, servo1_out);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 01, 300, 250, put_process, 01, 00);
            delay_ms(800);
            Servo_Angle_Config(3, servo3_open);
            delay_ms(100);
        default:
            break;
    }
}

// 将转盘中的物料堆叠到成品区域
void maduo(uint16_t num)
{

    switch (num) {
        case 1: // 红色

            Servo_Angle_Config(1, servo1_in);
            Servo_Angle_Config(3, servo3_open);
            zp(1);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 01, 300, 250, catch_turntable, 01, 00); // 脉冲数待定
            delay_ms(500);
            Servo_Angle_Config(3, servo3_close);
            delay_ms(500);
            Emm_V5_Pos_Control(1, 00, 300, 250, start_position, 01, 00); // 脉冲数待定
            delay_ms(700);
            Servo_Angle_Config(1, servo1_out);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 01, 300, 250, put_maduo, 01, 00); // 脉冲数待定
            delay_ms(800);
            Servo_Angle_Config(3, servo3_open);
            delay_ms(500);
            Emm_V5_Pos_Control(1, 0, 400, 250, start_position, 01, 00);
            delay_ms(1000);
            break;
        case 2: // 绿
            Servo_Angle_Config(1, servo1_in);
            Servo_Angle_Config(3, servo3_open);
            zp(2);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 01, 300, 250, catch_turntable, 01, 00); // 脉冲数待定
            delay_ms(500);
            Servo_Angle_Config(3, servo3_close);
            delay_ms(500);
            Emm_V5_Pos_Control(1, 00, 300, 250, start_position, 01, 00); // 脉冲数待定
            delay_ms(700);
            Servo_Angle_Config(1, servo1_out);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 01, 300, 250, put_maduo, 01, 00); // 脉冲数待定
            delay_ms(800);
            Servo_Angle_Config(3, servo3_open);
            delay_ms(500);
            Emm_V5_Pos_Control(1, 0, 400, 250, start_position, 01, 00);
            delay_ms(1000);
            break;
        case 3:
            Servo_Angle_Config(1, servo1_in);
            Servo_Angle_Config(3, servo3_open);
            zp(3);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 01, 300, 250, catch_turntable, 01, 00); // 脉冲数待定
            delay_ms(500);
            Servo_Angle_Config(3, servo3_close);
            delay_ms(500);
            Emm_V5_Pos_Control(1, 00, 300, 250, start_position, 01, 00); // 脉冲数待定
            delay_ms(700);
            Servo_Angle_Config(1, servo1_out);
            delay_ms(1000);
            Emm_V5_Pos_Control(1, 01, 300, 250, put_maduo, 01, 00); // 脉冲数待定
            delay_ms(800);
            Servo_Angle_Config(3, servo3_open);
            delay_ms(500);
            Emm_V5_Pos_Control(1, 0, 400, 250, start_position, 01, 00);
            delay_ms(1000);
            break;
        default:
            break;
    }
}