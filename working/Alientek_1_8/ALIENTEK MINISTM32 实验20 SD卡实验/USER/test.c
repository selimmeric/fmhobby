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
#include "24l01.h"
#include "mmc_sd.h"
//Mini STM32开发板范例代码20
//SD卡 实验
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
													 		    									    		    	 		  
u8 buf[512];//SD卡数据缓存区
int main(void)
{	
	u32 sd_size;
	u8 t=0;					    
  	Stm32_Clock_Init(9);//系统时钟设置
	delay_init(72);		//延时初始化
	uart_init(72,9600); //串口1初始化  	  
	LCD_Init();			//初始化液晶 
	LED_Init();         //LED初始化		  													    
					   
 	POINT_COLOR=RED;//设置字体为红色	   
	LCD_ShowString(60,50,"Mini STM32");	
	LCD_ShowString(60,70,"SD Card TEST");	
	LCD_ShowString(60,90,"ATOM@ALIENTEK");
	LCD_ShowString(60,110,"2011/1/1");				 
	while(SD_Init()!=0)//检测不到SD卡
	{
		LCD_ShowString(60,130,"SD Card Failed!");
		delay_ms(500);
		LCD_ShowString(60,130,"Please Check!      ");
		delay_ms(500);
		LED0=!LED0;//DS0闪烁
	}
	//检测SD卡成功 											    
	LCD_ShowString(60,130,"SD Card Checked OK ");
	LCD_ShowString(60,150,"SD Card Size:    Mb");
	sd_size=SD_GetCapacity();
	LCD_ShowNum(164,150,sd_size>>20,4,16);//显示SD卡容量
	while(1)
	{
		if(t==30)//每6s钟执行一次
		{
			if(SD_ReadSingleBlock(0,buf)==0)//读取MBR扇区
			{	
				LCD_ShowString(60,170,"USART1 Sending Data...");
				printf("SECTOR 0 DATA:\n");
				for(sd_size=0;sd_size<512;sd_size++)printf("%x ",buf[sd_size]);//打印MBR扇区数据    	   
				printf("\nDATA ENDED\n");
				LCD_ShowString(60,170,"USART1 Send Data Over!");
			}
			t=0;
		}   
		t++;
		delay_ms(200);
		LED0=!LED0;
	}
}


				 






