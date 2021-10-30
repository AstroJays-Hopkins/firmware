#include "spi.h"
#include "sam.h"
#include "pinmap.h"
#include "bit_utils.h"

#define FREF 2000000

void init_spi(int target_baud) {
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
    SERCOM1_REGS->SPIM.SERCOM_CTRLB =   SERCOM_SPIM_CTRLB_CHSIZE_8_BIT; // character size is 8 bits

    SERCOM1_REGS->SPIM.SERCOM_CTRLB = SERCOM_SPIM_CTRLB_MSSEN(0);

    SERCOM1_REGS->SPIM.SERCOM_BAUD = 9;//(FREF/(2 * target_baud)) - 1; // synchronous mode
    

    //SERCOM1_REGS->SPIM.SERCOM_CTRLB |= SERCOM_SPIM_CTRLB_RXEN(1); // enable receiver


    SERCOM1_REGS->SPIM.SERCOM_CTRLA |= SERCOM_SPIM_CTRLA_ENABLE(1); // enable SPI
    
    // synchronize CTRLA_ENABLE
    while(_FLD2VAL(SERCOM_SPIM_SYNCBUSY_ENABLE, SERCOM1_REGS->SPIM.SERCOM_SYNCBUSY));
    // synchronize CTRLB ?
    while(_FLD2VAL(SERCOM_SPIM_SYNCBUSY_CTRLB, SERCOM1_REGS->SPIM.SERCOM_SYNCBUSY));
    
    // initialize and enable SPI interrupts
    //NVIC_SetPriority(SERCOM1_IRQn, 3);       
    //NVIC_EnableIRQ(SERCOM1_IRQn); // enable interrupt

    // enable Receive Complete Interrupt
    //SERCOM1_REGS->SPIM.SERCOM_INTENSET |= SERCOM_SPIM_INTENSET_RXC(0x1);
}


void SERCOM1_Handler() {
    // check SPI RXC flag
    // read data
    //clear SPI RXC flagW
}



void transmit_spi(int data) {
    // set SS to low to start transaction

    //  set pin to low
    _CLR_BIT(PORT_REGS->GROUP[SS_BANK].PORT_OUT,SS_PIN);

    // delay?

    // wait for data to be empty
    while(!_FLD2VAL(SERCOM_SPIM_INTFLAG_DRE, SERCOM1_REGS->SPIM.SERCOM_INTFLAG));
    
    // transmit
    SERCOM1_REGS->SPIM.SERCOM_DATA = data;

    // wait for trasmit to finish
    while(!_FLD2VAL(SERCOM_SPIM_INTFLAG_TXC, SERCOM1_REGS->SPIM.SERCOM_INTFLAG));

    // then set SS to high

    //  set pin to high
    _SET_BIT(PORT_REGS->GROUP[SS_BANK].PORT_OUT,SS_PIN);

    // delay?
}

int receive_spi() {

    // wait for receiving to be complete
    while(!_FLD2VAL(SERCOM_SPIM_INTFLAG_RXC, SERCOM1_REGS->SPIM.SERCOM_INTFLAG));

    // check if there is a buffer overflow
    int bufovf = _FLD2VAL(SERCOM_SPIM_STATUS_BUFOVF, SERCOM1_REGS->SPIM.SERCOM_DATA);

    if(bufovf) {
        return -1;
    }

    int data = SERCOM1_REGS->SPIM.SERCOM_DATA;
    
    return data;
}

