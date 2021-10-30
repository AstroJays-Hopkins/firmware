#include "clock.h"

#include "sam.h"
#include "system_samd21.h"

#include "globals.h"
#include "bit_utils.h"
#include "pinmap.h"


/**
 * 1. XOSC32->GCLK[1]->dfll48M->48Mhz+/-0.005%->GCLK[0]->GCLK_MAIN
 */
void core_clock_init() {
    uint32_t xosc_ena = _FLD2VAL(SYSCTRL_XOSC32K_ENABLE, SYSCTRL_REGS->SYSCTRL_XOSC32K);
    SYSCTRL_REGS->SYSCTRL_XOSC32K = SYSCTRL_XOSC32K_STARTUP(0x2) // 2048 startup cycles
                                    | SYSCTRL_XOSC32K_ONDEMAND(0) // always run
                                    | SYSCTRL_XOSC32K_RUNSTDBY(1) // run in standby
                                    | SYSCTRL_XOSC32K_AAMPEN(1) // auto control amplitude
                                    | SYSCTRL_XOSC32K_EN32K(1) // enable clock output
                                    | SYSCTRL_XOSC32K_XTALEN(1)  // enable crystal
                                    | SYSCTRL_XOSC32K_ENABLE(xosc_ena); // preserve previous enable state per data sheet
    SYSCTRL_REGS->SYSCTRL_XOSC32K |= SYSCTRL_XOSC32K_ENABLE(1); // enable 32k osc
    /* _CLR_BIT(SYSCTRL_REGS->SYSCTRL_XOSC32K, 0); */
    // wait until crystal comes online
    while(!_FLD2VAL(SYSCTRL_PCLKSR_XOSC32KRDY, SYSCTRL_REGS->SYSCTRL_PCLKSR));

    // configure GCLK[1] to source from xo0sc32k
    GCLK_REGS->GCLK_GENDIV = GCLK_GENDIV_DIV(0x1) // no divide
                             | GCLK_GENDIV_ID(0x1); // channel 1
    GCLK_REGS->GCLK_GENCTRL  = GCLK_GENCTRL_RUNSTDBY(0x1)
                               | GCLK_GENCTRL_DIVSEL(0x0) // no divide
                               | GCLK_GENCTRL_OE(0x1) // don't output to pin
                               | GCLK_GENCTRL_GENEN(0x1) // enable generator
                               | GCLK_GENCTRL_SRC_XOSC32K // source from 32k xtal
                               | GCLK_GENCTRL_ID(0x1); // generator 1
    // wait for sync
    while(_FLD2VAL(GCLK_STATUS_SYNCBUSY, GCLK_REGS->GCLK_STATUS));

    // setup dfll48m reference clock
    GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN(0x1) // enable peripheral clock
                              | GCLK_CLKCTRL_GEN_GCLK1 // connect to GCLK[1]
                              | GCLK_CLKCTRL_ID_DFLL48; // for dfll48

    // setup dfll48m
    // extract bits 58:63 from the nvm cal row
    /* const uint8_t dfll48m_coarse_cal = *((uint8_t *) (OTP4_ADDR + 56))>>2; */
    SYSCTRL_REGS->SYSCTRL_DFLLCTRL = 0; // disable dfll
    // wait for sync
    while(!_FLD2VAL(SYSCTRL_PCLKSR_DFLLRDY, SYSCTRL_REGS->SYSCTRL_PCLKSR));
    /* SYSCTRL_REGS->SYSCTRL_DFLLVAL = SYSCTRL_DFLLVAL_COARSE(dfll48m_coarse_cal); */
    while(!_FLD2VAL(SYSCTRL_PCLKSR_DFLLRDY, SYSCTRL_REGS->SYSCTRL_PCLKSR));
    SYSCTRL_REGS->SYSCTRL_DFLLMUL = SYSCTRL_DFLLMUL_CSTEP(5) // maximum course adjustment
                                    | SYSCTRL_DFLLMUL_FSTEP(10) // maximum fine adjustment
                                    | SYSCTRL_DFLLMUL_MUL(CORE_CLOCK_MUL);

    SYSCTRL_REGS->SYSCTRL_DFLLCTRL = SYSCTRL_DFLLCTRL_WAITLOCK(0x0) // wait for lock
                                     | SYSCTRL_DFLLCTRL_ONDEMAND(0x0) // always run
                                     | SYSCTRL_DFLLCTRL_RUNSTDBY(0x1) // run in standby
                                     | SYSCTRL_DFLLCTRL_MODE(0x1) // closed loop
                                     | SYSCTRL_DFLLCTRL_ENABLE(0x1); // enable
    while(!_FLD2VAL(SYSCTRL_PCLKSR_DFLLRDY, SYSCTRL_REGS->SYSCTRL_PCLKSR));
    while(!_FLD2VAL(SYSCTRL_PCLKSR_DFLLLCKC, SYSCTRL_REGS->SYSCTRL_PCLKSR)
          && !_FLD2VAL(SYSCTRL_PCLKSR_DFLLLCKF, SYSCTRL_REGS->SYSCTRL_PCLKSR));
    /* PORT_REGS->GROUP[LED_BANK].PORT_OUT = 1 << LED_PIN; */
    // wait for dfll lock

    /*
     * VERY IMPORANT: the max operating freq at 0 wait state is 24 M. We must
     * set this to 1 before we cut the system clock over, otherwise the CPU will
     * fault!
     *
     * In theory, the extra wait state will halve performance for 1
     * cycle isntructions. In practice, the flash is cached, and most instructions
     * are 16 bits whilst the flash bus is >= 32 bits so this gets masked for
     * _linear code_. We might end up paying extra branch penalty though...
     */
    NVMCTRL_REGS->NVMCTRL_CTRLB |= NVMCTRL_CTRLB_RWS(0x11);

    // configure GCLK[0] to source from dfll0
    // WARNING: This will cut over the system clock. All clock dependent config
    // should happen before this point.
    GCLK_REGS->GCLK_GENDIV = GCLK_GENDIV_DIV(0x1) // no divide
                             | GCLK_GENDIV_ID(0x0); // channel 0
    GCLK_REGS->GCLK_GENCTRL  = GCLK_GENCTRL_RUNSTDBY(0x1)
                               | GCLK_GENCTRL_DIVSEL(0x0) // no divide
                               | GCLK_GENCTRL_OE(0x0) // don't output to pin
                               | GCLK_GENCTRL_GENEN(0x1) // enable generator
                               | GCLK_GENCTRL_SRC_DFLL48M // source from 32k xtal
                               | GCLK_GENCTRL_ID(0x0); // generator 1
    // wait for sync
    while(_FLD2VAL(GCLK_STATUS_SYNCBUSY, GCLK_REGS->GCLK_STATUS));

    // update current clock variable
    SystemCoreClock = CORE_CLOCK;
}

/**
 * . OSC8M(div/4)->GCLK[2]->2Mhz->serial clocks
 */
void sercom_clock_init() {
    uint32_t osc8m_state = SYSCTRL_REGS->SYSCTRL_OSC8M;
    _CLR_BIT(SYSCTRL_REGS->SYSCTRL_OSC8M, SYSCTRL_OSC8M_ENABLE_Pos); // disable
    _FLDCLR(osc8m_state, SYSCTRL_OSC8M_PRESC);
    _CLR_BIT(osc8m_state, SYSCTRL_OSC8M_ONDEMAND_Pos);
    osc8m_state |= SYSCTRL_OSC8M_PRESC(XOSC8M_PRESC) // divide down by 4 to get 2M
                    | SYSCTRL_OSC8M_ENABLE(0x1) // enable after update
                    | SYSCTRL_OSC8M_RUNSTDBY(0x1);  // always run in standby
    SYSCTRL_REGS->SYSCTRL_OSC8M = osc8m_state;
    // wait for oscillator to come back up
    while(!_FLD2VAL(SYSCTRL_PCLKSR_OSC8MRDY, SYSCTRL_REGS->SYSCTRL_PCLKSR));
    /* PORT_REGS->GROUP[LED_BANK].PORT_OUT = 1 << LED_PIN; */

    // configure GCLK[2] to source from osc8m
    GCLK_REGS->GCLK_GENDIV = GCLK_GENDIV_DIV(0x1) // no divide
                             | GCLK_GENDIV_ID(0x2); // channel 1
    GCLK_REGS->GCLK_GENCTRL  = GCLK_GENCTRL_RUNSTDBY(0x1)
                               | GCLK_GENCTRL_DIVSEL(0x0) // no divide
                               | GCLK_GENCTRL_OE(0x1) // don't output to pin
                               | GCLK_GENCTRL_GENEN(0x1) // enable generator
                               | GCLK_GENCTRL_SRC_OSC8M // source from 32k xtal
                               | GCLK_GENCTRL_ID(0x2); // generator 1
    // wait for sync
    while(_FLD2VAL(GCLK_STATUS_SYNCBUSY, GCLK_REGS->GCLK_STATUS));

    // setup SERCOM reference clocks
    GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN(0x1) // enable peripheral clock
                              | GCLK_CLKCTRL_GEN_GCLK2 // connect to GCLK[1]
                              | GCLK_CLKCTRL_ID_SERCOM0_CORE; // for SERCOM0
    GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN(0x1) // enable peripheral clock
                              | GCLK_CLKCTRL_GEN_GCLK2 // connect to GCLK[1]
                              | GCLK_CLKCTRL_ID_SERCOM1_CORE; // for SERCOM1
    GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN(0x1) // enable peripheral clock
                              | GCLK_CLKCTRL_GEN_GCLK2 // connect to GCLK[1]
                              | GCLK_CLKCTRL_ID_SERCOM2_CORE; // for SERCOM2
    GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN(0x1) // enable peripheral clock
                              | GCLK_CLKCTRL_GEN_GCLK2 // connect to GCLK[1]
                              | GCLK_CLKCTRL_ID_SERCOM3_CORE; // for SERCOM3
    GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN(0x1) // enable peripheral clock
                              | GCLK_CLKCTRL_GEN_GCLK2 // connect to GCLK[1]
                              | GCLK_CLKCTRL_ID_SERCOM4_CORE; // for SERCOM4
    GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN(0x1) // enable peripheral clock
                              | GCLK_CLKCTRL_GEN_GCLK2 // connect to GCLK[1]
                              | GCLK_CLKCTRL_ID_SERCOM5_CORE; // for SERCOM5

    // sync serial clock monitor
    sercom_clock = SERCOM_CLK_DEFAULT;
}
