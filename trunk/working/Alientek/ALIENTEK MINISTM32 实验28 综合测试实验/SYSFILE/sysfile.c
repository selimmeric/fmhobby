#include "sysfile.h"
#include "fat.h"
#include "jpegbmp.h"
//Mini STM32开发板
//系统文件查找代码					  
//正点原子@ALIENTEK
//2010/6/18
				   
u32 PICCLUSTER=0;//图片文件夹地址	 
u32 sys_ico[9];  //系统图标缓存区!不能篡改!
u32 file_ico[4]; //文件图标缓存区 folder;mus;pic;book;
u32 *Pic_Addr=(u32*)iclip;//注意 0：表示预览图片的地址 1~n，表示1~n块图片的地址
 											    
//系统文件夹		  
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

//系统文件名定义
const unsigned char *sysfile[]=
{			  
//系统字体图标 0开始
"GBK16.FON",
"GBK12.FON",
"UNI2GBK.SYS",
//系统文件图标	3开始
"FOLDER.BMP",
"MUS.BMP",
"PIC.BMP",
"BOOK.BMP",
//系统主界面图标 7开始
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
//得到游戏图片编号
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
	*str='\0';//加入结束符
}
//获取系统文件的存储地址
//次步出错,则无法启动!!!
//返回0，成功。返回其他，错误代码
//sel:bit7：0,查找系统文件
//sel:bit7：1,查找游戏文件
//sel:bit 0~6 编号
u8 SysInfoGet(u8 sel)
{			 		   
	u32 cluster=0;
	u32 syscluster=0;
	u8 t=0;
	u8 size;
	u8 imgx[9];
		
	FileInfoStruct t_file;  	  						    	 
	//得到根目录的簇号
	if(FAT32_Enable)cluster=FirstDirClust;
	else cluster=0;			
	   
	t_file=F_Search(cluster,(unsigned char *)folder[0],T_FILE);//查找system文件夹
	if(t_file.F_StartCluster==0)return 2;//系统文件夹丢失
	syscluster=t_file.F_StartCluster;//保存系统文件夹所在簇号	  
	if((sel&0x80)==0)//查找系统文件 
	{	
		t=sel&0x7F;//得到编号
		if(t==0)//系统文件
		{															   	  	   
			t_file=F_Search(syscluster,(unsigned char *)folder[2],T_FILE);//在system文件夹下查找SYSICO文件夹
			if(t_file.F_StartCluster==0)return 3; 
			cluster=t_file.F_StartCluster;//保存SYSICO文件夹簇号
			for(t=0;t<9;t++)//查找系统图标,九个
			{
				t_file=F_Search(cluster,(unsigned char *)sysfile[t+7],T_BMP);//在SYSICO文件夹下查找系统图标
				sys_ico[t]=t_file.F_StartCluster;
				if(t_file.F_StartCluster==0)return 4;//失败	
			}
			for(t=3;t<7;t++)//查找文件图标,4个
			{
				t_file=F_Search(cluster,(unsigned char *)sysfile[t],T_BMP);//在SYSICO文件夹下查找文件图标
				file_ico[t-3]=t_file.F_StartCluster;
				if(file_ico[t-3]==0)return 5;//失败 	  
			}
		}else//图片文件夹 
		{
			t_file=F_Search(cluster,(unsigned char *)folder[3],T_FILE);//查找PICTURE文件夹
			if(t_file.F_StartCluster==0)return 1;//图片文件夹丢失
			PICCLUSTER=t_file.F_StartCluster;//图片文件夹所在簇号	
		} 
	}else if(sel&0x80)//查找游戏
	{	 	
		size=sel&0x7F;//得到t:3,3*3;4,4*4;5,5*5									  
		t_file=F_Search(syscluster,(unsigned char *)folder[4],T_FILE);//在system文件夹下查找game文件夹
		if(t_file.F_StartCluster==0)return 6;  		
					   
		t_file=F_Search(t_file.F_StartCluster,(unsigned char *)folder[2+size],T_FILE);//在game文件夹下查找LEVEL(sizex-2)文件夹	
		if(t_file.F_StartCluster==0)return 7;//levelx 文件夹丢失
		cluster=t_file.F_StartCluster;
		  	  			   
		size*=size;//取平方
		for(t=0;t<size+1;t++) //X*X图片地址获取
		{
			Get_Game_Pic_Str(t,imgx);//得到图片编号		  
			t_file=F_Search(cluster,imgx,T_BMP);//在LEVEL1文件夹下查找BMP图片
			Pic_Addr[t]=t_file.F_StartCluster;
			if(Pic_Addr[t]==0)return 8;//系统文件夹丢失	 	
		}	 					
		//printf("level ok\n");
	}

	return 0;//成功
}

			 






