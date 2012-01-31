/*****************************************************
This program was produced by the
CodeWizardAVR V2.03.4 Standard
Automatic Program Generator
© Copyright 1998-2008 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : Decode For PT2262
Version : v3.0 ÔÚDemo2»ù´¡ÉÏ²ÉÓÃ¶à´ÎÏÂÑØÖÐ¶Ï½âÂë(25´Î)
Date    : 2008-8-26
Author  : Caijianqiang
Company : STEP
Comments: PT2262Õðµ´µç×èÎª4.7M£¬Èçµç×è²»Ò»ÑùÐè¸ÄÒ»ÏÂ³ÌÐòÊ±¼ä
          Á¬½Ó·½·¨ÊÇ½«PT2262µÄÊý¾ÝÊä³ö¶Ë¾­¹ýÈý¼«¹Ü·´Ïòºó
          Ö±½Ó½Óµ½MEGA8µÄINT0(PIND.2), ÐÅºÅÓëPT2262·´Ïò
          ½âÂë½á¹û·ÅÔÚReceive[]Àï, LLMMHH
          
Chip type           : ATmega8
Program type        : Application
Clock frequency     : 1.000000 MHz
Memory model        : Small
External RAM size   : 0
Data Stack size     : 256
*****************************************************/

#include <mega8.h>
#include <delay.h>

/*--------------------------------------------------*/
#define RUN   PORTB.0
#define LEDA  PORTB.1
#define LEDB  PORTB.2
#define IrDat PIND.2

/*--------------------------------------------------*/
#define MinSync	12000	// Í¬²½ÂëÏÂÏÞ
#define MaxSync 18000	// Í¬²½ÂëÉÏÏÞ
#define MinNarr 400		// Õ­Âö³åÏÂÏÞ
#define MaxNarr 612		// Õ­Âö³åÉÏÏÞ
#define MinWide 1200	// ¿íÂö³åÉÏÏÞ
#define MaxWide 1800	// ¿íÂö³åÉÏÏÞ

/*--------------------------------------------------*/
// Declare your global variables here
unsigned char CodeCnt;
unsigned char Receive[3];
unsigned int  IrCode;
unsigned char ValStr[5];

/*--------------------------------------------------*/
// External Interrupt 0 service routine
interrupt [EXT_INT0] void ext_int0_isr(void)
{
 // Place your code here
 unsigned char i;
 
 TCNT1=0;
 TCCR1B=0x01;						// ²âµÍµçÆ½¿í¶È
 
 if(CodeCnt==0)
 {									// ÕÒÍ¬²½Âë
  TCNT1=0;
  while(!IrDat);					// µÈ´ý¸ßµçÆ½
  TCCR1B=0x01;						// ²â¸ßµçÆ½¿í¶È
  while (IrDat)						// µÈ´ýµÍµçÆ½
   if(TIFR&0x04) goto RemExit;		// ³¬Ê±¾ÍÍË³ö
  TCCR1B=0x00;						// ÖÐÖ¹²âÁ¿
  
  if(TCNT1>MaxSync||TCNT1<MinSync) 	// ÅÐ¶ÏÍ¬²½ÂëµÄ¿í¶È
  {									// Í¬²½ÐÅºÅ´íÎó
   goto RemExit;
  }
  // Í¬²½ÐÅºÅ¼ì²â³É¹¦£¬×¼±¸¼ì²â24Î»Êý¾Ý(16770us)
  CodeCnt=1;
  IrCode=0;
 }
 else
 {
  if(CodeCnt<25)
  {
   // ¼ì²â24Î»Êý¾Ý
   while(!IrDat)  					// µÈ´ý±ä¸ßµçÆ½
    if(TIFR&0x04)
    {
     CodeCnt=0;
     goto RemExit;					// ³¬Ê±¾ÍÍË³ö
    }	
   TCCR1B=0x00;						// ÖÐÖ¹²âÁ¿
   i=(CodeCnt-1)/8;
   
   if (TCNT1>MinWide && TCNT1<MaxWide)
   {
    Receive[i]=Receive[i]>>1;
    Receive[i]=Receive[i]+0x80;		// ±£´æ1(1471us)
   }
   else
    if(TCNT1>MinNarr && TCNT1<MaxNarr)
    {
     Receive[i]=Receive[i]>>1;		// ±£´æ0(511us)
    }
    else
    {
     CodeCnt=0;
     goto RemExit;
    }
   if((CodeCnt&1)==0)
   {								// È¡Êý¾ÝÂë
   	IrCode>>=1;
   	if((Receive[i]&0xC0)==0xC0)
   	{								// 1
   	 IrCode|=0x800;
   	}
   }
   CodeCnt+=1;
  }
 }
 
 RemExit:
 TCCR1B=0x00;
 TIFR|=0x04;
}

/*--------------------------------------------------*/
#define RXB8 1
#define TXB8 0
#define UPE  2
#define OVR  3
#define FE   4
#define UDRE 5
#define RXC  7

#define FRAMING_ERROR       (1<<FE)
#define PARITY_ERROR        (1<<UPE)
#define DATA_OVERRUN        (1<<OVR)
#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE         (1<<RXC)

// USART Transmitter buffer
#define TX_BUFFER_SIZE 8
char tx_buffer[TX_BUFFER_SIZE];

#if TX_BUFFER_SIZE<256
unsigned char tx_wr_index,tx_rd_index,tx_counter;
#else
unsigned int tx_wr_index,tx_rd_index,tx_counter;
#endif

// USART Transmitter interrupt service routine
interrupt [USART_TXC] void usart_tx_isr(void)
{
 if (tx_counter)
 {
  --tx_counter;
  UDR=tx_buffer[tx_rd_index];
  if (++tx_rd_index == TX_BUFFER_SIZE) tx_rd_index=0;
 };
}

#ifndef _DEBUG_TERMINAL_IO_
// Write a character to the USART Transmitter buffer
#define _ALTERNATE_PUTCHAR_
#pragma used+
void putchar(char c)
{
 while (tx_counter == TX_BUFFER_SIZE);
 #asm("cli")
 if (tx_counter || ((UCSRA & DATA_REGISTER_EMPTY)==0))
 {
  tx_buffer[tx_wr_index]=c;
  if (++tx_wr_index == TX_BUFFER_SIZE) tx_wr_index=0;
  ++tx_counter;
 }
 else
  UDR=c;
 #asm("sei")
}
#pragma used-
#endif

// Standard Input/Output functions
#include <stdio.h>

/*--------------------------------------------------*/
void ValToStr(unsigned int Val)
{
 unsigned int i;
 
 i=Val;
 ValStr[0]=i/1000+0x30;
 i=i%1000;
 ValStr[1]=i/100+0x30;
 i=i%100;
 ValStr[2]=i/10+0x30;
 ValStr[3]=i%10+0x30;
 ValStr[4]=0;
}

/*--------------------------------------------------*/
void main(void)
{
 // Declare your local variables here
  
 // Input/Output Ports initialization
 // Port B initialization
 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=Out Func1=Out Func0=Out 
 // State7=T State6=T State5=T State4=T State3=T State2=0 State1=0 State0=0 
 PORTB=0x00;
 DDRB=0x07;

 // Port C initialization
 // Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
 // State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
 PORTC=0x00;
 DDRC=0x00;

 // Port D initialization
 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
 PORTD=0x00;
 DDRD=0x00;

 // Timer/Counter 0 initialization
 // Clock source: System Clock
 // Clock value: Timer 0 Stopped
 TCCR0=0x00;
 TCNT0=0x00;

 // Timer/Counter 1 initialization
 // Clock source: System Clock
 // Clock value: Timer 1 Stopped
 // Mode: Normal top=FFFFh
 // OC1A output: Discon.
 // OC1B output: Discon.
 // Noise Canceler: Off
 // Input Capture on Falling Edge
 // Timer 1 Overflow Interrupt: Off
 // Input Capture Interrupt: Off
 // Compare A Match Interrupt: Off
 // Compare B Match Interrupt: Off
 TCCR1A=0x00;
 TCCR1B=0x00;
 TCNT1H=0x00;
 TCNT1L=0x00;
 ICR1H=0x00;
 ICR1L=0x00;
 OCR1AH=0x00;
 OCR1AL=0x00;
 OCR1BH=0x00;
 OCR1BL=0x00;

 // Timer/Counter 2 initialization
 // Clock source: System Clock
 // Clock value: Timer 2 Stopped
 // Mode: Normal top=FFh
 // OC2 output: Disconnected
 ASSR=0x00;
 TCCR2=0x00;
 TCNT2=0x00;
 OCR2=0x00;

 // External Interrupt(s) initialization
 // INT0: On
 // INT0 Mode: Falling Edge
 // INT1: Off
 GICR|=0x40;
 MCUCR=0x02;
 GIFR=0x40;

 // Timer(s)/Counter(s) Interrupt(s) initialization
 TIMSK=0x00;

 // USART initialization
 // Communication Parameters: 8 Data, 1 Stop, No Parity
 // USART Receiver: Off
 // USART Transmitter: On
 // USART Mode: Asynchronous
 // USART Baud Rate: 4800
 UCSRA=0x00;
 UCSRB=0x48;
 UCSRC=0x86;
 UBRRH=0x00;
 UBRRL=0x0C;

 // Analog Comparator initialization
 // Analog Comparator: Off
 // Analog Comparator Input Capture by Timer/Counter 1: Off
 ACSR=0x80;
 SFIOR=0x00;

 // Global enable interrupts
 #asm("sei")
 
 CodeCnt=0;
 
 putchar('A');
 putchar('B');
 putchar('C');
 
 while (1)
 {
  // Place your code here
  delay_ms(500);
  if(RUN) RUN=0; else RUN=1;
   
  if(CodeCnt>24)
  {
   CodeCnt=0;
   // ½â³öÊý¾ÝÂë, Ðü¿Õ×÷0
   ValToStr(IrCode);
   puts(ValStr);
   if(IrCode==0x123)
   	if(LEDA) LEDA=0; else LEDA=1;
   if(IrCode==0x321)
   	if(LEDB) LEDB=0; else LEDB=1;
  }
 };
}
