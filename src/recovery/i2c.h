#include "sam.h"
//function to start uart process
//input is target rate that we are assigning to the baud
void init_i2c(int lowtout, int baud);
//function to recieve data from register
//returns the data if true, returns -1 if an error occured
int receive_i2c();
//puts data into register
//input is data to write to register
int transmit_i2c(int write_data);