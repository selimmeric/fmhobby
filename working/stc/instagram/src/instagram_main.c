/*******************************************************************************
Include Section
*******************************************************************************/

#include "instagram_main.h"
#include "stc_mcu.h"
#include "utility.h"
#include "lcd.h"



/*****************************************************************************
Global Variale Section
******************************************************************************/

extern unsigned char mCmdQ[32];

extern unsigned char mTMR0_Flag;


/*****************************************************************************
Main Section
******************************************************************************/
main()
{
	unsigned char idata *idata_point;
	unsigned char i,j;

	init_hardware();
	init_timer();
	UART_one_init();
	P1 = 0xff;

	init_system();
	lcd_initial();
	dsp_single_colour(0xf8,0x00);

	P1 = 0xff;

 	EA	= 1;

	u_puts(VERSION);
	u_puts("ID=");
	idata_point = 0xf1;
	for (i=0; i<=6; i++)
	{
		j = *(idata_point+i);
		u_putHexByte(j & 0xff,0x00);
	}

	u_puts("\r\n");

	mTMR0_Flag=1;
	
	dsp_single_colour_ex(0xff,0xf8,0x00);	//Red
	dsp_image_ex(0x00,0x01);	// Image 1			
	dsp_image_ex(0x01,0x02);	// Image 1			
	dsp_image_ex(0x02,0x03);	// Image 1			
	dsp_image_ex(0x03,0x04);	// Image 1			
	dsp_image_ex(0x04,0x05);	// Image 1			


	while(1)
	{
		process_uart();
		process_timer();
	}

}

// 
// Function start here
//

void process_timer(void)
{
	static unsigned char tCnt=0;
	static unsigned char tDebugCnt=0;

	if (mTMR0_Flag)		// @ 100mS
	{
		mTMR0_Flag = 0;
		tCnt ++;
		if (tCnt >=20)
		{
			task_1s();
			tCnt = 0;
			tDebugCnt++;
//			u_putHexByte(tDebugCnt,0x00);
//			u_puts("\r\n");
		}
	
	}


}

#define test_bl 
//#define test_rgb 

void task_led_test(void)
{

	static unsigned char tCnt=0;

#ifdef test_bl
	tCnt &=0x01;
	switch (tCnt)
	{
		case 0:
			LED_BL_ON();
			break;
		case 1:
			LED_BL_OFF();
			break;			
	}
#endif

#ifdef test_rbb

	tCnt &=0x07;
	switch (tCnt)
	{
		case 0:
			LED_R_ON();
			LED_G_OFF();
			LED_B_OFF();
			break;
		case 1:
			LED_R_OFF();
			LED_G_ON();
			LED_B_OFF();
			break;
		case 2:
			LED_R_OFF();
			LED_G_OFF();
			LED_B_ON();			
			break;
		case 3:
			LED_R_ON();
			LED_G_ON();
			LED_B_ON();			
			break;
		case 4:
			LED_R_OFF();
			LED_G_OFF();
			LED_B_OFF();	
			break;
		case 5:
			LED_R_ON();
			LED_G_ON();
			LED_B_OFF();			
			break;
		case 6:
			LED_R_ON();
			LED_G_OFF();
			LED_B_ON();			
			break;
		case 7:
			LED_R_OFF();
			LED_G_ON();
			LED_B_ON();			
			break;
		default:
			break;
	}
#endif
	tCnt++;

}

void task_lcd_test(void)
{

	static unsigned char tCnt=0;

	tCnt &= 0x03;
	switch (tCnt)
	{
		case 0:
//	   		dsp_single_colour(0x00,0x00);	//Black
	   		dsp_single_colour_ex(0x00,0x00,0x00);	//Black
			dsp_image_ex(0x01,0x00);	// Image 0			
	   		dsp_single_colour_ex(0x02,0xff,0xff);	//White
			break;
		case 1:
//    		dsp_single_colour(0x00,0x00);	//Black
//    		dsp_single_colour(0xf8,0x00);	//Red
    		dsp_single_colour_ex(0x00,0xf8,0x00);	//Red
			dsp_image_ex(0x01,0x01);	// Image 1			
	   		dsp_single_colour_ex(0x03,0x55,0x55);	//Grey

			break;			
		case 2:
//    		dsp_single_colour(0x07,0xe0);	//Green
    		dsp_single_colour_ex(0x00,0x07,0xe0);	//Green
			dsp_image_ex(0x01,0x02);	// Image 2			
	   		dsp_image_ex(0x04,0x01);	//Grey
			break;
		case 3:
//    		dsp_single_colour(0x00,0x1f);		//Blue
    		dsp_single_colour_ex(0x00,0x1f,0xe0);	//Green
			dsp_image_ex(0x01,0x03);	// Image 3			
			break;			
	}

	tCnt++;
}

void task_1s(void)
{

	task_led_test();
	task_lcd_test();


}

void process_uart(void)
{
	unsigned char i;

	if (mCmdQ[0]>0x80)	
	{
		for (i=1; i<=(mCmdQ[0] & 0x7f); i++)
		{
			u_putch(mCmdQ[i]);	
		}
		u_puts("\r\n");
		mCmdQ[0] = 0x00;
	}

}



