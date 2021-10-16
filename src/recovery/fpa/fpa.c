#include "fpa.h"

//TODO: implement overflow
q16_16_t add(q16_16_t a, q16_16_t b) {
	return a + b;
}

//TODO: implement overflow
//breaks if b > a
q16_16_t subtract(q16_16_t a, q16_16_t b) {
	return a - b;
}

//TODO: implement overflow
q16_16_t multiply(q16_16_t a, q16_16_t b) {
	return (uint64_t) a * b >> 16;
}

//TODO: implement overflow
q16_16_t divide(q16_16_t a, q16_16_t b) {
	return ((uint64_t) a << 16) / b;
}

//TODO: implement overflow if integer > 0x8000
q16_16_t tofpa(uint16_t integer) {
	return integer << 16;
}

q16_16_t partstofpa(uint16_t integer, uint16_t fraction) {
	return tofpa(integer) + fraction;
}

uint16_t extractint(q16_16_t fixed) {
	return fixed >> 16;
}

uint16_t extractfrac(q16_16_t fixed) {
	return fixed & 0x0000FFFF;
}
