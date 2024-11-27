#include "debug.h"

#include <stdio.h>
#include <stdarg.h>


static UART_HandleTypeDef *debugUART = NULL;


int __io_putchar(int ch) {
  /* Implementation of fputc here */
  if (debugUART != NULL) {
    HAL_UART_Transmit(debugUART, (uint8_t *)&ch, 1, 0xffffff);
  }
  return ch;
}

int _write(int file, char *ptr, int len) {
  for (int i = 0; i < len; i++) {
    __io_putchar(*ptr++);
  }
  return len;
}

void DebugInit(UART_HandleTypeDef *uart) {
  debugUART = uart;
}

void DebugPrint(const char * fmt, ...) {
  char buffer[200];

  va_list argp;
  va_start(argp, fmt);

  uint8_t len = vsprintf(buffer, fmt, argp);
  if (len > 0 && debugUART != NULL) {
    HAL_UART_Transmit(debugUART, (uint8_t*)buffer, len, 0xffffff);
  }

  va_end(argp);
}