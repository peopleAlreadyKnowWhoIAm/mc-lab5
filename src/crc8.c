#include "crc8.h"

static uint8_t crc_table[256];

inline static uint8_t calculate_for_byte(uint8_t data) {
  for (uint8_t i = 0; i < 8; i++) {
    if (data & 0b10000000) {
      data <<= 1;
      data ^= CRC8_POLY;
    } else {
      data <<= 1;
    }
  }
  return data;
}

void Crc8InitTable() {
  for(uint8_t i = 0; i < 0xff; i++) {
    crc_table[i] = calculate_for_byte(i);
  }
  crc_table[0xff] = calculate_for_byte(0xff);
}

bool Crc8Check(uint8_t data[6]) {
  if(Crc8Calculate(data) == data[5]){
    return true;
  }
  return false;
}

uint8_t Crc8Calculate(uint8_t data[5]) {
  uint8_t crc = crc_table[CRC8_INIT ^ data[0]];
  for (uint8_t i = 1; i < 5; i++) {
    crc = crc_table[crc ^ data[i]];
  }

  return crc ^ CRC8_XOR_OUT_WITH;
}
