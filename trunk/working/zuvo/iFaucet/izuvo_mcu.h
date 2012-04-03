
#define CaptureFreq 12000000/(64*4)

#define unit_tol 0x08
#define unit_base 0x13




extern volatile unsigned int 	mPulse[2];
extern volatile unsigned int 	mPulseCnt;
extern volatile unsigned char	mTxFlag;
extern volatile unsigned int  	mPulseFreq;	
extern volatile unsigned int  	mPulseTable[8][2];
//extern volatile unsigned char 	mPulseCode[32];	
extern volatile unsigned char 	mPulseLen;	
extern volatile unsigned char 	mPulseIndex;
extern volatile unsigned char  	mPulseRepeat;

extern volatile unsigned int 	mCapturePulse[200];
extern volatile unsigned int 	mCaptureIndex;
extern volatile unsigned char 	mCaptureState;


void process_remote_command(char *input);


void ir_wait_tx(void);


void ir_init_capture_pulse(char *input);
void ir_capture_pulse(void);
unsigned char izuvo_search_header(void);
void izuvo_rx_pulse(unsigned int iPulse);
unsigned char izuvo_rx_unit(unsigned int iCnt);
void izuvo_init_capture_pulse(void);




