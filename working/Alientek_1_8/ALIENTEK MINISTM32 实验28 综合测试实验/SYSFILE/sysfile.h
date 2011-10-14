#ifndef _SYSFILE_H_
#define _SYSFILE_H_		 	 
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//系统文件查找 驱动代码		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/18 
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////	 

extern u32 PICCLUSTER;  //图片文件夹首地址	
extern u32 sys_ico[9];  //系统图标缓存区!不能篡改!
extern u32 file_ico[4]; //文件图标缓存区 folder;mus;pic;book;
extern u32 *Pic_Addr; 											    

u8 SysInfoGet(u8 sel);//获取系统文件信息
#endif




