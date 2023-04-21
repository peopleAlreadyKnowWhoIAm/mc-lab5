#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdint.h>

#include "memory-mapping.h"

typedef struct Pin{
  Gpio* gpio;
  uint8_t pin_number;
} Pin;


#endif