/*******************************************************************************
*
* File Name:   remote.h
* Project  :   ha-blaster
*
*
* Ver.     Date         Author           Comments
* -------  ----------   --------------   ------------------------------
* 1.00	   2011.12.01   FM		 		 initial build
*
*
* Copyright (c) 2011
*****************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   any later version.                                                    *
 ***************************************************************************/

#ifndef REMOTE_H
#define REMOTE_H


#include <avr/io.h>
#include <avr/interrupt.h>
#include "avr_types.h"

//#include <avr/signal.h>

// Define Macro
//

#define  FALSE	0
#define  TRUE	1

#define  LED1	0	// Traffic indication
#define  LED2	1	// Reserved
#define  LED3	2	// Tmr


#define VERSION "HA-BLASTER 1.0 @ " __DATE__ "  " __TIME__ 
#define EXIT "\r\nexit\r\n"
#define LinuxCmd "\r\nrx_data.sh "


// Define Function Prototyp
//

void ISR_TIMER0_OVR (void);
void POLL_TIMER0_OVR(void);
void ISR_UART_RECV(void);
void POLL_UART_RECV(void);

void init_hardware(void);
void init_system(void);
void process_tmr(void);
void process_cmd(void);
void process_cmd_linux(unsigned char iMode);
void process_cmd_hal(unsigned char *sptr, unsigned char len);


#endif
