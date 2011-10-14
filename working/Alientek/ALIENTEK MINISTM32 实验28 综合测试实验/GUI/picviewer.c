#include "picviewer.h"
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
//Mini STM32�����巶������28
//�ۺ� ʵ��
//����ԭ��@ALIENTEK
 
//ͼƬ�����
//֧��FAT32/16�ļ�ϵͳ�����.	  
void Pic_Viewer(void)
{		    
	u8 key,temp;
	FileInfoStruct *FileInfo;		 
	u16 pic_cnt=0;//��ǰĿ¼��ͼƬ�ļ��ĸ���
	u16 index=0;  //��ǰѡ����ļ����	
	u8 enout=0; 
	u8 keycnt=0; 
				    
	u16 tcnt=0;	
	u8 pause=0;//��ͣ����־	    
	LCD_Clear(WHITE);
	//�Ӹ�Ŀ¼��ʼ���
	if(FAT32_Enable)Cur_Dir_Cluster=FirstDirClust;//��Ŀ¼�غ�
 	else Cur_Dir_Cluster=0;						   
	Fat_Dir_Cluster=Cur_Dir_Cluster;//��Ŀ¼���ǵ�ǰĿ¼    			   	  
	while(1)
	{
BROS:	 							  			 
		Load_Gui_Bro("ѡ��");//��ʾGUI����	
		POINT_COLOR=WHITE;	 
		Show_Str(88,2,"�������",16,0x01);//����ģʽ,���Զ�����
		index=File_Browse(T_JPG|T_JPEG|T_BMP);//����ļ���    
		if(!index)return;//�������,�˳��˹���!!!!!

		pic_cnt=0;	 
		Get_File_Info(Cur_Dir_Cluster,FileInfo,T_JPG|T_JPG|T_BMP,&pic_cnt);//��ȡ��ǰ�ļ��������Ŀ���ļ����� 
		Pen_Point.Key_Sta=Key_Up;         //�ͷ���ʾ�ڼ�Ĵ��� 
		FileInfo=&F_Info[0];//�����ݴ�ռ�.
		while(1)
		{
			Get_File_Info(Cur_Dir_Cluster,FileInfo,T_JPG|T_JPG|T_BMP,&index);//�õ�����ͼƬ����Ϣ	 
			LCD_Clear(WHITE);//����,������һ��ͼƬ��ʱ��,һ������   
			AI_LoadPicFile(FileInfo,0,0,240,320);//��ʾͼƬ	   
			POINT_COLOR=RED;
			Show_Str(0,0,FileInfo->F_Name,16,0x03);//�Զ�����,����
			Pen_Point.Key_Sta=Key_Up;//�ͷ���ʾ�ڼ�Ĵ��� 
			key=0;enout=0;
			tcnt=0; 	 
			while(1)
			{
	 			temp=AI_Read_TP(30);	    
				if(temp)//��������,����֧�ֵ����� 
				{	  		  
					if(enout&&Is_In_Area(200,300,40,20))
					{
						goto BROS;
					};//���� 

					key=Touch_To_Num(7);	
 				 	if(key==KEY_NEXT||key==KEY_PREV)//����/���°���ʱ��,Ҫ��ȷ���Ƿ��ɿ���
					{
						if(Pen_Point.Key_Reg&0x80)key=0;//û�����ɿ�	 
					}		 											    
					if((key==KEY_FUNC)&&(keycnt==0)&&((Pen_Point.Key_Reg&0x80)==0))pause=!pause;//���ΰ���,����ͣȡ��	 
					Pen_Point.Key_Reg|=0x80;//��ǰ���
				}else 
				{
					Pen_Point.Key_Reg&=~0x80;//����ɿ�
					keycnt=0;
				}   	 			  		
				if(key)
				{								 
 					switch(key)
					{
						case KEY_PREV:	  
							enout=0;
							if(index>1)index--;
							else index=pic_cnt;//�����һ��(ѭ��) 
							break;	
						case KEY_NEXT:
							enout=0; 
		   					index++;
							if(index>pic_cnt)index=1;//��ʾ��һ��,ѭ��
							break;
						case KEY_FUNC://�̰���ͣ�������˳�
							keycnt++;
							tcnt=0;//���ʱ�������	    
							if(keycnt>=20)
							{ 				 
								enout=1;//ʹ���˳�
								keycnt=0;			  
							 	POINT_COLOR=DARKBLUE;//����ɫ	 
								LCD_DrawLine(0,297,239,297);
							 	LCD_Fill(0,298,239,308,LIGHTBLUE);//���ײ���ɫ(ǳ��ɫ)	
							 	LCD_Fill(0,308,239,319,GRAYBLUE); //���ײ���ɫ(����ɫ)	  	   
								POINT_COLOR=WHITE;//��ɫ����
								Show_Str(203,300,"����",16,0x01);    //����ģʽ,���Զ���  
							} 	
					}
					if(key!=KEY_FUNC)break;//���·�ҳ		  
					key=0;//�����ֵ		
					delay_ms(100); 
				}   
				delay_ms(1);  
				if(tcnt>=3000 && pause==0)//�ӳ�ʱ�䵽��
				{	 
					tcnt=0;
   					index++;
					if(index>pic_cnt)index=1;//��ʾ��һ��,ѭ�� 
					break; 
				}else if(tcnt%200==0)
				{
					if(pause==0)LED0=!LED0;//��˸,֤�����Զ�����
					else LED0=0;//����,֤������ͣ
				}
				tcnt++;//ʱ���ۼ�			 
			}
		}
	}			 
}




