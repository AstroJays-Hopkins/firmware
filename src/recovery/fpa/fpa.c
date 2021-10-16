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
	return (uint64_t) a * b / 0x10000;
}

//TODO: implement overflow
q16_16_t divide(q16_16_t a, q16_16_t b) {
	return (uint64_t) a / b * 0x10000;
}

//TODO: implement overflow if integer > 0x8000
q16_16_t inttofixed(uint32_t integer) {
	return integer << 16;
}

q16_16_t partstofixed(uint32_t integer, uint32_t fraction) {
	return inttofixed(integer) + fraction;
}


uint32_t extractint(q16_16_t fixed) {
	return fixed >> 16;
}

uint32_t extractfrac(q16_16_t fixed) {
	return fixed & 0x0000FFFF;
}