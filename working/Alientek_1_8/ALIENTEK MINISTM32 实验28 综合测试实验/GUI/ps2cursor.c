#include "ps2cursor.h"
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


u16 tcnt=0;
void Mouse_Play(void)
{				  	
	u8 t;
	short x,y;
	u16 x0,y0;
	u16 temp_color=BLUE;		 

	x=y=0;
	LCD_Clear(WHITE);
	POINT_COLOR=BLUE;
	BACK_COLOR=WHITE;

RST:t=Init_Mouse();//��ʼ�����
	if(t==0)LCD_ShowString(60,130,"Mouse Init OK!");
	else
	{
		printf("ERROR_CODE:%d\n",t);
		LCD_ShowString(60,130,"Error CODE:");
		LCD_ShowNum(148,130,t,3,16);//���ģʽ
		LED0=!LED0;
		goto RST;
	}
	GUI_Cursor_Init();//��ʼ�����
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
				if((MouseX.bt_mask&LEFT_DOWN)&&((MY_Cursor.STA&0x02)==0))//�������,����û�е�������
				{
					POINT_COLOR=temp_color;
					MY_Cursor.Color=temp_color;
					Draw_Big_Point(x,y);  
				}	  
				if((MouseX.bt_mask&MID_DOWN)&&((MY_Cursor.STA&0x02)==0))//����,û�е�������
				{
					POINT_COLOR=WHITE;
					MY_Cursor.Color=RED;
					Draw_Big_Point(x,y); 
				}	 
				if(MouseX.bt_mask&RIGHT_DOWN)//�Ҽ�
				{
			     	if((MY_Cursor.STA&0x02)==0)//�Ҽ��˵���δ����
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
			printf("ERROR\n");
			while(Init_Mouse());//���³�ʼ�����
		}
		t++;		  
		if(t==200)
		{
			t=0;
			LED0=!LED0;
		}
	} 		   
}


