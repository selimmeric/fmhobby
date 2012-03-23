#ifndef __izuvo_faucet_main_h__
#define __izuvo_faucet_main_h__

#include<reg52.h>
#include <intrins.h>

#define VERSION "\r\nInstagram 1.0 @ " __DATE__ "  " __TIME__ "\r\n"



// Function Prototype

void process_uart(void);
void process_timer(void);

void task_1s(void);
void task_led_test(void);
void task_lcd_test(void);

#endif


