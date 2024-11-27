#ifndef CRC_H
#define CRC_H

#include <stdint.h>

uint32_t crc32(uint32_t crc, uint8_t *buffer, uint32_t size);
uint16_t crc16(uint16_t crc, uint8_t *buffer, uint32_t size);

#endif