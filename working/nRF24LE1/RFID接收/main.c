/*�����ƼѿƼ�����Ӧ�ã�Ӧ������*/
/*********************************************************************
����:
	RFID READER���ճ���
˵��:
	�յ�������ʾ����˸һ�£���ͨ����������ʾIDֵ
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

sbit LED3 = P0^1;                               // 1/0=��/��
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

#define TX_ADR_WIDTH    5   					// RF�շ���ַ��5 bytes 
#define TX_PLOAD_WIDTH  20  					// ���ݰ�����Ϊ20 bytes

uint8_t const TX_ADDRESS[TX_ADR_WIDTH]  = {0x34,0x56,0x78,0x90,0x12}; // ����RF�շ���ַ

uint8_t data rx_buf[TX_PLOAD_WIDTH];

uint8_t bdata sta;
sbit	RX_DR	=sta^6;
sbit	TX_DS	=sta^5;
sbit	MAX_RT	=sta^4;
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
���ܣ���RF�Ĵ������ֽ����ݵ�������
**************************************************/
uint8_t SPI_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
	uint8_t status,byte_ctr;

  	RFCSN = 0;                    		
  	status = SPI_RW(reg);       				// ѡ��RF�Ĵ���

  	for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
    	pBuf[byte_ctr] = SPI_RW(0);    			// ���Ӷ�������

  	RFCSN = 1;                          

  	return(status);                    			// ����RF״ֵ̬
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
void RX_Mode(void)
{
	RFCE=0;
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);   	// �ϵ�, CRCΪ2 bytes,����ģʽ,����RX_DR�����ж�
  	RFCE = 1; 									// ��������ģʽ
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

	if(RX_DR)									
	{
		SPI_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);// ����FIFO������
		SPI_RW_Reg(FLUSH_RX,0);							// ���RX��FIFO
	}

	SPI_RW_Reg(WRITE_REG+STATUS,0x70);					// ��������жϱ�־ 
}
/**************************************************
���ܣ����ڳ�ʼ���ӳ���
˵����������19.2K��ʹ���ڲ������ʷ�����
**************************************************/
void uart_init(void)
{
    ES0 = 0;                      				// ��UART0�ж�
    REN0 = 1;                     				// �������
    SM0 = 0;                      				// ����ģʽ1��8bit�ɱ䲨����
    SM1 = 1;                   
    PCON |= 0x80;                 				// SMOD = 1
    ADCON |= 0x80;                				// ѡ���ڲ������ʷ�����

    S0RELL = 0xe6;                				// ������19.2K(ʮ����998=ʮ������0x03e6)
    S0RELH = 0x03;
    TI0 = 0;					  				// �巢����ɱ�־
	S0BUF=0x99;					  				// �ͳ�ֵ
}
/**************************************************
���ܣ��򴮿ڷ���1 byte����
**************************************************/
void uart_putchar(uint8_t x)
{
	while (!TI0);								// �ȴ��������
	TI0=0;										// �巢����ɱ�־
	S0BUF=x;									// ��������
}
/**************************************************
����:I/O�ڳ�ʼ��
**************************************************/
void io_init(void)
{
  	P0DIR = p0dir;							   	// �趨I/O���������
  	P1DIR = p1dir;					

	LED1=LED2=LED3=0;							// ��ȫ��
	delay(1000);
	LED1=LED2=LED3=1;							// ��ȫ��                  
}  
/**************************************************
���ܣ�������
**************************************************/
void main(void)
{
	io_init();									// I/O�ڳ�ʼ��
	uart_init();                           		// ���ڳ�ʼ�� 
	rf_init();									// RF��ʼ��                            
  	EA=1;                                       // �����ж�

	RX_Mode();									//�������ģʽ

	uart_putchar ('h');
	uart_putchar ('e');
	uart_putchar ('l');
	uart_putchar ('l');
	uart_putchar ('o');

	while(1)
	{
		if(RX_DR)								// �������յ�
		{
			sta=0;

			LED2=0;

			uart_putchar(rx_buf[0]);			// ��ʾ����IDֵ
			uart_putchar(rx_buf[1]);
			uart_putchar(rx_buf[2]);
			uart_putchar(rx_buf[3]);
			uart_putchar(rx_buf[4]);
			uart_putchar(rx_buf[5]);

			delay(1000);

			LED1=LED2=LED3=1;					// ��ȫ��
		}
	}	
}              				
              				             
              				

