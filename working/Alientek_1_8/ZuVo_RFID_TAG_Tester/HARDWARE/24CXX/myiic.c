#include "myiic.h"
#include "delay.h"
//STM32���ģ��IIC��STM32��Ӳ��IIC̫�����ˣ�
//Mini STM32������
//IIC ��������
//����ԭ��@ALIENTEK
//2010/6/10 
//��ʼ��IIC
void IIC_Init(void)
{					     
 	RCC->APB2ENR|=1<<4;//��ʹ������IO PORTCʱ�� 							 
	GPIOC->CRH&=0XFFF00FFF;//PC11/12 �������
	GPIOC->CRH|=0X00033000;	   
	GPIOC->ODR|=3<<11;     //PC11,12 �����
}
//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//����I2C���߽����ź�
	delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }		
/*	
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
*/

	switch (ack)
	{
		case 0:
        	IIC_NAck();//����nACK
			break;
		case 1:
        	IIC_Ack(); //����ACK   
			break;
		default:	   // No ack
			break;
	
	}
		

    return receive;
}

//��1���ֽڣ� 
u8 IIC_Read_ByteEx(void)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(100);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(100); 
		IIC_SCL=0;
    }		
		delay_us(500);

    return receive;
}

void IIC_Send_ByteEx(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
 //   IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
		IIC_SCL=0;
		delay_us(50);   
        IIC_SDA=txd&0x01;
		delay_us(50);   
        txd>>=1; 	  
		IIC_SCL=1;	
		delay_us(100); 
//		delay_us(100);
    }	 
//		IIC_SCL=0;
		delay_us(500);
} 

void IIC_Send_Cmd(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    for(t=0;t<8;t++)
    {              
    	IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
		delay_us(50);   
        IIC_SDA=txd&0x01;
		delay_us(50);   
        txd>>=1; 	  
		delay_us(100);   
		IIC_SCL=1;
		delay_us(100); 
//		IIC_SCL=0;	
//		delay_us(100);
    }	 
	
	IIC_SCL = 0;
	IIC_SDA = 1;
	delay_us(300); 

 	SDA_IN();			 	// Waiting for ack
	delay_us(100);
	IIC_SCL = 1;							
	delay_us(100);

	IIC_SCL = 0;
	delay_us(100);

//	delay_us(200);


//		delay_us(500);
} 

void IIC_Send_CmdEx(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    for(t=0;t<8;t++)
    {              
    	IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
		delay_us(50);   
        IIC_SDA=txd&0x01;
		delay_us(50);   
        txd>>=1; 	  
		delay_us(100);   
		IIC_SCL=1;
		delay_us(100); 
//		IIC_SCL=0;	
//		delay_us(100);
    }	 
	
	IIC_SCL = 0;
	IIC_SDA = 1;
	delay_us(300); 

 	SDA_IN();			 	// Waiting for ack
	delay_us(100);
	IIC_SCL = 1;			// Wait for SDA to L
	
	delay_ms(9);	
	delay_us(100);
	IIC_SCL = 0;
	delay_us(100);
//	IIC_SCL = 1;

 /*
	t=0;
	while ((t<100) && (IIC_SDA==1))
	{
		delay_ms(1);	
	}						
*/	
//	delay_ms(9);

	IIC_SCL = 0;
	delay_us(100);

//	delay_us(200);

//	delay_ms(9);

//		delay_us(500);
} 


 void IIC_StartEx(void)
{
	SDA_OUT();     		//sda�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(40);
 	IIC_SDA=0;			//START:when CLK is high,DATA change form high to low 
	delay_ms(200);
	IIC_SCL=0;			//ǯסI2C���ߣ�׼�����ͻ�������� 
	delay_us(40);

}	  
//����IICֹͣ�ź�
void IIC_StopEx(void)
{
	SDA_OUT();//sda�����
//	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
	IIC_SCL=1; 
 	delay_ms(20);
	IIC_SDA=1;//����I2C���߽����ź�
	delay_us(40);							   	
}

void IIC_RFID_WRITE(u8 *pBuffer,u8 iLen)
{									    

	unsigned char i;
//	unsigned char tmp;

	IIC_StartEx();

	IIC_Send_Cmd(0xd0);

 /*
	for (i=0; i<7; i++)
	{
		*(pBuffer+i) = *(pBuffer+i) <<1;
		*(pBuffer+i) |= (*(pBuffer+i+1) >> 7);		 	
	}

		*(pBuffer+7) = *(pBuffer+7) <<1;
		*(pBuffer+7) |= 1;		 	
*/

	for (i=0; i<=7 ; i++)
	{
		IIC_Send_ByteEx(*(pBuffer+(7-i)));	 // B0
	}

	delay_us(100);							   	
	IIC_StopEx();


}

void IIC_RFID_READ(u8 *pBuffer,u8 iLen)
{

	IIC_StartEx();
	IIC_Send_CmdEx(0xC0);

	*(pBuffer + 7)= IIC_Read_ByteEx();		// B7
	*(pBuffer + 6)= IIC_Read_ByteEx();		// B6
	*(pBuffer + 5)= IIC_Read_ByteEx();		// B5
	*(pBuffer + 4)= IIC_Read_ByteEx();		// B4
	*(pBuffer + 3)= IIC_Read_ByteEx();		// B3
	*(pBuffer + 2)= IIC_Read_ByteEx();		// B2
	*(pBuffer + 1)= IIC_Read_ByteEx();		// B1
	*(pBuffer + 0)= IIC_Read_ByteEx();	   	// B0 

	delay_us(100);							   	
	IIC_StopEx();


}




















