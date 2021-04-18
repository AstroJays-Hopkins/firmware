#include "sam.h"
#include "system_samd51.h"

#include "bit_utils.h"

#include "globals.h"
#include "init.h"
#include "pinmap.h"


/**
 * Prereqs:
 *  - None
 * 
 * Hardware Effects:
 *  - Toggles LED pin every second
 * 
 * Side Effects:
 *  - Modifies global variable tick_count on every call
 * 
 * Called when the systick timer underflows (every 1 ms)
 */
void SysTick_Handler(void) {
    ++tick_count;
    if(tick_count == 1000-1) {
        tick_count = 0;
        // Use special "toggle" register
        PORT_REGS->GROUP[LED_BANK].PORT_OUTTGL = 1 << LED_PIN;
    }
}

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
    unsigned int ticks = ms * SystemCoreClock / (1000 * 4);
    while(ticks) --ticks;
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
    PORT_REGS->GROUP[LED2_BANK].PORT_OUTTGL = 1 << LED2_PIN;
    delay(500);
    // Don't use this way as it's a RMW operation
    _INV_BIT(PORT_REGS->GROUP[LED2_BANK].PORT_OUT, LED2_PIN);
    delay(500);
}

int main(void) {
    SystemInit();
    AppInit();
    for(;;) loop();
}
