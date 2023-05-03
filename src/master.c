#include "main.h"

int main() {

  MCC* rs = MCCInit(USART1, RS485_BR, MCC_MASTER,
                    (Pin){.gpio = GPIOD, .pin_number = 4});
  Usart* usrt = UsartInit(USART0, RS232_BR);
  CounterInit();
  Crc8InitTable();
  sei();
  char buf[100] = {0};
  uint8_t buf_pos = 0;
  while (1) {
    if (UsartReadAvalaible(usrt)) {
      uint8_t len = UsartRead(usrt, buf);
      if (len % 6 != 0 && len > 6) {
        continue;
      }
      char master_data[6];
      uint8_t master_data_len = Crc8Decode(master_data, buf, 6);
      if (master_data_len == 0) {
        GPIOD->PORT |= 0b10;
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

  }

  return 0;
}
