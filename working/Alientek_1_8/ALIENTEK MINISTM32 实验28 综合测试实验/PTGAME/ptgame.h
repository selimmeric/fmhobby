#ifndef __PTGAME_H
#define __PTGAME_H	
#include "sys.h"    
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//拼图游戏 驱动代码		   
//拼图游戏代码
//支持 3*3 4*4 5*5三个等级，要继续添加也是比较容易的
//基本只要修改图片编号Cur_Table的大小，PT_Size的大小就可以了
//不过要注意不要超出数据类型的范围						    
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2009/05/30
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////	 

void Game_Play(void);
void PTGame_Play(void);
void PTG_Best_Show(void);
void PTG_Win_Msg(void);
u8 PTGame_Key_To_Num(void);   
#endif







