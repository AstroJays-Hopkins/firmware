#include "sam.h"
#include "system_samd21.h"

#include "bit_utils.h"

#include "delay.h"
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
 * Main system loop body, called in main
 */
char i = 'A';

void loop() {
    // setup transmit buffer
    //const uint8_t buf[] = "Hello world!\n";
    //char c;
    
    PORT_REGS->GROUP[LED_BANK].PORT_OUT = 1 << LED_PIN;
    //transmit_uart(++i);
    
    //while ((c = buf[i]) != '\0') {
    //    transmit_uart(c);
    //    delay(5, DELAY_F_US);
    //   ++i;
    //}
    //transmit_uart('a');

    i = receive_uart();
    delay(100, DELAY_F_MS);
    transmit_uart('a');
    PORT_REGS->GROUP[LED_BANK].PORT_OUT = 0 << LED_PIN;
    delay(500, DELAY_F_MS);
    
}

int main(void) {
    SystemInit();
    app_init();
    for(;;) loop();
}
