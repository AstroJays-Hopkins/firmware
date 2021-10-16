#include "fpa.h"
#include <stdio.h>
#include <math.h>

double fptod(q16_16_t f) {
	printf("extractint %d, extractfrac %d\n",extractint(f),extractfrac(f));
	return ((double) extractint(f)) + extractfrac(f) * pow(2.0,-16);
}

int main() {
 	q16_16_t a = partstofixed(0,0x7FFF);
	q16_16_t b = partstofixed(2,0);
	printf("b is %04x\n",b);
	printf("mult: 0x%04x\n", multiply(a,b));
	printf("a*b is %lf", fptod(multiply(a,b)));
 	return 0;
}
