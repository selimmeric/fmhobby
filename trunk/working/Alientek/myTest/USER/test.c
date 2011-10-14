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

u8 mPrintBuf[80];

int main(void)
{		
	u8 x=0;
	u8 t;
	u8 len;	

  	Stm32_Clock_Init(9);//系统时钟设置
	delay_init(72);		//延时初始化
	uart_init(72,9600); //串口1初始化  

	LED_Init();
 	LCD_Init();
	KEY_Init();          //初始化与按键连接的硬件接口
	Timerx_Init(2000,7199);//10Khz的计数频率，计数到5000为500ms  


	LCD_Clear(WHITE);				
	POINT_COLOR=RED;		

//	LCD_ShowLine(30,0,	"Mini STM32 ^_^");	
	LCD_ShowLine(30,1,	"FM@ALIENTEK 2010/10/22");
	LCD_ShowLine(30,2,	"1. 2.4 TFTLCD TEST");	
	LCD_ShowLine(30,3,	"2. UART (9600) test");	
	LCD_ShowLine(30,4,	"3. Key test");	
	sprintf(mPrintBuf,"%s %s",__DATE__,__TIME__);	
	LCD_ShowLine(30,5,  mPrintBuf);
				 	
	LCD_DrawLine(0,140,300,140);
	POINT_COLOR=BLUE;		
  	while(1) 
	{	
	
		// Process UART Command
		if(USART_RX_STA&0x80)
		{					   
			len=USART_RX_STA&0x3f;//得到此次接收到的数据长度
			printf("\nRx message is :\n");
			for(t=0;t<len;t++)
			{
				USART1->DR=USART_RX_BUF[t];
				while((USART1->SR&0X40)==0);//等待发送结束
			}
			printf("\n\n");//插入换行
			USART_RX_STA=0;
			LCD_ShowLine(10,8,	"UART RX :");	
			USART_RX_BUF[len]=0x00;
			USART_RX_BUF[len+1]=0x00;
			LCD_ShowString(90,160, USART_RX_BUF);

		}		 
		t=KEY_Scan();//得到键值
	   	if(t)
		{						   
			switch(t)
			{				 
				case 1:
					LED0=!LED0;
					LCD_ShowLine(10,10,	"KEY 1");	
					break;
				case 2:
					LED1=!LED1;
					LCD_ShowLine(10,10,	"KEY  2");	
					break;
				case 3:				
					LED0=!LED0;
					LED1=!LED1;
					LCD_ShowLine(10,10,	"KEY   3");	
					break;
			}
		} 


	    x++;
		if(x==12)x=0;
	}											    
}

























