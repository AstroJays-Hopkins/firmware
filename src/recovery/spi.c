#include "spi.h"
#include "sam.h"

#define FREF 10000000

void init_spi(int target_baud) {
    SERCOM0_REGS->SPIM.SERCOM_CTRLA =   SERCOM_SPIM_CTRLA_MODE_SPI_MASTER // set SPI mode to master
                                        | SERCOM_SPIM_CTRLA_CPOL(0)
                                        | SERCOM_SPIM_CTRLA_CPHA(0)
                                        | SERCOM_SPIM_CTRLA_FORM_SPI_FRAME
                                        | SERCOM_SPIM_CTRLA_DIPO_PAD1 // what pins?
                                        | SERCOM_SPIM_CTRLA_DOPO_PAD0  // what pins?
                                        | SERCOM_SPIM_CTRLA_DORD_LSB  // this might have to change
                                        ;         
    SERCOM0_REGS->SPIM.SERCOM_CTRLB =   SERCOM_SPIM_CTRLB_CHSIZE_8_BIT; // character size is 8 bits

    SERCOM0_REGS->SPIM.SERCOM_BAUD = (FREF/(2 * target_baud)) - 1; // synchronous mode
    
    // this does not need synchronizing since SPI is disabled
    SERCOM0_REGS->SPIM.SERCOM_CTRLB |= SERCOM_SPIM_CTRLB_RXEN(1); // enable receiver

    SERCOM0_REGS->SPIM.SERCOM_CTRLA |= SERCOM_SPIM_CTRLA_ENABLE(1); // enable SPI

    // synchronize CTRLA_ENABLE
    while(_FLD2VAL(SERCOM_SPIM_SYNCBUSY_ENABLE, SERCOM0_REGS->SPIM.SERCOM_SYNCBUSY));
    

}

#define SS_BANK 0
#define SS_PIN 1 // bank A
void transmit_spi(int data) {
    // set SS to low to start transaction
    //  set direction to 1 for output
    PORT_REGS->GROUP[SS_BANK].PORT_DIR |= 1 << SS_PIN;
    //  set pin to low
    PORT_REGS->GROUP[SS_BANK].PORT_OUT |= 0 << SS_PIN;

    // wait for data to be empty
    while(!_FLD2VAL(SERCOM_SPIM_INTFLAG_DRE, SERCOM0_REGS->SPIM.SERCOM_INTFLAG));

    // transmit
    SERCOM0_REGS->SPIM.SERCOM_DATA = data;

    // wait for trasmit to finish
    while(!_FLD2VAL(SERCOM_SPIM_INTFLAG_TXC, SERCOM0_REGS->SPIM.SERCOM_INTFLAG));
    // then set SS to high
    //  set direction to 1 for output
    PORT_REGS->GROUP[SS_BANK].PORT_DIR |= 1 << SS_PIN;
    //  set pin to high
    PORT_REGS->GROUP[SS_BANK].PORT_OUT |= 1 << SS_PIN;
}

int receive_spi() {

    // wait for receiving to be complete
    while(!_FLD2VAL(SERCOM_SPIM_INTFLAG_RXC, SERCOM0_REGS->SPIM.SERCOM_INTFLAG));

    // check if there is a buffer overflow
    int bufovf = _FLD2VAL(SERCOM_SPIM_STATUS_BUFOVF, SERCOM0_REGS->SPIM.SERCOM_DATA);

    if(bufovf) {
        return -1;
    }

    int data = SERCOM0_REGS->SPIM.SERCOM_DATA;
    
    return data;
}

