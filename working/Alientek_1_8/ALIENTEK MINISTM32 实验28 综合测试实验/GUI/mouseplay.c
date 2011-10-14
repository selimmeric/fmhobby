#include "mouseplay.h"
#include "gui.h"			   
#include "lcd.h"			 
#include "delay.h"
#include "text.h"
#include "touch.h"
#include "fat.h"
#include "sysfile.h"
#include "jpegbmp.h"
#include "usart.h"	 
#include "led.h"
#include "mouse.h"
#include "cursor.h"	   
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


						 
//��껭�幦����ʾ����   
void Mouse_Play(void)
{				  	
	u8 t;
	short x,y;
	u16 x0,y0;
	u16 temp_color=BLUE;
	LCD_Clear(WHITE);

	POINT_COLOR=BLUE;
	BACK_COLOR=WHITE;
	x0=0;

	Show_Str(60,110,"��ʼ�����...",16,0X00);
RST:t=Init_Mouse();//��ʼ�����,����������JTAG/SWD
	if(t==0)Show_Str(60,130,"����ʼ���ɹ�!",16,0X00);
	else
	{								   
		Show_Str(60,130,"������:",16,0X00);
		LCD_ShowNum(116,130,t,3,16);//���ģʽ
		LED0=!LED0;
		x0++;
		if(x0>6)
		{
			JTAG_Set(SWD_ENABLE);//ʹ��SWD.
			return;//��ʼ��6�λ�û�ɹ�,��ֱ���Ƴ�
		}
		goto RST;
	}
	delay_ms(200);
	Load_Gui_Show("�ػ�","","����");  //���ؽ���     
	x=MY_Cursor.X;
	y=MY_Cursor.Y;
	Set_Cursor(x,y);
	while(1)
	{
		if(PS2_Status&0x80)//�õ���һ������
		{		  			 
			Mouse_Data_Pro();
			x+=MouseX.x_pos;
			if(x<0)x=0;	 
			if(x>239)x=239;
			y+=MouseX.y_pos;
			if(y<0)y=0;
			if(y>319)y=319;				   
/////////////////////////////////////////////////////////////////////			
			Clear_Cursor(MY_Cursor.X,MY_Cursor.Y);//�����һ�εĹ��
			if(MouseX.bt_mask)//�а�������
			{
				if(MY_Cursor.STA&0x02)//�Ҽ��˵��Ѿ�������
				{
					if(x>=(MY_Cursor.m_x0+65)||y>=(MY_Cursor.m_y0+29)||x<=MY_Cursor.m_x0||y<=MY_Cursor.m_y0)//���ڲ˵���
					{
						MY_Cursor.STA&=~0x02;//���������־
						Reshow_Temp_Color(MY_Cursor.m_x0,MY_Cursor.m_y0,65,29);//�ָ�֮ǰ����ɫ
					}else //�ڲ˵���
					{
						if(MouseX.bt_mask&LEFT_DOWN)//�������
						{
							temp_color=LCD_ReadPoint(x,y);//��ȡ�õ����ɫ
							if(temp_color!=FRAME2)//���ǰ��ڱ߿���
							{
								POINT_COLOR=temp_color;//�޸Ļ�����ɫ
								MY_Cursor.STA&=~0x02;//���������־
								Reshow_Temp_Color(MY_Cursor.m_x0,MY_Cursor.m_y0,65,29);//�ָ�֮ǰ����ɫ	   
							}
							MouseX.bt_mask&=~(0x01);//�˴β�ִ�л�ͼ
						}
					}
				}
				if((MY_Cursor.STA&0x02)==0)//û�е�������
				{							  
					if(y<296&&MY_Cursor.Y<296)//����Ч������
				    {
						if(MouseX.bt_mask&LEFT_DOWN)//�������,����û�е�������
						{		 
							POINT_COLOR=temp_color;
							MY_Cursor.Color=temp_color;
							LCD_DrawDLine(MY_Cursor.X,MY_Cursor.Y,x,y);//��һ��2�����(�߿�2������)  	 
						}	  
						if(MouseX.bt_mask&MID_DOWN)//����,û�е�������
						{
							POINT_COLOR=WHITE;
							MY_Cursor.Color=BLACK;//���ù��Ϊ��ɫ��
							Draw_Big_Point(x,y);//����Ч�������һ���� 
						}
					}else if(MouseX.bt_mask&LEFT_DOWN)//�ڹ��������
					{
						Pen_Point.X0=x;//ӳ�䵽�������Ĵ�����
						Pen_Point.Y0=y;
						if(Is_In_Area(5,300,32,16))//�����ť������
						{
							LCD_Fill(0,0,239,296,WHITE);//����
						}else if(Is_In_Area(203,300,32,16))//���ذ�ť������
						{
							JTAG_Set(SWD_ENABLE);//ʹ��SWD.
							return;
						}
					}		 
					if(MouseX.bt_mask&RIGHT_DOWN)//�Ҽ�
					{	    
						MY_Cursor.STA|=0x02;//
						x0=x;
						y0=y;
						if(x0>(240-65-1))//���ұ���
						{
							x0-=65;//�����ʾ
							if(y0>(320-29-1))y0-=29;//�ϱ���ʾ  
						}else if(y0>(320-29-1))y0-=29;//�ϱ���ʾ  
						MY_Cursor.m_x0=x0;	//��¼�˵����ڵ�
						MY_Cursor.m_y0=y0;
						Read_Temp_Color(x0,y0,65,29);//�ȶ���֮ǰ����ɫֵ
						Draw_Color_Table(x0,y0);    
					} 
				}		 
			}else MY_Cursor.Color=WHITE;//û�а�������	 
			Set_Cursor(x,y);
/////////////////////////////////////////////////////////////////////
			PS2_Status=MOUSE;
			PS2_En_Data_Report();//ʹ�����ݱ��� 
		}else if(PS2_Status&0x40)
		{			 
			PS2_Status=MOUSE;   //����������	   
			while(Init_Mouse());//���³�ʼ�����
		}else delay_ms(1);
		t++;		  
		if(t==200)
		{
			t=0;
			LED0=!LED0;
		}
	} 		   
}


