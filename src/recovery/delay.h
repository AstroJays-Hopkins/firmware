#ifndef DELAY_H_
#define DELAY_H_

#include <stdint.h>

/**
 * delay in milliseconds
 */
#define DELAY_F_MS 1000UL
/**
 * delay in microseconds
 */
#define DELAY_F_US 1000000UL

/**
 * Prereqs:
 *  - None
 *
 * Side Effects:
 *  - None
 *
 * Stalls exection for time, using freq as the frequency basis (e.g. 1000 for ms
 * based delay).
 *
 * @param time amount of time to wait for
 * @param frew frequency basis
 */

void delay(uint32_t time, const uint32_t freq);

#endif /* DELAY_H_ */
