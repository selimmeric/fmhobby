
#include "ir_remote.h"
#include "remote.h"
#include "utility.h"

extern unsigned char mCmdQ[128];	 		// mCmdQ[0] = index + cmd flag
extern volatile unsigned char  	mFunctionMode;

volatile unsigned int 	mPulse[2];
volatile unsigned int 	mPulseCnt;
volatile unsigned char	mTxFlag;
volatile unsigned int  	mPulseFreq;	
volatile unsigned int  	mPulseTable[8][2];
volatile unsigned char 	mPulseCode[32];	
volatile unsigned char 	mPulseLen;	
volatile unsigned char 	mPulseIndex;
volatile unsigned char  mPulseRepeat;

volatile unsigned int 	mCapturePulse[200];
volatile unsigned int 	mCaptureIndex;
volatile unsigned char 	mCaptureState;
volatile unsigned char 	mCaptureRepeat;


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
					ir_init_send_pulse(input+4);
					break;
				case 'C':
					ir_init_capture_pulse(input+4);
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

void decode_cpronto_code(char *input)
{
//	Format
//	"ff 0001 006C 03 22 015500AB 00150015 00150040 0112111111212221221121111122122221~";
//	"{REPEAT} {CODE}

// 	Frequency 
//	148 12MHz Count = 103 unit
//  Pronto Count * 1.4369 = MCU Count
//  1.4369 = 1  + 0.4369
//  0.4369 = 14 / 32 = 0.4375
//
//  MCU_Count = Pronto_Count + Pronto_Count * 14 / 32
//			  = Pronto_Count + Pronto_Count * (16 -2) /32

	unsigned char 	i;
	unsigned char 	index;
	unsigned int  	tmp;
	unsigned int	type;
	unsigned char 	pattern_cnt;

	type 		 	= u_asc2uint(4,(char *) (input+3));
	if (type != 0x0001)
	{
		return;
	}

	index = 0;
	mPulseRepeat 	= u_asc2uint(2,(char *) (input));
	mPulseFreq 		= u_asc2uint(4,(char *) (input+8));

	tmp = mPulseFreq << 4;
	tmp = tmp - (mPulseFreq <<1);
	tmp = tmp >> 5;
	tmp += mPulseFreq;

	mPulseFreq = tmp+1;

	pattern_cnt = u_asc2uint(2,(char *) (input+13));
	mPulseLen 	= u_asc2uint(2,(char *) (input+16));


	index = 19;
	for (i=0; i<=7; i++)
	{
		if (i<pattern_cnt)
		{	
			mPulseTable[i][0] = u_asc2uint(4,(char *) (input+index));
			mPulseTable[i][1] = u_asc2uint(4,(char *) (input+index+4));
			index = index + 9;	
		}
		else
		{
			mPulseTable[i][0] = 0;
			mPulseTable[i][1] = 0;
		}
	}


	for (i=0; i<sizeof(mPulseCode); i++)
	{
		mPulseCode[i]=u_asc2uint(2,(char *) (input+index));		
		index = index + 2;

	}

}

void decode_pronto_code(char *input)
{
//	Format
//	"ff 006d 015400aa 00150015 00150040 00160040 00150016 0015003f 001505e4 00000000 22 0112111112212223221111124122522126~";
//	"{REPEAT} {CODE}

// 	Frequency 
//	148 12MHz Count = 103 unit
//  Pronto Count * 1.4369 = MCU Count
//  1.4369 = 1  + 0.4369
//  0.4369 = 14 / 32 = 0.4375
//
//  MCU_Count = Pronto_Count + Pronto_Count * 14 / 32
//			  = Pronto_Count + Pronto_Count * (16 -2) /32

	unsigned char i;
	unsigned char index;
	unsigned int  tmp;

	mPulseRepeat 	= u_asc2uint(2,(char *) (input));
	mPulseFreq 		= u_asc2uint(4,(char *) (input+3));

	tmp = mPulseFreq << 4;
	tmp = tmp - (mPulseFreq <<1);
	tmp = tmp >> 5;
	tmp += mPulseFreq;

	mPulseFreq = tmp+1;


	index = 8;
	for (i=0; i<=7; i++)
	{
		mPulseTable[i][0] = u_asc2uint(4,(char *) (input+index));
		mPulseTable[i][1] = u_asc2uint(4,(char *) (input+index+4));
		index = index + 9;	
	}

	mPulseLen = u_asc2uint(2,(char *) (input+index));

	index = index + 3;
	for (i=0; i<sizeof(mPulseCode); i++)
	{
		mPulseCode[i]=u_asc2uint(2,(char *) (input+index));		
		index = index + 2;
	}

}

void ir_send_pulse(unsigned int ipulse0, unsigned int ipulse1)
{

	TIMSK  &= ~0b00111100;  // Disable all TIMER1 interupt
	TIMSK  |=  0b00010000;	// Enable OC1A interrupt

	TCCR1B 		&= 0b11111000;			// Stop Clock
	PORTC  		|= (1 << LED3);			// Set LED3
	PORTB  		&= ~(0x02);				// Set OC1A pin to "0"
	mTxFlag		= 1;					// Set Tx Flag
	TCCR1A  	= 0b01001000;			// Toggle OC1A on compare
	TCNT1  		= 0;					// Clear TIMER 1
	mPulseCnt 	= (ipulse0+ipulse1)<<1;	// High Pulse
	mPulse[0] 	= (ipulse0+ipulse1)<<1;	// Low  Pulse
	mPulse[1] 	= (ipulse1) << 1;	
	TCCR1B 		|= 0b00000001;			// Enable CLK : CLK=12MHz
}


void ir_send_pulseEx(void)
{
	unsigned char i;
	unsigned char index;


	if (mPulseRepeat==0)
	{
		mFunctionMode = 0;	// Change it to Idle mode
		return;
	}
	else if (mPulseRepeat != 0xff)
	{
		mPulseRepeat --;
	}

	// Set Carrier Frequencey
	OCR1A   = mPulseFreq; 

	TIMSK &= ~0x01;		// Turn OFF TIMER0 Interrupt
	UCSRB &= ~0x80;		// Turn OFF UART-RX interrupt

	for (i=0; i<mPulseLen; i++)
	{
		index = mPulseCode[(i>>1)];
		if ((i & 0x01)==0)
		{
			index = index >> 4;
		}
		index &= 0x07;
		ir_send_pulse(mPulseTable[index][0],mPulseTable[index][1]);
		ir_wait_tx();
	}

	UCSRB |= 0x80;		// Turn ON UART-RX interrupt
	TIMSK |= 0x01;		// Turn ON TIMER0 Interrupt

	if (mPulseRepeat==0)
	{
		mFunctionMode = 0;	// Change it to Idle mode
	}

}


void ir_init_send_pulse(char *input)
{
	mFunctionMode = 1;					// Send Pulse Function

	// decode pronto_code from CmdQ
	decode_cpronto_code(input);
//	decode_pronto_code(input);

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

void ir_capture_pulse(void)
{

//	When Capture Pulse is enabled
//	This function should be call @ every 200mS

	unsigned char i;
	unsigned char index;
	mCaptureState ++;
	unsigned int tPulse;

	// 2second time out OR buffer full
	if ((mCaptureState>mCaptureRepeat) || (mCaptureIndex==sizeof(mCapturePulse)/2))
	{
		TCCR1B = 0b00000000;	// Disable Input Capture
		index = mCaptureIndex & 0xff;
		mFunctionMode = 0;	// Change it to Idle mode
		u_puts(LinuxCmd);
		u_puts("'C0' '");		// Capture data
		u_putHexWord(CaptureFreq);
		u_puts("' '");		// Capture data
		u_putHexByte(index,0x00);
		u_puts("' '");		// Capture data
		for (i=0; i<index; i++)
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
	}
	
}
