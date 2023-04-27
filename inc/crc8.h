#ifndef _CRC8_H_
#define _CRC8_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define CRC8_POLY 0xd5
#define CRC8_INIT 0x00
// #define CRC8_REVERSE_INPUT false
// #define CRC8_REVERSE_OUTPUT false
#define CRC8_XOR_OUT_WITH 0x00

void Crc8InitTable();

bool Crc8Check(uint8_t data[6]);

uint8_t Crc8Calculate(uint8_t data[5]);

uint8_t Crc8Decode(char* result, const char* input, uint8_t input_length);
uint8_t Crc8Encode(char* result, const uint8_t* input, const uint8_t len);


#endif