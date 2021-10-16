#include <stdint.h>

typedef uint32_t q16_16_t; 

q16_16_t add(q16_16_t, q16_16_t);
q16_16_t subtract(q16_16_t, q16_16_t);
q16_16_t multiply(q16_16_t, q16_16_t);
q16_16_t divide(q16_16_t, q16_16_t);

q16_16_t tofixed(uint32_t);
q16_16_t partstofixed(uint32_t, uint32_t);

uint32_t extractint(q16_16_t);
uint32_t extractfrac(q16_16_t);