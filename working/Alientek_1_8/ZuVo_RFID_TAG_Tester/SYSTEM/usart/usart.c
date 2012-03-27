#include "sys.h"
#include "usart.h"

void cbuffer_init(unsigned char *cbuf, unsigned char size);
unsigned char cbuffer_rd (unsigned char * cbuf);
void cbuffer_wr (unsigned char * cbuf, unsigned char data);
unsigned char cbuffer_next_pt(unsigned char cpt, unsigned char size);




//Mini STM32������
//����1��ʼ��			    
//����ԭ��@ALIENTEK
//2010/5/27

//V1.3
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	USART1->DR = (u8) ch;      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////

#ifdef EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[UART_BUF_SIZE];     //���ջ���,���64���ֽ�.


//����״̬
//bit7��������ɱ�־
//bit6�����յ�0x0d
//bit5~0�����յ�����Ч�ֽ���Ŀ
u8 USART_RX_STA=0;       //����״̬���	  

  
void USART1_IRQHandlerOld(void)
{
	u8 res;	    
	if(USART1->SR&(1<<5))//���յ�����
	{	 
		res=(USART1->DR); 
		if((USART_RX_STA&0x80)==0)//����δ���
		{
			if(USART_RX_STA&0x40)//���յ���0x0d
			{
				if(res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x80;	//��������� 
			}else //��û�յ�0X0D
			{	
				if(res==0x0d)USART_RX_STA|=0x40;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3F]=res;
					USART_RX_STA++;
					if(USART_RX_STA>63)USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}  		 									     
	}  											 
} 
  	 
void USART1_IRQHandler(void)
{
//	u8 res;	    
	if(USART1->SR&(1<<5))//���յ�����
	{	 
		cbuffer_wr(USART_RX_BUF,(USART1->DR));
	}  											 
} 
#endif										 
//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������
//CHECK OK
//091209
void uart_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  
	RCC->APB2ENR|=1<<14;  //ʹ�ܴ���ʱ�� 
	GPIOA->CRH=0X444444B4;//IO״̬����
		  
	RCC->APB2RSTR|=1<<14;   //��λ����1
	RCC->APB2RSTR&=~(1<<14);//ֹͣ��λ	   	   
	//����������
 	USART1->BRR=mantissa; // ����������	 
	USART1->CR1|=0X200C;  //1λֹͣ,��У��λ.
#ifdef EN_USART1_RX		  //���ʹ���˽���
	//ʹ�ܽ����ж�
	USART1->CR1|=1<<8;    //PE�ж�ʹ��
	USART1->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(3,3,USART1_IRQChannel,2);//��2��������ȼ� 
	cbuffer_init(USART_RX_BUF,sizeof(USART_RX_BUF));
#endif

	

}


#ifdef EN_USART2_RX   //���ʹ���˽���
//����2�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART2_RX_BUF[UART_BUF_SIZE];     //���ջ���,���64���ֽ�.
//����״̬
//bit7��������ɱ�־
//bit6�����յ�0x0d
//bit5~0�����յ�����Ч�ֽ���Ŀ
u8 USART2_RX_STA=0;       //����״̬���	  
  
void USART2_IRQHandlerOld(void)
{
	u8 res;	    
	if(USART2->SR&(1<<5))//���յ�����
	{	 
		res=(USART2->DR); 
		if((USART2_RX_STA&0x80)==0)//����δ���
		{
			if(USART2_RX_STA&0x40)//���յ���0x0d
			{
				if(res!=0x0a)USART2_RX_STA=0;//���մ���,���¿�ʼ
				else USART2_RX_STA|=0x80;	//��������� 
			}else //��û�յ�0X0D
			{	
				if(res==0x0d)USART2_RX_STA|=0x40;
				else
				{
					USART2_RX_BUF[USART2_RX_STA&0X3F]=res;
					USART2_RX_STA++;
					if(USART2_RX_STA>63)USART2_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}  		 									     
	}  											 
} 

void USART2_IRQHandler(void)
{
	if(USART2->SR&(1<<5))//���յ�����
	{	 
		cbuffer_wr(USART2_RX_BUF,(USART2->DR));
	}  								 
} 
#endif	


//��ʼ��IO ����2
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������
//CHECK OK
//091209
void uart_init2(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
    mantissa<<=4;
	mantissa+=fraction; 


	RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  
	RCC->APB1ENR|=1<<17;  //ʹ�ܴ���ʱ�� 
	GPIOA->CRL=0X44444B44;//IO״̬����
		  
	RCC->APB1RSTR|=1<<17;   //��λ����2
	RCC->APB1RSTR&=~(1<<17);//ֹͣ��λ	   	   

	//����������

 	USART2->BRR=mantissa; // ����������	 
	USART2->CR1|=0X200C;  //1λֹͣ,��У��λ.
#ifdef EN_USART2_RX		  //���ʹ���˽���
	//ʹ�ܽ����ж�
	USART2->CR1|=1<<8;    //PE�ж�ʹ��
	USART2->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(3,3,USART2_IRQChannel,2);//��2��������ȼ� 
	cbuffer_init(USART2_RX_BUF,sizeof(USART2_RX_BUF));
#endif
}