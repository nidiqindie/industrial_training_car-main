// #ifndef __USART_H
// #define __USART_H

// #include "board.h"
// #include "fifo.h"

// /**********************************************************
// ***	Emm_V5.0�����ջ���������
// ***	��д���ߣ�ZHANGDATOU
// ***	����֧�֣��Ŵ�ͷ�ջ��ŷ�
// ***	�Ա����̣�https://zhangdatou.taobao.com
// ***	CSDN���ͣ�http s://blog.csdn.net/zhangdatou666
// ***	qq����Ⱥ��262438510
// **********************************************************/
// #define Enable_IRQ_USERT1_RX   1 

// // USART1
// #define  DEBUG_USART1                    USART1
// #define  DEBUG_USART1_CLK                RCC_APB2Periph_USART1
// #define  DEBUG_USART1_APBxClkCmd         RCC_APB2PeriphClockCmd
// #define  DEBUG_USART1_BAUDRATE           115200

// #define  DEBUG_USART1_GPIO_CLK           (RCC_APB2Periph_GPIOA)
// #define  DEBUG_USART1_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
// #define  DEBUG_USART1_TX_GPIO_PORT       GPIOA   
// #define  DEBUG_USART1_TX_GPIO_PIN        GPIO_Pin_9
// #define  DEBUG_USART1_RX_GPIO_PORT       GPIOA
// #define  DEBUG_USART1_RX_GPIO_PIN        GPIO_Pin_10

// #define  DEBUG_USART1_IRQ                USART1_IRQn
// #define  DEBUG_USART1_IRQHandler         USART1_IRQHandler

// // UART5
// #define  DEBUG_UART5                    UART5
// #define  DEBUG_UART5_CLK                RCC_APB1Periph_UART5
// #define  DEBUG_UART5_APBxClkCmd         RCC_APB1PeriphClockCmd
// #define  DEBUG_UART5_BAUDRATE           115200

// #define  DEBUG_UART5_GPIO_CLK1           (RCC_APB2Periph_GPIOC)
// #define  DEBUG_UART5_GPIO_CLK2           (RCC_APB2Periph_GPIOD)

// #define  DEBUG_UART5_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
   
// #define  DEBUG_UART5_TX_GPIO_PORT       GPIOC   
// #define  DEBUG_UART5_TX_GPIO_PIN        GPIO_Pin_12
// #define  DEBUG_UART5_RX_GPIO_PORT       GPIOD
// #define  DEBUG_UART5_RX_GPIO_PIN        GPIO_Pin_2

// #define  DEBUG_UART5_IRQ                UART5_IRQn
// #define  DEBUG_UART5_IRQHandler         UART5_IRQHandler


// extern __IO bool rxFrameFlag;
// extern __IO uint8_t rxCmd[FIFO_SIZE];
// extern __IO uint8_t rxCount;

// void usart_SendCmd(__IO uint8_t *cmd, uint8_t len);
// void usart_SendByte(uint16_t data);
// #endif
