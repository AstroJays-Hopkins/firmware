/**
 * main (entrypoint) implementation
 */
#include "sam.h"
#include "system_samd21.h"

#include "init.h"

void loop(void) {
    //implement
}

int main(void) {
    SystemInit();
    AppInit();
    for(;;) loop();
}
