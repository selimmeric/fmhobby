/*深圳云佳科技无线应用，应用无限*/
/*********************************************************************
功能:
	RFID TARGET发射程序
说明:
	本程序采用定时2秒发射ID一次,发射完毕进入低功耗模式,低功耗电流3uA
	ID长度为6 byte
**********************************************************************/
#include <reg24le1.h>
#include <stdint.h>
#include "API.h"
//#define p0dir	0xf0
//#define p1dir	0x02
#define INTERRUPT_RFIRQ	9
#define INTERRUPT_TICK 	13  
#define TX_ADR_WIDTH    5   					// RF收发地址共5 bytes 
#define TX_PLOAD_WIDTH  20  					// 数据包长度为20 bytes
uint8_t const TX_ADDRESS[TX_ADR_WIDTH]  = {0x34,0x56,0x78,0x90,0x12}; // 定义RF收发地址
uint8_t data id_buf[TX_PLOAD_WIDTH]={0x01,0xe2,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t bdata sta;
sbit	RX_DR	=sta^6;
sbit	TX_DS	=sta^5;
sbit	MAX_RT	=sta^4;
//============================================
#define P0_SLEEP_STATE      0x13
#define P1_SLEEP_STATE      0x1C
#define ROW0        P11
#define ROW1        P10
#define ROW2        P07
#define ROW3        P06
#define ROW4        P16

#define COL0        P13
#define COL0_PIN    3 //P1.3
#define COL0_PDIR   P1DIR
#define COL1        P12
#define COL1_PIN    2 //P1.2
#define COL1_PDIR   P1DIR
#define COL2        P01
#define COL2_PIN    1 //P0.1
#define COL2_PDIR   P0DIR
#define COL3        P00
#define COL3_PIN    0 //P0.0
#define COL3_PDIR   P0DIR

#define LED0		P03
#define LED1		P02		


#define COL0_SET()    do { COL0_PDIR &= ~(1<<COL0_PIN); COL0=1; }while(0)
#define COL1_SET()    do { COL1_PDIR &= ~(1<<COL1_PIN); COL1=1; }while(0)
#define COL2_SET()    do { COL2_PDIR &= ~(1<<COL2_PIN); COL2=1; }while(0)
#define COL3_SET()    do { COL3_PDIR &= ~(1<<COL3_PIN); COL3=1; }while(0)

#define COL0_CLR()    do { COL0=0; COL0_PDIR |= (1<<COL0_PIN); }while(0)
#define COL1_CLR()    do { COL1=0; COL1_PDIR |= (1<<COL1_PIN); }while(0)
#define COL2_CLR()    do { COL2=0; COL2_PDIR |= (1<<COL2_PIN); }while(0)
#define COL3_CLR()    do { COL3=0; COL3_PDIR |= (1<<COL3_PIN); }while(0)


static unsigned char mTFlag=0;
static unsigned char mFlag1S=0;
uint32_t mKeyMatrix[2];
unsigned char key_flag=0;

static uint8_t read_rows_ex(void);

void timer0_ISR (void) interrupt 1
{
	static unsigned char tCnt1S=0;
	static unsigned char tKey[4];
	unsigned char i;

	// 16MHz/12 = 0.75uS
	// 16bits counter 0.75uS * 65536 = 49.152mS

	// 13bits counter 0.75uS * 8 * 1024 = 6.144mS
	// 6.144mS * 8 = 49.152mS

	tCnt1S ++;
	if (tCnt1S >=17)
	{
		mFlag1S = 1;		// Set Timer Flag 100mS	
		tCnt1S = 0;
		mKeyMatrix[1]=mKeyMatrix[0];
		mKeyMatrix[0] = 0;
		for (i=0; i<=3; i++)
		{
			mKeyMatrix[0] << 5;
			mKeyMatrix[0] += tKey[i];
		}		
	}

	// Scan KeyMatrix
	switch (tCnt1S)
	{
		case 0:
			tKey[0] = 0;
			tKey[1] = 0;
			tKey[2] = 0;
			tKey[3] = 0;
			COL2_SET(); 
			break;
		case 1:
			tKey[0] =  read_rows_ex();
		    COL2_CLR();
			COL3_SET();			 
			break;
		case 2:
			tKey[1] =  read_rows_ex();
		    COL3_CLR();
			COL0_SET();			 
			break;
		case 3:
			tKey[2] =  read_rows_ex();
		    COL0_CLR();
			COL1_SET();			 
			break;
		case 4:
			tKey[3] =  read_rows_ex();
 		    COL1_CLR();
			break;		
		default:

			COL0_CLR();
			COL1_CLR();
			COL2_CLR();
			COL3_CLR();
			break;	
	}
		
}


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
//============================================
void keyinit(void)
{
  P0=0;
  P1=0;
//P0DIR=0xec;	//  
  P0DIR=0xc0;	//  0b11000000
  P1DIR=0x43;	//  0b01000011
 // P0CON = 0x70;
 // P0CON = 0x71;
 // P0CON = 0x72;
 // P0CON = 0x73;
 // P0CON = 0x74;
 // P0CON = 0x75;
  P1CON = 0x31;         // P1.1 - Dig in.buf w/ pull down res
  P1CON = 0x30;         // P1.0 - Dig in.buf w/ pull down res
  P1CON = 0x36;         // P1.6 - Dig in.buf w/ pull down res

  P0CON = 0x37;         // P0.7 - Dig in.buf w/ pull down res
  P0CON = 0x36;         // P0.6 - Dig in.buf w/ pull down res
 // P1CON = 0x72;
 // P1CON = 0x73;
 // P1CON = 0x74;
 // P1CON = 0x75;
 
}

static uint8_t read_rows_ex(void)
{
  return  (uint8_t)(ROW0 | ((uint8_t)ROW1 << 1) | ((uint8_t)ROW2 << 2) | ((uint8_t)ROW3 << 3) | ((uint8_t)ROW4 << 4));
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
/**************************************************
功能：设置为发射模式
**************************************************/
void TX_Mode(void)
{
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);     			// 上电, CRC为2 bytes,接收模式,允许RX_DR产生中断
  	SPI_Write_Buf(WR_TX_PLOAD, id_buf, TX_PLOAD_WIDTH); // 写数据到FIFO
	RFCE=1;												// 启动发射
	delay(100);											// 发射脉冲
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
	SPI_RW_Reg(WRITE_REG+STATUS,0x70);					// 清除所有中断标志 
}

             				
 
/**************************************************
功能：主程序
**************************************************/
void main(void)
{
    
	static unsigned char tCnt=0;
	rf_init();									// RF初始化
	keyinit();                            

/*--------------------------------------
Set Timer0 for 16-bit timer mode.  The
timer counts to 65535, overflows, and
generates an interrupt.

Set the Timer0 Run control bit.
--------------------------------------*/
TMOD = (TMOD & 0xF0) | 0x00;  /* Set T/C0 Mode 13bit timer mode*/
ET0 = 1;                      /* Enable Timer 0 Interrupts */
TR0 = 1;                      /* Start Timer 0 Running */
EA = 1;                       /* Global Interrupt Enable */

// 	EA=1;                                       // 允许中断	 
												
	while (1)
	{

		if (mFlag1S==1)				// 100mS flag
		{
			tCnt ++; 					
			mFlag1S = 0;
			if (tCnt>=5)
			{ 
				tCnt = 0;
				LED1 = !LED1;
			}

			if (mKeyMatrix[0] != mKeyMatrix[1])	
			{
				LED0 = !LED0;			
			}


					
		}
	}


 	while(0)
	{
	    if(key_flag)
		{
		  tCnt ^=01;
		  LED0 = tCnt;
		  key_flag=0;
		  sta = 0;
		  TX_Mode();								// 发射数据
		  while (!(TX_DS|MAX_RT));				// 等待发射结束
		  SPI_RW_Reg(FLUSH_TX,0);	
	      SPI_RW_Reg(WRITE_REG+STATUS,0xFF);
		  sta = 0;
		  delay(100);
		}
	}
}              				
