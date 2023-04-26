/**
 * @file rs485.h
 * @brief File which implements RS485 protocol with usart
 * @version 0.1
 * @date 2023-04-05
 * 
 * @copyright Copyright (c) 2023
 * 
 * How it works
 * Rules:
 * There are 1 master and slaves
 * Master choose slave to communicate
 * Master initialize communication
 * Slave must give response
 * Transmition ends with \4
 * Orr after timeout
 * \0 symbol indicates of transmission error
 * This symbols must not be used in messages
 * Communication ends when master send next address byte
 * After recognising address Slave must send \0 message to confirm
 * If
 * 
 */


#ifndef _MULTI_CPU_CONNECTION_H_
#define _MULTI_CPU_CONNECTION_H_

#ifndef F_CPU
#error "F_CPU must be predefined"
#endif


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdbool.h>

#include "memory-mapping.h"
#include "crc8.h"
#include "counter.h"
#include "gpio.h"

#define BUSY_TX(usart) (!(usart->UCSRA & _BV(UDRE0)))

#define MCC_BUFFER_SIZE 128

typedef struct String {
  char* str;
  uint8_t len;
} String;

typedef uint8_t MCCStatus;
typedef uint8_t MCCEararchy;

enum {
  //Needs user action to start operating
  MCC_IDLE_CONNECTED,
  MCC_IDLE_DISCONNECTED,
  MCC_PENDING_ADRESS,
  MCC_SENDING,
  MCC_LISTENING,
  MCC_RECEIVING,
};

enum {
  MCC_MASTER,
  MCC_SLAVE,
};

typedef struct {
  char buffer[MCC_BUFFER_SIZE];
  volatile MCCStatus status : 4;
  MCCEararchy eararchy : 1;
  bool receive_fault;
  uint8_t address;  // In master - target address, in slave - ic address
  uint8_t receive_timestamp;
  
  UsartMemoryMapping* mapping;

  Pin direction_pin;
  // Text fragment must end with '\0' termination
  uint8_t buffer_pos;
  uint8_t buffer_data_size;
  TimerGP8* timer;
} MCC;

MCC* MCCInit(UsartMemoryMapping* usart, const uint32_t baudrate,
                 MCCEararchy eararchy, Pin direction_pin);

int8_t MCCMasterSendAddress(MCC* master, uint8_t address);

//String must be reversed
// Returns -1 on failure
int8_t MCCWrite(MCC* mcc, uint8_t len);

char* MCCGetBuffer(MCC* mcc);
// Returns 0 on failure
uint8_t MCCGetDataLength(MCC* mcc);

bool MCCBusy(MCC* mcc);

void MCCFree(MCC* mcc);

#endif