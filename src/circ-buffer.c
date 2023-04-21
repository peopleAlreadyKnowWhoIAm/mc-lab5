#include "circ-buffer.h"

inline uint8_t incptr(uint8_t ptr) {
  ptr++;
  if (ptr == BUFFER_SIZE) {
    ptr = 0;
  }
  return ptr;
}
