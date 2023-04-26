#include "usart.h"

#include <avr/interrupt.h>



static Usart usart0;

void write_byte();

Usart* UsartInit(UsartMemoryMapping* usart, const uint32_t baudrate) {
  // Set baud rate
  uint16_t ubbr = F_CPU/16/ baudrate -1;
  usart->UBRRH = (uint8_t)ubbr >> 8;
  usart->UBRRL = (uint8_t)ubbr;

  // Set data length to 8 bit, no parity, 1 stop bit, asynchronous
  usart->UCSRC = 0b11 << UCSZ00;
  // usart->UCSRB |= _BV(UCSZ02) | _BV(TXB80);

  // Enable rx and tx
  usart->UCSRB |= 1 << RXEN0 | 1 << TXEN0;

  // Enable intterupt on receive complete
  usart->UCSRB |= 1 << RXCIE0;

  usart0.usart_mapping = usart;
  return &usart0;
}

static void read_byte(Usart* usart){
  usart->receive_timestamp = CounterGetCount() +4;
  usart->buffer.rx.buf[usart->buffer.rx.write] = usart->usart_mapping->UDR;
  usart->buffer.rx.write = incptr(usart->buffer.rx.write);

}

#ifdef __AVR_ATmega2560__
ISR(USART0_RX_vect) {
  read_byte(&usart0);
}

ISR(USART0_UDRE_vect){
  write_byte();
}

#endif

void write_byte() {
  if (usart0.buffer.tx.read != usart0.buffer.tx.write) {
    usart0.usart_mapping->UDR = usart0.buffer.tx.buf[usart0.buffer.tx.read];
    usart0.buffer.tx.read = incptr(usart0.buffer.tx.read);
  } else {
    usart0.usart_mapping->UCSRB &= (uint8_t) ~(
        1 << UDRIE0);  // Disable iterrupt when not transmitting data
  }
}

/**
 * @brief write char to buffer to transfer
 *
 *
 * @param data char
 * @return 0 - when written, -1 - when fail
 */
inline int8_t UsartWriteChar(Usart* usart, const char data) {
  uint8_t buf = usart->buffer.tx.write;
  usart->buffer.tx.buf[usart->buffer.tx.write] = data;

  usart->buffer.tx.write = incptr(usart->buffer.tx.write);
  if (usart->buffer.tx.write == usart->buffer.tx.read) {
    usart->buffer.tx.write = buf;
    return -1;
  }

  usart->usart_mapping->UCSRB |= 1 << UDRIE0;  // Enable intterupt for transmitting data
  return 0;
}

int8_t UsartWrite(Usart* usart, const char * string){
  int8_t length = 0;
  while(*string != '\0'){
    if(UsartWriteChar(usart, *string) == -1){
      break;
    }
    length++;
    string++;
  }
  return length;
}

int8_t UsartWriteChars(Usart* usart, const char * string, uint8_t len){
  int8_t length = 0;
  while(length!= len){
    if(UsartWriteChar(usart, *string) == -1){
      break;
    }
    length++;
    string++;
  }
  return length;
}

/**
 * @brief read char from serial buffer
 * 
 *
 * @return next char from buffer. When empty - -1.
 */
uint8_t UsartRead(Usart* usart, char* result) {
  uint8_t result_pos = 0;
  while (usart->buffer.rx.write != usart->buffer.rx.read) {
    result[result_pos] = usart->buffer.rx.buf[usart->buffer.rx.read];
    usart->buffer.rx.read = incptr(usart->buffer.rx.read);
    result_pos++;;
  }
  return result_pos;
}

bool UsartReadAvalaible(Usart* usart){
  if(usart->buffer.rx.write != usart->buffer.rx.read && CounterGetCount() -  usart->receive_timestamp >= 0) {
    return true;
  }
  return false;
}

bool UsartWriteBusy(Usart* usart) {
  return usart->buffer.tx.read != usart->buffer.tx.write;
}