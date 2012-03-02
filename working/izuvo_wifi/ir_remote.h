
#define CaptureFreq 12000000/(64*4)


extern volatile unsigned int 	mPulse[2];
extern volatile unsigned int 	mPulseCnt;
extern volatile unsigned char	mTxFlag;
extern volatile unsigned int  	mPulseFreq;	
extern volatile unsigned int  	mPulseTable[8][2];
extern volatile unsigned char 	mPulseCode[32];	
extern volatile unsigned char 	mPulseLen;	
extern volatile unsigned char 	mPulseIndex;
extern volatile unsigned char  	mPulseRepeat;

extern volatile unsigned int 	mCapturePulse[200];
extern volatile unsigned int 	mCaptureIndex;
extern volatile unsigned char 	mCaptureState;


void process_remote_command(char *input);


void ir_send_pulse(unsigned int ipulse0, unsigned int ipulse1);
void ir_send_pulseEx(void);
void ir_init_send_pulse(char *input);
void ir_wait_tx(void);


void ir_init_capture_pulse(char *input);
void ir_capture_pulse(void);


//void decode_pronto_code(void);
void decode_pronto_code(char *input);
void decode_cpronto_code(char *input);


