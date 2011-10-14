#include "ebookplay.h"
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


void Read_Book(FileInfoStruct *FileName);						 
//�ı������(.txt/.c/.h/.lrc)	  
//֧��FAT32/16�ļ�ϵͳ�����.  
void Ebook_Play(void)
{				 
	FileInfoStruct *FileInfo;		 			 
	u16 index=0;  //��ǰѡ����ļ����	   
	//�Ӹ�Ŀ¼��ʼ���
	if(FAT32_Enable)Cur_Dir_Cluster=FirstDirClust;//��Ŀ¼�غ�
 	else Cur_Dir_Cluster=0;						   
	Fat_Dir_Cluster=Cur_Dir_Cluster;//��Ŀ¼���ǵ�ǰĿ¼
	while(1)
	{	  
		POINT_COLOR=RED;	   
		Load_Gui_Bro("ѡ��");//��ʾGUI����
		Show_Str(88,2,"����ͼ��",16,0x01);//����ģʽ,���Զ�����
			
		index=File_Browse(T_LRC|T_TXT|T_C|T_H);//����ļ���    
		if(!index)return;//�������,�˳��˹���!!!!!	
				 												   
		Pen_Point.Key_Sta=Key_Up;         //�ͷ���ʾ�ڼ�Ĵ��� 	   
		FileInfo=&F_Info[0];//�����ݴ�ռ�.	 
		Get_File_Info(Cur_Dir_Cluster,FileInfo,T_LRC|T_TXT|T_C|T_H,&index);//�õ�����ͼƬ����Ϣ	  
		Read_Book(FileInfo);//��ȡTXT�ļ�   	  
	}
}
#define FSIZE 16
//�ı��ļ���ȡ����	 
//ʵ�ֹ���:���ļ� FileName ��.��Һ������ʾ���� 
//��txt/lrc�ļ�  							 
void Read_Book(FileInfoStruct *FileName)
{     							   
    u16 pointpos=0;   //ָ��λ��   
    unsigned long offlenth=0;//�ļ���ȡ�Ĵ�С 		   
    u16 x=0,y=0;
    u8 *p=0;            //ָ��txt�ļ�
    u8 temp[2]={0,0};   //������ʹ�� 
    u8 bHz=0;           //�ַ���������  	  
 				  
	u8 key,keyx,tcnt=0;
	u8 enout=0;//�ܷ��˳��ı�־ 
	u8 keycnt=0;

  
 	if((FileName->F_Type&(T_LRC|T_TXT|T_C|T_H))==0)return;//����TXT���͵��ļ�
	F_Open(FileName);//���ļ�	  

	LCD_Clear(WHITE);//����
	POINT_COLOR=DARKBLUE;//����ɫ����    
	while(1)
    {  	   	    	
		F_Read(FileName,jpg_buffer);//��ȡ����	 			 
	 	p=jpg_buffer; //ָ��txt�����׵�ַ   
		do
        {   				    
	        while(*p!=0)//����δ����
	        { 						  
		        if(!bHz)
		        {
			        if(*p>0x80)bHz=1;//�����ַ� 
			        else             //ASCII�ַ�
			        {      
                        if(x>(240-FSIZE/2)){y+=FSIZE;x=0;}//һ������,����  
				        if(y>(320-FSIZE))break;//Խ�緵��      
				        if(*p==13)//���з���
				        {         
				            y+=FSIZE;
							x=0;
				            p++;pointpos++;//���� 
				        }  
				        else LCD_ShowChar(x,y,*p,FSIZE,1); 
						p++;pointpos++; 
				        x+=FSIZE/2; //�ַ�,Ϊȫ�ֵ�һ�� 
			        }
		        }else
		        {     
		            bHz=0;//������ֱ�־ 
		            if(x>(240-FSIZE)){y+=FSIZE;x=0;}//����
			        if(y>(320-FSIZE))break;//Խ�緵��  
			        //�ٽ紦��   
			        if(pointpos==511)//������һ���������һ���ֽ�,�˳�        
			        {
			            temp[0]=*p; //��¼�����ֽ�                
			            break;      //�����������Ѿ�����
			        }
			        if(temp[0])//��һ��û����ʾ��
			        {                         
			            temp[1]=*p;//ȡ�ڶ������ĵ�һ���ֽ�
			            
						Show_Font(x,y,temp,16,1); //��ʾ�������,������ʾ	
			            p++;pointpos++;//ƫ��һ���ֽ�
			            temp[0]=0;     //������
			        }else Show_Font(x,y,p,16,1); //��ʾ�������,������ʾ			   
			        
			        p+=2;pointpos+=2;     //ָ��ƫ�� 
			        x+=FSIZE; 			  //��һ������ƫ��
		        }
		        if(pointpos>511)break;
	        }  
            if(pointpos>=511)break;//һ��������ȡ���,ֱ������,����һ������,��ֹͣ��		   
 			enout=0; //�˳�ʹ�ܱ�־��ʹ��
			key=0;		 
			while(1) //�Ƿ���ʾ��һ������
            {	    
				keyx=AI_Read_TP(30);	    
				if(keyx)//��������,����֧�ֵ����� 
				{	  		  
					if(enout&&Is_In_Area(200,300,40,20))return;//���� 

					key=Touch_To_Num(7);	
					if(key==KEY_FUNC)//�����м�
					{
						if(keyx!=2)key=0;//���ǵ㴥
					}else if(key==KEY_NEXT)
					{
						if(Pen_Point.Key_Reg&0x80)key=0;//û�����ɿ�	 
					}												    
					Pen_Point.Key_Reg|=0x80;//��ǰ���
				}else Pen_Point.Key_Reg&=~0x80;//����ɿ�
					   
				if(key==KEY_NEXT)//���·�ҳ
                {     			   
 					keycnt=0;	  
					if(enout){if(Is_In_Area(200,300,239,319))return;}//�˳��ļ��Ķ�		  	   
 					if(y>(320-FSIZE)){x=0;y=0;}//�ǽ�β����,��Ļ����,����       
                    if(offlenth+pointpos+1>=FileName->F_Size)goto QUIT;//�ļ���ȡ����(���Ƚ���) 	    
                    else 
					{
						LCD_Clear(WHITE);//����
						break;
			 		} 
                }
				if(key==KEY_FUNC)//�����м�������							   
				{									   
 					keycnt++;
					if(keycnt>=200)
					{ 
QUIT:
						keycnt=0;
						enout=1;//ʹ���˳�		    
					 	POINT_COLOR=DARKBLUE;//����ɫ	 
						LCD_DrawLine(0,297,239,297);
					 	LCD_Fill(0,298,239,308,LIGHTBLUE);//���ײ���ɫ(ǳ��ɫ)	
					 	LCD_Fill(0,308,239,319,GRAYBLUE); //���ײ���ɫ(����ɫ)	  	   
						POINT_COLOR=WHITE;//��ɫ����
						Show_Str(203,300,"����",16,0x01);    //����ģʽ,���Զ��� 
						POINT_COLOR=DARKBLUE;//����ɫ
					}  
				}
				key=0;//����˴ΰ���	   			  					  
                delay_ms(10); 
				tcnt++;
				if(tcnt>=20)
				{
					LED0=!LED0;
					tcnt=0;
				}
            }   
        }while(pointpos<511);  
        offlenth+=pointpos+1;//�ļ������ۼ�     
		pointpos=0;//���ָ������� 		    		  	 
    } 
}  






