#include "board/gpio.h"
#include "board/spi.h"
#include "sx1276.h"

static SX1276Dev SX1276;

static void SX1276Reset() {
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 1);

  GPIO_CFG(GPIOC, {.Pin = GPIO_PIN_0, .Mode = GPIO_MODE_OUTPUT_PP});
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
  HAL_Delay(1);

  GPIO_CFG(GPIOC, {.Pin = GPIO_PIN_0, .Mode = GPIO_MODE_INPUT});
  HAL_Delay(10);
}

static void SX1276Write(uint8_t addr, uint8_t *buffer, uint8_t size) {
  addr |= 0x80;
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 0);
  HAL_SPI_Transmit(&hspi1, &addr, 1, HAL_MAX_DELAY);
  HAL_SPI_Transmit(&hspi1, buffer, size, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 1);
}

static void SX1276Read(uint8_t addr, uint8_t *buffer, uint8_t size) {
  addr &= 0x7f;
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 0);
  HAL_SPI_Transmit(&hspi1, &addr, 1, HAL_MAX_DELAY);
  HAL_SPI_Receive(&hspi1, buffer, size, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 1);
}

static void SX1276ConfigureRfSwitch(uint8_t opMode, int8_t power) {
  switch (opMode) {
  case RFLR_OPMODE_TRANSMITTER:
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 0); // TX

    if (power > 15) {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, 0);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, 1);
    } else {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, 0);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, 1);
    }
    break;
  case RFLR_OPMODE_RECEIVER:
  case RFLR_OPMODE_RECEIVER_SINGLE:
  case RFLR_OPMODE_CAD:
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 1); // RX
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, 0);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, 0);
    break;
  default:
    break;
  }
}

void SX1276HalInit() {
  SX1276.Reset = SX1276Reset;
  SX1276.Write = SX1276Write;
  SX1276.Read = SX1276Read;
  SX1276.ConfigureRfSwitch = SX1276ConfigureRfSwitch;
  SX1276Init(&SX1276);
}