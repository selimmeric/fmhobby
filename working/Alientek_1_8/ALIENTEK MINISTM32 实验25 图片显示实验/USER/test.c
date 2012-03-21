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
//Mini STM32�����巶������25
//ͼƬ��ʾ ʵ��
//����ԭ��@ALIENTEK
//������̳:www.openedv.com		 					
int main(void)
{	 	
	u8 i;	 
	u8 key;	  
	FileInfoStruct *FileInfo;	   		 
	u16 pic_cnt=0;//��ǰĿ¼��ͼƬ�ļ��ĸ���
	u16 index=0;  //��ǰѡ����ļ����	   
	u16 time=0;    	     	  					   

	Stm32_Clock_Init(9);//ϵͳʱ������
	delay_init(72);		//��ʱ��ʼ��
	uart_init(72,9600); //����1��ʼ��  	  
	LCD_Init();			//��ʼ��Һ��		  
	KEY_Init();			//������ʼ��
	LED_Init();         //LED��ʼ��
	SPI_Flash_Init();   //SPI FLASHʹ��
	

	if(Font_Init())//�ֿⲻ����,������ֿ�
	{	 
		POINT_COLOR=RED;      
		LCD_ShowString(60,50,"Mini STM32");	
		LCD_ShowString(60,70,"Font ERROR"); 
		while(1);  	 	  
	}  	  
	POINT_COLOR=RED;      
	Show_Str(60,50,"Mini STM32������ - FM ",16,0);				    	 
	Show_Str(60,70,"ͼƬ��ʾ ����",16,0);				    	 
	Show_Str(60,90,"����ԭ��@ALIENTEK",16,0);				    	 
	Show_Str(60,110,"2011��1��2��",16,0);  
	SD_Init();
	while(FAT_Init())//FAT ����
	{
		Show_Str(60,130,"�ļ�ϵͳ����!",16,0);  
		i= SD_Init();
		if(i)Show_Str(60,150,"SD������!",16,0);//SD����ʼ��ʧ�� 	  
		delay_ms(500);
		LCD_Fill(60,130,240,170,WHITE);//�����ʾ			  
		delay_ms(500);
		LED0=!LED0;	   
	}	   				 
	while(SysInfoGet(1))//�õ�ͼƬ�ļ���  
	{
		Show_Str(60,130,"ͼƬ�ļ���δ�ҵ�!",16,0);  
		delay_ms(500);  
		FAT_Init();
		SD_Init();
		LED0=!LED0;
		LCD_Fill(60,130,240,170,WHITE);//�����ʾ			  
		delay_ms(500); 
	}
	Show_Str(60,130,"��ʼ��ʾ...",16,0); 
	delay_ms(1000);
	Cur_Dir_Cluster=PICCLUSTER;
	while(1)
	{	    			 
		pic_cnt=0;	 
		Get_File_Info(Cur_Dir_Cluster,FileInfo,T_JPEG|T_JPG|T_BMP,&pic_cnt);//��ȡ��ǰ�ļ��������Ŀ���ļ����� 		    
		if(pic_cnt==0)//û��ͼƬ�ļ�
		{
			LCD_Clear(WHITE);//����	   
			while(1)
			{	  
				if(time%2==0)Show_Str(32,150,"û��ͼƬ,����COPYͼƬ��SD����PICTURE�ļ���,Ȼ�������!",16,0);		 
				else LCD_Clear(WHITE);
				time++;
				delay_ms(300);
			}
		}								   
		FileInfo=&F_Info[0];//�����ݴ�ռ�.
		index=1;
		while(1)
		{
			Get_File_Info(Cur_Dir_Cluster,FileInfo,T_JPEG|T_JPG|T_BMP,&index);//�õ�����ͼƬ����Ϣ	 
			LCD_Clear(WHITE);//����,������һ��ͼƬ��ʱ��,һ������	
 			AI_LoadPicFile(FileInfo,0,0,240,320);//��ʾͼƬ	       	 
 			POINT_COLOR=RED;      
			Show_Str(0,0,FileInfo->F_Name,16,1);//��ʾͼƬ����
			while(1)//��ʱ3s
			{
				key=KEY_Scan();
				if(key==1)break;//��һ��
				else if(key==2)//��һ��
				{
					if(index>1)index-=2;
					else index=pic_cnt-1;
					break;
				}
				delay_ms(1);
				time++;
				if(time%100==0)LED0=!LED0;
				if(time>3000)
				{
					time=0;
					break;
				}
			}
			index++;
			if(index>pic_cnt)index=1;//��ʾ��һ��,ѭ��	  	 		 
		}
	}			   		 			  
}		 





