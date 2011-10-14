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
//Mini STM32������-��չʵ��2
//������Ӳ�����ʵ��
//����ԭ��@ALIENTEK
//2010.7.13	
						
void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);//����   
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
	LCD_ShowString(216,0,"RST");//��ʾ��������
  	POINT_COLOR=RED;//���û�����ɫ 
}	 		  
int main(void)
{	
	u8 key;
	u8 i=0;	
 	Stm32_Clock_Init(9);//ϵͳʱ������
	delay_init(72);		//��ʱ��ʼ��
	uart_init(72,9600); //����1��ʼ��  	  
	LCD_Init();			//��ʼ��Һ��		  
	KEY_Init();			//������ʼ��
	LED_Init();         //LED��ʼ��

 	POINT_COLOR=RED;//��������Ϊ��ɫ	   
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
		if(Pen_Point.Key_Sta==Key_Down)//������������
		{
			Pen_Int_Set(0);//�ر��ж�
			do
			{
				Convert_Pos();
				LCD_ShowNum(100,100,Pen_Point.X,4,16);//��ʾ��ֵ
				LCD_ShowNum(100,120,Pen_Point.Y,4,16);//��ʾ��ֵ	   
				Pen_Point.Key_Sta=Key_Up;
				if(Pen_Point.X0>216&&Pen_Point.Y0<16)Load_Drow_Dialog();//���
				else 
				{
					Draw_Big_Point(Pen_Point.X0,Pen_Point.Y0);//��ͼ	    
					GPIOC->ODR|=1<<1;    //PC1 ����	   
				}
			}while(PEN==0);//���PENһֱ��Ч,��һֱִ��
			Pen_Int_Set(1);//�����ж�
		}else delay_ms(1);
		if(key==1)//KEY0����,��ִ��У׼����
		{
			LCD_Clear(WHITE);//����
		    Touch_Adjust();  //��ĻУ׼ 
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


				 






