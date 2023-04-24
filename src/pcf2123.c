#include "pcf2123.h"

#define PCF_MASK _BV(4)
#define PCF_READ _BV(7)
#define PCF_SECONDS_ADDRESS 0x2

int8_t PCF2123Reset(SPI* spi, Pin* chip_enable) {
  if(SPIBusy(spi)){
    return -1;
  }
  uint8_t buf* = SPIGetBuffer(spi);
  buf[0] = PCF_MASK; // Write to address 0
  buf[1] = 0x58; // Combination to software reset
  return SPIWrite(spi, 2, chip_enable);
}

int8_t PCF2123WriteTime(SPI* spi, uint8_t time, Pin* chip_enable){
  if(SPIBusy(spi)){
    return -1;
  }
  uint8_t buf* = SPIGetBuffer(spi);
  buf[0] = PCF_MASK | PCF_SECONDS_ADDRESS;
  memcpy(&buf[1], time, sizeof(uint8_t) * 7);
  return SPIWrite(spi, 8, chip_enable);
}

int8_t PCF2123ReadTimeStart(SPI* spi, Pin* chip_enable) {
  if(SPIBusy(spi)){
    return -1;
  }
  uint8_t buf* = SPIGetBuffer(spi);
  buf[0] = PCF_MASK | PCF_READ | PCF_SECONDS_ADDRESS;
  memset(&buf[1], 0, 7);
  return SPIWrite(spi,8, chip_enable);
}

int8_t PCF2123ReadTimeFinish(SPI* spi, uint8_t* buffer) {
  if(SPIBusy(spi)){
    return -1;
  }
  uint8_t spi_buf* = SPIGetBuffer(spi);
  uint8_t spi_buf_len = SPIGetDataLength(spi);
  memcpy(buffer, &spi_buf[1], 7);
  return 0;
}