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
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//综合实验 驱动代码		 					    
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////	 


						 
//鼠标画板功能演示函数   
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

	Show_Str(60,110,"初始化鼠标...",16,0X00);
RST:t=Init_Mouse();//初始化鼠标,这里面会禁用JTAG/SWD
	if(t==0)Show_Str(60,130,"鼠标初始化成功!",16,0X00);
	else
	{								   
		Show_Str(60,130,"错误码:",16,0X00);
		LCD_ShowNum(116,130,t,3,16);//填充模式
		LED0=!LED0;
		x0++;
		if(x0>6)
		{
			JTAG_Set(SWD_ENABLE);//使能SWD.
			return;//初始化6次还没成功,则直接推出
		}
		goto RST;
	}
	delay_ms(200);
	Load_Gui_Show("重画","","返回");  //加载界面     
	x=MY_Cursor.X;
	y=MY_Cursor.Y;
	Set_Cursor(x,y);
	while(1)
	{
		if(PS2_Status&0x80)//得到了一次数据
		{		  			 
			Mouse_Data_Pro();
			x+=MouseX.x_pos;
			if(x<0)x=0;	 
			if(x>239)x=239;
			y+=MouseX.y_pos;
			if(y<0)y=0;
			if(y>319)y=319;				   
/////////////////////////////////////////////////////////////////////			
			Clear_Cursor(MY_Cursor.X,MY_Cursor.Y);//清除上一次的光标
			if(MouseX.bt_mask)//有按键按下
			{
				if(MY_Cursor.STA&0x02)//右键菜单已经弹出了
				{
					if(x>=(MY_Cursor.m_x0+65)||y>=(MY_Cursor.m_y0+29)||x<=MY_Cursor.m_x0||y<=MY_Cursor.m_y0)//不在菜单内
					{
						MY_Cursor.STA&=~0x02;//清除弹出标志
						Reshow_Temp_Color(MY_Cursor.m_x0,MY_Cursor.m_y0,65,29);//恢复之前的颜色
					}else //在菜单内
					{
						if(MouseX.bt_mask&LEFT_DOWN)//左键按下
						{
							temp_color=LCD_ReadPoint(x,y);//读取该点的颜色
							if(temp_color!=FRAME2)//不是按在边框上
							{
								POINT_COLOR=temp_color;//修改画笔颜色
								MY_Cursor.STA&=~0x02;//清除弹出标志
								Reshow_Temp_Color(MY_Cursor.m_x0,MY_Cursor.m_y0,65,29);//恢复之前的颜色	   
							}
							MouseX.bt_mask&=~(0x01);//此次不执行画图
						}
					}
				}
				if((MY_Cursor.STA&0x02)==0)//没有弹出窗口
				{							  
					if(y<296&&MY_Cursor.Y<296)//在有效区域内
				    {
						if(MouseX.bt_mask&LEFT_DOWN)//左键按下,并且没有弹出窗口
						{		 
							POINT_COLOR=temp_color;
							MY_Cursor.Color=temp_color;
							LCD_DrawDLine(MY_Cursor.X,MY_Cursor.Y,x,y);//画一个2类粗线(线宽2个像素)  	 
						}	  
						if(MouseX.bt_mask&MID_DOWN)//擦除,没有弹出窗口
						{
							POINT_COLOR=WHITE;
							MY_Cursor.Color=BLACK;//设置光标为黑色的
							Draw_Big_Point(x,y);//在有效区域清除一个点 
						}
					}else if(MouseX.bt_mask&LEFT_DOWN)//在功能面板上
					{
						Pen_Point.X0=x;//映射到触摸屏的触点上
						Pen_Point.Y0=y;
						if(Is_In_Area(5,300,32,16))//清除按钮按下了
						{
							LCD_Fill(0,0,239,296,WHITE);//清屏
						}else if(Is_In_Area(203,300,32,16))//返回按钮按下了
						{
							JTAG_Set(SWD_ENABLE);//使能SWD.
							return;
						}
					}		 
					if(MouseX.bt_mask&RIGHT_DOWN)//右键
					{	    
						MY_Cursor.STA|=0x02;//
						x0=x;
						y0=y;
						if(x0>(240-65-1))//靠右边了
						{
							x0-=65;//左边显示
							if(y0>(320-29-1))y0-=29;//上边显示  
						}else if(y0>(320-29-1))y0-=29;//上边显示  
						MY_Cursor.m_x0=x0;	//记录菜单所在的
						MY_Cursor.m_y0=y0;
						Read_Temp_Color(x0,y0,65,29);//先读出之前的颜色值
						Draw_Color_Table(x0,y0);    
					} 
				}		 
			}else MY_Cursor.Color=WHITE;//没有按键按下	 
			Set_Cursor(x,y);
/////////////////////////////////////////////////////////////////////
			PS2_Status=MOUSE;
			PS2_En_Data_Report();//使能数据报告 
		}else if(PS2_Status&0x40)
		{			 
			PS2_Status=MOUSE;   //读数错误处理	   
			while(Init_Mouse());//重新初始化鼠标
		}else delay_ms(1);
		t++;		  
		if(t==200)
		{
			t=0;
			LED0=!LED0;
		}
	} 		   
}


