#include "counter.h"

static volatile uint8_t count;
static TimerGP8* timer;
static TimerGP8MemoryMapping t_map= TIMER0;

void CounterInit() {
  timer = TimerGP8Init(&t_map, TimerPrescaller_1024, 0, TOIE);
}

ISR(TIMER0_OVF_vect) {
  count++;
}

uint8_t CounterGetCount() {
  return count;
}