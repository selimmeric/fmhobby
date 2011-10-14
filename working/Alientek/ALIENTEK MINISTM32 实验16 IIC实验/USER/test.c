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
//Mini STM32开发板范例代码16
//IIC 实验
//正点原子@ALIENTEK
//2010.6.8	

//要写入到24c02的字符串数组
const u8 TEXT_Buffer[]={"MiniSTM32 IIC TEST"};
#define SIZE sizeof(TEXT_Buffer)	 
int main(void)
{		
	u8 key;
	u16 i=0;
	u8 datatemp[SIZE];
			   
  	Stm32_Clock_Init(9);//系统时钟设置
	delay_init(72);		//延时初始化
	uart_init(72,9600); //串口1初始化   
	LED_Init();	   		//LED初始化
	KEY_Init();	   		//按键初始化
	LCD_Init();    		//TFTLCD液晶初始化
	AT24CXX_Init();		//IIC初始化 


 	POINT_COLOR=RED;//设置字体为蓝色	   
	LCD_ShowString(60,50,"Mini STM32");	
	LCD_ShowString(60,70,"IIC TEST");	
	LCD_ShowString(60,90,"ATOM@ALIENTEK");
	LCD_ShowString(60,110,"2010/6/10");			 
	while(AT24CXX_Check())//检测不到24c02
	{
		LCD_ShowString(60,130,"24C02 Check Failed!");
		delay_ms(500);
		LCD_ShowString(60,130,"Please Check!      ");
		delay_ms(500);
		LED0=!LED0;//DS0闪烁
	}
	LCD_ShowString(60,130,"24C02 Ready!");
	//显示提示信息
	LCD_ShowString(60,150,"KEY0:Write KEY2:Read");

	POINT_COLOR=BLUE;//设置字体为蓝色	  
	while(1)
	{
		key=KEY_Scan();
		if(key==1)//KEY0按下,写入24C02
		{
			LCD_Fill(0,170,239,319,WHITE);//清除半屏    
 			LCD_ShowString(60,170,"Start Write 24C02....");
			AT24CXX_Write(0,(u8*)TEXT_Buffer,SIZE);
			LCD_ShowString(60,170,"24C02 Write Finished!");//提示传送完成
		}
		if(key==3)//KEY1按下,读取字符串并显示
		{
 			LCD_ShowString(60,170,"Start Read 24C02.... ");
			AT24CXX_Read(0,datatemp,SIZE);
			LCD_ShowString(60,170,"The Data Readed Is:  ");//提示传送完成
			LCD_ShowString(60,190,datatemp);//显示读到的字符串
		}
		i++;
		delay_ms(1);
		if(i==200)
		{
			LED0=!LED0;//提示系统正在运行	
			i=0;
		}		   
	}
}






















