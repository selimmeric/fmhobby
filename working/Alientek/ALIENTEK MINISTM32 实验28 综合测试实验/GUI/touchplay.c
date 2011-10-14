#include "touchplay.h"
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
//Mini STM32�����巶������28
//�ۺ� ʵ��
//����ԭ��@ALIENTEK
						 
//�������幦����ʾ����   
void Touch_Play(void)
{				  	
	u8 i;		 
	u8 key;
	u8 M_STA=0;
	u16 temp_color;
	LCD_Clear(WHITE);	    
	Load_Gui_Show("�ػ�","��ɫ","����");//���ؽ���  
	POINT_COLOR=RED;  
	while(1)
	{
 		key=AI_Read_TP(20);//�õ������״̬
		if(key)//������������						  
		{	
			Pen_Point.Key_Reg|=0x80;//��ǰ����˰���

			if(Pen_Point.Y<296&&M_STA==0)//�ڻ�ͼ������,������ɫ��û�е�����
			{
				Draw_Big_Point(Pen_Point.X,Pen_Point.Y);//����	      
			}      
		}else if(Pen_Point.Key_Reg&0X80)//�����ɿ��� 
		{
			if(Pen_Point.Key_Reg&0x02)//�㶯����
			{
				if(M_STA)//�˵��Ѿ���������
				{
					if(Pen_Point.X>88&&Pen_Point.X<153&&Pen_Point.Y>268)//����ɫ����
					{
						temp_color=LCD_ReadPoint(Pen_Point.X,Pen_Point.Y);//��ȡ�õ����ɫ
						if(temp_color!=FRAME2)//���ǰ��ڱ߿���
						{
							Reshow_Temp_Color(88,268,65,29);//�ָ�֮ǰ����ɫ
							POINT_COLOR=temp_color;//�޸Ļ�����ɫ	    
							M_STA=0;	   
						}   
					}else 
					{
						Reshow_Temp_Color(88,268,65,29);//�ָ�֮ǰ����ɫ
						M_STA=0;
					}
				}
				if(Is_In_Area(5,300,32,16))//�����ť������
				{
					LCD_Fill(0,0,239,296,WHITE);//����
				}else if(Is_In_Area(104,300,32,16))//��ɫ��ť������
				{
					Read_Temp_Color(88,268,65,29);//�ȶ���֮ǰ����ɫֵ
					Draw_Color_Table(88,268); 
					M_STA=1;//�����ɫ���Ѿ���ʾ������ 
				}else if(Is_In_Area(203,300,32,16))//���ذ�ť������
				{
					Pen_Point.Key_Reg=0;//�����״̬			   
					return;
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


