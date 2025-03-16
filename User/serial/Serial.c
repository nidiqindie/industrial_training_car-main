#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "bsp_usart.h"
#include "bsp_led.h"
#include "Serial.h"

char Serial_RxPacket[12];//"@MSG\r\n"
int gg=0;
uint8_t Serial_RxFlag;
uint8_t K,X1, X2, X3, Y1, Y2, Y3, S1, S2, S3, S4, S5;
int ring_color;//圆环颜色  
uint16_t DistanceX,DistanceY,Size,XX,YY,XX1,YY1;
extern uint8_t KK='c';
extern int flag_color;
void UART5_init(void)
{
	USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);	//ʹ��USART3��GPIOBʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//ʹ��USART3��GPIOBʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	//ʹ��USART3��GPIOBʱ��

    USART_DeInit(UART5);  //��λ����3
    //USART3_TX   PB10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ��Pb10
    //USART3_RX	  PB11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
    GPIO_Init(GPIOD, &GPIO_InitStructure);  //��ʼ��PB11
 
    //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
 
    //USART ��ʼ������
    USART_InitStructure.USART_BaudRate = 115200;//һ������Ϊ115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(UART5, &USART_InitStructure); //��ʼ������
 
    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//����USART3�����ж�
    USART_Cmd(UART5, ENABLE);                    //ʹ�ܴ���
}


void UART5_IRQHandler(void)
{
    static uint8_t RxState = 0;        // 接收状态机
    static uint8_t pRxPacket = 0;      // 接收数据包索引

    // 检查是否接收到数据
    if (USART_GetITStatus(UART5, USART_IT_RXNE) == SET)
    {
        LED_BLUE;
        uint8_t RxData = USART_ReceiveData(UART5);  // 读取接收到的数据


        // 接收状态为0，等待数据包的开始标识'@'
        if (RxState == 0)
        {
            if (RxData == '@' && Serial_RxFlag == 0)
            {
                RxState = 1;   // 进入状态1，表示开始接收数据
                pRxPacket = 0; // 数据包索引重置
				LED_RED;

            }
        }
        // 接收状态为1，接收数据包内容
        else if (RxState == 1)
        {
            if (RxData == '@' && pRxPacket == 12)
            {
                RxState = 2;   // 进入状态2，数据接收完成
				LED_GREEN;

            }
            else
            {
                Serial_RxPacket[pRxPacket] = RxData;
                pRxPacket++;
				LED_WHITE;
                if (pRxPacket == 12)
                {
                    // 处理完成时的操作，例如显示 OLED
                }
            }
        }
        else if (RxState == 2)
        {
            if (RxData == '@')
            {
                K = Serial_RxPacket[0];
                X1 = Serial_RxPacket[1];
                X2 = Serial_RxPacket[2];
                X3 = Serial_RxPacket[3];
                Y1 = Serial_RxPacket[4];
                Y2 = Serial_RxPacket[5];
                Y3 = Serial_RxPacket[6];
                S1 = Serial_RxPacket[7];
                S2 = Serial_RxPacket[8];
                S3 = Serial_RxPacket[9];
                S4 = Serial_RxPacket[10];
                S5 = Serial_RxPacket[11];
			LED_CYAN;
                DistanceX = (X1 - 48) * 100 + (X2 - 48) * 10 + (X3 - 48);
                DistanceY = (Y1 - 48) * 100 + (Y2 - 48) * 10 + (Y3 - 48);
                //Size = (S1 - 48) * 10000 + (S2 - 48) * 1000 + (S3 - 48) * 100 + (S4 - 48) * 10 + (S5 - 48);


                RxState = 0;
                Serial_RxFlag = 0;
            }
        }

        if (K == 'R' || K == 'G' || K == 'B')
        {
            XX = DistanceX;
            YY = DistanceY;

        }
        if (K == 'C'&&(flag_color==0||(S1-48)==flag_color))//判断是否为目标颜色，为目标颜色才会覆盖坐标
        {
            XX1 = DistanceX;
            YY1 = DistanceY;
        }

        USART_ClearITPendingBit(UART5, USART_IT_RXNE);

    }
	//printf("%c", Y3);
    LED_RGBOFF;
}
