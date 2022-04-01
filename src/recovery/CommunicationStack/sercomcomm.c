//
// Created by ksasse on 3/13/22.
//
#include "Device.h"
#include "../i2c.h"
#include "../uart.h"
#include "../spi.h"

//TODO modify baremetal for using different sercom regs

int read_uart(Device * device, char * buf, int size){
    //TODO how do we tell which instance we are using
    unsigned counter = 0;
    while(counter<size){
        buf[counter] = receive_uart();
        ++counter;
    }
    return counter;
}
int write_uart(Device * device,  char * buf, int size){
    unsigned counter = 0;
    while(counter<size){
        transmit_uart(buf[counter], device->SERCOMREGS);
        ++counter;
    }
    return counter;
}

int initialize_uart(Device * device, uint32_t baud_rate){
    //MACRO TO IMPLEMENT MUX
    init_uart(baud_rate, device->SERCOMREGS, device->PIN_IN, device->PIN_OUT);
    return 0;
}

int read_i2c(Device * device,  char * buf, int size, int addr){
    unsigned counter = 0;
    //TODO REMEMBER that ADDR comes from datasheets/next layer
    int code = transmit_address(1, addr);
    if(code==1){
        while(counter<size){
            buf[counter] = receive_i2c(device->SERCOMREGS);
            ++counter;
        }
        return counter;
    }
    else{
        return -1;
    }

}
int write_i2c(Device * device,  char * buf, int size, int addr){
    unsigned counter = 0;
    //TODO REMEMBER that ADDR comes from datasheets/next layer
    int code = transmit_address(0, addr);
    if(code==0){
        while(counter<size){
            transmit_i2c(buf[counter], device->SERCOMREGS);
            ++counter;
        }
        return counter;
    }
    else{
        return -1;
    }
}

int initialize_i2c(Device * device, uint32_t baud, uint32_t lowtout){
    init_i2c(lowtout, baud, device->SERCOMREGS);
    return 0;
}

int initialize_spi(Device * device, uint32_t baud){
    init_spi(baud, device->SERCOMREGS, device->PIN_IN, device->PIN_OUT);
}

int read_spi(Device * device,  char * buf, int size){
    unsigned counter = 0;
    int error = 0;
    //TODO REMEMBER that ADDR comes from datasheets/next layer
    while(counter<size) {
        buf[counter] = receive_spi(device->SERCOMREGS);
        ++counter;
        if (buf[counter]==-1){
            error = 1;
            break;
        }
    }
    if(error){
        return -1;
    }
    return counter;
}

int write_spi(Device * device,  char * buf, int size, int bank, int pin){
    unsigned counter = 0;
    while(counter<size){
        transmit_i2c(buf[counter], bank, pin, device->SERCOMREGS);
        ++counter;
    }
    return counter;
}