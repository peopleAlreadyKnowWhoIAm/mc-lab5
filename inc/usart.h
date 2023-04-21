#ifndef _USART_H_
#define _USART_H_

#include "main.h"

#include "memory-mapping.h"
#include "circ-buffer.h"

typedef struct {
  CircBuffer tx;
  CircBuffer rx;
} UsartBuffer;


typedef struct {
    UsartMemoryMapping* usart_mapping;
    volatile UsartBuffer buffer;
} Usart;


#define BUSY_TRANSMITTER (!(UCSR0A & (1<< TXC0)))

Usart* UsartInit(UsartMemoryMapping* usart, const uint32_t baudrate);

int8_t UsartWriteChar(Usart* usart, const char data);
int8_t UsartWriteChars(Usart* usart, const char* data, uint8_t len);
int8_t UsartWrite(Usart* usart, const char* string);

char UsartReadChar(Usart* usart);

bool UsartWriteBusy(Usart* usart);

#endif