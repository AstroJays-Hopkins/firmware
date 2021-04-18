#include "init.h"

#include "sam.h"
#include "system_samd51.h"

#include "bit_utils.h"
#include "pinmap.h"


/**
 * Prereqs:
 *  - None
 * 
 * Side Effects:
 *  - None
 * 
 * Initialize the pins to requried intial states
 */
static inline void PinInit(void) {
    // Set pin directions
    _SET_BIT(PORT_REGS->GROUP[LED_BANK].PORT_DIR, LED_PIN);
    _SET_BIT(PORT_REGS->GROUP[LED2_BANK].PORT_DIR, LED2_PIN);
    // Configure PinMux for digital control
    // pins 4 and 5 are together on mux #2
    PORT_REGS->GROUP[LED_BANK].PORT_PMUX[2] = PORT_PMUX_PMUXE_A
                                              | PORT_PMUX_PMUXO_A;
}

/**
 * Prereqs:
 *  - None
 * 
 * Side Effects:
 *  - None
 * 
 * Initialize the system "tick" timer to generate itnerrupts ever millisecond
 */
static inline void SystickInit(void) {
    SysTick->CTRL = 0;
    SysTick->VAL = 0; // clear counter
    // 1ms interrupts, given by coreclock/interrupts/s
    SysTick->LOAD = SystemCoreClock/1000;
    SysTick->CTRL = _VAL2FLD(SysTick_CTRL_ENABLE, 1)       // enable systick
                    | _VAL2FLD(SysTick_CTRL_TICKINT, 1)    // enable systick exceptions
                    | _VAL2FLD(SysTick_CTRL_CLKSOURCE, 1); // use CPU clock
    NVIC_SetPriority(SysTick_IRQn, 3);        // set timer interrupts to lowest priority
    NVIC_EnableIRQ(SysTick_IRQn); // enable interrupt
}

void AppInit() {
    PinInit();
    SystickInit();
}
