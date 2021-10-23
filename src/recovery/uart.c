#include "uart.h"
#include "sam.h"

#include "bit_utils.h"
#include "globals.h"
#include "pinmap.h"

//GLOBAL DATA FOR INTERRUPT
static uint8_t global_data = 0; 

void comm_interrupt_init() {
    NVIC_SetPriority(SERCOM0_IRQn, 2);        // set usart interrupts to 2nd lowest priority
    NVIC_EnableIRQ(SERCOM0_IRQn);             // enable interrupt
}

void SERCOM0_Handler(){
    //if Recieve complete
    if (_FLD2VAL(SERCOM_USART_INT_INTFLAG_RXC, SERCOM0_REGS->USART_INT.SERCOM_INTFLAG)){
        //put data into global 
        global_data = SERCOM0_REGS->USART_INT.SERCOM_DATA;
        //clear the interrupt 
        SERCOM0_REGS->USART_INT.SERCOM_INTENCLR |= SERCOM_USART_INT_INTENCLR_RXC(0x1);
    }
}

//read 26.6.2.1 in the datasheet for more explanation
void init_uart(const uint32_t target_baud){
    PM_REGS->PM_APBCMASK |= PM_APBCMASK_SERCOM0(0x1);
    //Set External/internal clock in CTRLA.MODE Register which is already done
    SERCOM0_REGS->USART_INT.SERCOM_CTRLA = SERCOM_USART_INT_CTRLA_CMODE_ASYNC          //  ASYNC CLOCK
                                           | SERCOM_USART_INT_CTRLA_RXPO(0x3)          // recieve on pad 3
                                           | SERCOM_USART_INT_CTRLA_TXPO(0x1)          // transmit on pad 2
                                           | SERCOM_USART_INT_CTRLA_SAMPA(0x0)         // 16x over sampling
                                           | SERCOM_USART_INT_CTRLA_RUNSTDBY(0x1)      // run in standby mode
                                           | SERCOM_USART_INT_CTRLA_FORM(0x0)
                                           | SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK // use BAUD generator, not XCK line
                                           | SERCOM_USART_INT_CTRLA_DORD_LSB;          // LSB first in data stream
    SERCOM0_REGS->USART_INT.SERCOM_CTRLB = SERCOM_USART_INT_CTRLB_CHSIZE_8_BIT         // Configure Charactersize field in CTRLB.CHSIZE
                                           | SERCOM_USART_INT_CTRLB_SBMODE_1_BIT       // CTRLB.SBMODE - # of stop bits
                                           | SERCOM_USART_INT_CTRLB_RXEN(0x1)          // Write 1 to CTRLB.RXEN and CTRLB.TXEN
                                           | SERCOM_USART_INT_CTRLB_TXEN(0x1);
    //wait for sync busy
    while(_FLD2VAL(SERCOM_USART_INT_SYNCBUSY_CTRLB, SERCOM0_REGS->USART_INT.SERCOM_SYNCBUSY));
    //activate rxc interrupt
    SERCOM0_REGS->USART_INT.SERCOM_INTENCLR |= SERCOM_USART_INT_INTENCLR_RXC(0x1);
    SERCOM0_REGS->USART_INT.SERCOM_INTENSET |= SERCOM_USART_INT_INTENSET_RXC(0x1);
    // TODO: implement a FIXED_POINT math library and use for this calculation
    /* SERCOM0_REGS->USART_INT.SERCOM_BAUD = 65536 * (1-16*(target_baud/sercom_clock)); //the fraction is multiplied by 16 which is the sample rate defined in SAMR */
    SERCOM0_REGS->USART_INT.SERCOM_BAUD = 5138; // baud = 115200
    // Note data sheet says that enabling the uart will immediately disable the
    // TX and RX settings, but we must enable them to start the UART.
    SERCOM0_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_ENABLE(0x1);
    //wait for sync busy 
    while(_FLD2VAL(SERCOM_USART_INT_SYNCBUSY_ENABLE, SERCOM0_REGS->USART_INT.SERCOM_SYNCBUSY));
}

//read 26.6.2.5 and 26.8.10 in datasheet for more explanation
int transmit_uart(const uint8_t write_data){
    SERCOM0_REGS->USART_INT.SERCOM_CTRLB |= SERCOM_USART_INT_CTRLB_TXEN(0x1);
    while(_FLD2VAL(SERCOM_USART_INT_SYNCBUSY_CTRLB, SERCOM0_REGS->USART_INT.SERCOM_SYNCBUSY));
    /* waiting till the DRE bit is set */
    while(!_FLD2VAL(SERCOM_USART_INT_INTFLAG_DRE, SERCOM0_REGS->USART_INT.SERCOM_INTFLAG));
    //write to register
    SERCOM0_REGS->USART_INT.SERCOM_DATA = write_data;
    //wait for the register to clear
    while(!_FLD2VAL(SERCOM_USART_INT_INTFLAG_TXC, SERCOM0_REGS->USART_INT.SERCOM_INTFLAG));
    //close transmission
    _FLDCLR(SERCOM0_REGS->USART_INT.SERCOM_CTRLB, SERCOM_USART_INT_CTRLB_TXEN);
    while(_FLD2VAL(SERCOM_USART_INT_SYNCBUSY_CTRLB, SERCOM0_REGS->USART_INT.SERCOM_SYNCBUSY));
    return 0;
}
//read 26.6.2.6 and 26.8.10 in datasheet for more explanation
/*uint8_t receive_uart(){
    SERCOM0_REGS->USART_INT.SERCOM_CTRLB |= SERCOM_USART_INT_CTRLB_RXEN(0x1);
    while(_FLD2VAL(SERCOM_USART_INT_SYNCBUSY_CTRLB, SERCOM0_REGS->USART_INT.SERCOM_SYNCBUSY));
    //wait for receive clear 
    while(!_FLD2VAL(SERCOM_USART_INT_INTFLAG_RXC, SERCOM0_REGS->USART_INT.SERCOM_INTFLAG));
    
    int bufovf = _FLD2VAL(SERCOM_USART_INT_STATUS_BUFOVF, SERCOM0_REGS->USART_INT.SERCOM_STATUS);
    int ferr = _FLD2VAL(SERCOM_USART_INT_STATUS_FERR, SERCOM0_REGS->USART_INT.SERCOM_STATUS);
    int coll = _FLD2VAL(SERCOM_USART_INT_STATUS_COLL, SERCOM0_REGS->USART_INT.SERCOM_STATUS);
    //if any of the status are bad error
    if (bufovf||ferr||coll){
        return -1;
    }
    //reading from data register
    //uint8_t data = SERCOM0_REGS->USART_INT.SERCOM_DATA;
    //ending recieving
    _FLDCLR(SERCOM0_REGS->USART_INT.SERCOM_CTRLB, SERCOM_USART_INT_CTRLB_RXEN);
    while(_FLD2VAL(SERCOM_USART_INT_SYNCBUSY_CTRLB, SERCOM0_REGS->USART_INT.SERCOM_SYNCBUSY));
    return 0;
}*/
uint8_t receive_uart(){
    return global_data;
}