/*------------------------------------------------------------------------------
fm_3d8_main.C

------------------------------------------------------------------------------*/
//#pragma SRC

#include <REG52.H>                /* special function register declarations   */
                                  /* for the intended 8051 derivative         */
                                  /* is enabled		                          */

#include <intrins.h>

#include "fm_3d8.h"

// data  = direct data : 1st 128Byte
// idata = indirect data : all 256 bytes or above
// code  = program memory
unsigned char data mLED[8][8];			// LED matrix
unsigned char data mLED_BRIGHTNESS[8];	// LED brightness global to each layer
unsigned char idata mLEDA[8][8];	// LED matrix buffer

/* Timer0 interrupt routine */
// 1mS interrupt
void tm0_isr() interrupt 1 using 1
{

	TR0		= 0;				// Stop Timer1
	ET0		= 0;				// Disable Timer1 Interrupt
	G_DISABLE					// Disable LED

}

/* Timer0 interrupt routine */
// 1mS interrupt
void tm1_isr() interrupt 3 using 2
{

	static unsigned char tLayer=0;
	unsigned char i,j;
	unsigned char tByte;
	unsigned int  tmp=0;

	TL1 	= T1MS;				// Init Timer Lower Byte
	TH1		= T1MS >> 8;		// Init Timer Higher Byte


	TR0		= 0;
	ET0		= 0;
	SBUF = 'A';

	S_ENA_H		// disable output
	G_DISABLE	// Disable LED
	S_LATCH		// Latch data
	S_ENA_L		// enable output

	// Set brightness of the layer
	// Pulse Width = 51 + 7 * mLED_BRIGHTNESS

	if (mLED_BRIGHTNESS[tLayer]!=0)
	{
		P1 = (1<< tLayer);	// Change G
		tmp		= 65535 - 51;
		for (i=0; i<=6; i++)
		{
			tmp -= mLED_BRIGHTNESS[tLayer];
		}
		TL0 	= tmp;				// Init Timer Lower Byte
		TH0		= tmp >> 8;			// Init Timer Higher Byte
		TR0		= 1;				// Start Timer1
		ET0		= 1;				// Enable Timer1 Interrupt
	}
	else
	{
		P1 = 0x00;
	}

	// Prepare next layer of data 8 * 8 bits

	tLayer ++;
	tLayer &= 0x07;

	for (i=0; i<=7; i++)
	{
		tByte = mLED[tLayer][i];
		for (j=0; j<=7; j++)
		{
			if (tByte & 0x80)
				S_DATA_H
			else
				S_DATA_L

			S_CLK
			tByte = tByte << 1;
			uart_rx();
		}
	}

	S_DATA_L

	// Everything is prepared and waiting to latch out at next cycle

}


void uart_rx(void)
{
	unsigned char tbyte;

	if (RI)
	{
		RI 		= 0;		// Clear UART Rx Flag
		tbyte	= SBUF;		// Read data from UART_RX
		SBUF 	= tbyte;	// echo data back to UART_TX
	}

}
/*------------------------------------------------
The main C function.  Program execution starts
here after stack initialization.
------------------------------------------------*/
void main (void) {


//	Init Timer0
	TMOD 	|= 0x01;			// Set Timer to 16bits mode
	TL0 	= T500US;			// Init Timer Lower Byte
	TH0		= T500US >> 8;		// Init Timer Higher Byte
	TR0		= 0;				// Start Timer1
	ET0		= 0;				// Enable Timer1 Interrupt
	IP		|= 0x02;			// Set interrupt priority to 1

//	Init Timer1
	TMOD 	|= 0x10;			// Set Timer to 16bits mode
	TL1 	= T1MS;				// Init Timer Lower Byte
	TH1		= T1MS >> 8;		// Init Timer Higher Byte
	TR1		= 1;				// Start Timer1
	ET1		= 1;				// Enable Timer1 Interrupt

//	Init UART
	SCON = 0x50;				// 8-bits UART No parity
	TL2  = RCAP2L = (65536-(FOSC/32/BAUD));
	TH2  = RCAP2H = (65536-(FOSC/32/BAUD)) >> 8;
	T2CON = 0x34;


//	Init system variable

	mLED[0][0]	= 0x5A;
	mLED[0][1]  = 0xA5;
	mLED[0][2]	= 0xFF;
	mLED[0][3]	= 0x00;
	mLED[0][4]	= 0x01;
	mLED[0][5]	= 0x02;
	mLED[0][6]	= 0x03;
	mLED[0][7]	= 0x04;

	mLED[1][1]  = 0xff;
	mLED[2][2]  = 0xff;
	mLED[3][3]  = 0xff;
	mLED[4][4]  = 0xff;
	mLED[5][5]  = 0xff;
	mLED[6][6]  = 0xff;
	mLED[7][7]  = 0xff;

	mLED_BRIGHTNESS[0] = 0xF0;
	mLED_BRIGHTNESS[1] = 0xC0;
	mLED_BRIGHTNESS[2] = 0xA0;
	mLED_BRIGHTNESS[3] = 0x80;
	mLED_BRIGHTNESS[4] = 0x60;
	mLED_BRIGHTNESS[5] = 0x40;
	mLED_BRIGHTNESS[6] = 0x20;
	mLED_BRIGHTNESS[7] = 0x01;

//	Enable Global Interrupt
// 	Start the system
	EA		= 1;				// Enable Global Interrupt


  	while (1) 
  	{
    	P2 ^= 0x01;     		    /* Toggle P1.0 each time we print */
  	}
}


