#ifndef __izuvo_stc_h__
#define __izuvo_stc_h__

#include <reg52.h>
#include "utility.h"

#define FOSC 11078310L
#define T1MS (65536 - FOSC/12/1000)
#define T500US (65535 - (51+(256*7)))
#define RELOAD_COUNT 0xfd // 115200, 1T


// PORT0
#define PIN_RED 	3
#define PIN_GREEN   0
#define PIN_BLUE    1
#define PIN_BL      5


// Define macro

#define testbit(var, bit) 	((var) & (1 <<(bit)))
#define setbit(var, bit) 	((var) |= (1 << (bit)))
#define clrbit(var, bit) 	((var) &= ~(1 << (bit)))

#define LED_R_ON()		clrbit (P1, PIN_RED)
#define LED_R_OFF()		setbit (P1, PIN_RED)
#define LED_G_ON()		clrbit (P1, PIN_GREEN)
#define LED_G_OFF()		setbit (P1, PIN_GREEN)
#define LED_B_ON()		clrbit (P1, PIN_BLUE)
#define LED_B_OFF()		setbit (P1, PIN_BLUE)
#define LED_BL_ON()		clrbit (P1, PIN_BL)
#define LED_BL_OFF()	setbit (P1, PIN_BL)


// Function Prototype

void uart_tx(INT8U abyte); 
void UART_one_init(void);

void init_hardware(void);
void init_timer(void);
void init_system(void);


#endif
