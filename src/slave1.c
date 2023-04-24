#include "main.h"

int main() {
  MCC* rs = MCCInit(USART0, RS485_BR, MCC_SLAVE,
                    (Pin){.gpio = GPIOD, .pin_number = 2});
  rs->address = 94;
  SPI* spi = SPIInit(SPI0, SPI_master, SpiClockDiv128);
  const Pin pin_clock = {.gpio=GPIOB, .pin_number=2};
  PCF2123Reset(spi, &pin_clock);
  sei();
  char buf[100];
  while (1) {
    while (RS485Busy(rs)) {
    }

    RS485Read(rs, buf);
    switch(buf[0]){
      case 'r':
        PCF2123ReadTimeStart(spi,&pin_clock);
        while(SPIBusy(spi)){}
        PCF2123ReadTimeFinish(spi, buf);
        buf[7] = 0;
        RS485Write(rs, buf);
        break;
      case 's':
        PCF2123WriteTime(spi, &buf[1], &pin_clock);
        while(SPIBusy(spi)){}
        RS485Write(rs, "done");
        break;
    }

    // RS485Write(rs, buf);

  //   while (RS485Busy(rs)) {
  //  }
    // RS485WriteWithError(rs, buf, -1, 0b1);
    // rs->buffer[0] ^= 0b1;

    // while (RS485Busy(rs)) {
    // }
    // RS485Write(rs, buf);

    // while (RS485Busy(rs)) {
    // }
    // RS485Write(rs, buf);
    // while (RS485Busy(rs)) {
    // }
    // RS485WriteWithError(rs, buf, 4,0b00100101);
    
  }
  return 0;
}
