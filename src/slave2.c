#include "main.h"


int main() {
  MCC* rs = MCCInit(USART0, RS485_BR, MCC_SLAVE, (Pin){.gpio = GPIOD, .pin_number = 2});
  rs->address = 38;
  sei();
  char buf[] = "06/02/2004";
  while (1) {
    while (RS485Busy(rs)) {
    }
    RS485Write(rs, buf);

    while (RS485Busy(rs)) {
    }
    RS485Write(rs, buf);

    while (RS485Busy(rs)) {
    }
    // RS485WriteWithError(rs, buf, 0,0b1000);

    // rs->buffer[tx_len-1] ^= 0b1000;
    while (RS485Busy(rs)) {
    }
    // RS485WriteWithError(rs, buf, 1,0b10001000);
    // rs->buffer[tx_len-2] ^= 0b10001000;
    while (RS485Busy(rs)) {
    }
    RS485Write(rs, buf);
    
  }
  return 0;
}

