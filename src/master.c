#include "main.h"

int main() {
  // USART0->UCSRB |= _BV(TXEN0);
  // USART0->UCSRB &= ~_BV(TXEN0);

  MCC* rs = MCCInit(USART1, RS485_BR, MCC_MASTER,
                    (Pin){.gpio = GPIOD, .pin_number = 4});
  Usart* usrt = UsartInit(USART0, RS232_BR);
  sei();
  char buf[100] = {0};
  uint8_t buf_pos = 0;
  while (1) {
    char buf = UsartReadChar(usrt);
    if(buf != -1) {
      if(buf_pos == '\n'){
        
      }
      buf[buf_pos]
    }

    // for (uint8_t i = 0; i < 5; i++) {
    //   MCCMasterSendAddress(rs, 94);
    //   while (RS485Busy(rs)) {
    //   }
    //   RS485Write(rs, "a");
    //   while (RS485Busy(rs)) {
    //   }

    //   String tbuf = MCCRead(rs);
    //   // RS485Read(rs, buf);
    //   memcpy(buf, tbuf.str, tbuf.len);

    //   RS485MasterSendAddress(rs, 38);
    //   while (RS485Busy(rs)) {
    //   }
    //   RS485Write(rs, "a");
    //   while (RS485Busy(rs)) {
    //   }
    //   char a[100];
    //   // RS485Read(rs, a);
    //   String sbuf = MCCRead(rs);
    //   // tbuf.str[tbuf.len] = '\0';
    //   // strcat(buf, a);
    //   memcpy(&buf[tbuf.len], sbuf.str, sbuf.len);

    //   UsartWriteChars(usrt, sbuf.str, sbuf.len);
    //   while (UsartWriteBusy(usrt)) {
    //   }
    // }

    while (UsartReadChar(usrt) == -1) {
    }
  }
  return 0;
}
