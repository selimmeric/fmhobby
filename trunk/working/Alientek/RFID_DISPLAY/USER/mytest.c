#include <stm32f10x_lib.h>
#include <string.h>
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

unsigned char mTask_Sync_Flag=0;
unsigned char mLED[2];
unsigned char mFS=0;
unsigned char mFS_CNT[3][3];
unsigned char mUartCmdBuffer[64];
char mPrintBuf[80];
unsigned char mTMR_100ms=0;
struct flow_counter mFlowCounter;
struct flow_counterA mFlowCounterA[3];
struct flow_rate mFlowRate[3];
struct flow_rateA mFlowRateA[3];


unsigned char mFIFO[FIFO_SIZE];
void my_printQ(void);

unsigned char count_8_rd(unsigned char pCnt, unsigned char cCnt);
unsigned int count_16_rd(unsigned int pCnt, unsigned int cCnt);

u16  mFR=0;
u16  mDP=0;	// Delta Phase 
//u8 	 mRFID[9]={0xd8,0x00,0x00,0x00,0x00,0x00,0x52,0x00,0x07};
//u8   mRFID[11]={0x2A,0xFF,0x07,0x00,0x52,0x00,0x00,0x00,0x00,0x00,0xD8};
// DB0000000000820007FF2A
u8   mRFID[11]={0x2A,0xFF,0x07,0x00,0x82,0x00,0x00,0x00,0x00,0x00,0xDB};

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
	static unsigned char tLED=0;
	static unsigned int tTMR=0;

#ifdef debug_isr
	DEBUG_H();
#endif

	tTMR++;
	if (tTMR>=200)
	{
		// Generate Sync. Task Flag on Every 100mS
		// Decouple it from ISR
		mTask_Sync_Flag = 1;
		mTMR_100ms +=2;
		tTMR = 0;
	}


//	Phase Accumulator to generate Flow Sensor count
//	60000 phase unit per cycle
//	n 		delta phase unit = n count / min
//  1 		delta phase unit = 1 count / min
//  3000 	delta phase unit = 3000 counts / min

	mDP = mFR;
	tPACC += mDP;

	if (tPACC>=60000)
	{
		tPACC = 0;
	}

	if (tPACC<30000)
	{	FS_O = 1;
	}
	else
	{	FS_O = 0;
  	}

	// Flow Sensor Input 0
	tFS = FS_I0;
	if ((tFS != mFS_CNT[0][1]) && (tFS==1))
		mFS_CNT[0][0]++;
	mFS_CNT[0][1] = tFS;	

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


	if (mLED[0]>tLED)
	{
		LED_R_ON();		

	}		
	else
	{
		LED_R_OFF();

	}

	my_printQ();

#ifdef debug_isr
	DEBUG_L();
#endif

}

void my_init(void)
{
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

	// Init PC4 as Output
	GPIOC->CRL&=0XFFF0FFFF; 
	GPIOC->CRL|=0X00030000;	// Output and  (CNF=01,MODE=11)   	 
    GPIOC->ODR|=1<<4;     	// Output 1 (ODR=1)	
	
	// Init PC5 as Output
	GPIOC->CRL&=0XFF0FFFFF; 
	GPIOC->CRL|=0X00300000;	// Output and  (CNF=01,MODE=11)   	 
    GPIOC->ODR|=1<<5;     	// Output 1 (ODR=1)	

	
	LCD_Clear(WHITE);				
	POINT_COLOR=RED;		
	LCD_ShowLineEx(0,"      D2M RFID Reader / Flow Sensor");
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
	// Sync. Task
	if 	(mTask_Sync_Flag == 1)
	{
		mTask_Sync_Flag = 0;
		my_task_sync();
	}
		// Uart Task
		my_task_uart();	
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

/*
		switch (mFlowCounter.state)
		{
			case 0:
				mFlowCounter.elapsedtime=0;
				mFlowCounter.count[0]=0;
				mFlowCounter.count[1]=0;
				mFlowCounter.count[2]=0;
				break;
			case 1:
//				mFlowCounter.elapsedtime += flow_sensor_rd(mFlowCounter.pTimer,mTMR_100ms);
				mFlowCounter.elapsedtime += 2;
				mFlowCounter.count[0] += flow_sensor_rd (mFlowCounter.sensor[0],mFS_CNT[0][0]); 
				mFlowCounter.count[1] += flow_sensor_rd (mFlowCounter.sensor[1],mFS_CNT[1][0]); 
				mFlowCounter.count[2] += flow_sensor_rd (mFlowCounter.sensor[2],mFS_CNT[2][0]); 
				break;
			case 2:
				break;		
		}

		mFlowCounter.sensor[0]= mFS_CNT[0][0];
		mFlowCounter.sensor[1]= mFS_CNT[1][0];
		mFlowCounter.sensor[2]= mFS_CNT[2][0];
		mFlowCounter.pTimer = mTMR_100ms;
 */
}

void my_task_1s(void)
{
	static u8 tCnt=0;
#ifdef debug_task_1s
	DEBUG_T();
#endif


	tCnt++;

	my_flow_counter();
	my_flow_meter();
	my_flow_meterA();
	sprintf(mPrintBuf," TM %02X    FRA %05d A %02X %04X %04d %04d",tCnt,mFR,mFlowCounter.sensor[0],mFlowCounterA[0].fr_cnt,mFlowRate[0].rate,mFlowRateA[0].rate[4]);
	LCD_ShowLineEx(LN_IS+0, (u8 *)mPrintBuf);	
	sprintf(mPrintBuf,"          ELT %05d B %02X %04X %04d %04d",mFlowCounter.elapsedtime,mFlowCounter.sensor[1],mFlowCounterA[1].fr_cnt,mFlowRate[1].rate,mFlowRateA[1].rate[4]);
	LCD_ShowLineEx(LN_IS+1, (u8 *)mPrintBuf);	
	sprintf(mPrintBuf,"          ST  %d     C %02X %04X %04d %04d",mFlowCounter.state,mFlowCounter.sensor[2],mFlowCounterA[2].fr_cnt,mFlowRate[2].rate,mFlowRateA[2].rate[4]);
	LCD_ShowLineEx(LN_IS+2, (u8 *)mPrintBuf);	

}

// Call @ every 200ms
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
	if (tTMR_S >=5)
	{
		// 1Second Task
		tTMR_S = 0;		
		my_task_1s();	
	}


	// Scan Input
	t=KEY_Scan();//得到键值
   	if(t)
	{						   
		switch(t)
		{				 
			case 1:
				mFR += 0x200;
				mFR &= 0x0fff;
//				LED0=!LED0;
				LCD_ShowLineEx(LN_KEY, " KEY   : 1");
				break;
			case 2:
				mFR += 0x10;
				mFR &= 0x0fff;
//				LED1=!LED1;
				LCD_ShowLineEx(LN_KEY, " KEY   : 2");
				break;
			case 3:				
//				LED0=!LED0;
//				LED1=!LED1;
				LCD_ShowLineEx(LN_KEY, " KEY   : 3");
				mFlowCounter.state ++;
				if (mFlowCounter.state>=3)
					mFlowCounter.state=0;
				break;
		}
	} 


}
 
void my_task_uart(void)
{

	u8 len;	

			// Process UART 1 Command
		if(USART_RX_STA&0x80)
		{					   
			len=USART_RX_STA&0x3f;//得到此次接收到的数据长度
			// Copy the USART_RX_BUF to mUartCmdBuffer
			// Release the buffer for recieving next command
			memset(mUartCmdBuffer, 0x00, sizeof(mUartCmdBuffer));
			memcpy(mUartCmdBuffer, USART_RX_BUF, len);
			USART_RX_STA=0;

			// Echo back to PrintQ - UART1
			sprintf(mPrintBuf,"%s\n",mUartCmdBuffer);
			my_FIFO_WRQ((u8 *)mPrintBuf); 

			// Display on LCD
			sprintf(mPrintBuf," UART1 : %s   ",mUartCmdBuffer);	
  			LCD_ShowLineEx(LN_UART1, (u8 *) mPrintBuf);

			CMD_In (1, mUartCmdBuffer, len);

		}	
		
		// Process UART 2 Command
		if(USART2_RX_STA&0x80)
		{					   
			len=USART2_RX_STA&0x3f;//得到此次接收到的数据长度
			// Copy the USART_RX_BUF to mUartCmdBuffer
			// Release the buffer for recieving next command
			memset(mUartCmdBuffer, 0x00, sizeof(mUartCmdBuffer));
			memcpy(mUartCmdBuffer, USART2_RX_BUF, len);
			USART2_RX_STA=0;

			// Echo back to PrintQ	- UART1
			sprintf(mPrintBuf,"%s\n",mUartCmdBuffer);
			my_FIFO_WRQ((u8 *)mPrintBuf);

			// Display on LCD
			sprintf(mPrintBuf," UART2 : %s   ",mUartCmdBuffer);	
  			LCD_ShowLineEx(LN_UART2, (u8 *) mPrintBuf);

			CMD_In (2, mUartCmdBuffer, len);
 
		}		 
}

void CMD_In(u8 id , u8 *line, u8 len)
{
   	char *cmd;
   	char *ptok;

	// Seperate the 
	cmd = strtok_r((char *)line, " ", &ptok);

	if (strncmp(cmd, "R", 1)==0)
	{
		CMD_RFID(cmd,&ptok);	
	}	
	
	if (strncmp(cmd, "S", 1)==0)
	{
		CMD_STATUS(cmd,&ptok);	
	}

	if (strncmp(cmd, "cFR", 3)==0)		
	{
		CMD_FR(cmd,&ptok);
	}

}

void CMD_STATUS(char *cmd, char **p)
{
	u8 tUVL;
	u8 tBUZZER;
	u8 tR;
	u8 tG;
	u8 tB;
	u8 tC1;
	u8 tC2;

	sprintf(mPrintBuf,"   %s : %s",cmd, *p);
	LCD_ShowLineEx(LN_DEBUG, (u8 *)mPrintBuf);		

	if (strlen(*p)>=12)
	{
		tUVL = u_asc2uint(1,(u8 *)*p);
		tBUZZER = u_asc2uint(1,(u8 *)*p+1);
		tR = u_asc2uint(2,(u8 *)*p+2);
		tG = u_asc2uint(2,(u8 *)*p+4);
		tB = u_asc2uint(2,(u8 *)*p+6);
		tC1 = u_asc2uint(2,(u8 *)*p+8);
		tC2 = u_asc2uint(2,(u8 *)*p+10);

		sprintf(mPrintBuf,"    %s : U%X-Z%X-R%02X-G%02X-B%02X-C%02X-C%02X",cmd, tUVL,tBUZZER,tR,tG,tB,tC1,tC2);
		LCD_ShowLineEx(LN_STATUS, (u8 *)mPrintBuf);	
	}

}

void CMD_RFID(char *cmd, char **p)
{
	u8 tHeader[7];
	u8 tZuVo[7];
	u8 tFooter[8];

	sprintf(mPrintBuf,"   %s : %s",cmd, *p);
	LCD_ShowLineEx(LN_DEBUG, (u8 *)mPrintBuf);		

	if (strlen(*p)>=20)
	{
		memset(tHeader, 0x00,sizeof(tHeader)); 
		memset(tZuVo, 0x00,sizeof(tZuVo)); 
		memset(tFooter, 0x00,sizeof(tFooter)); 

		memcpy(tHeader,*p+4, 6);
		memcpy(tZuVo,*p+10, 5);
		memcpy(tFooter,*p+15, 5);

		sprintf(mPrintBuf,"    %s : %s-%s-%s",cmd, tHeader,tZuVo,tFooter);
		LCD_ShowLineEx(LN_RFID, (u8 *)mPrintBuf);	
	}
}

void CMD_FR(char *cmd, char **line)
{
	char     *param;
    param    = strtok_r(NULL, " ", line);

	sprintf(mPrintBuf,"FR Cmd %s     ",param);
	LCD_ShowLineEx(LN_DEBUG,(u8 *)mPrintBuf);

	mFR = atoi(param);

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












