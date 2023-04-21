#include "timer.h"

TimerGP8* TimerGP8Init(TimerGP8MemoryMapping* mapping, TimerPrescaller prescaller,
                  TimerGP8ModeIndex mode, TimerInteruptMask inter_mask){
  mapping->control->TCCRA = mode & 0b11;
  mapping->control->TCCRB = (mode & 0b100) << 3 | prescaller;
  *mapping->TIMSK = inter_mask;
  return mapping;
}

inline void TimerGP8SetValue(TimerGP8* timer, uint8_t tcnt) {
  // timer->control->TCNT = tcnt;
  TCNT0 = tcnt;

}

inline void TimerGP8EnableInterupts(TimerGP8* timer, TimerInteruptMask inter_mask) {
  *timer->TIMSK = inter_mask;
}

inline void TimerGP8ResetInteruptsStatus(TimerGP8* timer){
  *timer->TIFR = 0b111;
}
