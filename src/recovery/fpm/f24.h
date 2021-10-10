#include <stdint.h>

typedef uint32_t f24_t; 

f24_t add(f24_t, f24_t);
f24_t subtract(f24_t, f24_t);
f24_t multiply(f24_t, f24_t);
f24_t divide(f24_t, f24_t);

f24_t tofixed(uint32_t);

uint32_t extractint(f24_t);
uint32_t extractfrac(f24_t);