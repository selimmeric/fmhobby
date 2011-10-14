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
//Mini STM32�����巶������28
//�ۺ� ʵ��
//����ԭ��@ALIENTEK
	
//��ָ��λ����ʾһ�����ַ�
//30*60��С��
//num:0~9
//:/./C�������ַ�
void LCD_ShowBigChar(u8 x,u16 y,u8 num)
{
    u8 n,t;
	u8 temp;
	u8 t1,deadline;
	LCD_WriteReg(R80,x);           //ˮƽ����GRAM��ʼ��ַ
	LCD_WriteReg(R81,x+29);        //ˮƽ����GRAM������ַ
	LCD_WriteReg(R82,y);           //��ֱ����GRAM��ʼ��ַ
	LCD_WriteReg(R83,y+59);        //��ֱ����GRAM������ַ	
	LCD_SetCursor(x,y);            //���ù��λ��   
	if(num==':')t1=150;
	else if(num=='.')t1=165;
	else if(num=='C')t1=180;
	else t1=15*num;
	deadline=t1+15;
	LCD_WriteRAM_Prepare();        //��ʼд��GRAM			   
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
	//�ָ����
	LCD_WriteReg(R80, 0x0000); //ˮƽ����GRAM��ʼ��ַ
	LCD_WriteReg(R81, 0x00EF); //ˮƽ����GRAM������ַ
	LCD_WriteReg(R82, 0x0000); //��ֱ����GRAM��ʼ��ַ
	LCD_WriteReg(R83, 0x013F); //��ֱ����GRAM������ַ
}   
//��ʾһ������
//x,y:�������
//num:��ֵ(0~9);
void LCD_ShowBigNum(u8 x,u16 y,u8 num)
{   
    LCD_ShowBigChar(x,y,(num/10)%10); 
    LCD_ShowBigChar(x+30,y,num%10);        							     
} 	


const u8 *week_table[7]={"������","����һ","���ڶ�","������","������","������","������"};
//���ݵ�ǰ������,����������.
void Date_Refresh(void) 
{
 	u8 weekn;   //�ܼĴ�
	//������,�õ���������	   	    
	POINT_COLOR=GREEN;		  					  
	//��ʾ����������
	POINT_COLOR=BRED;  
	LCD_Show2Num(5,193,(timer.w_year/100)%100,2,16,0);//��ʾ��  20/19  
	LCD_Show2Num(21,193,timer.w_year%100,2,16,0);     //��ʾ��  
	LCD_ShowString(37,193,"-"); //"-"
	LCD_Show2Num(45,193,timer.w_month,2,16,0);        //��ʾ��
	LCD_ShowString(61,193,"-"); //"-"
	LCD_Show2Num(69,193,timer.w_date,2,16,0);         //��ʾ��	  
	//��ʾ�ܼ�?
	POINT_COLOR=RED;
    weekn=RTC_Get_Week(timer.w_year,timer.w_month,timer.w_date);//�õ�����	   
	Show_Str(21,218,(u8 *)week_table[weekn],16,0x01); //��ʾ�ܼ�?	
													 
}
//ʱ����ʾģʽ
void Time_Viewer(void)
{
	u8 key;	    
	u8 second;
	short temperate=0;//�¶�ֵ		   
	u8 t=0;
	u8 tempdate;
	u8 TEMP_SEN_TYPE=0;//Ĭ��ʹ��DS18B20	  
	while(!(RTC->CRL&(1<<3)));//�ȴ�RTC�Ĵ���ͬ��  
	RTC->CRH|=0X01;  		  //�������ж�
	while(!(RTC->CRL&(1<<5)));//�ȴ�RTC�Ĵ����������   
	LCD_Clear(BLACK);//�����    	  
	second=timer.sec;//�õ��˿̵�����
	POINT_COLOR=GBLUE;
	Show_Str(48,60,"���ڼ���,���Ժ�...",16,0x01); //��ʾ������Ϣ	    
	if(DS18B20_Init())
	{
		Show_Str(48,76,"δ��⵽DS18B20!",16,0x01);  
		delay_ms(500);
		Show_Str(48,92,"�����ڲ��¶ȴ�����...",16,0x01);  
		Adc_Init();//��ʼ���ڲ��¶ȴ�����	  
		TEMP_SEN_TYPE=1; 
	}   	    
	delay_ms(1100);//�ȴ�1.1s 
	BACK_COLOR= BLACK;
	LCD_Clear(BLACK);//����� 
	Date_Refresh();  //��������
	tempdate=timer.w_date;//�����ݴ���

	Pen_Point.Key_Sta=Key_Up;         //�ͷ���ʾ�ڼ�Ĵ���	   	  
	while(1)
	{	
		key=AI_Read_TP(50);//�ʺϻ������	    
		if(key)
		{
			if((Pen_Point.Key_Reg&0x03)==1)//��һ���ǻ�������
			{
				t=KEY_Move_Check();//��黬������
				if(t==MOVE_LEFT)break;//�˳�ʱ��ģʽ 	  
			}else Pen_Point.Key_Reg=0; 											    
		}	   	 
		if(second!=timer.sec) //���Ӹı���
		{ 	
			LED0=!LED0;//����
			second=timer.sec;		
			//��ʾ�����ʱ��
			POINT_COLOR=GBLUE;
			LCD_ShowBigNum(0,100,timer.hour);//��ʾʱ
			LCD_ShowBigChar(60,100,':'); //"/"
			LCD_ShowBigNum(90,100,timer.min);//��ʾ��
			LCD_ShowBigChar(150,100,':'); //"/"
			LCD_ShowBigNum(180,100,timer.sec);//��ʾ��	 					   
			
			if(t%6==0)//�ȴ�������
			{		 
				if(TEMP_SEN_TYPE)temperate=Get_Temp();//�õ��ڲ��¶�
				else temperate=DS18B20_Get_Temp();//�õ�18b20�¶�
				if(temperate<0)//�¶�Ϊ������ʱ�򣬺�ɫ��ʾ
				{
					POINT_COLOR=RED;
					temperate=-temperate;//��Ϊ���¶�
				}
				else POINT_COLOR=BRRED;		   //����Ϊ�غ�ɫ������ʾ
				LCD_ShowBigNum(90,184,temperate/10);//XX
				LCD_ShowBigChar(150,184,'.'); //"."
				LCD_ShowBigChar(180,184,temperate%10);//��ʾС��
				LCD_ShowBigChar(210,184,'C'); //"/"
				if(t>0)t=0;			 
			}  
			if(timer.w_date!=tempdate)
			{
				Date_Refresh();//�����仯��,��������.  
				tempdate=timer.w_date;//�޸�tempdate����ֹ�ظ�����
			}
			t++;  
		}			 			 
	};
	while(!(RTC->CRL&(1<<3)));//�ȴ�RTC�Ĵ���ͬ��  
	RTC->CRH&=~(0X01);  	  //�ر����ж�
	while(!(RTC->CRL&(1<<5)));//�ȴ�RTC�Ĵ����������
	LCD_Clear(WHITE);//�����    
	POINT_COLOR=BLUE;
	BACK_COLOR=WHITE ;	
}



