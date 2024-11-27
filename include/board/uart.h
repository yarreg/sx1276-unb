#ifndef UART_H
#define UART_H

#include <stdbool.h>
#include "stm32l0xx_hal.h"


extern UART_HandleTypeDef huart2;


bool UART_Init();

#endif