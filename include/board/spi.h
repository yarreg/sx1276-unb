#ifndef SPI_H
#define SPI_H

#include <stdbool.h>
#include "stm32l0xx_hal.h"

extern SPI_HandleTypeDef hspi1;


bool SPI_Init();

#endif