#include "init.h"

#include "sam.h"
#include "system_samd21.h"

#include "bit_utils.h"
#include "pinmap.h"
#include "clock.h"
#include "uart.h"

/**
 * Prereqs:
 *  - None
 *
 * Side Effects:
 *  - None
 *
 * Initialize the pins to requried intial states
 */
static inline void pin_init(void) {
    // Set pin directions
    _SET_BIT(PORT_REGS->GROUP[LED_BANK].PORT_DIR, LED_PIN);
    // set pinmux
    PORT_REGS->GROUP[LED_BANK].PORT_WRCONFIG = PORT_WRCONFIG_HWSEL(0) // lower half of pins
                                        | PORT_WRCONFIG_WRPINCFG(1)   // write non-mux configs
                                        | PORT_WRCONFIG_WRPMUX(1)     // write mux configs
                                        | PORT_WRCONFIG_DRVSTR(1)     // extra current
                                        | PORT_WRCONFIG_PULLEN(0)     // no pull up/down
                                        | PORT_WRCONFIG_INEN(0)       // no input buff
                                        | PORT_WRCONFIG_PMUXEN(0)     // don't mux i.e. digital ctrl
                                        | PORT_WRCONFIG_PINMASK(0x1 << LED_PIN);
    /* PORT_REGS->GROUP[LED_BANK].PORT_WRCONFIG = PORT_WRCONFIG_HWSEL(0) // lower half of pins */
                                        /* | PORT_WRCONFIG_WRPINCFG(1)   // write non-mux configs */
                                        /* | PORT_WRCONFIG_WRPMUX(1)     // write mux configs */
                                        /* | PORT_WRCONFIG_DRVSTR(1)     // extra current */
                                        /* | PORT_WRCONFIG_PULLEN(0)     // no pull up/down */
                                        /* | PORT_WRCONFIG_INEN(0)       // no input buff */
                                        /* | PORT_WRCONFIG_PMUXEN(1)     // don't mux i.e. digital ctrl */
                                        /* | PORT_WRCONFIG_PMUX(0x2)     // function c (PA10-PA11) */
                                        /* | PORT_WRCONFIG_PINMASK(0x3 << 10); */
    PORT_REGS->GROUP[LED_BANK].PORT_WRCONFIG = PORT_WRCONFIG_HWSEL(1) // upper half of pins
                                        | PORT_WRCONFIG_WRPINCFG(1)   // write non-mux configs
                                        | PORT_WRCONFIG_WRPMUX(1)     // write mux configs
                                        | PORT_WRCONFIG_DRVSTR(1)     // extra current
                                        | PORT_WRCONFIG_PULLEN(0)     // no pull up/down
                                        | PORT_WRCONFIG_INEN(0)       // no input buff
                                        | PORT_WRCONFIG_PMUXEN(1)     // don't mux i.e. digital ctrl
                                        | PORT_WRCONFIG_PMUX(0x7)     // function h
                                        | PORT_WRCONFIG_PINMASK(0x1 << 0);
    /* PORT_REGS->GROUP[LED_BANK].PORT_WRCONFIG = PORT_WRCONFIG_HWSEL(0) // lower half of pins */
                                        /* | PORT_WRCONFIG_WRPINCFG(1)   // write non-mux configs */
                                        /* | PORT_WRCONFIG_WRPMUX(1)     // write mux configs */
                                        /* | PORT_WRCONFIG_DRVSTR(1)     // extra current */
                                        /* | PORT_WRCONFIG_PULLEN(0)     // no pull up/down */
                                        /* | PORT_WRCONFIG_INEN(0)       // no input buff */
                                        /* | PORT_WRCONFIG_PMUXEN(1)     // don't mux i.e. digital ctrl */
                                        /* | PORT_WRCONFIG_PMUX(0x7)     // function h */
                                        /* | PORT_WRCONFIG_PINMASK(0x1 << 15); */
    PORT_REGS->GROUP[0].PORT_PINCFG[10] = PORT_PINCFG_PMUXEN(1);
    PORT_REGS->GROUP[0].PORT_PINCFG[11] = PORT_PINCFG_PMUXEN(1);
    PORT_REGS->GROUP[0].PORT_PMUX[5] = PORT_PMUX_PMUXO_C | PORT_PMUX_PMUXE_C;

    PORT_REGS->GROUP[0].PORT_PINCFG[15] = PORT_PINCFG_PMUXEN(1);
    PORT_REGS->GROUP[0].PORT_PMUX[7] = PORT_PMUX_PMUXO_H;
    PORT_REGS->GROUP[0].PORT_PINCFG[16] = PORT_PINCFG_PMUXEN(1);
    PORT_REGS->GROUP[0].PORT_PMUX[8] = PORT_PMUX_PMUXE_H;
    // PORT_REGS->GROUP[LED_BANK].PORT_DIR |= 0x1 << LED_PIN;
}

/**
 * Prereqs:
 *  - None
 *
 * Side Effects:
 *  - None
 *
 * Initialize the system "tick" timer to generate interrupts
 */
static inline void systick_init(unsigned int_per_s) {
    SysTick->CTRL = 0; // disable systick timer before attemping configuration
    SysTick->VAL = 0; // clear counter
    // clock cycles between interrupts, given by coreclock/(interrupts_per_s)
    SysTick->LOAD = SystemCoreClock/int_per_s;
    SysTick->CTRL = _VAL2FLD(SysTick_CTRL_ENABLE, 1)       // enable systick timer
                    | _VAL2FLD(SysTick_CTRL_TICKINT, 1)    // enable systick interrupts
                    | _VAL2FLD(SysTick_CTRL_CLKSOURCE, 1); // use CPU clock
    NVIC_SetPriority(SysTick_IRQn, 3);        // set timer interrupts to lowest priority
    NVIC_EnableIRQ(SysTick_IRQn); // enable interrupt
}

void app_init() {
    pin_init();
    core_clock_init();
    sercom_clock_init();
    systick_init(1000);
    init_uart(115200);
}
