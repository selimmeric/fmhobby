#include "izuvo_stc.h"

void UART_one_Interrupt_Receive(void) interrupt 4
{
    unsigned char   k   =   0;
    if(RI==1)
    {
        RI  =   0;
        k   =   SBUF;
        u_putch(k+1);
    }
    else
    {
        TI  =  0;
    }
}
