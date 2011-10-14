#ifndef __TIMEPLAY_H
#define __TIMEPLAY_H
#include "sys.h"	  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//综合实验 驱动代码		 					    
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2011/1/26
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//********************************************************************************
//V1.1  
//修改LCD_ShowBigChar为画点方式实现
//////////////////////////////////////////////////////////////////////////////////	 
  
void LCD_ShowBigChar(u8 x,u16 y,u8 num);
void LCD_ShowBigNum(u8 x,u16 y,u8 num);	
void Date_Refresh(void);
void Time_Viewer(void);

#endif



