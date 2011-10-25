/*深圳云佳科技无线应用，应用无限*/
/*********************************************************************
功能:
	RFID TARGET发射程序
说明:
	本程序采用定时2秒发射ID一次,发射完毕进入低功耗模式,低功耗电流3uA
	ID长度为6 byte
**********************************************************************/


#include "reg24le1.h"
#include <stdint.h>
#include "API.h"

//#define	PIN24
#define	PIN32
//#define	PIN48

#ifdef 	PIN24
sbit S3	 = P0^6;                               	// 1/0=未按/按下
sbit S2  = P0^4;                               	// 1/0=未按/按下
sbit S1  = P0^3;                               	// 1/0=未按/按下

sbit LED3 = P0^2;                         		// 1/0=灭/亮
sbit LED2 = P0^1;                               // 1/0=灭/亮
sbit LED1 = P0^0;                               // 1/0=灭/亮

#define p0dir	0xd8
#define p1dir	0xff
#endif

#ifdef 	PIN32
sbit S3	 = P1^4;                               	// 1/0=未按/按下
sbit S2  = P1^3;                               	// 1/0=未按/按下
sbit S1  = P1^2;                               	// 1/0=未按/按下

sbit LED3 = P0^4;                               // 1/0=灭/亮
sbit LED2 = P0^2;                               // 1/0=灭/亮
sbit LED1 = P0^0;                               // 1/0=灭/亮

#define p0dir	0xf0
#define p1dir	0xff
#endif

#ifdef 	PIN48
sbit S3	 = P0^7;                               	// 1/0=未按/按下
sbit S2  = P0^6;                               	// 1/0=未按/按下
sbit S1  = P0^5;                               	// 1/0=未按/按下

sbit LED3 = P0^2;                               // 1/0=灭/亮
sbit LED2 = P0^1;                               // 1/0=灭/亮
sbit LED1 = P0^0;                               // 1/0=灭/亮

#define p0dir	0xf0
#define p1dir	0x02
#endif

#define INTERRUPT_RFIRQ	9
#define INTERRUPT_TICK 	13  

#define TX_ADR_WIDTH    5   					// RF收发地址共5 bytes 
#define TX_PLOAD_WIDTH  20  					// 数据包长度为20 bytes

//uint8_t const TX_ADDRESS[TX_ADR_WIDTH]  = {0x34,0x56,0x78,0x90,0x12}; // 定义RF收发地址
//uint8_t data id_buf[TX_PLOAD_WIDTH]={0xff, 0x01, 0x02, 0x03, 0x04, 0x05};

uint8_t const TX_ADDRESS[TX_ADR_WIDTH]  = {0x34,0x56,0x78,0x90,0x12}; // 定义RF收发地址
uint8_t data id_buf[TX_PLOAD_WIDTH]={0x01,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};



uint8_t bdata sta;
sbit	RX_DR	=sta^6;
sbit	TX_DS	=sta^5;
sbit	MAX_RT	=sta^4;


unsigned char mFlagRTC=0; 

/**************************************************
功能：延时
**************************************************/
void delay(uint16_t x)
{
    uint16_t i,j;
    i=0;
    for(i=0;i<x;i++)
    {
       j=108;
           ;
       while(j--);
    }
}
/**************************************************
功能：硬件SPI读写
**************************************************/
uint8_t SPI_RW(uint8_t value)
{
  SPIRDAT = value;
  											       
  while(!(SPIRSTAT & 0x02));  					// 等待SPI传输完成

  return SPIRDAT;             					// 返回读出值
}
/**************************************************
功能：写RF寄存器，读RF状态值
**************************************************/
uint8_t SPI_RW_Reg(uint8_t reg, uint8_t value)
{
	uint8_t status;

  	RFCSN = 0;                   	
  	status = SPI_RW(reg);      					// 选择RF寄存器
  	SPI_RW(value);             					// 写入数据
  	RFCSN = 1;                   	

  	return(status);            					// 返回RF状态值
}
/**************************************************
功能：读RF寄存器
**************************************************/
uint8_t SPI_Read(uint8_t reg)
{
	uint8_t reg_val;

  	RFCSN = 0;                			
  	SPI_RW(reg);            					// 选择RF寄存器
  	reg_val = SPI_RW(0);    					// 读出数据
  	RFCSN = 1;                			

  	return(reg_val);        					// 返回RF状态值
}
/**************************************************
功能：把缓冲区的多字节数据写到RF寄存器
**************************************************/
uint8_t SPI_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
	uint8_t status,byte_ctr;

  	RFCSN = 0;                   		
  	status = SPI_RW(reg);    					// 选择RF寄存器
  	for(byte_ctr=0; byte_ctr<bytes; byte_ctr++) // 连接写入数据
    	SPI_RW(*pBuf++);
  	RFCSN = 1;                 			
  	return(status);          					// 返回RF状态值
}
/**************************************************
功能：设置为掉电模式
**************************************************/
void PD_Mode(void)
{
	RFCE=0;
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0c);   	// PWR_UP=0
}
/**************************************************
功能：设置为发射模式
**************************************************/
void TX_Mode(void)
{
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);     			// 上电, CRC为2 bytes,接收模式,允许RX_DR产生中断
  	SPI_Write_Buf(WR_TX_PLOAD, id_buf, TX_PLOAD_WIDTH); // 写数据到FIFO
	RFCE=1;												// 启动发射
	delay(10);											// 发射脉冲
	RFCE=0;												
}
/**************************************************
功能：RF初始化
**************************************************/
void rf_init(void)
{
  	RFCE = 0;                                   		// RF关闭
  	RFCKEN = 1;                                 		// 启动RF时钟
  	RF = 1;                                     		// 允许RF中断

	delay(1000);

  	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    	// 设置发射地址长度
  	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); 	// 设置接收地址长度

  	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      			// 启动自动应答功能
  	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  			// PIPE0接收数据
//  	SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x1a); 			// 自动重传10次
  	SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x10); 			// 自动重传 0 次

  	SPI_RW_Reg(WRITE_REG + RF_CH, 40);        			// RF频率2440MHz
  	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x0f);   			// 发射功率0dBm, 传输速率2Mbps,
  	SPI_RW_Reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH); 	// PIPE0 接收数据包长度			
}
/**************************************************
功能：RF中断服务程序
**************************************************/
void RF_IRQ(void) interrupt INTERRUPT_RFIRQ
{
	sta=SPI_Read(STATUS);								// 读出状态值
	SPI_RW_Reg(WRITE_REG+STATUS,0x70);					// 清除所有中断标志 
}

/**************************************************
功能：RTC2初始化
**************************************************/
void rtc2_init(void)
{
	CLKLFCTRL=0x01;									   	// 使用RC 32KHz时钟
	RTC2CMP0=0xff;										// 定时2秒
	RTC2CMP1=0xff;
	RTC2CON=0x06;										// 比较模式
	WUIRQ=1;											// 允许TICK中断
}
/**************************************************
功能：启动RTC2
**************************************************/
void rtc2_on(void)
{
	RTC2CON |=0x01;									
}
/**************************************************
功能：关闭RTC2
**************************************************/
void rtc2_off(void)
{													  	
	RTC2CON &=~0x01;							   		
}
/**************************************************
功能：RTC2中断服务程序
**************************************************/
void RTC2_IRQ(void) interrupt INTERRUPT_TICK 
{
//	LED2=!LED2;	
//	P03=0;
/*
	P0=0x00;
	LED2=1;
	delay(100);
	LED2=0;
  */
	mFlagRTC=1;

}              				
/**************************************************
功能:I/O口初始化
**************************************************/
void io_init(void)
{
  	P0DIR = p0dir;							   	// 设定I/O口输入输出
  	P1DIR = p1dir;					

	LED1=LED2=LED3=0;							// 灯全亮
//	delay(1000);
//	LED1=LED2=LED3=1;							// 灯全灭         
         
}  

void RF_TX_BUF(void)
{
	sta = 0;
	TX_Mode();								// 发射数据
	while (!(TX_DS|MAX_RT));				// 等待发射结束
	SPI_RW_Reg(FLUSH_TX,0);	
	SPI_RW_Reg(WRITE_REG+STATUS,0xFF);
	sta= 0;
}             				
 


/**************************************************
功能：主程序
**************************************************/
void main(void)
{
	unsigned char tCnt=0;
	unsigned char tIndex=0;
	unsigned char i;

	io_init();									// I/O口初始化
	rf_init();									// RF初始化                            
  	EA=1;                                       // 允许中断	 
	rtc2_init();								// RTC2初始化
	LED1=LED2=LED3=0;							// 灯全亮
												
	while(1)
	{
		rtc2_off();							   	// 关RTC2 
		if (mFlagRTC)
		{
			mFlagRTC=0;
			tCnt++;
			if (tCnt>=2)
			{
				P0DIR = 0xf0;
				tCnt=0;
				tIndex ++;
				tIndex &= 0x07;
				P0=0x00;												  
				LED2=1;
				delay(100);
				LED2=0;
/*
				id_buf[8]=0x1e + tIndex;
				TX_Mode();								// 发射数据
				while (!(TX_DS|MAX_RT));				// 等待发射结束
				sta = 0;
  */
//				id_buf[8] = 0x1e+tIndex;	 
				id_buf[8] = 0x2f;
				id_buf[9] = 0x1e+tIndex;
				id_buf[10] = 0x27;
				id_buf[11] = 0x1f;
				id_buf[12] = 0x30;
				id_buf[13] = 0x28;
//				id_buf[14] = 0x28;
				RF_TX_BUF();

				for (i=8; i<=14; i++)
				{
					id_buf[i] = 0x00;
				}
				RF_TX_BUF();
				PD_Mode();								// 关RF
			}
		}

		rtc2_on();								// 开RTC2
//		P0DIR = 0xff;
//		P1DIR = 0xff;

		PWRDWN = 0x04;							// 进入低功耗模式，等待RTC2唤醒
		delay(10);
	}
}              				


