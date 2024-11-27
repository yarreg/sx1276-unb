#include "dbpsk.h"
#include <stdio.h>

void DBPSKInit(Bits *bits, uint8_t *buffer, uint16_t size, uint8_t tsym) {
  bits->size = size;
  bits->lastIndex = 0;
  bits->buffer = buffer;
  bits->tsym = tsym;
}

void AddBit(Bits *bits, uint8_t bit) {
  if (bits->lastIndex >= bits->size) {
    // Assert
    return;
  }
  bit = !!bit; // convert to 0..1
  bits->buffer[bits->lastIndex] = bit;
  bits->lastIndex++;
}

void AddBits(Bits *bits, uint8_t bit, uint16_t count) {
  uint16_t i;
  for (i = 0; i < count; i++) {
    AddBit(bits, bit);
  }
}

void AddBuffer(Bits *bits, uint8_t *buffer, uint16_t size) {
  uint8_t byte, bit;
  uint16_t i;
  int8_t j;

  for (i = 0; i < size; i++) {
    byte = buffer[i];
    for (j = 7; j >= 0; j--) {
      bit = (byte >> j) & 1;
      if (bit) {
        AddBits(bits, 0, bits->tsym);
      } else {
        AddBits(bits, 1, 1);
        AddBits(bits, 0, bits->tsym - 1);
      }
    }
  }
}

void AddByte(Bits *bits, uint8_t byte) {
    AddBuffer(bits, &byte, sizeof(byte));
}

void AddWord(Bits *bits, uint16_t word) {
  word = htons(word);
  AddBuffer(bits, (uint8_t *)&word, sizeof(word));
}

void AddDWord(Bits *bits, uint32_t dword) {
  dword = htonl(dword);
  AddBuffer(bits, (uint8_t *)&dword, sizeof(dword));
}

uint8_t GetByteAtIndex(Bits *bits, uint16_t index) {
  int8_t i;
  uint8_t bit, byte;

  if (index + 8 > bits->lastIndex) {
    return 0; // Assert
  }

  byte = 0;
  for (i = 7; i >= 0; i--) {
    bit = bits->buffer[index++];
    byte |= bit << i;
  }

  return byte;
}

uint8_t GetByte(Bits *bits, uint16_t position) {
  return GetByteAtIndex(bits, position * 8);
}

uint16_t GetBytesCount(Bits *bits) {
  if (!(bits->lastIndex % 8)) {
    return bits->lastIndex / 8;
  }

  return (bits->lastIndex / 8) + 1;
}

uint16_t CopyBytes(Bits *bits, uint8_t *buffer) {
  uint16_t i;
  for (i = 0; i < GetBytesCount(bits); i++) {
    buffer[i] = GetByte(bits, i);
  }
  return i;
}

void BitsDump(Bits *bits) {
  uint16_t i;
  for (i = 0; i < bits->lastIndex; i++) {
    if (i % 8 == 0)
      printf(" ");
    if (i % 32 == 0)
      printf("\r\n");
    printf("%d", bits->buffer[i]);
  }
  printf("\r\n");
}
