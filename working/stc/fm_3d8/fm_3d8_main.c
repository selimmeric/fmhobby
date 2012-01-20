/*------------------------------------------------------------------------------
fm_3d8_main.C

------------------------------------------------------------------------------*/
//#pragma SRC

#include <REG51.H>                /* special function register declarations   */
                                  /* for the intended 8051 derivative         */
                                  /* is enabled		                          */

#include <intrins.h>

#define FOSC 22118400L
#define T1MS (65536 - FOSC/12/1000)
// T1MS = 65536 - 1843
//#define T500US (65536 - FOSC/12/500)
//#define T500US (65535 - 1200)
#define T500US (65535 - (51+(256*7)))


#define G_DISABLE	(P1 = 0x00);
#define G0_H		(P1 = 0x01);
#define G1_H		(P1 = 0x02);
#define G2_H		(P1 = 0x04);
#define G3_H		(P1 = 0x08);
#define G4_H		(P1 = 0x10);
#define G5_H		(P1 = 0x20);
#define G6_H		(P1 = 0x40);
#define G7_H		(P1 = 0x80);

#define S_ENA_H 	(P3 |= 0x04);
#define S_ENA_L 	(P3 &= ~0x04);
#define S_DATA_H 	(P3 |= 0x08);
#define S_DATA_L 	(P3 &= ~0x08);
#define S_LATCH_H 	(P3 |= 0x20);
#define S_LATCH_L 	(P3 &= ~0x20);
#define S_LATCH\
		(P3 |= 0x20);\
		(P3 &= ~0x20);
#define S_CLK\
		P3 |= 0x10;\
		P3 &= ~0x10;



unsigned char mLED[8][8];			// LED matrix
unsigned char mLED_BRIGHTNESS;		// LED brightness global to all

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

	S_ENA_H		// disable output
	G_DISABLE	// Disable LED
	S_LATCH		// Latch data
	P1 = tByte;	// Change G
	tLayer ++;
	tLayer &= 0x07;
	S_ENA_L		// enable output

	tmp		= 65535 - 51;
	for (i=0; i<=6; i++)
	{
		tmp -= mLED_BRIGHTNESS;
	}

//	tmp = (65535 - (51+(250*7)));
	
	TL0 	= tmp;				// Init Timer Lower Byte
	TH0		= tmp >> 8;			// Init Timer Higher Byte
	TR0		= 1;				// Start Timer1
	ET0		= 1;				// Enable Timer1 Interrupt

	// Prepare 1 layer of data 8 * 8 bits

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
		}
	}

	S_DATA_L

	switch (tLayer & 0x07)
	{
		case 0:
//			G0_H
			tByte = 0x01;
			break;
	  	case 1:
//			G1_H
			tByte = 0x02;
			break;
		case 2:
//			G2_H
			tByte = 0x04;
			break;
		case 3:
//			G3_H
			tByte = 0x08;
			break;
		case 4:
//			G4_H
			tByte = 0x10;
			break;
		case 5:
//			G5_H
			tByte = 0x20;
			break;
		case 6:
//			G6_H
			tByte = 0x40;
			break;
		case 7:
//			G7_H
			tByte = 0x80;
			break;
		default:
			break;
	}	
	
	// Everything is prepared and waiting for latch out in next cycle

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
//	PT0H	= 0;
//	PT0		= 1;

	IP		|= 0x02;

//	Init Timer1
	TMOD 	|= 0x10;			// Set Timer to 16bits mode
	TL1 	= T1MS;				// Init Timer Lower Byte
	TH1		= T1MS >> 8;		// Init Timer Higher Byte
	TR1		= 1;				// Start Timer1
	ET1		= 1;				// Enable Timer1 Interrupt


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

	mLED_BRIGHTNESS = 0xff;

//	Enable Global Interrupt
// 	Start the system
	EA		= 1;				// Enable Global Interrupt


  	while (1) 
  	{
    	P2 ^= 0x01;     		    /* Toggle P1.0 each time we print */
  	}
}


