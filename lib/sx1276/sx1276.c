#include "sx1276.h"
#include <delay.h>
#include <stddef.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static SX1276Dev *sx1276dev = NULL;
static SX1276TxConfig TxConfig;

static void Write(uint8_t addr, uint8_t data) {
  sx1276dev->Write(addr, &data, 1);
}

static uint8_t Read(uint8_t addr) {
  uint8_t data = 0;
  sx1276dev->Read(addr, &data, 1);
  return data;
}

static void WriteFifo(uint8_t *data, uint8_t size) {
  sx1276dev->Write(REG_FIFO, data, size);
}

static void RxChainCalibrate(void) {
  uint8_t regPaConfigInitVal;
  uint32_t initialFreq;

  // Save context
  regPaConfigInitVal = Read(REG_PACONFIG);
  initialFreq = (double)(((uint32_t)Read(REG_FRFMSB) << 16) |
                         ((uint32_t)Read(REG_FRFMID) << 8) |
                         ((uint32_t)Read(REG_FRFLSB))) *
                (double)FREQ_STEP;

  // Cut the PA just in case, RFO output, power = -1 dBm
  Write(REG_PACONFIG, 0x00);

  // Launch Rx chain calibration for LF band
  Write(REG_IMAGECAL, (Read(REG_IMAGECAL) & RF_IMAGECAL_IMAGECAL_MASK) |
                          RF_IMAGECAL_IMAGECAL_START);
  while ((Read(REG_IMAGECAL) & RF_IMAGECAL_IMAGECAL_RUNNING) ==
         RF_IMAGECAL_IMAGECAL_RUNNING) {
  }

  // Sets a Frequency in HF band
  SX1276SetFreq(868000000);

  // Launch Rx chain calibration for HF band
  Write(REG_IMAGECAL, (Read(REG_IMAGECAL) & RF_IMAGECAL_IMAGECAL_MASK) |
                          RF_IMAGECAL_IMAGECAL_START);
  while ((Read(REG_IMAGECAL) & RF_IMAGECAL_IMAGECAL_RUNNING) ==
         RF_IMAGECAL_IMAGECAL_RUNNING) {
  }

  // Restore context
  Write(REG_PACONFIG, regPaConfigInitVal);
  SX1276SetFreq(initialFreq);
}

bool SX1276Init(SX1276Dev *dev) {
  sx1276dev = dev;
  sx1276dev->Reset();

  RxChainCalibrate();

  if (Read(REG_VERSION) != 0x12) {
    return false;
  }

  Write(REG_OCP, (RF_OCP_ON | RF_OCP_TRIM_150_MA));
  Write(REG_TCXO,
        (Read(REG_TCXO) & RF_TCXO_TCXOINPUT_MASK) | RF_TCXO_TCXOINPUT_ON);

  return true;
}

static int8_t SX1276SetRfTxPower(int8_t power) {
  uint8_t paConfig, paDac;

  if (power < -4) {
    power = -4;
  }

  if (power > 20) {
    power = 20;
  }

  if (power > 15) {
    if (power > 17) {
      paConfig = (RF_PACONFIG_PASELECT_PABOOST | (power - 5));
      paDac = RF_PADAC_20DBM_ON;
    } else {
      paConfig = (RF_PACONFIG_PASELECT_PABOOST | (power - 2));
      paDac = RF_PADAC_20DBM_OFF;
    }
  } else {
    if (power > 0) {
      paConfig = (RF_PACONFIG_PASELECT_RFO | (7 << 4) | (power));
      paDac = RF_PADAC_20DBM_OFF;
    } else {
      paConfig = (RF_PACONFIG_PASELECT_RFO | (0 << 4) | (power + 4));
      paDac = RF_PADAC_20DBM_OFF;
    }
  }

  Write(REG_PACONFIG, paConfig);
  Write(REG_PADAC, ((Read(REG_PADAC) & RF_PADAC_20DBM_MASK) | paDac));

  return power;
}

static void SX1276SetOpMode(uint8_t opMode) {
  Write(REG_OPMODE, (Read(REG_OPMODE) & RF_OPMODE_MASK) | opMode);
}

bool SX1276SetTxConfig(SX1276TxConfig txConfig) {
  txConfig.power = SX1276SetRfTxPower(txConfig.power);

  if (txConfig.mode == MODE_FSK) {
    SX1276SetOpMode(RF_OPMODE_SLEEP);
    Write(REG_OPMODE, (Read(REG_OPMODE) & RFLR_OPMODE_LONGRANGEMODE_MASK) |
                          RFLR_OPMODE_LONGRANGEMODE_OFF);

    Write(REG_FDEVMSB, (uint8_t)(txConfig.fdev >> 8));
    Write(REG_FDEVLSB, (uint8_t)(txConfig.fdev & 0xFF));

    Write(REG_BITRATEMSB, (uint8_t)(txConfig.datarate >> 8));
    Write(REG_BITRATELSB, (uint8_t)(txConfig.datarate & 0xFF));

    Write(REG_PREAMBLEMSB, (txConfig.preambleLen >> 8) & 0x00FF);
    Write(REG_PREAMBLELSB, txConfig.preambleLen & 0xFF);

    Write(REG_PACKETCONFIG1,
          (Read(REG_PACKETCONFIG1) & RF_PACKETCONFIG1_CRC_MASK &
           RF_PACKETCONFIG1_PACKETFORMAT_MASK) |
              ((txConfig.fixLen == 1)
                   ? RF_PACKETCONFIG1_PACKETFORMAT_FIXED
                   : RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE) |
              (txConfig.crcOn << 4));
  } else if (txConfig.mode == MODE_LORA) {
    SX1276SetOpMode(RF_OPMODE_SLEEP);
    Write(REG_OPMODE, (Read(REG_OPMODE) & RFLR_OPMODE_LONGRANGEMODE_MASK) |
                          RFLR_OPMODE_LONGRANGEMODE_ON);

    bool lowDatarateOptimize = false;

    if (txConfig.datarate > 12) {
      txConfig.datarate = 12;
    } else if (txConfig.datarate < 6) {
      txConfig.datarate = 6;
    }

    if (((txConfig.bandwidth == 7) &&
         ((txConfig.datarate == 11) || (txConfig.datarate == 12))) ||
        ((txConfig.bandwidth == 8) && (txConfig.datarate == 12))) {
      lowDatarateOptimize = true;
    }

    Write(REG_LR_MODEMCONFIG1,
          (Read(REG_LR_MODEMCONFIG1) & RFLR_MODEMCONFIG1_BW_MASK &
           RFLR_MODEMCONFIG1_CODINGRATE_MASK &
           RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK) |
              (txConfig.bandwidth << 4) | (txConfig.coderate << 1) |
              txConfig.fixLen);

    Write(REG_LR_MODEMCONFIG2,
          (Read(REG_LR_MODEMCONFIG2) & RFLR_MODEMCONFIG2_SF_MASK &
           RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK) |
              (txConfig.datarate << 4) | (txConfig.crcOn << 2));

    Write(REG_LR_MODEMCONFIG3, (Read(REG_LR_MODEMCONFIG3) &
                                RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK) |
                                   (lowDatarateOptimize << 3));

    Write(REG_LR_PREAMBLEMSB, (txConfig.preambleLen >> 8) & 0x00FF);
    Write(REG_LR_PREAMBLELSB, txConfig.preambleLen & 0xFF);

    if (txConfig.datarate == 6) {
      Write(REG_LR_DETECTOPTIMIZE,
            (Read(REG_LR_DETECTOPTIMIZE) & RFLR_DETECTIONOPTIMIZE_MASK) |
                RFLR_DETECTIONOPTIMIZE_SF6);
      Write(REG_LR_DETECTIONTHRESHOLD, RFLR_DETECTIONTHRESH_SF6);
    } else {
      Write(REG_LR_DETECTOPTIMIZE,
            (Read(REG_LR_DETECTOPTIMIZE) & RFLR_DETECTIONOPTIMIZE_MASK) |
                RFLR_DETECTIONOPTIMIZE_SF7_TO_SF12);
      Write(REG_LR_DETECTIONTHRESHOLD, RFLR_DETECTIONTHRESH_SF7_TO_SF12);
    }

    // IQ invert
    if (txConfig.iqInverted) {
      Write(REG_LR_INVERTIQ, ((Read(REG_LR_INVERTIQ) & RFLR_INVERTIQ_TX_MASK &
                               RFLR_INVERTIQ_RX_MASK) |
                              RFLR_INVERTIQ_RX_OFF | RFLR_INVERTIQ_TX_ON));
      Write(REG_LR_INVERTIQ2, RFLR_INVERTIQ2_ON);
    } else {
      Write(REG_LR_INVERTIQ, ((Read(REG_LR_INVERTIQ) & RFLR_INVERTIQ_TX_MASK &
                               RFLR_INVERTIQ_RX_MASK) |
                              RFLR_INVERTIQ_RX_OFF | RFLR_INVERTIQ_TX_OFF));
      Write(REG_LR_INVERTIQ2, RFLR_INVERTIQ2_OFF);
    }

    // Syncword
    Write(REG_LR_SYNCWORD, 0x34);
  }

  TxConfig = txConfig;
  return true;
}

void SX1276SetFreq(uint32_t freq) {
  freq = (uint32_t)((double)freq / (double)FREQ_STEP);
  Write(REG_FRFMSB, (uint8_t)((freq >> 16) & 0xFF));
  Write(REG_FRFMID, (uint8_t)((freq >> 8) & 0xFF));
  Write(REG_FRFLSB, (uint8_t)(freq & 0xFF));
}

void SX1276Send(uint8_t *data, uint16_t size) {
  // FIFO operations can not take place in Sleep mode
  if ((Read(REG_OPMODE) & ~RF_OPMODE_MASK) == RF_OPMODE_SLEEP) {
    SX1276SetOpMode(RF_OPMODE_STANDBY);
    DelayMs(1);
  }

  SX1276SetOpMode(RF_OPMODE_SYNTHESIZER_TX);
  sx1276dev->ConfigureRfSwitch(RF_OPMODE_TRANSMITTER, TxConfig.power);

  if (TxConfig.mode == MODE_FSK) {
    // Set payload size
    Write(REG_PACKETCONFIG2,
          (Read(REG_PACKETCONFIG2) & RF_PACKETCONFIG2_PAYLOADLENGTH_MSB_MASK) |
              ((size >> 8) & 0x07));
    Write(REG_PAYLOADLENGTH, (uint8_t)(size & 0xFF));

    // Set fifo threshold level = 64 (FIFO_SIZE) / 2 = 32
    // FifoThreshold: number of bytes in FIFO >= FifoThreshold + 1
    Write(REG_FIFOTHRESH, RF_FIFOTHRESH_TXSTARTCONDITION_FIFONOTEMPTY |
                              (32 & ~RF_FIFOTHRESH_FIFOTHRESHOLD_MASK));

    // Prefill buffers
    uint8_t chunkSize = MIN(64, size);
    uint16_t txBytes = chunkSize;

    WriteFifo(data, (uint8_t)chunkSize);
    SX1276SetOpMode(RF_OPMODE_TRANSMITTER);
    data += chunkSize;

    do {
      while ((Read(REG_IRQFLAGS2) & RF_IRQFLAGS2_FIFOLEVEL)) {
        DelayMs(1);
      }

      chunkSize = MIN(32, size - txBytes);
      WriteFifo(data, (uint8_t)chunkSize);
      data += chunkSize;
      txBytes += chunkSize;
    } while (txBytes < size);

    while (!(Read(REG_IRQFLAGS2) & RF_IRQFLAGS2_PACKETSENT)) {
      DelayMs(1);
    };

  } else {
    Write(REG_LR_PAYLOADLENGTH, (uint8_t)(size & 0xFF));

    // Clear IRQ
    Write(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE);

    // Full buffer used for Tx
    Write(REG_LR_FIFOTXBASEADDR, 0);
    Write(REG_LR_FIFOADDRPTR, 0);

    WriteFifo(data, (uint8_t)(size & 0xFF));
    SX1276SetOpMode(RFLR_OPMODE_TRANSMITTER);

    while (!(Read(REG_LR_IRQFLAGS) & RFLR_IRQFLAGS_TXDONE)) {
      DelayMs(1);
    }

    Write(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE);
  }

  SX1276SetOpMode(RF_OPMODE_STANDBY);
}