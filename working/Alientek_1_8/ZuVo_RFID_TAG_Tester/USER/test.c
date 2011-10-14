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
 
//Mini STM32�����巶������10
//TFTLCD��ʾ ʵ��
//����ԭ��@ALIENTEK
//2010.6.5	


int main(void)
{		
  	Stm32_Clock_Init(9);//ϵͳʱ������
	delay_init(72);		//��ʱ��ʼ��
	uart_init(72,115200); //����1��ʼ��  
	uart_init2(36,4800); //����2��ʼ��  

	LED_Init();
 	LCD_Init();
	KEY_Init();          //��ʼ���밴�����ӵ�Ӳ���ӿ�
	Timerx_Init(9,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms  

	my_init();

  	while(1) 
	{	
	
		my_task();

	}	
	
	
											    
}

























