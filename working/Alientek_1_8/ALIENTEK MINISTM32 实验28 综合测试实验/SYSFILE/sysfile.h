#ifndef _SYSFILE_H_
#define _SYSFILE_H_		 	 
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//ϵͳ�ļ����� ��������		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/6/18 
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////	 

extern u32 PICCLUSTER;  //ͼƬ�ļ����׵�ַ	
extern u32 sys_ico[9];  //ϵͳͼ�껺����!���ܴ۸�!
extern u32 file_ico[4]; //�ļ�ͼ�껺���� folder;mus;pic;book;
extern u32 *Pic_Addr; 											    

u8 SysInfoGet(u8 sel);//��ȡϵͳ�ļ���Ϣ
#endif




