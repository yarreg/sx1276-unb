#ifndef DEBUG_H
#define DEBUG_H

#include "stm32l0xx_hal.h"


void DebugInit(UART_HandleTypeDef *uart);
void DebugPrint(const char * fmt, ...);


#endif