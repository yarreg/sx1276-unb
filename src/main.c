
#include "board/board.h"
#include "sx1276-hal.h"
#include "crc.h"
#include "dbpsk.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define RF_FREQUENCY               868800000 // Hz
#define TX_OUTPUT_POWER            5         // dBm


#define FSK_FDEV                   5         //
#define FSK_DATARATE               0x682A    // See SX1276 datasheet
#define FSK_PREAMBLE_LENGTH        0         // Preamble length
#define FSK_FIX_LENGTH_PAYLOAD_ON  true
#define FSK_CRC_ON                 false

#define LORA_BANDWIDTH              7         // [0: 7.8 kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8kHz, 4: 31.25 kHz, 5: 41.7 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz]
#define LORA_SPREADING_FACTOR       12        // [SF7..SF12]
#define LORA_CODINGRATE             1         // [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
#define LORA_PREAMBLE_LENGTH        8         // Preamble length
#define LORA_FIX_LENGTH_PAYLOAD_ON  false
#define LORA_CRC_PAYLOAD_ON         true
#define LORA_IQ_INVERSION_ON        false


#define DBPSK_TSYM                   12
#define TRANSFER_BUFFER_SIZE         32        // Max bytes you plan to send (UNB)

static CREATE_DBPSK_BUFFER(dbpskBuffer, TRANSFER_BUFFER_SIZE, DBPSK_TSYM);
static Bits bits;


#define RAND_RANGE(min, max) (min + rand() / (RAND_MAX / (max - min + 1) + 1))


void SysTick_Handler() {
  HAL_IncTick();
}


void InitBoard() {
  HAL_Init();
  SystemClock_Config();
  GPIO_Init();
  UART_Init();
  SPI_Init();
  DebugInit(&huart2);
  SX1276HalInit();
}

void SendUNB(uint32_t freq, char *text, uint32_t counter) {
  SX1276TxConfig txConfig;
  txConfig.mode = MODE_FSK;
  txConfig.power = TX_OUTPUT_POWER;
  txConfig.fdev = FSK_FDEV;
  txConfig.bandwidth = 0;
  txConfig.datarate = FSK_DATARATE;
  txConfig.coderate = 0;
  txConfig.preambleLen = FSK_PREAMBLE_LENGTH;
  txConfig.fixLen = FSK_FIX_LENGTH_PAYLOAD_ON;
  txConfig.crcOn = FSK_CRC_ON;
  txConfig.iqInverted = false;

  SX1276SetTxConfig(txConfig);
  SX1276SetFreq(freq);

  // Formating packet
  DBPSKInit(&bits, dbpskBuffer, sizeof(dbpskBuffer), DBPSK_TSYM);
  AddDWord(&bits, 0xAAAAAAAA);                      // Preamble       [32 bits] [4 bytes]
  AddByte(&bits, 0xDB);                             // Sync world     [8 bits]  [1 byte]
  AddBuffer(&bits, (uint8_t*)text, strlen(text));   // Payload        [vary]
  AddDWord(&bits, counter);                         // Counter        [32 bits] [4 bytes]

  uint16_t checkSum = 0;
  checkSum = crc16(checkSum, (uint8_t*)text, strlen(text));
  counter  = htonl(counter);
  checkSum = crc16(checkSum, (uint8_t*)&counter, sizeof(counter));

  AddWord(&bits, checkSum);                         // CRC16           [16 bits] [2 byte]

  uint8_t data[TRANSFER_BUFFER_SIZE*8]; //Fixme
  uint16_t size = CopyBytes(&bits, data);
  SX1276Send(data, size);
}

void SendLoRa(uint32_t freq, char *text, uint32_t counter) {
  SX1276TxConfig txConfig;
  txConfig.mode = MODE_LORA;
  txConfig.power = 0;
  txConfig.fdev = 0;
  txConfig.bandwidth = 8;
  txConfig.datarate = 12;
  txConfig.coderate = LORA_CODINGRATE;
  txConfig.preambleLen = 12;
  txConfig.fixLen = LORA_FIX_LENGTH_PAYLOAD_ON;
  txConfig.crcOn = LORA_CRC_PAYLOAD_ON;
  txConfig.iqInverted = LORA_IQ_INVERSION_ON;

  SX1276SetTxConfig(txConfig);
  SX1276SetFreq(freq);

  uint8_t buffer[100] = {0xff};
  SX1276Send(buffer, 100);
}

void SendLoRa(uint32_t freq, char *text, uint32_t counter) {
  SX1276TxConfig txConfig;
  txConfig.mode = MODE_LORA;
  txConfig.power = TX_OUTPUT_POWER;
  txConfig.fdev = 0;
  txConfig.bandwidth = LORA_BANDWIDTH;
  txConfig.datarate = LORA_SPREADING_FACTOR;
  txConfig.coderate = LORA_CODINGRATE;
  txConfig.preambleLen = LORA_PREAMBLE_LENGTH;
  txConfig.fixLen = LORA_FIX_LENGTH_PAYLOAD_ON;
  txConfig.crcOn = LORA_CRC_PAYLOAD_ON;
  txConfig.iqInverted = LORA_IQ_INVERSION_ON;

  SX1276SetTxConfig(txConfig);
  SX1276SetFreq(freq);

  uint8_t buffer[100] = {0};
  memcpy(buffer, text, strlen(text));

  counter = htonl(counter);
  memcpy(buffer+strlen(text), &counter, sizeof(counter));

  SX1276Send(buffer, strlen(text) + sizeof(counter));
}

int main() {
  InitBoard();
  srand(0);

  uint32_t counter = 0;

  while (1) {
    if (counter++ % 2) {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, 1);
      SendUNB(RF_FREQUENCY + RAND_RANGE(-25000, 25000), "UNB", counter);
      SendUNB(RF_FREQUENCY, "UNB", counter);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, 0);
    } else {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 1);
      SendLoRa(RF_FREQUENCY + RAND_RANGE(-25000, 25000), "LoRa", counter);
      SendLoRa(RF_FREQUENCY, "LoRa", counter);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 0);
    }

    HAL_Delay(1000 + RAND_RANGE(-250, 250));
  }
}
