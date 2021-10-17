#ifndef FPA_H
#define FPA_H

#include <stdint.h>

// unsigned fixed point arithmetic implementation with 2^-16 precision
typedef uint32_t q16_16_t;

// TODO: implement overflow
q16_16_t add(q16_16_t, q16_16_t);
q16_16_t subtract(q16_16_t, q16_16_t);
q16_16_t multiply(q16_16_t, q16_16_t);
q16_16_t divide(q16_16_t, q16_16_t);

// int to fpa
q16_16_t tofpa(uint16_t);
// int + frac to fpa
q16_16_t partstofpa(uint16_t, uint16_t);

uint16_t extractint(q16_16_t);
uint16_t extractfrac(q16_16_t);

#endif
