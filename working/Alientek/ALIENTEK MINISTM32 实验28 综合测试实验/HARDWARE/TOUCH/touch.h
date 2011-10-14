#ifndef __TOUCH_H__
#define __TOUCH_H__
#include "sys.h"
//Mini STM32������
//ADS7843/7846/UH7843/7846/XPT2046/TSC2046 ��������
//����ԭ��@ALIENTEK
//2010/6/13
//V1.3
//������u8 AI_Read_TP(void)����,���ڶ�ȡ����״̬	 
	    
//����״̬	 
#define Key_Down 0x01
#define Key_Up   0x00 
//�ʸ˽ṹ��
typedef struct 
{
	//��һ�ΰ��µ�����ֵ(LCD����),��AI_Read_TP�����ﱻ��¼
	//ֱ�������ɿ�,��ֵ�ǲ���ı��
	//�����洢����հ���ʱ������
	u16 X0;
	u16 Y0;
	//��û��ִ��Convert_Pos֮ǰ����������
	//��ִ����֮����LCD����,Ҳ���ǵ�ǰ����	    
	u16 X; //LCD����/��ǰ���� 
	u16 Y;						   	    
	u8 Key_Sta;//�ʵ�״̬
//�ʼĴ���
//bit7:���ڱ�Ǵ��㱻�������,���ⲿ�����޸�
//bit1,0:��һ�ΰ��µİ���ģʽ(1:����;2,�㴥)
	u8 Key_Reg;//�ʼĴ���	  
//������У׼����
	float xfac;
	float yfac;
	short xoff;
	short yoff;
}Pen_Holder;	   
extern Pen_Holder Pen_Point;
//�봥����оƬ��������	   
#define PEN  PCin(1)   //PC1  INT
#define DOUT PCin(2)   //PC2  MISO
#define TDIN PCout(3)  //PC3  MOSI
#define TCLK PCout(0)  //PC0  SCLK
#define TCS  PCout(13) //PC13 CS    
//ADS7843/7846/UH7843/7846/XPT2046/TSC2046 ָ�
#define CMD_RDY 0X90  //0B10010000���ò�ַ�ʽ��X����
#define CMD_RDX	0XD0  //0B11010000���ò�ַ�ʽ��Y����   											 
#define TEMP_RD	0XF0  //0B11110000���ò�ַ�ʽ��Y����    
//ʹ�ñ���
#define ADJ_SAVE_ENABLE	    
			  
void Touch_Init(void);		 //��ʼ��
u16 T_abs(u16 m,u16 n);      //������֮��ľ���ֵ
u8 Read_ADS(u16 *x,u16 *y);	 //��������˫�����ȡ
u8 Read_ADS2(u16 *x,u16 *y); //����ǿ�˲���˫���������ȡ
u16 ADS_Read_XY(u8 xy);		 //���˲��������ȡ(������)
u16 ADS_Read_AD(u8 CMD);	 //��ȡADת��ֵ
void ADS_Write_Byte(u8 num); //�����оƬд��һ������
void Drow_Touch_Point(u8 x,u16 y);//��һ�������׼��
void Draw_Big_Point(u8 x,u16 y);  //��һ�����
void Touch_Adjust(void);          //������У׼
void Save_Adjdata(void);		  //����У׼����
u8 Get_Adjdata(void); 			  //��ȡУ׼����
void Pen_Int_Set(u8 en); 		  //PEN�ж�ʹ��/�ر�
void Convert_Pos(void);           //���ת������	 
u8 AI_Read_TP(u8 MS_TIME);        //��ȡTP״̬	 
u8 Is_In_Area(u16 x,u16 y,u16 xlen,u16 ylen);//�жϰ����Ƿ������������.
#endif

















