#include "multi-cpu-connection.h"

static MCC_internal *mcc_inter;

static volatile uint8_t timer_counter = 0;

static volatile uint8_t timer_end_reception_event = 0;

static void init_transmition(MCC_internal *control) {
  control->direction_pin.gpio->PORT |= _BV(control->direction_pin.pin_number);
  control->mapping->UCSRB |= _BV(TXEN0) | _BV(UDRIE0);
}

static void end_reception(MCC_internal *control) {
  // Clear intterupts
  TimerGP8EnableInterupts(control->timer, 0);

  // Disable rx
  control->mapping->UCSRB &= ~(_BV(RXEN0) | _BV(RXCIE0));

  // Change state
  // if (control->pdata.eararchy == MCC_SLAVE)
  // {
  if (control->pdata.status != MCC_PENDING_ADRESS) {
    control->pdata.status = MCC_IDLE_CONNECTED;
  } else {
    control->pdata.status = MCC_IDLE_DISCONNECTED;
  }
  // }
  // else
  // {
  //   control->pdata.status = MCC_PENDING_ADRESS;
  // }
  // control->pdata.rx[control->rx_pos] = '\0';
  // control->rx_pos++;
}

static TimerGP8MemoryMapping timer_m = TIMER0;

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

  MCC_internal *control = malloc(sizeof(MCC_internal));
  control->b_pos = 0;
  control->mapping = usart;
  control->pdata.eararchy = eararchy;
  control->pdata.address = 0;
  control->pdata.receive_fault = false;
  control->direction_pin = direction_pin;
  control->pdata.status =
      (eararchy == MCC_MASTER) ? MCC_IDLE_DISCONNECTED : MCC_PENDING_ADRESS;
  control->timer = TimerGP8Init(&timer_m, TimerPrescaller_1024, 0, 0);

  control->direction_pin.gpio->DDB |= _BV(control->direction_pin.pin_number);
  control->direction_pin.gpio->PORT &= ~_BV(control->direction_pin.pin_number);

  // May switch in future
  mcc_inter = control;
  return &control->pdata;
}

int8_t MCCMasterSendAddress(MCC *rs, uint8_t address) {
  if ((rs->status != MCC_IDLE_DISCONNECTED && rs->status != MCC_IDLE_CONNECTED) || rs->eararchy != MCC_MASTER) {
    return -1;
  }
  rs->address = address;
  rs->status = MCC_PENDING_ADRESS;
  MCC_internal *control = (MCC_internal *)rs;
  control->direction_pin.gpio->PORT |= _BV(control->direction_pin.pin_number);
  control->mapping->UCSRB |= _BV(TXEN0);
  control->mapping->UCSRB |= _BV(TXB80);
  control->mapping->UDR = rs->address;
  control->mapping->UCSRB |= _BV(TXCIE0);
  return 0;
}

void data_received(MCC_internal *control) {
  // Error in receive
  if (control->mapping->UCSRA & (_BV(FE0) | _BV(DOR0) | _BV(UPE0))) {
    control->pdata.receive_fault = true;
  }
  // Slave got address
  if (control->pdata.eararchy == MCC_SLAVE &&
      control->mapping->UCSRB & _BV(RXB80)) {
    uint8_t address = control->mapping->UDR;
      TimerGP8EnableInterupts(control->timer, 0);

    if (control->pdata.address == address) {
      // Earlier for pause
      // Enable Tx complited intterupt and send confirm signal
      control->mapping->UCSRB |= _BV(TXEN0);

      control->pdata.status = MCC_PENDING_ADRESS;
      
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
      control->pdata.status = MCC_PENDING_ADRESS;
    }
  } else {
    // Rezero timer
    timer_end_reception_event = timer_counter + 4;

    uint8_t buf = control->mapping->UDR;

    if (control->pdata.status == MCC_LISTENING) {
      control->pdata.status = MCC_RECEIVING;
      // Start timer
      TimerGP8EnableInterupts(control->timer, TOIE);
    }

    if (control->pdata.eararchy == MCC_MASTER &&
        control->pdata.status == MCC_PENDING_ADRESS) {
        //Disable reception
        control->mapping->UCSRB &= ~(_BV(RXEN0) | _BV(RXCIE0));
      // Adress set right
      TimerGP8EnableInterupts(control->timer, 0);
      control->pdata.status = MCC_IDLE_CONNECTED;
      if (buf == control->pdata.address) {
        // Coneection was established

        control->pdata.status = MCC_IDLE_CONNECTED;
      } else {
        control->pdata.status = MCC_IDLE_CONNECTED;
        // Resend address
        // MCCMasterSendAddress(&control->pdata, control->pdata.address);
      }
    } else {
      // End of receive frame
      // if (buf == '\0') {
      //   end_reception(control);
      // } else {
      control->pdata.buffer[control->b_pos] = buf;
      control->b_pos++;
      // }
    }
  }
}

void data_tx_empty(MCC_internal *control) {
  // Send data
  if (control->b_pos == 0) {
    // Write ending \0
    control->mapping->UCSRB &=
        ~(1 << UDRIE0);  // Disable iterrupt when not transmitting data
    control->mapping->UCSRB |= _BV(TXCIE0);  // Enable complition intterupt
  } else {
    control->b_pos--;
    control->mapping->UDR = control->pdata.buffer[control->b_pos];
  }
}

void data_tx_completed(MCC_internal *control) {
  // disable interrupt and clear 9th bit and disable transmitter
  control->mapping->UCSRB &= ~(_BV(TXCIE0) | _BV(TXB80) | _BV(TXEN0));

  control->direction_pin.gpio->PORT &= ~_BV(control->direction_pin.pin_number);
  // Prepare to listen
  // Enable rx and ex intterupt
  control->mapping->UCSRB |= _BV(RXEN0) | _BV(RXCIE0);

  // Clear buffer
  control->b_pos = 0;

  // Start listening
  if (control->pdata.eararchy == MCC_MASTER &&
      control->pdata.status == MCC_PENDING_ADRESS) {
    timer_end_reception_event = timer_counter + 2;
    // Start timeout
    // TimerGP8EnableInterupts(control->timer, TOIE);
  } else {
    control->pdata.status = MCC_LISTENING;
  }
}



ISR(
#ifdef __AVR_ATmega2560__
    USART1_RX_vect
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
    USART_RX_vect
#endif
) {
  data_received(mcc_inter);
}

ISR(
#ifdef __AVR_ATmega2560__
    USART1_UDRE_vect
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
    USART_UDRE_vect
#endif

) {
  data_tx_empty(mcc_inter);
}

ISR(
#ifdef __AVR_ATmega2560__
    USART1_TX_vect
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
    USART_TX_vect
#endif

) {
  data_tx_completed(mcc_inter);
}

ISR(TIMER0_OVF_vect) {
  timer_counter++;
  if (timer_counter == timer_end_reception_event) {
    end_reception(mcc_inter);
  }
}

int8_t MCCWrite(MCC *rs485, uint8_t len) {
  if (rs485->status != MCC_IDLE_CONNECTED) {
    return -1;
  }
  rs485->status = MCC_SENDING;

  MCC_internal *mcc = (MCC_internal *)rs485;
  mcc->b_pos = len;
  init_transmition(mcc);
  // data_tx_empty(control);
  return 0;
}

char *MCCGetBuffer(MCC *mcc) {
  if (mcc->status != MCC_IDLE_CONNECTED) {
    return 0;
  }
  return mcc->buffer;
}

String MCCRead(MCC *rs485) {
  if (rs485->status != MCC_IDLE_CONNECTED) {
    return (String){.len = 0};
  }
  return (String){.len = ((MCC_internal *)rs485)->b_pos, .str = rs485->buffer};
}

void MCCFree(MCC *rs485) { free(rs485); }