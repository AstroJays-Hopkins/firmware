/**
 * Global variable implementation file. Execise care in use!
 */

#include <stdint.h>


volatile uint32_t system_ticks = 0;
volatile uint32_t sercom_clock = 1000000;