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
 
//Mini STM32开发板范例代码10
//TFTLCD显示 实验
//正点原子@ALIENTEK
//2010.6.5	


int main(void)
{		
  	Stm32_Clock_Init(9);//系统时钟设置
	delay_init(72);		//延时初始化
	uart_init(72,115200); //串口1初始化  
	uart_init2(36,4800); //串口2初始化  

	LED_Init();
 	LCD_Init();
	KEY_Init();          //初始化与按键连接的硬件接口
	Timerx_Init(9,7199);//10Khz的计数频率，计数到5000为500ms  

	my_init();

  	while(1) 
	{	
	
		my_task();

	}	
	
	
											    
}

























