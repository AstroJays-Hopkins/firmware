//function to start uart process
//input is target rate that we are assigning to the baud
void init_uart(unsigned target_baud);
//function to recieve data from register
//returns the data if true, returns -1 if an error occured
int receive_uart();
//puts data into register
//input is data to write to register
int transmit_uart(int write_data);