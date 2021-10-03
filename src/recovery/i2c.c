#include "sam.h"
#define fref 10
void init_i2c(int lowtout, int baud){
    //lowtout 0 or 1
    SERCOM0_REGS->I2CM.SERCOM_CTRLA = SERCOM_I2CM_CTRLA_MODE(0x5)| 
                                      SERCOM_I2CM_CTRLA_INACTOUT_205US|
                                      SERCOM_I2CM_STATUS_LOWTOUT(lowtout);
    SERCOM0_REGS->I2CM.SERCOM_CTRLB |= _VAL2FLD(SERCOM_I2CM_CTRLB_SMEN, 1);
    SERCOM0_REGS->I2CM.SERCOM_BAUD = (fref/(2*baud))-1;
    SERCOM0_REGS->I2CM.SERCOM_CTRLA |= _VAL2FLD(SERCOM_I2CM_CTRLA_ENABLE, 1);
}
int receive_i2c(){
 
}
int transmit_i2c(int write_data){

}