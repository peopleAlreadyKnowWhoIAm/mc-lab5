/**
 * @file timer.h
 * @brief File for managing timers
 * Currently manage timer1 only in CTC mode
 * @version 0.1
 * @date 2023-03-19
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef _TIMER_H_
#define _TIMER_H_

#include <avr/io.h>
#include <stdlib.h>

#include "memory-mapping.h"

typedef uint8_t TimerPrescaller;
typedef uint8_t TimerGP8ModeIndex;
typedef uint8_t TimerInteruptMask;
enum {
  TimerPrescaller_off = 0b0,
  TimerPrescaller_1 = 0b001,
  TimerPrescaller_8 = 0b010,
  TimerPrescaller_64 = 0b011,
  TimerPrescaller_256 = 0b100,
  TimerPrescaller_1024 = 0b101,
  TimerPrescaller_external_falling = 0b110,
  TimerPrescaller_external_rising = 0b111,

};

#define OCIEB 0b100
#define OCIEA 0b010
#define TOIE  0b001

typedef TimerGP8MemoryMapping TimerGP8;


TimerGP8* TimerGP8Init(TimerGP8MemoryMapping* mapping, TimerPrescaller prescaller,
                  TimerGP8ModeIndex mode, TimerInteruptMask inter_mask);

void TimerGP8SetValue(TimerGP8* timer, uint8_t tcnt);
void TimerGP8EnableInterupts(TimerGP8* timer, TimerInteruptMask inter_mask);
void TimerGP8ResetInteruptsStatus(TimerGP8* timer);

#endif