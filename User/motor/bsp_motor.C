#include "bsp_motor.h"


//运动控制函数，speed范围在0到5000，j表示加速度
//角度环pid
/*偏航角PID参数定义*/
extern uint16_t DistanceX,DistanceY,Size;
int flag_adjusting=1,flag_cx_complete,flag_cy_complete,flag_cy_start;
extern int task[2];
extern uint8_t K;

/*

装车设置顺序
3	5


4	2
 板子
*/
int mpu=0,lflag=0;

int tarAngle = 0; // 目标偏航角角度

float curErr = 0;  //角度偏差
float lastErr = 0;  //上次角度偏差
float iErr = 0;  //累计误差
float pidOut_L = 0,pidOut_R = 0;  //PID计算输出值

float Angle_Kp = 30;  //增量式微分系数
float Angle_Ki = 1.5;  //增量式比例系数
/******************/

//位置环开启逻辑位
int pos_flag=0;

//速度
int speed_allL=100;
int speed_allR=100;

//转弯突变
int Angular_mutation_flag=0;

//电机转动方向标志位
int cw_5,cw_2,cw_3,cw_4;

void yaw_run(int16_t target_yaw,int16_t error_range)
{
	
	if((target_yaw >= curAngle) && (target_yaw-curAngle) > 2)  
	{
		if((target_yaw - curAngle) <= 180) //右转
		{
			L_R(50,10);
		}
		else{                //左转
             R_R(50,10);
		}
		while(abs(target_yaw-curAngle) > error_range);
		stop();
	}
	
	if((curAngle >= target_yaw) && (curAngle-target_yaw) > 2) 
	{
		if((curAngle- target_yaw) <= 180) //左转
		{
	R_R(50,10);
		}
		else{                //右转
	L_R(50,10);	
	}
		while(abs(target_yaw-curAngle) > error_range);
		stop();
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

//这几个控制运动函数speed表示速度，100速度就行
void move_forward(uint16_t speed,uint16_t j, float quan)  //speed指每10ms脉冲个数
{
cw_5=1;
cw_2=1;
cw_3=0;
cw_4=0;
Emm_V5_Pos_Control(5, 1, speed, j, (int)(3200*quan), 0, 0);
	 delay_ms(1);
	
Emm_V5_Pos_Control(2, 1, speed-2, j, (int)(3200*quan), 0, 0);
	 delay_ms(1);
	
Emm_V5_Pos_Control(3, 0, speed+2, j, (int)(3200*quan), 0, 0);
	 delay_ms(1);
	
Emm_V5_Pos_Control(4, 0, speed, j, (int)(3200*quan), 0, 0);
	 delay_ms(1);


}


//后退
void move_backward(uint16_t speed,uint16_t j,float quan)  //speed指每10ms脉冲个数
{

cw_5=0;
cw_2=0;
cw_3=1;
cw_4=1;
Emm_V5_Pos_Control(5, 0, speed-1, j,(int)(3200*quan), 0, 0);
	 delay_ms(1);
	
Emm_V5_Pos_Control(2, 0, speed+1, j, (int)(3200*quan), 0, 0);
	 delay_ms(1);
	
Emm_V5_Pos_Control(3, 1, speed-1, j, (int)(3200*quan), 0, 0);
	 delay_ms(1);
	
Emm_V5_Pos_Control(4, 1, speed+1, j,(int)(3200*quan), 0, 0);
	 delay_ms(1);


}
//绝对位置模式
void move_forward_pos(uint16_t speed,uint16_t j, float quan)  //speed指每10ms脉冲个数
{
cw_5=1;
cw_2=1;
cw_3=0;
cw_4=0;
Emm_V5_Pos_Control(5, 1, speed-1, j, (int)(3200*quan), 1, 0);
	 delay_ms(1);
	
Emm_V5_Pos_Control(2, 1, speed-1, j, (int)(3200*quan), 1, 0);
	 delay_ms(1);
	
Emm_V5_Pos_Control(3, 0, speed+2, j, (int)(3200*quan), 1, 0);
	 delay_ms(1);
	
Emm_V5_Pos_Control(4, 0, speed+1, j, (int)(3200*quan), 1, 0);
	 delay_ms(1);


}
//后退
void move_backward_pos(uint16_t speed,uint16_t j,float quan)  //speed指每10ms脉冲个数
{

cw_5=0;
cw_2=0;
cw_3=1;
cw_4=1;
Emm_V5_Pos_Control(5, 0, speed-1, j,(int)(3200*quan), 1, 0);
	 delay_ms(1);
	
Emm_V5_Pos_Control(2, 0, speed-1, j, (int)(3200*quan),1, 0);
	 delay_ms(1);
	
Emm_V5_Pos_Control(3, 1, speed+2, j, (int)(3200*quan),1, 0);
	 delay_ms(1);
	
Emm_V5_Pos_Control(4, 1, speed+1, j,(int)(3200*quan), 1, 0);
	 delay_ms(1);


}
//绝对位置模式清零
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


//左

void move_left(uint16_t speed,uint16_t j,float quan)  //speed指每10ms脉冲个数
{

cw_5=1;
cw_2=0;
cw_3=1;
cw_4=0;
Emm_V5_Pos_Control(5, 1, speed, j, (int)(3200*quan), 0, 0);
	 delay_ms(1);
	
Emm_V5_Pos_Control(2, 0, speed-1, j, (int)(3200*quan), 0, 0);
	 delay_ms(1);
	
Emm_V5_Pos_Control(3, 1, speed, j, (int)(3200*quan), 0, 0);
	 delay_ms(1);
	
Emm_V5_Pos_Control(4, 0, speed-2, j, (int)(3200*quan), 0, 0);
	 delay_ms(1);

}
//右
void move_right(uint16_t speed,uint16_t j,float quan)  //speed指每10ms脉冲个数
{

cw_5=0;
cw_2=1;
cw_3=0;
cw_4=1;

Emm_V5_Pos_Control(5, 0, speed, j, (int)(3200*quan), 0, 0);
	 delay_ms(1);
	
Emm_V5_Pos_Control(2, 1, speed, j, (int)(3200*quan), 0, 0);
	 delay_ms(1);
	
Emm_V5_Pos_Control(3, 0, speed-1, j, (int)(3200*quan), 0, 0);
	 delay_ms(1);
	
Emm_V5_Pos_Control(4, 1, speed-1, j,(int)(3200*quan), 0, 0);
	 delay_ms(1);


}

//i表示是否开启多机同步
void stop(void)  
{	
	Emm_V5_Stop_Now(0,0);
	delay_ms(1);
}

void L_R(int speed , int j)  //左自转
{

Emm_V5_Vel_Control(5, 1, speed-2, j, 0);
	 delay_ms(1);
	
Emm_V5_Vel_Control(2, 1, speed-1, j,  0);
	 delay_ms(1);
	
Emm_V5_Vel_Control(3, 1, speed+1, j,  0);
	 delay_ms(1);
	
Emm_V5_Vel_Control(4, 1, speed+1, j,  0);
	 delay_ms(1);


}


void R_R(int speed , int j) //右自转
{
Emm_V5_Vel_Control(5, 0, speed-2, j, 0);
	 delay_ms(1);
	
Emm_V5_Vel_Control(2, 0, speed-1, j,  0);
	 delay_ms(1);
	
Emm_V5_Vel_Control(3, 0, speed+1, j,  0);
	 delay_ms(1);
	
Emm_V5_Vel_Control(4, 0, speed+1, j,  0);
	 delay_ms(1);

}

void Backward(int speeed)
{
	Emm_V5_Vel_Control(5, 0,speeed, 1, 0);
	 delay_ms(1);
	
Emm_V5_Vel_Control(2, 0, speeed, 1,  0);
	 delay_ms(1);
	
Emm_V5_Vel_Control(3, 1, speeed, 1,  0);
	 delay_ms(1);
	
Emm_V5_Vel_Control(4, 1, speeed, 1,  0);
	 delay_ms(1);
}

void Forward(int speeed)
{	Emm_V5_Vel_Control(5, 1,speeed, 1, 0);
	 delay_ms(1);
	
Emm_V5_Vel_Control(2, 1, speeed, 1,  0);
	 delay_ms(1);
	
Emm_V5_Vel_Control(3, 0, speeed, 1,  0);
	 delay_ms(1);
	
Emm_V5_Vel_Control(4, 0, speeed, 1,  0);
	 delay_ms(1);
}


void R_Translation(int speeed)
{

	Emm_V5_Vel_Control(5, 0,speeed, 1, 0);
	 delay_ms(1);
	
Emm_V5_Vel_Control(2, 1, speeed, 1,  0);
	 delay_ms(1);
	
Emm_V5_Vel_Control(3, 0, speeed, 1,  0);
	 delay_ms(1);
	
Emm_V5_Vel_Control(4, 1, speeed, 1,  0);
	 delay_ms(1);


}

void L_Translation(int speeed)
{

	Emm_V5_Vel_Control(5, 1,speeed, 1, 0);
	 delay_ms(1);
	
Emm_V5_Vel_Control(2, 0, speeed, 1,  0);
	 delay_ms(1);
	
Emm_V5_Vel_Control(3, 1, speeed, 1,  0);
	 delay_ms(1);
	
Emm_V5_Vel_Control(4, 0, speeed, 1,  0);
	 delay_ms(1);


}
#define gg_0_X 180
#define gg_0_Y 134 
#define gg_1_X 153 
#define gg_1_Y 185
#define gg_2_X 151
#define gg_2_Y 146  
//gg=0就是色环，gg=2就是地上色块,gg1就是原料区的色块
void weitiao(int gg)
{
	//原料区的色块
	if(gg==1)
   {
    flag_adjusting=1;
	
	while(flag_adjusting<=2&&flag_adjusting!=0)
    {
		
        if(flag_cx_complete==0&&XX!=0)
        {
            if(XX<gg_1_X-2)
			{
			 Forward(5);
			 while (XX<gg_1_X-2);

			}
		
			else if(XX>gg_1_X+2)
			{
				Backward(5);
			  while(XX>gg_1_X+2);

			}
              
				stop();flag_cx_complete=1;flag_cy_complete=0;flag_cy_start=1;
        }
        if(flag_cy_start==1&&flag_cy_complete==0&&YY!=0)
        {
            if(YY<gg_1_Y-2)
			 {
				R_Translation(5);
				while(YY<gg_1_Y-2);

			}
            else if(YY>gg_1_Y+2) 
			{
				L_Translation(5);
				while(YY>gg_1_Y+2);

			}
				stop();
				flag_cy_complete=1;
				flag_adjusting++;
				flag_cx_complete=0;
				delay_ms(200);
        }
    }
   }
   //色环
   if(gg==0)
   {
	    flag_adjusting=1;
	    while(flag_adjusting<=4&&flag_adjusting!=0)
    {
            printf("XX1:%d YY1:%d\n\r",XX1,YY1);
            if (flag_cx_complete == 0 && XX1 != 0) {
                if (XX1 < gg_0_X - 2) {
                    Forward(5);
                    while (XX1 < gg_0_X - 2);

                }

                else if (XX1 > gg_0_X + 2) {
                    Backward(5);
                    while (XX1 > gg_0_X + 2);
                }
				stop();flag_cx_complete=1;flag_cy_complete=0;flag_cy_start=1;
            }
        if(flag_cy_start==1&&flag_cy_complete==0&&YY1!=0)
        {
            if(YY1<gg_0_Y-2)
			 {
				R_Translation(5);
				while(YY1<gg_0_Y-2);
	
			}
            else if(YY1>gg_0_Y+2) 
			{
				L_Translation(5);
				while(YY1>gg_0_Y+2);

			}
				stop();flag_cy_complete=1;flag_adjusting++;flag_cx_complete=0;
				delay_ms(200);
	    }

    }

   }
//地上的色块
      if(gg==2)
   {
	    flag_adjusting=1;
	    while(flag_adjusting<=4&&flag_adjusting!=0)
    {
        if(flag_cx_complete==0&&XX!=0)
        {
            if(XX<gg_2_X-2)
			{
			 Forward(15);
			  while (XX<gg_2_X-2);

			}
			
			else if(XX>gg_2_X+2)
			{
				Backward(15);
			  while(XX>gg_2_X+2);

			}
				stop();flag_cx_complete=1;flag_cy_complete=0;flag_cy_start=1;
        }
        if(flag_cy_start==1&&flag_cy_complete==0&&YY!=0)
        {
            if(YY<gg_2_Y-2)
			 {
				R_Translation(15);
				while(YY<gg_2_Y-2);
	
			}
            else if(YY>gg_2_Y+2) 
			{
				L_Translation(15);
				while(YY>gg_2_Y+2);

			}
				stop();flag_cy_complete=1;flag_adjusting++;flag_cx_complete=0;
				delay_ms(200);
	    }

    }

   }

}
void weitiao_2(void)
{
	char KKK;
	int x;
	x=XX;

	while (1)
	{
	delay_ms(300);

	if(abs(XX-x)<=20)
	break;
	x=XX;
	}
		KKK=K;x=XX;
	while(1)
	{
	delay_ms(300);

	if(abs(XX-x)<=20&&KKK!=K)
	break;
	x=XX;

	}
	weitiao(2);
}
void se_huan(uint16_t num)
{

 if(num==1)
 {move_forward_pos(100,10,0);delay_ms(2000);}
 if(num==2)
 {move_forward_pos(100,10,0.6);delay_ms(2000);}
 if(num==3)
 {move_forward_pos(100,10,1.2);delay_ms(2000);}
    
    
}




void huan(void)
{
	 if(task[0]==123)
 {
 	move_forward(100,10,2.92);
 	delay_ms(3000);
	 move_right(100,10,0.45);
delay_ms(1500);
 	weitiao(0);
	zp(1);
	delay_ms(1000);
 	put_huan(1);
 	move_forward(100,10,0.6);
 	delay_ms(1500);
 	weitiao(0);
		zp(2);
	delay_ms(1000);
 	put_huan(2);
 	move_forward(100,10,0.6);
 	delay_ms(1500);
 	weitiao(0);
		zp(3);
	delay_ms(1000);
 	put_huan(3);
 	move_backward(100,10,1.2);
 	delay_ms(2000);
		zp(1);
	delay_ms(1000);
 	catch_huan();
	put(1);
 	move_forward(100,10,0.6);
 	delay_ms(1500);
		zp(2);
	delay_ms(1000);
 	catch_huan();
	put(2);
 	move_forward(100,10,0.6);
 	delay_ms(1500);
		zp(3);
	delay_ms(1000);
 	catch_huan();
	put(3);
	move_left(100,10,0.3);
	delay_ms(2000);

 	move_forward(100,10,2.2);
 	delay_ms(3000);
	
 }
 if(task[0]==132)
 {
 	move_forward(100,10,2.92);
 	delay_ms(3000);
	 move_right(100,10,0.45);
delay_ms(1500);
 	weitiao(0);
		zp(1);
	delay_ms(1000);
 	put_huan(1);
 	move_forward(100,10,1.2);
 	delay_ms(2000);
 	weitiao(0);
		zp(3);
	delay_ms(1000);
 	put_huan(3);
 	move_backward(100,10,0.6);
 	delay_ms(1500);
 	weitiao(0);
		zp(2);
	delay_ms(1000);
 	put_huan(2);
 	move_backward(100,10,0.6);
	delay_ms(1500);
		zp(1);
	delay_ms(1000);
 	catch_huan();
	put(1);
 	move_forward(100,10,1.2);
	delay_ms(2000);
		zp(3);
	delay_ms(1000);
 	catch_huan();
	put(3);
 	move_backward(100,10,0.6);
	delay_ms(1500);
		zp(2);
	delay_ms(1000);
 	catch_huan();
	put(2);
move_left(100,10,0.3);
delay_ms(2000);

 	move_forward(100,10,2.8);
 	delay_ms(3000);
	
 }
 if(task[0]==213)
 {
 	move_forward(100,10,3.52);
 	delay_ms(4000);
	 move_right(100,10,0.45);
delay_ms(1500);
 	weitiao(0);	zp(2);
	delay_ms(1000);
 	put_huan(2);
 	move_backward(100,10,0.6);
 	delay_ms(1500);
 	weitiao(0);
		zp(1);
	delay_ms(1000);
 	put_huan(1);
 	move_forward(100,10,1.2);
 	delay_ms(2000);
 	weitiao(0);
		zp(3);
	delay_ms(1000);
 	put_huan(3);
 	move_backward(100,10,0.6);
 	delay_ms(1500);
		zp(2);
	delay_ms(1000);
 	catch_huan();
	put(2);
 	move_backward(100,10,0.6);
 	delay_ms(1500);
		zp(1);
	delay_ms(1000);
 	catch_huan();
	put(1);
 	move_forward(100,10,1.2);
 	delay_ms(2000);
		zp(3);
	delay_ms(1000);
 	catch_huan();
	put(3);
	move_left(100,10,0.3);
delay_ms(2000);

 	move_forward(100,10,2.2);
 	delay_ms(3000);
	
 }
 if(task[0]==231)
 {
 	move_forward(100,10,3.52);
 	delay_ms(4000);
	 move_right(100,10,0.45);
delay_ms(1500);
 	weitiao(0);
		zp(2);
	delay_ms(1000);
 	put_huan(2);
 	move_forward(100,10,0.6);
 	delay_ms(1500);
 	weitiao(0);
		zp(3);
	delay_ms(1000);
 	put_huan(3);
 	move_backward(100,10,1.2);
 	delay_ms(2000);
 	weitiao(0);
		zp(1);
	delay_ms(1000);
 	put_huan(1);
 	move_forward(100,10,0.6);
 	delay_ms(1500);
		zp(2);
	delay_ms(1000);
 	catch_huan();
	put(2);
 	move_forward(100,10,0.6);
 	delay_ms(1500);
		zp(3);
	delay_ms(1000);
 	catch_huan();
	put(3);
 	move_backward(100,10,1.2);
 	delay_ms(2000);
		zp(1);
	delay_ms(1000);
 	catch_huan();
	put(1);
move_left(100,10,0.3);
delay_ms(2000);

 	move_forward(100,10,3.4);
 	delay_ms(4500);
	
 }
 if(task[0]==321)
 {
 	move_forward(100,10,4.3);
 	delay_ms(5000);
	 	 move_right(100,10,0.45);
delay_ms(1500);
 	weitiao(0);
		zp(3);
	delay_ms(1000);
 	put_huan(3);
 	move_backward(100,10,0.6);
 	delay_ms(1500);
	  	weitiao(0);
			zp(2);
	delay_ms(1000);
 	put_huan(2);
 	move_backward(100,10,0.6);
 	delay_ms(1500);
 	weitiao(0);
		zp(1);
	delay_ms(1000);
 	put_huan(1);
 	move_forward(100,10,1.2);
 	delay_ms(2000);
		zp(3);
	delay_ms(1000);
 	catch_huan();
	put(3);
 	move_backward(100,10,0.6);
 	delay_ms(1500);
		zp(2);
	delay_ms(1000);
 	catch_huan();
	put(2);
 	move_backward(100,10,0.6);
 	delay_ms(1500);
		zp(1);
	delay_ms(1000);
 	catch_huan();
	put(1);
move_left(100,10,0.3);
delay_ms(2000);
 	move_forward(100,10,3.4);
 	delay_ms(5000);
	
 }
 if(task[0]==312)
 {
 	move_forward(100,10,4.3);
 	delay_ms(5000);
	 	 	 move_right(100,10,0.45);
delay_ms(1500);
 	weitiao(0);
		zp(3);
	delay_ms(1000);
 	put_huan(3);
 	move_backward(100,10,1.2);
 	delay_ms(2000);
	  	weitiao(0);
			zp(1);
	delay_ms(1000);
 	put_huan(1);
 	move_forward(100,10,0.6);
 	delay_ms(1500);
	  	weitiao(0);
			zp(2);
	delay_ms(1000);
 	put_huan(2);
 	move_forward(100,10,0.6);
 	delay_ms(1500);
	 
	 	zp(3);
	delay_ms(1000);
 	catch_huan();
	put(3);
 	move_backward(100,10,1.2);
 	delay_ms(2000);
		zp(1);
	delay_ms(1000);
 	catch_huan();
	put(1);
 	move_forward(100,10,0.6);
 	delay_ms(1500);
		zp(2);
	delay_ms(1000);
 	catch_huan();
	put(2);
	move_left(100,10,0.3);
delay_ms(2000);
 	move_forward(100,10,2.8);
 	delay_ms(4000);
}


}

void huan1(void)
{
	if(task[1]==123)
 {
 	move_forward(100,10,2.92);
 	delay_ms(3000);
	 move_right(100,10,0.45);
delay_ms(1500);
 	weitiao(0);
		zp(1);
	delay_ms(1000);
 	put_huan(1);
 	move_forward(100,10,0.6);
 	delay_ms(1500);
 	weitiao(0);
		zp(2);
	delay_ms(1000);
	zp(2);
 	put_huan(2);
 	move_forward(100,10,0.6);
 	delay_ms(1500);
 	weitiao(0);
		zp(3);
	delay_ms(1000);
 	put_huan(3);
 	move_backward(100,10,1.2);
 	delay_ms(2000);
	
	zp(1);
	delay_ms(1000);
 	catch_huan();
	put(1);
 	move_forward(100,10,0.6);
 	delay_ms(1500);
	zp(2);
	delay_ms(1000);
 	catch_huan();
	put(2);
 	move_forward(100,10,0.6);
 	delay_ms(1500);
	zp(3);
	delay_ms(1000);
 	catch_huan();
	put(3);
	move_left(100,10,0.3);
	delay_ms(2000);

 	move_forward(100,10,2.2);
 	delay_ms(3000);
	
 }
 if(task[1]==132)
 {
 	move_forward(100,10,2.92);
 	delay_ms(3000);
	 move_right(100,10,0.45);
delay_ms(1500);
 	weitiao(0);
	zp(1);
	delay_ms(1000);
 	put_huan(1);
 	move_forward(100,10,1.2);
 	delay_ms(2000);
 	weitiao(0);
	zp(3);
	delay_ms(1000);
 	put_huan(3);
 	move_backward(100,10,0.6);
 	delay_ms(1500);
 	weitiao(0);
	zp(2);
	delay_ms(1000);
 	put_huan(2);
 	move_backward(100,10,0.6);
	delay_ms(1500);
	zp(1);
	delay_ms(1000);

 	catch_huan();
	put(1);
 	move_forward(100,10,1.2);
	delay_ms(1500);
		zp(3);
	delay_ms(1000);
 	catch_huan();
	put(3);
 	move_backward(100,10,0.6);
	delay_ms(1500);
		zp(2);
	delay_ms(1000);
 	catch_huan();
	put(2);
move_left(100,10,0.3);
delay_ms(2000);

 	move_forward(100,10,2.8);
 	delay_ms(3000);
	
 }
 if(task[1]==213)
 {
 	move_forward(100,10,3.62);
 	delay_ms(4000);
	 move_right(100,10,0.45);
delay_ms(1500);
 	weitiao(0);
		zp(2);
	delay_ms(1000);
 	put_huan(2);
 	move_backward(100,10,0.6);
 	delay_ms(1500);
 	weitiao(0);
		zp(1);
	delay_ms(1000);
 	put_huan(1);
 	move_forward(100,10,1.2);
 	delay_ms(2000);
 	weitiao(0);
		zp(3);
	delay_ms(1000);
 	put_huan(3);
 	move_backward(100,10,0.6);
 	delay_ms(1500);
		zp(2);
	delay_ms(1000);
 	catch_huan();
	put(2);
 	move_backward(100,10,0.6);
 	delay_ms(1500);
		zp(1);
	delay_ms(1000);
 	catch_huan();
	put(1);
 	move_forward(100,10,1.2);
 	delay_ms(2000);
		zp(3);
	delay_ms(1000);
 	catch_huan();
	put(3);
	move_left(100,10,0.3);
delay_ms(2000);

 	move_forward(100,10,2.2);
 	delay_ms(3000);
	
 }
 if(task[1]==231)
 {
 	move_forward(100,10,3.62);
 	delay_ms(4000);
	 move_right(100,10,0.45);
delay_ms(1500);
 	weitiao(0);
		zp(2);
	delay_ms(1000);
 	put_huan(2);
 	move_forward(100,10,0.6);
 	delay_ms(1500);
 	weitiao(0);
		zp(3);
	delay_ms(1000);
 	put_huan(3);
 	move_backward(100,10,1.2);
 	delay_ms(2000);
 	weitiao(0);
		zp(1);
	delay_ms(1000);
 	put_huan(1);
 	move_forward(100,10,0.6);
 	delay_ms(1500);
		zp(2);
	delay_ms(1000);
 	catch_huan();
	put(2);
 	move_forward(100,10,0.6);
 	delay_ms(1500);
		zp(3);
	delay_ms(1000);
 	catch_huan();
	put(3);
 	move_backward(100,10,1.2);
 	delay_ms(2000);
		zp(1);
	delay_ms(1000);
 	catch_huan();
	put(1);
move_left(100,10,0.3);
delay_ms(2000);

 	move_forward(100,10,3.4);
 	delay_ms(4500);
	
 }
 if(task[1]==321)
 {
 	move_forward(100,10,4.32);
 	delay_ms(5000);
	 	 move_right(100,10,0.45);
delay_ms(1500);
 	weitiao(0);
		zp(3);
	delay_ms(1000);
 	put_huan(3);
 	move_backward(100,10,0.6);
 	delay_ms(1500);
	  	weitiao(0);
			zp(2);
	delay_ms(1000);
 	put_huan(2);
 	move_backward(100,10,0.6);
 	delay_ms(1500);
 	weitiao(0);
		zp(1);
	delay_ms(1000);
 	put_huan(1);
 	move_forward(100,10,1.2);
 	delay_ms(2000);
		zp(3);
	delay_ms(1000);
 	catch_huan();
	put(3);
 	move_backward(100,10,0.6);
 	delay_ms(1500);
		zp(2);
	delay_ms(1000);
 	catch_huan();
	put(2);
 	move_backward(100,10,0.6);
 	delay_ms(1500);
		zp(1);
	delay_ms(1000);
 	catch_huan();
	put(1);
move_left(100,10,0.3);
delay_ms(2000);
 	move_forward(100,10,3.4);
 	delay_ms(5000);
	
 }
 if(task[1]==312)
 {
 	move_forward(100,10,4.32);
 	delay_ms(5000);
	 	 	 move_right(100,10,0.45);
delay_ms(1500);
 	weitiao(0);
		zp(3);
	delay_ms(1000);
 	put_huan(3);
 	move_backward(100,10,1.2);
 	delay_ms(2000);
	  	weitiao(0);
			zp(1);
	delay_ms(1000);
 	put_huan(1);
 	move_forward(100,10,0.6);
 	delay_ms(1500);
	  	weitiao(0);
			zp(2);
	delay_ms(1000);
 	put_huan(2);
 	move_forward(100,10,0.6);
 	delay_ms(1500);
	 	zp(3);
	delay_ms(1000);
 	catch_huan();
	put(3);
 	move_backward(100,10,1.2);
 	delay_ms(2000);
		zp(1);
	delay_ms(1000);
 	catch_huan();
	put(1);
 	move_forward(100,10,0.6);
 	delay_ms(1500);
		zp(2);
	delay_ms(1000);
 	catch_huan();
	put(2);
	move_left(100,10,0.3);
delay_ms(2000);
 	move_forward(100,10,2.8);
 	delay_ms(4000);
}
}