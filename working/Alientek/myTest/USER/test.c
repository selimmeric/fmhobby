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

u8 mPrintBuf[80];

int main(void)
{		
	u8 x=0;
	u8 t;
	u8 len;	

  	Stm32_Clock_Init(9);//ϵͳʱ������
	delay_init(72);		//��ʱ��ʼ��
	uart_init(72,9600); //����1��ʼ��  

	LED_Init();
 	LCD_Init();
	KEY_Init();          //��ʼ���밴�����ӵ�Ӳ���ӿ�
	Timerx_Init(2000,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms  


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
			len=USART_RX_STA&0x3f;//�õ��˴ν��յ������ݳ���
			printf("\nRx message is :\n");
			for(t=0;t<len;t++)
			{
				USART1->DR=USART_RX_BUF[t];
				while((USART1->SR&0X40)==0);//�ȴ����ͽ���
			}
			printf("\n\n");//���뻻��
			USART_RX_STA=0;
			LCD_ShowLine(10,8,	"UART RX :");	
			USART_RX_BUF[len]=0x00;
			USART_RX_BUF[len+1]=0x00;
			LCD_ShowString(90,160, USART_RX_BUF);

		}		 
		t=KEY_Scan();//�õ���ֵ
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

























