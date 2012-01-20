/*------------------------------------------------------------------------------
HELLO.C

Copyright 1995-1999 Keil Software, Inc.
------------------------------------------------------------------------------*/

#include <REG52.H>                /* special function register declarations   */
                                  /* for the intended 8051 derivative         */

#include <stdio.h>                /* prototype declarations for I/O functions */


#ifdef MONITOR51                         /* Debugging with Monitor-51 needs   */
char code reserve [3] _at_ 0x23;         /* space for serial interrupt if     */
#endif                                   /* Stop Exection with Serial Intr.   */
                                         /* is enabled                        */
#define FOSC 18432000L
#define T1MS (65536 - FOSC/12/1000)


/* Timer0 interrupt routine */
// 1mS interrupt
void tm0_isr() interrupt 1 using 1
{

	static unsigned int count=0;
	TL0 = T1MS;
	TH0 = T1MS >> 8;
	if (count -- == 0)
	{
		count = 250;
		P1 ^= 0x02;
	}

}



/*------------------------------------------------
The main C function.  Program execution starts
here after stack initialization.
------------------------------------------------*/
void main (void) {

/*------------------------------------------------
Setup the serial port for 1200 baud at 16MHz.
------------------------------------------------*/
#ifndef MONITOR51
    SCON  = 0x50;		        /* SCON: mode 1, 8-bit UART, enable rcvr      */
    TMOD |= 0x20;               /* TMOD: timer 1, mode 2, 8-bit reload        */
    TH1   = 221;                /* TH1:  reload value for 1200 baud @ 16MHz   */
    TR1   = 1;                  /* TR1:  timer 1 run                          */
    TI    = 1;                  /* TI:   set TI to send first char of UART    */
#endif

//	Timer0 : produce 1ms Interrupt
	TMOD 	= 0x01;				// Set Timer0 to 16bits mode
	TL0 	= T1MS;				// Init Timer Lower Byte
	TH0		= T1MS >> 8;		// Init Timer Higher Byte
	TR0		= 1;				// Start Timer0
	ET0		= 1;				// Enable Timer0 Interrupt
	EA		= 1;				// Enable Global Interrupt

/*------------------------------------------------
Note that an embedded program never exits (because
there is no operating system to return to).  It
must loop and execute forever.
------------------------------------------------*/
  while (1) {
    P1 ^= 0x01;     		    /* Toggle P1.0 each time we print */
//    printf ("Hello World\n");   /* Print "Hello World" */
  }
}


