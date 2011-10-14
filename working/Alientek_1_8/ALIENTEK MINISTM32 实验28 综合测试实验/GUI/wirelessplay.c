#include "wirelessplay.h"
#include "24l01.h"
#include "gui.h"			   
#include "lcd.h"			 
#include "delay.h"
#include "text.h"
#include "touch.h"		   
#include "usart.h"	 
#include "led.h"    
#include "key.h"
#include "spi.h"	     
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//�ۺ�ʵ�� ��������		 					    
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/6/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////	 


//����24L01��ģʽ,TX/RX
//����ֵ:2,����ģʽ
//       1,����ģʽ
u8 Wireless_Mode_Set(void)
{
	u8 key,t=0;	   
	LCD_Clear(LGRAY);					  
	NRF24L01_Init();    //��ʼ��NRF24L01 
	BACK_COLOR=LGRAY;//��������Ϊ��ɫ   	   
	POINT_COLOR=BLUE;//��������Ϊ��ɫ    	  			 
	while(NRF24L01_Check())//��ⲻ��24L01
	{
 		POINT_COLOR=RED;//��������Ϊ��ɫ	   
		Show_Str(60,130,"24L01���ʧ��",16,0);
		delay_ms(500);
		Show_Str(60,130,"����       ",16,0);
		delay_ms(500);
		LED0=!LED0;//DS0��˸
		t++;
		if(t>5)return 0;//ʧ����
	}				  
	LCD_Fill(60,130,240,160,LGRAY);//�����ʾ	     
	Load_Gui_Show("","","����");//���ؽ��� 
	LCD_Fill(0,0,240,296,LGRAY);//����ɫ 
	GUI_Draw_Button(40,60,160,40,"����Ϊ����ģʽ",BLACK);
 	GUI_Draw_Button(40,140,160,40,"����Ϊ����ģʽ",BLACK);	   
	while(1)//�ȴ�ģʽѡ��
	{
		key=AI_Read_TP(20);//�õ������״̬
		if(key)//������������						  
		{	
			Pen_Point.Key_Reg|=0x80;//��ǰ����˰���   
		}else if(Pen_Point.Key_Reg&0X80)//�����ɿ��� 
		{
			if(Pen_Point.Key_Reg&0x03)//���۵㶯���ǻ���,�����Դ���
			{	    
                if(Is_In_Area(203,300,32,16))//���ذ�ť������
				{
					Pen_Point.Key_Reg=0;//�����״̬			   
					return 0;//ֱ���˳�
				}
                if(Is_In_Area(40,60,160,40))//����ģʽ
				{
					Pen_Point.Key_Reg=0;//�����״̬			   
					return 2; 
				} 
                if(Is_In_Area(40,140,160,40))//����ģʽ
				{
					Pen_Point.Key_Reg=0;//�����״̬			   
					return 1;//ֱ���˳�
				}    
			}
			Pen_Point.Key_Reg=0;//�����״̬	  	 
		}else delay_ms(1);	
		t++;
		if(t==200)
		{
			LED0=!LED0;
			t=0;
		}				   
	}  	
}
						 
//�������幦����ʾ����   
void Wireless_Play(void)
{				  	
	u8 err=0,i;		 
	u8 key;	 
	u8 mode;//24L01��ģʽ����
	u8 tmp_buf[5];//��4���ֽ�,��������ֵ,���һ���ֽ�:0,������ͼ,1���;2,�˳�;  
	  
	mode=Wireless_Mode_Set();
	if(mode==0)
	{
		SPIx_Init();//���û�ԭ����				 
		SPIx_SetSpeed(SPI_SPEED_4);//18Mhz 
		return;//24L01���ʧ��,ֱ���˳�
	}	 			
	if(mode==2)
	{			    
		Load_Gui_Show("���","����","����");//���ؽ��� 
		TX_Mode();//����ģʽ
	}else
	{
		Load_Gui_Show("���","����","����");//���ؽ��� 
		RX_Mode();//����ģʽ
	}
	LCD_Fill(0,0,240,296,LGRAY);//����ɫ 
	POINT_COLOR=BLUE;//��������Ϊ��ɫ  	 
	while(1)
	{	  		   				 
		key=AI_Read_TP(20);//�õ������״̬
		if(mode==1)//����ģʽ
		{
			if(NRF24L01_RxPacket(tmp_buf)==0)//һ�����յ���Ϣ,����ʾ����.
			{
				Pen_Point.X=tmp_buf[0];
				Pen_Point.X=(Pen_Point.X<<8)+tmp_buf[1];
 				Pen_Point.Y=tmp_buf[2];
				Pen_Point.Y=(Pen_Point.Y<<8)+tmp_buf[3];
				switch(tmp_buf[4])
				{
					case 0://��������
						Draw_Big_Point(Pen_Point.X,Pen_Point.Y);//����
						break;
					case 1://���
						LCD_Fill(0,0,240,296,LGRAY);//����ɫ
						break;
					case 2://�˳�
						Pen_Point.Key_Reg=0;//�����״̬
						SPIx_Init();//���û�ԭ����				 
						SPIx_SetSpeed(SPI_SPEED_4);//18Mhz 
						return; 
				}	  
			}
		}
		if(key)//������������						  
		{	
			Pen_Point.Key_Reg|=0x80;//��ǰ����˰���    
			if(Pen_Point.Y<296&&mode==2)//�ڻ�ͼ������,��Ϊ����ģʽ
			{
				tmp_buf[0]=Pen_Point.X>>8;
				tmp_buf[1]=Pen_Point.X&0xFF;
				tmp_buf[2]=Pen_Point.Y>>8;	 
				tmp_buf[3]=Pen_Point.Y&0xFF; 
				tmp_buf[4]=0;//����Ϊ0,������ͼ  
				Draw_Big_Point(Pen_Point.X,Pen_Point.Y);//����
				err=0;	      
				while(NRF24L01_TxPacket(tmp_buf)!=TX_OK)//�ȴ��������
				{
					err++;
					if(err>10)return;//��������10�δ���,�򷵻�
				}
			}      
		}else if(Pen_Point.Key_Reg&0X80)//�����ɿ��� 
		{
			if(Pen_Point.Key_Reg&0x02)//�㶯����
			{	    
                if(Is_In_Area(203,300,32,16))//���ذ�ť������
				{
					if(mode==2)
					{
						tmp_buf[4]=2;//����Ϊ2,�˳�
						NRF24L01_TxPacket(tmp_buf);//����һ��
					} 
					Pen_Point.Key_Reg=0;//�����״̬
					SPIx_Init();//���û�ԭ����				 
					SPIx_SetSpeed(SPI_SPEED_4);//18Mhz 			   
					return;
				}
				if(Is_In_Area(5,300,32,16))//�����ť������
				{
					if(mode==2)
					{
						tmp_buf[4]=1;//����Ϊ1,����
						NRF24L01_TxPacket(tmp_buf);//����һ��
					} 									   	  
					LCD_Fill(0,0,239,296,WHITE);//����
				}   
			}
			Pen_Point.Key_Reg=0;//�����״̬	  	 
		}else delay_ms(1);
		i++;
		if(i==200)
		{
			i=0;
			LED0=!LED0;
		}					 			    
	}     
}






