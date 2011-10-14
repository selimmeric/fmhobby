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
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//�ۺ�ʵ�� ��������		 					    
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/6/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////	 


//ϵͳ���˵�
const u8 *Menu[9]=
{
"����ͼ��",
"�������",
"ƴͼ��Ϸ",
"��������",
"ϵͳʱ��",
"��껭��",
"USB���� ",
"����ң��",
"���ߴ���",	 
};
//////////////////////////GUI ��ť///////////////////////////////
void GUI_DrawLine(u16 x,u16 y,u16 xlen,u16 ylen,u16 color)
{
	u16 tempcolor=POINT_COLOR;
	POINT_COLOR=color;
	LCD_DrawLine(x,y,x+xlen,y+ylen);
	POINT_COLOR=tempcolor;	
}
//��ָ��λ����ʾָ����ɫ�İ�ť
//(x,y):��ť��ʼ����
//xlen,ylen:x,y����ĳ���
//str:��ť��Ҫ��ʾ���ַ�
//color:�ַ���ɫ
void GUI_Draw_Button(u16 x,u16 y,u16 xlen,u16 ylen,u8 *str,u16 color)
{	
	u16 tback_color,tpoint_color;
	//////////////////����ť����//////////////////////   
	LCD_Fill(x,y,x+xlen,y+ylen,LGRAY);//��ʵ��
	GUI_DrawLine(x,y,xlen,0,WHITE);   //����߿�
	GUI_DrawLine(x,y,0,ylen,WHITE);   //����߿�   	   
	GUI_DrawLine(x+1,y+1,xlen-1,0,0XB5B6);//���ڱ߿�
	GUI_DrawLine(x+1,y+1,0,ylen-1,0XB5B6);//���ڱ߿�
	GUI_DrawLine(x,y+ylen,xlen,0,0X8431); //����߿�
	GUI_DrawLine(x+xlen,y,0,ylen,0X8431); //����߿�   	   
	GUI_DrawLine(x+1,y+ylen-1,xlen-2,0,0XA535);//���ڱ߿�
	GUI_DrawLine(x+xlen-1,y,0,ylen-1,0XA535);  //���ڱ߿�
	//////////////////��ʾ��ť�������Ϣ///////////////  
	if(ylen<16)return;//������	 
	tback_color=BACK_COLOR;
	tpoint_color=POINT_COLOR;
	BACK_COLOR=LGRAY;
	POINT_COLOR=color; 
	Show_Str_Mid(x,y+(ylen-16)/2,str,16,xlen);//ֻ��16����
	BACK_COLOR=tback_color;	 //�ָ�֮ǰ����ɫ
	POINT_COLOR=tpoint_color; 
}
/////////////////////////����ͼ�μ���/////////////////////////////

//ѡ��˵�
void Select_Menu(u8 M_Type,u8 Cnt,u8 En)
{
	u8 tx;
	u16 ty;
	u8 t;	
	ty=10; 
	if(M_Type==0)//���˵�
	{	  
		tx=80*(Cnt%3)+10;//10,90,170			
		
		if(Cnt>5)ty=216;
		else if(Cnt>2)ty=113;	
		//��ʾ��򲿷� 
		if(En)POINT_COLOR=BLUE;
		else POINT_COLOR=WHITE;
		for(t=0;t<4;t++)
		{
			LCD_DrawRectangle(tx-7+t,ty-7+t,tx+66-t,ty+67-t); 
		} 
		//��ʾ���壬����
		if(En)//ѡ�е�ǰ�Ĳ˵�
		{	  
			POINT_COLOR=WHITE;//��ɫ����
			BACK_COLOR=LBBLUE;  //��ɫ����      
		}else
		{
			POINT_COLOR=DARKBLUE;//����ɫ����
			BACK_COLOR=WHITE; //��ɫ����   
		}	 												  
		Show_Str(tx-3,ty+74,(u8*)Menu[Cnt],16,0);	
	}
}
//����������
void Load_Sys_ICO(void)
{		  
	u8 t,tx;
	u16 ty;	   
	FileInfoStruct temp; 
	POINT_COLOR=RED;//��ɫ����				   
    LCD_Clear(WHITE);//���� 
	temp.F_Type=T_BMP;//�ļ��������� 
	tx=10;ty=10;
	for(t=0;t<9;t++)
	{	   
		temp.F_StartCluster=sys_ico[t];								    
		tx=80*(t%3)+10;//10,90,170	    
		AI_LoadPicFile(&temp,tx,ty,tx+59,ty+60);
		Select_Menu(0,t,0);//��������    		  
		if(t>=5)ty=216;
		else if(t>=2)ty=113;  
	}
}
//������Ϸ����
//size:3,3*3ͼƬ
//     4,4*4ͼƬ
//     5,5*5ͼƬ
//#define FRAME1 0XBC40 //��ɫ
void Load_Game_ICO(u8 size)
{
	u8 i;
	LCD_Clear(LGRAY);//����
	for(i=0;i<4;i++)//����߿�
	{		
		POINT_COLOR=FRAME1;
		LCD_DrawRectangle(i,i,239-i,239-i);
	}
	if(size==3)
	{
		for(i=0;i<4;i++)//��С�߿�
		{		
			POINT_COLOR=FRAME1;
			LCD_DrawRectangle(154+i,235+i,239-i,316-i);
		}									 
	    LCD_Fill(158,235,235,239,WHITE);//�Ѷ����ȥ��	
		POINT_COLOR=RED;
		LCD_DrawRectangle(158,235,235,312);//���յķ������Ȳ���ͼƬ
		LCD_DrawRectangle(4,4,235,235); //�����ĺ��

		LCD_DrawLine(81,4,81,235);  //���ڲ����
		LCD_DrawLine(158,4,158,235);//���ڲ����
		LCD_DrawLine(4,81,235,81);  //���ڲ����
		LCD_DrawLine(4,158,235,158);//���ڲ����
	}else if(size==4)
	{
		for(i=0;i<4;i++)//��С�߿�
		{		
			POINT_COLOR=FRAME1;
			LCD_DrawRectangle(174+i,236+i,239-i,298-i);
		}									 
	    LCD_Fill(178,236,236,240,WHITE);//�Ѷ����ȥ��	
		POINT_COLOR=RED;
		LCD_DrawRectangle(178,236,236,294);//���յķ��򣬵�ʮ����
		LCD_DrawRectangle(4,4,236,236);    //�����ĺ��

		for(i=0;i<size;i++)LCD_DrawLine(4,4+58*i,236,4+58*i);  //���ڲ���������
		for(i=0;i<size;i++)LCD_DrawLine(4+58*i,4,4+58*i,236);  //���ڲ���������	   										  
	}else if(size==5)
	{
		for(i=0;i<4;i++)//��С�߿�
		{		
			POINT_COLOR=FRAME1;
			LCD_DrawRectangle(184+i,234+i,239-i,284-i);
		}	
		LCD_DrawLine(4,235,236,235);  //�����������
		LCD_DrawLine(235,4,235,281);  //������������
										 
	    LCD_Fill(188,234,234,239,WHITE);//�Ѷ����ȥ��	
		POINT_COLOR=RED;
		LCD_DrawRectangle(188,234,234,280);//���յķ��򣬵�ʮ����
		LCD_DrawRectangle(4,4,234,234);    //�����ĺ��

		for(i=0;i<size;i++)LCD_DrawLine(4,4+46*i,234,4+46*i);  //���ڲ���������
		for(i=0;i<size;i++)LCD_DrawLine(4+46*i,4,4+46*i,234);  //���ڲ���������
	}	  		  								    	  		  
	LCD_DrawRectangle(2,241,79,318);//����Ԥ��ͼƬ
	
	BACK_COLOR=WHITE;
	POINT_COLOR=BLUE;//������ɫ
	Show_Str(81,242,"�ȼ�:",16,0x01);
	POINT_COLOR=RED;//������ɫ
	if(size==3)Show_Str(121,242,"��",16,0x01);
	else if(size==4)Show_Str(121,242,"һ��",16,0x01);
	else Show_Str(121,242,"����",16,0x01);
	POINT_COLOR=BLUE;//������ɫ
	Show_Str(81,258,"����:",16,0x01);
	Show_Str(81,274,"��ʱ:",16,0x01);   
	GUI_Draw_Button(98,298,37,20,"�˳�",BLACK);		  					     
} 		 	 
//�����������
//���ý��� 
//str:"ѡ��/ȷ��"
void Load_Gui_Bro(u8 * str)
{			 	   					  			   
    LCD_Clear(WHITE);//����
	//���� 						   
 	LCD_Fill(0,0,239,9,LIGHTBLUE);  //���ײ���ɫ(ǳ��ɫ)	
 	LCD_Fill(0,10,239,20,GRAYBLUE); //���ײ���ɫ(����ɫ)
	//�в�
	POINT_COLOR=DARKBLUE;//����ɫ  
	LCD_DrawLine(0,21,239,21);
	LCD_Fill(0,22,239,41,LIGHTBLUE);//����в�����ɫ(ǳ����ɫ)
	LCD_DrawLine(0,41,239,41);	    
	//�ײ�
	LCD_DrawLine(0,298,239,298);
 	LCD_Fill(0,299,239,308,LIGHTBLUE);//���ײ���ɫ(ǳ��ɫ)	
 	LCD_Fill(0,308,239,319,GRAYBLUE);//���ײ���ɫ(����ɫ)

	POINT_COLOR=DARKBLUE;
	Show_Str(0,23,"��ѡ��:",16,0x01); //����ģʽ,���Զ�����
	POINT_COLOR=WHITE;
	Show_Str(5,300,str,16,0x01);   //����ģʽ,���Զ�����
	Show_Str(203,300,"����",16,0x01); //����ģʽ,���Զ�����  
}
//������ʾ����
//��PS2/���ػ�ͼʱ�õ�
//���ý��� 
//str:"ѡ��/ȷ��"
void Load_Gui_Show(u8 *lbtstr,u8 *mbtstr,u8 *rbtstr)
{			 	   					  			   
    LCD_Clear(WHITE);  //����  
 	POINT_COLOR=DARKBLUE;//����ɫ	 
	LCD_DrawLine(0,297,239,297);
 	LCD_Fill(0,298,239,308,LIGHTBLUE);//���ײ���ɫ(ǳ��ɫ)	
 	LCD_Fill(0,308,239,319,GRAYBLUE); //���ײ���ɫ(����ɫ)	  	   
	POINT_COLOR=WHITE;//��ɫ����
	Show_Str(5,300,lbtstr,16,0x01);    //����ģʽ,���Զ�����
	Show_Str(104,300,mbtstr,16,0x01);  //����ģʽ,���Զ�����		  
	Show_Str(203,300,rbtstr,16,0x01);  //����ģʽ,���Զ�����  
}

////////////////////////////////�봥����������صĺ���////////////////////////////
//�Ѵ������õ�������ת��Ϊ����(��ťֵ)
//M_Type:��ťģʽ;
//0,ϵͳ�˵�	
//1,�ļ����ģʽ 	
//2,ʱ��ģʽ��,�����Ļ�ȡ,����֧��! 
//3,���ֲ���ģʽ��
//4,����ѡ��ģʽ��
//5,������ģʽ��
//6,�����жϳ��򰴼�

//7,TXT���/JPEG��� ʱ��������			  
//����ֵ:��Ӧ�İ���ֵ(1~n)
//����0,���ʾ�˴ΰ�����Ч  
u8 Touch_To_Num(u8 M_Type)
{
	u8 xtemp;
	u16 t,ytemp;
	switch(M_Type)
	{
		case 0://ϵͳ�˵���										  
			ytemp=10;
			for(t=0;t<9;t++)
			{
				xtemp=80*(t%3)+10;
				if(t>5)ytemp=216;
				else if(t>2)ytemp=113;
				if(Is_In_Area(xtemp,ytemp,59,90))return t+1;//���ذ���ֵ 
			}			  
		case 1://1,�ļ����ģʽ	   	  
			for(t=0;t<8;t++)
			{	  
				if(Is_In_Area(0,42+t*32,240,32))return t+1;
			}
			if(Is_In_Area(0,300,40,20))return 9;//ѡ��ť������
			if(Is_In_Area(200,300,40,20))return 10;//���ذ�ť������
			return 0;//������Ч	    
//			if(xtemp)return xtemp&0xf0;//����������!! 
			//break; 
		case 2://4,����ѡ��ģʽ�� 
			for(t=0;t<8;t++)
			{	  
				if(Is_In_Area(0,42+t*32,240,32))return t+1;
			}
			if(Is_In_Area(0,300,40,20))return 9;//ѡ��ť������
			if(Is_In_Area(200,300,40,20))return 10;//���ذ�ť������
			return 0;//������Ч	 
			//break;
		case 3:
			break;
		case 4:						    
			break;
		case 5://5,������ģʽ�� 
			break; 
		case 6://6,�����жϳ��򰴼�	  
			break;
		case 7://JPEG/BMP/TXT�������ģʽ  
			if(Is_In_Area(0,0,239,60))return KEY_PREV;        //����
			else if(Is_In_Area(0,60,239,200))return KEY_FUNC; //����		    
			else if(Is_In_Area(0,260,239,60))return KEY_NEXT; //���·� 	   
			return 0;//������Ч	    			
			//break; 	  	    		 
	}
	return 0;
}
//�����������
u8 KEY_Move_Check(void)
{
#define MOVE_PIXES 30 //��������						    
	if(Pen_Point.X>Pen_Point.X0)//����
	{
		if(T_abs(Pen_Point.X0,Pen_Point.X)>MOVE_PIXES&&T_abs(Pen_Point.Y0,Pen_Point.Y)<MOVE_PIXES)return MOVE_RIGHT;
	}else  //����
	{
		if(T_abs(Pen_Point.X0,Pen_Point.X)>MOVE_PIXES&&T_abs(Pen_Point.Y0,Pen_Point.Y)<MOVE_PIXES)return MOVE_LEFT;
	}
	if(Pen_Point.Y>Pen_Point.Y0)//����
	{
		if(T_abs(Pen_Point.Y0,Pen_Point.Y)>MOVE_PIXES&&T_abs(Pen_Point.X0,Pen_Point.X)<MOVE_PIXES)return MOVE_DOWN;
	}else			   //����
	{
		if(T_abs(Pen_Point.Y0,Pen_Point.Y)>MOVE_PIXES&&T_abs(Pen_Point.X0,Pen_Point.X)<MOVE_PIXES)return MOVE_UP;
	}	 
	return 0;							 
}
//////////////////////////////���ļ������صĺ���//////////////////////////////
//����ѡ����Ⱥ���	    
//list  ��������Ŀָ��
//title ������
//func  ��ѡ��/ȷ��/������
//len   ��������Ŀ�ĸ���(1~8)
//sel   ����ʼѡ�е���Ŀ(0~7)
//����ֵ��0~7,ѡ�е���Ŀ��8��û��ѡ���κ���Ŀ��
u8 Load_Fun_Sel(const u8 * *list,const u8 *title,u8 *func,u8 len,u8 sel)
{	
	u8 t,key;
	Load_Gui_Bro(func);	   
	//��ʾ����
	POINT_COLOR=WHITE;
	Show_Str_Mid(0,2,(u8*)title,16,240); //����ģʽ,���Զ�����  
	POINT_COLOR=DARKBLUE;
	Show_Str(0,23,"��ѡ��:",16,0x01);    //����ģʽ,���Զ�����	 
	//������Ŀ��Ϣ
	for(t=0;t<len;t++)
	{	 											    
		if(t==sel)
		{
			LCD_Fill(0,45+sel*32,239,37+(sel+1)*32,LBBLUE );//��ѡ�е���Ŀ�������ɫ 
			POINT_COLOR=WHITE;
		}else POINT_COLOR=DARKBLUE;	  
		Show_Str(0,50+t*32,(u8*)list[t],16,0x01); //����ģʽ,���Զ�����       
	} 
 	key=0;
	Pen_Point.Key_Sta=Key_Up;//�ͷ���ʾ�ڼ�Ĵ���	    
	while(1)//�ȴ�����
	{	 
		key=AI_Read_TP(30);
		if(key)Pen_Point.Key_Reg|=0X80;//��ǰ����Ѿ������� 	   
		if(key==0&&(Pen_Point.Key_Reg&0X80))//�����ɿ���
		{
			Pen_Point.Key_Reg=0;			    
			key=Touch_To_Num(2);
			if(key)//�а�������
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
					case 9://ȷ�ϼ�����
					key-=1;
					if(sel!=key&&key<len)//ѡ��һ��
					{
						POINT_COLOR=DARKBLUE;//������ɫѡ��		 
						LCD_Fill(0,45+sel*32,239,37+(sel+1)*32,WHITE);//����ϴ�ѡ���
						Show_Str(0,50+sel*32,(u8*)list[sel],16,0x01);//������ʾ�ϴε�����
			        	sel=key;//ѡ�����ѡ���
						LCD_Fill(0,45+sel*32,239,37+(sel+1)*32,LBBLUE);//����ɫ
						POINT_COLOR=WHITE;//������ɫѡ��		 
						Show_Str(0,50+sel*32,(u8*)list[sel],16,0x01);//��ʾ���� 
					}else if(sel==key||key==8)//ѡ��ť,����˫��
					{	 	
						return sel;//����ѡ�����Ŀ��
					}
						break;  		  	  
					case 10:return 8;//ѡ����"����"��ť,����һ��������ֵ  
				}  
				key=0;//ȡ������ֵ
			}											    
		}	  
		delay_ms(1);	
	}  
}
//��ʾ�ļ�
//fcnt:Ҫ��ʾ�ļ��Ķ���
//sel :ѡ�еڼ���1~8 
//����ֵ:Ҫ�򿪵��ļ����1~8(�����ļ���,��FileInfo�����)	    
//����9,����ֱ���˻���һ��Ŀ¼  		    
//��ʾ�ļ�
//check ok 09/04/19
u8 Show_File(u8 fcnt,u8 sel)
{
	u16 t;	
	u8 key; 					   
	FileInfoStruct temp;
	temp.F_Type=T_BMP;//�ļ���������

	sel-=1;										 
	LCD_Fill(0,42,239,297,WHITE);//����ɫ,����ϴε���ʾ
	for(t=0;t<fcnt;t++)
	{
		if(F_Info[t].F_Type==T_FILE)temp.F_StartCluster=file_ico[0];//�ļ���ͼ��
		else 
		{
			if(F_Info[t].F_Type&(T_JPG|T_JPEG|T_BMP))temp.F_StartCluster=file_ico[2];//ͼƬ��ͼ��
			else if(F_Info[t].F_Type&(T_MP3|T_OGG|T_WMA|T_WAV|T_FLAC|T_MID))temp.F_StartCluster=file_ico[1];//������ͼ��
			else if(F_Info[t].F_Type&(T_LRC|T_TXT|T_C|T_H))temp.F_StartCluster=file_ico[3];//������ͼ��
		}				   
		AI_LoadPicFile(&temp,0,42+t*32,32,42+t*32+32);//����ͼ��  
		if(t==sel)
		{
			LCD_Fill(32,45,239,69,LBBLUE);//�����ɫ BROWN	  
			POINT_COLOR=WHITE;
		}else POINT_COLOR=DARKBLUE;
		Show_Str(32,50+t*32,F_Info[t].F_Name,16,0x03); //����ģʽ,���Զ����� 
		//printf("N[%d]%s\n",t,F_Info[t].F_Name);      
	}
	key=0;
	Pen_Point.Key_Sta=Key_Up;//�ͷ���ʾ�ڼ�Ĵ���
	while(1)//�ȴ���������
	{
		key=AI_Read_TP(50);//�ʺϻ������	    
		if(key)
		{
			if((Pen_Point.Key_Reg&0x03)==1)//��һ���ǻ�������
			{
				t=KEY_Move_Check();//��黬������
				if(t)
				{
					key=t;  
					goto KEY_D;
				}
			}
			Pen_Point.Key_Reg|=0X80;  //��ǰ����Ѿ�����
		}	   
		if(key==0&&(Pen_Point.Key_Reg&0X80))//�����ɿ���
		{
			key=Touch_To_Num(1);
KEY_D:	    
			Pen_Point.Key_Reg=0;		  	
			if(key)//�а�������
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
						if(sel!=key&&key<fcnt)//ѡ��һ��
						{
							POINT_COLOR=DARKBLUE;//������ɫѡ��		 
							LCD_Fill(32,45+sel*32,239,37+(sel+1)*32,WHITE);//����ϴ�ѡ���
							Show_Str(32,50+sel*32,F_Info[sel].F_Name,16,0x03);//������ʾ�ϴε�����
				        	sel=key;//ѡ�����ѡ���
							LCD_Fill(32,45+sel*32,239,37+(sel+1)*32,LBBLUE);//����ϴ�ѡ���
							POINT_COLOR=WHITE;//������ɫѡ��		 
							Show_Str(32,50+sel*32,F_Info[sel].F_Name,16,0x03);//��ʾ���� 
						}else if(sel==key)return sel+1;//�ٴ�ѡ��,˫����.
						break;
					case 9: //ѡ����"ѡ��"��ť
						return sel+1; 	  
					case 10://ѡ����"����"��ť 
						return 9;
					case MOVE_UP:  //��ҳ���� 
					case MOVE_LEFT: 
				    case MOVE_DOWN:  
					case MOVE_RIGHT: 
						return key;
				}
				key=0;//�Ѱ���ֵȡ����				  
				Pen_Point.Key_Sta=Key_Up;//�ͷ���ʾ�ڼ�Ĵ���
			}  
		}
		delay_ms(1);
		t++;
		if(t%500==0)LED0=!LED0;	
	}
}
//����ļ���������ļ�
//file_type:Ҫ������ļ�����
//����Ҫ�򿪵�Ŀ���ļ���� 1~n(����Ŀ���ļ������)
//�������ļ���!!!
//�������0������˳����
u16 File_Browse(u32 file_type)
{			 
	FileInfoStruct *FileInfo;
	u16 folder_cnt;//�ļ��еĸ���
	u16 des_cnt;   //Ŀ���ļ��ĸ���		    
	u8 fcnt;	   //��ʾ���ļ���Ŀ
	u16 temp; 					     									  

	u16 total_file;//���ļ���Ŀ
	u16 pos_cnt;//��ǰ�ļ��� 1~folder_cnt+des_cnt
	u8 key;	  			  
							   			  
Reset:				    
	POINT_COLOR=BLUE;  	   		   //��ɫ��ͷʹ��
	folder_cnt=0;des_cnt=0;fcnt=0;key=0;pos_cnt=0;	 	
	Get_File_Info(Cur_Dir_Cluster,FileInfo,T_FILE,&folder_cnt);//�õ���ǰĿ¼���ļ��еĸ���	    
	Get_File_Info(Cur_Dir_Cluster,FileInfo,file_type,&des_cnt);//�õ���ǰĿ¼��Ŀ���ļ��ĸ���	 
									   
	total_file=des_cnt+folder_cnt;//�ܵ��ļ���Ŀ=�ļ�����Ŀ+Ŀ���ļ���Ŀ
	while(1)
	{
		if(pos_cnt<total_file)//�ļ����,δ�����ļ�ĩβ
		{	   
			FileInfo=&F_Info[fcnt];
			if(pos_cnt<folder_cnt)//���ļ���
			{
			    temp=pos_cnt+1;			    
				Get_File_Info(Cur_Dir_Cluster,FileInfo,T_FILE,&temp);
			}else				   //��Ŀ���ļ�
			{
				temp=pos_cnt-folder_cnt+1; 
				Get_File_Info(Cur_Dir_Cluster,FileInfo,file_type,&temp);
			}
			fcnt++;   //�Ѵ���ļ���Ŀ����
			pos_cnt++;//�ļ�λ�ñ�־����
		}	  
		if(fcnt==8||pos_cnt==total_file)//�����˸���,�����ļ����Ѿ�������
		{  
			key=Show_File(fcnt,1);//��ʾ�˴εõ����ļ�
			switch(key)
			{
				case 1://Ҫ��Ŀ���ļ�
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
					key-=1;
					if(key<fcnt)//��������Ч��Χ��
					{
						if(F_Info[key].F_Type==T_FILE)//Ҫ�򿪵��Ǹ��ļ���
						{
							Cur_Dir_Cluster=F_Info[key].F_StartCluster;//������ļ���		  
							goto Reset;
						}else    //Ҫ�򿪵���Ŀ���ļ�
						{	
							//LCD_Clear(WHITE);//����		   
							return pos_cnt+key-folder_cnt-fcnt+1;//����Ŀ���ļ����
						}
					}
					break;
				case 9:	//ѡ���˷��ذ�ť.�˳�����һ��Ŀ¼
					//����Ŀ¼�������,���·��ذ�ť,���˳����ģʽ	
					if((FAT32_Enable&&Cur_Dir_Cluster==FirstDirClust)||Cur_Dir_Cluster==0)return 0;//�˳����ģʽ	  					
					Cur_Dir_Cluster=Fat_Dir_Cluster;//�˵���Ŀ¼  			    	 
					goto Reset;	   	 
				case MOVE_UP:  //���Ϸ�ҳ
				case MOVE_LEFT:
					if(pos_cnt>8)pos_cnt-=(8+fcnt);//�ص���һҳ
					else pos_cnt=0;//��ͷ��ʼ��
					break;
				case MOVE_DOWN://���·�ҳ
		    	case MOVE_RIGHT:
					if(pos_cnt==total_file)pos_cnt-=fcnt;//������
					break;

			}	 				    
			fcnt=0;//�����ʾ�ļ�������
		}
	}		
}
//////////////////////////��LCD��ʾ��صĺ���//////////////////////////////////
//������
//x1,y1:�������
//x2,y2:�յ�����  
void LCD_DrawDLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		Draw_Big_Point(uRow,uCol);//���� 
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
u16 *Temp_Color_Buf=(u16*)iclip;//�����ݴ����iclip����,��JPEG���빲��
//��ȡ�Դ浽Temp_Color_Buf
//(x,y):��ʼ����
//xsize:x�����ϵĳ���
//ysize:y�����ϵĳ���
//�ܹ����������ݴ�СΪ:xsize*ysize
void Read_Temp_Color(u16 x,u16 y,u16 xsize,u16 ysize)
{
	u16 i,t;
	u16 tcnt=0;
	for(i=0;i<xsize;i++)
	{
		for(t=0;t<ysize;t++)
		{
			Temp_Color_Buf[tcnt]=LCD_ReadPoint(x+i,y+t);//��ȡ��ǰ�����ɫֵ
			tcnt++;
		}
	}
}
//����ȡ���Դ�Temp_Color_Buf������ʾ����
//(x,y):��ʼ����
//xsize:x�����ϵĳ���
//ysize:y�����ϵĳ���
//��д������ݴ�СΪ:xsize*ysize
void Reshow_Temp_Color(u16 x,u16 y,u16 xsize,u16 ysize)
{
	u16 i,t;
	u16 tcnt=0;
	for(i=0;i<xsize;i++)
	{
		for(t=0;t<ysize;t++)
		{
			POINT_COLOR=Temp_Color_Buf[tcnt];//��ȡ��ǰ�����ɫֵ
			LCD_DrawPoint(x+i,y+t);
			tcnt++;
		}
	}
}
//����ɫ�б�
//����ɫ������СΪ51*37
//�ֳ�5*7����ɫ��,�ܹ�35����ɫ
const u16 Color_Tab[3][7]=
{
{0XF800,0X7E00,0X001F,0X0000,0XFFFF,0X8000,0X0400},
{0XFFE0,0X07FF,0XF81F,0X2B12,0X8400,0XFFF0,0XFC10},
{0X0010,0X841F,0X041F,0XFC10,0X87F0,0XF810,0XFC00},
};
//����ɫ�б�
//����ɫ������СΪ65*29
//�ֳ�3*7����ɫ��,�ܹ�21����ɫ
//#define FRAME1 0XEF5B //����ɫ 	 
//#define FRAME2 0XDED7 //����ɫ	 
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
	for(i=0;i<8;i++)//��С�߿�
	{					    
		LCD_DrawLine(x+9*i+1,y,x+9*i+1,y+28);
	}
	for(t=0;t<3;t++)
	{
		for(i=0;i<7;i++)
		{
			LCD_Fill(x+2+i*9,y+2+t*9,x+2+i*9+6,y+2+t*9+6,Color_Tab[t][i]);//�Ѷ����ȥ��
		}
	}								    	  
}










