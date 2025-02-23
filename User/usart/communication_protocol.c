#include "bsp_usart.h"
#include "communication_protocol.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "math.h"

FC_DATA fc_data;

NANO_DATA nano_data;

CAR car;

uint8_t command_fc[5] = {0xAA,0xFF,0x00,0x00,0x00};

uint8_t command_nano[5] = {0xAA,0xFF,0x00,0x00,0x00};


//发送启动命令至主控
void fc_fly(uint8_t code)
{
	uint8_t sumcheck = 0;
	uint8_t addcheck = 0;
	uint8_t i = 0;
	command_fc[2] = code;  //0x01是启动命令，0x02是关闭系统
	for(i = 0;i < 3;i++)
	{
		sumcheck += command_fc[i];
		addcheck += sumcheck;
	}
	command_fc[3] = sumcheck; 
	command_fc[4] = addcheck; 
	Usart_SendArray(DEBUG_USART1, command_fc, fc_send_buff_size);
}

//处理来自飞机主控的数据
void FC_RecvArray_Process(void)
{
	uint8_t sumcheck = 0;
	uint8_t addcheck = 0;
	uint8_t i = 0;

	for(i = 0;i < 16;i++)
	{
		sumcheck += fc_recv_buff[i];
		addcheck += sumcheck;
	}	
	// if((sumcheck == fc_recv_buff[fc_recv_buff[16]])&&(addcheck == fc_recv_buff[fc_recv_buff[17]]))   /此处去掉注释会有问题
	// {
	// 		LED_RED;
			fc_data.raspi_state = fc_recv_buff[2];
			fc_data.orgpi_state = fc_recv_buff[3];
			fc_data.flow_state = fc_recv_buff[4];
			fc_data.uwb_state = fc_recv_buff[5];
			fc_data.sys_state = fc_recv_buff[6];
			fc_data.flag = fc_recv_buff[7];
			fc_data.new_x_pos = (int32_t)(fc_recv_buff[8] + (fc_recv_buff[9]<<8) + (fc_recv_buff[10]<<16) + (fc_recv_buff[11]<<24));
			fc_data.new_y_pos = (int32_t)(fc_recv_buff[12] + (fc_recv_buff[13]<<8) + (fc_recv_buff[14]<<16) + (fc_recv_buff[15]<<24));
			fc_data.new_x_pos+=40;
			fc_data.new_y_pos+=40;
			// printf("system_state:%d\n\r",fc_data.flag);				
			// printf("flag:%d\n\r",fc_data.flag);		
			// printf("x_pos:%d\n\r",fc_data.new_x_pos);
			// printf("y_pos:%d\n\r",fc_data.new_y_pos);
			if(fc_data.raspi_state == 0x01)   	printf("page0.t0.bco=2016\xff\xff\xff");
			else printf("page0.t0.bco=63488\xff\xff\xff");
			if(fc_data.orgpi_state == 0x01)   	printf("page0.t1.bco=2016\xff\xff\xff");
			else printf("page0.t1.bco=63488\xff\xff\xff");
			if(fc_data.flow_state == 0x01)   	printf("page0.t2.bco=2016\xff\xff\xff");
			else printf("page0.t2.bco=63488\xff\xff\xff");
			if(fc_data.uwb_state == 0x01)   	printf("page0.t3.bco=2016\xff\xff\xff");
			else printf("page0.t3.bco=63488\xff\xff\xff");
			if(fc_data.sys_state == 0x01)   	printf("page0.t4.bco=2016\xff\xff\xff");
			else printf("page0.t4.bco=63488\xff\xff\xff");

			printf("page0.x0.val=%d\xff\xff\xff", fc_data.new_x_pos);
			printf("page0.x1.val=%d\xff\xff\xff", fc_data.new_y_pos);
			//计算两坐标点的距离
			fc_data.x_distance = fc_data.new_x_pos - fc_data.old_x_pos;
			fc_data.y_distance = fc_data.new_y_pos - fc_data.old_y_pos;			
			fc_data.distance +=  (uint32_t)sqrt(fc_data.x_distance*fc_data.x_distance+fc_data.y_distance*fc_data.y_distance);

			//打印无人机数据到串口屏
			Print_flydata();

			//更新旧数据坐标
			fc_data.old_x_pos = fc_data.new_x_pos;
			fc_data.old_y_pos = fc_data.new_y_pos;
	// }
}

//串口屏显示无人机数据
void Print_flydata(void)
{
	uint8_t r = 1;
	uint16_t color_dot  = 1055;
	uint16_t color_line  = 1024;
	uint8_t old_x,old_y,new_x,new_y;

	//显示无人机的实时坐标
	printf("page2.x0.val=%d\xff\xff\xff", fc_data.new_x_pos);
	printf("page2.x1.val=%d\xff\xff\xff", fc_data.new_y_pos);

	if(fc_data.flag != 0x00)
		{
			r = 4;
			color_dot = 63488;
			//显示火情点数据
			printf("page2.x2.val=%d\xff\xff\xff", fc_data.new_x_pos);
			printf("page2.x3.val=%d\xff\xff\xff", fc_data.new_y_pos);
		}
	//显示累计航程
	printf("page2.x4.val=%d\xff\xff\xff", fc_data.distance);

	old_x = (uint8_t)((fc_data.old_x_pos)*10/23+10);
	old_y = (uint8_t)(194-(fc_data.old_y_pos)*10/23);
	new_x = (uint8_t)((fc_data.new_x_pos)*10/23+10);
	new_y = (uint8_t)(194-(fc_data.new_y_pos)*10/23);

	//显示更新的航点和航线
 	printf("cirs %d,%d,%d,%d\xff\xff\xff", new_x, new_y, r, color_dot);
 	printf("line %d,%d,%d,%d,%d\xff\xff\xff", old_x, old_y, new_x, new_y, color_line);
}



void send_command_nano(uint8_t code)
{
	uint8_t sumcheck = 0;
	uint8_t addcheck = 0;
	uint8_t i = 0;

	command_nano[2] = code; 
	for(i = 0;i < 3;i++)
	{
		sumcheck += command_nano[i];
		addcheck += sumcheck;
	}
	command_nano[3] = sumcheck; 
	command_nano[4] = addcheck; 

	Usart_SendArray(DEBUG_USART2, command_nano, nano_send_buff_size);
}


void Nano_RecvArray_Process(void)
{
	uint8_t sumcheck = 0;
	uint8_t addcheck = 0;
	uint8_t i = 0;

	for(i = 0;i < 6;i++)
	{
		sumcheck += nano_recv_buff[i];
		addcheck += sumcheck;
	}
	if((sumcheck == nano_recv_buff[6])&&(addcheck == nano_recv_buff[7]))
	{
		nano_data.obj_x = (int16_t)(nano_recv_buff[2] + (nano_recv_buff[3]<<8));
		nano_data.obj_y = (int16_t)(nano_recv_buff[4] + (nano_recv_buff[5]<<8));
	
		// printf("obj_x:%d\n\r",nano_data.obj_x);
		// printf("obj_y:%d\n\r",nano_data.obj_y);	
	
	}	
}

void Yaw_RecvArray_Process(void)
{
	uint8_t sumcheck = 0;
	uint8_t addcheck = 0;
	uint8_t i = 0;

	for(i = 0;i < (yaw_recv_buff[3]+4);i++)
	{
		sumcheck += yaw_recv_buff[i];
		addcheck += sumcheck;
	}	
	if((sumcheck == yaw_recv_buff[yaw_recv_buff[3]+4])&&(addcheck == yaw_recv_buff[yaw_recv_buff[3]+5]))
	{
		if(yaw_recv_buff[2] ==0x03)
		car.yaw = (int16_t)(yaw_recv_buff[8] + (yaw_recv_buff[9]<<8))/100.00;
		// printf("yaw: %f" ,car.yaw);		
		car.yaw = ((int16_t)car.yaw + 360) % 360; 
	// 	printf("yaw: %f" ,car.yaw);
	}
}

