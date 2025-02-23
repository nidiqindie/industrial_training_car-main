#ifndef __SERIAL_k210H
#define __SERIAL_k210H

#include "stm32f10x.h"
#include <stdio.h>

extern char Serial_RxPacket[];
extern uint8_t Serial_RxFlag;

void UART4_init(void);

#endif
