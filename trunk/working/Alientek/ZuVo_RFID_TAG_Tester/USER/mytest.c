#include <stm32f10x_lib.h>
#include <string.h>
#include "24cxx.h"
#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "key.h"
#include "exti.h"
#include "wdg.h"
#include "timer.h"
#include "lcd.h"	   
#include "mytest.h"
#include "cbuffer.h"


unsigned char mMCU_OP[2];

unsigned char mTask_Sync_Flag=0;
unsigned char mLED[2];
unsigned char mFS=0;
unsigned char mFS_CNT[3][3];
unsigned char mUartCmdBuffer[64];  
unsigned char mUart1CmdBuf[64];
unsigned char mUart1CmdIndex=0;
unsigned char mUart2CmdBuf[64];
unsigned char mUart2CmdIndex=0;

char mPrintBuf[80];
unsigned char mTMR_100ms=0;
struct flow_counter mFlowCounter;
struct flow_counterA mFlowCounterA[3];
struct flow_rate mFlowRate[3];
struct flow_rateA mFlowRateA[3];
char mSN[32];
unsigned int mCntMS=0;
unsigned char mTest=0xff;
unsigned char mData_Ena= 0;
unsigned char mData[10]; 		
unsigned char mDataCnt=0;


unsigned char mFIFO[FIFO_SIZE];	  
void my_printQ(void);			  

unsigned char mCBUF[10];	   

unsigned char count_8_rd(unsigned char pCnt, unsigned char cCnt);
unsigned int count_16_rd(unsigned int pCnt, unsigned int cCnt);

u8   mFL=0;
u16  mFR=200;
u16  mDP=0;	// Delta Phase 
//u8 	 mRFID[9]={0xd8,0x00,0x00,0x00,0x00,0x00,0x52,0x00,0x07};
//u8   mRFID[11]={0x2A,0xFF,0x07,0x00,0x52,0x00,0x00,0x00,0x00,0x00,0xD8};
// DB0000000000820007FF2A
u8   mRFID[11]={0x2A,0xFF,0x07,0x00,0x82,0x00,0x00,0x00,0x00,0x00,0xDB};
u8	mFID[5];
u8	mMCU_STATUS[20];
u8	mTST_STATUS[20];
struct flow_meter mFlowMeter[3];
u8  mEchoMCU=0;
u8  mEchoStatus=0;
u8  mFakeRFID=0;
unsigned int mMCU_FC[10];	// 10 integer to store variable from MCU\
							// mMCU_FC[0] = CurrentFlow count
							// mMCU_FC[1] = AccFlow count  (Gallons)
							// mMCU_FC[2] = AccFlow count  (Counts)
							// mMCU_FC[3] = Elapsed time
							// mMCU_FC[4] = mSystemState
							// mMCU_FC[5] = mLED
							// mMCU_FC[6] = mBuzzer	


u8 mRFID_P[2][8];			// RFID buffer for RFID programmer
							// [0][0-7] : RFID write to  programmer
							// [1][0-7] : RFID read from programmer


void LCD_ShowLineEx(u16 y, u8 *p)
{
// 	16 Pixel = 1 character
	u8 tBuf[80];
 	memset(tBuf, ' ', sizeof(tBuf));
	sprintf((char *)tBuf,"%s",p);
	tBuf[strlen((char *)p)]=' ';
	tBuf[40]=0x00;

	LCD_ShowString(0,y*LN_HEIGHT,tBuf);	

}

void LCD_DrawLineEx(u16 y)
{

	LCD_DrawLine(0,y*LN_HEIGHT-1,319,y*LN_HEIGHT-1);

}

void LCD_ShowLine(u8 x,u16 y, u8 *p)		 //显示一个字符串,16字体
{


	LCD_ShowString(0,y*20,"                                       ");
	LCD_ShowString(x, y*20,p);

}


//
// Call at 1ms ISR - Part of ISR 
// ISR complete within 5uS
//
void my_TMR_ISR(void)
{
	unsigned char tFS=0;
	static u32 tPACC=0;
//	static unsigned char tLED=0;
	static unsigned int tTMR=0;
	static unsigned char tClk[2] = {0,0};
	static unsigned int tBitCnt=0;
	unsigned char tCntBit =0 ; 
	unsigned char tCntByte =0 ; 
	mCntMS ++;

#ifdef debug_isr
	DEBUG_H();
#endif

	tTMR++;
	if (tTMR>=100)
	{
		// Generate Sync. Task Flag on Every 100mS
		// Decouple it from ISR
		mTask_Sync_Flag = 1;
		mTMR_100ms +=1;
		tTMR = 0;
	}


//	Phase Accumulator to generate Flow Sensor count
//	60000 phase unit per cycle
//	n 		delta phase unit = n count / min
//  1 		delta phase unit = 1 count / min
//  3000 	delta phase unit = 3000 counts / min

	mDP = mFR;

	if (mFL==1)
	{	
		tPACC += mDP;
	}

	if (tPACC>=60000)
	{
		tPACC -= 60000;
	}



//	mStatus[0]  = (PIN_BUZ<<5) | (PIN_R << 4) | (PIN_G << 3) | (PIN_B << 2)| (PIN_UVL<<0);

	mMCU_OP[0]  = (PIN_BUZ<<5) | (PIN_R << 4) | (PIN_B << 2) | (PIN_G << 3) | (PIN_UVL<<0);

	tClk[1] = tClk[0];
	tClk[0]	= PIN_R;

	// Feeding data to MCU
	if (mData_Ena ==0)
	{
		if (tPACC<30000)
		{	FS_O = 1;
		}
		else
		{	FS_O = 0;
 	 	}
 		tBitCnt	=0;
	 }
	 else
	 {
		if (mTest <16)
		{
			FS_O = PIN_G;
		}
		else
		{
			if ((tClk[0]==0) && (tClk[1]==1))
			{
				tCntByte = (tBitCnt>>3) & 0xff;
				tCntBit  = (tBitCnt & 0x07);
				if (tCntByte >= sizeof(mData))				
				{
					tCntByte = sizeof (mData);
				}

				FS_O = (mData[tCntByte]>>(7-tCntBit)) & 0x01;
				tBitCnt ++;			
			}
		}
		mDataCnt = tBitCnt;
	 }

	// Flow Sensor Input 0
	tFS = FS_I0;
	if ((tFS != mFS_CNT[0][1]) && (mFS_CNT[0][1]==0x01))
		mFS_CNT[0][0]++;
	mFS_CNT[0][1] = tFS;	

/*
	// Flow Sensor Input 1
	tFS = FS_I1;
	if ((tFS != mFS_CNT[1][1]) && (tFS==1))
		mFS_CNT[1][0]++;
	mFS_CNT[1][1] = tFS;	

	// Flow Sensor Input 2
	tFS = FS_I2;
	if ((tFS != mFS_CNT[2][1]) && (tFS==1))
		mFS_CNT[2][0]++;
	mFS_CNT[2][1] = tFS;	

 	tLED ++;
	if (tLED>=16)
	{
		tLED = 0;
	}
 */
 /*
	if (mLED[0]>tLED)
	{
		LED_R_ON();		
	}		
	else
	{
		LED_R_OFF();
	}
   */
	my_printQ();

#ifdef debug_isr
	DEBUG_L();
#endif

}

void my_init(void)
{

//	unsigned char i;

//	unsigned char t[2];
//	unsigned int  i[2];	
	
	// Init PC12 as Input
	RCC->APB2ENR|=1<<4;    //使能PORTC时钟	   	 
	GPIOC->CRH&=0XFFF0FFFF; 
//	GPIOC->CRH|=0X00070000;//PC12 推挽输出   	 
//    GPIOC->ODR|=1<<12;     //PC12 输出高
	GPIOC->CRH|=0X00080000;//PC12 Input and Pull Up/Down (CNF=10,MODE=00)   	 
    GPIOC->ODR|=1<<12;     //PC12 Pull Up (ODR=1)

	// Init PC11 as Input
	GPIOC->CRH&=0XFFFF0FFF; 
//	GPIOC->CRH|=0X00007000;	//PC11 推挽输出   	 
//    GPIOC->ODR|=1<<11;    //PC11 输出高
	GPIOC->CRH|=0X0008000;	//PC11 Input and Pull Up/Down (CNF=10,MODE=00)   	 
    GPIOC->ODR|=1<<11;     	//PC11 Pull Up (ODR=1)

	// Init PC4 as Input
	GPIOC->CRL&=0XFFF0FFFF; 
	GPIOC->CRL|=0X00080000;	// Output and  (CNF=10,MODE=00)   	 
    GPIOC->ODR|=1<<4;     	// Output 1 (ODR=1)	
	
	// Init PC5 as Input
	GPIOC->CRL&=0XFF0FFFFF; 
	GPIOC->CRL|=0X00800000;	// Output and  (CNF=10,MODE=00)   	 
    GPIOC->ODR|=1<<5;     	// Output 1 (ODR=1) - Internal Pull Up	

	
	LCD_Clear(WHITE);				
	POINT_COLOR=RED;		
	LCD_ShowLineEx(0,"      D2M ZuVo_300_MCU_Tester");
	sprintf(mPrintBuf,"   Build Date %s %s",__DATE__,__TIME__);	
	LCD_ShowLineEx(1, (u8 *) mPrintBuf);
	LCD_DrawLineEx(2);
	LCD_DrawLineEx(LN_IS);
	LCD_DrawLineEx(LN_UART1);
	LCD_DrawLineEx(LN_DEBUG);
	POINT_COLOR=BLUE;		
//	LCD_ShowLineEx(0, "0123456789012345678901234567890123456789");
	LCD_ShowLineEx(LN_RFID, "    R00 : 00000-xxxxx-20007");

/*
	t[0]=0xfe; t[1]=0x01;
	i[0]=0xfffe; i[1]=0x01;
	sprintf(mPrintBuf,"Test %02X %02X %02X %04X %04X %04X",t[0],t[1],count_8_rd(t[0],t[1]),i[0],i[1],count_16_rd(i[0],i[1]));
  	LCD_ShowLineEx(LN_DEBUG, (u8 *)mPrintBuf);	
*/
	mLED[0]=0;
	mLED[1]=0;

	mData[0] = 0x5a;
	mData[1] = 0x01;
	mData[2] = 0x23;
	mData[3] = 0x45;
	mData[4] = 0x67;
	mData[5] = 0x89;
	mData[6] = 0xab;
	mData[7] = 0xcd;
	mData[8] = 0xef;
	mData[9] = 0xa5;

	cbuffer_wr(USART_RX_BUF,'H');
	cbuffer_wr(USART_RX_BUF,'e');
	cbuffer_wr(USART_RX_BUF,'l');
	cbuffer_wr(USART_RX_BUF,'l');
	cbuffer_wr(USART_RX_BUF,'o');
	cbuffer_wr(USART_RX_BUF,0x0d);
	cbuffer_wr(USART_RX_BUF,0x0a);


	AT24CXX_Init();		//IIC初始化 
	if(AT24CXX_Check())//检测不到24c02
	{
		LCD_ShowLine(12,11,"24C02 Check Fail !!!!");
	}
	else
	{
		LCD_ShowLine(12,11,"24C02 Check Pass !!!!");
	}



}

void my_putch(u8 ch)
{
	while((USART1->SR&0X40)==0);//等待发送结束
	USART1->DR=ch;

}


void my_FIFO_WR(unsigned char iByte)
{

	unsigned char tmp=0;
	tmp = FIFO_PT_WR + 1;
	if (tmp >= (FIFO_SIZE - 2))
	{
		tmp = 0;
	}
	mFIFO[tmp] = iByte;
	FIFO_PT_WR = tmp;	
}


void my_FIFO_WRQ(u8 *p)
{
	unsigned char i ;
	
	for(i=0; i<strlen((char *)p); i++)
	{
		   my_FIFO_WR (*(p+i));
	}

}
void my_FIFO_WR_HexByte(unsigned char c)
{

	static const char STR_HEX[]="0123456789ABCDEF";
	my_FIFO_WR(STR_HEX[(c&0xF0)>>4]);
	my_FIFO_WR(STR_HEX[c&0x0F]);

}
void my_printQ(void)
{
	char ch;
	unsigned char tpt;

	// FIFO Not empty
	if (FIFO_PT_RD !=  FIFO_PT_WR)
	{
		tpt = FIFO_PT_RD + 1;
		if (tpt >= (FIFO_SIZE - 2))
			tpt = 0;
		ch = mFIFO[tpt];
		FIFO_PT_RD = tpt;
		my_putch (ch);
	}
}

void my_task(void)
{
	
	char tmp[30];
	sprintf(tmp," ");

	// Sync. Task  @ 100mS
	if 	(mTask_Sync_Flag == 1)
	{
		mTask_Sync_Flag = 0;
		my_task_sync();
		my_flow_meterEx();

		if (mTest < 36)
//		if ((mTest & 0xF0) == 0x00)	
		{
			switch (mTest)
			{
				case 0:
					PIN_RST=0;						
					sprintf(mPrintBuf,"TXX : P-R-G-B-Z-U-F\r\n" );
					my_FIFO_WRQ((u8 *)mPrintBuf); 
					break;					
				case 1:
					PIN_RST=1;
					break;					
				case 2:
					PIN_RST=1;
					mData_Ena = 1;
					break;					
				case 3:
					break;					
				case 4:
					break;					
				case 5:
					break;					
				case 6:
					break;					
			}

			if ((mTest >0) & (mDataCnt<20))
			{
				sprintf(mPrintBuf,"T%02X : %d-%d-%d-%d-%d-%d-%d\r\n",mTest,PIN_RST,PIN_R,PIN_G,PIN_B,PIN_BUZ,PIN_UVL,PIN_FS);
				my_FIFO_WRQ((u8 *)mPrintBuf);
			} 
			mTest ++;
		}
		else
		{
			mData_Ena = 0;
			mDataCnt =0;
		}

	}
		// Uart Task
		my_task_uart();	
//		my_printQ();
}

unsigned int count_16_rd(unsigned int pCnt, unsigned int cCnt)
{
	unsigned int tCnt = 0;

	tCnt = cCnt;

	if (cCnt >= pCnt)
	{
		tCnt -= pCnt;
	}
	else
	{
		tCnt += 1;
		tCnt +=	(pCnt ^ 0xffff);
	}

	return ((unsigned int)tCnt);

}

unsigned char count_8_rd(unsigned char pCnt, unsigned char cCnt)
{
	unsigned char tCnt = 0;

	tCnt = cCnt;

	if (cCnt >= pCnt)
	{
		tCnt -= pCnt;
	}
	else
	{
		tCnt += 1;
		tCnt +=	(pCnt ^ 0xff);
	}

	return ((unsigned char)tCnt);
}

unsigned char flow_sensor_rd(unsigned char pFS, unsigned char cFS)
{
	unsigned int tCnt = 0;

	tCnt = cFS;

	if (cFS >= pFS)
	{
		tCnt -= pFS;
	}
	else
	{
		tCnt += 0x100;
		tCnt -=	pFS;
	}

	return ((unsigned char)tCnt);

}

void my_flow_meterA(void)
{
	unsigned char i,j;

	for (i=0; i<=2; i++)
	{
		for (j=0;j<5;j++)
		{
			mFlowRateA[i].cnt[5-j]=mFlowRateA[i].cnt[5-j-1];
		}
		mFlowRateA[i].cnt[0]=mFlowCounterA[i].fr_cnt;
		for (j=0; j<5; j++)
		{
			mFlowRateA[i].rate[j] = count_16_rd(mFlowRateA[i].cnt[j+1],mFlowRateA[i].cnt[0]);
		}		

	}

}

void my_flow_meter(void)
{
	unsigned char i;

	for (i=0; i<=2; i++)
	{
	 	mFlowRate[i].pcnt = mFlowRate[i].ccnt;
		mFlowRate[i].ccnt = mFlowCounterA[i].fr_cnt;
		mFlowRate[i].ptmr = mFlowRate[i].ctmr;		
		mFlowRate[i].ctmr = mTMR_100ms;		
		mFlowRate[i].rate = count_16_rd(mFlowRate[i].pcnt,mFlowRate[i].ccnt);
	}

}

void my_flow_meterEx(void)
{
	unsigned char i;
	unsigned char tmp;

	i = 0;
	
		tmp = mFS_CNT[i][0];		// Read current flow sensor count
		mFlowMeter[i].acc_fc += count_8_rd (mFlowMeter[i].fs,tmp);
		mFlowMeter[i].fs = tmp;

}

void my_flow_counter(void)
{
	unsigned char i;
	unsigned char tmp;

	for (i=0; i<=2; i++)
	{
		tmp = mFS_CNT[i][0];
		mFlowCounterA[i].fr_cnt += count_8_rd (mFlowCounterA[i].sensor,tmp);
		mFlowCounterA[i].sensor = tmp;
		tmp = mTMR_100ms;
		mFlowCounterA[i].fr_tmr += 1;
		mFlowCounterA[i].pTimer = tmp;	
	}

}

void my_task_update_io(void)
{

	unsigned char tbyte;

	sprintf(mPrintBuf," MCU IO : R-G-B-Z-U-FS");
	LCD_ShowLineEx(LN_IS-4, (u8 *)mPrintBuf);	

	tbyte = mMCU_OP[0];
	tbyte ^= (1 <<5) | (1<<4)| (1<<3)| (1<<2);

//	mStatus[0]  = (PIN_BUZ<<5) | (PIN_R << 4) | (PIN_G << 3) | (PIN_B << 2)| (PIN_UVL<<0);
	sprintf(mPrintBuf,"        : %x-%x-%x-%x-%x-%02x",(tbyte>>4)&0x01,(tbyte>>2)&0x01,(tbyte>>3)&0x01,(tbyte>>5)&0x01,(tbyte>>0)&0x01,tbyte );
	LCD_ShowLineEx(LN_IS-3, (u8 *)mPrintBuf);

//	mStatus[0]  = (PIN_BUZ<<5) | (PIN_R << 4) | (PIN_G << 3) | (PIN_B << 2)| (PIN_UVL<<0);
	tbyte = mMCU_OP[1];
	tbyte ^= (1<<4)|(1<<3)|(1<<2);
	sprintf(mPrintBuf,"        : %x-%x-%x-%x-%x-%02x",(tbyte>>4)&0x01,(tbyte>>2)&0x01,(tbyte>>3)&0x01,(tbyte>>5)&0x01,(tbyte>>0)&0x01,tbyte );
	LCD_ShowLineEx(LN_IS-2, (u8 *)mPrintBuf);

	sprintf(mPrintBuf,"   RFID : 00000-%02X%02X%X-20007   %02X-%02X", mFID[0],mFID[1],mFID[2]>>4,mFID[3],mFID[4]);
	LCD_ShowLineEx(LN_RFID, (u8 *)mPrintBuf);

}

void my_task_1s(void)
{

	unsigned char i;

	static u8 tCnt=0;
#ifdef debug_task_1s
	DEBUG_T();
#endif


	tCnt++;

//	my_flow_meterEx();

//	my_flow_counter();
//	my_flow_meter();
//	my_flow_meterA();

	if (0)
	{
	sprintf(mPrintBuf," TM %02X    FRA %05d A %02X %06X %06d",tCnt,mFR,mFlowMeter[0].fs,mFlowMeter[0].acc_fc,mFlowMeter[0].acc_fc);
	LCD_ShowLineEx(LN_IS+0, (u8 *)mPrintBuf);	
//	sprintf(mPrintBuf,"          ELT %05d B %02X %04X %04d %04d",mFlowCounter.elapsedtime,mFlowCounter.sensor[1],mFlowCounterA[1].fr_cnt,mFlowRate[1].rate,mFlowRateA[1].rate[4]);
	sprintf(mPrintBuf,"          ELT %05d      %06X %06d",mFlowCounter.elapsedtime,mMCU_FC[0],mMCU_FC[0]);
	LCD_ShowLineEx(LN_IS+1, (u8 *)mPrintBuf);	
//	sprintf(mPrintBuf,"          ST  %d     C %02X %04X %04d %04d",mFlowCounter.state,mFlowCounter.sensor[2],mFlowCounterA[2].fr_cnt,mFlowRate[2].rate,mFlowRateA[2].rate[4]);
	sprintf(mPrintBuf,"          ST  %d   %06X %06X %06X",mFlowCounter.state,mMCU_FC[1],mMCU_FC[2],mMCU_FC[3]);
	LCD_ShowLineEx(LN_IS+2, (u8 *)mPrintBuf);	
	}

//	Echo Status

//	D0 <><>	status from MCU
//	
//	D1 <><>	status from Tester
//
//

	if (mEchoStatus==1)	
	{
//		mTST_STATUS[2] = mFlowMeter[0].fs;
		mTST_STATUS[3] = mFlowMeter[0].acc_fc>>8;
		mTST_STATUS[4] = mFlowMeter[0].acc_fc &0xff;

		mTST_STATUS[16] = mFID [0];
		mTST_STATUS[17] = mFID [1];
		mTST_STATUS[18] = mFID [2];
		


		my_FIFO_WRQ("D0");
		for (i=0; i<19; i++)
			 my_FIFO_WR_HexByte( mMCU_STATUS[i]);
		my_FIFO_WRQ("\r\n");

		my_FIFO_WRQ("D1");
		for (i=0; i<19; i++)
			 my_FIFO_WR_HexByte( mTST_STATUS[i]);
		my_FIFO_WRQ("\r\n");

	}
}

// Call @ every 100ms
void my_task_sync(void)
{
	u8 t;
	static u8 tTMR=0;
	static u8 tLED=0;
	static u8 tTMR_S=0;

#ifdef debug_task_200ms
	DEBUG_T();
#endif

	tTMR ++;
	if (tTMR >=1)
	{
		tTMR=0;
		tLED ++;
		if (tLED>=32)
			tLED=0;
	
		if (tLED>16)
			mLED[0] = (tLED ^ 0x1f);
		else
			mLED[0] = tLED ;
	 }
		 
	tTMR_S ++;

	switch (tTMR_S)
	{
		case 0:
			break;
		case 1:
//			my_task_update_io();
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			my_task_update_io();
			break;		
		case 6:
			break;		
		case 7:
			break;		
		case 8:
			break;		
		default :
			// 1Second Task
			tTMR_S = 0;		
//			my_task_update_io();
			my_task_1s();			
			break;
	}



	// Scan Input
	t=KEY_Scan();//得到键值
   	if(t)
	{						   
		switch(t)
		{				 
			case 1:
				if (mFL==0)
					mFL = 1;
				else
					mFL = 0;

				LED0=!LED0;
				LCD_ShowLineEx(LN_KEY, " KEY   : 1");
				break;
			case 2:
				LCD_ShowLineEx(LN_KEY, " KEY   : 2");
				rfid_test(1);
				break;
			case 3:				
				LCD_ShowLineEx(LN_KEY, " KEY   : 3");
				rfid_test(0);
				break;
		}
	} 

}
 
void my_task_uart(void)
{

	unsigned char tmp;
	unsigned char i;

		// Process UART 1 Command

		while (cbuffer_is_empty(USART_RX_BUF) !=1)
		{
			tmp = u_toupper(cbuffer_rd(USART_RX_BUF));
			mUart1CmdBuf[mUart1CmdIndex]= tmp;
			mUart1CmdIndex++;
			if ((tmp == 0x0d) || (tmp == 0x0a))
			{
				memset(mUartCmdBuffer, 0x00, sizeof(mUartCmdBuffer));
				memcpy(mUartCmdBuffer, mUart1CmdBuf, mUart1CmdIndex-1);	
				if (mUart1CmdIndex>1)			
					CMD_In (1, mUartCmdBuffer, mUart1CmdIndex);				
				mUart1CmdIndex=0;
			}
		}		

		// Process UART 2 Command
		while (cbuffer_is_empty(USART2_RX_BUF) !=1)
		{
			tmp = u_toupper(cbuffer_rd(USART2_RX_BUF));
			
			mUart2CmdBuf[mUart2CmdIndex]= tmp;
			mUart2CmdIndex++;
			if ((tmp == 0x0d) || (tmp == 0x0a))
			{
				memset(mUartCmdBuffer, 0x00, sizeof(mUartCmdBuffer));
				memcpy(mUartCmdBuffer, mUart2CmdBuf, mUart2CmdIndex-1);	
				
				if (mUart2CmdIndex>3)
				{
					if (mEchoMCU ==1)
					{	
						// Echo to USB-UART
						for (i=0; i<mUart2CmdIndex-1; i++)
						{
							tmp = mUart2CmdBuf[i];
							my_FIFO_WR(tmp);
						}	
						my_FIFO_WR(0x0d);
						my_FIFO_WR(0x0a);
					}
					sprintf(mPrintBuf,"UART2:%s", mUartCmdBuffer);
					LCD_ShowLineEx(LN_UART2, (u8 *)mPrintBuf);
					CMD_In (2, mUartCmdBuffer, mUart1CmdIndex);				
				}				
				mUart2CmdIndex=0;
			}
		}

 
}

void CMD_In(u8 id , u8 *line, u8 len)
{
   	char *cmd;
   	char *ptok;

	// Seperate the cmd and parameter with white space
	// all input characters are in upper case

	cmd = strtok_r((char *)line, " ", &ptok);


	if (strncmp(cmd, "R", 1)==0)
	{
		CMD_RFID(cmd,&ptok);	
		return;
	}	
	
	if (strncmp(cmd, "D", 1)==0)
	{
		CMD_STATUS(cmd,&ptok);	
		return;
	}

	if (strncmp(cmd, "CFR", 3)==0)		
	{
		CMD_FR(cmd,&ptok);
		return;
	}

	if (strncmp(cmd, "O", 1)==0)		
	{
		CMD_DO(cmd,&ptok);
		return;
	}

	if (strncmp(cmd, "I", 1)==0)		
	{
		CMD_DI(cmd,&ptok);												 
		return;
	}

	if (strncmp(cmd,"PW", 2)==0)
	{
		  CMD_RFID_WRITE(cmd,&ptok);
		  return;
	}

	if (strncmp(cmd,"PR", 2)==0)
	{
		  CMD_RFID_READ(cmd,&ptok);
		  return;
	}

	if (strncmp(cmd,"P", 1)==0)
	{
		  CMD_P(cmd,&ptok);
		  return;
	}

	if (strncmp(cmd,"EW", 2)==0)	// EEP Write
	{
		  CMD_EW(cmd,&ptok);
		  return;
	}

	if (strncmp(cmd,"ER", 2)==0)	// EEP Read
	{
		  CMD_ER(cmd,&ptok);
		  return;
	}

	if (strncmp(cmd,"T", 1)==0)		// Testing
	{
		  CMD_TEST(cmd,&ptok);
		  return;
	}


	if (id==1)
	{	
	// Echo back to PrintQ - UART1 for unknow command
	sprintf(mPrintBuf,"unknown %s\n\r",mUartCmdBuffer);
	my_FIFO_WRQ((u8 *)mPrintBuf); 
	}	
}


void CMD_TEST(char *cmd, char **p)
{

	unsigned char add;

	add = u_asc2uint(1, (u8 *)*p) & 0xff;

	sprintf(mPrintBuf,"Testing %s %s - %02x ",cmd,*p,add);
	LCD_ShowLineEx(LN_DEBUG,(u8 *)mPrintBuf);

	switch (add)
	{
		case 0:
			AT24CXX_RFID_Write(&mRFID_P[0][0],8);
			break;
		case 1:
			AT24CXX_RFID_Read(&mRFID_P[1][0],8);
			break;
	}

}


void CMD_STATUS(char *cmd, char **p)
{

//	Status debug info
//	  <----------- input ------------->	<-state varialble -> < -output- >
//	D <debug_cnt><rfid_cnt><fs_cnt><FC> <fsm_state><uvl_cnt> <latc><led><FG>
//	D 4B005D0000  0000  1C41014A

//  D 01AAAABBBBCCCCDDDDEEEEFFFF

	unsigned char i;
	unsigned int tFG,tFC;

	for (i=0; i<sizeof(mMCU_STATUS);i++)
	{
		mMCU_STATUS[i] = u_asc2uint(2, (u8 *)(*p+(i<<1))) & 0xff;
	}

	switch (mMCU_STATUS[0])
	{
		case 0x00:
			// Get RFID status
			if (mFID[3] != mMCU_STATUS[1])
			{
				mFID[4] = 0;
				mFID[3] = mMCU_STATUS[1];
			}
			else
			{
				mFID[4] ++;
			}
		
			mMCU_OP[1] = mMCU_STATUS[9];
		
//			mMCU_FC[0]	= (mMCU_STATUS[3]<<8) | (mMCU_STATUS[4]); 
//			mMCU_FC[1]	= (mMCU_STATUS[3]<<8) | (mMCU_STATUS[4]); 
			break;
		case 0x01:

			// Capture Current Flow Count
			tFG			= (mMCU_STATUS[1]<<8)  | (mMCU_STATUS[2]); 
			tFC			= (mMCU_STATUS[3]<<8)  | (mMCU_STATUS[4]); 

			for (i=0; i<tFG; i++)
			{
				tFC += FS_CPG;
			}	

		//	tFC += 0x00340000;
			mMCU_FC[0] = tFC;

			// Capture Accumlated Flow Count
			tFG			= (mMCU_STATUS[5]<<8)  | (mMCU_STATUS[6]); 
			tFC			= (mMCU_STATUS[7]<<8)  | (mMCU_STATUS[8]); 
			/*
			for (i=0; i<tFG; i++)
			{
				tFC += FS_CPG;
			}	
			*/
			mMCU_FC[1] = tFG;		
			mMCU_FC[2] = tFC;		
				
			// Capture Current Flow Time
			tFG			= (mMCU_STATUS[9]<<8)   | (mMCU_STATUS[10]); 
			tFC			= (mMCU_STATUS[11]<<8)  | (mMCU_STATUS[12]); 
			tFC		   |= (tFG<<16);
	
			mMCU_FC[3] = tFC/5;		
			
			mMCU_FC[4] = (mMCU_STATUS[13]) ;
			mMCU_FC[5] = (mMCU_STATUS[14]) ;
			mMCU_FC[6] = (mMCU_STATUS[15]) ;
																   	
					
			break;
	}
//	sprintf(mPrintBuf,"   %s : %s %02X%02X%02X%02X",cmd, *p,mMCU_STATUS[0],mMCU_STATUS[1],mMCU_STATUS[2],mMCU_STATUS[3]);
//	LCD_ShowLineEx(LN_DEBUG, (u8 *)mPrintBuf);		

}

void CMD_RFID(char *cmd, char **p)
{
//	sprintf(mPrintBuf,"   %s : %s",cmd, *p);
//	LCD_ShowLineEx(LN_DEBUG, (u8 *)mPrintBuf);		

// u8   mRFID[11]={0x2A,0xFF,0x07,0x00,0x82,0x00,0x00,0x00,0x00,0x00,0xDB};

	unsigned char len;

	len = strlen(*p); 

	switch (len)
	{
		case 6:		// Only 20bits ZuVo Filter ID + 4bits Constant
			mFID[0] = u_asc2uint(2, (u8 *)*p) & 0xff;
	 		mFID[1] = u_asc2uint(2, (u8 *)(*p+2)) & 0xff;
	 		mFID[2] = u_asc2uint(2, (u8 *)(*p+4)) & 0xff;

			sprintf(mPrintBuf,"  RFID V 000000%02X%02X%02X0007",mFID[0],mFID[1],mFID[2]);
			LCD_ShowLineEx(LN_IS+2, (u8 *)mPrintBuf);	

			break;
		case 16:  	// Whole 64bits RFID
			my_FIFO_WRQ("R ");	
			my_FIFO_WRQ((u8 *)*p);
			my_FIFO_WRQ("\r\n");			
			break;

	}

	sprintf(mPrintBuf,"   %s : %s : %d",cmd, *p, strlen(*p));
	LCD_ShowLineEx(LN_DEBUG, (u8 *)mPrintBuf);		

}

void CMD_ER(char *cmd, char **p)
{

	unsigned char add,len,i;

	add = u_asc2uint(2, (u8 *)*p) & 0xff;
	len = u_asc2uint(2, (u8 *)(*p+2)) & 0xff;

	sprintf(mPrintBuf,"EEP Read %s %s - %02x %02X",cmd,*p,add,len);
	LCD_ShowLineEx(LN_DEBUG,(u8 *)mPrintBuf);

	my_FIFO_WRQ("ER ");	
	u_putHexByte(add);
	for (i=0; i<len; i++)
	{
		u_putHexByte(AT24CXX_ReadOneByte(add+i));
	}
	my_FIFO_WRQ("\r\n");

}


void CMD_EW(char *cmd, char **p)
{

	unsigned char add,len;
	unsigned char i;
	unsigned char tData[16];

	add = u_asc2uint(2, (u8 *)*p) & 0xff;
	len = (strlen(*p)-2)>>1;

	for (i=0; i<len; i++)
	{
		tData[i]=  u_asc2uint(2, (u8 *)(*p+2+(i<<1))) & 0xff;
	}

	sprintf(mPrintBuf,"EEP Write %s %s - %02X %d %02X",cmd,*p,add,len,tData[1]);
	LCD_ShowLineEx(LN_DEBUG,(u8 *)mPrintBuf);

	my_FIFO_WRQ("EW ");	
	u_putHexByte(add);
	for (i=0; i<len; i++)
	{
		AT24CXX_WriteOneByte(add+i,tData[i]);
		u_putHexByte(AT24CXX_ReadOneByte(add+i));

	}
	my_FIFO_WRQ("\r\n");

}

void CMD_RFID_READ(char *cmd, char **p)
{
	// Program RFID
	// PW 0000000123452007	- Write RFID
	// PR 					- Read  RFID

	u8 i;

	sprintf(mPrintBuf,"RFID READ %s %s %d",cmd,*p, strlen(*p));
	LCD_ShowLineEx(LN_DEBUG,(u8 *)mPrintBuf);

	AT24CXX_RFID_Read(&mRFID_P[1][0],8);
	AT24CXX_RFID_Read(&mRFID_P[1][0],8);
//	delay_ms(100);

	my_FIFO_WRQ("V ");	
	for (i=0; i<sizeof(mRFID_P[1]); i++)
	{
		mRFID_P[1][i]=swapByte(mRFID_P[1][i]);
		u_putHexByte(mRFID_P[1][i]);
	}
	my_FIFO_WRQ("\r\n");

  	sprintf(mPrintBuf,"  RFID W ----------------");
	LCD_ShowLineEx(LN_IS+0, (u8 *)mPrintBuf);	

  	sprintf(mPrintBuf,"  RFID R %02X%02X%02X%02X%02X%02X%02X%02X",mRFID_P[1][0],mRFID_P[1][1],mRFID_P[1][2],mRFID_P[1][3],mRFID_P[1][4],mRFID_P[1][5],mRFID_P[1][6],mRFID_P[1][7]);
	LCD_ShowLineEx(LN_IS+1, (u8 *)mPrintBuf);	


}

void CMD_RFID_WRITE(char *cmd, char **p)
{
	// Program RFID
	// PW 0000000123452007	- Write RFID
	// PR 					- Read  RFID

	u8 i;
	u8 tFlag=0;

	sprintf(mPrintBuf,"RFID WRITE %s %s %d",cmd,*p, strlen(*p));
	LCD_ShowLineEx(LN_DEBUG,(u8 *)mPrintBuf);

	if (strlen(*p) !=16)
	{
		sprintf(mPrintBuf,"#RFID has to be 16 digits : %s\n\r",*p);
		my_FIFO_WRQ((u8 *)mPrintBuf); 		
		return;
	}

	// Reading RFID
	for (i=0; i<sizeof(mRFID_P[0]); i++)
	{
		mRFID_P[0][i]= u_asc2uint(2, (u8 *)(*p+(i<<1))) & 0xff;
		mRFID_P[1][i]= swapByte(mRFID_P[0][i]);
	}

	AT24CXX_RFID_Write(&mRFID_P[0][0],8);

	if (mFakeRFID==0)
	{
		AT24CXX_RFID_Read(&mRFID_P[1][0],8);
		AT24CXX_RFID_Read(&mRFID_P[1][0],8);
	}

	my_FIFO_WRQ("V ");	

	for (i=0; i<sizeof(mRFID_P[0]); i++)
	{
		mRFID_P[1][i]=swapByte(mRFID_P[1][i]);
		if (mRFID_P[0][i]!=mRFID_P[1][i])
		{
			tFlag = 1;
		}
	}


	for (i=0; i<sizeof(mRFID_P[0]); i++)
	{
		u_putHexByte(mRFID_P[1][i]);
	}
	my_FIFO_WRQ("\r\n");

	if (tFlag)
	{
	 	POINT_COLOR=RED;		
	}

  	sprintf(mPrintBuf,"  RFID W %02X%02X%02X%02X%02X%02X%02X%02X",mRFID_P[0][0],mRFID_P[0][1],mRFID_P[0][2],mRFID_P[0][3],mRFID_P[0][4],mRFID_P[0][5],mRFID_P[0][6],mRFID_P[0][7]);
	LCD_ShowLineEx(LN_IS+0, (u8 *)mPrintBuf);	

  	sprintf(mPrintBuf,"  RFID R %02X%02X%02X%02X%02X%02X%02X%02X",mRFID_P[1][0],mRFID_P[1][1],mRFID_P[1][2],mRFID_P[1][3],mRFID_P[1][4],mRFID_P[1][5],mRFID_P[1][6],mRFID_P[1][7]);
	LCD_ShowLineEx(LN_IS+1, (u8 *)mPrintBuf);	

  	sprintf(mPrintBuf,"  RFID V ");
	LCD_ShowLineEx(LN_IS+2, (u8 *)mPrintBuf);	

 	POINT_COLOR=BLUE;		

	
}

void CMD_P(char *cmd, char **p)
{
	// Program RFID
	// P 0000000123452007
	// V 0000000123452007	- Return RFID from programmer

	u8 i;

	sprintf(mPrintBuf,"P_RFID %s %s %d",cmd,*p, strlen(*p));
	LCD_ShowLineEx(LN_DEBUG,(u8 *)mPrintBuf);

	if (strlen(*p) !=16)
	{
		sprintf(mPrintBuf,"#RFID has to be 16 digits : %s\n\r",*p);
		my_FIFO_WRQ((u8 *)mPrintBuf); 		
		return;
	}

	// Reading RFID
	for (i=0; i<sizeof(mRFID_P[0]); i++)
	{
		mRFID_P[0][i]= u_asc2uint(2, (u8 *)(*p+(i<<1))) & 0xff;
	}

	// Writting to II2 Programmer

/*
	for (i=0; i<sizeof(mRFID_P[0]); i++)
	{
		AT24CXX_WriteOneByte(0x00+i,mRFID_P[0][i]);
	}
  */

//	AT24CXX_Write(0,&mRFID_P[0][0],8);

	
	AT24CXX_RFID_Write(&mRFID_P[0][0],8);
//	delay_ms(1000);
//	delay_ms(1000);
//	delay_ms(1000);

	// Returning RFID
/*
	for (i=0; i<sizeof(mRFID_P[1]); i++)
	{																				
		mRFID_P[1][i]= AT24CXX_ReadOneByte(0x00+i);
	}
*/

//	AT24CXX_Read(0,&mRFID_P[1][0],8);


	AT24CXX_RFID_Read(&mRFID_P[1][0],8);
	delay_ms(100);
	AT24CXX_RFID_Read(&mRFID_P[1][0],8);
	delay_ms(100);


	
	my_FIFO_WRQ("V ");	
	for (i=0; i<sizeof(mRFID_P[1]); i++)
	{
		u_putHexByte(mRFID_P[1][i]);
	}
	my_FIFO_WRQ("\r\n");
	
}


void CMD_FR(char *cmd, char **line)
{
	char     *param;
    param    = strtok_r(NULL, " ", line);

	sprintf(mPrintBuf,"FR Cmd %s     ",param);
	LCD_ShowLineEx(LN_DEBUG,(u8 *)mPrintBuf);

	mFR = atoi(param);

	sprintf(mPrintBuf,"CMD FlowRate  \n\r");
	my_FIFO_WRQ((u8 *)mPrintBuf); 

}

void CMD_DO(char *cmd, char **line)
{
	unsigned int address;	
	unsigned char i;
	char     *param;
	char 	 *param2;
    param    = strtok_r(NULL, " ", line);
	param2 	 = strtok_r(NULL, " ", line);
	sprintf(mPrintBuf,"Output Cmd %s %s",param,param2);
	LCD_ShowLineEx(LN_DEBUG,(u8 *)mPrintBuf);

	address = atoi(param);

	switch (address)
	{
		case 0:
			if (atoi(param2) == 0)
				LED0=0;
			else
				LED0=1;
			break;
		case 1:	//	Flow On/OFF
			if (atoi(param2) == 0)
				mFL=0;
			else
				mFL=1;
			break;		
		case 2:	//	Flow Meter Reset
			switch (atoi(param2))
			{
				case 0:
					mFlowMeter[0].acc_fc=0;
					break;
				case 1:
					mFlowMeter[1].acc_fc=0;
					break;
				case 2:
					mFlowMeter[2].acc_fc=0;
					break;
			}

			break;
		case 4:	//	Fake RFID programmer On/Off
			if (atoi(param2) == 0)
				mFakeRFID=0;
			else
				mFakeRFID=1;
			break;					

		case 5:	//	Echo Status
			if (atoi(param2) == 0)
				mEchoStatus=0;
			else
				mEchoStatus=1;
			break;					
		case 6:	//	Echo MCU UART On/OFF
			if (atoi(param2) == 0)
				mEchoMCU=0;
			else
				mEchoMCU=1;
			break;						
		case 7:	// Start Test
			mTest = atoi(param2);
			sprintf(mPrintBuf,"Test Cmd %s     ",param);
			LCD_ShowLineEx(LN_DEBUG,(u8 *)mPrintBuf);
			break;
		case 8:	// Change Flow Rate
			mFR = atoi(param2);
			sprintf(mPrintBuf,"FR Cmd %s     ",param);
			LCD_ShowLineEx(LN_DEBUG,(u8 *)mPrintBuf);
			break;
		case 9:	// Change SN		
			memset(mSN, 0x00,sizeof(mSN)); 
			sprintf(mSN,"%s",(char *)param2);
			sprintf(mPrintBuf,"   SN   %s",mSN);
			LCD_ShowLineEx(LN_RFID+3,(u8 *)mPrintBuf);

 			memset(mData,0x00,sizeof(mData));
			for (i=0; i<=(sizeof(mData)); i++)
			{
				mData[i] = u_asc2uint(2,(u8 *)&mSN[i<<1]) ;
			}			
			sprintf(mPrintBuf," Data : %02X%02X%02X%02X%02X%02X%02X%02X",mData[0],mData[1],mData[2],mData[3],mData[4],mData[5],mData[6],mData[7]);
 			LCD_ShowLineEx(LN_RFID+4,(u8 *)mPrintBuf);

			break;

		default:
			break;	
	
	}

	sprintf(mPrintBuf,"CMD DO %s %s \n\r",param,param2);
	my_FIFO_WRQ((u8 *)mPrintBuf); 


}

void CMD_DI(char *cmd, char **line)
{
	unsigned int address;	
	char     *param;
    param    = strtok_r(NULL, " ", line);

	address = atoi(param);	
	sprintf(mPrintBuf,"Input Cmd %s %d ",param,address);
	LCD_ShowLineEx(LN_DEBUG,(u8 *)mPrintBuf);
	
	switch (address)
	{
		case 0:
		case 1:
 			sprintf(mPrintBuf,"i,%d,%04x \n\r",address,mCntMS& 0xffff);
			my_FIFO_WRQ((u8 *)mPrintBuf); 		
			break;
		case 8:		// FS Status
 			sprintf(mPrintBuf,"i,%d,%05X,%06X,%06X,%06X,%02X,%02X,%02X,%02X \n\r",address,mFlowMeter[0].acc_fc,mMCU_FC[0],mMCU_FC[1],mMCU_FC[2],mMCU_FC[3],mMCU_FC[4],mMCU_FC[5],mMCU_FC[6]);
			my_FIFO_WRQ((u8 *)mPrintBuf); 		
			break;
		default:
 			sprintf(mPrintBuf,"i,%d\n\r",address);
			my_FIFO_WRQ((u8 *)mPrintBuf); 		
			break;
	}



}


void rfid_test(u8 index )
{
	char tCmd[20];
	static char tFlag=0;

	switch (index)
	{
		case 0 :
			tFlag ^= 0x01;
			LCD_ShowLineEx(LN_KEY, " KEY   : 3 (Test RFID Tag - Write !!)");
	
			if (tFlag)
			{														 
			sprintf(tCmd,"PW 000000ABCDE20007");
			}
			else
			{
			sprintf(tCmd,"PW 0000001234520007");
			}
			CMD_In(1,(u8 *)tCmd,20);
			break;
		case 1:

			LCD_ShowLineEx(LN_KEY, " KEY   : 2 (Test RFID Tag - Read !!)");
			sprintf(tCmd,"PR ");
			CMD_In(1,(u8 *)tCmd,20);
			break;
	}

}



void testRFID(void)
{
//	D80000000000520007FF2A

	u16 tcrc=0;
	u8	i,j;

	tcrc = 0x00;
	for (i=0; i<8; i++)
	{
		tcrc ^= swapByte(mRFID[i+2]);
		for (j=0; j<=7; j++)
		{
			if ((tcrc & 0x80) !=0 )
			{
				tcrc = (tcrc << 1);
				tcrc ^= 0x11d;
			}
			else
			{
				tcrc = tcrc << 1;		
			}
			tcrc &= 0x1ff;
		}	

	}
	
	tcrc = swapByte(tcrc);	

   	i = tcrc;

}

u8 swapByte(u8 iByte)
{
	u8 tByte=0;
	u8 i;

	for (i=0; i<=7; i++)
	{
		tByte = tByte << 1;
		if ((iByte & (1<<i)) != 0)
		{
			tByte++;
		}	
	}

	return (tByte);

} 

u8 u_hex2dec(u8 c) 
{
	if ((c>='0') && (c<='9')) 
    {
		return (c-0x30);
	}else if ((c>='A') && (c<='F')) 
    {
		return (c-0x37);
	}
	return 0;
}



u16 u_asc2uint(u8 len, u8 *sptr)
{
	u8 i;
	u16 temp;

	temp = u_hex2dec(*sptr++);
	for(i = 1; i < len; i++)
		temp = temp * 0x10 + u_hex2dec(*sptr++);
	return (temp);
}

unsigned char u_toupper(unsigned char c)

{

	if ((c>='a') && (c<='z'))		//convert lower case to upper case
		return(c-0x20);
	else
		return(c);

}

void u_putch(unsigned char c)
{
	my_FIFO_WR(c);

}

void u_putHexByte(unsigned char c) 
{
	static const char STR_HEX[]="0123456789ABCDEF";
	u_putch(STR_HEX[(c&0xF0)>>4]);
	u_putch(STR_HEX[c&0x0F]);

}









