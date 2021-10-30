#include "sam.h"
#include "system_samd21.h"

#include "bit_utils.h"

#include "delay.h"
#include "globals.h"
#include "init.h"
#include "pinmap.h"
//#include "uart.h"
#include "spi.h"


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
    // setup transmit buffer
    //const uint8_t buf[] = "Hello world!\n";
    //char c;
    
    PORT_REGS->GROUP[LED_BANK].PORT_OUT = 1 << LED_PIN;
    //_CLR_BIT(PORT_REGS->GROUP[SS_BANK].PORT_OUT,SS_PIN);
    //PORT_REGS->GROUP[SS_BANK].PORT_OUT = 1 << SS_PIN;

    transmit_spi('c');

    delay(100, DELAY_F_MS);

    PORT_REGS->GROUP[LED_BANK].PORT_OUT = 0 << LED_PIN;
    //PORT_REGS->GROUP[SS_BANK].PORT_OUT = 0 << SS_PIN;
    //_SET_BIT(PORT_REGS->GROUP[SS_BANK].PORT_OUT,SS_PIN);
    delay(500, DELAY_F_MS);
    
}

int main(void) {
    SystemInit();
    app_init();
    for(;;) loop();
}