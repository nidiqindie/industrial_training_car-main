#ifndef _BSP_MOTOR_H_
#define	_BSP_MOTOR_H_

#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_led.h"
#include <stdlib.h>
#include <math.h>
#include "Emm_V5.h"
#include <stdbool.h>
#include "delay.h"
#include <math.h>
#include "Serial.h"
#include "servo.h"
#include "Serial_k210.h"
#include "hwt101.h"
#define side_error 5
#define THE_ANGLE_90 90
#define THE_ANGLE_180 180
#define THE_ANGLE_0 0
#define curAngle yaw
/*PID时基(TIMER6定义*/
#define            PID_TIMEBASE_TIM               TIM6
#define            PID_TIMEBASE_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            PID_TIMEBASE_CLK               RCC_APB1Periph_TIM6
#define            PID_TIMEBASE_Period            10000
#define            PID_TIMEBASE_Prescaler         72

#define            PID_TIMEBASE_IRQn                TIM6_IRQn
#define            PID_TIMEBASE_IRQ_IRQHandler      TIM6_IRQHandler
    /*end*/

extern int pos_flag;
extern int speed;
extern int flag_color;
// 原始任务码是1-红 2-绿 3-蓝，对应地图上1靠近暂存区的颜色是红色，2中间是绿色，3另一边上是蓝色
#define red_num 1
#define green_num 2
#define blue_num 3


// void PID_timebase_init(void);

void move_forward(uint16_t speed,uint16_t j, float quan) ; //speed指每10ms脉冲个数

void move_backward(uint16_t speed,uint16_t j,float quan);  //speed指每10ms脉冲个数

void stop(void);

void move_left(uint16_t speed,uint16_t j,float quan); //speed指每10ms脉冲个数

void move_right(uint16_t speed,uint16_t j,float quan);  //speed指每10ms脉冲个数
void move_forward_pos(uint16_t speed,uint16_t j,float quan);
void move_backward_pos(uint16_t speed,uint16_t j,float quan);

void L_R(int speed , int j) ; //左自转

void R_R(int speed , int j); //右自转
void L_R_impulse(int speed, int j, int impulse);

void R_R_impulse(int speed, int j, int impulse);
void weitiao_2(void);

void yaw_run(float target_yaw, int16_t error_range) ;
void weitiao(int gg);
void oo(void);
void se_huan(uint16_t num);
void Forward(int speeed);
void Angle_Adjust(float tar_angle);
void maduo(uint16_t num);
void TurnLeft(uint16_t vel, uint8_t acc, float mm_or_angle);
void TurnRight(uint16_t vel, uint8_t acc, float mm_or_angle);
#endif


