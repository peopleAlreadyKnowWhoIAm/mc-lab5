#ifndef _SPI_H_
#define _SPI_H_

#include "memory-mapping.h"
#include "gpio.h"

#include <stdlib.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>

typedef struct {
  SPIMemoryMapping* mapping;
  
  uint8_t buffer[16];
  volatile uint8_t counter;
  uint8_t data_len;
  Pin* slave_pin;
  // uint8_t busy :1;
} SPI;

typedef uint8_t SPIEararchy;
typedef uint8_t SPIClock;

enum {
  SPI_master,
  SPI_slave,
};

enum {
  SPIClockDiv4 = 0,
  SPIClockDiv16 = 0b1,
  SpiClockDiv64 = 0b10,
  SpiClockDiv128 = 0b11,
  SpiClockPolarityInverse = 0b100,
  SpiClockSampleSetupLeading = 0b1000,
};

 // SCK and MOSI must be set ouput beforehand
SPI* SPIInit(SPIMemoryMapping* mapping, SPIEararchy eararchy, SPIClock clock);

bool SPIBusy(SPI* spi);

uint8_t* SPIGetBuffer(SPI* spi);
uint8_t SPIGetDataLength(SPI*spi);

int8_t SPIWrite(SPI* spi, uint8_t len, Pin* slave_enable_pin);



#endif