#ifndef _CIRC_BUFFER_H_
#define _CIRC_BUFFER_H_

#include <stdint.h>

#define BUFFER_SIZE 128

typedef struct CircBuffer {
  volatile char buf[BUFFER_SIZE];
  uint8_t read;
  uint8_t write;
} CircBuffer;

uint8_t incptr(uint8_t ptr);

#endif