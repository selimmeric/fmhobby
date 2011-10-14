#include <htc.h>
#include "zuvo_300.h"

// RA0 - ICSP_DATA
// RA1 - ICSP_CLK
// RA2 - Input with Pull Up
// RA3 - MCLR
// RA4 - Output
// RA5 - Input

// RC0 - Output
// RC1 - Output
// RC2 - Output
// RC3 - Output
// RC4 - Output
// RC5 - Output


// Declare Global variable

// State variable
//unsigned char mState_UVL=0;
//unsigned char mState_System=0;
//unsigned int  mState_DelayCnt=0;

// Timer Flag
unsigned char mTimer_Flag0=0;		// 10mS flag
unsigned char mFS_Cnt=0;			// Flow Sensor Count

//unsigned int mFlowVol=0;			// Flow Volume

unsigned long mRFID=0;				// RFID

// Output variable
unsigned char mLED_Color=0;
unsigned char mUVL=0;
unsigned char mBuzzer=0;


// Declare Function Prototype
void system_input(void);
void system_process(void);
void system_output(void);

void 	LED_RGB(unsigned char ivalue);
void 	Buzzer(unsigned char iVal);
void 	UVL(unsigned char iVal);

unsigned char FS_RD(unsigned char iFS);
unsigned long RFID_RD(void);

void test_io(void)
{

}

void main (void)
{ 

	unsigned long i=0,j=0;
	unsigned char tmp=0;
	unsigned long UVLCnt=0;

	// Initialize IO PORT Direction / Initial Value

	OSCCON		= 0x72;		// select internal osc. 8MHz 0x72

//	TRISA 		= ~0x10;
	ANSELA		= 0x00;
	TRISA		= 0xff;
	TRISC		= ~0xff;	
	OPTION_REG	= 0x82;			// Enable internal pull , 
								// TMR0 PS = 8	, TMR0 OverFlow = 1/8M * 4 * 8 * 256 = 1.024mS

//	WPUA 		= 0x24;			// Internal Pull-Up
//	WDTCON		= 0x01;			// WDT @16ms

	// Initialize Time

	T0IE = 1;		// Enable interrupt on TMR0 overflow
//	INTEDG = 1;		// falling edge trigger the interrupt
//	INTE = 1;		// enable the external interrupt
	GIE = 1;		// Global interrupt enable

	// Initialize System State Variable

	mLED_Color = 0x00;
	mUVL	   = 0x01;
	mBuzzer    = 0x00;

	// Start Task

	LATC = 0x1c;
	LATA = 0xff;

	while (1)
	{

//		CLRWDT();

		if (mTimer_Flag0)		
		{
			// @ 10.24mS
			mTimer_Flag0 = 0;



			// --------------------------------------------------
			// Prepare Input Variable
			//---------------------------------------------------
//			system_input();
//			mRFID = RFID_RD();

			// --------------------------------------------------
			// Process state input
			//---------------------------------------------------
			system_process();

			// --------------------------------------------------
			// Output Variable to Device Output 
			//---------------------------------------------------
			system_output();
		
		}
	}

}

static void interrupt
isr(void)			// Here be interrupt function - the name is unimportant.
{
	static unsigned char tCntA = 0;
	static unsigned char pFS_IN= 0;
	unsigned char tFS_IN;
	
	if(T0IF) {				// timer interrupt @ 1.024ms
		T0IF = 0;			// clear the interrupt flag

		tFS_IN = PORTAbits.RA2;		

		if ((pFS_IN != tFS_IN) && (tFS_IN !=0))
			mFS_Cnt ++;
		pFS_IN = tFS_IN;

		tCntA++;
		if (tCntA>=10)
		{
			tCntA = 0;
			mTimer_Flag0 = 1;
		}
	}

	if(INTF) {				// did we see a button press?
		INTF = 0;			// clear the interrupt
	}

}
/*

void system_input(void)
{

	mRFID_CTag = RFID_RD();	

}
*/
// Call @ 10.24mS
void system_1s(void)
{
	static unsigned char tSec=60;

//	mBuzzer = 0x81;

	switch (tSec)
	{
		case 0:
			mLED_Color = 0x01;		// Solid Green
//			mBuzzer = 0x00;			// Buzzer OFF
			break;
		case 15:
			mLED_Color = 0x82;		// Blue Blink
//			mBuzzer = 0x00;			// Buzzer OFF
			break;
		case 30:
			mLED_Color = 0x02;		// Blue Solid
//			mBuzzer = 0x00;			// Buzzer ON
			break;
		case 45:
			mLED_Color = 0x05;		// Yellow Solid = R + G
//			mBuzzer = 0x81;			// Buzzer ON
			break;
		case 60:
			mLED_Color = 0x84;		// Red Blink
//			mBuzzer = 0x01;			// Buzzer ON
			break;
		case 75:
			mLED_Color = 0x04;		// Red Solid
//			mBuzzer = 0x00;			// Buzzer ON
			break;
		case 90:
			mLED_Color = 0xD4;		// Red/Yellow
//			mBuzzer = 0x00;			// Buzzer ON
			break;
		case 105:
			mLED_Color = 0x94;		// Red/Green
//			mBuzzer = 0x00;			// Buzzer ON
			break;

	}
	tSec ++;
	if (tSec >= 120)
		tSec = 0;

	mRFID = RFID_RD(); 

	if (mRFID ==0)
		mBuzzer = mBuzzer ^ 0x01;

}

void system_process(void)
{

	static unsigned int tTMR_1S_CNT=0;
	static unsigned int tDelay=0;

	unsigned char tFC=0;

	// Update acc. flow count
	tFC = FS_RD(mFS_Cnt);

	if (tFC != 0)
		tDelay=300;
	else if (tDelay > 0)
		tDelay--;
			
	if (tDelay == 0)
		mUVL = 0;
	else
		mUVL = 1;

//	mUVL = (mFS_Cnt >> 2) & 0x01;

	UVL(mUVL);


	tTMR_1S_CNT ++;	
	if (tTMR_1S_CNT >= (1/SYS_CYCLE))
	{
		tTMR_1S_CNT =0;
		system_1s();
	}

}

// Checked

void system_output(void)
{
	unsigned char tmp;
	unsigned char ttmp;
	static unsigned int tLED_FlashCnt=0;
	static unsigned int tBuzzer_FlashCnt=0;

	// UVL
//	UVL(mUVL);

	// LED Flashing
	if ((mLED_Color & 0x80) ==0)
		tLED_FlashCnt=0;
	else
		tLED_FlashCnt++;
	
	tmp = mLED_Color & 0x07;
	if ((tLED_FlashCnt>=LED_ON_PER) && (tLED_FlashCnt<=(LED_OFF_PER+LED_ON_PER)))
	{
		tmp = (mLED_Color >> 4) & 0x07;
	}
	else if (tLED_FlashCnt>(LED_OFF_PER+LED_ON_PER))
	{
		tLED_FlashCnt =0;
	}

	// Output LED
	LED_RGB(tmp); 


	// Buzzer Flashing

	if ((mBuzzer & 0x80) == 0)
		tBuzzer_FlashCnt=0;	
	else
		tBuzzer_FlashCnt++;

	ttmp = mBuzzer & 0x01;
	if ((tBuzzer_FlashCnt>=BUZZER_ON_PER) && (tBuzzer_FlashCnt<=(BUZZER_OFF_PER+BUZZER_ON_PER)))
	{
		ttmp = 0x00;
	}
	else if (tBuzzer_FlashCnt>(BUZZER_OFF_PER+BUZZER_ON_PER))
	{
		tBuzzer_FlashCnt =0;
	}

	// Output LED
	Buzzer(ttmp); 
	
}


// Output Function

void LED_RGB(unsigned char iVal)
{

	unsigned char tPort;

	tPort = LATC & (~(0x07<<2));
	tPort = tPort | ((~iVal & 0x07)<<2);
	LATC  = tPort;

}

void Buzzer(unsigned char iVal)
{
	unsigned char tPort;

	tPort = LATC & (~(1<<PIN_BUZZER));
	tPort = tPort | ((iVal & 0x01)<<PIN_BUZZER);
	LATC  = tPort;

}


void UVL(unsigned char iVal)
{
	unsigned char tPort;

	tPort = LATC & (~(1<<PIN_UVL));
	tPort = tPort | ((iVal & 0x01)<<PIN_UVL);
	LATC  = tPort;

}


// Input Process Function

unsigned char FS_RD(unsigned char iFS)
{
	static unsigned char pFS=0;
	unsigned int tFS=0;

	tFS = iFS;

	if (pFS > iFS)
		tFS += 0x100;

	tFS -= pFS;

	pFS = iFS;

	return (tFS & 0xff);

}


unsigned long RFID_RD(void)
{
	unsigned int i=0;
	unsigned long tRFID=0;;

	// Clear CLK, CMD pin
	LATA &= ~(1<<PIN_CMD);
	LATC &= ~(1<<PIN_CLK);

	// TD goes LOW - Tag detected
	// Reload Fifo
	LATA |= (1<<PIN_CMD);
	for (i=0; i<2; i++)
	{
		LATC |= ((1<<PIN_CLK));	
		LATC &= ~((1<<PIN_CLK));
	}
	LATA &= ~(1<<PIN_CMD);

	// Send 16 clk to read ID
	for (i=0; i<16; i++)
	{
		LATC |= ((1<<PIN_CLK));	
		LATC &= ~((1<<PIN_CLK));
		tRFID = tRFID << 1;
		if ((PORTA & (1<<PIN_DATA)))
			tRFID +=1;
	}

	return (tRFID);
}