#ifndef GPIO_H
#define GPIO_H

#include "stm32l0xx_hal.h"


#define GPIO_CFG(PORT, ...) HAL_GPIO_Init(PORT, (&(GPIO_InitTypeDef)__VA_ARGS__))


void GPIO_Init();

#endif