#ifndef uart_H_
#define uart_H_

#include <stdint.h>


//function to start uart process
//input is target rate that we are assigning to the baud
void init_uart(const uint32_t target_baud);
//function to recieve data from register
//returns the data if true, returns -1 if an error occured
uint8_t receive_uart();
//puts data into register
//input is data to write to register
int transmit_uart(const uint8_t write_data);

#endif /* uart_H_ */
