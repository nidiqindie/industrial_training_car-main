#include "HMI.h"
#include "stm32f10x.h"
#include "math.h"
#include "stdio.h"
void HMISends(char *buf1)		  //字符串发送函数
{
	u8 i=0;
	while(1)
	{
	 if(buf1[i]!=0)
	 	{
			USART_SendData(UART4,buf1[i]);  //发送一个字节
			while(USART_GetFlagStatus(UART4,USART_FLAG_TXE)==RESET){};//等待发送结束
		 	i++;
		}
	 else 
	 return ;

		}
	}
void HMISendb(int k)		         //字节发送函数
{		 
	u8 i;
	 for(i=0;i<3;i++)
	 {
	 if(k!=0)
	 	{
			USART_SendData(UART4,k);  //发送一个字节
			while(USART_GetFlagStatus(UART4,USART_FLAG_TXE)==RESET){};//等待发送结束
		}
	 else 
	 return ;

	 } 
}

