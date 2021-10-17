#include "fpa.h"

q16_16_t add(q16_16_t a, q16_16_t b) {
	return a + b;
}

q16_16_t subtract(q16_16_t a, q16_16_t b) {
	return a - b;
}

q16_16_t multiply(q16_16_t a, q16_16_t b) {
	return (uint64_t) a * b >> 16;
}

q16_16_t divide(q16_16_t a, q16_16_t b) {
	return ((uint64_t) a << 16) / b;
}

q16_16_t tofpa(uint16_t integer) {
	return (q16_16_t)integer << 16;
}

q16_16_t partstofpa(uint16_t integer, uint16_t fraction) {
	return tofpa(integer) + fraction;
}

uint16_t extractint(q16_16_t fixed) {
	return fixed >> 16;
}

uint16_t extractfrac(q16_16_t fixed) {
	return fixed;
}
