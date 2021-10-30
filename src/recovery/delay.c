#include "delay.h"

#include "system_samd21.h"

void delay(uint32_t time, const uint32_t freq) {
    // Calculation is probably off since I don't recall the cmp/jmp istruction latency
    // Use a systick driven event instead if time accruacy is critical.
    volatile unsigned int ticks = ((SystemCoreClock/freq) * time ) / 3;
    __asm volatile (
        ".syntax unified\n\t"
        "1:\n\t"
            "subs %[ticks], %[ticks], #1\n\t"
            "bne 1b\n\t"
        ".syntax divided\n\t"
        : : [ticks] "l" (ticks)
    );
}