#ifndef __izuvo_stc_h__
#define __izuvo_stc_h__

#include <reg52.h>
#include "utility.h"

#define RELOAD_COUNT 0xfd // 115200, 1T

// Function Prototype

void uart_tx(INT8U abyte); 
void UART_one_init(void);

void init_hardware(void);
void init_timer(void);
void init_system(void);


#endif
