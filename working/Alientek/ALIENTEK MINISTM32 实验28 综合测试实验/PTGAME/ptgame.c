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

//ƴͼ��Ϸ����
//֧�� 3*3 4*4 5*5�����ȼ���Ҫ�������Ҳ�ǱȽ����׵�
//����ֻҪ�޸�ͼƬ���Cur_Table�Ĵ�С��PT_Size�Ĵ�С�Ϳ�����
//����Ҫע�ⲻҪ�����������͵ķ�Χ						    
//����ԭ��@SCUT
//2009/05/30 
//V1.2												  

//�洢��sram����� ͼƬ��ʼ��ַ����SD���ϣ� �б�
//u32 *Pic_Addr=(u32*)iclip;//ע�� 0����ʾԤ��ͼƬ�ĵ�ַ 1~n����ʾ1~n��ͼƬ�ĵ�ַ
u8 Cur_Table[26];//ͼƬ��Ŵ洢�б�	5*5ʱ��СΪ25+1=26

u8 Xscal;        //������
//���ĸ�������Ҫ���浽EEPROM����
u8 PT_Size=3;    //ƴͼ��Ϸ���Ѷȼ��� 3��4����ֵ

u8  PT_New=0; //�Ƿ��Ѿ����棿1��û�����棬������¡�0���Ѿ����棬����δ��ɣ����Բ����¡�
u16 PT_Step=0;//ƴͼ���ò���
u16 PT_Time=0;//ƴͼ����ʱ��	
								   
const u8 *PTGmenu_F1[1]={"ƴͼ��Ϸ"};//��1����Ŀ¼Ŀ¼
const u8 *PTGmenu_S1[4]={"����","����Ϸ","�������","��Ϸ����"};//��1����Ŀ¼
const u8 *PTGmenu_S2[3]={"��","һ��","����"}; //��2����Ŀ¼ ����	 

#define PT_BASIC 1 //������Ϣ
#define PT_TABLE 2 //ͼ����Ϣ	 

//���浱ǰ��Ϸ��Ϣ
//��ַ���䣺412~443
#define PTGAME_24C0X_BASE_ADDR 200
void Save_PTG_Info(u8 type)
{			 
	u8 i;
	if(type==PT_BASIC)//���������Ϣ
	{								 
		AT24CXX_WriteOneByte(PTGAME_24C0X_BASE_ADDR,PT_Size);//����ͼƬ��С
		AT24CXX_WriteOneByte(PTGAME_24C0X_BASE_ADDR+1,PT_New); //�����Ƿ��Ѿ�����
		//���浱ǰ���ò���
		AT24CXX_WriteLenByte(PTGAME_24C0X_BASE_ADDR+2,PT_Step,2);	
		//���浱ǰ����ʱ��
		AT24CXX_WriteLenByte(PTGAME_24C0X_BASE_ADDR+4,PT_Time,2);
	}else if(type==PT_TABLE)//����ͼ����Ϣ
	{
		for(i=0;i<26;i++)AT24CXX_WriteOneByte(PTGAME_24C0X_BASE_ADDR+6+i,Cur_Table[i]);//���浱ǰͼ����Ϣ	  
	}
}
//��ȡ��ǰ��Ϸ��Ϣ
//��ַ���䣺412~443 
void Read_PTG_Info(u8 type)
{			 
	u8 i;
	if(type==PT_BASIC)//��ȡ������Ϣ
	{										
		PT_Size=AT24CXX_ReadOneByte(PTGAME_24C0X_BASE_ADDR);//��ȡͼƬ��С
		PT_New=AT24CXX_ReadOneByte(PTGAME_24C0X_BASE_ADDR+1); //��ȡ�Ƿ��Ѿ�����
		//��ȡ��ǰ���ò���	    
		PT_Step=AT24CXX_ReadLenByte(PTGAME_24C0X_BASE_ADDR+2,2);	 	   
		//��ȡ��ǰ����ʱ��
		PT_Time=AT24CXX_ReadLenByte(PTGAME_24C0X_BASE_ADDR+4,2);		   
	}else if(type==PT_TABLE)//����ͼ����Ϣ
	{
		for(i=0;i<26;i++)Cur_Table[i]=AT24CXX_ReadOneByte(PTGAME_24C0X_BASE_ADDR+6+i);//��ȡ��ǰͼ����Ϣ	  
	}
}
//�������ֵ
//level:�ȼ� 0~2 
// step:����
// time:��ʱ
//��ַ����:444~	455
void Save_PTG_Best(u8 level,u16 step,u16 time)
{		
	//���浱ǰ���ò���
	AT24CXX_WriteLenByte(PTGAME_24C0X_BASE_ADDR+32+level*4,step,2); 	 		
	//���浱ǰ����ʱ��
	AT24CXX_WriteLenByte(PTGAME_24C0X_BASE_ADDR+34+level*4,time,2);	   	
}
//��ȡ���ֵ
//level:�ȼ� 0~2 == 3 4 5 
// step:����
// time:��ʱ
//��ַ����:444~	455
void Read_PTG_Best(u8 level,u16 *step,u16 *time)
{				
	//��ȡ��ǰ���ò���	
	*step=AT24CXX_ReadLenByte(PTGAME_24C0X_BASE_ADDR+32+level*4,2); 	 
	//��ȡ��ǰ����ʱ��
	*time=AT24CXX_ReadLenByte(PTGAME_24C0X_BASE_ADDR+34+level*4,2); 			
}

//��ָ���б��õ�ͼƬ��ʼ�����
//���������LCD��ʾ�й�
//table:�̶�����ֵ 0~PT_Size*PT_Size
//xpos :x����ֵ
//ypos :y����ֵ	
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
	if(table==PT_Size*PT_Size)//��PT_Size*PT_Size�����ʼ��ַ
	{
		*xpos=Xscal*(PT_Size-1)+5;
		*ypos=Xscal*PT_Size+5;
	}
}
/////////////////////////////////////////////////////////////////////
//���ƴͼ��Ϸ��ʤ���㷨
//����ֵ��0��û�ɹ���1���ɹ���
//֧�ִ�3*3~15*15�����̴�С
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
//��ʼ��(�������λ��)	 
//�����ƴͼ�ܷ���ɵ��㷨
//CHECK OK 09/05/30
void PT_Map_Init(void)
{		 
	u8 size;		 
	u8 i,t,a, b;	 
	u8 d=1;
	signed char w,h;	 
	size=PT_Size*PT_Size;	 								  
	srand(RTC->CNTL);//�õ�����	   
	for(i=0;i<size;i++)Cur_Table[i]=i;	  
	Cur_Table[size]=size-1;  //���һ��ͼƬ���������ԭλ��������Ϸ�޷���� 
	// ��ʼ��(�������λ��)	 	   
	i=0;
	while(i++<size||d==0)// �������ٵ�λ������
	{
		do 	
		{										  
			a = rand()%(size);			   
			b = rand()%(size);
		}while(a==b);   
		t=Cur_Table[a];
		Cur_Table[a]=Cur_Table[b];
		Cur_Table[b]=t;
		// ������룬����ҪģΪ1�н�
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

//װ����ϷͼƬ
//֧�ִ�3*3~15*15�����̴�С
//CHECK OK 09/05/30
void Load_Game_PIC(void) 
{
	FileInfoStruct TempPic;//����ͼƬ����ʱ��ſռ�	    
	u8 t;
	u8 temp;
	u8 tempx,tempy; 
	u8 size;
	size=PT_Size*PT_Size;   									  
	TempPic.F_StartCluster=Pic_Addr[0];//����Ԥ��ͼƬ		   
	TempPic.F_Type=T_BMP;//ָ��ΪBMPͼƬ
	AI_LoadPicFile(&TempPic,3,242,79,318);//װ��Ԥ��ͼƬ 
	//����	  
	for(t=0;t<size+1;t++)
	{
		temp=Cur_Table[t];
		Get_Ps_Addr(t,&tempx,&tempy);   
		if(temp==size)//�ո�������������ɫ	
		{
		 	LCD_Fill(tempx,tempy,tempx+Xscal-2,tempy+Xscal-2,LGRAY);	 
		}else
		{
			TempPic.F_StartCluster=Pic_Addr[temp+1];//ͼƬ��ַ		   
			AI_LoadPicFile(&TempPic,tempx,tempy,tempx+Xscal-1,tempy+Xscal-1);	 
		}
	}	 
}
//sour:���������Ĺ̶����
//dest:�ո����ڵĹ̶����
//�ո�һ������dest������
//CHECK OK 09/05/30
void Move_Pic(u8 sour,u8 dest)
{
	FileInfoStruct PicPart;//����ͼƬ����ʱ��ſռ�	    
	u8 pic_real_addr;
	u8 tempx,tempy;			 

	pic_real_addr=Cur_Table[sour]+1;//�õ�ͼƬ�ı�ŵ�ַ 
	PicPart.F_StartCluster=Pic_Addr[pic_real_addr];
	PicPart.F_Type=T_BMP;//ָ��ΪBMPͼƬ	
	//���sour����ͼƬ�����ǰѿո��Ƶ������
	Get_Ps_Addr(sour,&tempx,&tempy);//�õ�Դ����   
	LCD_Fill(tempx,tempy,tempx+Xscal-2,tempy+Xscal-2,LGRAY);											    	 
	//��sour����ͼƬ������ʾ��dest��
	Get_Ps_Addr(dest,&tempx,&tempy);//�õ�Ŀ������
	AI_LoadPicFile(&PicPart,tempx,tempy,tempx+Xscal-1,tempy+Xscal-1);												 
	Cur_Table[dest]=Cur_Table[sour];//��sour����ͼƬ��Ÿ���dest��
	Cur_Table[sour]=PT_Size*PT_Size;//�ո���sour���ˣ�������� 	
}   
	    
//����ֵ��PT_Size*PT_Size+1������Ҫ�ƶ�
//0~PT_Size*PT_Size ��Ҫ�ƶ�����key����
//ƴͼ��Ϸ�ĺ����㷨		   
//֧�ִ�3*3~15*15�����̴�С
//CHECK OK 09/05/30
u8 Need_Move(u8 key)
{
	u8 temp;
	u8 size;
	u8 i;
	size=PT_Size*PT_Size;	  
	if(Cur_Table[key]==size)return size+1;//�����˿ո񣬲��ƶ�
	else 
	{
		///////////////////////////////////�ж��ǲ�����Ҫ��PT_Size	 
		if(key>=PT_Size)//�жϼ�PT_Size(3,4,5...)
		{
			if(key==size)temp=size-1;//���������һ�� 9���������������-1�൱����������-size
			else temp=key-PT_Size;
			if(Cur_Table[temp]==size)return temp;//�ո���˴ΰ���ֵ���ڣ����ؿո����ڵ�ַ
		}	
		///////////////////////////////////�ж��ǲ�����Ҫ��PT_Size     
		if(key<(size-PT_Size))//�жϼ�PT_Size
		{
			temp=key+PT_Size;
			if(Cur_Table[temp]==size)return temp;//�ո���˴ΰ���ֵ���ڣ����ؿո����ڵ�ַ
		}    
		///////////////////////////////////�ж��ǲ�����Ҫ��1 
		for(i=0;i<PT_Size;i++)//�߼ʴ���3��0,3,6;4:0,4,8,12;5:0,5,10,15,20....��Щ��ֵ����Ҫ��1
		{
			if(key==PT_Size*i)break;
		}
		if(i==PT_Size)//��Ҫ��1��
		{
			temp=key-1;			    
			if(Cur_Table[temp]==size)return temp;//�ո���˴ΰ���ֵ���ڣ����ؿո����ڵ�ַ
		}	 
		///////////////////////////////////�ж��ǲ�����Ҫ��1
		for(i=0;i<PT_Size;i++)//�߼ʴ���
		{
			if((key+1)==PT_Size*i)break;
		}
		if(i==PT_Size)//��Ҫ��1��
		{
			temp=key+1;//�ж�+1		 
		    if(temp==(size+1))return size+1;//����Ҫ�ƶ�  
			if(Cur_Table[temp]==size)return temp;//�ո���˴ΰ���ֵ���ڣ����ؿո����ڵ�ַ
		}	 
		return size+1;//����Ҫ�ƶ�
	}
}
/////////////////////////////////////////////////////////////////////
//ƴͼ��Ϸ�İ���ֵ��ȡ����
//���ذ���ֵ 		 
u8 PTGame_Key_To_Num(void)
{
	u8 t,ytemp;						   
	if(Is_In_Area(100,300,33,16))return 0XFF;//ѡ�����˳��������˳�ģʽ		 
	for(t=0;t<PT_Size;t++)//���س��水��
	{	  		   	
		for(ytemp=0;ytemp<PT_Size;ytemp++)
		if(Is_In_Area(5+Xscal*t,5+Xscal*ytemp,Xscal,Xscal))return t+ytemp*PT_Size+1;
	}    
	if(PT_Size==3&&Is_In_Area(159,236,77,77))return 10;
	else if(PT_Size==4&&Is_In_Area(179,237,58,77))return 17;
	else if(PT_Size==5&&Is_In_Area(188,234,46,46))return 26;
	return 0;//������Ч
} 				    
//��ʾ����
//��ʾ�����ʱ��	 
void PTGame_Show_Msg(void)
{
	POINT_COLOR=RED;//������ɫ
	BACK_COLOR=LGRAY;	   
	LCD_ShowNum(121,258,PT_Step,4,16);//��ʾ����
	LCD_ShowNum(121,274,PT_Time,4,16);//��ʾʱ��	  		  
}
//���RTC�ڵ����Ӽ�����	
u32 Get_PTG_Time(void)
{
	u32 temp;
	temp=RTC->CNTH;
	temp<<=16;
	temp=RTC->CNTL;  
	return temp; 
}
//ƴͼ��Ϸʤ��ʱ�Ľ���
//�����Ƿ񴴼�¼���    
void PTG_Win_Msg(void)
{		   
	u8 BEST=0;
	u16 timex,stepx;  
	u16 minx=0;	 
	u8 tkey;  
	//��ʼ�ж��ǲ��Ǵ���¼��
	Read_PTG_Best(PT_Size-3,&stepx,&timex);//��ȡ�ü�������¼	
	if(timex==0)BEST=1;				       //��һ��
	if(timex<stepx)minx=timex;			   //ȥ��С��ֵ
	else minx=stepx;      
	if(PT_Time<minx||PT_Step<minx)BEST=1;//ʱ��/����С����ǰ���ֵ����¼����  
	else if(PT_Time==timex&&PT_Step<stepx)BEST=1;//ʱ����ȣ��������٣�Ҳ�Ƽ�¼
	 
	LCD_Fill(40,80,200,160,GRAY);//���հ���
	POINT_COLOR=BROWN;
	LCD_DrawRectangle(39,79,201,161);//���յķ���
	LCD_DrawRectangle(40,80,200,160);//���յķ���
	LCD_DrawRectangle(41,81,199,159);//���յķ���	 
	BACK_COLOR=GRAY;
	POINT_COLOR=RED;
	if(BEST)
	{	
		Save_PTG_Best(PT_Size-3,PT_Step,PT_Time);//������Ѽ�¼
		Show_Str_Mid(40,82,"����¼�ˣ�",16,160);//����ģʽ,���Զ�����						 
	}
	else Show_Str_Mid(40,82,"��ϲ���أ�",16,160);//����ģʽ,���Զ�����
	POINT_COLOR=BLACK;	
	Show_Str(44,100,"���ĳɼ���",16,0X01);//����ģʽ,���Զ�����
	Show_Str(44,118,"����:",16,0X01);//����ģʽ,���Զ�����	  
	Show_Str(124,118,"ʱ��:",16,0X01);//����ģʽ,���Զ�����
	POINT_COLOR=RED;
	LCD_ShowNum(84,118,PT_Step,4,16);//��ʾ����
	LCD_ShowNum(164,118,PT_Time,4,16);//��ʾʱ��   
	BACK_COLOR=LGRAY;
    Show_Str(104,139,"ȷ��",16,0X00);//����ģʽ,���Զ�����		   
	POINT_COLOR=BROWN;	   
	LCD_DrawRectangle(104,139,136,155);//���յķ���
	
	Pen_Point.Key_Sta=Key_Up;//�ͷ���ʾ�ڼ�Ĵ��� 
	timex=0;
	while(1)//�ȴ������˳�
	{
		tkey=AI_Read_TP(30);
		if(tkey==2)Pen_Point.Key_Reg|=0X80;//��ǰ����Ѿ������� 	   
		if(tkey==0&&(Pen_Point.Key_Reg&0X80))//�����ɿ���
		{
			Pen_Point.Key_Reg=0;
  			if(Is_In_Area(104,139,32,16))return;//ȷ������������  

		}   
	}
}	
//��ʾ��Ѽ�¼
void PTG_Best_Show(void)
{	
	u8 temp;
	u16 timex,stepx;

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
	//��ʾ����
	POINT_COLOR=WHITE;
	Show_Str_Mid(0,2,"��Ѽ�¼",16,240); //����ģʽ,���Զ����� 
	Show_Str(5,300,"ȷ��",16,0x01);   //����ģʽ,���Զ�����
    //��ʾ��Ϣ	   
	POINT_COLOR=RED;
	Show_Str_Mid(0,50,"��",16,240);   
	Show_Str_Mid(0,114,"һ��",16,240); 
	Show_Str_Mid(0,178,"����",16,240); 
		  
	LCD_Fill(0,74,239,106,LGRAY);   //�����ɫ
	LCD_Fill(0,138,239,170,LGRAY);  //�����ɫ
	LCD_Fill(0,202,239,234,LGRAY);  //�����ɫ
	
	BACK_COLOR=LGRAY;//�ٶ�û��ѡ�У�ԭ���� 
	POINT_COLOR=DARKBLUE;    
	for(temp=0;temp<3;temp++)
	{
		Show_Str(32,82+temp*64,"����:",16,0x01);   //����ģʽ,���Զ����� 
		Show_Str(136,82+temp*64,"ʱ��:",16,0x01);  //����ģʽ,���Զ����� 
	}
	POINT_COLOR=RED; 
	for(temp=0;temp<3;temp++)
	{
		Read_PTG_Best(temp,&stepx,&timex);//��ȡ���ֵ	 
		LCD_ShowNum(72,82+temp*64,stepx,4,16);//��ʾ����
		LCD_ShowNum(176,82+temp*64,timex,4,16);//��ʾʱ�� 	   
	}
	Pen_Point.Key_Sta=Key_Up;//�ͷ���ʾ�ڼ�Ĵ��� 
	temp=0;
	while(1)//�ȴ������˳�
	{
		temp=AI_Read_TP(30);
		if(temp==2)Pen_Point.Key_Reg|=0X80;//��ǰ����Ѿ������� 	   
		if(temp==0&&(Pen_Point.Key_Reg&0X80))//�����ɿ���
		{
			Pen_Point.Key_Reg=0;
  			if(Is_In_Area(0,300,40,20))return;//ȷ������������  

		}   	    
	}	  						   	   
}   
//��ƴͼ��Ϸ									   
void PTGame_Play(void)
{
	u8 key,temp;
	u8 size;		  
	u32 game_time;		    

	//���ȵõ���Ϸ��ͼƬ�ļ�
	temp=SysInfoGet(0x80+PT_Size);//����ͼƬ
	if(temp)
	{
	    LCD_Clear(WHITE);//����  		  
		POINT_COLOR=RED;
		Show_Str_Mid(0,144,"ƴͼ��Ϸ���ݶ�ʧ",16,240); //����ģʽ,���Զ�����
		Show_Str_Mid(0,160,"����ϵͳ�ļ��������ԣ�",16,240); //����ģʽ,���Զ�����
		delay_ms(1500);
		return;
	}			    		 
	//��һ��Ҫ�õ���������
	if(PT_Size==3)Xscal=77;
	else if(PT_Size==4)Xscal=58;
	else if(PT_Size==5)Xscal=46;    
	size=PT_Size*PT_Size; 
	if(PT_New)
	{
		PT_Time=0;//ʱ�����
		PT_Step=0;//�������		
		PT_Map_Init();//��Ҫ���´���ͼƬ
	}else Read_PTG_Info(PT_TABLE);//��ȡ�б���Ϣ		 

	Load_Game_ICO(PT_Size);//װ��X*X����
	Load_Game_PIC();//װ��ͼƬ
	//�õ���Ϸ��ʼʱ��
	game_time=Get_PTG_Time();//�õ�ʱ��
	key=0;		  
	PTGame_Show_Msg();//��ʾʱ�䣬����  
	Pen_Point.Key_Sta=Key_Up;//�ͷ���ʾ�ڼ�Ĵ��� 
	while(1)//�ȴ�����
	{
		key=AI_Read_TP(30);	    
		if(key)Pen_Point.Key_Reg|=0X80;  //��ǰ����Ѿ�������(�����ǻ������Ƕ���) 	   
		if(key==0&&(Pen_Point.Key_Reg&0X80))//�����ɿ���
		{
			Pen_Point.Key_Reg=0;		  	
			key=PTGame_Key_To_Num(); 		   	   
			if(key==0XFF)
			{
				PT_New=0;//��Ϸû�н������´ο��Բ�����
				Save_PTG_Info(PT_BASIC);//���������Ϣ
				Save_PTG_Info(PT_TABLE);//�����б���Ϣ
				return;//ѡ�����˳�	 
			}else
			{
				key-=1;//����ֵΪ1~10������1��ƥ��
				temp=Need_Move(key);//����1		  
				if(temp<=size)
				{
					Move_Pic(key,temp);//��key�����ͼƬ��������temp����
					PT_Step++;         //��������
					PTGame_Show_Msg(); //��ʾʱ�䣬����   
					if(PT_Win_Chk())//��Ϸ������ƴͼ�ɹ���
					{								 	 
						PT_New=1;//��Ϸ�������´α������
						Save_PTG_Info(PT_BASIC);//���������Ϣ
						PTG_Win_Msg();//�ɹ�ʱ�Ľ�����ʵ
						return; 
					}
				}
			}
			key=0;
		}
		if(game_time!=Get_PTG_Time())//����������1s
		{
			game_time=Get_PTG_Time();
			PT_Time++;//ʱ������
			LED0=!LED0;//LED��˸
			PTGame_Show_Msg();//��ʾʱ�䣬����
		}
	}	 
}	    
//��Ϸ ����
void Game_Play(void)
{			    
	u8 selx=0;	 
	while(1)
	{
		selx=Load_Fun_Sel(PTGmenu_F1,"��Ϸѡ��","ȷ��",1,0);
		if(selx==8)return;//����
		else if(selx==0)//ѡ����ƴͼ��Ϸ����ʱֻ����һ����Ϸ
		{
		 	Read_PTG_Info(PT_BASIC);//��ȡ������Ϣ
			if(PT_Size>5||PT_Size<3)//��Ϣ�Ƿ�,���±�����Ϣ
			{
				PT_Size=3;PT_New=0;
				PT_Step=0;PT_Time=0;
				Save_PTG_Info(PT_BASIC);//���������Ϣ	   
				for(selx=0;selx<26;selx++)Cur_Table[selx]=0;
				Save_PTG_Info(PT_TABLE);//����ͼ����Ϣ
				Save_PTG_Best(0,0,0);
				Save_PTG_Best(1,0,0);
				Save_PTG_Best(2,0,0);
			}else if(!PT_New)Read_PTG_Info(PT_TABLE);//��ȡͼ����Ϣ

			while(1)
			{	    
				if(PT_New)selx=Load_Fun_Sel(PTGmenu_S1+1,"ƴͼ��Ϸ","ȷ��",3,0);//����ʾ������ť
				else selx=Load_Fun_Sel(PTGmenu_S1,"ƴͼ��Ϸ","ȷ��",4,0);//��ʾ������ť
				if(selx==8)break;//������һ��Ŀ¼
				if(PT_New)selx+=1;//ƫ��һ��
				switch(selx)
				{
					case 0://ѡ���˼���
						PTGame_Play();
						break;
					case 1://ѡ��������Ϸ
						PT_New=1;//
						PTGame_Play();
						break;
					case 2://�������
						PTG_Best_Show();
						break;
					case 3://�Ѷ�����
						selx=Load_Fun_Sel(PTGmenu_S2,"�Ѷ�ѡ��","ȷ��",3,PT_Size-3);//ѡ���Ѷ�
						if(selx<3)//ѡ������Ч����
						{
							PT_Size=3+selx; 				
							PT_New=1;//����Ҫ����������� 
						}
						break;
				}
			}
		}	 
	} 
}






























