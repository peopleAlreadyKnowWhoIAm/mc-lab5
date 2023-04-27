#ifndef _RS485_H_
#define _RS485_H_

#include <stdbool.h>

#include "multi-cpu-connection.h"
#include "crc8.h"

#include <string.h>

typedef MCC RS485;

RS485* RS485InitMaster(UsartMemoryMapping* usart, const uint32_t baudrate, Pin direction_pin);

RS485* RS485InitSlave(UsartMemoryMapping* usart, const uint32_t baudrate, const uint8_t address, Pin direction_pin);

int8_t RS485MasterSendAddress(RS485* master, uint8_t address);
// Returns -1 on failure
int8_t RS485Write(RS485* mcc, const char* text);

int8_t RS485WriteBytes(RS485* rs, const uint8_t* data, const uint8_t len);

int8_t RS485WriteWithError(RS485*mcc, const char* text, int8_t error_byte, uint8_t error_mask);
// Returns -1 on busy
// Returns -2 on receive fault
// Output must be atleast 96 bytes in length
int8_t RS485Read(RS485* mcc, char* output);

bool RS485Busy(RS485 * rs);

void RS485Free(RS485* mcc);
#endif