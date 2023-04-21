#include "rs485.h"

RS485* RS485InitMaster(UsartMemoryMapping* usart, const uint32_t baudrate, Pin direction_pin) {
  return MCCInit(usart, baudrate, MCC_MASTER,direction_pin);
}

RS485* RS485InitSlave(UsartMemoryMapping* usart, const uint32_t baudrate,
                      const uint8_t address, Pin direction_pin) {
  RS485* rs = MCCInit(usart, baudrate, MCC_SLAVE, direction_pin);
  rs->address = address;
  return rs;
}

int8_t RS485MasterSendAddress(RS485* master, uint8_t address) {
  return MCCMasterSendAddress(master, address);
}

int8_t prepare_text_for_sending(char* target, const char* text){
  uint8_t tx_len = (strlen(text) + 4) / 5 * 6;
  uint8_t tx_indx = tx_len - 1;
  if (tx_len > MCC_BUFFER_SIZE) {
    return -1;
  }
  uint8_t sub_buf[5];
  uint8_t sub_buf_idx = 0;
  while (*text != '\0' || sub_buf_idx != 0) {
    sub_buf[sub_buf_idx] = *text;
    target[tx_indx] = *text;
    tx_indx--;
    sub_buf_idx++;
    if (*text != '\0') {
      text++;
    }
    if (sub_buf_idx == 5) {
      uint8_t crc = Crc8Calculate(sub_buf);
      sub_buf_idx = 0;
      target[tx_indx] = crc;
      tx_indx--;
    }
  }

  return tx_len;
}

int8_t RS485Write(RS485* rs, const char* text) {
  char* tx = MCCGetBuffer(rs);
  int8_t len = prepare_text_for_sending(tx,text);
  if(len < 0 ){
    return -1;
  }

  return MCCWrite(rs, len);
}

int8_t RS485WriteWithError(RS485* rs, const char* text, int8_t error_byte, uint8_t error_mask){

  char* tx = MCCGetBuffer(rs);
  int8_t len = prepare_text_for_sending(tx,text);
  if(len < 0 ){
    return -1;
  }
  uint8_t pos;
  if (error_byte < 0){
    pos = -error_byte - 1;
  } else {
    pos = len-error_byte-1;
  }

  tx[pos] ^= error_mask;

  return MCCWrite(rs, len);
}

int8_t RS485Read(RS485* mcc, char* output_buf) {
  String readed = MCCRead(mcc);
  uint8_t out_indx = 0;
  uint8_t pos = 0;
  if (readed.len == 0) {
    return -1;
  }
  uint8_t sub_buf[6];
  while (pos != readed.len && !mcc->receive_fault) {
    sub_buf[pos % 6] = readed.str[pos];
    if (pos % 6 == 5) {
      mcc->receive_fault = !Crc8Check(sub_buf);
    } else {
      output_buf[out_indx] = readed.str[pos];
      out_indx++;
    }
    pos++;
  }

  if (mcc->receive_fault) {
    output_buf[0] = '\0';
    return -2;
  }

  output_buf[out_indx] = '\0';
  return 0;
}

bool RS485Busy(RS485* rs) {
  return (rs->status == MCC_IDLE_CONNECTED ||
          rs->status == MCC_IDLE_DISCONNECTED)
             ? false
             : true;
}

void RS485Free(RS485* rs) {
  MCCFree(rs);
}