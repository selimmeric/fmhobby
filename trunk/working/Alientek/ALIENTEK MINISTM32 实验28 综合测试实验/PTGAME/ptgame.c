#include "ptgame.h"
#include "gui.h"
#include "sysfile.h"
#include "lcd.h"
#include "delay.h"
#include "fat.h"
#include "jpegbmp.h"
#include "touch.h"
#include "24cxx.h"
#include "text.h"
#include "usart.h"
#include "delay.h"
#include "led.h"

//拼图游戏代码
//支持 3*3 4*4 5*5三个等级，要继续添加也是比较容易的
//基本只要修改图片编号Cur_Table的大小，PT_Size的大小就可以了
//不过要注意不要超出数据类型的范围						    
//正点原子@SCUT
//2009/05/30 
//V1.2												  

//存储在sram里面的 图片起始地址（在SD卡上） 列表
//u32 *Pic_Addr=(u32*)iclip;//注意 0：表示预览图片的地址 1~n，表示1~n块图片的地址
u8 Cur_Table[26];//图片编号存储列表	5*5时大小为25+1=26

u8 Xscal;        //倍乘数
//这四个变量需要保存到EEPROM里面
u8 PT_Size=3;    //拼图游戏的难度级别 3、4两个值

u8  PT_New=0; //是否已经在玩？1，没有在玩，必须更新。0，已经在玩，但是未完成，可以不更新。
u16 PT_Step=0;//拼图所用步骤
u16 PT_Time=0;//拼图所用时间	
								   
const u8 *PTGmenu_F1[1]={"拼图游戏"};//第1级父目录目录
const u8 *PTGmenu_S1[4]={"继续","新游戏","最佳排行","游戏设置"};//第1级子目录
const u8 *PTGmenu_S2[3]={"简单","一般","困难"}; //第2级子目录 设置	 

#define PT_BASIC 1 //基本信息
#define PT_TABLE 2 //图表信息	 

//保存当前游戏信息
//地址区间：412~443
#define PTGAME_24C0X_BASE_ADDR 200
void Save_PTG_Info(u8 type)
{			 
	u8 i;
	if(type==PT_BASIC)//保存基本信息
	{								 
		AT24CXX_WriteOneByte(PTGAME_24C0X_BASE_ADDR,PT_Size);//保存图片大小
		AT24CXX_WriteOneByte(PTGAME_24C0X_BASE_ADDR+1,PT_New); //保存是否已经在玩
		//保存当前所用步骤
		AT24CXX_WriteLenByte(PTGAME_24C0X_BASE_ADDR+2,PT_Step,2);	
		//保存当前所用时间
		AT24CXX_WriteLenByte(PTGAME_24C0X_BASE_ADDR+4,PT_Time,2);
	}else if(type==PT_TABLE)//保存图标信息
	{
		for(i=0;i<26;i++)AT24CXX_WriteOneByte(PTGAME_24C0X_BASE_ADDR+6+i,Cur_Table[i]);//保存当前图表信息	  
	}
}
//读取当前游戏信息
//地址区间：412~443 
void Read_PTG_Info(u8 type)
{			 
	u8 i;
	if(type==PT_BASIC)//读取基本信息
	{										
		PT_Size=AT24CXX_ReadOneByte(PTGAME_24C0X_BASE_ADDR);//读取图片大小
		PT_New=AT24CXX_ReadOneByte(PTGAME_24C0X_BASE_ADDR+1); //读取是否已经在玩
		//读取当前所用步骤	    
		PT_Step=AT24CXX_ReadLenByte(PTGAME_24C0X_BASE_ADDR+2,2);	 	   
		//读取当前所用时间
		PT_Time=AT24CXX_ReadLenByte(PTGAME_24C0X_BASE_ADDR+4,2);		   
	}else if(type==PT_TABLE)//保存图标信息
	{
		for(i=0;i<26;i++)Cur_Table[i]=AT24CXX_ReadOneByte(PTGAME_24C0X_BASE_ADDR+6+i);//读取当前图表信息	  
	}
}
//保存最佳值
//level:等级 0~2 
// step:步数
// time:用时
//地址区间:444~	455
void Save_PTG_Best(u8 level,u16 step,u16 time)
{		
	//保存当前所用步数
	AT24CXX_WriteLenByte(PTGAME_24C0X_BASE_ADDR+32+level*4,step,2); 	 		
	//保存当前所用时间
	AT24CXX_WriteLenByte(PTGAME_24C0X_BASE_ADDR+34+level*4,time,2);	   	
}
//读取最佳值
//level:等级 0~2 == 3 4 5 
// step:步数
// time:用时
//地址区间:444~	455
void Read_PTG_Best(u8 level,u16 *step,u16 *time)
{				
	//读取当前所用步骤	
	*step=AT24CXX_ReadLenByte(PTGAME_24C0X_BASE_ADDR+32+level*4,2); 	 
	//读取当前所用时间
	*time=AT24CXX_ReadLenByte(PTGAME_24C0X_BASE_ADDR+34+level*4,2); 			
}

//对指定列表，得到图片起始坐标点
//这个函数与LCD显示有关
//table:固定区域值 0~PT_Size*PT_Size
//xpos :x坐标值
//ypos :y坐标值	
//CHECK OK 09/05/30			 
void Get_Ps_Addr(u8 table,u8 *xpos,u8 *ypos)
{
	u8 x,y;						 
	for(y=0;y<PT_Size;y++)
	{
		for(x=0;x<PT_Size;x++)
		{
			if(table==y*PT_Size+x)
			{
				*xpos=Xscal*x+5;
				*ypos=Xscal*y+5;
				return;
			}
		}
	}
	if(table==PT_Size*PT_Size)//第PT_Size*PT_Size格的起始地址
	{
		*xpos=Xscal*(PT_Size-1)+5;
		*ypos=Xscal*PT_Size+5;
	}
}
/////////////////////////////////////////////////////////////////////
//检查拼图游戏获胜的算法
//返回值：0，没成功；1，成功了
//支持从3*3~15*15的棋盘大小
//CHECK OK 09/05/30
u8 PT_Win_Chk(void)
{
	u8 t;
	u8 size;
	size=PT_Size*PT_Size; 
	for(t=0;t<size;t++){if(Cur_Table[t]!=t)break;}    
	if(t==size)return 1;
	return 0;
}
//初始化(生成随机位置)	 
//带检测拼图能否完成的算法
//CHECK OK 09/05/30
void PT_Map_Init(void)
{		 
	u8 size;		 
	u8 i,t,a, b;	 
	u8 d=1;
	signed char w,h;	 
	size=PT_Size*PT_Size;	 								  
	srand(RTC->CNTL);//得到种子	   
	for(i=0;i<size;i++)Cur_Table[i]=i;	  
	Cur_Table[size]=size-1;  //最后一个图片，必须放在原位，否则游戏无法完成 
	// 初始化(生成随机位置)	 	   
	i=0;
	while(i++<size||d==0)// 交换至少单位数量次
	{
		do 	
		{										  
			a = rand()%(size);			   
			b = rand()%(size);
		}while(a==b);   
		t=Cur_Table[a];
		Cur_Table[a]=Cur_Table[b];
		Cur_Table[b]=t;
		// 计算距离，必须要模为1有解
		if(Cur_Table[a]==(size-1)|| Cur_Table[b]==(size-1))
		{
			w=a%PT_Size-b%PT_Size+1;
			h=a/PT_Size-b/PT_Size;
			w=w<0?-w:w;
			h=h<0?-h:h;
			d=(d+w+h)%2;
		}else d=!d;	 
	}	 
	for(i=0;i<size;i++)
	{
		if(Cur_Table[i]==size-1){Cur_Table[i]=size;break;}	   
	}
}

//装载游戏图片
//支持从3*3~15*15的棋盘大小
//CHECK OK 09/05/30
void Load_Game_PIC(void) 
{
	FileInfoStruct TempPic;//部分图片的临时存放空间	    
	u8 t;
	u8 temp;
	u8 tempx,tempy; 
	u8 size;
	size=PT_Size*PT_Size;   									  
	TempPic.F_StartCluster=Pic_Addr[0];//加载预览图片		   
	TempPic.F_Type=T_BMP;//指定为BMP图片
	AI_LoadPicFile(&TempPic,3,242,79,318);//装载预览图片 
	//加载	  
	for(t=0;t<size+1;t++)
	{
		temp=Cur_Table[t];
		Get_Ps_Addr(t,&tempx,&tempy);   
		if(temp==size)//空格所在区域，填充灰色	
		{
		 	LCD_Fill(tempx,tempy,tempx+Xscal-2,tempy+Xscal-2,LGRAY);	 
		}else
		{
			TempPic.F_StartCluster=Pic_Addr[temp+1];//图片地址		   
			AI_LoadPicFile(&TempPic,tempx,tempy,tempx+Xscal-1,tempy+Xscal-1);	 
		}
	}	 
}
//sour:按键所按的固定编号
//dest:空格所在的固定编号
//空格一定是在dest区里面
//CHECK OK 09/05/30
void Move_Pic(u8 sour,u8 dest)
{
	FileInfoStruct PicPart;//部分图片的临时存放空间	    
	u8 pic_real_addr;
	u8 tempx,tempy;			 

	pic_real_addr=Cur_Table[sour]+1;//得到图片的编号地址 
	PicPart.F_StartCluster=Pic_Addr[pic_real_addr];
	PicPart.F_Type=T_BMP;//指定为BMP图片	
	//清空sour区的图片，就是把空格移到这个区
	Get_Ps_Addr(sour,&tempx,&tempy);//得到源坐标   
	LCD_Fill(tempx,tempy,tempx+Xscal-2,tempy+Xscal-2,LGRAY);											    	 
	//把sour区的图片部分显示到dest区
	Get_Ps_Addr(dest,&tempx,&tempy);//得到目标坐标
	AI_LoadPicFile(&PicPart,tempx,tempy,tempx+Xscal-1,tempy+Xscal-1);												 
	Cur_Table[dest]=Cur_Table[sour];//把sour区的图片编号赋给dest区
	Cur_Table[sour]=PT_Size*PT_Size;//空格在sour区了，调换完成 	
}   
	    
//返回值：PT_Size*PT_Size+1，不需要移动
//0~PT_Size*PT_Size 需要移动，与key交换
//拼图游戏的核心算法		   
//支持从3*3~15*15的棋盘大小
//CHECK OK 09/05/30
u8 Need_Move(u8 key)
{
	u8 temp;
	u8 size;
	u8 i;
	size=PT_Size*PT_Size;	  
	if(Cur_Table[key]==size)return size+1;//点中了空格，不移动
	else 
	{
		///////////////////////////////////判断是不是需要减PT_Size	 
		if(key>=PT_Size)//判断减PT_Size(3,4,5...)
		{
			if(key==size)temp=size-1;//按中了最后一个 9，对于这个按键，-1相当于其他按键-size
			else temp=key-PT_Size;
			if(Cur_Table[temp]==size)return temp;//空格与此次按键值相邻，返回空格所在地址
		}	
		///////////////////////////////////判断是不是需要加PT_Size     
		if(key<(size-PT_Size))//判断加PT_Size
		{
			temp=key+PT_Size;
			if(Cur_Table[temp]==size)return temp;//空格与此次按键值相邻，返回空格所在地址
		}    
		///////////////////////////////////判断是不是需要减1 
		for(i=0;i<PT_Size;i++)//边际处理，3：0,3,6;4:0,4,8,12;5:0,5,10,15,20....这些数值不需要减1
		{
			if(key==PT_Size*i)break;
		}
		if(i==PT_Size)//需要减1的
		{
			temp=key-1;			    
			if(Cur_Table[temp]==size)return temp;//空格与此次按键值相邻，返回空格所在地址
		}	 
		///////////////////////////////////判断是不是需要加1
		for(i=0;i<PT_Size;i++)//边际处理
		{
			if((key+1)==PT_Size*i)break;
		}
		if(i==PT_Size)//需要加1的
		{
			temp=key+1;//判断+1		 
		    if(temp==(size+1))return size+1;//不需要移动  
			if(Cur_Table[temp]==size)return temp;//空格与此次按键值相邻，返回空格所在地址
		}	 
		return size+1;//不需要移动
	}
}
/////////////////////////////////////////////////////////////////////
//拼图游戏的按键值获取函数
//返回按键值 		 
u8 PTGame_Key_To_Num(void)
{
	u8 t,ytemp;						   
	if(Is_In_Area(100,300,33,16))return 0XFF;//选择了退出按键，退出模式		 
	for(t=0;t<PT_Size;t++)//返回常规按键
	{	  		   	
		for(ytemp=0;ytemp<PT_Size;ytemp++)
		if(Is_In_Area(5+Xscal*t,5+Xscal*ytemp,Xscal,Xscal))return t+ytemp*PT_Size+1;
	}    
	if(PT_Size==3&&Is_In_Area(159,236,77,77))return 10;
	else if(PT_Size==4&&Is_In_Area(179,237,58,77))return 17;
	else if(PT_Size==5&&Is_In_Area(188,234,46,46))return 26;
	return 0;//按键无效
} 				    
//显示数据
//显示步骤和时间	 
void PTGame_Show_Msg(void)
{
	POINT_COLOR=RED;//画笔颜色
	BACK_COLOR=LGRAY;	   
	LCD_ShowNum(121,258,PT_Step,4,16);//显示步数
	LCD_ShowNum(121,274,PT_Time,4,16);//显示时间	  		  
}
//获得RTC内的秒钟计数器	
u32 Get_PTG_Time(void)
{
	u32 temp;
	temp=RTC->CNTH;
	temp<<=16;
	temp=RTC->CNTL;  
	return temp; 
}
//拼图游戏胜利时的界面
//包括是否创纪录检查    
void PTG_Win_Msg(void)
{		   
	u8 BEST=0;
	u16 timex,stepx;  
	u16 minx=0;	 
	u8 tkey;  
	//开始判断是不是创纪录了
	Read_PTG_Best(PT_Size-3,&stepx,&timex);//读取该级别的最纪录	
	if(timex==0)BEST=1;				       //第一次
	if(timex<stepx)minx=timex;			   //去最小的值
	else minx=stepx;      
	if(PT_Time<minx||PT_Step<minx)BEST=1;//时间/步数小于先前最佳值，记录被破  
	else if(PT_Time==timex&&PT_Step<stepx)BEST=1;//时间相等，步数较少，也破纪录
	 
	LCD_Fill(40,80,200,160,GRAY);//填充空白域
	POINT_COLOR=BROWN;
	LCD_DrawRectangle(39,79,201,161);//画空的方框
	LCD_DrawRectangle(40,80,200,160);//画空的方框
	LCD_DrawRectangle(41,81,199,159);//画空的方框	 
	BACK_COLOR=GRAY;
	POINT_COLOR=RED;
	if(BEST)
	{	
		Save_PTG_Best(PT_Size-3,PT_Step,PT_Time);//保存最佳纪录
		Show_Str_Mid(40,82,"创记录了！",16,160);//叠加模式,非自动换行						 
	}
	else Show_Str_Mid(40,82,"恭喜过关！",16,160);//叠加模式,非自动换行
	POINT_COLOR=BLACK;	
	Show_Str(44,100,"您的成绩：",16,0X01);//叠加模式,非自动换行
	Show_Str(44,118,"步数:",16,0X01);//叠加模式,非自动换行	  
	Show_Str(124,118,"时间:",16,0X01);//叠加模式,非自动换行
	POINT_COLOR=RED;
	LCD_ShowNum(84,118,PT_Step,4,16);//显示步数
	LCD_ShowNum(164,118,PT_Time,4,16);//显示时间   
	BACK_COLOR=LGRAY;
    Show_Str(104,139,"确定",16,0X00);//叠加模式,非自动换行		   
	POINT_COLOR=BROWN;	   
	LCD_DrawRectangle(104,139,136,155);//画空的方框
	
	Pen_Point.Key_Sta=Key_Up;//释放显示期间的触发 
	timex=0;
	while(1)//等待按键退出
	{
		tkey=AI_Read_TP(30);
		if(tkey==2)Pen_Point.Key_Reg|=0X80;//标记按键已经按下了 	   
		if(tkey==0&&(Pen_Point.Key_Reg&0X80))//按键松开了
		{
			Pen_Point.Key_Reg=0;
  			if(Is_In_Area(104,139,32,16))return;//确定键被按下了  

		}   
	}
}	
//显示最佳纪录
void PTG_Best_Show(void)
{	
	u8 temp;
	u16 timex,stepx;

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
	//显示标题
	POINT_COLOR=WHITE;
	Show_Str_Mid(0,2,"最佳纪录",16,240); //叠加模式,非自动换行 
	Show_Str(5,300,"确定",16,0x01);   //叠加模式,非自动换行
    //显示信息	   
	POINT_COLOR=RED;
	Show_Str_Mid(0,50,"简单",16,240);   
	Show_Str_Mid(0,114,"一般",16,240); 
	Show_Str_Mid(0,178,"困难",16,240); 
		  
	LCD_Fill(0,74,239,106,LGRAY);   //填充颜色
	LCD_Fill(0,138,239,170,LGRAY);  //填充颜色
	LCD_Fill(0,202,239,234,LGRAY);  //填充颜色
	
	BACK_COLOR=LGRAY;//假定没有选中，原背景 
	POINT_COLOR=DARKBLUE;    
	for(temp=0;temp<3;temp++)
	{
		Show_Str(32,82+temp*64,"步数:",16,0x01);   //叠加模式,非自动换行 
		Show_Str(136,82+temp*64,"时间:",16,0x01);  //叠加模式,非自动换行 
	}
	POINT_COLOR=RED; 
	for(temp=0;temp<3;temp++)
	{
		Read_PTG_Best(temp,&stepx,&timex);//读取最佳值	 
		LCD_ShowNum(72,82+temp*64,stepx,4,16);//显示步数
		LCD_ShowNum(176,82+temp*64,timex,4,16);//显示时间 	   
	}
	Pen_Point.Key_Sta=Key_Up;//释放显示期间的触发 
	temp=0;
	while(1)//等待按键退出
	{
		temp=AI_Read_TP(30);
		if(temp==2)Pen_Point.Key_Reg|=0X80;//标记按键已经按下了 	   
		if(temp==0&&(Pen_Point.Key_Reg&0X80))//按键松开了
		{
			Pen_Point.Key_Reg=0;
  			if(Is_In_Area(0,300,40,20))return;//确定键被按下了  

		}   	    
	}	  						   	   
}   
//玩拼图游戏									   
void PTGame_Play(void)
{
	u8 key,temp;
	u8 size;		  
	u32 game_time;		    

	//首先得到游戏的图片文件
	temp=SysInfoGet(0x80+PT_Size);//查找图片
	if(temp)
	{
	    LCD_Clear(WHITE);//清屏  		  
		POINT_COLOR=RED;
		Show_Str_Mid(0,144,"拼图游戏数据丢失",16,240); //叠加模式,非自动换行
		Show_Str_Mid(0,160,"请检查系统文件的完整性！",16,240); //叠加模式,非自动换行
		delay_ms(1500);
		return;
	}			    		 
	//第一步要得到缩放因子
	if(PT_Size==3)Xscal=77;
	else if(PT_Size==4)Xscal=58;
	else if(PT_Size==5)Xscal=46;    
	size=PT_Size*PT_Size; 
	if(PT_New)
	{
		PT_Time=0;//时间清空
		PT_Step=0;//步数清空		
		PT_Map_Init();//需要重新打乱图片
	}else Read_PTG_Info(PT_TABLE);//读取列表信息		 

	Load_Game_ICO(PT_Size);//装载X*X界面
	Load_Game_PIC();//装载图片
	//得到游戏开始时间
	game_time=Get_PTG_Time();//得到时间
	key=0;		  
	PTGame_Show_Msg();//显示时间，步数  
	Pen_Point.Key_Sta=Key_Up;//释放显示期间的触发 
	while(1)//等待按键
	{
		key=AI_Read_TP(30);	    
		if(key)Pen_Point.Key_Reg|=0X80;  //标记按键已经按下了(不管是滑动还是顶点) 	   
		if(key==0&&(Pen_Point.Key_Reg&0X80))//按键松开了
		{
			Pen_Point.Key_Reg=0;		  	
			key=PTGame_Key_To_Num(); 		   	   
			if(key==0XFF)
			{
				PT_New=0;//游戏没有结束，下次可以不更新
				Save_PTG_Info(PT_BASIC);//保存基本信息
				Save_PTG_Info(PT_TABLE);//保存列表信息
				return;//选择了退出	 
			}else
			{
				key-=1;//返回值为1~10，减少1，匹配
				temp=Need_Move(key);//级别1		  
				if(temp<=size)
				{
					Move_Pic(key,temp);//把key那里的图片，调换到temp那里
					PT_Step++;         //步骤增加
					PTGame_Show_Msg(); //显示时间，步数   
					if(PT_Win_Chk())//游戏结束，拼图成功了
					{								 	 
						PT_New=1;//游戏结束，下次必须更新
						Save_PTG_Info(PT_BASIC);//保存基本信息
						PTG_Win_Msg();//成功时的界面现实
						return; 
					}
				}
			}
			key=0;
		}
		if(game_time!=Get_PTG_Time())//秒钟增加了1s
		{
			game_time=Get_PTG_Time();
			PT_Time++;//时间增加
			LED0=!LED0;//LED闪烁
			PTGame_Show_Msg();//显示时间，步数
		}
	}	 
}	    
//游戏 界面
void Game_Play(void)
{			    
	u8 selx=0;	 
	while(1)
	{
		selx=Load_Fun_Sel(PTGmenu_F1,"游戏选择","确定",1,0);
		if(selx==8)return;//返回
		else if(selx==0)//选中了拼图游戏，暂时只有这一个游戏
		{
		 	Read_PTG_Info(PT_BASIC);//读取基本信息
			if(PT_Size>5||PT_Size<3)//信息非法,重新保存信息
			{
				PT_Size=3;PT_New=0;
				PT_Step=0;PT_Time=0;
				Save_PTG_Info(PT_BASIC);//保存基本信息	   
				for(selx=0;selx<26;selx++)Cur_Table[selx]=0;
				Save_PTG_Info(PT_TABLE);//保存图标信息
				Save_PTG_Best(0,0,0);
				Save_PTG_Best(1,0,0);
				Save_PTG_Best(2,0,0);
			}else if(!PT_New)Read_PTG_Info(PT_TABLE);//读取图标信息

			while(1)
			{	    
				if(PT_New)selx=Load_Fun_Sel(PTGmenu_S1+1,"拼图游戏","确定",3,0);//不显示继续按钮
				else selx=Load_Fun_Sel(PTGmenu_S1,"拼图游戏","确定",4,0);//显示继续按钮
				if(selx==8)break;//返回上一层目录
				if(PT_New)selx+=1;//偏移一下
				switch(selx)
				{
					case 0://选择了继续
						PTGame_Play();
						break;
					case 1://选择了新游戏
						PT_New=1;//
						PTGame_Play();
						break;
					case 2://最佳排行
						PTG_Best_Show();
						break;
					case 3://难度设置
						selx=Load_Fun_Sel(PTGmenu_S2,"难度选择","确定",3,PT_Size-3);//选择难度
						if(selx<3)//选中了有效按键
						{
							PT_Size=3+selx; 				
							PT_New=1;//必须要更新随机序列 
						}
						break;
				}
			}
		}	 
	} 
}






























