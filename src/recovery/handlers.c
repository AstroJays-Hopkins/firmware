/**
 * ISR handler implementation file
 */
#include "handlers.h"
#include "globals.h"

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
    ++system_ticks;
    /* if(tick_count == 1000-1) { */
        /* tick_count = 0; */
        /* // Use special "toggle" register */
        /* PORT_REGS->GROUP[LED_BANK].PORT_OUTTGL = 1 << LED_PIN; */
    /* } */
}