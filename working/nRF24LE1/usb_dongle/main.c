/*�����ƼѿƼ�����Ӧ�ã�Ӧ������*/
#include <Nordic\reg24lu1.h>
#include <stdint.h>
#include <intrins.h>
#include <stdbool.h>
#include <string.h>
#include "hal_usb.h"
#include "hal_usb_hid.h"
//#include "usb_map.h"
//#include "app_params.h"
#include "API.h"
//-----------------------------------------------------------------------------
// Application state functions and variables
//-----------------------------------------------------------------------------
typedef enum
{
  APP_ACTIVE,
  APP_SUSPEND,
} app_states_t;

extern code usb_string_desc_templ_t g_usb_string_desc;
static xdata uint8_t rx_pipe, rx_data_length;
static xdata app_states_t app_state;
void app_enter_active_state();
void app_enter_suspend_state(void);
void app_transfer_to_usb(uint8_t* report1, uint8_t* report2);

//-----------------------------------------------------------------------------
// USB callback function prototypes
//-----------------------------------------------------------------------------
static hal_usb_dev_req_resp_t device_req_cb(hal_usb_device_req* req, uint8_t** data_ptr, uint8_t* size) reentrant;
static void suspend_cb(void) reentrant;
static void resume_cb() reentrant;
static void reset_cb() reentrant;
static uint8_t ep_1_in_cb(uint8_t *adr_ptr, uint8_t* size) reentrant;

//-----------------------------------------------------------------------------
// Main dongle application
//-----------------------------------------------------------------------------
static xdata bool app_pending_usb_write = false;
static xdata bool app_endpoint_cleared = true;
#define NRFR_LENGTH_REPORT_1 5
#define NRFR_LENGTH_REPORT_2 9
static xdata uint16_t app_keep_alive_cnt= 0;
void app_maintain_keep_alive();
//=======================================
#define INTERRUPT_RFIRQ	9
#define TX_ADR_WIDTH    5   					// RF�շ���ַ��5 bytes 
#define TX_PLOAD_WIDTH  20  					// ���ݰ�����Ϊ20 bytes
#define CE_LOW() do { RFCE = 0; } while(0)
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
  RFDAT = value;
  RFSPIF = 0;     // ! IMPORTANT ! Clear RF SPI ready flag
                  // after data written to RFDAT..
  while(!RFSPIF); // wait for byte transfer finished
    ;
  return RFDAT;   // return SPI read value
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
	delay(10);	
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
//=======================================
void main(void)
{   
  uint8_t remote_id1_packet[NRFR_LENGTH_REPORT_1], remote_id2_packet[NRFR_LENGTH_REPORT_2];
  uint8_t i;
//=======================================
  CLKCTL=0;
  RFCE=0;
  RFCTL=0x10;
  rf_init();									// RF��ʼ��
//=======================================
  hal_usb_init(true, device_req_cb, reset_cb, resume_cb, suspend_cb);   
  hal_usb_endpoint_config(0x81, 32, ep_1_in_cb); // Configure 32 byte IN endpoint 1
  EA=1;
  app_keep_alive_cnt=0;
  RX_Mode();									//�������ģʽ
  while(true)                                                                               
  {
     	  if(RX_DR)
     	  {
			sta=0;
			for(i=0;i<5;i++)
			{
			   remote_id1_packet[i]=rx_buf[i];
			}
			for(i=0;i<9;i++)
			{
			   remote_id2_packet[i]=rx_buf[i+5];
			}
			app_transfer_to_usb(remote_id1_packet, remote_id2_packet);
			app_keep_alive_cnt=1;
			delay(100);
          }
		  app_maintain_keep_alive();        
  }
}                          

//-----------------------------------------------------------------------------
// State configuration functions
//-----------------------------------------------------------------------------

void app_enter_active_state()
{
  app_state = APP_ACTIVE;
  app_pending_usb_write = false;
}

void app_enter_suspend_state(void)
{  
  app_state = APP_SUSPEND;
  app_pending_usb_write = false;
  WUCONF = 0x22;                              // Enable wakeup on RTC and USBWU
}
void app_maintain_keep_alive()
{
  uint8_t i;
  uint8_t remote_id1_packet[NRFR_LENGTH_REPORT_1], remote_id2_packet[NRFR_LENGTH_REPORT_2];      
  if(app_keep_alive_cnt > 0 && !app_endpoint_cleared)
  {
    for(i = 1; i < NRFR_LENGTH_REPORT_1; i++)
    {
      remote_id1_packet[i] = 0;
    }
  
    for(i = 1; i < NRFR_LENGTH_REPORT_2; i++)
    {
      remote_id2_packet[i] = 0;
    }                                                                     

    remote_id1_packet[0] = 0x01;
    remote_id2_packet[0] = 0x02;

    app_transfer_to_usb(remote_id1_packet, remote_id2_packet);
    app_keep_alive_cnt = 0;    
  }                     
}
void app_transfer_to_usb(uint8_t* report1, uint8_t* report2)
{    
  uint8_t i;
  app_endpoint_cleared = true;

  for(i = 1; i < NRFR_LENGTH_REPORT_1; i++)
  {
    if(*(report1 + i) != 0)
    {
      app_endpoint_cleared = false;
      break;  
    }
  }
  
  for(i = 1; i < NRFR_LENGTH_REPORT_2; i++)
  {
    if(*(report2 + i) != 0)
    {
      app_endpoint_cleared = false;
      break;  
    }
  }

  while(app_pending_usb_write)
  ;
  app_pending_usb_write = true;                               
  hal_usb_send_data(0x80 | 1, report1, NRFR_LENGTH_REPORT_1);
              
  while(app_pending_usb_write)
  ;
  app_pending_usb_write = true;
  hal_usb_send_data(0x80 | 1, report2, NRFR_LENGTH_REPORT_2);
}
//-----------------------------------------------------------------------------
// USB callback functions
//-----------------------------------------------------------------------------

static void suspend_cb(void) reentrant
{
  USBSLP = 1; // Disable USB clock (auto clear)  
  app_enter_suspend_state();
}

static void resume_cb() reentrant
{
  app_enter_active_state();
}

static void reset_cb() reentrant
{
  app_enter_active_state();
}

static uint8_t ep_1_in_cb(uint8_t *adr_ptr, uint8_t* size) reentrant
{
  app_pending_usb_write = false;
  return 0x60; // NAK
  adr_ptr = adr_ptr;
  size = size;
}

static hal_usb_dev_req_resp_t device_req_cb(hal_usb_device_req* req, uint8_t** data_ptr, uint8_t* size) reentrant
{
  hal_usb_dev_req_resp_t retval;

  if( hal_usb_hid_device_req_proc(req, data_ptr, size, &retval) == true ) 
  {
      // The request was processed with the result stored in the retval variable
      return retval;
  }
  else
  {
    return STALL;
  }

  return STALL;
}