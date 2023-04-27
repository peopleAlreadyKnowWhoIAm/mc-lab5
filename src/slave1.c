#include "main.h"

int main() {
  MCC* rs = MCCInit(USART0, RS485_BR, MCC_SLAVE,
                    (Pin){.gpio = GPIOD, .pin_number = 2});
  rs->address = 94;
  Crc8InitTable();
  sei();
  char buf[] = "Дуфанець Назар Андрійович ";
  while (1) {
    while (RS485Busy(rs)) {
    }
    RS485Write(rs, buf);

    while (RS485Busy(rs)) {
   }
    RS485WriteWithError(rs, buf, -1, 0b1);
    // rs->buffer[0] ^= 0b1;

    while (RS485Busy(rs)) {
    }
    RS485Write(rs, buf);

    while (RS485Busy(rs)) {
    }
    RS485Write(rs, buf);
    while (RS485Busy(rs)) {
    }
    RS485WriteWithError(rs, buf, 3,0b00100101);
    
  }
  return 0;
}
