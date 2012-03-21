#ifndef __izuvo_faucet_main_h__
#define __izuvo_faucet_main_h__

#include<reg52.h>
#include <intrins.h>

#define VERSION "iZuVo_Faucet 1.0 @ " __DATE__ "  " __TIME__ "\r\n"

#define T1MS (65536 - FOSC/12/1000)
#define T500US (65535 - (51+(256*7)))


// Function Prototype

void process_uart(void);
void process_timer(void);

void delayms(unsigned char ms);

#endif

