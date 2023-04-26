#ifndef _COUNTER_H_
#define _COUNTER_H_

#include "timer.h"

#include <avr/interrupt.h>

void CounterInit();

uint8_t CounterGetCount();

#endif