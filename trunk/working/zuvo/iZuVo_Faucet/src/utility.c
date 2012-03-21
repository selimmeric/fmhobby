//----------------------------------------------------------------
//----------------------------------------------------------------
// Module name: User Utility
//
// Copyright 2008 Fong Ming as an  unpublished work. 
// All Rights Reserved.
//
// The information contained herein is confidential 
// property of Company. The user, copying, transfer or 
// disclosure of such information is prohibited except
// by express written agreement with Company.
//
// First written on 2008-09-23 by Fong Ming
//
//----------------------------------------------------------------
//----------------------------------------------------------------



//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// Include section
// Add all #includes here
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#include "utility.h"

extern void uart_tx(unsigned char ibyte);

/*
//----------------------------------------------------------------------------
// Function Prototype - User Library
//----------------------------------------------------------------------------

INT16U  u_abs(INT16S a);
INT16S  u_sign(INT16S x);
INT8U   u_toupper(INT8U c);
INT8U   u_hex2dec(INT8U c);
INT16U  u_asc2uint(INT8U len, INT8U *sptr);
void    u_putch(INT8U abyte);
void 	u_puts( char *sptr); 
void    u_putHexByte(INT8U c, INT8U t); 
void    u_putHexWord(INT16U w);
void    u_putDecByte(INT8U num);
void    u_putDecWord(INT16U num); 
void    u_Lputch(INT8U abyte);
void    u_Delay(INT8U iDelay);

*/


#if (false)
//----------------------------------------------------------------------------
// Function name	: u_delay(int a)
//    returns		: void
//    arg1			: none
//    arg2			: none
// Created by		: Fong Ming
// Date created		: 2002-03-30
// Description		: return a Absolute value  
//
// 
// Affected 
// Global Variable  : none  
// Modified           
// Global Variable  : none
// Function Called  : none
//                       
//                       
// Assumption       : 
//                    
// Notes			:
//----------------------------------------------------------------------------
void u_delay(unsigned int iDelay){ 
	//delay n 100us 
	unsigned int x; 

	while(iDelay--){ 
	x=130; 		// 8MHz Clock
	while(x--); 
	} 
} 


//----------------------------------------------------------------------------
// Function name	: int abs(int a)
//    returns		: int
//    arg1			: int
//    arg2			: none
// Created by		: Fong Ming
// Date created		: 2002-03-30
// Description		: return a Absolute value  
//
// 
// Affected 
// Global Variable  : none  
// Modified           
// Global Variable  : none
// Function Called  : none
//                       
//                       
// Assumption       : 
//                    
// Notes			:
//----------------------------------------------------------------------------

INT16U u_abs(INT16S a)
{
	if(a < 0)
		return -a;
	return (INT16U) a;
}

//----------------------------------------------------------------------------
// Function name	: INT16S sign(INT16S x)
//    returns		: 1 for Pos, -1 for Neg
//    arg1			: INT16S
//    arg2			: none
// Created by		: Fong Ming
// Date created		: 2002-03-30
// Description		: return Sign of int
//
// Affected 
// Global Variable  : none  
// Modified           
// Global Variable  : none
// Function Called  : none
//                       
// Assumption        : 
//                     
// Notes			    :
//----------------------------------------------------------------------------

INT16S u_sign(INT16S x) 
{
	if (x<0) return (-1);
	return(1);
}


//----------------------------------------------------------------------------
// Function name		: static INT16U asc2uint(INT8U len, INT8U *sptr)
//   returns		: A 16 bit unsigned integer
//    arg1			: len, number of alphanumeric character
//					: to be converted.
//    arg2			: sptr, pointer that points to alphanumeric string
// Created by		: Derek Tong
// Date created		: 16 May 2002
// Description		: Convert alphanumeric string to hex. value
// Notes				:
//----------------------------------------------------------------------------
INT16U u_asc2uint(INT8U len, INT8U *sptr)
{
	INT8U i;
	INT16U temp;

	temp = u_hex2dec(*sptr++);
	for(i = 1; i < len; i++)
		temp = temp * 0x10 + u_hex2dec(*sptr++);
	return (temp);
}

//----------------------------------------------------------------------------
// Function name	: INT8U hex2dec(INT8U c) 
//    returns		: INT8U
//    arg1			: INT8U
//    arg2			: none
// Created by		: Fong Ming
// Date created		: 2002-03-30
// Description		: Convert hexadecimal ASCII char. to 
//                    hexadecimal number
//
// Affected 
// Global Variable  : none  
// Modified           
// Global Variable  : none
// Function Called  : none
//                       
// Assumption        : 
//                     
// Notes			 :
//----------------------------------------------------------------------------

INT8U u_hex2dec(INT8U c) 
{
	if ((c>='0') && (c<='9')) 
    {
		return (c-0x30);
	}else if ((c>='A') && (c<='F')) 
    {
		return (c-'A'+10);
	}else if ((c>='a') && (c<='f')) 
    {
		return (c-'a'+10);
	}

	return 0;
}



//----------------------------------------------------------------------------
// Function name	: void u_putHexWord(INT16U w)
//    returns		: none
//    arg1			: INT16U 
//    arg2			: none
// Created by		: Fong Ming
// Date created		: 2002-03-30
// Description		: Print INT16Ueger number 
//                     to debug UART port.
//
// Affected 
// Global Variable  : none  
// Modified           
// Global Variable  : none
// Function Called  : u_putHexByte
//                       
// Assumption       :
//                     
//                     
// Notes			:
//----------------------------------------------------------------------------

void u_putHexWord(INT16U w)
{	
    u_putHexByte((INT8U) (w>>8),0);
	u_putHexByte((INT8U) (w & 0xff),0);
    u_putch(' ');
}
#endif

#if false

//----------------------------------------------------------------------------
// Function name	: void u_putDecByte(INT8U c) 
//    returns		: none
//    arg1			: INT8U pointer
//    arg2			: none
// Created by		: Fong Ming
// Date created		: 2002-03-30
// Description		: Print unsigned decimal number 
//                     to debug UART port.
//
// Affected 
// Global Variable  : none  
// Modified           
// Global Variable  : none
// Function Called  : u_putch
//                       
//                     
// Notes			:
//----------------------------------------------------------------------------


void u_putDecByte(INT8U num)
{
    INT8U temp;

    if(num >= 100)
    {
        temp=num/100;
        u_putch(temp+'0');
        temp=temp*100;
        num-=temp;
    }
    else
    {
        u_putch('0');
    }

    if(num >=10)
    {
        temp=num/10;
        u_putch(temp+'0');
        temp=temp*10;
        num-=temp;
    }
    else
    {
        u_putch('0');
    }

    u_putch(num+'0');

}

//----------------------------------------------------------------------------
// Function name	: void u_putDecWord(INT16U num) 
//    returns		: none
//    arg1			: INT8U pointer
//    arg2			: none
// Created by		: Fong Ming
// Date created		: 2002-03-30
// Description		: Print unsigned decimal number 
//                     to debug UART port.
//
// Affected 
// Global Variable  : none  
// Modified           
// Global Variable  : none
// Function Called  : u_putch
//                       
//                     
// Notes			:
//----------------------------------------------------------------------------


void u_putDecWord(INT16U num) 
{

    INT16U temp;

    if(num >= 10000)
    {
        temp=num/10000;
        u_putch(temp+'0');
        temp=temp*10000;
        num-=temp;
    }
    else
    {
        u_putch('0');
    }

    if(num >= 1000)
    {
        temp=num/1000;
        u_putch(temp+'0');
        temp=temp*1000;
        num-=temp;
    }
    else
    {
        u_putch('0');
    }

    if(num >= 100)
    {
        temp=num/100;
        u_putch(temp+'0');
        temp=temp*100;
        num-=temp;
    }
    else
    {
        u_putch('0');
    }

    if(num >=10)
    {
        temp=num/10;
        u_putch(temp+'0');
        temp=temp*10;
        num-=temp;
    }
    else
    {
        u_putch('0');
    }

    u_putch(num+'0');


}

#endif

//----------------------------------------------------------------------------
// Function name	: void u_putHexByte(INT8U c) 
//    returns		: none
//    arg1			: INT8U pointer
//    arg2			: none
// Created by		: Fong Ming
// Date created		: 2002-03-30
// Description		: Print unsigned hexadecmial number 
//                     to debug UART port.
//
// Affected 
// Global Variable  : none  
// Modified           
// Global Variable  : none
// Function Called  : u_putch
//                       
//                     
// Notes			:
//----------------------------------------------------------------------------

void u_putHexByte(INT8U c, INT8U t) 
{
	static const char STR_HEX[]="0123456789ABCDEF";
	u_putch(STR_HEX[(c&0xF0)>>4]);
	u_putch(STR_HEX[c&0x0F]);

    if (t != 0)
        u_putch(t);

}





//----------------------------------------------------------------------------
// Function name    : void putch(INT8U abyte) 
//    returns		: none
//    arg1			: INT8U
//    arg2			: none
// Created by		: Fong Ming
// Date created		: 2002-03-30
// Description		: Send the specified data byte to UART
//
// Affected 
// Global Variable  : none  
// Modified           
// Global Variable  : none
// Function Called  : none
//                       
// Assumption       : TXREG = UART Transmission Registor
//                     
// Notes			:
//----------------------------------------------------------------------------
//  AVR UART
//	UCSRB |= 0x08; // enable transmitter
//    while (!(UCSRA & 0x20))
//	{
//		// do nothing
//	}
//	
//    UDR = abyte;

void u_putch(INT8U abyte) 
{
	uart_tx(abyte);

}
//----------------------------------------------------------------------------
// Function name	: void puts(const char //sptr) 
//    returns		: none
//    arg1			: INT8U pointer
//    arg2			: none
// Created by		: Fong Ming
// Date created		: 2002-03-30
// Description		: Put string to UART. String must 
//                    terminated by null character
//
// Affected 
// Global Variable  : none  
// Modified           
// Global Variable  : none
// Function Called  : u_putch
//                       
// Assumption       :   
//                     
// Notes			:
//----------------------------------------------------------------------------

void u_puts( INT8U *sptr) 
{
	while(*sptr != 0) 
        u_putch(*sptr++);
}

#if false 
INT8U u_toupper(INT8U c)

{

	if ((c>='a') && (c<='z'))		//convert lower case to upper case
		return(c-0x20);
	else
		return(c);

}

#endif

//----------------------------------------------------------------------------
//  User Library END
//----------------------------------------------------------------------------

