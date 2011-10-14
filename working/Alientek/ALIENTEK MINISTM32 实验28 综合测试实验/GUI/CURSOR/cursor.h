#ifndef __CURSOR_H
#define __CURSOR_H
#include "sys.h"
//���
typedef struct 
{
	u16 X;	  //��굱ǰ���ڵ�����
	u16 Y;		
	u16 Color;//������ɫ
//�Ҽ��˵������Ĵ���
	u16 m_x0;
	u16 m_y0;	  
	u8 STA;	     //���״̬,bit0:0,û����ʾ,1,��ʼ��ʾ��;bit1:�Ҽ��˵��������
	void(*Set_Cur)(u16,u16); //�������ù�굽ĳ������,�������		 
	void(*Clr_Cur)(u16,u16); //����������һ�εĹ��		 
	void(*Cursor)(u16,u16);	 //��������һ�εĹ��,�������µĹ��	 
}GUI_Cursor;  

extern GUI_Cursor MY_Cursor;				 
void Set_Cursor(u16 x,u16 y);  //���ù��
void Clear_Cursor(u16 x,u16 y);//������
void Cursor_Draw(u16 x,u16 y);//�����,�����	 
#endif











