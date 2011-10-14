#include <stm32f10x_lib.h>
#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "key.h"
#include "exti.h"
#include "wdg.h"
#include "timer.h"
#include "lcd.h"	   
#include "mytest.h"

void LCD_ShowLine(u8 x,u16 y, u8 *p)		 //显示一个字符串,16字体
{
	LCD_ShowString(0,y*20,"                                       ");
	LCD_ShowString(x, y*20,p);

}
















