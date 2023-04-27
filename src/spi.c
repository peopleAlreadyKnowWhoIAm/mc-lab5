#include "spi.h"

static SPI* spi0;

SPI* SPIInit(SPIMemoryMapping* mapping, SPIEararchy eararchy, SPIClock clock) {
  mapping->spcr = _BV(SPE) | (eararchy & 0b1) << MSTR | clock;

  SPI* spi = malloc(sizeof(SPI));
  spi->mapping = mapping;

  spi0 = spi;
  return spi;
}

bool SPIBusy(SPI* spi) { return (_BV(SPIE) & spi->mapping->spcr); }

uint8_t* SPIGetBuffer(SPI* spi) { return spi->buffer; }

uint8_t SPIGetDataLength(SPI* spi) { return spi->data_len; }

int8_t SPIWrite(SPI* spi, uint8_t len, Pin* slave_pin) {
  if(SPIBusy(spi) || len == 0){
    return -1;
  }

  // Enable chip

  spi->data_len = len;
  spi->counter = 0;
  spi->slave_pin = slave_pin;
  // Write first byte
  slave_pin->gpio->PORT ^= _BV(slave_pin->pin_number);
  uint8_t a = spi->mapping->spdr;
  spi->mapping->spdr = spi->buffer[0];
  spi->mapping->spcr |= _BV(SPIE);
  return 0;
}

static void send_data(SPI* spi) {
  if (spi->counter == spi->data_len - 1) {
    spi->buffer[spi->counter] = spi->mapping->spdr;
    spi->mapping->spcr &= ~_BV(SPIE);
    spi->slave_pin->gpio->PORT ^= _BV(spi->slave_pin->pin_number);
    
  } else {
    spi->buffer[spi->counter] = spi->mapping->spdr;
    spi->counter++;
    spi->mapping->spdr = spi->buffer[spi->counter];
  }
}

ISR(SPI_STC_vect) { send_data(spi0); }