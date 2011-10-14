#include "usbplay.h"
#include "gui.h"			   
#include "lcd.h"			 
#include "delay.h"  
#include "touch.h"	   	   	     	  
#include "usart.h"	 
#include "led.h"   
#include "mmc_sd.h"
#include "mass_mal.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "usb_regs.h"
					   
//Mini STM32开发板范例代码28
//综合 实验
//正点原子@ALIENTEK

//设置USB 连接/断线
//Enable:0,断开
//       1,允许连接	   
void USB_Port_Set(u8 Enable)
{
	RCC->APB2ENR|=1<<2;    //使能PORTA时钟	   	 
	if(Enable)
	{			 
		_SetCNTR(_GetCNTR()&(~(1<<1)));//退出断电模式
		GPIOA->CRH&=0XFFF00FFF;
		GPIOA->CRH|=0X00044000; 		  
	}else
	{	  
		_SetCNTR(_GetCNTR()|(1<<1));  // 断电模式
		GPIOA->CRH&=0XFFF00FFF;
		GPIOA->CRH|=0X00033000;
		PAout(12)=0;	    		  
	}
}   	  
//bit0:表示电脑正在向SD卡写入数据
//bit1:表示电脑正从SD卡读出数据
//bit2:SD卡写数据错误标志位
//bit3:SD卡读数据错误标志位
//bit4:1,表示电脑有轮询操作(表明连接还保持着)
u8 Usb_Status_Reg=0; 
//进入USB连接   
void USB_Play(void) 
{	  	 
	u8 offline_cnt=0;
 	u8 USB_STA;
	u8 Divece_STA;
	u8 key,i;
	u8 err_back=0;

	u16 werr=0,rerr=0;
						 	    
	Load_Gui_Show("","","返回");//加载界面 
	LCD_Fill(0,0,240,296,LGRAY);//填充灰色 
	POINT_COLOR=BLUE;  
	BACK_COLOR=LGRAY;  
	LCD_ShowString(60,50,"Mini STM32");	
	LCD_ShowString(60,70,"SD/MMC Card Reader");	
	LCD_ShowString(60,90,"ALIENTEK");	 
		 										
    Mass_Memory_Size[0]=SD_GetCapacity();//得到SD卡容量
    Mass_Block_Size[0] =512;//因为我们在Init里面设置了SD卡的操作字节为512个,所以这里一定是512个字节.
    Mass_Block_Count[0]=Mass_Memory_Size[0]/Mass_Block_Size[0];
	LCD_ShowString(60,130,"USB Connecting...");//提示SD卡已经准备了	 
	//USB配置
	USB_Port_Set(0);//USB先关闭一下
	delay_ms(500);  
	USB_Port_Set(1);//USB使能
	USB_Interrupts_Config();    
	Set_USBClock();   
	USB_Init();	  	 		 
	while(1)
	{
 		key=AI_Read_TP(30);//得到触点的状态
		if(key)//触摸屏被按下						  
		{	
			Pen_Point.Key_Reg|=0x80;//标记按下了按键      
		}else if(Pen_Point.Key_Reg&0X80)//按键松开了 
		{
			if((Pen_Point.Key_Reg&0x02)&&Is_In_Area(203,300,32,16))//点动按下
			{ 															  
				Pen_Point.Key_Reg=0;//清除笔状态
				if(USB_STA&0x03)//USB正在读写
				{
					POINT_COLOR=RED;  					    
					LCD_ShowString(60,110,"USB BUSY!!!");//提示SD卡已经准备了
					POINT_COLOR=BLUE;  
					err_back=0x80;//标记最高位   
				}else
				{			   
					USB_Port_Set(0);//USB关闭
					return;	
				} 
			}
			Pen_Point.Key_Reg=0;//清除笔状态	  	 
		}else delay_ms(1);	 
		if(USB_STA!=Usb_Status_Reg)//状态改变了 
		{	 						   
			LCD_ShowString(60,150,"              ");//清除  
			if(Usb_Status_Reg&0x01)//正在写		  
			{
				LCD_ShowString(60,150,"USB Writing...");//提示USB正在写入数据
				Usb_Status_Reg&=0xFD;//清除读标记
			}
			if(Usb_Status_Reg&0x02)//正在读
			{
				LCD_ShowString(60,150,"USB Reading...");//提示USB正在读出数据  
				Usb_Status_Reg&=0xFE;//清除写标记	 
			}	 										  
			if(Usb_Status_Reg&0x04)
			{
				POINT_COLOR=RED; 
				werr++; 
				LCD_ShowString(60,170,"USB Write Err:");//提示写入错误
				LCD_ShowNum(172,170,werr,3,16);//显示读数错误的次数
				POINT_COLOR=BLUE;  
			}													        
			if(Usb_Status_Reg&0x08)
			{
				POINT_COLOR=RED;  
				rerr++;
				LCD_ShowString(60,190,"USB Read  Err:");//提示读出错误
				LCD_ShowNum(172,190,rerr,3,16);//显示读数错误的次数
				POINT_COLOR=BLUE;  
			}														   
			USB_STA=Usb_Status_Reg;//记录最后的状态
		}
		if(Divece_STA!=bDeviceState) 
		{
			if(bDeviceState==CONFIGURED)LCD_ShowString(60,130,"USB Connected    ");//提示USB连接已经建立
			else LCD_ShowString(60,130,"USB DisConnected ");//提示USB被拔出了
			Divece_STA=bDeviceState;
		}
		i++;
		if(i==200)//200ms时间到了
		{
			i=0;
			LED0=!LED0;//提示系统在运行
			if(Usb_Status_Reg&0x10)
			{
				offline_cnt=0;//USB连接了,则清除offline计数器
				bDeviceState=CONFIGURED;
			}else//没有得到轮询 
			{
				offline_cnt++;  
				if(offline_cnt>10)bDeviceState=UNCONNECTED;//2s内没收到在线标记,代表USB被拔出了
			}
			if(err_back&0x80)//发生了错误操作
			{
				err_back++;
				if((err_back&0x07)==0x07)
				{
					err_back=0;//清除 	 
					LCD_ShowString(60,110,"           ");//清除显示
				}
			}
			Usb_Status_Reg=0;
		}
			
	} 	   		  
} 



