#ifndef _SYSFILE_H_
#define _SYSFILE_H_		 	 
#include "sys.h" 
//Mini STM32开发板
//系统文件查找代码					  
//正点原子@ALIENTEK
//2010/6/18
	 
extern u32 PICCLUSTER;  //图片文件夹首地址	
extern u32 sys_ico[9];  //系统图标缓存区!不能篡改!
extern u32 file_ico[4]; //文件图标缓存区 folder;mus;pic;book;
extern u32 *Pic_Addr; 											    

u8 SysInfoGet(u8 sel);//获取系统文件信息
#endif




