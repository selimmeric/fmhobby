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
#include "rtc.h"
#include "wkup.h"
#include "adc.h"
#include "dma.h"
#include "24cxx.h"
#include "flash.h"
#include "touch.h" 		   
//Mini STM32开发板-扩展实验2
//触摸屏硬件检测实验
//正点原子@ALIENTEK
//2010.7.13	
						
void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);//清屏   
 	POINT_COLOR=BLUE;//设置字体为蓝色 
	LCD_ShowString(216,0,"RST");//显示清屏区域
  	POINT_COLOR=RED;//设置画笔蓝色 
}	 		  
int main(void)
{	
	u8 key;
	u8 i=0;	
 	Stm32_Clock_Init(9);//系统时钟设置
	delay_init(72);		//延时初始化
	uart_init(72,9600); //串口1初始化  	  
	LCD_Init();			//初始化液晶		  
	KEY_Init();			//按键初始化
	LED_Init();         //LED初始化

 	POINT_COLOR=RED;//设置字体为蓝色	   
	LCD_ShowString(60,50,"Mini STM32");	
	LCD_ShowString(60,70,"TOUCH TEST");	
	LCD_ShowString(60,90,"ATOM@ALIENTEK");
	LCD_ShowString(60,110,"2010/6/13");	
	LCD_ShowString(60,130,"Press KEY0 to Adjust");	
	Touch_Init();
	delay_ms(1500);
	Load_Drow_Dialog();
	while(1)
	{	
		key=KEY_Scan();
		if(Pen_Point.Key_Sta==Key_Down)//触摸屏被按下
		{
			Pen_Int_Set(0);//关闭中断
			do
			{
				Convert_Pos();
				LCD_ShowNum(100,100,Pen_Point.X,4,16);//显示数值
				LCD_ShowNum(100,120,Pen_Point.Y,4,16);//显示数值	   
				Pen_Point.Key_Sta=Key_Up;
				if(Pen_Point.X0>216&&Pen_Point.Y0<16)Load_Drow_Dialog();//清除
				else 
				{
					Draw_Big_Point(Pen_Point.X0,Pen_Point.Y0);//画图	    
					GPIOC->ODR|=1<<1;    //PC1 上拉	   
				}
			}while(PEN==0);//如果PEN一直有效,则一直执行
			Pen_Int_Set(1);//开启中断
		}else delay_ms(1);
		if(key==1)//KEY0按下,则执行校准程序
		{
			LCD_Clear(WHITE);//清屏
		    Touch_Adjust();  //屏幕校准 
			Save_Adjdata();	 
			Load_Drow_Dialog();
		}
		i++;
		if(i==200)
		{
			i=0;
			LED0=!LED0;
		}
	};	   		  
}


				 






