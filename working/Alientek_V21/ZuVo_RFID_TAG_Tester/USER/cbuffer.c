#include "cbuffer.h"

void cbuffer_init(unsigned char *cbuf, unsigned char size)
{
	
	*cbuf = size;		// Buffer Size
	*(cbuf+1)  = 3;	   	// Read Pointer
	*(cbuf+2)  = 3;		// Write Pointer
}

unsigned char cbuffer_is_empty(unsigned char * cbuf)
{

	if (*(cbuf+1) == *(cbuf+2))
		return(1);
	else
		return(0);		

}

unsigned char cbuffer_is_full(unsigned char * cbuf)
{
	unsigned char tmp;
	tmp = cbuffer_next_pt(*(cbuf+2),*cbuf);
	if (tmp == *(cbuf+1))
		return (1);
	else
		return (0);

}


unsigned char cbuffer_rd (unsigned char * cbuf)
{
	unsigned char tmp;
	tmp = *(cbuf+1);

	if (*(cbuf+1) != *(cbuf+2))
	{
		tmp = cbuffer_next_pt(*(cbuf+1),*cbuf);
		*(cbuf+1) = tmp;
	}	

	return (*(cbuf+tmp));


}

void cbuffer_wr (unsigned char * cbuf, unsigned char data)
{

	unsigned char tmp;
	tmp = cbuffer_next_pt(*(cbuf+2),*cbuf);

	if (tmp != *(cbuf+1))
	{
		*(cbuf+2) = tmp;
		*(cbuf+tmp) = data;
	 }


}

unsigned char cbuffer_next_pt(unsigned char cpt, unsigned char size)
{
	unsigned char tmp;
	tmp = cpt+ 1;

	if (tmp>=size)
		return (3);
	else
		return (tmp);

}
