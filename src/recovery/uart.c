#include "sam.h"
#define fref 10

//read 26.6.2.1 in the datasheet for more explanation
void init_uart(unsigned target_baud){
    //Set External/internal clock in CTRLA.MODE Register which is already done 
    SERCOM0_REGS->USART_EXT.SERCOM_CTRLA = SERCOM_USART_EXT_CTRLA_CMODE_ASYNC //Set Async or Sync in CTRLA.CMODE Register
                                           | SERCOM_USART_EXT_CTRLA_RXPO_PAD0 //Select PIN to receive data to CTRLA.RXPO
                                           | SERCOM_USART_EXT_CTRLA_TXPO_PAD1 //Pads for CTRLA.TXPO
                                           | SERCOM_USART_EXT_CTRLA_DORD_LSB; //Data order bit in CTRLA.DORD register
    SERCOM0_REGS->USART_EXT.SERCOM_CTRLB = SERCOM_USART_EXT_CTRLB_CHSIZE_8_BIT //Configure Charactersize field in CTRLB.CHSIZE
                                           | SERCOM_USART_EXT_CTRLB_SBMODE_1_BIT //CTRLB.SBMODE - # of stop bits
                                           | SERCOM_USART_EXT_CTRLB_RXEN(0x1) //Write 1 to CTRLB.RXEN and CTRLB.TXEN
                                           | SERCOM_USART_EXT_CTRLB_TXEN(0x1);
    SERCOM0_REGS->USART_EXT.SERCOM_BAUD = 65536 * (1-16*(target_baud/fref)); //the fraction is multiplied by 16 which is the sample rate defined in SAMR
    SERCOM0_REGS->USART_EXT.SERCOM_CTRLA |= _VAL2FLD(SERCOM_USART_EXT_CTRLA_ENABLE, 1);
}  
//read 26.6.2.5 and 26.8.10 in datasheet for more explanation
int transmit_uart(int write_data){
    //waiting till the RXC bit is set and the DRE bit is set
    while(!(_FLD2VAL(SERCOM_USART_EXT_INTFLAG_RXC, SERCOM0_REGS->USART_EXT.SERCOM_INTFLAG) && _FLD2VAL(SERCOM_USART_EXT_INTFLAG_DRE, SERCOM0_REGS->USART_EXT.SERCOM_INTFLAG)));
    //write to register
    SERCOM0_REGS->USART_EXT.SERCOM_DATA = write_data;
    //wait for the register to clear
    while(!(_FLD2VAL(SERCOM_USART_EXT_INTFLAG_RXC, SERCOM0_REGS->USART_EXT.SERCOM_INTFLAG) && _FLD2VAL(SERCOM_USART_EXT_INTFLAG_DRE, SERCOM0_REGS->USART_EXT.SERCOM_INTFLAG)));
    //close transmission
    SERCOM0_REGS->USART_EXT.SERCOM_CTRLB = SERCOM_USART_EXT_CTRLB_TXEN(0x0);
    return 0;
}
//read 26.6.2.6 and 26.8.10 in datasheet for more explanation
int receive_uart(){
    //waiting till the RXC bit is set and the DRE bit is set
    while(!(_FLD2VAL(SERCOM_USART_EXT_INTFLAG_RXC, SERCOM0_REGS->USART_EXT.SERCOM_INTFLAG) && _FLD2VAL(SERCOM_USART_EXT_INTFLAG_DRE, SERCOM0_REGS->USART_EXT.SERCOM_INTFLAG)));
    //error handling
    int bufovf = _FLD2VAL(SERCOM_USART_EXT_STATUS_BUFOVF, SERCOM0_REGS->USART_EXT.SERCOM_STATUS);
    int ferr = _FLD2VAL(SERCOM_USART_EXT_STATUS_FERR, SERCOM0_REGS->USART_EXT.SERCOM_STATUS);
    int coll = _FLD2VAL(SERCOM_USART_EXT_STATUS_COLL, SERCOM0_REGS->USART_EXT.SERCOM_STATUS);
    //if any of the status are bad error
    if (bufovf||ferr||coll){
        return -1;
    } 
    //reading from data register
    int data = _FLD2VAL(SERCOM_USART_EXT_DATA_DATA, SERCOM0_REGS->USART_EXT.SERCOM_DATA);
    //ending recieving
    SERCOM0_REGS->USART_EXT.SERCOM_CTRLB = SERCOM_USART_EXT_CTRLB_RXEN(0x0);
    return data;
}