#include "init.h"

#include "sam.h"
#include "system_samd21.h"

#include "bit_utils.h"
#include "pinmap.h"
#include "clock.h"
//#include "spi.h"
#include "spi.h"

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
    PORT_REGS->GROUP[LED_BANK].PORT_WRCONFIG = PORT_WRCONFIG_HWSEL(1) // upper half of pins
                                        | PORT_WRCONFIG_WRPINCFG(1)   // write non-mux configs
                                        | PORT_WRCONFIG_WRPMUX(1)     // write mux configs
                                        | PORT_WRCONFIG_DRVSTR(1)     // extra current
                                        | PORT_WRCONFIG_PULLEN(0)     // no pull up/down
                                        | PORT_WRCONFIG_INEN(0)       // no input buff
                                        | PORT_WRCONFIG_PMUXEN(1)     // don't mux i.e. digital ctrl
                                        | PORT_WRCONFIG_PMUX(0x7)     // function h
                                        | PORT_WRCONFIG_PINMASK(0x1 << 0);


    //PORT_REGS->GROUP[0].PORT_PINCFG[10] = PORT_PINCFG_PMUXEN(1);
    //PORT_REGS->GROUP[0].PORT_PINCFG[11] = PORT_PINCFG_PMUXEN(1);
    //PORT_REGS->GROUP[0].PORT_PMUX[5] = PORT_PMUX_PMUXO_C | PORT_PMUX_PMUXE_C;
    
    //PORT_REGS->GROUP[0].PORT_PINCFG[15] = PORT_PINCFG_PMUXEN(1);
    //PORT_REGS->GROUP[0].PORT_PMUX[7] = PORT_PMUX_PMUXO_H;
    //PORT_REGS->GROUP[0].PORT_PINCFG[16] = PORT_PINCFG_PMUXEN(1);
    //PORT_REGS->GROUP[0].PORT_PMUX[8] = PORT_PMUX_PMUXE_H;
    PORT_REGS->GROUP[LED_BANK].PORT_DIR |= 0x1 << LED_PIN;
    /*
    SERCOM_USART_INT_CTRLA_RXPO(0x3)          // recieve on pad 3
    SERCOM_USART_INT_CTRLA_TXPO(0x1) */


    // SPI pins all function D
    // DOPO PAD2 MOSI pin 19 PB10 group 1
    // DOPO PAD3 SCK pin 20 PB11 group 1
    // DOPO PAD1 SS pin 8 PB09 group 1 not pin muxed ignore
    // DIPO PAD0 MISO pin 21 PA12 group 0

    //  set direction to 1 for output SS
    PORT_REGS->GROUP[SS_BANK].PORT_DIR |= 0x1 << SS_PIN;

    //PORT_REGS->GROUP[1].PORT_PINCFG[9] = PORT_PINCFG_PMUXEN(1);
    //PORT_REGS->GROUP[1].PORT_PINCFG[19] = PORT_PINCFG_PMUXEN(1);
    //PORT_REGS->GROUP[1].PORT_PINCFG[20] = PORT_PINCFG_PMUXEN(1);
    //PORT_REGS->GROUP[0].PORT_PINCFG[21] = PORT_PINCFG_PMUXEN(1);

    // SERCOM1 pad0 PA16 pin 25 MISO 11 on the board
    //         pad1 PA17 pin 26 not connected 
    //         pad2 PA18 pin 27 MOSI 10 on the board
    //         pad3 PA19 pin 28 SCK 12 on the board
    //         soft PB09 pin 8 SS a2 on the board

    PORT_REGS->GROUP[0].PORT_PINCFG[16] = PORT_PINCFG_PMUXEN(1);
    PORT_REGS->GROUP[0].PORT_PINCFG[17] = PORT_PINCFG_PMUXEN(1);
    PORT_REGS->GROUP[0].PORT_PINCFG[18] = PORT_PINCFG_PMUXEN(1);
    PORT_REGS->GROUP[0].PORT_PINCFG[19] = PORT_PINCFG_PMUXEN(1);
    PORT_REGS->GROUP[0].PORT_PMUX[8] = PORT_PMUX_PMUXE_C | PORT_PMUX_PMUXO_C;
    PORT_REGS->GROUP[0].PORT_PMUX[9] = PORT_PMUX_PMUXE_C | PORT_PMUX_PMUXO_C;
 
    //PORT_REGS->GROUP[1].PORT_PMUX[9] = PORT_PMUX_PMUXO_D;
    //PORT_REGS->GROUP[1].PORT_PMUX[10] = PORT_PMUX_PMUXE_D;
    //PORT_REGS->GROUP[0].PORT_PMUX[10] = PORT_PMUX_PMUXO_D;
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
    //init_uart(115200);
    init_spi(100000);
}