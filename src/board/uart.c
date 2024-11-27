#include "board/uart.h"


UART_HandleTypeDef huart2;


bool UART_Init() {
  __HAL_RCC_USART2_CLK_ENABLE();

  huart2.Instance                    = USART2;
  huart2.Init.BaudRate               = 9600;
  huart2.Init.WordLength             = UART_WORDLENGTH_8B;
  huart2.Init.StopBits               = UART_STOPBITS_1;
  huart2.Init.Parity                 = UART_PARITY_NONE;
  huart2.Init.Mode                   = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling           = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  return (HAL_UART_Init(&huart2) == HAL_OK);     
}
