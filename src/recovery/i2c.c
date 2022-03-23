#include "sam.h"
#include "globals.h"

//TODO Ask if I have to handle ACK and NACK or should i rely on smart enable doing ACK and NACK


//defined in 28.6.2.1
void init_i2c(int lowtout, int baud) {
    //lowtout 0 or 1
    PM_REGS->PM_APBCMASK |= PM_APBCMASK_SERCOM0(0x1);
    SERCOM0_REGS->I2CM.SERCOM_CTRLA |= SERCOM_I2CM_CTRLA_MODE_I2C_MASTER| //setting it as master
                                      SERCOM_I2CM_CTRLA_INACTOUT_205US| //setting the inactive time out to the highest possible
                                      SERCOM_I2CM_STATUS_LOWTOUT(lowtout); //setting the low
    SERCOM0_REGS->I2CM.SERCOM_CTRLB |= SERCOM_I2CM_CTRLB_SMEN(0x0); //setting smart enable
    
    
    SERCOM0_REGS->I2CM.SERCOM_BAUD = (sercom_clock/(2*baud))-1; //calculating baud rate for sync
    SERCOM0_REGS->I2CM.SERCOM_CTRLA |= SERCOM_I2CM_CTRLA_ENABLE(0x1); //enabling the serial protocol
    while(_FLD2VAL(SERCOM_I2CM_SYNCBUSY_ENABLE, SERCOM0_REGS->I2CM.SERCOM_SYNCBUSY));
    SERCOM0_REGS->I2CM.SERCOM_STATUS |= SERCOM_I2CM_STATUS_BUSSTATE(0x1);
    while(_FLD2VAL(SERCOM_I2CM_SYNCBUSY_SYSOP, SERCOM0_REGS->I2CM.SERCOM_SYNCBUSY));
}
//defined in 28.6.2.4.2
int transmit_address(int read_or_write, int addr) {
    //read = 1
    //write = 0
    //checking if the BUS is busy
    while(_FLD2VAL(SERCOM_I2CM_STATUS_BUSSTATE, SERCOM0_REGS->I2CM.SERCOM_STATUS)==0x3);
    //send start condition
    SERCOM0_REGS->I2CM.SERCOM_STATUS |= SERCOM_I2CM_CTRLB_CMD(0x1);
    while(_FLD2VAL(SERCOM_I2CM_SYNCBUSY_SYSOP, SERCOM0_REGS->I2CM.SERCOM_SYNCBUSY));
    //write address
    SERCOM0_REGS->I2CM.SERCOM_ADDR = SERCOM_I2CM_ADDR_ADDR((addr<<1)| read_or_write); //sending the address of the peripheral with a read or write bit ORed on

    while(_FLD2VAL(SERCOM_I2CM_SYNCBUSY_SYSOP, SERCOM0_REGS->I2CM.SERCOM_SYNCBUSY));
    int master_on_bus = _FLD2VAL(SERCOM_I2CM_INTFLAG_MB, SERCOM0_REGS->I2CM.SERCOM_INTFLAG); //checking for master_on_bus bit set

    int bus_error  = _FLD2VAL(SERCOM_I2CM_STATUS_BUSERR, SERCOM0_REGS->I2CM.SERCOM_STATUS); //checking for master bus error
    //checking if bus error
    if (bus_error){
        return -1;
    }
    //checking if an ack was recieved
    int rxnack = _FLD2VAL(SERCOM_I2CM_STATUS_RXNACK, SERCOM0_REGS->I2CM.SERCOM_STATUS); //checking if no ack was recieved
    if (master_on_bus && rxnack){
        return -3;
    }
    //checking if the write was successful
    if (master_on_bus && !rxnack && !read_or_write){
        //three things to do
        //either send data
        //transmit a new address
        //end the transaction
        return 0;
    }
    //checking if the read was successful 
    int slave_on_bus = _FLD2VAL(SERCOM_I2CM_INTFLAG_SB, SERCOM0_REGS->I2CM.SERCOM_STATUS);
    if (slave_on_bus && !rxnack && read_or_write){
        //read data by sending ACK or automatically sent in smart mode.
        //transmit a new address
        //end the transaction
        return 1;
    }
}   
//defined in 28.6.2.4.4 and 28.6.2.4.5
int receive_i2c() {
    //read data in from data register
    int sent_byte = _FLD2VAL(SERCOM_I2CM_SYNCBUSY_SYSOP, SERCOM0_REGS->I2CM.SERCOM_INTFLAG);
    //TODO MAX RESOLVE RACE CONDITION IF IT EXISTS
    if (sent_byte){
        SERCOM0_REGS->I2CM.SERCOM_CTRLB |= SERCOM_I2CM_CTRLB_ACKACT(0x0);
        int data = SERCOM0_REGS->I2CM.SERCOM_DATA;
    }
    else{
        int data = 0;
        SERCOM0_REGS->I2CM.SERCOM_CTRLB |= SERCOM_I2CM_CTRLB_ACKACT(0x1);
    }
    //TODO MAX RESOLVE TURNING OFF RECIEVE AFTER ONE USE
    //send end condition
    SERCOM0_REGS->I2CM.SERCOM_STATUS |= SERCOM_I2CM_CTRLB_CMD(0x3);
    while(_FLD2VAL(SERCOM_I2CM_SYNCBUSY_SYSOP, SERCOM0_REGS->I2CM.SERCOM_SYNCBUSY));

    return data;
}
//defined in 28.6.2.4.3
int transmit_i2c(int write_data) {
    //write to register
    SERCOM0_REGS->I2CM.SERCOM_DATA = write_data;
    while(_FLD2VAL(SERCOM_I2CM_SYNCBUSY_SYSOP, SERCOM0_REGS->I2CM.SERCOM_SYNCBUSY));
    //wait till arbitration loss and rxnack are not set
    //must wait for rxnack to not be set 
    while(!_FLD2VAL(SERCOM_I2CM_STATUS_RXNACK, SERCOM0_REGS->I2CM.SERCOM_STATUS));
    //send end condition
    //TODO MAX Needs to decide whether to end the transmission after each packet
    SERCOM0_REGS->I2CM.SERCOM_STATUS |= SERCOM_I2CM_CTRLB_CMD(0x3);
    while(_FLD2VAL(SERCOM_I2CM_SYNCBUSY_SYSOP, SERCOM0_REGS->I2CM.SERCOM_SYNCBUSY));
    //if not return successful 
    return 0;
}