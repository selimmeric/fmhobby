
#pragma used+
sfrb TWBR=0;
sfrb TWSR=1;
sfrb TWAR=2;
sfrb TWDR=3;
sfrb ADCL=4;
sfrb ADCH=5;
sfrw ADCW=4;      
sfrb ADCSRA=6;
sfrb ADMUX=7;
sfrb ACSR=8;
sfrb UBRRL=9;
sfrb UCSRB=0xa;
sfrb UCSRA=0xb;
sfrb UDR=0xc;
sfrb SPCR=0xd;
sfrb SPSR=0xe;
sfrb SPDR=0xf;
sfrb PIND=0x10;
sfrb DDRD=0x11;
sfrb PORTD=0x12;
sfrb PINC=0x13;
sfrb DDRC=0x14;
sfrb PORTC=0x15;
sfrb PINB=0x16;
sfrb DDRB=0x17;
sfrb PORTB=0x18;
sfrb EECR=0x1c;
sfrb EEDR=0x1d;
sfrb EEARL=0x1e;
sfrb EEARH=0x1f;
sfrw EEAR=0x1e;   
sfrb UBRRH=0x20;
sfrb UCSRC=0X20;
sfrb WDTCR=0x21;
sfrb ASSR=0x22;
sfrb OCR2=0x23;
sfrb TCNT2=0x24;
sfrb TCCR2=0x25;
sfrb ICR1L=0x26;
sfrb ICR1H=0x27;
sfrw ICR1=0x26;   
sfrb OCR1BL=0x28;
sfrb OCR1BH=0x29;
sfrw OCR1B=0x28;  
sfrb OCR1AL=0x2a;
sfrb OCR1AH=0x2b;
sfrw OCR1A=0x2a;  
sfrb TCNT1L=0x2c;
sfrb TCNT1H=0x2d;
sfrw TCNT1=0x2c;  
sfrb TCCR1B=0x2e;
sfrb TCCR1A=0x2f;
sfrb SFIOR=0x30;
sfrb OSCCAL=0x31;
sfrb TCNT0=0x32;
sfrb TCCR0=0x33;
sfrb MCUCSR=0x34;
sfrb MCUCR=0x35;
sfrb TWCR=0x36;
sfrb SPMCR=0x37;
sfrb TIFR=0x38;
sfrb TIMSK=0x39;
sfrb GIFR=0x3a;
sfrb GICR=0x3b;
sfrb SPL=0x3d;
sfrb SPH=0x3e;
sfrb SREG=0x3f;
#pragma used-

#asm
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x80
	.EQU __sm_mask=0x70
	.EQU __sm_powerdown=0x20
	.EQU __sm_powersave=0x30
	.EQU __sm_standby=0x60
	.EQU __sm_ext_standby=0x70
	.EQU __sm_adc_noise_red=0x10
	.SET power_ctrl_reg=mcucr
	#endif
#endasm

#pragma used+

void delay_us(unsigned int n);
void delay_ms(unsigned int n);

#pragma used-

unsigned char CodeCnt;
unsigned char Receive[3];
unsigned int  IrCode;
unsigned char ValStr[5];

interrupt [2] void ext_int0_isr(void)
{

unsigned char i;

TCNT1=0;
TCCR1B=0x01;						

if(CodeCnt==0)
{									
TCNT1=0;
while(!PIND.2);					
TCCR1B=0x01;						
while (PIND.2)						
if(TIFR&0x04) goto RemExit;		
TCCR1B=0x00;						

if(TCNT1>18000	||TCNT1<12000	) 	
{									
goto RemExit;
}

CodeCnt=1;
IrCode=0;
}
else
{
if(CodeCnt<25)
{

while(!PIND.2)  					
if(TIFR&0x04)
{
CodeCnt=0;
goto RemExit;					
}	
TCCR1B=0x00;						
i=(CodeCnt-1)/8;

if (TCNT1>1200	 && TCNT1<1800	)
{
Receive[i]=Receive[i]>>1;
Receive[i]=Receive[i]+0x80;		
}
else
if(TCNT1>400		 && TCNT1<612		)
{
Receive[i]=Receive[i]>>1;		
}
else
{
CodeCnt=0;
goto RemExit;
}
if((CodeCnt&1)==0)
{								
IrCode>>=1;
if((Receive[i]&0xC0)==0xC0)
{								
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

char tx_buffer[8];

unsigned char tx_wr_index,tx_rd_index,tx_counter;

interrupt [14] void usart_tx_isr(void)
{
if (tx_counter)
{
--tx_counter;
UDR=tx_buffer[tx_rd_index];
if (++tx_rd_index == 8) tx_rd_index=0;
};
}

#pragma used+
void putchar(char c)
{
while (tx_counter == 8);
#asm("cli")
if (tx_counter || ((UCSRA & (1<<5))==0))
{
tx_buffer[tx_wr_index]=c;
if (++tx_wr_index == 8) tx_wr_index=0;
++tx_counter;
}
else
UDR=c;
#asm("sei")
}
#pragma used-

typedef char *va_list;

#pragma used+

char getchar(void);
void putchar(char c);
void puts(char *str);
void putsf(char flash *str);

char *gets(char *str,unsigned int len);

void printf(char flash *fmtstr,...);
void sprintf(char *str, char flash *fmtstr,...);
void snprintf(char *str, unsigned int size, char flash *fmtstr,...);
void vprintf (char flash * fmtstr, va_list argptr);
void vsprintf (char *str, char flash * fmtstr, va_list argptr);
void vsnprintf (char *str, unsigned int size, char flash * fmtstr, va_list argptr);
signed char scanf(char flash *fmtstr,...);
signed char sscanf(char *str, char flash *fmtstr,...);

#pragma used-

#pragma library stdio.lib

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

void main(void)
{

PORTB=0x00;
DDRB=0x07;

PORTC=0x00;
DDRC=0x00;

PORTD=0x00;
DDRD=0x00;

TCCR0=0x00;
TCNT0=0x00;

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

ASSR=0x00;
TCCR2=0x00;
TCNT2=0x00;
OCR2=0x00;

GICR|=0x40;
MCUCR=0x02;
GIFR=0x40;

TIMSK=0x00;

UCSRA=0x00;
UCSRB=0x48;
UCSRC=0x86;
UBRRH=0x00;
UBRRL=0x0C;

ACSR=0x80;
SFIOR=0x00;

#asm("sei")

CodeCnt=0;

putchar('A');
putchar('B');
putchar('C');

while (1)
{

delay_ms(500);
if(PORTB.0) PORTB.0=0; else PORTB.0=1;

if(CodeCnt>24)
{
CodeCnt=0;

ValToStr(IrCode);
puts(ValStr);
if(IrCode==0x123)
if(PORTB.1) PORTB.1=0; else PORTB.1=1;
if(IrCode==0x321)
if(PORTB.2) PORTB.2=0; else PORTB.2=1;
}
};
}
