#include "sam.h"
//function to start i2c process
//input is target rate that we are assigning to the baud
//lowtout is the lowtime out
void init_i2c(int lowtout, int baud);
//function to transmit address of the peripheral we are writing to 
//read_or_write is a 1/0 meaning read or write
//addr is the address of the peripheral
//returns 0 for starting a successful write
//returns 1 for starting a successful read
//returns -1,-2,-3 for master bus, arbitration loss, and nack error respectively 
int transmit_address(int read_or_write, int addr);
//reads data from register
int receive_i2c();
//puts data into register
//write_data is data to write to register
//returns -1 for errors
int transmit_i2c(int write_data);