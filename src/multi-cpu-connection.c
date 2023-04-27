#include "multi-cpu-connection.h"

static MCC *mcc_usart0;


static void end_reception(MCC *control) {
  // Clear intterupts
  TimerGP8EnableInterupts(control->timer, 0);

  control->mapping->UCSRB &= ~(_BV(RXEN0) | _BV(RXCIE0));
  if (control->status != MCC_PENDING_ADRESS) {
    control->status = MCC_IDLE_CONNECTED;
  } else {
    control->status = MCC_IDLE_DISCONNECTED;
  }
  control->buffer_data_size = control->buffer_pos;
}

MCC *MCCInit(UsartMemoryMapping *usart, const uint32_t baudrate,
             MCCEararchy eararchy, Pin direction_pin) {
  uint16_t ubbr = F_CPU / 16 / baudrate - 1;
  usart->UBRRH = (uint8_t)ubbr >> 8;
  usart->UBRRL = (uint8_t)ubbr;

  // 9-bit, odd parity, 2 stop bits
  usart->UCSRC = (0b11 << UPM00) | _BV(USBS0) | (0b11 << UCSZ00);
  usart->UCSRB = _BV(UCSZ02);
  if (eararchy == MCC_SLAVE) {
    // Multiproccessor mode
    usart->UCSRA |= _BV(MPCM0);
    // Enable rx and rx intterupt
    usart->UCSRB |= _BV(RXEN0) | _BV(RXCIE0);
  }

  MCC *control = malloc(sizeof(MCC));
  control->buffer_pos = 0;
  control->mapping = usart;
  control->eararchy = eararchy;
  control->address = 0;
  control->receive_fault = false;
  control->direction_pin = direction_pin;
  control->status =
      (eararchy == MCC_MASTER) ? MCC_IDLE_DISCONNECTED : MCC_PENDING_ADRESS;

  control->direction_pin.gpio->DDB |= _BV(control->direction_pin.pin_number);
  control->direction_pin.gpio->PORT &= ~_BV(control->direction_pin.pin_number);

  // May switch in future
  mcc_usart0 = control;
  return control;
}

int8_t MCCMasterSendAddress(MCC *rs, uint8_t address) {
  if ((rs->status != MCC_IDLE_DISCONNECTED &&
       rs->status != MCC_IDLE_CONNECTED) ||
      rs->eararchy != MCC_MASTER) {
    return -1;
  }
  rs->address = address;
  rs->status = MCC_PENDING_ADRESS;
  rs->direction_pin.gpio->PORT |= _BV(rs->direction_pin.pin_number);
  rs->mapping->UCSRB |= _BV(TXEN0);
  rs->mapping->UCSRB |= _BV(TXB80);
  rs->mapping->UDR = rs->address;
  rs->mapping->UCSRB |= _BV(TXCIE0);
  return 0;
}

void data_received(MCC *control) {
  // Error in receive
  if (control->mapping->UCSRA & (_BV(FE0) | _BV(DOR0) | _BV(UPE0))) {
    control->receive_fault = true;
  }
  // Slave got address
  if (control->eararchy == MCC_SLAVE &&
      control->mapping->UCSRB & _BV(RXB80)) {
    uint8_t address = control->mapping->UDR;

    if (control->address == address) {
      // Earlier for pause
      // Enable Tx complited intterupt and send confirm signal
      control->mapping->UCSRB |= _BV(TXEN0);

      control->status = MCC_PENDING_ADRESS;

      control->direction_pin.gpio->PORT |=
          _BV(control->direction_pin.pin_number);

      control->mapping->UCSRA &= ~_BV(MPCM0);

      // Disable rx and rx intterupt
      control->mapping->UCSRB &= ~(_BV(RXEN0) | _BV(RXCIE0));
      control->mapping->UDR = address;
      control->mapping->UCSRB |= _BV(TXCIE0);
    } else {
      // Not this address
      // Ifcontr has been data trasfer -> enable MPCM
      control->mapping->UCSRA |= _BV(MPCM0);
      control->status = MCC_PENDING_ADRESS;
    }
  } else {
    // Rezero timer
    control->receive_timestamp = CounterGetCount();

    uint8_t buf = control->mapping->UDR;

    if (control->status == MCC_LISTENING) {
      control->status = MCC_RECEIVING;
      // Start timer
      // TimerGP8EnableInterupts(control->timer, TOIE);
    }

    if (control->eararchy == MCC_MASTER &&
        control->status == MCC_PENDING_ADRESS) {
      // Disable reception
      control->mapping->UCSRB &= ~(_BV(RXEN0) | _BV(RXCIE0));
      // Adress set right
      control->status = MCC_IDLE_CONNECTED;
      if (buf == control->address) {
        // Coneection was established

        control->status = MCC_IDLE_CONNECTED;
      } else {
        control->status = MCC_IDLE_CONNECTED;
        // Resend address
        MCCMasterSendAddress(control, control->address);
      }
    } else {
      control->buffer[control->buffer_pos] = buf;
      control->buffer_pos++;
    }
  }
}

void data_tx_empty(MCC *control) {
  // Send data
  if (control->buffer_pos == control->buffer_data_size) {
    control->mapping->UCSRB &=
        ~(1 << UDRIE0);  // Disable iterrupt when not transmitting data
    control->mapping->UCSRB |= _BV(TXCIE0);  // Enable complition intterupt
  } else {
    control->mapping->UDR = control->buffer[control->buffer_pos];
    control->buffer_pos++;
  }
}

void data_tx_completed(MCC *control) {
  // disable interrupt and clear 9th bit and disable transmitter
  control->mapping->UCSRB &= ~(_BV(TXCIE0) | _BV(TXB80) | _BV(TXEN0));

  control->direction_pin.gpio->PORT &= ~_BV(control->direction_pin.pin_number);
  // Prepare to listen
  // Enable rx and ex intterupt
  control->mapping->UCSRB |= _BV(RXEN0) | _BV(RXCIE0);

  control->buffer_pos = 0;

  // Start listening
  if (control->eararchy == MCC_MASTER &&
      control->status == MCC_PENDING_ADRESS) {
    // timer_end_reception_event = timer_counter + 2;
    // Start timeout
    // TimerGP8EnableInterupts(control->timer, TOIE);
  } else {
    control->status = MCC_LISTENING;
  }
}

ISR(
#ifdef __AVR_ATmega2560__
    USART1_RX_vect
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
    USART_RX_vect
#endif
) {
  data_received(mcc_usart0);
}

ISR(
#ifdef __AVR_ATmega2560__
    USART1_UDRE_vect
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
    USART_UDRE_vect
#endif

) {
  data_tx_empty(mcc_usart0);
}

ISR(
#ifdef __AVR_ATmega2560__
    USART1_TX_vect
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
    USART_TX_vect
#endif

) {
  data_tx_completed(mcc_usart0);
}

int8_t MCCWrite(MCC *rs485, uint8_t len) {
  if (rs485->status != MCC_IDLE_CONNECTED) {
    return -1;
  }
  rs485->status = MCC_SENDING;

  rs485->buffer_data_size = len;
  rs485->buffer_pos = 0;
  // data_tx_empty(control);
  rs485->direction_pin.gpio->PORT |= _BV(rs485->direction_pin.pin_number);
  rs485->mapping->UCSRB |= _BV(TXEN0) | _BV(UDRIE0);
  return 0;
}

char *MCCGetBuffer(MCC *mcc) {
  if (mcc->status != MCC_IDLE_CONNECTED) {
    return 0;
  }
  return mcc->buffer;
}

uint8_t MCCGetDataLength(MCC *rs485) {
  if (rs485->status != MCC_IDLE_CONNECTED) {
    return 0;
  }
  return rs485->buffer_data_size;
}

bool MCCBusy(MCC* mcc){
  // Recheck receiving completion
  if(mcc->status == MCC_RECEIVING && CounterGetCount() - mcc->receive_timestamp >= 4){
    end_reception(mcc);
  }
  return !(mcc->status == MCC_IDLE_CONNECTED ||
          mcc->status == MCC_IDLE_DISCONNECTED);

}

void MCCFree(MCC *rs485) { free(rs485); }