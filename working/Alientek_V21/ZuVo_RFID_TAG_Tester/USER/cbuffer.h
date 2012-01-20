
void cbuffer_init(unsigned char *cbuf, unsigned char size);
unsigned char cbuffer_rd (unsigned char * cbuf);
void cbuffer_wr (unsigned char * cbuf, unsigned char data);
unsigned char cbuffer_next_pt(unsigned char cpt, unsigned char size);
unsigned char cbuffer_is_empty(unsigned char * cbuf);

