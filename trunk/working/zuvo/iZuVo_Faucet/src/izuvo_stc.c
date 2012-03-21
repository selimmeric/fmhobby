
#include "izuvo_stc.h"

// STC register

sfr P1M1 = 0x91;
sfr P1M0 = 0x92;
sfr P0M1 = 0x93;
sfr P0M0 = 0x94;
sfr AUXR = 0x8e;
sfr	BRT	 = 0x9c;


// ***************************************************
// Global variable
//****************************************************

unsigned char mCmdQ[32];	 		// mCmdQ[0] = index

// ***************************************************
// STC UART 1 Routine
//****************************************************

void UART_one_Interrupt_Receive(void) interrupt 4
{
    unsigned char tdata  = 0;
    if(RI==1)
    {
        RI  =   0;
        tdata   =   SBUF;

	 	if ((tdata=='\r') || (tdata=='\n')) 
		{
			mCmdQ[0] |= 0x80;	// Set Cmd Rx Flag
		}

		if (mCmdQ[0] < ((sizeof(mCmdQ))-1))	 
		{
			mCmdQ[0]++;
			mCmdQ[mCmdQ[0]]=tdata;
		}	
    }
    else
    {
        TI  =  0;
    }
}

void uart_tx(INT8U abyte) 
{
    ES     =   0;  
    TI     =   0;  
    SBUF   =   abyte;
    while(TI ==0); 
    TI     =   0;  
    ES     =   1;  
}

void UART_one_init(void)
{

//	Init UART
	SCON  = 0x50;				// 8-bits UART No parity
	T2CON = 0x34;
	BRT	  =	RELOAD_COUNT;
	AUXR  =	0x11; 
	AUXR |=	0x04; 
    ES    =   1;    

}


// ***************************************************
// STC Init Routine
//****************************************************

void init_hardware(void)
{

	P1M0 = 0xff;
	P1M1 = 0x00;

}

void init_timer(void)
{

}

void init_system(void)
{

	unsigned char i ;

	for (i=0; i<sizeof(mCmdQ); i++)
	{
		mCmdQ[i]=0;
	}



}
