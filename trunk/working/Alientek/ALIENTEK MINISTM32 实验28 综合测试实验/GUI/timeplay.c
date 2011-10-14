#include "timeplay.h"
#include "gui.h"			   
#include "lcd.h"			 
#include "delay.h"
#include "text.h"
#include "touch.h"
#include "rtc.h" 
#include "ds18b20.h"
#include "adc.h"
#include "bigascii.h"  	  
#include "usart.h"	 
#include "led.h"	  
//Mini STM32开发板范例代码28
//综合 实验
//正点原子@ALIENTEK
	
//在指定位置显示一个大字符
//30*60大小的
//num:0~9
//:/./C这三个字符
void LCD_ShowBigChar(u8 x,u16 y,u8 num)
{
    u8 n,t;
	u8 temp;
	u8 t1,deadline;
	LCD_WriteReg(R80,x);           //水平方向GRAM起始地址
	LCD_WriteReg(R81,x+29);        //水平方向GRAM结束地址
	LCD_WriteReg(R82,y);           //垂直方向GRAM起始地址
	LCD_WriteReg(R83,y+59);        //垂直方向GRAM结束地址	
	LCD_SetCursor(x,y);            //设置光标位置   
	if(num==':')t1=150;
	else if(num=='.')t1=165;
	else if(num=='C')t1=180;
	else t1=15*num;
	deadline=t1+15;
	LCD_WriteRAM_Prepare();        //开始写入GRAM			   
	for(;t1<deadline;t1++)
	{	 
		for(n=0;n<16;n++)
		{
			temp=BIG_ASCII[t1][n];
			for(t=0;t<8;t++)
			{
				if(temp&0x80){LCD_WR_DATA(POINT_COLOR);}
				else {LCD_WR_DATA(BACK_COLOR);} 
				temp<<=1;
				if(((n%4)==3)&&t==5)break;
			}
		}
	} 	  
	//恢复面板
	LCD_WriteReg(R80, 0x0000); //水平方向GRAM起始地址
	LCD_WriteReg(R81, 0x00EF); //水平方向GRAM结束地址
	LCD_WriteReg(R82, 0x0000); //垂直方向GRAM起始地址
	LCD_WriteReg(R83, 0x013F); //垂直方向GRAM结束地址
}   
//显示一个数字
//x,y:起点坐标
//num:数值(0~9);
void LCD_ShowBigNum(u8 x,u16 y,u8 num)
{   
    LCD_ShowBigChar(x,y,(num/10)%10); 
    LCD_ShowBigChar(x+30,y,num%10);        							     
} 	


const u8 *week_table[7]={"星期天","星期一","星期二","星期三","星期四","星期五","星期六"};
//根据当前的日期,更新日历表.
void Date_Refresh(void) 
{
 	u8 weekn;   //周寄存
	//从阳历,得到阴历日期	   	    
	POINT_COLOR=GREEN;		  					  
	//显示阳历年月日
	POINT_COLOR=BRED;  
	LCD_Show2Num(5,193,(timer.w_year/100)%100,2,16,0);//显示年  20/19  
	LCD_Show2Num(21,193,timer.w_year%100,2,16,0);     //显示年  
	LCD_ShowString(37,193,"-"); //"-"
	LCD_Show2Num(45,193,timer.w_month,2,16,0);        //显示月
	LCD_ShowString(61,193,"-"); //"-"
	LCD_Show2Num(69,193,timer.w_date,2,16,0);         //显示日	  
	//显示周几?
	POINT_COLOR=RED;
    weekn=RTC_Get_Week(timer.w_year,timer.w_month,timer.w_date);//得到星期	   
	Show_Str(21,218,(u8 *)week_table[weekn],16,0x01); //显示周几?	
													 
}
//时间显示模式
void Time_Viewer(void)
{
	u8 key;	    
	u8 second;
	short temperate=0;//温度值		   
	u8 t=0;
	u8 tempdate;
	u8 TEMP_SEN_TYPE=0;//默认使用DS18B20	  
	while(!(RTC->CRL&(1<<3)));//等待RTC寄存器同步  
	RTC->CRH|=0X01;  		  //允许秒中断
	while(!(RTC->CRL&(1<<5)));//等待RTC寄存器操作完成   
	LCD_Clear(BLACK);//清黑屏    	  
	second=timer.sec;//得到此刻的秒钟
	POINT_COLOR=GBLUE;
	Show_Str(48,60,"正在加载,请稍候...",16,0x01); //显示进入信息	    
	if(DS18B20_Init())
	{
		Show_Str(48,76,"未检测到DS18B20!",16,0x01);  
		delay_ms(500);
		Show_Str(48,92,"启用内部温度传感器...",16,0x01);  
		Adc_Init();//初始化内部温度传感器	  
		TEMP_SEN_TYPE=1; 
	}   	    
	delay_ms(1100);//等待1.1s 
	BACK_COLOR= BLACK;
	LCD_Clear(BLACK);//清黑屏 
	Date_Refresh();  //加载日历
	tempdate=timer.w_date;//天数暂存器

	Pen_Point.Key_Sta=Key_Up;         //释放显示期间的触发	   	  
	while(1)
	{	
		key=AI_Read_TP(50);//适合滑动检测	    
		if(key)
		{
			if((Pen_Point.Key_Reg&0x03)==1)//第一次是滑动按下
			{
				t=KEY_Move_Check();//检查滑动按下
				if(t==MOVE_LEFT)break;//退出时钟模式 	  
			}else Pen_Point.Key_Reg=0; 											    
		}	   	 
		if(second!=timer.sec) //秒钟改变了
		{ 	
			LED0=!LED0;//闪灯
			second=timer.sec;		
			//显示大体的时钟
			POINT_COLOR=GBLUE;
			LCD_ShowBigNum(0,100,timer.hour);//显示时
			LCD_ShowBigChar(60,100,':'); //"/"
			LCD_ShowBigNum(90,100,timer.min);//显示分
			LCD_ShowBigChar(150,100,':'); //"/"
			LCD_ShowBigNum(180,100,timer.sec);//显示秒	 					   
			
			if(t%6==0)//等待五秒钟
			{		 
				if(TEMP_SEN_TYPE)temperate=Get_Temp();//得到内部温度
				else temperate=DS18B20_Get_Temp();//得到18b20温度
				if(temperate<0)//温度为负数的时候，红色显示
				{
					POINT_COLOR=RED;
					temperate=-temperate;//改为正温度
				}
				else POINT_COLOR=BRRED;		   //正常为棕红色字体显示
				LCD_ShowBigNum(90,184,temperate/10);//XX
				LCD_ShowBigChar(150,184,'.'); //"."
				LCD_ShowBigChar(180,184,temperate%10);//显示小数
				LCD_ShowBigChar(210,184,'C'); //"/"
				if(t>0)t=0;			 
			}  
			if(timer.w_date!=tempdate)
			{
				Date_Refresh();//天数变化了,更新日历.  
				tempdate=timer.w_date;//修改tempdate，防止重复进入
			}
			t++;  
		}			 			 
	};
	while(!(RTC->CRL&(1<<3)));//等待RTC寄存器同步  
	RTC->CRH&=~(0X01);  	  //关闭秒中断
	while(!(RTC->CRL&(1<<5)));//等待RTC寄存器操作完成
	LCD_Clear(WHITE);//清黑屏    
	POINT_COLOR=BLUE;
	BACK_COLOR=WHITE ;	
}



