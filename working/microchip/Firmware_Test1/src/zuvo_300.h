
//#define CFG_300
//#define CFG_150

// define macro	

#define  testbit(var, bit)   ((var) & (1 <<(bit)))
#define  setbit(var, bit)    ((var) |= (1 << (bit)))
#define  clrbit(var, bit)    ((var) &= ~(1 << (bit)))


// PORTA
#define PIN_DATA  	5
#define PIN_CMD  	4
#define PIN_FS		2

// PORTC
#define PIN_BUZZER  5
#define PIN_RED 	4
#define PIN_GREEN   3
#define PIN_BLUE  	2
#define PIN_CLK 	1
#define PIN_UVL   	0



// Define Constant
// unit @ 10mS
#define SYS_CYCLE			0.01024			// 10.24mS 
#define LED_ON_PER			(0.5/SYS_CYCLE)	// 0.5S	
#define LED_OFF_PER			(1.5/SYS_CYCLE)	// 1.5S
#define BUZZER_ON_PER		(1.5/SYS_CYCLE)	// 1.5S
#define BUZZER_OFF_PER		(1.5/SYS_CYCLE)	// 1.5S



#define FS_CPL				2724					// 2724 Count = 1L
#define Filter_Life			1000 * 3.78 			// 1000 Gallon
#define Filter_Life_0		Filter_Life * 0.004 	// 15L   = 0x000f	
#define Filter_Life_0_LED	0x11					// Blue - Blink
#define Filter_Life_1		Filter_Life * 0.9 		// 3402L = 0x0D4A	0x0D40
#define Filter_Life_1_LED	0x01					// Blue - Solid
#define Filter_Life_2		Filter_Life 			// 3780L = 0x0EC4	0x0EB0
#define Filter_Life_2_LED	0x06					// Yellow (R+G) - Solid
#define Filter_Life_3		Filter_Life * 1.1		// 4158L = 0x103E	0x1020
#define Filter_Life_3_LED	0x04					// Red - Solid
#define Filter_Life_4_LED	0x14					// Red - Blink


#define FlowStop_Delay		2000					// 20S = 20/10mS 


/*
LED Blinking	0.5s On 1.5s Off


LED							Buzzer			Description
Blue LED blinking			No Buzzer		Initial setup or loss of power (changing locations etc.) Blinks for 5 minutes to indicate time needed to flush filter if new and/or replaced.
Blue LED solid				No Buzzer		LED changes to solid blue after blinking sequence and remains constantly on (whether in use or not) to indicate that the filter life is between 0 ?90% used.
Yellow LED solid			Buzzer sounds for 1.5 seconds at beginning of use and another instance at the end of use for 1.5 seconds.	LED changes to solid yellow and is constantly on (whether in use or not) to indicate that the filter life is between 90 ?100% used.
Red LED blinking			Buzzer sounds for 1.5 seconds at beginning of use and another instance at the end of use for 1.5 seconds.	LED changes to blinking red (cycles between 0.5 seconds on; 1.5 seconds off, 0.5 seconds on; 1.5 seconds off) and is constantly on (whether in use or not) to indicate that the filter life is between 90 ?100% used.
Red LED solid				Buzzer sounds for 1.5 seconds at beginning of use and another instance at the end of use for 1.5 seconds.	LED changes to solid red and is constantly on (whether in use or not) to indicate that the filter life is more than 110% used.
Red LED/Yellow LED Blinking	Buzzer sounds for 1.5 seconds at beginning of use and another instance at the end of use for 1.5 seconds.	LED changes to blinking sequence (red LED on 0.5 seconds; yellow LED on 0.5 seconds; red LED on 0.5 seconds; yellow LED on 0.5 seconds etc.) and is constantly on (whether in use or not) to indicate that the UVT% is inadequate for disinfection; turbid water or end of lamp life.
Red LED/Green LED Blinking	Buzzer sounds for 1.5 seconds at beginning of use and another instance at the end of use for 1.5 seconds.	LED changes to blinking sequence (red LED on 0.5 seconds; green LED on 0.5 seconds; red LED on 0.5 seconds; green LED on 0.5 seconds etc.) and is constantly on (whether in use or not) to indicate an error with RFID, lamp, ballast, UV sensor.

*/