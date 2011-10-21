/*�����ƼѿƼ�����Ӧ�ã�Ӧ������*/
/*********************************************************************
����:
	RFID TARGET�������
˵��:
	��������ö�ʱ2�뷢��IDһ��,������Ͻ���͹���ģʽ,�͹��ĵ���3uA
	ID����Ϊ6 byte
**********************************************************************/
#include <reg24le1.h>
#include <stdint.h>
#include "API.h"
//#define p0dir	0xf0
//#define p1dir	0x02
#define INTERRUPT_RFIRQ	9
#define INTERRUPT_TICK 	13  
#define TX_ADR_WIDTH    5   					// RF�շ���ַ��5 bytes 
#define TX_PLOAD_WIDTH  20  					// ���ݰ�����Ϊ20 bytes
uint8_t const TX_ADDRESS[TX_ADR_WIDTH]  = {0x34,0x56,0x78,0x90,0x12}; // ����RF�շ���ַ
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




uint8_t temp=0;
unsigned char key_flag=0;
/**************************************************
���ܣ���ʱ
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
/*void key(void)
{
  P0=0;
  P1=0;
  P0DIR = 0xc0;
  P1DIR = 0x43;
  COL0=1;
  P1=0;
  COL0=1;
  P1=0;
  COL0=1;
  P1=0;
}*/
static uint8_t read_rows(void)
{
  return  (uint8_t)(ROW0 | ((uint8_t)ROW1 << 1) | ((uint8_t)ROW2 << 2) | ((uint8_t)ROW3 << 3) | ((uint8_t)ROW4 << 4));
}
void key_scan(void)
{
    uint8_t i;
	//uint8_t temp1;
    id_buf[0]=0x01;
	id_buf[1]=0x00;
	id_buf[2]=0x00;
	id_buf[3]=0x00;
	id_buf[4]=0x00;
	id_buf[5]=0x02;
	for(i=6;i<14;i++)
	{
	  id_buf[i]=0x00;
	}
    COL2_SET();         //ɨ���һ��
	temp = read_rows();
	delay(500);
	if(temp==0x01)		  // key 1
	 {
	   key_flag=1;
	   id_buf[8]=0x1e;
	   delay(500);
	 }
	if(temp==0x02)		  //key 2
	 {
	   key_flag=1;
	   id_buf[8]=0x1f;
	   delay(500);
	 }
	if(temp==0x04)		 //key 3
	 {
	   key_flag=1;
	   id_buf[8]=0x20;
	   delay(500);
	 }
	if(temp==0x08)		 //key 4
	 {
	   key_flag=1;
	   id_buf[8]=0x21;
	   delay(500);
	 }
	if(temp==0x10)		 //key 5
	 {
	   key_flag=1;
	   id_buf[8]=0x22;
	   delay(500);
	 }
    COL2_CLR();

	COL3_SET();			 //ɨ��ڶ���
	temp = read_rows();
	delay(500);
	if(temp==0x01)		  // key 6
	 {							
	   key_flag=1;
	   id_buf[8]=0x23;
	   delay(500);
	 }
	if(temp==0x02)		  //key 7
	 {
	   key_flag=1;
	   id_buf[8]=0x24;
	   delay(500);
	 }
	if(temp==0x04)		 //key 8
	 {
	   key_flag=1;
	   id_buf[8]=0x25;
	   delay(500);
	 }
	if(temp==0x08)		 //key 9
	 {
	   key_flag=1;
	   id_buf[8]=0x26;
	   delay(500);
	 }
	if(temp==0x10)		 //key 0
	 {
	   key_flag=1;
	   id_buf[8]=0x27;
	   delay(500);
	 }
   COL3_CLR();
   
   COL0_SET();            //ɨ�������
   temp = read_rows();
   delay(500);
	if(temp==0x01)		  // key mute
	 {							
	   key_flag=1;
	   id_buf[1]=0xe2;
	   delay(500);
	 }
	if(temp==0x02)		  //key close
	 {
	   key_flag=1;
	   id_buf[3]=0x02;
	   delay(500);
	 }
	if(temp==0x04)		 //key stop
	 {
	   key_flag=1;
	   id_buf[1]=0xB7;
	   delay(500);
	 }
	if(temp==0x08)		 //key up
	 {
	   key_flag=1;
	   id_buf[1]=0xB6;
	   delay(500);
	 }
	if(temp==0x10)		 //key play
	 {
	   key_flag=1;
	   id_buf[1]=0xCD;
	   delay(500);
	 }
   COL0_CLR();

   COL1_SET() ;       //ɨ�������
   	temp = read_rows();
	delay(500);
	if(temp==0x01)		  // key dowm
	 {							
	   key_flag=1;
	   id_buf[1]=0xb5;
	   delay(500);
	 }
	if(temp==0x02)		  //key vol+
	 {
	   key_flag=1;
	   id_buf[1]=0xe9;
	   delay(500);
	 }
	if(temp==0x04)		 //key vol-
	 {
	   key_flag=1;
	   id_buf[1]=0xea;
	   delay(500);
	 }
	if(temp==0x08)		 //key ch+
	 {
	   key_flag=1;
	   id_buf[1]=0x9c;
	   delay(500);
	 }
	if(temp==0x10)		 //key ch-
	 {
	   key_flag=1;
	   id_buf[1]=0x9d;
	   delay(500);
	 }
   COL1_CLR();

}

/**************************************************
���ܣ�Ӳ��SPI��д
**************************************************/
uint8_t SPI_RW(uint8_t value)
{
  SPIRDAT = value;										       
  while(!(SPIRSTAT & 0x02));  					// �ȴ�SPI�������
  return SPIRDAT;             					// ���ض���ֵ
}
/**************************************************
���ܣ�дRF�Ĵ�������RF״ֵ̬
**************************************************/
uint8_t SPI_RW_Reg(uint8_t reg, uint8_t value)
{
	uint8_t status;
  	RFCSN = 0;                   	
  	status = SPI_RW(reg);      					// ѡ��RF�Ĵ���
  	SPI_RW(value);             					// д������
  	RFCSN = 1;                   	
  	return(status);            					// ����RF״ֵ̬
}
/**************************************************
���ܣ���RF�Ĵ���
**************************************************/
uint8_t SPI_Read(uint8_t reg)
{
	uint8_t reg_val;
  	RFCSN = 0;                			
  	SPI_RW(reg);            					// ѡ��RF�Ĵ���
  	reg_val = SPI_RW(0);    					// ��������
  	RFCSN = 1;                			
  	return(reg_val);        					// ����RF״ֵ̬
}
/**************************************************
���ܣ��ѻ������Ķ��ֽ�����д��RF�Ĵ���
**************************************************/
uint8_t SPI_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
	uint8_t status,byte_ctr;
  	RFCSN = 0;                   		
  	status = SPI_RW(reg);    					// ѡ��RF�Ĵ���
  	for(byte_ctr=0; byte_ctr<bytes; byte_ctr++) // ����д������
    	SPI_RW(*pBuf++);
  	RFCSN = 1;                 			
  	return(status);          					// ����RF״ֵ̬
}
/**************************************************
���ܣ�����Ϊ����ģʽ
**************************************************/
/**************************************************
���ܣ�����Ϊ����ģʽ
**************************************************/
void TX_Mode(void)
{
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);     			// �ϵ�, CRCΪ2 bytes,����ģʽ,����RX_DR�����ж�
  	SPI_Write_Buf(WR_TX_PLOAD, id_buf, TX_PLOAD_WIDTH); // д���ݵ�FIFO
	RFCE=1;												// ��������
	delay(100);											// ��������
	RFCE=0;												
}
/**************************************************
���ܣ�RF��ʼ��
**************************************************/
void rf_init(void)
{
  	RFCE = 0;                                   		// RF�ر�
  	RFCKEN = 1;                                 		// ����RFʱ��
  	RF = 1;                                     		// ����RF�ж�
	delay(1000);
  	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    	// ���÷����ַ����
  	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); 	// ���ý��յ�ַ����
  	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      			// �����Զ�Ӧ����
  	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  			// PIPE0��������
  	SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x1a); 			// �Զ��ش�10��
  	SPI_RW_Reg(WRITE_REG + RF_CH, 40);        			// RFƵ��2440MHz
  	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x0f);   			// ���书��0dBm, ��������2Mbps,
  	SPI_RW_Reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH); 	// PIPE0 �������ݰ�����			
}
/**************************************************
���ܣ�RF�жϷ������
**************************************************/
void RF_IRQ(void) interrupt INTERRUPT_RFIRQ
{
	sta=SPI_Read(STATUS);								// ����״ֵ̬
	SPI_RW_Reg(WRITE_REG+STATUS,0x70);					// ��������жϱ�־ 
}

/**************************************************
���ܣ�RTC2��ʼ��
**************************************************/
void rtc2_init(void)
{
	CLKLFCTRL=0x01;									   	// ʹ��RC 32KHzʱ��
	RTC2CMP0=0xff;										// ��ʱ2��
	RTC2CMP1=0xff;
	RTC2CON=0x06;										// �Ƚ�ģʽ
	WUIRQ=1;											// ����TICK�ж�
}
/**************************************************
���ܣ�RTC2�жϷ������
**************************************************/
void RTC2_IRQ(void) interrupt INTERRUPT_TICK 
{
//	LED2=!LED2;										   	
}              				
 
/**************************************************
���ܣ�������
**************************************************/
void main(void)
{
    
	static char tCnt=0;
    delay(1000);
//	key();
	rf_init();									// RF��ʼ��
	keyinit();                            
  	EA=1;                                       // �����ж�	 
	rtc2_init();
												// RTC2��ʼ��

	LED0 = 1;
	LED1 = 0;
	while(1)
	{
	    key_scan();
	    if(key_flag)
		{
		  tCnt ^=01;
		  LED0 = tCnt;
		  key_flag=0;
		  sta = 0;
		  TX_Mode();								// ��������
		  while (!(TX_DS|MAX_RT));				// �ȴ��������
		 SPI_RW_Reg(FLUSH_TX,0);	
	     SPI_RW_Reg(WRITE_REG+STATUS,0xFF);
		  sta = 0;
		  delay(100);
		}
	}
}              				


