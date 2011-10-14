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
					   
//Mini STM32�����巶������28
//�ۺ� ʵ��
//����ԭ��@ALIENTEK

//����USB ����/����
//Enable:0,�Ͽ�
//       1,��������	   
void USB_Port_Set(u8 Enable)
{
	RCC->APB2ENR|=1<<2;    //ʹ��PORTAʱ��	   	 
	if(Enable)
	{			 
		_SetCNTR(_GetCNTR()&(~(1<<1)));//�˳��ϵ�ģʽ
		GPIOA->CRH&=0XFFF00FFF;
		GPIOA->CRH|=0X00044000; 		  
	}else
	{	  
		_SetCNTR(_GetCNTR()|(1<<1));  // �ϵ�ģʽ
		GPIOA->CRH&=0XFFF00FFF;
		GPIOA->CRH|=0X00033000;
		PAout(12)=0;	    		  
	}
}   	  
//bit0:��ʾ����������SD��д������
//bit1:��ʾ��������SD����������
//bit2:SD��д���ݴ����־λ
//bit3:SD�������ݴ����־λ
//bit4:1,��ʾ��������ѯ����(�������ӻ�������)
u8 Usb_Status_Reg=0; 
//����USB����   
void USB_Play(void) 
{	  	 
	u8 offline_cnt=0;
 	u8 USB_STA;
	u8 Divece_STA;
	u8 key,i;
	u8 err_back=0;

	u16 werr=0,rerr=0;
						 	    
	Load_Gui_Show("","","����");//���ؽ��� 
	LCD_Fill(0,0,240,296,LGRAY);//����ɫ 
	POINT_COLOR=BLUE;  
	BACK_COLOR=LGRAY;  
	LCD_ShowString(60,50,"Mini STM32");	
	LCD_ShowString(60,70,"SD/MMC Card Reader");	
	LCD_ShowString(60,90,"ALIENTEK");	 
		 										
    Mass_Memory_Size[0]=SD_GetCapacity();//�õ�SD������
    Mass_Block_Size[0] =512;//��Ϊ������Init����������SD���Ĳ����ֽ�Ϊ512��,��������һ����512���ֽ�.
    Mass_Block_Count[0]=Mass_Memory_Size[0]/Mass_Block_Size[0];
	LCD_ShowString(60,130,"USB Connecting...");//��ʾSD���Ѿ�׼����	 
	//USB����
	USB_Port_Set(0);//USB�ȹر�һ��
	delay_ms(500);  
	USB_Port_Set(1);//USBʹ��
	USB_Interrupts_Config();    
	Set_USBClock();   
	USB_Init();	  	 		 
	while(1)
	{
 		key=AI_Read_TP(30);//�õ������״̬
		if(key)//������������						  
		{	
			Pen_Point.Key_Reg|=0x80;//��ǰ����˰���      
		}else if(Pen_Point.Key_Reg&0X80)//�����ɿ��� 
		{
			if((Pen_Point.Key_Reg&0x02)&&Is_In_Area(203,300,32,16))//�㶯����
			{ 															  
				Pen_Point.Key_Reg=0;//�����״̬
				if(USB_STA&0x03)//USB���ڶ�д
				{
					POINT_COLOR=RED;  					    
					LCD_ShowString(60,110,"USB BUSY!!!");//��ʾSD���Ѿ�׼����
					POINT_COLOR=BLUE;  
					err_back=0x80;//������λ   
				}else
				{			   
					USB_Port_Set(0);//USB�ر�
					return;	
				} 
			}
			Pen_Point.Key_Reg=0;//�����״̬	  	 
		}else delay_ms(1);	 
		if(USB_STA!=Usb_Status_Reg)//״̬�ı��� 
		{	 						   
			LCD_ShowString(60,150,"              ");//���  
			if(Usb_Status_Reg&0x01)//����д		  
			{
				LCD_ShowString(60,150,"USB Writing...");//��ʾUSB����д������
				Usb_Status_Reg&=0xFD;//��������
			}
			if(Usb_Status_Reg&0x02)//���ڶ�
			{
				LCD_ShowString(60,150,"USB Reading...");//��ʾUSB���ڶ�������  
				Usb_Status_Reg&=0xFE;//���д���	 
			}	 										  
			if(Usb_Status_Reg&0x04)
			{
				POINT_COLOR=RED; 
				werr++; 
				LCD_ShowString(60,170,"USB Write Err:");//��ʾд�����
				LCD_ShowNum(172,170,werr,3,16);//��ʾ��������Ĵ���
				POINT_COLOR=BLUE;  
			}													        
			if(Usb_Status_Reg&0x08)
			{
				POINT_COLOR=RED;  
				rerr++;
				LCD_ShowString(60,190,"USB Read  Err:");//��ʾ��������
				LCD_ShowNum(172,190,rerr,3,16);//��ʾ��������Ĵ���
				POINT_COLOR=BLUE;  
			}														   
			USB_STA=Usb_Status_Reg;//��¼����״̬
		}
		if(Divece_STA!=bDeviceState) 
		{
			if(bDeviceState==CONFIGURED)LCD_ShowString(60,130,"USB Connected    ");//��ʾUSB�����Ѿ�����
			else LCD_ShowString(60,130,"USB DisConnected ");//��ʾUSB���γ���
			Divece_STA=bDeviceState;
		}
		i++;
		if(i==200)//200msʱ�䵽��
		{
			i=0;
			LED0=!LED0;//��ʾϵͳ������
			if(Usb_Status_Reg&0x10)
			{
				offline_cnt=0;//USB������,�����offline������
				bDeviceState=CONFIGURED;
			}else//û�еõ���ѯ 
			{
				offline_cnt++;  
				if(offline_cnt>10)bDeviceState=UNCONNECTED;//2s��û�յ����߱��,����USB���γ���
			}
			if(err_back&0x80)//�����˴������
			{
				err_back++;
				if((err_back&0x07)==0x07)
				{
					err_back=0;//��� 	 
					LCD_ShowString(60,110,"           ");//�����ʾ
				}
			}
			Usb_Status_Reg=0;
		}
			
	} 	   		  
} 



