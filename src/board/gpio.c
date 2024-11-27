#include "board/gpio.h"


void GPIO_Init() {
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /**USART2 GPIO Configuration
  PA2 ------> USART2_TX
  PA3 ------> USART2_RX 
  */
  GPIO_CFG(GPIOA, {.Pin = GPIO_PIN_2, .Mode = GPIO_MODE_AF_PP, .Speed = GPIO_SPEED_FREQ_HIGH, .Alternate = GPIO_AF4_USART2});
  GPIO_CFG(GPIOA, {.Pin = GPIO_PIN_3, .Mode = GPIO_MODE_AF_PP, .Speed = GPIO_SPEED_FREQ_HIGH, .Alternate = GPIO_AF4_USART2});

  /**SPI1 GPIO Configuration  
  PA15 ------> SPI1_NSS
  PB3  ------> SPI1_SCK
  PA7  ------> SPI1_MOSI
  PA6  ------> SPI1_MISO 
  */
  //GPIO_CFG(GPIOA, {.Pin = GPIO_PIN_15, .Mode = GPIO_MODE_AF_PP, .Speed = GPIO_SPEED_FREQ_HIGH, .Alternate = GPIO_AF0_SPI1});
  GPIO_CFG(GPIOA, {.Pin = GPIO_PIN_15, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_HIGH});
  GPIO_CFG(GPIOB, {.Pin = GPIO_PIN_3,  .Mode = GPIO_MODE_AF_PP, .Speed = GPIO_SPEED_FREQ_HIGH, .Alternate = GPIO_AF0_SPI1});  
  GPIO_CFG(GPIOA, {.Pin = GPIO_PIN_7,  .Mode = GPIO_MODE_AF_PP, .Speed = GPIO_SPEED_FREQ_HIGH, .Alternate = GPIO_AF0_SPI1});  
  GPIO_CFG(GPIOA, {.Pin = GPIO_PIN_6,  .Mode = GPIO_MODE_AF_PP, .Speed = GPIO_SPEED_FREQ_HIGH, .Alternate = GPIO_AF0_SPI1});

  /**SX1276 GPIO
  PC0  ------> RESET
  PB4  ------> DIO0
  PB1  ------> DIO1
  PB0  ------> DIO2  
  PC13 ------> DIO3
  PA5  ------> DIO4
  PA4  ------> DIO5
  PA1  ------> ANT_SWITCH_RX
  PC2  ------> SWITCH_TX_RFO
  PC1  ------> SWITCH_TX_BOOST
  */
  GPIO_CFG(GPIOC, {.Pin = GPIO_PIN_0,  .Mode = GPIO_MODE_INPUT});
  GPIO_CFG(GPIOB, {.Pin = GPIO_PIN_4,  .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_HIGH});
  GPIO_CFG(GPIOB, {.Pin = GPIO_PIN_1,  .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_HIGH});
  GPIO_CFG(GPIOB, {.Pin = GPIO_PIN_0,  .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_HIGH});
  GPIO_CFG(GPIOC, {.Pin = GPIO_PIN_13, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_HIGH});
  GPIO_CFG(GPIOA, {.Pin = GPIO_PIN_5,  .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_HIGH});
  GPIO_CFG(GPIOA, {.Pin = GPIO_PIN_4,  .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_HIGH});
  GPIO_CFG(GPIOA, {.Pin = GPIO_PIN_1,  .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_HIGH});
  GPIO_CFG(GPIOC, {.Pin = GPIO_PIN_2,  .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_HIGH});
  GPIO_CFG(GPIOC, {.Pin = GPIO_PIN_1,  .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_HIGH});

  /**LEDS GPIO
  PB5 ------> LED1 (Green)
  PA5 ------> LED2 (Green)
  PB6 ------> LED3 (Blue)
  PB7 ------> LED4 (Red)
  */
  GPIO_CFG(GPIOB, {.Pin = GPIO_PIN_5, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_HIGH});
  GPIO_CFG(GPIOA, {.Pin = GPIO_PIN_5, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_HIGH});
  GPIO_CFG(GPIOB, {.Pin = GPIO_PIN_6, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_HIGH});  
  GPIO_CFG(GPIOB, {.Pin = GPIO_PIN_7, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_HIGH});  

  /** Button
  PB2 ------> User button
  */
  GPIO_CFG(GPIOB, {.Pin = GPIO_PIN_2, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_HIGH});
}