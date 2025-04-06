#pragma once

#include <inttypes.h>

enum
{
    CRC_1021,
    CRC_1189,
};

extern const unsigned short *const crc16tab[2];

uint8_t crc8(const uint8_t *ptr, uint32_t len);
uint8_t crc8_BA(const uint8_t *ptr, uint32_t len);
uint16_t crc16(uint8_t index, const uint8_t *buf, uint32_t len, uint16_t start = 0);
