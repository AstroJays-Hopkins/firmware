//
// Created by ksasse on 3/13/22.
//

#ifndef FIRMWARE_DEVICE_H
#define FIRMWARE_DEVICE_H
typedef struct  {
    uint8_t PIN_IN;
    uint8_t PIN_OUT;
    uint8_t PROTOCOL;
    void *SERCOMREGS;
} Device;
#endif //FIRMWARE_DEVICE_H
