#ifndef __TIMEPLAY_H
#define __TIMEPLAY_H
#include "sys.h"	  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//�ۺ�ʵ�� ��������		 					    
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2011/1/26
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************
//V1.1  
//�޸�LCD_ShowBigCharΪ���㷽ʽʵ��
//////////////////////////////////////////////////////////////////////////////////	 
  
void LCD_ShowBigChar(u8 x,u16 y,u8 num);
void LCD_ShowBigNum(u8 x,u16 y,u8 num);	
void Date_Refresh(void);
void Time_Viewer(void);

#endif



