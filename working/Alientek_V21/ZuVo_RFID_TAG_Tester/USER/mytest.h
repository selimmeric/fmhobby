#ifndef __MYTEST_H
#define __MYTEST_H		
#include "sys.h"	 
#include "stdlib.h"

//#define debug_isr	
//#define debug_task_200ms
//#define debug_task_1s

#define PIN_RST		PAout(8)
#define PIN_R		PAin(3)
#define PIN_G		PCin(11)
#define PIN_B		PCin(12)
#define PIN_UVL		PCin(4)
#define PIN_BUZ		PCin(5)
#define PIN_FS		PDout(2)


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
		
#define FIFO_SIZE	512
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
	

#define FS_CPL				2724					// 2724 Count = 1L
#define FS_CPG				((int)(FS_CPL * 3.785))	// Count per Gallon = 10310


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

struct flow_meter
{
	unsigned char	fs;			// Flow sensor 
	unsigned int 	acc_fc;		// Acc. Flow Count
	unsigned int	zData[5];	// Store Historical data
};


void my_TMR_ISR(void);
void my_init(void);
void my_task(void);
void my_task_uart(void);
void my_task_sync(void);
void my_task_update_io(void);

void LCD_ShowLine(u8 x,u16 y, u8 *p);		 //显示一个字符串,16字体
void CMD_In(u8 id , u8 *p, u8 len);
void CMD_RFID(char *cmd, char **p);
void CMD_FR(char *cmd, char **par);
void CMD_STATUS(char *cmd, char **p);
void CMD_DO(char *cmd, char **line);		// Data Output			
void CMD_DI(char *cmd, char **line); 		// Data Input
void CMD_P(char *cmd, char **p);			// Program RFID
void CMD_ER(char *cmd, char **p);
void CMD_EW(char *cmd, char **p);
void CMD_TEST(char *cmd, char **p);
void CMD_P_EX(char *cmd, char **p);
void CMD_RFID_WRITE(char *cmd, char **p);
void CMD_RFID_READ(char *cmd, char **p);



u8 u_hex2dec(u8 c);
u16 u_asc2uint(u8 len, u8 *sptr); 
unsigned char u_toupper(unsigned char c);
void u_putHexByte(unsigned char c) ;

u16 cal_crc(u8 idata, u16 icrc);
void testRFID(void);
u8 swapByte(u8 iByte);

void my_flow_meterEx(void);
void rfid_test(u8 index);

#endif


