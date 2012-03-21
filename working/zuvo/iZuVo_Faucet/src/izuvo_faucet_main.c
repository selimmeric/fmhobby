/*******************************************************************************
Include Section
*******************************************************************************/

#include "izuvo_faucet_main.h"
#include "izuvo_stc.h"
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

	init_hardware();
	init_timer();
	UART_one_init();

	init_system();

	mLED = 0x03;
	mLED |= 0x04;	// Red
	mLED |= 0x08;	// Common Anode
//	mLED |= 0x10;	// Blue
//	mLED |= 0x20;	// Green
	P1 = mLED;

 	EA	= 1;

	u_puts(VERSION);

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
	static unsigned char tCnt=0;

		tCnt ++;
//		delayms(250);
		mLED ^= 0x02 ;		
		mLED &= 0x03;
		switch (tCnt)
		{
		case 1:
			mLED |= 0x38;
			break;
		case 2:
			mLED |= 0x2C;
			break;
		case 3:
			mLED |= 0x1C;
			break;
		case 4:
			mLED |= 0x3C;
			tCnt = 0x00;
			break;
		default:
			tCnt = 0x00;
			break;
		}
		P1  = mLED;

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

