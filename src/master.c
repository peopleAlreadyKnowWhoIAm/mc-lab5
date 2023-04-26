#include "main.h"

int main() {
  // USART0->UCSRB |= _BV(TXEN0);
  // USART0->UCSRB &= ~_BV(TXEN0);

  MCC* rs = MCCInit(USART1, RS485_BR, MCC_MASTER,
                    (Pin){.gpio = GPIOD, .pin_number = 4});
  Usart* usrt = UsartInit(USART0, RS232_BR);
  CounterInit();
  sei();
  char buf[100] = {0};
  uint8_t buf_pos = 0;
  while (1) {
    if (UsartReadAvalaible(usrt)) {
      uint8_t len = UsartRead(usrt, buf);
      if (len % 6 != 0 && len > 6) {
        continue;
      }
      char master_data[5];
      uint8_t master_data_len = Crc8Decode(master_data, buf, 6);
      if (master_data_len == 0) {
        continue;
      }

      RS485MasterSendAddress(rs, master_data[0]);
      while (RS485Busy(rs)) {
      }
      // Without first 6 bytes which was addressed to the master
      memcpy(MCCGetBuffer(rs), &buf[6], len - 6);
      MCCWrite(rs, len - 6);
      while (RS485Busy(rs)) {
      }
      len = MCCGetDataLength(rs);
      UsartWriteChars(usrt, MCCGetBuffer(rs), len);
      while (UsartWriteBusy(usrt)) {
      }
    }

    // for (uint8_t i = 0; i < 5; i++) {
    //   MCCMasterSendAddress(rs, 94);
    //   while (RS485Busy(rs)) {
    //   }
    //   RS485Write(rs, "a");
    //   while (RS485Busy(rs)) {
    //   }

    //   char* tbuf = MCCGetBuffer(rs);
    //   uint8_t tlen = MCCGetDataLength(rs);
    //   // RS485Read(rs, buf);
    //   memcpy(buf, tbuf, tlen);

    //   RS485MasterSendAddress(rs, 38);
    //   while (RS485Busy(rs)) {
    //   }
    //   RS485Write(rs, "a");
    //   while (RS485Busy(rs)) {
    //   }
    //   char a[100];
    //   // RS485Read(rs, a);
    //   char* sbuf = MCCGetBuffer(rs);
    //   uint8_t slen = MCCGetDataLength(rs);
    //   // tbuf.str[tbuf.len] = '\0';
    //   // strcat(buf, a);
    //   memcpy(&buf[tlen], sbuf, slen);

    //   UsartWriteChars(usrt, buf, slen + tlen);
    //   while (UsartWriteBusy(usrt)) {
    //   }
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

  // while (UsartReadChar(usrt) == -1) {
  // }
  // }
  return 0;
}
