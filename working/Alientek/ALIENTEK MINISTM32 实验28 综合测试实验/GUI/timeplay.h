#ifndef __TIMEPLAY_H
#define __TIMEPLAY_H
#include "sys.h"	    
void LCD_ShowBigChar(u8 x,u16 y,u8 num);
void LCD_ShowBigNum(u8 x,u16 y,u8 num);	
void Date_Refresh(void);
void Time_Viewer(void);

#endif



