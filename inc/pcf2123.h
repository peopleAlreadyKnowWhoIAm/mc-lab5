#ifndef _PCF2123_H_
#define _PCF2123_H_

#include "spi.h"
#include <stdlib.h>
#include <string.h>

int8_t PCF2123Reset(SPI* spi, Pin* chip_enable);

int8_t PCF2123WriteTime(SPI* spi, uint8_t* time, Pin*chip_enable);

int8_t PCF2123ReadTimeStart(SPI* spi, Pin* chip_enable);
int8_t PCF2123ReadTimeFinish(SPI* spi, uint8_t* buffer);


#endif