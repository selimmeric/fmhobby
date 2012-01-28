
#include <REG52.H>                /* special function register declarations   */
                                  /* for the intended 8051 derivative         */
                                  /* is enabled		                          */
								  
#include <intrins.h>
#include "utility.h"


#define FOSC 2*22118400L
#define BAUD 115200
//#define BAUD 57600

#define T1MS (65536 - FOSC/12/1000)
#define T500US (65535 - (51+(256*7)))


#define G_DISABLE	(P1 = 0x00);
#define G0_H		(P1 = 0x01);
#define G1_H		(P1 = 0x02);
#define G2_H		(P1 = 0x04);
#define G3_H		(P1 = 0x08);
#define G4_H		(P1 = 0x10);
#define G5_H		(P1 = 0x20);
#define G6_H		(P1 = 0x40);
#define G7_H		(P1 = 0x80);

#define S_ENA_H 	(P3 |= 0x04);
#define S_ENA_L 	(P3 &= ~0x04);
#define S_DATA_H 	(P3 |= 0x08);
#define S_DATA_L 	(P3 &= ~0x08);
#define S_LATCH_H 	(P3 |= 0x20);
#define S_LATCH_L 	(P3 &= ~0x20);
#define S_LATCH\
		(P3 |= 0x20);\
		(P3 &= ~0x20);
#define S_CLK\
		P3 |= 0x10;\
		P3 &= ~0x10;



void uart_rx(void);
