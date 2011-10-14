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
//Mini STM32�����巶������16
//IIC ʵ��
//����ԭ��@ALIENTEK
//2010.6.8	

//Ҫд�뵽24c02���ַ�������
const u8 TEXT_Buffer[]={"MiniSTM32 IIC TEST"};
#define SIZE sizeof(TEXT_Buffer)	 
int main(void)
{		
	u8 key;
	u16 i=0;
	u8 datatemp[SIZE];
			   
  	Stm32_Clock_Init(9);//ϵͳʱ������
	delay_init(72);		//��ʱ��ʼ��
	uart_init(72,9600); //����1��ʼ��   
	LED_Init();	   		//LED��ʼ��
	KEY_Init();	   		//������ʼ��
	LCD_Init();    		//TFTLCDҺ����ʼ��
	AT24CXX_Init();		//IIC��ʼ�� 


 	POINT_COLOR=RED;//��������Ϊ��ɫ	   
	LCD_ShowString(60,50,"Mini STM32");	
	LCD_ShowString(60,70,"IIC TEST");	
	LCD_ShowString(60,90,"ATOM@ALIENTEK");
	LCD_ShowString(60,110,"2010/6/10");			 
	while(AT24CXX_Check())//��ⲻ��24c02
	{
		LCD_ShowString(60,130,"24C02 Check Failed!");
		delay_ms(500);
		LCD_ShowString(60,130,"Please Check!      ");
		delay_ms(500);
		LED0=!LED0;//DS0��˸
	}
	LCD_ShowString(60,130,"24C02 Ready!");
	//��ʾ��ʾ��Ϣ
	LCD_ShowString(60,150,"KEY0:Write KEY2:Read");

	POINT_COLOR=BLUE;//��������Ϊ��ɫ	  
	while(1)
	{
		key=KEY_Scan();
		if(key==1)//KEY0����,д��24C02
		{
			LCD_Fill(0,170,239,319,WHITE);//�������    
 			LCD_ShowString(60,170,"Start Write 24C02....");
			AT24CXX_Write(0,(u8*)TEXT_Buffer,SIZE);
			LCD_ShowString(60,170,"24C02 Write Finished!");//��ʾ�������
		}
		if(key==3)//KEY1����,��ȡ�ַ�������ʾ
		{
 			LCD_ShowString(60,170,"Start Read 24C02.... ");
			AT24CXX_Read(0,datatemp,SIZE);
			LCD_ShowString(60,170,"The Data Readed Is:  ");//��ʾ�������
			LCD_ShowString(60,190,datatemp);//��ʾ�������ַ���
		}
		i++;
		delay_ms(1);
		if(i==200)
		{
			LED0=!LED0;//��ʾϵͳ��������	
			i=0;
		}		   
	}
}






















