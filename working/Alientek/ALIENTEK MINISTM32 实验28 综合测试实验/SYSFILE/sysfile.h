#ifndef _SYSFILE_H_
#define _SYSFILE_H_		 	 
#include "sys.h" 
//Mini STM32������
//ϵͳ�ļ����Ҵ���					  
//����ԭ��@ALIENTEK
//2010/6/18
	 
extern u32 PICCLUSTER;  //ͼƬ�ļ����׵�ַ	
extern u32 sys_ico[9];  //ϵͳͼ�껺����!���ܴ۸�!
extern u32 file_ico[4]; //�ļ�ͼ�껺���� folder;mus;pic;book;
extern u32 *Pic_Addr; 											    

u8 SysInfoGet(u8 sel);//��ȡϵͳ�ļ���Ϣ
#endif




