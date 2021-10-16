#include "fpa.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>

uint32_t rand_uint32_t() {
	uint32_t n = 0;
	for (int i = 0; i < 32; i++)
		n = (n << 1) + rand() % 2;
	return n;
}

double todouble(q16_16_t f) {
	return extractint(f) + extractfrac(f) * pow(2.0,-16);
}

void testadd() {
	q16_16_t a, b;

	do {
		a = rand_uint32_t();
		b = rand_uint32_t();
	} while ((uint64_t)a + b > UINT32_MAX);

	assert(todouble(a) + todouble(b) == todouble(add(a,b)));
}

void testsubtract() {
	q16_16_t a, b;

	do {
		a = rand_uint32_t();
		b = rand_uint32_t();
	} while (a < b);

	assert(todouble(a) - todouble(b) == todouble(subtract(a,b)));
}

void testmultiply() {
	q16_16_t a, b;

	do {
		a = rand_uint32_t();
		b = rand_uint32_t();
	} while (todouble(a) * todouble(b) >= UINT16_MAX+1);

	printf("multiply: %f\n", fabs(todouble(a) * todouble(b) - todouble(multiply(a,b))));
}

void testdivide() {
	q16_16_t a = rand_uint32_t();
	q16_16_t b = rand_uint32_t();

	printf("divide: %f\n", fabs(todouble(a) / todouble(b) - todouble(divide(a,b))));
}

int main() {
	time_t t;
	srand(time(&t));

	testadd();
	testsubtract();
	testmultiply();
	testdivide();

	return 0;
}
