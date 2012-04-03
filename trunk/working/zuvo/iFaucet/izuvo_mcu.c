#include "izuvo_mcu.h"
#include "izuvo.h"
#include "utility.h"

extern unsigned char mCmdQ[128];	 		// mCmdQ[0] = index + cmd flag
extern volatile unsigned char  	mFunctionMode;

volatile unsigned int 	mPulse[2];
volatile unsigned int 	mPulseCnt;
volatile unsigned char	mTxFlag;
volatile unsigned int  	mPulseFreq;	
volatile unsigned int  	mPulseTable[8][2];
//volatile unsigned char 	mPulseCode[32];	
volatile unsigned char 	mPulseLen;	
volatile unsigned char 	mPulseIndex;
volatile unsigned char  mPulseRepeat;

volatile unsigned int 	mCapturePulse[200];
volatile unsigned int 	mCaptureIndex;
volatile unsigned char 	mCaptureState;
volatile unsigned char 	mCaptureRepeat;

volatile unsigned char  mDecodeData[30];
volatile unsigned int   mDecodeIndex;
volatile unsigned int   mDecodeBit;

volatile unsigned char  mZuvoPulse[3];
volatile unsigned int   mZuvoHeader[2]={0,0};
volatile unsigned char  mZuvoData[30];
volatile unsigned char  mZuvoIndex=0xff;
volatile unsigned char  mZuvoBit=0;
volatile unsigned char  mZuvoBitCnt=0;
volatile unsigned char  mZuvoByteCnt=0;
volatile unsigned int   mZuvoUartByte=0;
volatile unsigned char  mZuvoAuto=0;

extern unsigned char 	mLinuxMode;


void process_remote_command(char *input)
{

//	"~R{A}{B} {REPEAT} {CODE}
//  A = Action 	:  S = Send ; C = Capture ...
//	B = TYPE 	:  0 = IR; 1 = 433MHz; 2 = 335MHz; 3 = X10 ...
//

	char tType=0;
	char tAction = 0;

	tAction = *(input+1);
	tType 	= *(input+2);

	switch (tType)
	{
		case '0':	// iR
			switch (tAction)
			{
				case 'S':
//					ir_init_send_pulse(input+4);
					break;
				case 'C':
					mPulseIndex = 0;
//					ir_init_capture_pulse(input+4);
					break;
			}

		case '1':
			break;

		case '2':
			break;

		default:
			break;
	
	}

}


void ir_wait_tx(void)
{
	unsigned int x=0;

	while (x<15000)					// Time Out
	{

		if ((TIFR & 0x01)==0x01)	// TIMER0 Overflow
		{
			POLL_TIMER0_OVR();
		}

		if ((UCSRA & 0x80)==0x80)	// UART RX data ready for recieve
		{
	 		POLL_UART_RECV();
		}

		x++;
//		asm volatile ("nop");
		if (mTxFlag==0)
		{
			break;
		}
	}
	TCCR1B &= 0b11111000;	// Stop Clock

}

#if (0)

void ir_init_capture_pulse(char *input)
{

	// Set TIMER1 WGM to Mode 0
	TCCR1B = 0b00000000;	// Disable Input Capture
	TCNT1  = 0;
	mCaptureIndex = 0;
	mCaptureState = 1;
	mFunctionMode = 2;		// Change it to Capture Mode
	mCaptureRepeat =  u_asc2uint(2,(char *) (input));

	TCCR1B  = 0b11000011;	// Enable Input Capture
	TIMSK  &= ~0b00111100;  // Disable all TIMER1 interupt
	TIMSK  |=  0b00100000;	// Enable Input Capture1 Interrupt

}

void izuvo_init_capture_pulse(void)
{

	// Set TIMER1 WGM to Mode 0
	TCCR1B = 0b00000000;	// Disable Input Capture
	TCNT1  = 0;
	mCaptureIndex = 0;
	mCaptureState = 1;
	mFunctionMode = 2;		// Change it to Capture Mode
	mCaptureRepeat =  0x01;

	TCCR1B  = 0b11000011;	// Enable Input Capture
	TIMSK  &= ~0b00111100;  // Disable all TIMER1 interupt
	TIMSK  |=  0b00100000;	// Enable Input Capture1 Interrupt


}


unsigned char izuvo_search_header(void)
{

//  0135 26 13 13 13
	unsigned char i;
	unsigned char index;
	unsigned int  tPulseArr[3];

	index = sizeof(mCapturePulse)/2;

	if (index >=10)
	{
		index = index -4;
	}

	tPulseArr[0]=0;
	tPulseArr[1]=0;
	tPulseArr[2]=0;

	for (i=0; i<index; i++)
	{
	
		tPulseArr[0]=tPulseArr[1];
		tPulseArr[1]=tPulseArr[2];
		tPulseArr[2]=mCapturePulse[i]>>1;


		if ((tPulseArr[0] >= 0x0100) && (tPulseArr[1]>0x20) && (tPulseArr[1]<0x30) && (tPulseArr[2]<0x15))
		{ 
				return(i-1);
		}

	}

	return (0);
}



unsigned char swapByte(unsigned char iByte)
{

	unsigned char i;
	unsigned char tByte=0;


	for (i=0; i<=7; i++)
	{
		tByte  = (tByte << 1);
		if ((iByte & (0x01<<i)) !=0)
		{
			tByte += 1;
		}
	
	}

	return (tByte);

}

void izuvo_rx_pulse(unsigned int iPulse)
{

	unsigned char unit;
	unsigned char i,j;
	unsigned char tByte;

	unit = izuvo_rx_unit(iPulse>>1);

	mZuvoHeader[1] = (mZuvoHeader[1] << 4) + (mZuvoHeader[0]>>12);
	mZuvoHeader[0] = (mZuvoHeader[0] << 4) + (unit & 0x0f);


	if (mZuvoIndex<sizeof(mZuvoData))
	{
		for (i=0; i<unit; i++)
		{
			mZuvoUartByte = mZuvoUartByte<<1;
			if (mZuvoBit !=0)
			{
				mZuvoUartByte +=1;
			}
			mZuvoBitCnt +=1;

			if (mZuvoBitCnt>=10)
			{
				tByte = swapByte((mZuvoUartByte>>1)&0xff);
				mZuvoData[mZuvoByteCnt]=tByte;
				mZuvoBitCnt  -=10;
				mZuvoByteCnt +=1;
				if (tByte==0x0D)
				{
					if ((mLinuxMode==2) || (1))	
					{
					PORTC ^= (1 << LED2);
					u_puts(LinuxCmd);
//					u_puts("Footer detected\r\n");
					for (j=0; j<mZuvoByteCnt; j++)
					{
						u_putch(mZuvoData[j]);
					}
					u_puts("\r\n");
					}
					mZuvoIndex=0xff;
				}
			}

		}

		mZuvoBit ^= 0x01;
	}

	if (((mZuvoHeader[1]&0x0fff)==0x0A21) && (mZuvoHeader[0]==0x1141))
	{
		mZuvoIndex=0;
		mZuvoBit=0;
		mZuvoBitCnt=0;
		mZuvoByteCnt=0;
		mZuvoUartByte=0;
//		u_puts("Header Detected\r\n");
	}

	if (mZuvoHeader[0]==0xaa)
	{
		mZuvoIndex=0xff;	
	}


}

unsigned char izuvo_rx_unit(unsigned int iCnt)
{

	unsigned char unit;
	unsigned char iByte;

	if (iCnt > 0xff)
	{
		iByte = 0xff;
	}
	else
	{
		iByte = iCnt & 0xff;
	}


	if (iByte < ((unit_base*2) - unit_tol))
	{
		unit = 1;
	}
	else if (iByte < ((unit_base*3) - unit_tol))
	{
		unit = 2;
	}
	else if (iByte < ((unit_base*4) - unit_tol))
	{
		unit = 3;
	}
	else if (iByte < ((unit_base*5) - unit_tol))
	{
		unit = 4;
	}
	else if (iByte < ((unit_base*6) - unit_tol))
	{
		unit = 5;
	}
	else if (iByte < ((unit_base*7) - unit_tol))
	{
		unit = 6;
	}
	else if (iByte < ((unit_base*8) - unit_tol))
	{
		unit = 7;
	}
	else if (iByte < ((unit_base*9) - unit_tol))
	{
		unit = 8;
	}
	else if (iByte < ((unit_base*10) - unit_tol))
	{
		unit = 9;
	}
	else
	{
		unit = 10;
	}

	return (unit);

}


void izuvo_decode_byte(unsigned char iByte)
{

		unsigned char unit;
		unsigned int  tIndex;
		static unsigned int tRx;
		unsigned char i;

//		unit = izuvo_rx_unit(iByte);

		if (iByte < ((unit_base*2) - unit_tol))
		{
			unit = 1;
		}
		else if (iByte < ((unit_base*3) - unit_tol))
		{
			unit = 2;
		}
		else if (iByte < ((unit_base*4) - unit_tol))
		{
			unit = 3;
		}
		else if (iByte < ((unit_base*5) - unit_tol))
		{
			unit = 4;
		}
		else if (iByte < ((unit_base*6) - unit_tol))
		{
			unit = 5;
		}
		else if (iByte < ((unit_base*7) - unit_tol))
		{
			unit = 6;
		}
		else if (iByte < ((unit_base*8) - unit_tol))
		{
			unit = 7;
		}
		else if (iByte < ((unit_base*9) - unit_tol))
		{
			unit = 8;
		}
		else if (iByte < ((unit_base*10) - unit_tol))
		{
			unit = 9;
		}
		else
		{
			unit = 10;
		}

		// Valid bit(s)
		if (unit<10)
		{

			for (i=0; i<unit; i++)
			{
				tRx = (tRx << 1);

				if (mDecodeBit==1)
				{
					tRx = tRx +1;
				}
			}
					
		}

		mDecodeBit ^= 0x01;

		mDecodeIndex = mDecodeIndex + unit;

		u_putHexByte(unit,0x00);
		u_puts(" ");		
//		u_putHexWord(mDecodeIndex&0xff);
//		u_puts(" ");		

		tIndex = mDecodeIndex;

		while (tIndex >=0x0A)
		{
			tIndex = tIndex - 0x0A;
			if (tIndex ==0)
			{
				tRx = swapByte((tRx>>1) & 0xff);
				mDecodeData[0] +=1;
				mDecodeData[mDecodeData[0]]= tRx;
//				u_putHexByte(mDecodeData[mDecodeData[0]],'_');
//				u_putHexByte(mDecodeData[0],'-');
//				u_putHexWord(mDecodeIndex&0xff);
				u_puts("\r\n");	
				if (tRx==0x0D)
				{
					u_puts("End detected\r\n");
					for (i=1; i<mDecodeData[0]; i++)
					{
						u_putch(mDecodeData[i]);
					}
					u_puts("\r\n");
//					u_putHexByte(mDecodeData[0],' ');
				}
				tRx = 0x00;

			}
		}


}

void izuvo_decode_data(void)
{

		unsigned char i ;
		unsigned int tPulse;

		u_puts("decode data :");		// 

		mDecodeIndex=0;
		mDecodeBit = 0;
		mDecodeData[0]=0x00;

		for (i=mPulseIndex; i<= (mCaptureIndex&0xff); i++)
		{
		
			tPulse = mCapturePulse[i];
			tPulse = tPulse >> 1;
			if (tPulse>0xff)
				izuvo_decode_byte(0xff);
			else
				izuvo_decode_byte(tPulse  & 0xff);
		
		}

		for (i=1; i<mDecodeData[0]; i++)
		{
			u_putch(mDecodeData[i]);	
		}

		u_puts("\r\n");


}

void ir_capture_pulse(void)
{

//	When Capture Pulse is enabled
//	This function should be call @ every 200mS

//	unsigned char i;
	unsigned char index;
//	unsigned char j=0;
	mCaptureState ++;
//	unsigned int tPulse;

	// 2second time out OR buffer full
	if ((mCaptureState>mCaptureRepeat) || (mCaptureIndex==sizeof(mCapturePulse)/2))
	{
		TCCR1B = 0b00000000;	// Disable Input Capture
		index = mCaptureIndex & 0xff;
		mFunctionMode = 0;	// Change it to Idle mode
//		u_puts(LinuxCmd);
//		u_puts("'C0' '");		// Capture data
//		u_putHexWord(CaptureFreq);
//		u_puts("' '");		// Capture data
//		u_putHexByte(index,0x00);
//		u_puts("' '");		// Capture data

//		j=izuvo_search_header();
//		u_putHexByte(j,' ');

//		izuvo_decode_data();
/*
		u_putHexByte(mPulseIndex,' ');
		for (i=mPulseIndex; i<index; i++)
		{
			tPulse = mCapturePulse[i];
			tPulse = tPulse >> 1;
			if (tPulse >= 0x0100)
			{
				u_putHexWord(tPulse);
			}
			else
			{
				u_putHexByte(tPulse & 0xff,0x00);
			}
			u_puts(" ");		
		}
		u_puts("'\r\n");
		mCaptureIndex = 0;
*/
	}
	
}

#endif
