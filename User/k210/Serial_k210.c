#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "bsp_usart.h"
#include "bsp_led.h"
#include "Serial_k210.h"

char Serial_RxPacket1[12];//"@MSG\r\n"
uint8_t Serial_RxFlag1;
uint8_t K1,X11, X21, X31, Y11, Y21, Y31, S11, S21, S31, S41, S51;  
uint16_t DistanceX1,DistanceY1,Size1;
int task[2]={123,123};  

void UART4_init(void)
{
	USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	//ʹ��USART3��GPIOBʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//ʹ��USART3��GPIOBʱ��
    USART_DeInit(UART4);  //��λ����3
    //USART3_TX   PB10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ��Pb10
    //USART3_RX	  PB11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
    GPIO_Init(GPIOC, &GPIO_InitStructure);  //��ʼ��PB11
 
    //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
 
    //USART ��ʼ������
    USART_InitStructure.USART_BaudRate = 9600;//һ������Ϊ115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(UART4, &USART_InitStructure); //��ʼ������
 
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//����USART3�����ж�
    USART_Cmd(UART4, ENABLE);                    //ʹ�ܴ���
}


void UART4_IRQHandler(void)
{
	//uint16_t i=0;
	static uint8_t RxState1 = 0;
	static uint8_t pRxPacket1 = 0;	

	if (USART_GetITStatus(UART4, USART_IT_RXNE) == SET)
	{	

		LED_RED;
		uint8_t RxData1 = USART_ReceiveData(UART4);
		
		if (RxState1 == 0)
		{
			if (RxData1 == '@' && Serial_RxFlag1 == 0)
			{	
//				OLED_ShowNum(1, 9,01,2);
				
				RxState1 = 1;
				pRxPacket1 = 0;
			}
		}
		else if (RxState1 == 1)
		{
			//OLED_ShowNum(2,16,9,1);				
			if (RxData1 == '@'&& pRxPacket1==12)
			{
				RxState1 = 2;
			}
			else
			{
				Serial_RxPacket1[pRxPacket1] = RxData1;
				pRxPacket1 ++;//��??������???��???������pRxPacket=12
//				OLED_ShowNum(2,2,Serial_RxPacket[0],1);
				if(pRxPacket1==12)
				{
//			OLED_ShowNum(1,16,9,1);				
				}
			}
		}
		else if (RxState1 == 2)
		{
			//OLED_ShowNum(1,13,02,2);
			
			if (RxData1 == '@')
			{ 
				K1 =Serial_RxPacket1[0];//��?
				X11=Serial_RxPacket1[1];
				X21=Serial_RxPacket1[2];
				X31=Serial_RxPacket1[3];//?��3?
				Y11=Serial_RxPacket1[4];
				Y21=Serial_RxPacket1[5];
				Y31=Serial_RxPacket1[6];
				S11=Serial_RxPacket1[7];
				S21=Serial_RxPacket1[8];
				S31=Serial_RxPacket1[9];
        		S41=Serial_RxPacket1[10];
        		S51=Serial_RxPacket1[11];				//����5?
//				Serial_RxPacket[pRxPacket] = '\0';
				// printf("%c",X1);
				// printf("    Y%c      ",Y1);
				DistanceX1=(X11-48)*100+(X21-48)*10+(X31-48);
				DistanceY1=(Y11-48)*100+(Y21-48)*10+(Y31-48);
				Size1=(S11-48)*10000+(S21-48)*1000+(S31-48)*100+(S41-48)*10+(S51-48);
				RxState1 = 0;
				//printf("%d",DistanceX);
				//printf("%d",DistanceY);

				//printf("%c",K);
//				for(i=0;i<11;i++)
//					{
//						Serial_RxPacket[i]=0x00;  
//					}
						Serial_RxFlag1 = 0;
			}
		}
		if(K1=='S')
		{
			task[0]=(X11-48)*100+(X21-48)*10+(X31-48);
			task[1]=(Y11-48)*100+(Y21-48)*10+(Y31-48);
		}
		USART_ClearITPendingBit(UART4, USART_IT_RXNE);
	}
}

