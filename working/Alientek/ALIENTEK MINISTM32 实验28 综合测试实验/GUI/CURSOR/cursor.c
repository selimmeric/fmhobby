#include "cursor.h"
#include "lcd.h"  
//Mini STM32������ 
//��� ����
//����ԭ��@ALIENTEK

//���ṹ���ʼ����ֵ
GUI_Cursor MY_Cursor=
{
	LCD_W/2, //��굱ǰ���ڵ�����
	LCD_H/2, 	
	WHITE,   //������ɫ
//�Ҽ��˵������Ĵ���
	0,
	0,	  
	0,	          //���״̬,bit0:0,û����ʾ,1,��ʼ��ʾ��;bit1:�Ҽ��˵��������
	Set_Cursor,   //�������ù�굽ĳ������,�������		 
	Clear_Cursor, //����������һ�εĹ��		 
	Cursor_Draw,  //��������һ�εĹ��,�������µĹ��
};				 
//��׼���
//12*20��С
const u8 std_cur_tab[36]=	
{0xFF,0xFF,0x00,0x80,0x02,0x00,0x40,0x04,0x00,
 0x20,0x08,0x00,0x10,0x18,0x00,0x08,0x06,0x00,
 0x04,0x01,0x80,0x02,0x38,0x60,0x01,0x26,0x10,
 0x00,0xA1,0x90,0x00,0x60,0x60,0x00,0x20,0x00};
//��׼�������
const u8 std_cur_msk[36]=
{0xFF,0xFF,0x00,0xFF,0xFE,0x00,0x7F,0xFC,0x00,
 0x3F,0xF8,0x00,0x1F,0xF8,0x00,0x0F,0xFE,0x00,
 0x07,0xFF,0x80,0x03,0xFF,0xE0,0x01,0xE7,0xF0,
 0x00,0xE1,0xF0,0x00,0x60,0x60,0x00,0x20,0x00};

//��긲���������ݻ�����
//12*20�Ĺ��ʵ����ֻ��Ҫ125���ֽ�
u16 cur_buf[125];   

//������
//����������һ�εĹ��
//(x,y)һ�����(MY_Cursor.X,MY_Cursor.Y)							    
void Clear_Cursor(u16 x,u16 y)
{
	u16 y0=y;
	u8 t,t1,temp,temp1;	  		 
	u8 rccnt=0;//��ȡ������ɫ������	
	if((MY_Cursor.STA&0x01)==0)return;//�����껹û��ʼ��,��ֱ�ӷ���	  
    for(t=0;t<36;t++)
    {   												   
	    temp=std_cur_tab[t]; //�õ����� 
		temp1=std_cur_msk[t];//�õ���������                         
        for(t1=0;t1<8;t1++)
		{
			if(temp1&0x80)
			{
              	POINT_COLOR=cur_buf[rccnt];//��ԭԭ������ɫ
				if(y<320)LCD_DrawPoint(x,y);//LCD_DrawPoint����û���жϷ�Χ,�����������ⲿ�ж�
				rccnt++;
 			}	   
			temp<<=1;
			temp1<<=1;
			y++;							   
			if((y-y0)==20)
			{
				y=y0;
				x++;
				if(x>=LCD_W)return;//X���� Խ��
				break;
			}
		}  	 
	} 
}	 
//(x,y):���ù���λ��
//color:������ɫ
//���Ϊ��ɫ,��Ϊ��׼���
void Set_Cursor(u16 x,u16 y)
{	
	u16 y0=y;
	u8 t,t1,temp,temp1;	  
	u16 out_color,in_color;	 
	u8 rccnt=0;//��ȡ������ɫ������

	out_color=in_color=MY_Cursor.Color;
	if(MY_Cursor.Color==WHITE)out_color=BLACK;//���Ϊ��ɫ,��Ϊ��׼��� 
	MY_Cursor.STA|=0X01;//��ǿ�ʼ
	MY_Cursor.X=x;//��¼�˴�����
	MY_Cursor.Y=y;   
	POINT_COLOR=MY_Cursor.Color;
	BACK_COLOR=WHITE;
    for(t=0;t<36;t++)
    {   												   
	    temp=std_cur_tab[t]; //�õ����� 
		temp1=std_cur_msk[t];//�õ���������                         
        for(t1=0;t1<8;t1++)
		{
			if(temp1&0x80)
			{
				if(temp&0x80)POINT_COLOR=out_color;//�߿�
                else POINT_COLOR=in_color;//�ڲ� 
				cur_buf[rccnt]=LCD_ReadPoint(x,y);//��ȡ��ǰ�����ɫֵ 
				rccnt++;
				if(y<320)LCD_DrawPoint(x,y);//LCD_DrawPoint����û���жϷ�Χ,�����������ⲿ�ж�
 			}//͸������,�������� 	   
			temp<<=1;
			temp1<<=1;
			y++;							  
			if((y-y0)==20)
			{
				y=y0;
				x++;
				if(x>=LCD_W)return;//X�᷽�� Խ��
				break;
			}
		}  	 
	}   
}
//�����,�����
//�ú����������һ�εĹ��
//�ٻ���һ����Ҫ�����Ĺ��
void Cursor_Draw(u16 x,u16 y)
{
	Clear_Cursor(MY_Cursor.X,MY_Cursor.Y);//�����һ�εĹ��
	Set_Cursor(x,y);
}







