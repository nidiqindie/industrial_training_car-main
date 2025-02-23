#ifndef __COMMUNICATION_PROTOCOL_H
#define	__COMMUNICATION_PROTOCOL_H
#include "stm32f10x.h"
#include <stdio.h>

/*对飞机主控的控制指令*/
#define 	fc_starttask 	0x01	 //开始任务
#define 	fc_reset 		0x02 	 //c重启系统
#define 	fc_close 		0x03 	 //关闭系统
/*end*/

/*对树莓派的控制指令*/
#define 	nano_recognize_reset 	0x01
#define 	nano_close 				0x02
#define 	nano_recognize_yellow 	0x03
#define 	nano_recognize_red   	0x04
/*end*/


/*飞机数据定义*/
typedef struct {
	uint8_t raspi_state; 
	uint8_t orgpi_state;
	uint8_t flow_state;
	uint8_t uwb_state;
	uint8_t sys_state;
	uint8_t flag;           //发现火情的标志位，如果发现火情置1，否则为0
	int32_t new_x_pos;
	int32_t new_y_pos;
	int32_t old_x_pos;
	int32_t old_y_pos;
	int32_t distance;
	int32_t x_distance;	
	int32_t y_distance;	
}FC_DATA;
/*end*/

/*nano数据定义*/
typedef struct {
	int16_t obj_x;
	int16_t obj_y;
}NANO_DATA;
/*end*/

/*串口屏数据定义*/
typedef struct {
	uint8_t flag_task;
	uint8_t flag_nano_contr;
	uint8_t flag_orapi_contr;
	uint8_t flag_sys_contr;
	uint8_t num_waypoints;
	int16_t waypoints_x[7];
	int16_t waypoints_y[7];
}SCREEN_DATA;
/*end*/

/*nano数据定义*/
typedef struct {
	float yaw;
	int32_t pos_x;
	int32_t pos_y;
}CAR;
/*end*/

extern	FC_DATA fc_data;

extern	NANO_DATA nano_data;

extern	CAR car;

void fc_fly(uint8_t code);
void FC_RecvArray_Process(void);

void send_command_nano(uint8_t code);
void Nano_RecvArray_Process(void);

void Yaw_RecvArray_Process(void);


void Print_flydata(void);
#endif /* __COMMUNICATION_PROTOCOL_H */
