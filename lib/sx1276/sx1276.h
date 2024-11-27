#ifndef __SX1276_H__
#define __SX1276_H__

#include <stdbool.h>

#include "sx1276Regs-Fsk.h"
#include "sx1276Regs-LoRa.h"

#include "sx1276Dev.h"

/*!
 * SX1276 definitions
 */
#define XTAL_FREQ 32000000
#define FREQ_STEP 61.03515625

typedef enum
{
    MODE_FSK = 0,
    MODE_LORA,
} SX1276RadioMode;

typedef struct
{
    SX1276RadioMode mode;
    int8_t power;
    uint32_t fdev;
    uint32_t bandwidth;
    uint32_t datarate;
    uint8_t coderate;
    uint16_t preambleLen;
    bool fixLen;
    bool crcOn;
    bool iqInverted;
} SX1276TxConfig;

bool SX1276Init(SX1276Dev *dev);
bool SX1276SetTxConfig(SX1276TxConfig txConfig);
void SX1276SetFreq(uint32_t freq);
void SX1276Send(uint8_t *buffer, uint16_t size);

#endif