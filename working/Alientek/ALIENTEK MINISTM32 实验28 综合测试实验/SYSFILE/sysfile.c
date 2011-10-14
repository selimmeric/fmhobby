#include "sysfile.h"
#include "fat.h"
#include "jpegbmp.h"
//Mini STM32������
//ϵͳ�ļ����Ҵ���					  
//����ԭ��@ALIENTEK
//2010/6/18
				   
u32 PICCLUSTER=0;//ͼƬ�ļ��е�ַ	 
u32 sys_ico[9];  //ϵͳͼ�껺����!���ܴ۸�!
u32 file_ico[4]; //�ļ�ͼ�껺���� folder;mus;pic;book;
u32 *Pic_Addr=(u32*)iclip;//ע�� 0����ʾԤ��ͼƬ�ĵ�ַ 1~n����ʾ1~n��ͼƬ�ĵ�ַ
 											    
//ϵͳ�ļ���		  
const unsigned char *folder[]=
{
"SYSTEM",
"FONT",
"SYSICO",
"PICTURE",
"GAME",
"LEVEL1",
"LEVEL2",
"LEVEL3",	 
};		   

//ϵͳ�ļ�������
const unsigned char *sysfile[]=
{			  
//ϵͳ����ͼ�� 0��ʼ
"GBK16.FON",
"GBK12.FON",
"UNI2GBK.SYS",
//ϵͳ�ļ�ͼ��	3��ʼ
"FOLDER.BMP",
"MUS.BMP",
"PIC.BMP",
"BOOK.BMP",
//ϵͳ������ͼ�� 7��ʼ
//"EBOOK.BMP",
"PICTURE.BMP",
"PICTURE.BMP",
"GAME.BMP",
"TOUCH.BMP",
"TIME.BMP",
"MOUSE.BMP",  
"USB.BMP",
"REMOTE.BMP",
"WIRELESS.BMP",  	  
};
//�õ���ϷͼƬ���
void Get_Game_Pic_Str(u8 t,u8 *str)
{
	*str++='I';
	*str++='M';
	*str++='G';
	*str++=(t/10)%10+'0';
	*str++=t%10+'0';
	*str++='.';	  
	*str++='B';		
	*str++='M';
	*str++='P';
	*str='\0';//���������
}
//��ȡϵͳ�ļ��Ĵ洢��ַ
//�β�����,���޷�����!!!
//����0���ɹ��������������������
//sel:bit7��0,����ϵͳ�ļ�
//sel:bit7��1,������Ϸ�ļ�
//sel:bit 0~6 ���
u8 SysInfoGet(u8 sel)
{			 		   
	u32 cluster=0;
	u32 syscluster=0;
	u8 t=0;
	u8 size;
	u8 imgx[9];
		
	FileInfoStruct t_file;  	  						    	 
	//�õ���Ŀ¼�Ĵغ�
	if(FAT32_Enable)cluster=FirstDirClust;
	else cluster=0;			
	   
	t_file=F_Search(cluster,(unsigned char *)folder[0],T_FILE);//����system�ļ���
	if(t_file.F_StartCluster==0)return 2;//ϵͳ�ļ��ж�ʧ
	syscluster=t_file.F_StartCluster;//����ϵͳ�ļ������ڴغ�	  
	if((sel&0x80)==0)//����ϵͳ�ļ� 
	{	
		t=sel&0x7F;//�õ����
		if(t==0)//ϵͳ�ļ�
		{															   	  	   
			t_file=F_Search(syscluster,(unsigned char *)folder[2],T_FILE);//��system�ļ����²���SYSICO�ļ���
			if(t_file.F_StartCluster==0)return 3; 
			cluster=t_file.F_StartCluster;//����SYSICO�ļ��дغ�
			for(t=0;t<9;t++)//����ϵͳͼ��,�Ÿ�
			{
				t_file=F_Search(cluster,(unsigned char *)sysfile[t+7],T_BMP);//��SYSICO�ļ����²���ϵͳͼ��
				sys_ico[t]=t_file.F_StartCluster;
				if(t_file.F_StartCluster==0)return 4;//ʧ��	
			}
			for(t=3;t<7;t++)//�����ļ�ͼ��,4��
			{
				t_file=F_Search(cluster,(unsigned char *)sysfile[t],T_BMP);//��SYSICO�ļ����²����ļ�ͼ��
				file_ico[t-3]=t_file.F_StartCluster;
				if(file_ico[t-3]==0)return 5;//ʧ�� 	  
			}
		}else//ͼƬ�ļ��� 
		{
			t_file=F_Search(cluster,(unsigned char *)folder[3],T_FILE);//����PICTURE�ļ���
			if(t_file.F_StartCluster==0)return 1;//ͼƬ�ļ��ж�ʧ
			PICCLUSTER=t_file.F_StartCluster;//ͼƬ�ļ������ڴغ�	
		} 
	}else if(sel&0x80)//������Ϸ
	{	 	
		size=sel&0x7F;//�õ�t:3,3*3;4,4*4;5,5*5									  
		t_file=F_Search(syscluster,(unsigned char *)folder[4],T_FILE);//��system�ļ����²���game�ļ���
		if(t_file.F_StartCluster==0)return 6;  		
					   
		t_file=F_Search(t_file.F_StartCluster,(unsigned char *)folder[2+size],T_FILE);//��game�ļ����²���LEVEL(sizex-2)�ļ���	
		if(t_file.F_StartCluster==0)return 7;//levelx �ļ��ж�ʧ
		cluster=t_file.F_StartCluster;
		  	  			   
		size*=size;//ȡƽ��
		for(t=0;t<size+1;t++) //X*XͼƬ��ַ��ȡ
		{
			Get_Game_Pic_Str(t,imgx);//�õ�ͼƬ���		  
			t_file=F_Search(cluster,imgx,T_BMP);//��LEVEL1�ļ����²���BMPͼƬ
			Pic_Addr[t]=t_file.F_StartCluster;
			if(Pic_Addr[t]==0)return 8;//ϵͳ�ļ��ж�ʧ	 	
		}	 					
		//printf("level ok\n");
	}

	return 0;//�ɹ�
}

			 






