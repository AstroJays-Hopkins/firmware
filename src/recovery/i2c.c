#include "sam.h"
#define FREF 10000000 //10 million


//defined in 28.6.2.1
void init_i2c(int lowtout, int baud) {
    //lowtout 0 or 1
    SERCOM0_REGS->I2CM.SERCOM_CTRLA = SERCOM_I2CM_CTRLA_MODE_I2C_MASTER| //setting it as master
                                      SERCOM_I2CM_CTRLA_INACTOUT_205US| //setting the inactive time out to the highest possible
                                      SERCOM_I2CM_STATUS_LOWTOUT(lowtout); //setting the low
    SERCOM0_REGS->I2CM.SERCOM_CTRLB |= SERCOM_I2CM_CTRLB_SMEN(1); //setting smart enable
    SERCOM0_REGS->I2CM.SERCOM_BAUD = (FREF/(2*baud))-1; //calculating baud rate for sync
    SERCOM0_REGS->I2CM.SERCOM_CTRLA |= SERCOM_I2CM_CTRLA_ENABLE(1); //enabling the serial protocol
}
//defined in 28.6.2.4.2
int transmit_address(int read_or_write, int addr) {
    //read = 1
    //write = 0
    SERCOM0_REGS->I2CM.SERCOM_ADDR = SERCOM_I2CM_ADDR_ADDR(addr| read_or_write); //sending the address of the peripheral with a read or write bit ORed on
    int master_on_bus = _FLD2VAL(SERCOM_I2CM_INTFLAG_MB, SERCOM0_REGS->I2CM.SERCOM_INTFLAG); //checking for master_on_bus bit set
    int arb_loss_error = _FLD2VAL(SERCOM_I2CM_STATUS_ARBLOST, SERCOM0_REGS->I2CM.SERCOM_STATUS); //checking for arbitration loss error
    int bus_error  = _FLD2VAL(SERCOM_I2CM_STATUS_BUSERR, SERCOM0_REGS->I2CM.SERCOM_STATUS); //checking for master bus error
    //checking if bus error
    if (master_on_bus && arb_loss_error && bus_error){
        return -1;
    }
    //checking if arbitration loss error
    else if (master_on_bus && arb_loss_error){
        return -2;
    }
    //checking if an ack was recieved
    int rxnack = _FLD2VAL(SERCOM_I2CM_STATUS_RXNACK, SERCOM0_REGS->I2CM.SERCOM_STATUS); //checking if no ack was recieved
    if (master_on_bus && rxnack){
        return -3;
    }
    //checking if the write was successful
    if (master_on_bus && !rxnack && !read_or_write){
        return 0;
    }
    //checking if the read was successful 
    int slave_on_bus = _FLD2VAL(SERCOM_I2CM_INTFLAG_SB, SERCOM0_REGS->I2CM.SERCOM_STATUS);
    if (slave_on_bus && !rxnack && read_or_write){
        return 1;
    }
}   
//defined in 28.6.2.4.4 and 28.6.2.4.5
int receive_i2c() {
    //read data in from data register
    int data = _FLD2VAL(SERCOM_I2CM_DATA_DATA, SERCOM0_REGS->I2CM.SERCOM_DATA);
    return data;
}
//defined in 28.6.2.4.3
int transmit_i2c(int write_data) {
    //write to register
    SERCOM0_REGS->I2CM.SERCOM_DATA = write_data;
    //wait till arbitration loss and rxnack are not set
    //must wait for rxnack to not be set 
    while(!_FLD2VAL(SERCOM_I2CM_STATUS_ARBLOST, SERCOM0_REGS->I2CM.SERCOM_STATUS)&&!_FLD2VAL(SERCOM_I2CM_STATUS_RXNACK, SERCOM0_REGS->I2CM.SERCOM_STATUS));
    //if arbitration loss happens error
    if (_FLD2VAL(SERCOM_I2CM_STATUS_ARBLOST, SERCOM0_REGS->I2CM.SERCOM_STATUS)){
        return -1;
    }
    //if not return successful 
    return 0;
}