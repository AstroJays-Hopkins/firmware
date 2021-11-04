#ifndef PRINTF_H_
#define PRINTF_H_

#include <stdint.h>

void init_spi(uint32_t target_baud);

uint8_t transmit_spi(uint8_t data);

uint8_t receive_spi();

#endif