#include "main.h"


int main() {
  MCC* rs = MCCInit(USART0, RS485_BR, MCC_SLAVE, (Pin){.gpio = GPIOD, .pin_number = 2});
  rs->address = 38;
  GPIOB->DDB |= _BV(PB3) | _BV(PB5);
  SPI* spi = SPIInit(SPI0, SPI_master, SpiClockDiv128);
  Pin pin_clock = {.gpio=GPIOB, .pin_number=2};
  PCF2123Reset(spi, &pin_clock);
  Crc8InitTable();
  CounterInit();
  sei();
  char buf[100];
  while (1) {
    while (RS485Busy(rs)) {
    }
    RS485Read(rs, buf);
    switch(buf[0]){
      case 'g':
        PCF2123ReadTimeStart(spi,&pin_clock);
        while(SPIBusy(spi)){}
        PCF2123ReadTimeFinish(spi,(uint8_t*) buf);
        buf[7] = 0;
        RS485Write(rs, buf);
        break;
      case 's':
        PCF2123WriteTime(spi,(uint8_t*) &buf[1], &pin_clock);
        while(SPIBusy(spi)){}
        RS485Write(rs, "done");
        break;
    }
    // RS485Write(rs, buf);

    // while (RS485Busy(rs)) {
    // }
    // RS485Write(rs, buf);

    // while (RS485Busy(rs)) {
    // }
    // RS485WriteWithError(rs, buf, 0,0b1000);

    // // rs->buffer[tx_len-1] ^= 0b1000;
    // while (RS485Busy(rs)) {
    // }
    // RS485WriteWithError(rs, buf, 1,0b10001000);
    // // rs->buffer[tx_len-2] ^= 0b10001000;
    // while (RS485Busy(rs)) {
    // }
    // RS485Write(rs, buf);
    
  }
  return 0;
}

