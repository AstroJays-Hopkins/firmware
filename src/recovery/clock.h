#ifndef CLOCK_H_
#define CLOCK_H_


#define CORE_CLOCK_MUL 1465 // multiplies to 48.005M
#define CORE_CLOCK (CORE_CLOCK_MUL * 32768)

#define XOSC8M_PRESC 0x4 // divide down by 4 to get 2M out
#define SERCOM_CLK_DEFAULT 2000000

void core_clock_init();
void sercom_clock_init();

#endif /* CLOCK_H_ */
