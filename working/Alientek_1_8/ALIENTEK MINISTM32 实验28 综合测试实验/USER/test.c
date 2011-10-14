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
#include "remote.h"
#include "ds18b20.h"
#include "mouse.h"
#include "text.h"
#include "fat.h"
#include "fontupd.h"
#include "sysfile.h"
#include "spi.h"
#include "jpegbmp.h"
#include "gui.h"
#include "ptgame.h"
#include "picviewer.h"
#include "touchplay.h"
#include "timeplay.h"
#include "mouseplay.h"
#include "usbplay.h"
#include "wirelessplay.h"
#include "remoteplay.h"
#include "ebookplay.h"
//Mini STM32�����巶������28
//�ۺ� ʵ��
//����ԭ��@ALIENTEK
//������̳:www.openedv.com		


const u8 *COMPILED_DATE=__DATE__;//��ñ�������
const u8 *COMPILED_TIME=__TIME__;//��ñ���ʱ��
int main(void)
{	 	
	u8 i,t,key;
	u8 index=0;
	u16 detime=0;  	     	  					   
	Stm32_Clock_Init(9);//ϵͳʱ������
	delay_init(72);		//��ʱ��ʼ��
	uart_init(72,9600); //����1��ʼ��  	  
	LCD_Init();			//��ʼ��Һ��		  
    KEY_Init();			//������ʼ��
	LED_Init();         //LED��ʼ��	
	SPI_Flash_Init();   //SPI FLASHʹ��
	Touch_Init();
	key=KEY_Scan();	    //��ⰴ��
	switch(key)
	{   					    
		case 1://ǿ������ʱ��Ϊ����ʱ��
			POINT_COLOR=RED;   
			Auto_Time_Set();//����ʱ��Ϊ����ʱ��
			LCD_ShowString(20,20,"Set to Compiled Time OK!"); 
			delay_ms(500);
			LCD_Clear(WHITE);
			break;
		case 2://ǿ��У׼
			Touch_Adjust(); //������ǿ��У׼
			break;
		case 3://ǿ�Ƹ����ֿ�
			while(1)
			{
UPDF:			
				POINT_COLOR=RED;   
				SD_Init();
				while(FAT_Init())//FAT ����	 
				{
					SD_Init();			  
					LCD_ShowString(20,100,"SD/FAT ERROR");
					delay_ms(300); 
				}	  				 
				if(Update_Font())LCD_ShowString(20,100,"  NO Font!  ");
				else break; 
				delay_ms(300); 
			}
			break;		 
	} 	
////////////////////////��ʼӲ�����////////////////////////////
	POINT_COLOR=BLUE;   
	LCD_ShowString(20,0,"Start Hardware Checking..."); 	
	while(RTC_Init())   //���RTC�����Ƿ�����
	{
		POINT_COLOR=RED;     
		LCD_ShowString(20,20,"RTC ERROR    "); 	
		delay_ms(500);        	  								  
		LCD_ShowString(20,20,"RTC Trying...");
  		delay_ms(500); 
	}		  
	POINT_COLOR=BLUE;      
	LCD_ShowString(20,20,"RTC OK       ");//RTC���OK
	while(AT24CXX_Check())
	{
		POINT_COLOR=RED;     
		LCD_ShowString(20,40,"AT24C02 ERROR"); 	
		delay_ms(500);        	  								  
		LCD_ShowString(20,40,"Rechecking...");
  		delay_ms(500); 
	}  
	POINT_COLOR=BLUE;      
	LCD_ShowString(20,40,"AT24C02 OK   ");//AT24C02 ���OK 
	while(SPI_Flash_ReadID()!=0XEF14)//FLASH���
	{
		POINT_COLOR=RED;     
		LCD_ShowString(20,60,"W25X16 ERROR "); 	
		delay_ms(500);        	  								  
		LCD_ShowString(20,60,"Rechecking...");
  		delay_ms(500);   
	}
	POINT_COLOR=BLUE;      
	LCD_ShowString(20,60,"W25X16 OK    ");	    
	if(Font_Init())goto UPDF;//�ֿⲻ����,������ֿ� 
	LCD_ShowString(20,80,"Font Check Ok!");	        
	delay_ms(300);        	  								  
	SD_Init();
	while(FAT_Init())//FAT ����
	{				   
		POINT_COLOR=RED;      
		LCD_ShowString(20,100,"Fat Check ERR");  
		i= SD_Init();		
		if(i)LCD_ShowString(20,100," SD Card ERR ");//SD����ʼ��ʧ�� 	  
		delay_ms(500);		  
		LED0=!LED0;	   		    
	}
	POINT_COLOR=BLUE;      
	LCD_ShowString(20,100,"SD Card Check OK!");		   
	while(SysInfoGet(0))//����ϵͳ�ļ�  
	{					
		POINT_COLOR=RED;      
		LCD_ShowString(20,120,"System File Lost!");  
		delay_ms(300);  
		LCD_ShowString(20,120," Researching...  ");  
		FAT_Init();
		SD_Init();	 
		LED0=!LED0;
		delay_ms(300); 
	}
	POINT_COLOR=BLUE;      
	LCD_ShowString(20,120,"System File Check OK!"); 
	LCD_ShowString(20,140,"Now Loading System."); 	 	 
	delay_ms(300); 
	LCD_ShowChar(20+152,140,'.',16,0);delay_ms(300); 
	LCD_ShowChar(20+160,140,'.',16,0);delay_ms(300);  
	LCD_ShowChar(20+168,140,'.',16,0);delay_ms(300);  
///////////////////////////////////////////////////////////////
	LCD_Clear(WHITE);
	delay_ms(300);
	LCD_DisplayOff();
	Load_Sys_ICO();	 
	LCD_DisplayOn();  
	t=5;    	  
	while(1)
	{	
		key=AI_Read_TP(30);	    
		if(key)Pen_Point.Key_Reg|=0X80;  //��ǰ����Ѿ�����	   
		if(key==0&&(Pen_Point.Key_Reg&0x80))//�����ɿ���
		{
			Pen_Point.Key_Reg=0;		  	
			t=Touch_To_Num(0);
		}     						   
		if(t)
		{ 							     
			Select_Menu(0,t-1,1);//ѡ�����
			if(index!=t-1)Select_Menu(0,index,0);//ȡ����һ�ε�
			else //�˴�ѡ����ϴ�һ��,����ù���
			{
				switch(index)
				{
					case 0://�������Ķ�	    
						Ebook_Play();	    					  
						Load_Sys_ICO();//���¼���ϵͳ�˵� 
						Select_Menu(0,index,1);//ѡ���ϴ�ѡ�е�
						Pen_Point.Key_Reg=0;
						Pen_Point.Key_Sta=Key_Up;
						break; 
					case 1://���ͼƬ		 	  
						Pic_Viewer();//ͼƬ���
						Load_Sys_ICO();//���¼���ϵͳ�˵� 
						Select_Menu(0,index,1);//ѡ���ϴ�ѡ�е�
						Pen_Point.Key_Reg=0;
						Pen_Point.Key_Sta=Key_Up;
						break;
					case 2://��Ϸģʽ	  
						Game_Play();
						Load_Sys_ICO();//���¼���ϵͳ�˵�
						Select_Menu(0,index,1);//ѡ���ϴ�ѡ�е�
						Pen_Point.Key_Reg=0;
						Pen_Point.Key_Sta=Key_Up;
						break;
					case 3://������ͼģʽ 
						Touch_Play();  
						Load_Sys_ICO();//���¼���ϵͳ�˵�
						Select_Menu(0,index,1);//ѡ���ϴ�ѡ�е�	
						Pen_Point.Key_Reg=0;
						Pen_Point.Key_Sta=Key_Up;				   
						break;
					case 4://ʱ��ģʽ	 
						Time_Viewer();
						Load_Sys_ICO();//���¼���ϵͳ�˵�
						Select_Menu(0,index,1);//ѡ���ϴ�ѡ�е�
						Pen_Point.Key_Reg=0;
						Pen_Point.Key_Sta=Key_Up;
						break; 
					case 5://��껭��ģʽ
						Mouse_Play();
						Load_Sys_ICO();//���¼���ϵͳ�˵�
						Select_Menu(0,index,1);//ѡ���ϴ�ѡ�е�	
						Pen_Point.Key_Reg=0; 
						Pen_Point.Key_Sta=Key_Up;				      
						break;     
					case 6://USB����ʵ��	   	    				    
						USB_Play();
						Load_Sys_ICO();//���¼���ϵͳ�˵�
						Select_Menu(0,index,1);//ѡ���ϴ�ѡ�е�
						Pen_Point.Key_Reg=0;
						Pen_Point.Key_Sta=Key_Up;
						break; 
					case 7://�������ģʽ
						Remote_Play(); 
						Load_Sys_ICO();//���¼���ϵͳ�˵�
						Select_Menu(0,index,1);//ѡ���ϴ�ѡ�е�
						Pen_Point.Key_Reg=0;
						Pen_Point.Key_Sta=Key_Up;	  					 
						break;
					case 8://����ͨ��ģʽ	  
						Wireless_Play();
						Load_Sys_ICO();//���¼���ϵͳ�˵�
						Select_Menu(0,index,1);//ѡ���ϴ�ѡ�е�
						Pen_Point.Key_Reg=0;
						Pen_Point.Key_Sta=Key_Up;
						break;
						//goto RESTART;
				}	
			}
			index=t-1; 									   
			t=0;	  
		}else 
		{
			delay_ms(10);
			detime++;
			if(detime<10)LED0=0;//2s��100ms
			else 
			{
				LED0=1;
				if(detime>200)detime=0;
			}										   
		}				   	   
	}			   		 			  
}		 





