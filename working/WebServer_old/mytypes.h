//----------------------------------------------------------------
// Header file to define commonly used Data Type
//----------------------------------------------------------------
#ifndef _avr_types_h
#define _avr_types_h

//typedef bit             BOOLEAN;	// Logical (TRUE or FALSE) 
typedef char   			INT8U;		// Unsigned 8 bit value 
typedef signed char     INT8S;		// Signed 8 bit value 
typedef unsigned short  INT16U;		// Unsigned 16 bit value 
typedef signed short    INT16S;		// Signed 16 bit value 
typedef unsigned long   INT32U;		// Unsigned 32 bit value 
typedef signed long     INT32S;		// Signed 32 bit value 
typedef float           FP24;		// Float 24 


typedef struct {
  float   batt_l;		// Battery Left
  float   batt_r;		// Battery Right
  float   pitch_rate;	// Gyroscop
  INT8U   ax;			// Accelerometer X
  INT8U   ay;			// Accelerometer Y
  INT8U   az;			// Accelerometer Z
} TYPE_IP_INPUT;


typedef struct 
{
    INT8U p;
    INT8U i;
    INT8U d;

} TYPE_PID;



/*
{
	unsigned char b0:1;
	unsigned char b1:1;
	unsigned char b2:1;
	unsigned char b3:1;
	unsigned char b4:1;
	unsigned char b5:1;
	unsigned char b6:1;
	unsigned char b7:1;
} TYPE_BYTE_FIELD;

typedef union 
{
	unsigned char byte;
	TYPE_BYTE_FIELD field;
} TYPE_BYTE_BIT;

typedef struct 
{
	unsigned char low;
	unsigned char high;
} TYPE_BYTE_WORD;

typedef union 
{
	TYPE_BYTE_WORD bytes;
	unsigned int unsignedWord;
	signed int signedWord;
} TYPE_BYTEWORD;

*/
#endif
//----------------------------------------------------------------
// End of File
//----------------------------------------------------------------

