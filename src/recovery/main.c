#include "sam.h"
#include "system_samd21.h"

#include "bit_utils.h"

#include "globals.h"
#include "init.h"
#include "pinmap.h"

#include "uart.h"

/**
 * Prereqs:
 *  - None
 *
 * Side Effects:
 *  - None
 *
 * Stalls exection for ms milliseconds
 */
void delay(int ms) {
    // Calculation is probably off since I don't recall the cmp/jmp istruction latency
    // Use a systick driven event instead if time accruacy is critical.
    volatile unsigned int ticks = ((SystemCoreClock) * (ms / (1000))) / 3;
    __asm volatile (
        ".syntax unified\n\t"
        "1:\n\t"
            "subs %[ticks], %[ticks], #1\n\t"
            "bne 1b\n\t"
        ".syntax divided\n\t"
        : : [ticks] "l" (ticks)
    );
}

/**
 * Prereqs:
 *  - None
 *
 * Side Effects:
 *  - None
 *
 * Main system loop body, called in main
 */
void loop() {
    // Better way to toggle pin
    //PORT_REGS->GROUP[LED_BANK].PORT_OUTTGL = 1 << LED_PIN;
    /* unsigned ticks = 60000; */
    /* while(ticks) --ticks; */
    transmit_uart(10);
    delay(1000);
    /* // Don't use this way as it's a RMW operation */
    /* _INV_BIT(PORT_REGS->GROUP[LED2_BANK].PORT_OUT, LED2_PIN); */
    /* delay(500); */
}

int main(void) {
    SystemInit();
    app_init();
    for(;;) loop();
}
