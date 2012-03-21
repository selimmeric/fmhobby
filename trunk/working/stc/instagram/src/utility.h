#include "avr_types.h"

#define false 0

INT16U  u_abs(INT16S a);
INT16S  u_sign(INT16S x);
INT8U   u_toupper(INT8U c);
INT8U   u_hex2dec(INT8U c);
INT16U  u_asc2uint(INT8U len, INT8U *sptr);
void    u_putch(INT8U abyte);
void    u_puts( INT8U *sptr);
void    u_putHexByte(INT8U c, INT8U t); 
void    u_putHexWord(INT16U w);
void    u_putDecByte(INT8U num);
void    u_putDecWord(INT16U num); 
void    u_Lputch(INT8U abyte);
void    u_Delay(INT16U iDelay);
