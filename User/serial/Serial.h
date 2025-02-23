#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"
#include <stdio.h>
// extern int gg;
extern uint16_t XX,YY,XX1,YY1;
extern char Serial_RxPacket[];
extern uint8_t Serial_RxFlag;

void UART5_init(void);

#endif
