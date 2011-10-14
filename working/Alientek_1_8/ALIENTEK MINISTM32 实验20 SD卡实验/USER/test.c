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
//Mini STM32�����巶������20
//SD�� ʵ��
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
													 		    									    		    	 		  
u8 buf[512];//SD�����ݻ�����
int main(void)
{	
	u32 sd_size;
	u8 t=0;					    
  	Stm32_Clock_Init(9);//ϵͳʱ������
	delay_init(72);		//��ʱ��ʼ��
	uart_init(72,9600); //����1��ʼ��  	  
	LCD_Init();			//��ʼ��Һ�� 
	LED_Init();         //LED��ʼ��		  													    
					   
 	POINT_COLOR=RED;//��������Ϊ��ɫ	   
	LCD_ShowString(60,50,"Mini STM32");	
	LCD_ShowString(60,70,"SD Card TEST");	
	LCD_ShowString(60,90,"ATOM@ALIENTEK");
	LCD_ShowString(60,110,"2011/1/1");				 
	while(SD_Init()!=0)//��ⲻ��SD��
	{
		LCD_ShowString(60,130,"SD Card Failed!");
		delay_ms(500);
		LCD_ShowString(60,130,"Please Check!      ");
		delay_ms(500);
		LED0=!LED0;//DS0��˸
	}
	//���SD���ɹ� 											    
	LCD_ShowString(60,130,"SD Card Checked OK ");
	LCD_ShowString(60,150,"SD Card Size:    Mb");
	sd_size=SD_GetCapacity();
	LCD_ShowNum(164,150,sd_size>>20,4,16);//��ʾSD������
	while(1)
	{
		if(t==30)//ÿ6s��ִ��һ��
		{
			if(SD_ReadSingleBlock(0,buf)==0)//��ȡMBR����
			{	
				LCD_ShowString(60,170,"USART1 Sending Data...");
				printf("SECTOR 0 DATA:\n");
				for(sd_size=0;sd_size<512;sd_size++)printf("%x ",buf[sd_size]);//��ӡMBR��������    	   
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


				 






