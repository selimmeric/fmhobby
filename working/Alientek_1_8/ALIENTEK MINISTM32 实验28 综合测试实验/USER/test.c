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
//Mini STM32开发板范例代码28
//综合 实验
//正点原子@ALIENTEK
//技术论坛:www.openedv.com		


const u8 *COMPILED_DATE=__DATE__;//获得编译日期
const u8 *COMPILED_TIME=__TIME__;//获得编译时间
int main(void)
{	 	
	u8 i,t,key;
	u8 index=0;
	u16 detime=0;  	     	  					   
	Stm32_Clock_Init(9);//系统时钟设置
	delay_init(72);		//延时初始化
	uart_init(72,9600); //串口1初始化  	  
	LCD_Init();			//初始化液晶		  
    KEY_Init();			//按键初始化
	LED_Init();         //LED初始化	
	SPI_Flash_Init();   //SPI FLASH使能
	Touch_Init();
	key=KEY_Scan();	    //检测按键
	switch(key)
	{   					    
		case 1://强制设置时间为编译时间
			POINT_COLOR=RED;   
			Auto_Time_Set();//设置时间为编译时间
			LCD_ShowString(20,20,"Set to Compiled Time OK!"); 
			delay_ms(500);
			LCD_Clear(WHITE);
			break;
		case 2://强制校准
			Touch_Adjust(); //触摸屏强制校准
			break;
		case 3://强制更新字库
			while(1)
			{
UPDF:			
				POINT_COLOR=RED;   
				SD_Init();
				while(FAT_Init())//FAT 错误	 
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
////////////////////////开始硬件检测////////////////////////////
	POINT_COLOR=BLUE;   
	LCD_ShowString(20,0,"Start Hardware Checking..."); 	
	while(RTC_Init())   //检测RTC部分是否正常
	{
		POINT_COLOR=RED;     
		LCD_ShowString(20,20,"RTC ERROR    "); 	
		delay_ms(500);        	  								  
		LCD_ShowString(20,20,"RTC Trying...");
  		delay_ms(500); 
	}		  
	POINT_COLOR=BLUE;      
	LCD_ShowString(20,20,"RTC OK       ");//RTC检测OK
	while(AT24CXX_Check())
	{
		POINT_COLOR=RED;     
		LCD_ShowString(20,40,"AT24C02 ERROR"); 	
		delay_ms(500);        	  								  
		LCD_ShowString(20,40,"Rechecking...");
  		delay_ms(500); 
	}  
	POINT_COLOR=BLUE;      
	LCD_ShowString(20,40,"AT24C02 OK   ");//AT24C02 检测OK 
	while(SPI_Flash_ReadID()!=0XEF14)//FLASH检测
	{
		POINT_COLOR=RED;     
		LCD_ShowString(20,60,"W25X16 ERROR "); 	
		delay_ms(500);        	  								  
		LCD_ShowString(20,60,"Rechecking...");
  		delay_ms(500);   
	}
	POINT_COLOR=BLUE;      
	LCD_ShowString(20,60,"W25X16 OK    ");	    
	if(Font_Init())goto UPDF;//字库不存在,则更新字库 
	LCD_ShowString(20,80,"Font Check Ok!");	        
	delay_ms(300);        	  								  
	SD_Init();
	while(FAT_Init())//FAT 错误
	{				   
		POINT_COLOR=RED;      
		LCD_ShowString(20,100,"Fat Check ERR");  
		i= SD_Init();		
		if(i)LCD_ShowString(20,100," SD Card ERR ");//SD卡初始化失败 	  
		delay_ms(500);		  
		LED0=!LED0;	   		    
	}
	POINT_COLOR=BLUE;      
	LCD_ShowString(20,100,"SD Card Check OK!");		   
	while(SysInfoGet(0))//查找系统文件  
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
		if(key)Pen_Point.Key_Reg|=0X80;  //标记按键已经按下	   
		if(key==0&&(Pen_Point.Key_Reg&0x80))//按键松开了
		{
			Pen_Point.Key_Reg=0;		  	
			t=Touch_To_Num(0);
		}     						   
		if(t)
		{ 							     
			Select_Menu(0,t-1,1);//选中这个
			if(index!=t-1)Select_Menu(0,index,0);//取消上一次的
			else //此次选择和上次一样,进入该功能
			{
				switch(index)
				{
					case 0://电子书阅读	    
						Ebook_Play();	    					  
						Load_Sys_ICO();//重新加载系统菜单 
						Select_Menu(0,index,1);//选中上次选中的
						Pen_Point.Key_Reg=0;
						Pen_Point.Key_Sta=Key_Up;
						break; 
					case 1://浏览图片		 	  
						Pic_Viewer();//图片浏览
						Load_Sys_ICO();//重新加载系统菜单 
						Select_Menu(0,index,1);//选中上次选中的
						Pen_Point.Key_Reg=0;
						Pen_Point.Key_Sta=Key_Up;
						break;
					case 2://游戏模式	  
						Game_Play();
						Load_Sys_ICO();//重新加载系统菜单
						Select_Menu(0,index,1);//选中上次选中的
						Pen_Point.Key_Reg=0;
						Pen_Point.Key_Sta=Key_Up;
						break;
					case 3://触摸画图模式 
						Touch_Play();  
						Load_Sys_ICO();//重新加载系统菜单
						Select_Menu(0,index,1);//选中上次选中的	
						Pen_Point.Key_Reg=0;
						Pen_Point.Key_Sta=Key_Up;				   
						break;
					case 4://时间模式	 
						Time_Viewer();
						Load_Sys_ICO();//重新加载系统菜单
						Select_Menu(0,index,1);//选中上次选中的
						Pen_Point.Key_Reg=0;
						Pen_Point.Key_Sta=Key_Up;
						break; 
					case 5://鼠标画板模式
						Mouse_Play();
						Load_Sys_ICO();//重新加载系统菜单
						Select_Menu(0,index,1);//选中上次选中的	
						Pen_Point.Key_Reg=0; 
						Pen_Point.Key_Sta=Key_Up;				      
						break;     
					case 6://USB连接实验	   	    				    
						USB_Play();
						Load_Sys_ICO();//重新加载系统菜单
						Select_Menu(0,index,1);//选中上次选中的
						Pen_Point.Key_Reg=0;
						Pen_Point.Key_Sta=Key_Up;
						break; 
					case 7://红外测试模式
						Remote_Play(); 
						Load_Sys_ICO();//重新加载系统菜单
						Select_Menu(0,index,1);//选中上次选中的
						Pen_Point.Key_Reg=0;
						Pen_Point.Key_Sta=Key_Up;	  					 
						break;
					case 8://无线通信模式	  
						Wireless_Play();
						Load_Sys_ICO();//重新加载系统菜单
						Select_Menu(0,index,1);//选中上次选中的
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
			if(detime<10)LED0=0;//2s亮100ms
			else 
			{
				LED0=1;
				if(detime>200)detime=0;
			}										   
		}				   	   
	}			   		 			  
}		 





