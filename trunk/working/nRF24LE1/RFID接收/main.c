/*深圳云佳科技无线应用，应用无限*/
/*********************************************************************
功能:
	RFID READER接收程序
说明:
	收到数据显示灯闪烁一下，并通过串口上显示ID值
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

sbit LED3 = P0^1;                               // 1/0=灭/亮
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

#define TX_ADR_WIDTH    5   					// RF收发地址共5 bytes 
#define TX_PLOAD_WIDTH  20  					// 数据包长度为20 bytes

uint8_t const TX_ADDRESS[TX_ADR_WIDTH]  = {0x34,0x56,0x78,0x90,0x12}; // 定义RF收发地址

uint8_t data rx_buf[TX_PLOAD_WIDTH];

uint8_t bdata sta;
sbit	RX_DR	=sta^6;
sbit	TX_DS	=sta^5;
sbit	MAX_RT	=sta^4;
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
功能：读RF寄存器多字节数据到缓冲区
**************************************************/
uint8_t SPI_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
	uint8_t status,byte_ctr;

  	RFCSN = 0;                    		
  	status = SPI_RW(reg);       				// 选择RF寄存器

  	for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
    	pBuf[byte_ctr] = SPI_RW(0);    			// 连接读出数据

  	RFCSN = 1;                          

  	return(status);                    			// 返回RF状态值
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
功能：设置为接收模式
**************************************************/
void RX_Mode(void)
{
	RFCE=0;
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);   	// 上电, CRC为2 bytes,接收模式,允许RX_DR产生中断
  	RFCE = 1; 									// 启动接收模式
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
  	SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x1a); 			// 自动重传10次
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

	if(RX_DR)									
	{
		SPI_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);// 读出FIFO的数据
		SPI_RW_Reg(FLUSH_RX,0);							// 清除RX的FIFO
	}

	SPI_RW_Reg(WRITE_REG+STATUS,0x70);					// 清除所有中断标志 
}
/**************************************************
功能：串口初始化子程序
说明：波特率19.2K，使用内部波特率发生器
**************************************************/
void uart_init(void)
{
    ES0 = 0;                      				// 关UART0中断
    REN0 = 1;                     				// 允许接收
    SM0 = 0;                      				// 串口模式1，8bit可变波特率
    SM1 = 1;                   
    PCON |= 0x80;                 				// SMOD = 1
    ADCON |= 0x80;                				// 选择内部波特率发生器

    S0RELL = 0xe6;                				// 波特率19.2K(十进制998=十六进制0x03e6)
    S0RELH = 0x03;
    TI0 = 0;					  				// 清发送完成标志
	S0BUF=0x99;					  				// 送初值
}
/**************************************************
功能：向串口发送1 byte数据
**************************************************/
void uart_putchar(uint8_t x)
{
	while (!TI0);								// 等待发送完成
	TI0=0;										// 清发送完成标志
	S0BUF=x;									// 发送数据
}
/**************************************************
功能:I/O口初始化
**************************************************/
void io_init(void)
{
  	P0DIR = p0dir;							   	// 设定I/O口输入输出
  	P1DIR = p1dir;					

	LED1=LED2=LED3=0;							// 灯全亮
	delay(1000);
	LED1=LED2=LED3=1;							// 灯全灭                  
}  
/**************************************************
功能：主程序
**************************************************/
void main(void)
{
	io_init();									// I/O口初始化
	uart_init();                           		// 串口初始化 
	rf_init();									// RF初始化                            
  	EA=1;                                       // 允许中断

	RX_Mode();									//进入接收模式

	uart_putchar ('h');
	uart_putchar ('e');
	uart_putchar ('l');
	uart_putchar ('l');
	uart_putchar ('o');

	while(1)
	{
		if(RX_DR)								// 数据已收到
		{
			sta=0;

			LED2=0;

			uart_putchar(rx_buf[0]);			// 显示所有ID值
			uart_putchar(rx_buf[1]);
			uart_putchar(rx_buf[2]);
			uart_putchar(rx_buf[3]);
			uart_putchar(rx_buf[4]);
			uart_putchar(rx_buf[5]);

			delay(1000);

			LED1=LED2=LED3=1;					// 灯全灭
		}
	}	
}              				
              				             
              				

