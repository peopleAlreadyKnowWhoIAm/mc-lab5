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


int8_t RS485Write(RS485* rs, const char* text) {
  char* tx = MCCGetBuffer(rs);
  int8_t len = Crc8Encode(tx,text, strlen(text));
  if(len == 0 ){
    return -1;
  }

  return MCCWrite(rs, len);
}

int8_t RS485WriteBytes(RS485* rs, const uint8_t* data, const uint8_t len){
  char* tx = MCCGetBuffer(rs);
  uint8_t n_len = Crc8Encode(tx,data, len);
  if(n_len == 0 ){
    return -1;
  }

  return MCCWrite(rs, n_len);

}

int8_t RS485WriteWithError(RS485* rs, const char* text, int8_t error_byte, uint8_t error_mask){

  char* tx = MCCGetBuffer(rs);
  int8_t len = Crc8Encode(tx,text, strlen(text));
  if(len == 0 ){
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
  char* buffer = MCCGetBuffer(mcc);
  uint8_t len = MCCGetDataLength(mcc);
  if (mcc->receive_fault) {
    return -2;
  }
  
  if(len == 0){
    return -1;
  }

  if(len<10) {
    GPIOC->PORT |= 0b001;
  }

  uint8_t res_len = Crc8Decode(output_buf, buffer, len);
  if(res_len<7) {
    GPIOC->PORT |= _BV(res_len);
  }
  return res_len;
}

bool RS485Busy(RS485* rs) {
  return MCCBusy(rs);
}

void RS485Free(RS485* rs) {
  MCCFree(rs);
}