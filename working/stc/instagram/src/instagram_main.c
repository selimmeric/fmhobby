/*******************************************************************************
Include Section
*******************************************************************************/

#include "instagram_main.h"
#include "stc_mcu.h"
#include "utility.h"



/*****************************************************************************
Global Variale Section
******************************************************************************/

extern unsigned char mCmdQ[32];

unsigned char mLED=0;


/*****************************************************************************
Main Section
******************************************************************************/
main()
{
	unsigned char idata *idata_point;
	unsigned char i,j;

	init_hardware();
	init_timer();
	UART_one_init();

	init_system();

//	mLED = 0x03;
//	mLED |= 0x04;	// Red
//	mLED |= 0x08;	// Common Anode
//	mLED |= 0x10;	// Blue
//	mLED |= 0x20;	// Green
	mLED = 0xff;
//	mLED ^= 0x02;
	P1 = mLED;

 	EA	= 1;

	u_puts(VERSION);
	u_puts("ID=");
	idata_point = 0xf1;
	for (i=0; i<=6; i++)
	{
		j = *(idata_point+i);
		u_putHexByte(j & 0xff,0x00);
	}

	u_puts("\r\n");

	while(1)
	{
		process_uart();
		process_timer();
	}

}

// 
// Function start here
//

void process_timer(void)
{


}


void process_uart(void)
{
	unsigned char i;

	if (mCmdQ[0]>0x80)	
	{
		for (i=1; i<=(mCmdQ[0] & 0x7f); i++)
		{
			u_putch(mCmdQ[i]);	
		}
		u_puts("\r\n");
		mCmdQ[0] = 0x00;
	}

}


void delayms(unsigned char ms)
{
	unsigned int i;
	while(ms--)
	{
		for(i = 0; i < 2000; i++)
		{
			_nop_();
		}
	}
}

