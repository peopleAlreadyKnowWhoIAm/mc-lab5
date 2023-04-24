#ifndef _MEMORY_MAPPING_H_
#define _MEMORY_MAPPING_H_

#include <stdint.h>
#include <avr/io.h>

typedef volatile uint8_t io_t;

typedef struct UsartMemoryMapping{
    io_t UCSRA;
    io_t UCSRB;
    io_t UCSRC;
    uint8_t reserved;
    io_t UBRRL;
    io_t UBRRH;
    io_t UDR;
} UsartMemoryMapping;


typedef struct TimerGP8ControlMemoryMapping {
    io_t TCCRA;
    io_t TCCRB;
    io_t TCNT;
    io_t OCRA;
    io_t OCRB;
} TimerGP8ControlMemoryMapping;

typedef struct TimerGP8MemoryMapping {
    TimerGP8ControlMemoryMapping* control;
    io_t* TIMSK;
    io_t* TIFR;
} TimerGP8MemoryMapping;

typedef struct SPIMemoryMapping {
    io_t spcr;
    io_t spsr;
    io_t spdr;
} SPIMemoryMapping;

typedef struct Gpio {
    io_t PIN;
    io_t DDB;
    io_t PORT;
} Gpio;

#define SPI0 ((SPIMemoryMapping*) 0x4c)

#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
#define TIMER0 (TimerGP8MemoryMapping) { \
    .control = (TimerGP8ControlMemoryMapping*) 0x44,\
    .TIMSK = (io_t*) 0x6E,\
    .TIFR = (io_t*) 0x35\
}
#define USART0 ((UsartMemoryMapping*) 0xC0)

#define GPIOB (((Gpio*) 0x23))
#define GPIOC (((Gpio*) 0x26))
#define GPIOD (((Gpio*) 0x29))

#elif defined (__AVR_ATmega2560__)
#define USART0 ((UsartMemoryMapping*) 0xC0)
#define USART1 ((UsartMemoryMapping*) 0xC8)
#define TIMER0 (TimerGP8MemoryMapping) { \
    .control = (TimerGP8ControlMemoryMapping*) 0x44,\
    .TIMSK = (io_t*) 0x6E,\
    .TIFR = (io_t*) 0x35\
}
#define GPIOB (((Gpio*) 0x23))
#define GPIOC (((Gpio*) 0x26))
#define GPIOD (((Gpio*) 0x29))
#define GPIOE (((Gpio*) 0x2C))

#endif

#endif
