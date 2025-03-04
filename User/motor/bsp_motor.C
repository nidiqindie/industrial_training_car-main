#include "bsp_motor.h"

// 运动控制函数，speed范围在0到5000，j表示加速度
// 角度环pid
/*偏航角PID参数定义*/
extern uint16_t DistanceX, DistanceY, Size;
int flag_adjusting = 1, flag_cx_complete, flag_cy_complete, flag_cy_start;
extern int task[2];
extern uint8_t K;

/*

装车设置顺序
3车头5


4	2
 板子
*/
int mpu = 0, lflag = 0;

int tarAngle = 0; // 目标偏航角角度

float curErr   = 0;               // 角度偏差
float lastErr  = 0;               // 上次角度偏差
float iErr     = 0;               // 累计误差
float pidOut_L = 0, pidOut_R = 0; // PID计算输出值

float Angle_Kp = 30;  // 增量式微分系数
float Angle_Ki = 1.5; // 增量式比例系数
/******************/

// 位置环开启逻辑位
int pos_flag = 0;

// 速度
int speed_allL = 100;
int speed_allR = 100;

// 转弯突变
int Angular_mutation_flag = 0;

// 电机转动方向标志位
int cw_5, cw_2, cw_3, cw_4;
// 转动的速度
#define yaw_vel 30
// 微调坐标
// gg=0就是色环,gg1就是原料区的色块,gg=2就是地上色块，gg=3就是色块上叠加色块
#define gg_0_X 77
#define gg_0_Y 50
#define gg_1_X 83
#define gg_1_Y 58
#define gg_2_X 79
#define gg_2_Y 52
#define gg_3_X 75
#define gg_3_Y 55 
uint8_t Array[16] = {0}; // 步进电机发送数据数组  //步进电机发送数据数组
extern int flag_err, flag_arrive, flag_arrive_lifting;
// 前左L1-3 前右R1-5 后左L2-4 后右R2-2
// 各轮子方向定义
#define DIR_FORWARD_L1  0x00
#define DIR_BACKWARD_L1 0x01

#define DIR_FORWARD_L2  0x00
#define DIR_BACKWARD_L2 0x01

#define DIR_FORWARD_R1  0x01
#define DIR_BACKWARD_R1 0x00

#define DIR_FORWARD_R2  0x01
#define DIR_BACKWARD_R2 0x00

/*脉冲与实际量关系
10000脉冲 --- 前进763mm --- 10000÷763=13.106 ---xPulse_Per_1mm_FB
10000脉冲 --- 左移742mm --- 10000÷742=13.477 ---xPulse_Per_1mm_LR
 5000脉冲 --- 左转108.2° --- 5000÷1082=4.621 ---xPulse_Per_0.1D --- 4.621*10=46.21 ---xPulse_Per_1D
10000脉冲 --- 升降126mm --- 10000÷126=79.365 ---xPulse_Per_1mm_UD
*/

#define xPulse_Per_1mm_FB 13.11   // 前进或后退1毫米需要的脉冲数
#define xPulse_Per_1mm_LR 13.48   // 左移或右移1毫米需要的脉冲数
#define xPulse_Per_1D     46.91   // 左转或右转1度需要的脉冲数
#define xPulse_Per_1mm_UD 105.263 // 上升或下降1毫米需要的脉冲数
// 转动函数
void yaw_run(float target_yaw, int16_t error_range)
{
    if (error_range <= 1) // 角度调整
    {
        Angle_Adjust(target_yaw);
    } else // 大转
    {
        float angle = curAngle;
        if (target_yaw<=180&&target_yaw>=175&&curAngle<-90)
        {
            angle=curAngle+360;
            /* code */
        }
        
        if ((target_yaw >= angle) && (target_yaw - angle) > 2) {
            if ((target_yaw - angle) <= 180) // 左转
            {
                TurnLeft(150, 150, target_yaw );
                delay_ms(2000);
            } else { // 右转
                TurnRight(150, 150, target_yaw );
                delay_ms(2000);
            }
        }

        if ((angle >= target_yaw) && (angle - target_yaw) > 2) {
            if ((angle - target_yaw) <= 180) // 左转
            {
                TurnLeft(150, 150, angle - target_yaw);
                delay_ms(2000);
            } else { // 右转
                TurnRight(150, 150, angle - target_yaw);
                delay_ms(2000);
            }
        }
    }
}
// /*偏航角增量式PID计算*/
// int IncPIDCalcR()
// {
// 	float incPidout;
// 	curErr =  curAngle-tarAngle;  //当前误差
// 	incPidout = Angle_Kp*(curErr-lastErr) + (Angle_Ki)*curErr;  //增量式PI控制器
// 	lastErr = curErr;

// 	return incPidout;  //返回增量值
// }

// int IncPIDCalcL()
// {
// 	float incPidout;
// 	curErr = tarAngle - curAngle;  //当前误差
// 	incPidout = Angle_Kp*(curErr-lastErr) + (Angle_Ki)*curErr;  //增量式PI控制器
// 	lastErr = curErr;

// 	return incPidout;  //返回增量值
// }

/*

装车设置顺序
3	5


4	2
 板子
*/

// 这几个控制运动函数speed表示速度，100速度就行
// 前进
#define reparation_forward 0
void move_forward(uint16_t speed, uint16_t j, float quan) // speed指每10ms脉冲个数
{
    cw_5 = 1;
    cw_2 = 1;
    cw_3 = 0;
    cw_4 = 0;
    Emm_V5_Pos_Control(5, 1, speed - (reparation_forward * speed), j, (int)(3200 * quan), 0, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(2, 1, speed - (reparation_forward * speed), j, (int)(3200 * quan), 0, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(3, 0, speed + (reparation_forward * speed), j, (int)(3200 * quan), 0, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(4, 0, speed + (reparation_forward * speed), j, (int)(3200 * quan), 0, 0);
    delay_ms(1);
}

// 后退
#define reparation_backward 0
void move_backward(uint16_t speed, uint16_t j, float quan) // speed指每10ms脉冲个数
{

    cw_5 = 0;
    cw_2 = 0;
    cw_3 = 1;
    cw_4 = 1;
    Emm_V5_Pos_Control(5, 0, speed - (reparation_backward * speed), j, (int)(3200 * quan), 0, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(2, 0, speed - (reparation_backward * speed), j, (int)(3200 * quan), 0, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(3, 1, speed + (reparation_backward * speed), j, (int)(3200 * quan), 0, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(4, 1, speed + (reparation_backward * speed), j, (int)(3200 * quan), 0, 0);
    delay_ms(1);
}

// 左
#define reparation_left 0.2
void move_left(uint16_t speed, uint16_t j, float quan) // speed指每10ms脉冲个数
{

    cw_5 = 1;
    cw_2 = 0;
    cw_3 = 1;
    cw_4 = 0;
    Emm_V5_Pos_Control(5, 1, speed + ceil(reparation_left * speed), j, (int)(3200 * quan), 0, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(2, 0, speed - ceil(reparation_left * speed), j, (int)(3200 * quan), 0, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(3, 1, speed + ceil(reparation_left * speed), j, (int)(3200 * quan), 0, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(4, 0, speed - ceil(reparation_left * speed), j, (int)(3200 * quan), 0, 0);
    delay_ms(1);
}
// 右
#define reparation_right 0.1
void move_right(uint16_t speed, uint16_t j, float quan) // speed指每10ms脉冲个数
{

    cw_5 = 0;
    cw_2 = 1;
    cw_3 = 0;
    cw_4 = 1;

    Emm_V5_Pos_Control(5, 0, speed + ceil(reparation_right * speed), j, (int)(3200 * quan), 0, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(2, 1, speed - ceil(reparation_right * speed), j, (int)(3200 * quan), 0, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(3, 0, speed + ceil(reparation_right * speed), j, (int)(3200 * quan), 0, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(4, 1, speed - ceil(reparation_right * speed), j, (int)(3200 * quan), 0, 0);
    delay_ms(1);
}

// 绝对位置模式
void move_forward_pos(uint16_t speed, uint16_t j, float quan) // speed指每10ms脉冲个数
{
    cw_5 = 1;
    cw_2 = 1;
    cw_3 = 0;
    cw_4 = 0;
    Emm_V5_Pos_Control(5, 1, speed - 1, j, (int)(3200 * quan), 1, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(2, 1, speed - 1, j, (int)(3200 * quan), 1, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(3, 0, speed + 2, j, (int)(3200 * quan), 1, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(4, 0, speed + 1, j, (int)(3200 * quan), 1, 0);
    delay_ms(1);
}
// 后退
void move_backward_pos(uint16_t speed, uint16_t j, float quan) // speed指每10ms脉冲个数
{

    cw_5 = 0;
    cw_2 = 0;
    cw_3 = 1;
    cw_4 = 1;
    Emm_V5_Pos_Control(5, 0, speed - 1, j, (int)(3200 * quan), 1, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(2, 0, speed - 1, j, (int)(3200 * quan), 1, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(3, 1, speed + 2, j, (int)(3200 * quan), 1, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(4, 1, speed + 1, j, (int)(3200 * quan), 1, 0);
    delay_ms(1);
}
// 绝对位置模式清零
void oo(void)
{
    Emm_V5_Reset_CurPos_To_Zero(5);
    delay_ms(1);
    Emm_V5_Reset_CurPos_To_Zero(2);
    delay_ms(1);
    Emm_V5_Reset_CurPos_To_Zero(3);
    delay_ms(1);

    Emm_V5_Reset_CurPos_To_Zero(4);
    delay_ms(1);
}

// i表示是否开启多机同步
void stop(void)
{
    Emm_V5_Stop_Now(0, 0);
    delay_ms(1);
}

void L_R(int speed, int j) // 左自转
{

    Emm_V5_Vel_Control(5, 1, speed - 2, j, 0);
    delay_ms(1);

    Emm_V5_Vel_Control(2, 1, speed - 1, j, 0);
    delay_ms(1);

    Emm_V5_Vel_Control(3, 1, speed + 1, j, 0);
    delay_ms(1);

    Emm_V5_Vel_Control(4, 1, speed + 1, j, 0);
    delay_ms(1);
}

void R_R(int speed, int j) // 右自转
{
    Emm_V5_Vel_Control(5, 0, speed - 2, j, 0);
    delay_ms(1);

    Emm_V5_Vel_Control(2, 0, speed - 1, j, 0);
    delay_ms(1);

    Emm_V5_Vel_Control(3, 0, speed + 1, j, 0);
    delay_ms(1);

    Emm_V5_Vel_Control(4, 0, speed + 1, j, 0);
    delay_ms(1);
}
void L_R_impulse(int speed, int j, int impulse)
{

    Emm_V5_Pos_Control(5, 1, speed, j, impulse, 0, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(2, 1, speed, j, impulse, 0, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(3, 1, speed, j, impulse, 0, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(4, 1, speed, j, impulse, 0, 0);
    delay_ms(1);
}
void R_R_impulse(int speed, int j, int impulse)
{

    Emm_V5_Pos_Control(5, 0, speed, j, impulse, 0, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(2, 0, speed, j, impulse, 0, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(3, 0, speed, j, impulse, 0, 0);
    delay_ms(1);

    Emm_V5_Pos_Control(4, 0, speed, j, impulse, 0, 0);
    delay_ms(1);
}
void Backward(int speeed)
{
    Emm_V5_Vel_Control(5, 0, speeed, 1, 0);
    delay_ms(1);

    Emm_V5_Vel_Control(2, 0, speeed, 1, 0);
    delay_ms(1);

    Emm_V5_Vel_Control(3, 1, speeed, 1, 0);
    delay_ms(1);

    Emm_V5_Vel_Control(4, 1, speeed, 1, 0);
    delay_ms(1);
}

void Forward(int speeed)
{
    Emm_V5_Vel_Control(5, 1, speeed, 1, 0);
    delay_ms(1);

    Emm_V5_Vel_Control(2, 1, speeed, 1, 0);
    delay_ms(1);

    Emm_V5_Vel_Control(3, 0, speeed, 1, 0);
    delay_ms(1);

    Emm_V5_Vel_Control(4, 0, speeed, 1, 0);
    delay_ms(1);
}

void R_Translation(int speeed)
{

    Emm_V5_Vel_Control(5, 0, speeed, 1, 0);
    delay_ms(1);

    Emm_V5_Vel_Control(2, 1, speeed, 1, 0);
    delay_ms(1);

    Emm_V5_Vel_Control(3, 0, speeed, 1, 0);
    delay_ms(1);

    Emm_V5_Vel_Control(4, 1, speeed, 1, 0);
    delay_ms(1);
}

void L_Translation(int speeed)
{

    Emm_V5_Vel_Control(5, 1, speeed, 1, 0);
    delay_ms(1);

    Emm_V5_Vel_Control(2, 0, speeed, 1, 0);
    delay_ms(1);

    Emm_V5_Vel_Control(3, 1, speeed, 1, 0);
    delay_ms(1);

    Emm_V5_Vel_Control(4, 0, speeed, 1, 0);
    delay_ms(1);
}

#define gg1_speed 15
#define gg0_calibation 1
// gg=0就是色环,gg1就是原料区的色块,gg=2就是地上色块
void weitiao(int gg)
{
    catch_p();
    // 原料区的色块
    if (gg == 1) {
        flag_adjusting = 1;

        while (flag_adjusting <= 2 && flag_adjusting != 0) {

            if (flag_cx_complete == 0 && XX != 0) {
                if (XX < gg_1_X - 2) {
                    Forward(gg1_speed);
                    while (XX < gg_1_X - 2);

                }

                else if (XX > gg_1_X + 2) {
                    Backward(gg1_speed);
                    while (XX > gg_1_X + 2);
                }

                stop();
                flag_cx_complete = 1;
                flag_cy_complete = 0;
                flag_cy_start    = 1;
            }
            if (flag_cy_start == 1 && flag_cy_complete == 0 && YY != 0) {
                if (YY < gg_1_Y - 2) {
                    R_Translation(gg1_speed);
                    while (YY < gg_1_Y - 2);

                } else if (YY > gg_1_Y + 2) {
                    L_Translation(gg1_speed);
                    while (YY > gg_1_Y + 2);
                }
                stop();
                flag_cy_complete = 1;
                flag_adjusting++;
                flag_cx_complete = 0;
                delay_ms(100);
            }
        }
    }
    // 色环
    if (gg == 0) {
        flag_adjusting = 1;
        while (flag_adjusting <= 4 && flag_adjusting != 0) {
            printf("XX1:%d YY1:%d\n\r", XX1, YY1);
            if (flag_cx_complete == 0 && XX1 != 0) {
                if (XX1 < gg_0_X - gg0_calibation) {
                    Forward(5);
                    while (XX1 < gg_0_X - gg0_calibation);

                }

                else if (XX1 > gg_0_X + gg0_calibation) {
                    Backward(5);
                    while (XX1 > gg_0_X + gg0_calibation);
                }
                stop();
                flag_cx_complete = 1;
                flag_cy_complete = 0;
                flag_cy_start    = 1;
            }
            if (flag_cy_start == 1 && flag_cy_complete == 0 && YY1 != 0) {
                if (YY1 < gg_0_Y - gg0_calibation) {
                    R_Translation(5);
                    while (YY1 < gg_0_Y - gg0_calibation);

                } else if (YY1 > gg_0_Y + gg0_calibation) {
                    L_Translation(5);
                    while (YY1 > gg_0_Y + gg0_calibation);
                }
                stop();
                flag_cy_complete = 1;
                flag_adjusting++;
                flag_cx_complete = 0;
                delay_ms(200);
            }
        }
    }
    // 地上的色块
    if (gg == 2) {
        flag_adjusting = 1;
        while (flag_adjusting <= 4 && flag_adjusting != 0) {
            if (flag_cx_complete == 0 && XX != 0) {
                if (XX < gg_2_X - 2) {
                    Forward(15);
                    while (XX < gg_2_X - 2);

                }

                else if (XX > gg_2_X + 2) {
                    Backward(15);
                    while (XX > gg_2_X + 2);
                }
                stop();
                flag_cx_complete = 1;
                flag_cy_complete = 0;
                flag_cy_start    = 1;
            }
            if (flag_cy_start == 1 && flag_cy_complete == 0 && YY != 0) {
                if (YY < gg_2_Y - 2) {
                    R_Translation(15);
                    while (YY < gg_2_Y - 2);

                } else if (YY > gg_2_Y + 2) {
                    L_Translation(15);
                    while (YY > gg_2_Y + 2);
                }
                stop();
                flag_cy_complete = 1;
                flag_adjusting++;
                flag_cx_complete = 0;
                delay_ms(200);
            }
        }
    }

    // 地上的色块
    if (gg == 3) {
        flag_adjusting = 1;
        while (flag_adjusting <= 2 && flag_adjusting != 0) {
            if (flag_cx_complete == 0 && XX != 0) {
                if (XX < gg_2_X - 2) {
                    Forward(15);
                    while (XX < gg_2_X - 2);

                }

                else if (XX > gg_2_X + 2) {
                    Backward(15);
                    while (XX > gg_2_X + 2);
                }
                stop();
                flag_cx_complete = 1;
                flag_cy_complete = 0;
                flag_cy_start    = 1;
            }
            if (flag_cy_start == 1 && flag_cy_complete == 0 && YY != 0) {
                if (YY < gg_2_Y - 2) {
                    R_Translation(15);
                    while (YY < gg_2_Y - 2);

                } else if (YY > gg_2_Y + 2) {
                    L_Translation(15);
                    while (YY > gg_2_Y + 2);
                }
                stop();
                flag_cy_complete = 1;
                flag_adjusting++;
                flag_cx_complete = 0;
                delay_ms(200);
            }
        }
    }
}
void weitiao_2(void)
{
    char KKK;
    int x;
    x = XX;

    while (1) {
        delay_ms(1000);

        if (abs(XX - x) >= 10)
            break;
        x = XX;
    }
    KKK = K;
    x   = XX;
    while (1) {
        delay_ms(50);

        if (abs(XX - x) <= 20 && KKK != K)
            break;
        x = XX;
    }
    weitiao(1);
}
void se_huan(uint16_t num)
{

    if (num == 1) {
        move_forward_pos(100, 10, 0);
        delay_ms(2000);
    }
    if (num == 2) {
        move_forward_pos(100, 10, 0.6);
        delay_ms(2000);
    }
    if (num == 3) {
        move_forward_pos(100, 10, 1.2);
        delay_ms(2000);
    }
}
/*

装车设置顺序
3车头5


4	2
 板子
*/

void Usart1_SendArray(uint8_t *array, uint16_t num)
{
    Usart_SendArray(DEBUG_USART1, array, num);
}

// 小车步进电机--同步模式
// 前左L1-3 前右R1-5 后左L2-4 后右R2-2
void CarGoTogether(uint8_t dir, uint16_t vel, uint8_t acc, float mm_or_angle)
{
    uint8_t Array[16] = {0};

    uint32_t clk = 0;
    if (dir == 1 || dir == 2) clk = (uint32_t)(mm_or_angle * xPulse_Per_1mm_FB);
    if (dir == 3 || dir == 4) clk = (uint32_t)(mm_or_angle * xPulse_Per_1mm_LR);
    if (dir == 5 || dir == 6) clk = (uint32_t)(mm_or_angle * xPulse_Per_1D);
    // 装载命令
    Array[0] = 0x03; // 地址
    Array[1] = 0xFD;
    if (dir == 1)
        Array[2] = DIR_FORWARD_L1; // 功能码
    else if (dir == 2)
        Array[2] = DIR_BACKWARD_L1;
    else if (dir == 3)
        Array[2] = DIR_BACKWARD_L1;
    else if (dir == 4)
        Array[2] = DIR_FORWARD_L1;
    else if (dir == 5)
        Array[2] = DIR_BACKWARD_L1;
    else if (dir == 6)
        Array[2] = DIR_FORWARD_L1;
    Array[3]  = (uint8_t)(vel >> 8);  // 速度(RPM)高8位字节
    Array[4]  = (uint8_t)(vel >> 0);  // 速度(RPM)低8位字节
    Array[5]  = acc;                  // 加速度，注意：0是直接启动
    Array[6]  = (uint8_t)(clk >> 24); // 脉冲数(bit24 - bit31)
    Array[7]  = (uint8_t)(clk >> 16); // 脉冲数(bit16 - bit23)
    Array[8]  = (uint8_t)(clk >> 8);  // 脉冲数(bit8  - bit15)
    Array[9]  = (uint8_t)(clk >> 0);  // 脉冲数(bit0  - bit7 )
    Array[10] = 0x00;                 // 相位/绝对标志，false为相对运动，true为绝对值运动
    Array[11] = 0x01;                 // 多机同步运动标志，false为不启用，true为启用
    Array[12] = 0x6B;                 // 校验字节
                                      // 发送命令
    Usart1_SendArray(Array, 13);
    delay_ms(2);

    Array[0] = 0x05;
    if (dir == 1)
        Array[2] = DIR_FORWARD_R1;
    else if (dir == 2)
        Array[2] = DIR_BACKWARD_R1;
    else if (dir == 3)
        Array[2] = DIR_FORWARD_R1;
    else if (dir == 4)
        Array[2] = DIR_BACKWARD_R1;
    else if (dir == 5)
        Array[2] = DIR_FORWARD_R1;
    else if (dir == 6)
        Array[2] = DIR_BACKWARD_R1;
    Usart1_SendArray(Array, 13);
    delay_ms(2);

    Array[0] = 0x04;
    if (dir == 1)
        Array[2] = DIR_FORWARD_L2;
    else if (dir == 2)
        Array[2] = DIR_BACKWARD_L2;
    else if (dir == 3)
        Array[2] = DIR_FORWARD_L2;
    else if (dir == 4)
        Array[2] = DIR_BACKWARD_L2;
    else if (dir == 5)
        Array[2] = DIR_BACKWARD_L2;
    else if (dir == 6)
        Array[2] = DIR_FORWARD_L2;
    Usart1_SendArray(Array, 13);
    delay_ms(2);

    Array[0] = 0x02;
    if (dir == 1)
        Array[2] = DIR_FORWARD_R2;
    else if (dir == 2)
        Array[2] = DIR_BACKWARD_R2;
    else if (dir == 3)
        Array[2] = DIR_BACKWARD_R2;
    else if (dir == 4)
        Array[2] = DIR_FORWARD_R2;
    else if (dir == 5)
        Array[2] = DIR_FORWARD_R2;
    else if (dir == 6)
        Array[2] = DIR_BACKWARD_R2;
    Usart1_SendArray(Array, 13);
    delay_ms(2);

    // 启动同步控制
    Array[0] = 0x00;
    Array[1] = 0xFF;
    Array[2] = 0x66;
    Array[3] = 0x6B; // 校验字节
    Usart1_SendArray(Array, 4);
    delay_ms(10);
}
void TurnLeft(uint16_t vel, uint8_t acc, float mm_or_angle)
{
    CarGoTogether(5, vel, acc, mm_or_angle);
    delay_ms(2);
}

void TurnRight(uint16_t vel, uint8_t acc, float mm_or_angle)
{
    CarGoTogether(6, vel, acc, mm_or_angle);
    delay_ms(2);
}
// 角度校准
void Angle_Adjust(float tar_angle)
{

    float angle_err = tar_angle - curAngle;
    if (tar_angle <= 185 && tar_angle >= 175 && curAngle < 0) {
        angle_err = tar_angle - curAngle - 360;
        /* code */
    }

    if (angle_err > 0) {
        TurnLeft(150, 150, angle_err);
        delay_ms(800);
    }

    else if (angle_err < 0) {
        angle_err = -angle_err;
        TurnRight(150, 150, angle_err);
        delay_ms(800);
    }

    angle_err = tar_angle - curAngle;
    if (tar_angle <= 185 && tar_angle >= 175 && curAngle < 0) {
        angle_err = tar_angle - curAngle - 360;
        /* code */
    }
    if (angle_err >= 0.1) {
        TurnLeft(150, 150, angle_err);
        delay_ms(600);
    }

    else if (angle_err <= 0.1) {
        angle_err = -angle_err;
        TurnRight(150, 150, angle_err);
        delay_ms(600);
    }
  
}
