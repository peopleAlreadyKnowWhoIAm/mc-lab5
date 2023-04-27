#include "main.h"

int main() {
  MCC* rs = MCCInit(USART0, RS485_BR, MCC_SLAVE,
                    (Pin){.gpio = GPIOD, .pin_number = 2});
  rs->address = 94;
  GPIOB->DDB |= _BV(PB3) | _BV(PB5) | _BV(PB2);  // SPI
  SPI* spi = SPIInit(SPI0, SPI_master, SpiClockDiv4);
  Pin pin_clock = {.gpio = GPIOB, .pin_number = 2};
  PCF2123Reset(spi, &pin_clock);
  Crc8InitTable();
  CounterInit();
  sei();
  char buf[100];
  while (1) {
    while (RS485Busy(rs)) {
    }
    RS485Read(rs, buf);
    switch (buf[0]) {
      case 'g':
        PCF2123ReadTimeStart(spi, &pin_clock);
        while (SPIBusy(spi)) {
        }
        PCF2123ReadTimeFinish(spi, (uint8_t*)buf);
        RS485WriteBytes(rs, buf,7);
        break;
      case 's':
        PCF2123WriteTime(spi, (uint8_t*)&buf[1], &pin_clock);
        while (SPIBusy(spi)) {
        }
        RS485Write(rs, "done");
        break;
    }
  }
  return 0;
}
