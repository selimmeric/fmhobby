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
//Mini STM32开发板范例代码25
//图片显示 实验
//正点原子@ALIENTEK
//技术论坛:www.openedv.com		 					
int main(void)
{	 	
	u8 i;	 
	u8 key;	  
	FileInfoStruct *FileInfo;	   		 
	u16 pic_cnt=0;//当前目录下图片文件的个数
	u16 index=0;  //当前选择的文件编号	   
	u16 time=0;    	     	  					   

	Stm32_Clock_Init(9);//系统时钟设置
	delay_init(72);		//延时初始化
	uart_init(72,9600); //串口1初始化  	  
	LCD_Init();			//初始化液晶		  
	KEY_Init();			//按键初始化
	LED_Init();         //LED初始化
	SPI_Flash_Init();   //SPI FLASH使能
	

	if(Font_Init())//字库不存在,则更新字库
	{	 
		POINT_COLOR=RED;      
		LCD_ShowString(60,50,"Mini STM32");	
		LCD_ShowString(60,70,"Font ERROR"); 
		while(1);  	 	  
	}  	  
	POINT_COLOR=RED;      
	Show_Str(60,50,"Mini STM32开发板 - FM ",16,0);				    	 
	Show_Str(60,70,"图片显示 程序",16,0);				    	 
	Show_Str(60,90,"正点原子@ALIENTEK",16,0);				    	 
	Show_Str(60,110,"2011年1月2日",16,0);  
	SD_Init();
	while(FAT_Init())//FAT 错误
	{
		Show_Str(60,130,"文件系统错误!",16,0);  
		i= SD_Init();
		if(i)Show_Str(60,150,"SD卡错误!",16,0);//SD卡初始化失败 	  
		delay_ms(500);
		LCD_Fill(60,130,240,170,WHITE);//清除显示			  
		delay_ms(500);
		LED0=!LED0;	   
	}	   				 
	while(SysInfoGet(1))//得到图片文件夹  
	{
		Show_Str(60,130,"图片文件夹未找到!",16,0);  
		delay_ms(500);  
		FAT_Init();
		SD_Init();
		LED0=!LED0;
		LCD_Fill(60,130,240,170,WHITE);//清除显示			  
		delay_ms(500); 
	}
	Show_Str(60,130,"开始显示...",16,0); 
	delay_ms(1000);
	Cur_Dir_Cluster=PICCLUSTER;
	while(1)
	{	    			 
		pic_cnt=0;	 
		Get_File_Info(Cur_Dir_Cluster,FileInfo,T_JPEG|T_JPG|T_BMP,&pic_cnt);//获取当前文件夹下面的目标文件个数 		    
		if(pic_cnt==0)//没有图片文件
		{
			LCD_Clear(WHITE);//清屏	   
			while(1)
			{	  
				if(time%2==0)Show_Str(32,150,"没有图片,请先COPY图片到SD卡的PICTURE文件夹,然后后重启!",16,0);		 
				else LCD_Clear(WHITE);
				time++;
				delay_ms(300);
			}
		}								   
		FileInfo=&F_Info[0];//开辟暂存空间.
		index=1;
		while(1)
		{
			Get_File_Info(Cur_Dir_Cluster,FileInfo,T_JPEG|T_JPG|T_BMP,&index);//得到这张图片的信息	 
			LCD_Clear(WHITE);//清屏,加载下一幅图片的时候,一定清屏	
 			AI_LoadPicFile(FileInfo,0,0,240,320);//显示图片	       	 
 			POINT_COLOR=RED;      
			Show_Str(0,0,FileInfo->F_Name,16,1);//显示图片名字
			while(1)//延时3s
			{
				key=KEY_Scan();
				if(key==1)break;//下一张
				else if(key==2)//上一张
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
			if(index>pic_cnt)index=1;//显示第一副,循环	  	 		 
		}
	}			   		 			  
}		 





