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
  if(Crc8Calculate(data) == 0xD){
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
uint8_t Crc8Decode(char* result, const char* input, uint8_t input_length) {
  uint8_t out_indx = 0;
  uint8_t pos = 0;
  if (input_length == 0 || input_length % 6 != 0) {
    return 0;
  }
  uint8_t sub_buf[6];
  while (pos != input_length) {
    sub_buf[pos % 6] = input[pos];
    if (pos % 6 == 5) {
      if(!Crc8Check(sub_buf)) {
        return 0;
      }
    } else {
      result[out_indx] = input[pos];
      out_indx++;
    }
    pos++;
  }

  result[pos] = 0;
  return pos;
}


uint8_t Crc8Encode(char* result, const char* input)
{

  uint8_t tx_len = (strlen(input) + 4) / 5 * 6;
  uint8_t tx_indx = 0;
  uint8_t sub_buf[5];
  uint8_t sub_buf_idx = 0;
  while (*input != '\0' || sub_buf_idx != 0) {
    sub_buf[sub_buf_idx] = *input;
    result[tx_indx] = *input;
    tx_indx++;
    sub_buf_idx++;
    if (*input != '\0') {
      input++;
    }
    if (sub_buf_idx == 5) {
      uint8_t crc = Crc8Calculate(sub_buf);
      sub_buf_idx = 0;
      result[tx_indx] = crc;
      tx_indx++;
    }
  }

  return tx_len;
}