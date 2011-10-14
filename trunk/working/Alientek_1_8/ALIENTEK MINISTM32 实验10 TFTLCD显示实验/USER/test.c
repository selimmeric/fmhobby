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
//Mini STM32开发板范例代码10
//TFTLCD显示 实验
//正点原子@ALIENTEK
//技术论坛:www.openedv.com		
int main(void)
{		
	u8 x=0;
  	Stm32_Clock_Init(9);//系统时钟设置
	delay_init(72);		//延时初始化
	uart_init(72,9600); //串口1初始化  
	LED_Init();
 	LCD_Init();
	POINT_COLOR=RED;				 	
  	while(1) 
	{		 
		switch(x)
		{
			case 0:LCD_Clear(WHITE);break;
			case 1:LCD_Clear(BLACK);break;
			case 2:LCD_Clear(BLUE);break;
			case 3:LCD_Clear(RED);break;
			case 4:LCD_Clear(MAGENTA);break;
			case 5:LCD_Clear(GREEN);break;
			case 6:LCD_Clear(CYAN);break;

			case 7:LCD_Clear(YELLOW);break;
			case 8:LCD_Clear(BRRED);break;
			case 9:LCD_Clear(GRAY);break;
			case 10:LCD_Clear(LGRAY);break;
			case 11:LCD_Clear(BROWN);break;
		}
		POINT_COLOR=RED;	  
		LCD_ShowString(30,50,"Mini STM32 ^_^");	
		LCD_ShowString(30,70,"2.4'/2.8' TFTLCD TEST");	
		LCD_ShowString(30,90,"ATOM@ALIENTEK");
		LCD_ShowString(30,110,"2010/12/30");	    					 
	    x++;
		if(x==12)x=0;
		LED0=!LED0;					 
		delay_ms(1000);	
	}											    
}

























