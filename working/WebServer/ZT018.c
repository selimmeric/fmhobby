// ZT018 Driver

// 引入相关芯片的头文件 
#include <LPC2103.H>
#include "types.h"

#define BIT(x) (1 << x)

#define I2C_EN BIT(6)
#define I2C_STA BIT(5)
#define I2C_STO BIT(4)
#define I2C_SI BIT(3)
#define I2C_AA BIT(2)

#define LCD_WRITE	0x50
#define LCD_READ	0x51

void LCD_I2C0Init(void) {
	PINSEL0 |= 0x00000050;
	I2C0SCLH = (60000000/400000 + 1) / 2;
	I2C0SCLL = (60000000/400000) / 2;
	I2C0CONCLR = I2C_STA | I2C_STO | I2C_SI | I2C_AA;
	I2C0CONSET = I2C_EN;
	}//I2C0Init

void I2CwriteByte( U8 data ) {
	I2C0DAT = data; //load x0
	I2C0CONCLR = I2C_SI; //send data
	while (~I2C0CONSET & I2C_SI); //wait
	}
void I2CwriteWord( U16 data ) {
	I2C0DAT = data>>8; //load x0
	I2C0CONCLR = I2C_SI; //send data
	while (~I2C0CONSET & I2C_SI); //wait
	I2C0DAT = data; //load x0
	I2C0CONCLR = I2C_SI; //send data
	while (~I2C0CONSET & I2C_SI); //wait
	}
void I2Cstop(void) {
	I2C1CONSET = I2C_STO; //setup stop condition
	I2C1CONCLR = I2C_SI; //start sending stop
	while (I2C1CONSET & I2C_STO); //wait for stop to be sent
	}
void LCD_WriteCmd( U8 Cmd ) {
	volatile unsigned int s;
	do {
		I2C0CONSET = I2C_STA | I2C_SI; //setup start condition
		I2C0CONCLR = I2C_SI; //send start condition
		s =  I2C0CONSET;
		while (~I2C0CONSET & I2C_SI)
		 s=I2C0CONSET; //wait for start to be sent
		I2C0DAT = LCD_WRITE; //load the LCD address
		I2C0CONCLR = I2C_STA | I2C_SI; //send EEPROM address
		while (~I2C0CONSET & I2C_SI); //wait for address to be sent
		s=I2C0STAT;
		}while (I2C0STAT != 0x18); //repeat if no ACK received
	I2CwriteByte( Cmd );
	}
unsigned char I2CReadByte (U8 Cmd) {
	volatile unsigned int s;
	unsigned char Data;
	LCD_WriteCmd( Cmd );

	do {
		I2C0CONSET = I2C_STA | I2C_SI; //setup start condition
		I2C0CONCLR = I2C_SI; //send start condition
		while (~I2C0CONSET & I2C_SI); //wait for start to be sent
		I2C0DAT = LCD_READ; //load address of eeprom
		I2C0CONCLR = I2C_STA | I2C_SI; //start sending address
		while (~I2C0CONSET & I2C_SI); //wait for address to be sent
		} while (I2C0STAT != 0x40); //repeat if no ack received
	I2C0CONCLR = I2C_SI; //start reading data
	while (~I2C0CONSET & I2C_SI); //wait for data to be received
	Data = I2C0DAT;
	I2C0CONSET = I2C_STO; //setup stop condition
	I2C0CONCLR = I2C_SI; //send stop condition
	while (I2C0CONSET & I2C_STO); //wait for stop to be sent
	return Data; //return the data
	}//ReadEEPROM

U8 LCD_Status_Read( void ) {
	return I2CReadByte(0);
	}

void LCD_Write1coor( U8 Cmd, U8 x0, U8 y0 ) {
	LCD_WriteCmd( Cmd );
	I2CwriteByte( x0 );
	I2CwriteByte( y0 );
	}
void LCD_Write2coor( U8 Cmd, U8 x0, U8 y0, U8 x1, U8 y1 ) {
	LCD_WriteCmd( Cmd );
	I2CwriteByte( x0 );
	I2CwriteByte( y0 );
	I2CwriteByte( x1 );
	I2CwriteByte( y1 );
	}

void LCD_BMP( U8 x0, U8 y0, U8 x1, U8 y1 , U8 * Bmp ) {
	S16 i,j;

	while( LCD_Status_Read()&0x80 );	// wait while lcd busy

	LCD_Write2coor( 0x10, x0,y0,x1,y1 );
	for( i = y1 - y0 + 1; i >= 0; i-- )
		for( j = 0; j <= x1 - x0; j++ ) {
			U16* PixP =  (U16*)(Bmp+0x46+i*(x1-x0+1)*2);
			U16 Pix = *(PixP+j);

			I2C0DAT = Pix>>8; //load high
			I2C0CONCLR = I2C_SI; //send data
			while (~I2C0CONSET & I2C_SI); //wait
			I2C0DAT = Pix; //load high
			I2C0CONCLR = I2C_SI; //send data
			while (~I2C0CONSET & I2C_SI); //wait
			}
	I2Cstop();
	}
void LCD_Pixel( U8 x, U8 y, U16 Color ) {
	while( LCD_Status_Read()&0x80 );	// wait while lcd busy
	LCD_Write1coor( 0x11, x, y );
	I2CwriteWord( Color );
	I2Cstop();
	}
void LCD_Line( U8 x0, U8 y0, U8 x1, U8 y1 , U16 Color ) {
	while( LCD_Status_Read()&0x80 );	// wait while lcd busy
	LCD_Write2coor( 0x12, x0,y0,x1,y1 );
	I2CwriteWord( Color );
	I2Cstop();
	}
void LCD_Rectangle( U8 x0, U8 y0, U8 x1, U8 y1 , U16 Color ) {
	while( LCD_Status_Read()&0x80 );	// wait while lcd busy
	LCD_Write2coor( 0x13, x0,y0,x1,y1 );
	I2CwriteWord( Color );
	I2Cstop();
	}
void LCD_RectangleFill( U8 x0, U8 y0, U8 x1, U8 y1 , U16 Color ) {
	while( LCD_Status_Read()&0x80 );	// wait while lcd busy
	LCD_Write2coor( 0x14, x0,y0,x1,y1 );
	I2CwriteWord( Color );
	I2Cstop();
	}
void LCD_Circle( U8 x0, U8 y0, U8 r, U16 Color ) {
	while( LCD_Status_Read()&0x80 );	// wait while lcd busy

	LCD_Write1coor( 0x15, x0,y0 );
	I2CwriteByte( r );
	I2CwriteWord( Color );
	I2Cstop();
	}
void LCD_CircleFill( U8 x0, U8 y0, U8 r, U16 Color ) {
	while( LCD_Status_Read()&0x80 );	// wait while lcd busy

	LCD_Write1coor( 0x16, x0,y0 );
	I2CwriteByte( r );
	I2CwriteWord( Color );
	I2Cstop();
	}
void LCD_Ellipse( U8 x0, U8 y0, U8 x1, U8 y1 , U16 Color ) {
	while( LCD_Status_Read()&0x80 );	// wait while lcd busy
	LCD_Write2coor( 0x17, x0,y0,x1,y1 );
	I2CwriteWord( Color );
	I2Cstop();
	}
void LCD_EllipseFill( U8 x0, U8 y0, U8 x1, U8 y1 , U16 Color ) {
	while( LCD_Status_Read()&0x80 );	// wait while lcd busy
	LCD_Write2coor( 0x18, x0,y0,x1,y1 );
	I2CwriteWord( Color );
	I2Cstop();
	}
void LCD_Pieslice( U8 x0, U8 y0, U8 r, U16 start, U16 end, U16 Color ) {
	while( LCD_Status_Read()&0x80 );	// wait while lcd busy

	LCD_Write1coor( 0x19, x0,y0 );
	I2CwriteByte( r );
	I2CwriteWord( start );
	I2CwriteWord( end );
	I2CwriteWord( Color );
	I2Cstop();
	}
void LCD_MoveTo( U8 x, U8 y ) {
	while( LCD_Status_Read()&0x80 );	// wait while lcd busy
	LCD_Write1coor( 0x30, x, y );
	I2Cstop();
	}
void LCD_PrintU8( U8 data ) {
	while( LCD_Status_Read()&0x80 );	// wait while lcd busy

	LCD_WriteCmd( 0x61 );
	I2CwriteByte( data );
	I2Cstop();
	}
void LCD_PrintX8( U8 data ) {
	while( LCD_Status_Read()&0x80 );	// wait while lcd busy

	LCD_WriteCmd( 0x63 );
	I2CwriteByte( data );
	I2Cstop();
	}
void LCD_SetFcolor( U16 Color ) {
	while( LCD_Status_Read()&0x80 );	// wait while lcd busy
	LCD_WriteCmd( 0x31 );
	I2CwriteWord( Color );
	I2Cstop();
	}
void LCD_SetBcolor( U16 Color ) {
	while( LCD_Status_Read()&0x80 );	// wait while lcd busy
	LCD_WriteCmd( 0x32 );
	I2CwriteWord( Color );
	I2Cstop();
	}
void LCD_SetFont( U8 data ) {
	while( LCD_Status_Read()&0x80 );	// wait while lcd busy

	LCD_WriteCmd( 0x80 );
	I2CwriteByte( data );
	I2Cstop();
	}

void LCD_PutString( char *s ) {
	while( *s ) {
		while( LCD_Status_Read()&0x80 );	// wait while lcd busy
		LCD_WriteCmd( 0x60 );
		I2CwriteByte( *s );
		++s;
		I2Cstop();
		}
	}
void putch( char c ) {
	while( LCD_Status_Read()&0x80 );	// wait while lcd busy

	LCD_WriteCmd( 0x81 );
	I2CwriteByte( c );
	I2Cstop();
	}

void PutString( U8 x, U8 y, char *s, U16 F, U16 B ) {
	LCD_MoveTo( x, y );
	LCD_SetFcolor( F );
	LCD_SetBcolor( B );
	LCD_PutString( s );
	}
void LCD_Clear_Screen( U16 B ) {
	LCD_RectangleFill( 0, 0, 159, 127, B );
	}

void printStr( char *s ) {
	while( *s ) {
		putch( *s++ );
		}
	}
