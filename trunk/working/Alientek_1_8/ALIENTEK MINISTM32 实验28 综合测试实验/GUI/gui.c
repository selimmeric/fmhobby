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


//系统主菜单
const u8 *Menu[9]=
{
"电子图书",
"数码相框",
"拼图游戏",
"触摸画板",
"系统时间",
"鼠标画板",
"USB连接 ",
"红外遥控",
"无线传书",	 
};
//////////////////////////GUI 按钮///////////////////////////////
void GUI_DrawLine(u16 x,u16 y,u16 xlen,u16 ylen,u16 color)
{
	u16 tempcolor=POINT_COLOR;
	POINT_COLOR=color;
	LCD_DrawLine(x,y,x+xlen,y+ylen);
	POINT_COLOR=tempcolor;	
}
//在指定位置显示指定颜色的按钮
//(x,y):按钮起始坐标
//xlen,ylen:x,y方向的长度
//str:按钮上要显示的字符
//color:字符颜色
void GUI_Draw_Button(u16 x,u16 y,u16 xlen,u16 ylen,u8 *str,u16 color)
{	
	u16 tback_color,tpoint_color;
	//////////////////画按钮部分//////////////////////   
	LCD_Fill(x,y,x+xlen,y+ylen,LGRAY);//画实体
	GUI_DrawLine(x,y,xlen,0,WHITE);   //上外边框
	GUI_DrawLine(x,y,0,ylen,WHITE);   //左外边框   	   
	GUI_DrawLine(x+1,y+1,xlen-1,0,0XB5B6);//上内边框
	GUI_DrawLine(x+1,y+1,0,ylen-1,0XB5B6);//左内边框
	GUI_DrawLine(x,y+ylen,xlen,0,0X8431); //下外边框
	GUI_DrawLine(x+xlen,y,0,ylen,0X8431); //右外边框   	   
	GUI_DrawLine(x+1,y+ylen-1,xlen-2,0,0XA535);//下内边框
	GUI_DrawLine(x+xlen-1,y,0,ylen-1,0XA535);  //右内边框
	//////////////////显示按钮上面的信息///////////////  
	if(ylen<16)return;//不够画	 
	tback_color=BACK_COLOR;
	tpoint_color=POINT_COLOR;
	BACK_COLOR=LGRAY;
	POINT_COLOR=color; 
	Show_Str_Mid(x,y+(ylen-16)/2,str,16,xlen);//只对16字体
	BACK_COLOR=tback_color;	 //恢复之前的颜色
	POINT_COLOR=tpoint_color; 
}
/////////////////////////界面图形加载/////////////////////////////

//选择菜单
void Select_Menu(u8 M_Type,u8 Cnt,u8 En)
{
	u8 tx;
	u16 ty;
	u8 t;	
	ty=10; 
	if(M_Type==0)//主菜单
	{	  
		tx=80*(Cnt%3)+10;//10,90,170			
		
		if(Cnt>5)ty=216;
		else if(Cnt>2)ty=113;	
		//显示外框部分 
		if(En)POINT_COLOR=BLUE;
		else POINT_COLOR=WHITE;
		for(t=0;t<4;t++)
		{
			LCD_DrawRectangle(tx-7+t,ty-7+t,tx+66-t,ty+67-t); 
		} 
		//显示字体，反白
		if(En)//选中当前的菜单
		{	  
			POINT_COLOR=WHITE;//白色字体
			BACK_COLOR=LBBLUE;  //蓝色背景      
		}else
		{
			POINT_COLOR=DARKBLUE;//深蓝色字体
			BACK_COLOR=WHITE; //白色背景   
		}	 												  
		Show_Str(tx-3,ty+74,(u8*)Menu[Cnt],16,0);	
	}
}
//加载主界面
void Load_Sys_ICO(void)
{		  
	u8 t,tx;
	u16 ty;	   
	FileInfoStruct temp; 
	POINT_COLOR=RED;//黑色字体				   
    LCD_Clear(WHITE);//清屏 
	temp.F_Type=T_BMP;//文件属性设置 
	tx=10;ty=10;
	for(t=0;t<9;t++)
	{	   
		temp.F_StartCluster=sys_ico[t];								    
		tx=80*(t%3)+10;//10,90,170	    
		AI_LoadPicFile(&temp,tx,ty,tx+59,ty+60);
		Select_Menu(0,t,0);//加载字体    		  
		if(t>=5)ty=216;
		else if(t>=2)ty=113;  
	}
}
//加载游戏界面
//size:3,3*3图片
//     4,4*4图片
//     5,5*5图片
//#define FRAME1 0XBC40 //棕色
void Load_Game_ICO(u8 size)
{
	u8 i;
	LCD_Clear(LGRAY);//清屏
	for(i=0;i<4;i++)//画大边框
	{		
		POINT_COLOR=FRAME1;
		LCD_DrawRectangle(i,i,239-i,239-i);
	}
	if(size==3)
	{
		for(i=0;i<4;i++)//画小边框
		{		
			POINT_COLOR=FRAME1;
			LCD_DrawRectangle(154+i,235+i,239-i,316-i);
		}									 
	    LCD_Fill(158,235,235,239,WHITE);//把多余的去掉	
		POINT_COLOR=RED;
		LCD_DrawRectangle(158,235,235,312);//画空的方框，事先不放图片
		LCD_DrawRectangle(4,4,235,235); //画最大的红框

		LCD_DrawLine(81,4,81,235);  //画内部红框
		LCD_DrawLine(158,4,158,235);//画内部红框
		LCD_DrawLine(4,81,235,81);  //画内部红框
		LCD_DrawLine(4,158,235,158);//画内部红框
	}else if(size==4)
	{
		for(i=0;i<4;i++)//画小边框
		{		
			POINT_COLOR=FRAME1;
			LCD_DrawRectangle(174+i,236+i,239-i,298-i);
		}									 
	    LCD_Fill(178,236,236,240,WHITE);//把多余的去掉	
		POINT_COLOR=RED;
		LCD_DrawRectangle(178,236,236,294);//画空的方框，第十六格
		LCD_DrawRectangle(4,4,236,236);    //画最大的红框

		for(i=0;i<size;i++)LCD_DrawLine(4,4+58*i,236,4+58*i);  //画内部横网格线
		for(i=0;i<size;i++)LCD_DrawLine(4+58*i,4,4+58*i,236);  //画内部竖网格线	   										  
	}else if(size==5)
	{
		for(i=0;i<4;i++)//画小边框
		{		
			POINT_COLOR=FRAME1;
			LCD_DrawRectangle(184+i,234+i,239-i,284-i);
		}	
		LCD_DrawLine(4,235,236,235);  //误差横线填充线
		LCD_DrawLine(235,4,235,281);  //误差竖线填充线
										 
	    LCD_Fill(188,234,234,239,WHITE);//把多余的去掉	
		POINT_COLOR=RED;
		LCD_DrawRectangle(188,234,234,280);//画空的方框，第十六格
		LCD_DrawRectangle(4,4,234,234);    //画最大的红框

		for(i=0;i<size;i++)LCD_DrawLine(4,4+46*i,234,4+46*i);  //画内部横网格线
		for(i=0;i<size;i++)LCD_DrawLine(4+46*i,4,4+46*i,234);  //画内部竖网格线
	}	  		  								    	  		  
	LCD_DrawRectangle(2,241,79,318);//包括预览图片
	
	BACK_COLOR=WHITE;
	POINT_COLOR=BLUE;//画笔颜色
	Show_Str(81,242,"等级:",16,0x01);
	POINT_COLOR=RED;//画笔颜色
	if(size==3)Show_Str(121,242,"简单",16,0x01);
	else if(size==4)Show_Str(121,242,"一般",16,0x01);
	else Show_Str(121,242,"困难",16,0x01);
	POINT_COLOR=BLUE;//画笔颜色
	Show_Str(81,258,"步数:",16,0x01);
	Show_Str(81,274,"用时:",16,0x01);   
	GUI_Draw_Button(98,298,37,20,"退出",BLACK);		  					     
} 		 	 
//加载浏览界面
//设置界面 
//str:"选择/确定"
void Load_Gui_Bro(u8 * str)
{			 	   					  			   
    LCD_Clear(WHITE);//清屏
	//顶部 						   
 	LCD_Fill(0,0,239,9,LIGHTBLUE);  //填充底部颜色(浅蓝色)	
 	LCD_Fill(0,10,239,20,GRAYBLUE); //填充底部颜色(灰蓝色)
	//中部
	POINT_COLOR=DARKBLUE;//深蓝色  
	LCD_DrawLine(0,21,239,21);
	LCD_Fill(0,22,239,41,LIGHTBLUE);//填充中部部颜色(浅灰蓝色)
	LCD_DrawLine(0,41,239,41);	    
	//底部
	LCD_DrawLine(0,298,239,298);
 	LCD_Fill(0,299,239,308,LIGHTBLUE);//填充底部颜色(浅蓝色)	
 	LCD_Fill(0,308,239,319,GRAYBLUE);//填充底部颜色(灰蓝色)

	POINT_COLOR=DARKBLUE;
	Show_Str(0,23,"请选择:",16,0x01); //叠加模式,非自动换行
	POINT_COLOR=WHITE;
	Show_Str(5,300,str,16,0x01);   //叠加模式,非自动换行
	Show_Str(203,300,"返回",16,0x01); //叠加模式,非自动换行  
}
//加载显示界面
//在PS2/触控画图时用到
//设置界面 
//str:"选择/确定"
void Load_Gui_Show(u8 *lbtstr,u8 *mbtstr,u8 *rbtstr)
{			 	   					  			   
    LCD_Clear(WHITE);  //清屏  
 	POINT_COLOR=DARKBLUE;//深蓝色	 
	LCD_DrawLine(0,297,239,297);
 	LCD_Fill(0,298,239,308,LIGHTBLUE);//填充底部颜色(浅蓝色)	
 	LCD_Fill(0,308,239,319,GRAYBLUE); //填充底部颜色(灰蓝色)	  	   
	POINT_COLOR=WHITE;//白色字体
	Show_Str(5,300,lbtstr,16,0x01);    //叠加模式,非自动换行
	Show_Str(104,300,mbtstr,16,0x01);  //叠加模式,非自动换行		  
	Show_Str(203,300,rbtstr,16,0x01);  //叠加模式,非自动换行  
}

////////////////////////////////与触摸屏控制相关的函数////////////////////////////
//把触摸屏得到的数据转换为数字(按钮值)
//M_Type:按钮模式;
//0,系统菜单	
//1,文件浏览模式 	
//2,时间模式下,按键的获取,连按支持! 
//3,音乐播放模式下
//4,功能选择模式下
//5,收音机模式下
//6,闹钟中断程序按键

//7,TXT浏览/JPEG浏览 时按键处理			  
//返回值:对应的按键值(1~n)
//返回0,则表示此次按键无效  
u8 Touch_To_Num(u8 M_Type)
{
	u8 xtemp;
	u16 t,ytemp;
	switch(M_Type)
	{
		case 0://系统菜单下										  
			ytemp=10;
			for(t=0;t<9;t++)
			{
				xtemp=80*(t%3)+10;
				if(t>5)ytemp=216;
				else if(t>2)ytemp=113;
				if(Is_In_Area(xtemp,ytemp,59,90))return t+1;//返回按键值 
			}			  
		case 1://1,文件浏览模式	   	  
			for(t=0;t<8;t++)
			{	  
				if(Is_In_Area(0,42+t*32,240,32))return t+1;
			}
			if(Is_In_Area(0,300,40,20))return 9;//选择按钮被按下
			if(Is_In_Area(200,300,40,20))return 10;//返回按钮被按下
			return 0;//按键无效	    
//			if(xtemp)return xtemp&0xf0;//滑动触摸了!! 
			//break; 
		case 2://4,功能选择模式下 
			for(t=0;t<8;t++)
			{	  
				if(Is_In_Area(0,42+t*32,240,32))return t+1;
			}
			if(Is_In_Area(0,300,40,20))return 9;//选择按钮被按下
			if(Is_In_Area(200,300,40,20))return 10;//返回按钮被按下
			return 0;//按键无效	 
			//break;
		case 3:
			break;
		case 4:						    
			break;
		case 5://5,收音机模式下 
			break; 
		case 6://6,闹钟中断程序按键	  
			break;
		case 7://JPEG/BMP/TXT浏览按键模式  
			if(Is_In_Area(0,0,239,60))return KEY_PREV;        //向上
			else if(Is_In_Area(0,60,239,200))return KEY_FUNC; //不定		    
			else if(Is_In_Area(0,260,239,60))return KEY_NEXT; //向下翻 	   
			return 0;//按键无效	    			
			//break; 	  	    		 
	}
	return 0;
}
//按键滑动检测
u8 KEY_Move_Check(void)
{
#define MOVE_PIXES 30 //滑动距离						    
	if(Pen_Point.X>Pen_Point.X0)//右移
	{
		if(T_abs(Pen_Point.X0,Pen_Point.X)>MOVE_PIXES&&T_abs(Pen_Point.Y0,Pen_Point.Y)<MOVE_PIXES)return MOVE_RIGHT;
	}else  //左移
	{
		if(T_abs(Pen_Point.X0,Pen_Point.X)>MOVE_PIXES&&T_abs(Pen_Point.Y0,Pen_Point.Y)<MOVE_PIXES)return MOVE_LEFT;
	}
	if(Pen_Point.Y>Pen_Point.Y0)//下移
	{
		if(T_abs(Pen_Point.Y0,Pen_Point.Y)>MOVE_PIXES&&T_abs(Pen_Point.X0,Pen_Point.X)<MOVE_PIXES)return MOVE_DOWN;
	}else			   //上移
	{
		if(T_abs(Pen_Point.Y0,Pen_Point.Y)>MOVE_PIXES&&T_abs(Pen_Point.X0,Pen_Point.X)<MOVE_PIXES)return MOVE_UP;
	}	 
	return 0;							 
}
//////////////////////////////与文件浏览相关的函数//////////////////////////////
//功能选择调度函数	    
//list  ：功能条目指针
//title ：标题
//func  ：选择/确定/。。。
//len   ：功能条目的个数(1~8)
//sel   ：开始选中的条目(0~7)
//返回值：0~7,选中的条目；8，没有选择任何条目。
u8 Load_Fun_Sel(const u8 * *list,const u8 *title,u8 *func,u8 len,u8 sel)
{	
	u8 t,key;
	Load_Gui_Bro(func);	   
	//显示标题
	POINT_COLOR=WHITE;
	Show_Str_Mid(0,2,(u8*)title,16,240); //叠加模式,非自动换行  
	POINT_COLOR=DARKBLUE;
	Show_Str(0,23,"请选择:",16,0x01);    //叠加模式,非自动换行	 
	//加载条目信息
	for(t=0;t<len;t++)
	{	 											    
		if(t==sel)
		{
			LCD_Fill(0,45+sel*32,239,37+(sel+1)*32,LBBLUE );//对选中的条目，填充棕色 
			POINT_COLOR=WHITE;
		}else POINT_COLOR=DARKBLUE;	  
		Show_Str(0,50+t*32,(u8*)list[t],16,0x01); //叠加模式,非自动换行       
	} 
 	key=0;
	Pen_Point.Key_Sta=Key_Up;//释放显示期间的触发	    
	while(1)//等待按键
	{	 
		key=AI_Read_TP(30);
		if(key)Pen_Point.Key_Reg|=0X80;//标记按键已经按下了 	   
		if(key==0&&(Pen_Point.Key_Reg&0X80))//按键松开了
		{
			Pen_Point.Key_Reg=0;			    
			key=Touch_To_Num(2);
			if(key)//有按键按下
			{  									    
				switch(key)
				{
					case 1:
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
					case 8:
					case 9://确认键按下
					key-=1;
					if(sel!=key&&key<len)//选中一个
					{
						POINT_COLOR=DARKBLUE;//画笔颜色选择		 
						LCD_Fill(0,45+sel*32,239,37+(sel+1)*32,WHITE);//清空上次选择的
						Show_Str(0,50+sel*32,(u8*)list[sel],16,0x01);//重新显示上次的名字
			        	sel=key;//选择这次选择的
						LCD_Fill(0,45+sel*32,239,37+(sel+1)*32,LBBLUE);//填充底色
						POINT_COLOR=WHITE;//画笔颜色选择		 
						Show_Str(0,50+sel*32,(u8*)list[sel],16,0x01);//显示名字 
					}else if(sel==key||key==8)//选择按钮,或者双击
					{	 	
						return sel;//返回选择的条目号
					}
						break;  		  	  
					case 10:return 8;//选择了"返回"按钮,返回一个不可能值  
				}  
				key=0;//取消按键值
			}											    
		}	  
		delay_ms(1);	
	}  
}
//显示文件
//fcnt:要显示文件的多少
//sel :选中第几个1~8 
//返回值:要打开的文件编号1~8(包括文件夹,在FileInfo里面的)	    
//返回9,代表直接退回上一层目录  		    
//显示文件
//check ok 09/04/19
u8 Show_File(u8 fcnt,u8 sel)
{
	u16 t;	
	u8 key; 					   
	FileInfoStruct temp;
	temp.F_Type=T_BMP;//文件属性设置

	sel-=1;										 
	LCD_Fill(0,42,239,297,WHITE);//填充白色,清空上次的显示
	for(t=0;t<fcnt;t++)
	{
		if(F_Info[t].F_Type==T_FILE)temp.F_StartCluster=file_ico[0];//文件夹图标
		else 
		{
			if(F_Info[t].F_Type&(T_JPG|T_JPEG|T_BMP))temp.F_StartCluster=file_ico[2];//图片类图标
			else if(F_Info[t].F_Type&(T_MP3|T_OGG|T_WMA|T_WAV|T_FLAC|T_MID))temp.F_StartCluster=file_ico[1];//音乐类图标
			else if(F_Info[t].F_Type&(T_LRC|T_TXT|T_C|T_H))temp.F_StartCluster=file_ico[3];//文字类图标
		}				   
		AI_LoadPicFile(&temp,0,42+t*32,32,42+t*32+32);//加载图标  
		if(t==sel)
		{
			LCD_Fill(32,45,239,69,LBBLUE);//填充棕色 BROWN	  
			POINT_COLOR=WHITE;
		}else POINT_COLOR=DARKBLUE;
		Show_Str(32,50+t*32,F_Info[t].F_Name,16,0x03); //叠加模式,非自动换行 
		//printf("N[%d]%s\n",t,F_Info[t].F_Name);      
	}
	key=0;
	Pen_Point.Key_Sta=Key_Up;//释放显示期间的触发
	while(1)//等待按键操作
	{
		key=AI_Read_TP(50);//适合滑动检测	    
		if(key)
		{
			if((Pen_Point.Key_Reg&0x03)==1)//第一次是滑动按下
			{
				t=KEY_Move_Check();//检查滑动按下
				if(t)
				{
					key=t;  
					goto KEY_D;
				}
			}
			Pen_Point.Key_Reg|=0X80;  //标记按键已经按下
		}	   
		if(key==0&&(Pen_Point.Key_Reg&0X80))//按键松开了
		{
			key=Touch_To_Num(1);
KEY_D:	    
			Pen_Point.Key_Reg=0;		  	
			if(key)//有按键按下
			{
				switch(key)
				{
					case 1:
					case 2:
			 		case 3:
					case 4:
					case 5:
					case 6:
					case 7:
					case 8:
						key-=1;
						if(sel!=key&&key<fcnt)//选中一个
						{
							POINT_COLOR=DARKBLUE;//画笔颜色选择		 
							LCD_Fill(32,45+sel*32,239,37+(sel+1)*32,WHITE);//清空上次选择的
							Show_Str(32,50+sel*32,F_Info[sel].F_Name,16,0x03);//重新显示上次的名字
				        	sel=key;//选择这次选择的
							LCD_Fill(32,45+sel*32,239,37+(sel+1)*32,LBBLUE);//清空上次选择的
							POINT_COLOR=WHITE;//画笔颜色选择		 
							Show_Str(32,50+sel*32,F_Info[sel].F_Name,16,0x03);//显示名字 
						}else if(sel==key)return sel+1;//再次选择,双击打开.
						break;
					case 9: //选择了"选择"按钮
						return sel+1; 	  
					case 10://选择了"返回"按钮 
						return 9;
					case MOVE_UP:  //翻页操作 
					case MOVE_LEFT: 
				    case MOVE_DOWN:  
					case MOVE_RIGHT: 
						return key;
				}
				key=0;//把按键值取消掉				  
				Pen_Point.Key_Sta=Key_Up;//释放显示期间的触发
			}  
		}
		delay_ms(1);
		t++;
		if(t%500==0)LED0=!LED0;	
	}
}
//浏览文件夹下面的文件
//file_type:要浏览的文件类型
//返回要打开的目标文件编号 1~n(在总目标文件里面的)
//不包括文件夹!!!
//如果返回0则代表退出浏览
u16 File_Browse(u32 file_type)
{			 
	FileInfoStruct *FileInfo;
	u16 folder_cnt;//文件夹的个数
	u16 des_cnt;   //目标文件的个数		    
	u8 fcnt;	   //显示的文件数目
	u16 temp; 					     									  

	u16 total_file;//总文件数目
	u16 pos_cnt;//当前文件号 1~folder_cnt+des_cnt
	u8 key;	  			  
							   			  
Reset:				    
	POINT_COLOR=BLUE;  	   		   //蓝色笔头使用
	folder_cnt=0;des_cnt=0;fcnt=0;key=0;pos_cnt=0;	 	
	Get_File_Info(Cur_Dir_Cluster,FileInfo,T_FILE,&folder_cnt);//得到当前目录下文件夹的个数	    
	Get_File_Info(Cur_Dir_Cluster,FileInfo,file_type,&des_cnt);//得到当前目录下目标文件的个数	 
									   
	total_file=des_cnt+folder_cnt;//总的文件数目=文件夹数目+目标文件数目
	while(1)
	{
		if(pos_cnt<total_file)//文件标尺,未到达文件末尾
		{	   
			FileInfo=&F_Info[fcnt];
			if(pos_cnt<folder_cnt)//找文件夹
			{
			    temp=pos_cnt+1;			    
				Get_File_Info(Cur_Dir_Cluster,FileInfo,T_FILE,&temp);
			}else				   //找目标文件
			{
				temp=pos_cnt-folder_cnt+1; 
				Get_File_Info(Cur_Dir_Cluster,FileInfo,file_type,&temp);
			}
			fcnt++;   //已存的文件数目增加
			pos_cnt++;//文件位置标志增加
		}	  
		if(fcnt==8||pos_cnt==total_file)//找满八个了,或者文件夹已经找完了
		{  
			key=Show_File(fcnt,1);//显示此次得到的文件
			switch(key)
			{
				case 1://要打开目标文件
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
					key-=1;
					if(key<fcnt)//按键在有效范围内
					{
						if(F_Info[key].F_Type==T_FILE)//要打开的是个文件夹
						{
							Cur_Dir_Cluster=F_Info[key].F_StartCluster;//打开这个文件夹		  
							goto Reset;
						}else    //要打开的是目标文件
						{	
							//LCD_Clear(WHITE);//清屏		   
							return pos_cnt+key-folder_cnt-fcnt+1;//返回目标文件编号
						}
					}
					break;
				case 9:	//选择了返回按钮.退出到上一层目录
					//在主目录浏览里面,按下返回按钮,则退出浏览模式	
					if((FAT32_Enable&&Cur_Dir_Cluster==FirstDirClust)||Cur_Dir_Cluster==0)return 0;//退出浏览模式	  					
					Cur_Dir_Cluster=Fat_Dir_Cluster;//退到父目录  			    	 
					goto Reset;	   	 
				case MOVE_UP:  //向上翻页
				case MOVE_LEFT:
					if(pos_cnt>8)pos_cnt-=(8+fcnt);//回到上一页
					else pos_cnt=0;//从头开始了
					break;
				case MOVE_DOWN://向下翻页
		    	case MOVE_RIGHT:
					if(pos_cnt==total_file)pos_cnt-=fcnt;//重新找
					break;

			}	 				    
			fcnt=0;//清除显示文件计数器
		}
	}		
}
//////////////////////////与LCD显示相关的函数//////////////////////////////////
//画粗线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void LCD_DrawDLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		Draw_Big_Point(uRow,uCol);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
} 	 
u16 *Temp_Color_Buf=(u16*)iclip;//把数据存放在iclip区域,与JPEG解码共享
//读取显存到Temp_Color_Buf
//(x,y):起始坐标
//xsize:x方向上的长度
//ysize:y方向上的长度
//总共读出的数据大小为:xsize*ysize
void Read_Temp_Color(u16 x,u16 y,u16 xsize,u16 ysize)
{
	u16 i,t;
	u16 tcnt=0;
	for(i=0;i<xsize;i++)
	{
		for(t=0;t<ysize;t++)
		{
			Temp_Color_Buf[tcnt]=LCD_ReadPoint(x+i,y+t);//读取当前点的颜色值
			tcnt++;
		}
	}
}
//将读取的显存Temp_Color_Buf重新显示出来
//(x,y):起始坐标
//xsize:x方向上的长度
//ysize:y方向上的长度
//总写入的数据大小为:xsize*ysize
void Reshow_Temp_Color(u16 x,u16 y,u16 xsize,u16 ysize)
{
	u16 i,t;
	u16 tcnt=0;
	for(i=0;i<xsize;i++)
	{
		for(t=0;t<ysize;t++)
		{
			POINT_COLOR=Temp_Color_Buf[tcnt];//读取当前点的颜色值
			LCD_DrawPoint(x+i,y+t);
			tcnt++;
		}
	}
}
//画颜色列表
//该颜色表外框大小为51*37
//分成5*7个颜色块,总共35种颜色
const u16 Color_Tab[3][7]=
{
{0XF800,0X7E00,0X001F,0X0000,0XFFFF,0X8000,0X0400},
{0XFFE0,0X07FF,0XF81F,0X2B12,0X8400,0XFFF0,0XFC10},
{0X0010,0X841F,0X041F,0XFC10,0X87F0,0XF810,0XFC00},
};
//画颜色列表
//该颜色表外框大小为65*29
//分成3*7个颜色块,总共21种颜色
//#define FRAME1 0XEF5B //窗体色 	 
//#define FRAME2 0XDED7 //窗体色	 
void Draw_Color_Table(u16 x,u16 y)
{	
	u8 t,i;	  
	POINT_COLOR=FRAME2;
	for(i=0;i<4;i++) 
	{	 
		LCD_DrawLine(x,y+i*9,x+63,y+i*9);
	}
	for(i=0;i<4;i++) 
	{					    
		LCD_DrawLine(x,y+i*9+1,x+63,y+i*9+1);
	}		    
	for(i=0;i<8;i++) 
	{	 
		LCD_DrawLine(x+9*i,y,x+9*i,y+28);
	}
	for(i=0;i<8;i++)//画小边框
	{					    
		LCD_DrawLine(x+9*i+1,y,x+9*i+1,y+28);
	}
	for(t=0;t<3;t++)
	{
		for(i=0;i<7;i++)
		{
			LCD_Fill(x+2+i*9,y+2+t*9,x+2+i*9+6,y+2+t*9+6,Color_Tab[t][i]);//把多余的去掉
		}
	}								    	  
}










