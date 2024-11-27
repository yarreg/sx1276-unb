#ifndef DBPSK_H
#define DBPSK_H

#include <stdint.h>
#include "dbpsk-utils.h"

#define CREATE_DBPSK_BUFFER(bufferVarName, bytes, tsym) uint8_t bufferVarName[bytes*8*tsym];

typedef struct
{
    uint8_t *buffer;    // Buffer to store bits
    uint16_t lastIndex; // Current bit index
    uint16_t size;      // Size of buffer
	uint8_t  tsym;      // Symbol time
} Bits;


void DBPSKInit(Bits *bits, uint8_t *buffer, uint16_t size, uint8_t tsym);

void AddBit(Bits *bits, uint8_t bit);
void AddBits(Bits *bits, uint8_t bit, uint16_t count);
void AddBuffer(Bits *bits, uint8_t *buffer, uint16_t size);
void AddByte(Bits *bits, uint8_t byte);
void AddWord(Bits *bits, uint16_t word);
void AddDWord(Bits *bits, uint32_t dword);

uint8_t  GetByteAtIndex(Bits *bits, uint16_t index);
uint8_t  GetByte(Bits *bits, uint16_t position);
uint16_t GetBytesCount(Bits *bits);
uint16_t CopyBytes(Bits *bits, uint8_t *buffer);

void BitsDump(Bits *bits);

#endif
