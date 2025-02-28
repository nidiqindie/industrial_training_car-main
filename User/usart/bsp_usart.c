#include "bsp_usart.h"
#include "bsp_led.h"
#include "app.h"
 #include <stdbool.h>
#include "bsp_motor.h"
#include "OLED.h"
//��ǰ�ض���Ϊ������
__IO bool rxFrameFlag = false;
__IO uint8_t rxCmd[FIFO_SIZE] = {0};
__IO uint8_t rxCount = 0;

float yaw;

static void NVIC_USART1_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /*     USART? ? ? */
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART1_IRQ;
  /*        ? */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /*      ?  */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* '   ?  */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /*   '      NVIC */
  NVIC_Init(&NVIC_InitStructure);
}

static void NVIC_USART2_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /*     USART? ? ? */
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART2_IRQ;
  /*        ? */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /*      ?  */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  /* '   ?  */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /*   '      NVIC */
  NVIC_Init(&NVIC_InitStructure);
}

static void NVIC_USART3_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /*     USART? ? ? */
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART3_IRQ;
  /*        ? */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /*      ?  */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  /* '   ?  */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /*   '      NVIC */
  NVIC_Init(&NVIC_InitStructure);
}
static void NVIC_UART5_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /*     USART? ? ? */
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_UART5_IRQ;
  /*        ? */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  /*      ?  */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  /* '   ?  */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /*   '      NVIC */
  NVIC_Init(&NVIC_InitStructure);
}

static void NVIC_UART4_Configuration()
{
	  NVIC_InitTypeDef NVIC_InitStructure;
 
 /*     USART? ? ? */
 NVIC_InitStructure.NVIC_IRQChannel = DEBUG_UART4_IRQ;
 /*        ? */
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
 /*      ?  */
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
 /* '   ?  */
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 /*   '      NVIC */
 NVIC_Init(&NVIC_InitStructure);

}
 /*
  * @brief  USART GPIO     ,            
  * @param    
  * @retval   
  */
 void USART1_Config(void)
 {
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

 	GPIO_InitTypeDef GPIO_InitStructure;
 	USART_InitTypeDef USART_InitStructure;

 	//  ??   GPIO  ?  
 	DEBUG_USART1_GPIO_APBxClkCmd(DEBUG_USART1_GPIO_CLK, ENABLE);
	
 	//  ??        ?  ?
 	DEBUG_USART1_APBxClkCmd(DEBUG_USART1_CLK, ENABLE);

 	//   USART Tx  GPIO    ?   ?  g?
 	GPIO_InitStructure.GPIO_Pin = DEBUG_USART1_TX_GPIO_PIN;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(DEBUG_USART1_TX_GPIO_PORT, &GPIO_InitStructure);

   //   USART Rx  GPIO    ?        g?
 	GPIO_InitStructure.GPIO_Pin = DEBUG_USART1_RX_GPIO_PIN;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 	GPIO_Init(DEBUG_USART1_RX_GPIO_PORT, &GPIO_InitStructure);
	
 	//    �?  ?L       
 	//    �?     
 	USART_InitStructure.USART_BaudRate = DEBUG_USART1_BAUDRATE;
 	//             ? 
 	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
 	//     ???
 	USART_InitStructure.USART_StopBits = USART_StopBits_1;
 	//     ?  ?
 	USART_InitStructure.USART_Parity = USART_Parity_No ;
 	//     ?        
 	USART_InitStructure.USART_HardwareFlowControl = 
 	USART_HardwareFlowControl_None;
 	//    �?   g?   ? h  
 	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
 	//   ?  ?? '      ?
 	USART_Init(DEBUG_USART1, &USART_InitStructure);
	
 #ifdef Enable_IRQ_USERT1_RX
 	{//      ?    ?     
 	NVIC_USART1_Configuration();
 	// ' ?  ?    ? 
 	USART_ITConfig(DEBUG_USART1, USART_IT_RXNE, ENABLE);	
 	}
 	#endif
 	// ' ?   
 	USART_Cmd(DEBUG_USART1, ENABLE);	    
 }



void USART2_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	//  ??   GPIO  ?  
	DEBUG_USART2_GPIO_APBxClkCmd(DEBUG_USART2_GPIO_CLK, ENABLE);
	
	//  ??        ?  ?
	DEBUG_USART2_APBxClkCmd(DEBUG_USART2_CLK, ENABLE);

	//   USART Tx  GPIO    ?   ?  g?
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART2_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART2_TX_GPIO_PORT, &GPIO_InitStructure);

  //   USART Rx  GPIO    ?        g?
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART2_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART2_RX_GPIO_PORT, &GPIO_InitStructure);
	
	//    �?  ?L       
	//    �?     
	USART_InitStructure.USART_BaudRate = DEBUG_USART2_BAUDRATE;
	//             ? 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	//     ???
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	//     ?  ?
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	//     ?        
	USART_InitStructure.USART_HardwareFlowControl = 
	USART_HardwareFlowControl_None;
	//    �?   g?   ? h  
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//   ?  ?? '      ?
	USART_Init(DEBUG_USART2, &USART_InitStructure);
	
#ifdef Enable_IRQ_USERT2_RX
	{//      ?    ?     
	NVIC_USART2_Configuration();
	// ' ?  ?    ? 
	USART_ITConfig(DEBUG_USART2, USART_IT_RXNE, ENABLE);	
	}
	#endif
	// ' ?   
	NVIC_USART2_Configuration();
	USART_Cmd(DEBUG_USART2, ENABLE);	    
}



// USART3初始化配置函数
void USART3_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /*------------------------ GPIO配置 ------------------------*/
    // 使能USART3相关GPIO时钟
    DEBUG_USART3_GPIO_APBxClkCmd(DEBUG_USART3_GPIO_CLK, ENABLE);
    
    // 使能USART3模块时钟
    DEBUG_USART3_APBxClkCmd(DEBUG_USART3_CLK, ENABLE);

    /* 配置TX引脚（复用推挽输出模式） */
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART3_TX_GPIO_PIN;  // 选择TX引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;          // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        // 50MHz速率
    GPIO_Init(DEBUG_USART3_TX_GPIO_PORT, &GPIO_InitStructure);

    /* 配置RX引脚（浮空输入模式） */
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART3_RX_GPIO_PIN;  // 选择RX引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    // 浮空输入模式
    GPIO_Init(DEBUG_USART3_RX_GPIO_PORT, &GPIO_InitStructure);

    /*------------------------ USART参数配置 ------------------------*/
    USART_InitStructure.USART_BaudRate = DEBUG_USART3_BAUDRATE;  // 设置波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  // 8位数据位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;       // 1位停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;          // 无校验位
    USART_InitStructure.USART_HardwareFlowControl = 
        USART_HardwareFlowControl_None;                          // 无硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 使能收发模式

    // 应用USART3配置
    USART_Init(DEBUG_USART3, &USART_InitStructure);

    /*------------------------ 中断配置 ------------------------*/
#ifdef Enable_IRQ_USERT3_RX
    {
        // 配置USART3中断优先级
        NVIC_USART3_Configuration();
        // 使能接收中断（RXNE中断）
        USART_ITConfig(DEBUG_USART3, USART_IT_RXNE, ENABLE);
    }
#endif

    // 使能USART3模块
    USART_Cmd(DEBUG_USART3, ENABLE);
}

void UART4_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // 使能GPIO时钟
    DEBUG_UART4_GPIO_APBxClkCmd(DEBUG_UART4_GPIO_CLK, ENABLE);
    
    // 使能USART时钟
    DEBUG_UART4_APBxClkCmd(DEBUG_UART4_CLK, ENABLE);

    // 配置USART Tx GPIO为复用推挽输出模式
    GPIO_InitStructure.GPIO_Pin = DEBUG_UART4_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEBUG_UART4_TX_GPIO_PORT, &GPIO_InitStructure);

    // 配置USART Rx GPIO为浮空输入模式
    GPIO_InitStructure.GPIO_Pin = DEBUG_UART4_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(DEBUG_UART4_RX_GPIO_PORT, &GPIO_InitStructure);
    
    // 配置USART参数
    USART_InitStructure.USART_BaudRate = DEBUG_UART4_BAUDRATE;  // 波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  // 数据位设置为8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  // 停止位设置为1
    USART_InitStructure.USART_Parity = USART_Parity_No;  // 无校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // 无硬件流控
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  // 配置为收发模式

    // 初始化USART
    USART_Init(DEBUG_UART4, &USART_InitStructure);
    
#ifdef Enable_IRQ_UART4_RX
    // 如果使能接收中断，配置NVIC并使能USART接收中断
    {
        NVIC_UART4_Configuration();
        USART_ITConfig(DEBUG_UART4, USART_IT_RXNE, ENABLE);    
    }
#endif

    // 使能USART
    USART_Cmd(DEBUG_UART4, ENABLE);	    
}



void UART5_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // 使能GPIO时钟
    DEBUG_UART5_GPIO_APBxClkCmd(DEBUG_UART5_GPIO_CLK1, ENABLE);
    DEBUG_UART5_GPIO_APBxClkCmd(DEBUG_UART5_GPIO_CLK2, ENABLE);

    // 使能USART时钟
    DEBUG_UART5_APBxClkCmd(DEBUG_UART5_CLK, ENABLE);

    // 配置USART Tx GPIO为复用推挽输出模式
    GPIO_InitStructure.GPIO_Pin = DEBUG_UART5_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEBUG_UART5_TX_GPIO_PORT, &GPIO_InitStructure);

    // 配置USART Rx GPIO为浮空输入模式
    GPIO_InitStructure.GPIO_Pin = DEBUG_UART5_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(DEBUG_UART5_RX_GPIO_PORT, &GPIO_InitStructure);
    
    // 配置USART参数
    USART_InitStructure.USART_BaudRate = DEBUG_UART5_BAUDRATE;  // 波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  // 数据位设置为8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  // 停止位设置为1
    USART_InitStructure.USART_Parity = USART_Parity_No;  // 无校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // 无硬件流控
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  // 配置为收发模式

    // 初始化USART
    USART_Init(DEBUG_UART5, &USART_InitStructure);
    
    // 如果使能接收中断，配置NVIC并使能USART接收中断
#ifdef Enable_IRQ_UART5_RX
    {
        NVIC_UART5_Configuration();
        USART_ITConfig(DEBUG_UART5, USART_IT_RXNE, ENABLE);    
    }
#endif

    // 使能USART
    USART_Cmd(DEBUG_UART5, ENABLE);	    
}



/*****************      发送一个字节到USART **********************/
void Usart_SendByte(USART_TypeDef * pUSARTx, uint8_t ch)
{
    /* 发送一个字节到USART */
    USART_SendData(pUSARTx, ch);
    
    /* 等待直到数据寄存器为空，表示数据已发送完成 */
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);  
}

/******************     发送字节数组到USART ************************/
void Usart_SendArray(USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
    uint8_t i;
    
    for(i = 0; i < num; i++)
    {
        /* 发送数组中的每一个字节到USART */
        Usart_SendByte(pUSARTx, array[i]);  
    }
    
    /* 等待直到传输完成 */
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET);
}

/*****************       发送字符串到USART **********************/
void Usart_SendString(USART_TypeDef * pUSARTx, char *str)
{
    unsigned int k = 0;
    do 
    {
        /* 发送字符串中的每一个字符到USART */
        Usart_SendByte(pUSARTx, *(str + k));
        k++;
    } while(*(str + k) != '\0');  /* 直到遇到字符串结束符'\0' */
    
    /* 等待直到所有数据传输完成 */
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET)
    {}
}


/*****************      发送一个16位的数据 **********************/
void Usart_SendHalfWord(USART_TypeDef * pUSARTx, uint16_t ch)
{
    uint8_t temp_h, temp_l;
    
    /* 提取高字节 */
    temp_h = (ch & 0XFF00) >> 8;
    /* 提取低字节 */
    temp_l = ch & 0XFF;
    
    /* 发送高字节 */
    USART_SendData(pUSARTx, temp_h);  
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);

    /* 发送低字节 */
    USART_SendData(pUSARTx, temp_l);  
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);  
}


/// 重定向c库的printf函数到USART，输出字符到串口
int fputc(int ch, FILE *f)
{
    /* 发送字符到USART */
    USART_SendData(DEBUG_USART, (uint8_t) ch);
    
    /* 等待直到数据寄存器为空，表示数据已发送完成 */
    while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);    
    
    return (ch);
}

/// 重定向c库的scanf函数，通过USART接收字符
int fgetc(FILE *f)
{
    /* 等待直到接收到数据 */
    while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_RXNE) == RESET);

    /* 返回接收到的数据 */
    return (int)USART_ReceiveData(DEBUG_USART);
}


void USART1_IRQHandler(void)
{
    __IO uint16_t i = 0;

    /**********************************************************
    *** 处理接收中断（RXNE）——接收到数据
    **********************************************************/
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {


        // 清除接收中断标志
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}




extern char Serial_RxPacket[12];
extern uint8_t Serial_RxFlag;
extern uint8_t K,X1, X2, X3, Y1, Y2, Y3, S1, S2, S3, S4, S5;  
extern uint16_t DistanceX,DistanceY,Size,XX,YY,XX1,YY1;
extern uint8_t KK;  
//串口三的中断
uint8_t Rxflag_0 = 0;
uint8_t Rxflag_1 = 0;
uint8_t Rxflag_2 = 0;
float a          = 0;
void DEBUG_USART3_IRQHandler(void)
{    
    static uint8_t RxState = 0;        // 接收状态机
    static uint8_t pRxPacket = 0;      // 接收数据包索引

    // 检查是否接收到数据
    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {    
        uint8_t RxData = USART_ReceiveData(USART3);  // 读取接收到的数据
        
        // 接收状态为0，等待数据包的开始标识'@'
        if (RxState == '0')
        {
            Rxflag_0 = 1;
        }
        // 接收状态为1，接收数据包内容
        if (RxState == '1') {
Rxflag_1 = 1;
        
        }
        // 接收状态为2，处理数据包内容
        if (RxState == '2') {
            Rxflag_2 = 1;
        }

        // 根据K值进行条件处理
        if (K == 'R' || K == 'G' || K == 'B')
        {
            XX = DistanceX;  // 设置XX为DistanceX
            YY = DistanceY;  // 设置YY为DistanceY
        }
        if (K == 'C')
        {
            XX1 = DistanceX;  // 设置XX1为DistanceX
            YY1 = DistanceY;  // 设置YY1为DistanceY
        }

        // 清除接收中断标志
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);    
    }
}

// void DEBUG_UART4_IRQHandler(void)
// {
	
//   uint8_t ucTemp;
// 	if(USART_GetITStatus(DEBUG_UART4,USART_IT_RXNE)!=RESET)
// 	{		
		
// 		ucTemp = USART_ReceiveData(DEBUG_UART4);
//     USART_SendData(DEBUG_UART4,ucTemp);    

// 	}	 
// }

//串口中断服务函数


// void DEBUG_UART5_IRQHandler(void)
// {

// 	if(USART_GetFlagStatus(DEBUG_UART5,USART_IT_RXNE)!=RESET)
// 	{
// 		uint16_t ucTep;
// 		ucTep = USART_ReceiveData(DEBUG_UART5);
// 		JY61P_SerialData((unsigned char)ucTep);
// 	}

// }	 

void usart_SendCmd(__IO uint8_t *cmd, uint8_t len)
{
    __IO uint8_t i = 0;
    
    // 遍历命令数组，逐字节发送命令
    for (i = 0; i < len; i++) 
    {
        usart_SendByte(cmd[i]);  // 调用usart_SendByte发送单个字节
    }
}

/**
 * @brief   通过USART发送一个字节
 * @param   data: 要发送的数据字节
 * @retval  无返回值
 */
void usart_SendByte(uint16_t data)
{
    __IO uint16_t t0 = 0;  // 定时器，用于避免死循环
    
    // 将数据发送到USART数据寄存器，限制为9位数据（0-511）
    USART1->DR = (data & (uint16_t)0x01FF);

    // 等待直到数据寄存器空，准备发送下一个字节
    while (!(USART1->SR & USART_FLAG_TXE))
    {
        ++t0;
        if (t0 > 8000)  // 如果等待时间过长（防止死循环），退出
        {
            return;
        }
    }
}





