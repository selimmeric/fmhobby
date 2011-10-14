#include <stdio.h>
#include <string.h>
#include "ip_arp_udp_tcp.h"
#include "enc28j60.h"
#include "net.h"
#include <LPC2103.H>
#include "utility.h"

#define MYWWWPORT 80
#define PSTR(s) s

//#define debug_ethernet
//#define debug_ip
#define debug_icmp
#define debug_tcp
#define debug_udp

#define VERSION "Embedded Ethernet Server "__DATE__ "/" __TIME__ "\r\n"
#define BUFFER_SIZE 1500//450
static uint8_t buf[BUFFER_SIZE+1];
#define LED  ( 1 << 17 )        //P0.17¿ØÖÆLED
extern void delay_ms(unsigned char ms);
extern int8_t analyse_get_url(char *str);
extern uint16_t moved_perm(uint8_t *buf);
extern uint16_t print_webpage(uint8_t *buf,uint8_t on_off);




inituart(){
	U0FCR = 0xF;
	U0LCR = 0x83;
	//DLL = 0x27; // For 19200 BAUD with input clock = 12 MHz
	U0DLL = 0x0D; // For 57600 BAUD with input clock = 12 MHz
	U0DLM = 0x00; 
	U0LCR = 0x03;

 	PINSEL0|=0x00000005; 
	PINSEL1|=0x00000000; 
	U0LCR=0x83; 
	U0DLM=0x00; 
//	U0DLL=0x48; 
//	U0DLL=0xc2;		// 9600
	U0DLL=0x10;		// 115200
	U0LCR=0x03; 

}


printUART(char a[])
{ 
	int i = 0;
	while(a[i])
	{
	U0THR=a[i];
	while(!(U0LSR & (1<<6))){}
	i++;
	}
}

UART0_Tx(char ch)
{
	while(!(U0LSR & (1<<6))){}
	U0THR=ch;
}

void decodeICMP(uint16_t len,uint8_t* data)
{

	unsigned char i;
	unsigned int plen=0;

#ifdef debug_icmp
	u_puts("ICMP:TYPE|CODE|CS|QUENCH=");
	u_putHexByte(data[IP_PL],'|');
	u_putHexByte(data[IP_PL+1],'|');
	u_putHexByte(data[IP_PL+2],0x00);
	u_putHexByte(data[IP_PL+3],'|');
	u_putHexByte(data[IP_PL+4],0x00);
	u_putHexByte(data[IP_PL+5],0x00);
	u_putHexByte(data[IP_PL+6],0x00);
	u_putHexByte(data[IP_PL+7],'|');
	u_puts("\r\n");
	u_puts("  PL>00:"); 
	i = 0;
	plen = (data[ETH_HEADER_LEN+2]<<8) + data[ETH_HEADER_LEN+3]-20-8;
 
	while (i<plen)
	{
		u_putHexByte(data[IP_PL+8+i],'-');
		if ((i&0x0f)==0x0f)
			{
				u_puts("\r\n     ");
				u_putHexByte(i+1,':');
			}
		i++;
	}
 	u_puts("\r\n");

#endif 	

make_echo_reply_from_request(buf,len);

}

// prepare the webpage by writing the data to the tcp send buffer
uint16_t print_webcgi(uint8_t *buf,uint8_t on_off)
{
        uint16_t plen;
        plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n"));
        plen=fill_tcp_data_p(buf,plen,PSTR("myOutput is: "));
        if (on_off){
                plen=fill_tcp_data_p(buf,plen,PSTR("ON"));
        }else{
                plen=fill_tcp_data_p(buf,plen,PSTR("OFF"));
        }
        return(plen);
}

processTCP(uint16_t len,uint8_t* data)
{
    uint16_t plen;
    uint16_t dat_p;
    uint8_t i=0;
    int8_t cmd;

    // tcp port www start, compare only the lower byte
    if (buf[IP_PROTO_P]==IP_PROTO_TCP_V&&buf[TCP_DST_PORT_H_P]==0&&buf[TCP_DST_PORT_L_P]==MYWWWPORT){
            if (buf[TCP_FLAGS_P] & TCP_FLAGS_SYN_V){
                    make_tcp_synack_from_syn(buf);
                    // make_tcp_synack_from_syn does already send the syn,ack
                    return;
            }
            if (buf[TCP_FLAGS_P] & TCP_FLAGS_ACK_V){
                    init_len_info(buf); // init some data structures
                    // we can possibly have no data, just ack:
                    dat_p=get_tcp_data_pointer();
                    if (dat_p==0){
                            if (buf[TCP_FLAGS_P] & TCP_FLAGS_FIN_V){
                                    // finack, answer with ack
                                    make_tcp_ack_from_any(buf);
                            }
                            // just an ack with no data, wait for next packet
                            return;
                    }
                    if (strncmp("GET ",(char *)&(buf[dat_p]),4)!=0){
                            // head, post and other methods:
                            //
                            // for possible status codes see:
                            // http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
                            plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>200 OK</h1>"));
                            goto SENDTCP;
                    }
                    if (strncmp("/ ",(char *)&(buf[dat_p+4]),2)==0){
                            plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"));
                            plen=fill_tcp_data_p(buf,plen,PSTR("<p>Usage: http://host_or_ip/password</p>\n"));
                            goto SENDTCP;
                    }
                    cmd=analyse_get_url((char *)&(buf[dat_p+5]));
                    // for possible status codes see:
                    // http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
                    if (cmd==-1){
                            plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 401 Unauthorized\r\nContent-Type: text/html\r\n\r\n<h1>401 Unauthorized</h1>"));
                            goto SENDTCP;
                    }
                    if (cmd==1){
                            //PORTD|= (1<<PD7);// transistor on
							IOCLR = LED;
							i = 1;
                    }
                    if (cmd==0){
                            //PORTD &= ~(1<<PD7);// transistor off
							IOSET = LED;
							i = 0;
                    }
                    if (cmd==-3){
                            // redirect to add a trailing slash
                            plen=moved_perm(buf);
                            goto SENDTCP;
                    }
                    // if (cmd==-2) or any other value
                    // just display the status:
                    //plen=print_webpage(buf,(PORTD & (1<<PD7)));
					plen=print_webcgi(buf,(i));
                    //
SENDTCP:
                    make_tcp_ack_from_any(buf); // send ack for http get
                    make_tcp_ack_with_data(buf,plen); // send data
                    return;
            }

    }
    // tcp port www end


}

void decodeTCP(uint16_t len,uint8_t* data)
{

	unsigned char i;
	unsigned int plen=0;

#ifdef debug_tcp

	u_puts("TCP :SP|DP|SQ|ACK=");
	u_putHexByte(data[IP_PL],0x00);
	u_putHexByte(data[IP_PL+1],'|');
	u_putHexByte(data[IP_PL+2],0x00);
	u_putHexByte(data[IP_PL+3],'|');
	u_putHexByte(data[IP_PL+4],0x00);
	u_putHexByte(data[IP_PL+5],0x00);
	u_putHexByte(data[IP_PL+6],0x00);
	u_putHexByte(data[IP_PL+7],'|');
	u_putHexByte(data[IP_PL+8],0x00);
	u_putHexByte(data[IP_PL+9],0x00);
	u_putHexByte(data[IP_PL+10],0x00);
	u_putHexByte(data[IP_PL+11],'|');
	u_puts("\r\n");
	u_puts("TCP :OS|FL|WS|CS|PT=");
	u_putHexByte(data[IP_PL+12],'|');
	u_putHexByte(data[IP_PL+13],'|');
	u_putHexByte(data[IP_PL+14],0x00);
	u_putHexByte(data[IP_PL+15],'|');
	u_putHexByte(data[IP_PL+16],0x00);
	u_putHexByte(data[IP_PL+17],'|');
	u_putHexByte(data[IP_PL+18],0x00);
	u_putHexByte(data[IP_PL+19],'|');
	u_puts("\r\n");

	u_puts("  PL:"); 
	i = 0;
	plen = len - (IP_PL+20);
 
	while (i<plen)
	{
		u_putHexByte(data[UDP_SRC_PORT_H_P+20+i],'-');
		if ((i&0x0f)==0x0f)
			{
				u_puts("\r\n     ");
			}
		i++;
	}
	u_puts("\r\n");
#endif

}

void decodeUDP(uint16_t len,uint8_t* data)
{

	unsigned char i;
	unsigned int plen=0;
	char str[100];
	char msg[100];


#ifdef debug_udp

	u_puts("UDP :SP|DP|Len|CS=");
	u_putHexByte(data[IP_PL],0x00);
	u_putHexByte(data[IP_PL+1],'|');
	u_putHexByte(data[IP_PL+2],0x00);
	u_putHexByte(data[IP_PL+3],'|');
	u_putHexByte(data[IP_PL+4],0x00);
	u_putHexByte(data[IP_PL+5],'|');
	u_putHexByte(data[IP_PL+6],0x00);
	u_putHexByte(data[IP_PL+7],'|');
	u_puts("\r\n");

	u_puts("  PL>00:"); 
	i = 0;
	plen = (data[IP_PL+4]<<8) + 	data[IP_PL+5]-8;

	while (i<plen)
	{
		u_putHexByte(data[IP_PL+8+i],'-');
		if ((i&0x0f)==0x0f)
			{
				u_puts("\r\n     ");
				u_putHexByte(i+1,':');
			}
		i++;
	}

	u_puts("\r\n");
#endif

	memset(msg,0x00,sizeof(msg));
	memcpy(msg,&data[IP_PL+8],plen);

	sprintf(str,"UDP Echo : %s\r\n",msg);
	u_puts(str);
	make_udp_reply_from_request(buf,str,strlen(str),8888);

}

void decodePacket(uint16_t len, uint8_t* data)
{
	unsigned char i;
	unsigned int plen=0;
	static unsigned char tCnt=0;
	tCnt ++;
/*
	u_puts("myPacket : ");
	u_putHexByte(len,'-');
	for (i=0; i<len; i++)
	{
		u_putHexByte(data[i],0x00);
	}			
	u_puts("\r\n");

	u_puts("D_MAC : ");
	for (i=0; i<6; i++)
	{
		u_putHexByte(data[i],0x00);
	}
	u_puts("\r\n");

	u_puts("S_MAC : ");
	for (i=6; i<12; i++)
	{
		u_putHexByte(data[i],0x00);
	}
	u_puts("\r\n");

	u_puts("Type/Length : ");	  // 0x0800 - IPV4
	for (i=12; i<14; i++)
	{
		u_putHexByte(data[i],0x00);
	}
	if ((data[12]==0x08) && (data[13]==0x00))
	{
		u_puts(" IPV4 ");
	}
	u_puts("\r\n");
	u_puts("IP Packet : ");
	u_puts("VER|IHL|TOS|LEN|="); 
	u_putHexByte(data[IP_P]>>4,'|');
	u_putHexByte(data[IP_P]&0xf,'|');
	u_putHexByte(data[IP_P+1],'|');
	u_putHexByte(data[IP_P+2],0x00);
	u_putHexByte(data[IP_P+3],'|');
	u_puts("\r\n");

	u_puts("ID|FLG|FRAG|="); 
	u_putHexByte(data[IP_P+4],0x00);
	u_putHexByte(data[IP_P+5],'|');
	u_putHexByte(data[IP_P+6]>>5,'|');
	u_putHexByte(data[IP_P+6]&0x1f,0x00);
	u_putHexByte(data[IP_P+7],'|');
	u_puts("\r\n");
	 
	u_puts("TTL|PRO|HCS="); 
	u_putHexByte(data[IP_P+8],'|');
	u_putHexByte(data[IP_P+9],'|');
	u_putHexByte(data[IP_P+10],0x00);
	u_putHexByte(data[IP_P+11],'|');
	u_puts("\r\n");
  */
  	u_puts("IP  :");
  	u_putHexByte(tCnt,':');
	u_puts("SIP|DIP|PRO|LEN="); 
	u_putDecByte(data[IP_P+12]); 	u_puts(".");
	u_putDecByte(data[IP_P+13]);	u_puts(".");
	u_putDecByte(data[IP_P+14]);	u_puts(".");
	u_putDecByte(data[IP_P+15]);	u_puts("|");
	u_putDecByte(data[IP_P+16]);	u_puts(".");
	u_putDecByte(data[IP_P+17]);	u_puts(".");
	u_putDecByte(data[IP_P+18]);	u_puts(".");
	u_putDecByte(data[IP_P+19]);	u_puts("|");

	u_putHexByte(data[IP_P+9],'|');

	u_putHexByte(data[IP_P+2],0x00);
	u_putHexByte(data[IP_P+3],'|');
	u_puts("\r\n");

	u_puts("  PL>00:"); 

	i = 0;
	plen = (data[IP_P+2]<<8) + 	data[IP_P+3]-20;

	while (i<plen)
	{
		u_putHexByte(data[IP_P+20+i],'-');
		if ((i&0x0f)==0x0f)
			{
				u_puts("\r\n     ");
				u_putHexByte(i+1,':');
			}
		i++;
	}

	u_puts("\r\n");

}

void myServerTask(void)
{
	uint16_t plen;
	inituart();

	u_puts(VERSION);

	while(1)
	{
/*
		IOSET = LED;
//		IOSET = 1;
		delay_ms(200);
		IOCLR = LED;				
//		IOCLR = 1;				
		delay_ms(200);	
*/
	        // get the next new packet:
	    plen = enc28j60PacketReceive(BUFFER_SIZE, buf);
	
	    /*plen will ne unequal to zero if there is a valid 
	     * packet (without crc error) */
	    if(plen==0){
				//printStr( "no data\n" );
	            continue;
	    }
	    // arp is broadcast if unknown but a host may also
	    // verify the mac address by sending it to 
	    // a unicast address.
	    if(eth_type_is_arp_and_my_ip(buf,plen)){
	            make_arp_answer_from_request(buf);
				//printStr( "make_arp_answer_from_request\n" );
	            continue;
	    }
	
	    // check if ip packets are for us:
	    if(eth_type_is_ip_and_my_ip(buf,plen)==0){
	            continue;
	    }
	
#ifdef debug_ethernet
  				decodePacket(plen,buf);
#endif

		switch (buf[IP_PROTO_P])
		{
			case IP_PROTO_ICMP_V :
				decodeICMP(plen,buf);
				break;
			case IP_PROTO_UDP_V:
				decodeUDP(plen,buf);
				break;
			case IP_PROTO_TCP_V:
				decodeTCP(plen,buf);
				processTCP(plen,buf);
				break;	
			default:

				break;	
		}
/*
	    if(buf[IP_PROTO_P]==IP_PROTO_ICMP_V && buf[ICMP_TYPE_P]==ICMP_TYPE_ECHOREQUEST_V){
	            // a ping packet, let's send pong
	            make_echo_reply_from_request(buf,plen);

				sprintf(str,"ICMP:Ping Received.. \r\n");
				u_puts(str);
//                make_udp_reply_from_request(buf,str,strlen(str),8888);
				//printStr( "make_echo_reply_from_request\n" );
	            continue;
	    }

        // decode udp packet
        if (buf[IP_PROTO_P]==IP_PROTO_UDP_V){
				decodeUDP(plen,buf);
        }

        // decode tcp packet
        if (buf[IP_PROTO_P]==IP_PROTO_TCP_V){
				decodeTCP(plen,buf);
        }
	
 */
	
	 }
}
