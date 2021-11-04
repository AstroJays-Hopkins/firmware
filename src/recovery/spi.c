#include "spi.h"
#include "sam.h"
#include "pinmap.h"
#include "bit_utils.h"


#define FREF 2000000

void init_spi(uint32_t target_baud) {
    // reset SPI
    SERCOM1_REGS->SPIM.SERCOM_CTRLA = SERCOM_SPIM_CTRLA_SWRST(0x1);
    while(_FLD2VAL(SERCOM_SPIM_CTRLA_SWRST,SERCOM1_REGS->SPIM.SERCOM_CTRLA)
       && _FLD2VAL(SERCOM_SPIM_SYNCBUSY_SWRST,SERCOM1_REGS->SPIM.SERCOM_CTRLB));

    PM_REGS->PM_APBCMASK |= PM_APBCMASK_SERCOM1(0x1);
    SERCOM1_REGS->SPIM.SERCOM_CTRLA =   SERCOM_SPIM_CTRLA_MODE_SPI_MASTER // set SPI mode to master
                                        | SERCOM_SPIM_CTRLA_CPOL(0)
                                        | SERCOM_SPIM_CTRLA_CPHA(0)
                                        | SERCOM_SPIM_CTRLA_FORM(0x0)
                                        | SERCOM_SPIM_CTRLA_DIPO(0x0) // what pins?
                                        | SERCOM_SPIM_CTRLA_DOPO(0x1)  // what pins?
                                        | SERCOM_SPIM_CTRLA_DORD_MSB  // this might have to change
                                        | SERCOM_SPIM_CTRLA_RUNSTDBY(0x1)
                                        ;
    // character size is 8 bits
    SERCOM1_REGS->SPIM.SERCOM_CTRLB =   SERCOM_SPIM_CTRLB_CHSIZE_8_BIT;
    // we will control the CS pin
    SERCOM1_REGS->SPIM.SERCOM_CTRLB = SERCOM_SPIM_CTRLB_MSSEN(0);
    // synchronous formula, but for now hardcode baud rate
    SERCOM1_REGS->SPIM.SERCOM_BAUD = 9;//(FREF/(2 * target_baud)) - 1;
    // enable SPI
    SERCOM1_REGS->SPIM.SERCOM_CTRLA |= SERCOM_SPIM_CTRLA_ENABLE(1);
    
    // synchronize CTRLA_ENABLE
    while(_FLD2VAL(SERCOM_SPIM_SYNCBUSY_ENABLE, SERCOM1_REGS->SPIM.SERCOM_SYNCBUSY));
    // synchronize CTRLB ?
    while(_FLD2VAL(SERCOM_SPIM_SYNCBUSY_CTRLB, SERCOM1_REGS->SPIM.SERCOM_SYNCBUSY));
    
    // initialize and enable SPI interrupts
    //NVIC_SetPriority(SERCOM1_IRQn, 3);       
    //NVIC_EnableIRQ(SERCOM1_IRQn); // enable interrupt

    // enable Receive Complete Interrupt
    //SERCOM1_REGS->SPIM.SERCOM_INTENSET |= SERCOM_SPIM_INTENSET_RXC(0x1);

    // start CS high
    PORT_REGS->GROUP[SS_BANK].PORT_OUTSET = 1 << SS_PIN;
}


void SERCOM1_Handler() {
    // check SPI RXC flag
    // read data
    //clear SPI RXC flagW
}



uint8_t transmit_spi(uint8_t data) {
    // set SS to low to start transaction

    //  set pin to low
    PORT_REGS->GROUP[SS_BANK].PORT_OUTCLR = 1 << SS_PIN;

    // delay?

    // wait for data to be empty
    while(!_FLD2VAL(SERCOM_SPIM_INTFLAG_DRE, SERCOM1_REGS->SPIM.SERCOM_INTFLAG));
    
    // transmit
    SERCOM1_REGS->SPIM.SERCOM_DATA = data;

    // wait for trasmit to finish
    while(!_FLD2VAL(SERCOM_SPIM_INTFLAG_RXC, SERCOM1_REGS->SPIM.SERCOM_INTFLAG));

    // read back

    return SERCOM1_REGS->SPIM.SERCOM_DATA;

    // then set SS to high
    //  set pin to high
    PORT_REGS->GROUP[SS_BANK].PORT_OUTSET = 1 << SS_PIN;

    // delay?
}

uint8_t receive_spi() {

    SERCOM1_REGS->SPIM.SERCOM_CTRLB |= SERCOM_SPIM_CTRLB_RXEN(1); // enable receiver
    // synchronize CTRLB
    while(_FLD2VAL(SERCOM_SPIM_SYNCBUSY_CTRLB, SERCOM1_REGS->SPIM.SERCOM_SYNCBUSY));

    // wait for receiving to be complete
    //while(!_FLD2VAL(SERCOM_SPIM_INTFLAG_RXC, SERCOM1_REGS->SPIM.SERCOM_INTFLAG));
    PORT_REGS->GROUP[LED_BANK].PORT_OUTCLR = 1 << LED_PIN;
    // check if there is a buffer overflow
    int bufovf = _FLD2VAL(SERCOM_SPIM_STATUS_BUFOVF, SERCOM1_REGS->SPIM.SERCOM_DATA);

    if(bufovf) {
        return -1;
    }

    uint8_t data = SERCOM1_REGS->SPIM.SERCOM_DATA;

    SERCOM1_REGS->SPIM.SERCOM_CTRLB |= SERCOM_SPIM_CTRLB_RXEN(0); // enable receiver
    // synchronize CTRLB
    while(_FLD2VAL(SERCOM_SPIM_SYNCBUSY_CTRLB, SERCOM1_REGS->SPIM.SERCOM_SYNCBUSY));

    return data;
}

