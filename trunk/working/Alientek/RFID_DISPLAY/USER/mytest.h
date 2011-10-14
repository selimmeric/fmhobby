#ifndef __MYTEST_H
#define __MYTEST_H		
#include "sys.h"	 
#include "stdlib.h"

//#define debug_isr	
//#define debug_task_200ms
#define debug_task_1s


#define PIN_DEBUG	PCout(4)
#define	DEBUG_H()	PIN_DEBUG = 1
#define DEBUG_L()	PIN_DEBUG = 0
#define DEBUG_T()	PIN_DEBUG ^= 0x01 

#define PIN_DEBUG1	PCout(5)
#define	DEBUG1_H()	PIN_DEBUG1 = 1
#define DEBUG1_L()	PIN_DEBUG1 = 0
#define DEBUG1_T()	PIN_DEBUG1 ^= 0x01 

#define LED_R		LED0
#define LED_R_ON() 	LED_R = 0
#define LED_R_OFF()	LED_R = 1
#define	LED_G		LED1
#define LED_G_ON()	LED_G = 0
#define LED_G_OFF()	LED_G = 1
 
#define BALLAST		PCin(5)
#define FS_O		LED_G		// LED1			
#define FS_I0		PDin(2)		// Internal Flow sensor generator
#define FS_I1		PCin(11) 	// External Flow sensor 1
#define FS_I2		PCin(12)	// External Flow sensor 2
		
#define FIFO_SIZE	64
#define FIFO_PT_RD	mFIFO[FIFO_SIZE-1]
#define FIFO_PT_WR 	mFIFO[FIFO_SIZE-2]

#define LN_HEIGHT	17
#define	LN_RFID		2
#define	LN_STATUS	3
#define LN_IS		7
#define	LN_UART1	10		
#define	LN_UART2	LN_UART1 + 1	
#define LN_KEY		LN_UART1 + 2
#define LN_DEBUG	13
	
struct flow_counter
{
	unsigned char state;
	unsigned int  elapsedtime;
	unsigned char pTimer;
	unsigned int  count[3];
	unsigned char sensor[3];
};

struct flow_counterA
{
	unsigned char state;
	unsigned int  elapsedtime;
	unsigned char pTimer;
	unsigned int  fr_tmr;	//  Free Running Timer (100mS resolution)
	unsigned int  fr_cnt;	//	Free Running Counter
	unsigned int  st_cnt;	//  Start Count
	unsigned int  sp_cnt;	//  Stop Count
	unsigned char sensor;
};

struct flow_rate
{
	unsigned int  pcnt;		//	Store Previous count
	unsigned int  ccnt; 	// 	Store Current count
	unsigned int  ptmr;		//	Store Previous timer
	unsigned int  ctmr;		//	Store Current timer
	unsigned int  rate;		//  Store Flow Rate
};

struct flow_rateA
{
	unsigned int  cnt[5];	//	Store count
	unsigned int  tmr[5];	//	Store timer
	unsigned int  rate[5];	//	Store rate
};


struct flow_sensor 
{ 
	unsigned char fs_in;
	unsigned char fs_cnt;
	unsigned char fs_pcnt;
	unsigned int fs_acc_cnt;
	unsigned int fs_fr_cnt;
};  



void my_TMR_ISR(void);
void my_init(void);
void my_task(void);
void my_task_uart(void);
void my_task_sync(void);


void LCD_ShowLine(u8 x,u16 y, u8 *p);		 //显示一个字符串,16字体
void CMD_In(u8 id , u8 *p, u8 len);
void CMD_RFID(char *cmd, char **p);
void CMD_FR(char *cmd, char **par);
void CMD_STATUS(char *cmd, char **p);

u8 u_hex2dec(u8 c);
u16 u_asc2uint(u8 len, u8 *sptr); 

u16 cal_crc(u8 idata, u16 icrc);
void testRFID(void);
u8 swapByte(u8 iByte);



#endif


