/*�����ƼѿƼ�����Ӧ�ã�Ӧ������*/
/*********************************************************************
����:
	RFID TARGET�������
˵��:
	��������ö�ʱ2�뷢��IDһ��,������Ͻ���͹���ģʽ,�͹��ĵ���3uA
	ID����Ϊ6 byte
**********************************************************************/


#include "reg24le1.h"
#include <stdint.h>
#include "API.h"

//#define	PIN24
#define	PIN32
//#define	PIN48

#ifdef 	PIN24
sbit S3	 = P0^6;                               	// 1/0=δ��/����
sbit S2  = P0^4;                               	// 1/0=δ��/����
sbit S1  = P0^3;                               	// 1/0=δ��/����

sbit LED3 = P0^2;                         		// 1/0=��/��
sbit LED2 = P0^1;                               // 1/0=��/��
sbit LED1 = P0^0;                               // 1/0=��/��

#define p0dir	0xd8
#define p1dir	0xff
#endif

#ifdef 	PIN32
sbit S3	 = P1^4;                               	// 1/0=δ��/����
sbit S2  = P1^3;                               	// 1/0=δ��/����
sbit S1  = P1^2;                               	// 1/0=δ��/����

sbit LED3 = P0^4;                               // 1/0=��/��
sbit LED2 = P0^2;                               // 1/0=��/��
sbit LED1 = P0^0;                               // 1/0=��/��

#define p0dir	0xf0
#define p1dir	0xff
#endif

#ifdef 	PIN48
sbit S3	 = P0^7;                               	// 1/0=δ��/����
sbit S2  = P0^6;                               	// 1/0=δ��/����
sbit S1  = P0^5;                               	// 1/0=δ��/����

sbit LED3 = P0^2;                               // 1/0=��/��
sbit LED2 = P0^1;                               // 1/0=��/��
sbit LED1 = P0^0;                               // 1/0=��/��

#define p0dir	0xf0
#define p1dir	0x02
#endif

#define INTERRUPT_RFIRQ	9
#define INTERRUPT_TICK 	13  

#define TX_ADR_WIDTH    5   					// RF�շ���ַ��5 bytes 
#define TX_PLOAD_WIDTH  20  					// ���ݰ�����Ϊ20 bytes

//uint8_t const TX_ADDRESS[TX_ADR_WIDTH]  = {0x34,0x56,0x78,0x90,0x12}; // ����RF�շ���ַ
//uint8_t data id_buf[TX_PLOAD_WIDTH]={0xff, 0x01, 0x02, 0x03, 0x04, 0x05};

uint8_t const TX_ADDRESS[TX_ADR_WIDTH]  = {0x34,0x56,0x78,0x90,0x12}; // ����RF�շ���ַ
uint8_t data id_buf[TX_PLOAD_WIDTH]={0x01,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};



uint8_t bdata sta;
sbit	RX_DR	=sta^6;
sbit	TX_DS	=sta^5;
sbit	MAX_RT	=sta^4;


unsigned char mFlagRTC=0; 

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
void PD_Mode(void)
{
	RFCE=0;
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0c);   	// PWR_UP=0
}
/**************************************************
���ܣ�����Ϊ����ģʽ
**************************************************/
void TX_Mode(void)
{
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);     			// �ϵ�, CRCΪ2 bytes,����ģʽ,����RX_DR�����ж�
  	SPI_Write_Buf(WR_TX_PLOAD, id_buf, TX_PLOAD_WIDTH); // д���ݵ�FIFO
	RFCE=1;												// ��������
	delay(10);											// ��������
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
//  	SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x1a); 			// �Զ��ش�10��
  	SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x10); 			// �Զ��ش� 0 ��

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
���ܣ�����RTC2
**************************************************/
void rtc2_on(void)
{
	RTC2CON |=0x01;									
}
/**************************************************
���ܣ��ر�RTC2
**************************************************/
void rtc2_off(void)
{													  	
	RTC2CON &=~0x01;							   		
}
/**************************************************
���ܣ�RTC2�жϷ������
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
����:I/O�ڳ�ʼ��
**************************************************/
void io_init(void)
{
  	P0DIR = p0dir;							   	// �趨I/O���������
  	P1DIR = p1dir;					

	LED1=LED2=LED3=0;							// ��ȫ��
//	delay(1000);
//	LED1=LED2=LED3=1;							// ��ȫ��         
         
}  

void RF_TX_BUF(void)
{
	sta = 0;
	TX_Mode();								// ��������
	while (!(TX_DS|MAX_RT));				// �ȴ��������
	SPI_RW_Reg(FLUSH_TX,0);	
	SPI_RW_Reg(WRITE_REG+STATUS,0xFF);
	sta= 0;
}             				
 


/**************************************************
���ܣ�������
**************************************************/
void main(void)
{
	unsigned char tCnt=0;
	unsigned char tIndex=0;
	unsigned char i;

	io_init();									// I/O�ڳ�ʼ��
	rf_init();									// RF��ʼ��                            
  	EA=1;                                       // �����ж�	 
	rtc2_init();								// RTC2��ʼ��
	LED1=LED2=LED3=0;							// ��ȫ��
												
	while(1)
	{
		rtc2_off();							   	// ��RTC2 
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
				TX_Mode();								// ��������
				while (!(TX_DS|MAX_RT));				// �ȴ��������
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
				PD_Mode();								// ��RF
			}
		}

		rtc2_on();								// ��RTC2
//		P0DIR = 0xff;
//		P1DIR = 0xff;

		PWRDWN = 0x04;							// ����͹���ģʽ���ȴ�RTC2����
		delay(10);
	}
}              				


