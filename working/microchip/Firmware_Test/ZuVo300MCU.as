opt subtitle "HI-TECH Software Omniscient Code Generator (Lite mode) build 6446"

opt pagewidth 120

	opt lm

	processor	16F1823
clrc	macro
	bcf	3,0
	endm
clrz	macro
	bcf	3,2
	endm
setc	macro
	bsf	3,0
	endm
setz	macro
	bsf	3,2
	endm
skipc	macro
	btfss	3,0
	endm
skipz	macro
	btfss	3,2
	endm
skipnc	macro
	btfsc	3,0
	endm
skipnz	macro
	btfsc	3,2
	endm
indf	equ	0
indf0	equ	0
indf1	equ	1
pc	equ	2
pcl	equ	2
status	equ	3
fsr0l	equ	4
fsr0h	equ	5
fsr1l	equ	6
fsr1h	equ	7
bsr	equ	8
wreg	equ	9
intcon	equ	11
c	equ	1
z	equ	0
pclath	equ	10
	FNCALL	_main,_LED_RGB
	FNROOT	_main
	FNCALL	intlevel1,_isr
	global	intlevel1
	FNROOT	intlevel1
	global	_mFilter_acc_ft
	global	_mFilter_acc_fv
	global	_mFilter_cur_fc
	global	_mFilter_cur_ft
	global	_mRFID_CTag
	global	_mRFID_ETag
	global	_mFlowVol
	global	_mState_DelayCnt
	global	_mBuzzer
	global	_mTimer_Flag0
	global	main@tLEDCnt
	global	FS_RD@pFS
	global	_mFS_Cnt
	global	_mState_System
	global	_mState_UVL
	global	isr@pFS_IN
	global	isr@tCntA
	global	_mLED_Color
psect	nvBANK0,class=BANK0,space=1
global __pnvBANK0
__pnvBANK0:
_mLED_Color:
       ds      1

	global	_mUVL
_mUVL:
       ds      1

	global	_PORTA
_PORTA	set	12
	global	_GIE
_GIE	set	95
	global	_INTE
_INTE	set	92
	global	_T0IE
_T0IE	set	93
	global	_T0IF
_T0IF	set	90
	global	_OPTION_REG
_OPTION_REG	set	149
	global	_OSCCON
_OSCCON	set	153
	global	_TRISA
_TRISA	set	140
	global	_TRISC
_TRISC	set	142
	global	_INTEDG
_INTEDG	set	1198
	global	_LATA
_LATA	set	268
	global	_LATC
_LATC	set	270
	global	_WPUA
_WPUA	set	524
	file	"ZuVo300MCU.as"
	line	#
psect cinit,class=CODE,delta=2
global start_initialization
start_initialization:

psect	bssCOMMON,class=COMMON,space=1
global __pbssCOMMON
__pbssCOMMON:
FS_RD@pFS:
       ds      1

_mFS_Cnt:
       ds      1

_mState_System:
       ds      1

_mState_UVL:
       ds      1

isr@pFS_IN:
       ds      1

isr@tCntA:
       ds      1

psect	bssBANK0,class=BANK0,space=1
global __pbssBANK0
__pbssBANK0:
_mFilter_acc_ft:
       ds      4

_mFilter_acc_fv:
       ds      4

_mFilter_cur_fc:
       ds      4

_mFilter_cur_ft:
       ds      4

_mRFID_CTag:
       ds      4

_mRFID_ETag:
       ds      4

_mFlowVol:
       ds      2

_mState_DelayCnt:
       ds      2

_mBuzzer:
       ds      1

_mTimer_Flag0:
       ds      1

main@tLEDCnt:
       ds      1

psect clrtext,class=CODE,delta=2
global clear_ram
;	Called with FSR0 containing the base address, and
;	WREG with the size to clear
clear_ram:
	clrwdt			;clear the watchdog before getting into this loop
clrloop:
	clrf	indf0		;clear RAM location pointed to by FSR
	addfsr	0,1
	decfsz wreg		;Have we reached the end of clearing yet?
	goto clrloop	;have we reached the end yet?
	retlw	0		;all done for this memory range, return
; Clear objects allocated to COMMON
psect cinit,class=CODE,delta=2
	global __pbssCOMMON
	clrf	((__pbssCOMMON)+0)&07Fh
	clrf	((__pbssCOMMON)+1)&07Fh
	clrf	((__pbssCOMMON)+2)&07Fh
	clrf	((__pbssCOMMON)+3)&07Fh
	clrf	((__pbssCOMMON)+4)&07Fh
	clrf	((__pbssCOMMON)+5)&07Fh
; Clear objects allocated to BANK0
psect cinit,class=CODE,delta=2
	global __pbssBANK0
	movlw	low(__pbssBANK0)
	movwf	fsr0l
	movlw	high(__pbssBANK0)
	movwf	fsr0h
	movlw	01Fh
	fcall	clear_ram
psect cinit,class=CODE,delta=2
global end_of_initialization

;End of C runtime variable initialization code

end_of_initialization:
movlb 0
ljmp _main	;jump to C main() function
psect	cstackCOMMON,class=COMMON,space=1
global __pcstackCOMMON
__pcstackCOMMON:
	global	?_LED_RGB
?_LED_RGB:	; 0 bytes @ 0x0
	global	?_main
?_main:	; 0 bytes @ 0x0
	global	?_isr
?_isr:	; 0 bytes @ 0x0
	global	??_isr
??_isr:	; 0 bytes @ 0x0
	ds	1
	global	isr@tFS_IN
isr@tFS_IN:	; 1 bytes @ 0x1
	ds	1
	global	??_LED_RGB
??_LED_RGB:	; 0 bytes @ 0x2
	ds	2
	global	??_main
??_main:	; 0 bytes @ 0x4
	ds	1
psect	cstackBANK0,class=BANK0,space=1
global __pcstackBANK0
__pcstackBANK0:
	global	LED_RGB@tLED
LED_RGB@tLED:	; 1 bytes @ 0x0
	ds	1
	global	LED_RGB@iVal
LED_RGB@iVal:	; 1 bytes @ 0x1
	ds	1
	global	LED_RGB@tPort
LED_RGB@tPort:	; 1 bytes @ 0x2
	ds	1
	global	main@i
main@i:	; 4 bytes @ 0x3
	ds	4
	global	main@j
main@j:	; 4 bytes @ 0x7
	ds	4
	global	main@tCnt
main@tCnt:	; 1 bytes @ 0xB
	ds	1
;;Data sizes: Strings 0, constant 0, data 0, bss 37, persistent 2 stack 0
;;Auto spaces:   Size  Autos    Used
;; COMMON          14      5      11
;; BANK0           80     12      45
;; BANK1           32      0       0

;;
;; Pointer list with targets:



;;
;; Critical Paths under _main in COMMON
;;
;;   _main->_LED_RGB
;;
;; Critical Paths under _isr in COMMON
;;
;;   None.
;;
;; Critical Paths under _main in BANK0
;;
;;   _main->_LED_RGB
;;
;; Critical Paths under _isr in BANK0
;;
;;   None.
;;
;; Critical Paths under _main in BANK1
;;
;;   None.
;;
;; Critical Paths under _isr in BANK1
;;
;;   None.

;;
;;Main: autosize = 0, tempsize = 1, incstack = 0, save=0
;;

;;
;;Call Graph Tables:
;;
;; ---------------------------------------------------------------------------------
;; (Depth) Function   	        Calls       Base Space   Used Autos Params    Refs
;; ---------------------------------------------------------------------------------
;; (0) _main                                                10    10      0     111
;;                                              4 COMMON     1     1      0
;;                                              3 BANK0      9     9      0
;;                            _LED_RGB
;; ---------------------------------------------------------------------------------
;; (1) _LED_RGB                                              5     5      0     102
;;                                              2 COMMON     2     2      0
;;                                              0 BANK0      3     3      0
;; ---------------------------------------------------------------------------------
;; Estimated maximum stack depth 1
;; ---------------------------------------------------------------------------------
;; (Depth) Function   	        Calls       Base Space   Used Autos Params    Refs
;; ---------------------------------------------------------------------------------
;; (2) _isr                                                  2     2      0      67
;;                                              0 COMMON     2     2      0
;; ---------------------------------------------------------------------------------
;; Estimated maximum stack depth 2
;; ---------------------------------------------------------------------------------

;; Call Graph Graphs:

;; _main (ROOT)
;;   _LED_RGB
;;
;; _isr (ROOT)
;;

;; Address spaces:

;;Name               Size   Autos  Total    Cost      Usage
;;BIGRAM              70      0       0       0        0.0%
;;EEDATA             100      0       0       0        0.0%
;;NULL                 0      0       0       0        0.0%
;;CODE                 0      0       0       0        0.0%
;;BITCOMMON            E      0       0       1        0.0%
;;BITSFR0              0      0       0       1        0.0%
;;SFR0                 0      0       0       1        0.0%
;;COMMON               E      5       B       2       78.6%
;;BITSFR1              0      0       0       2        0.0%
;;SFR1                 0      0       0       2        0.0%
;;BITSFR2              0      0       0       3        0.0%
;;SFR2                 0      0       0       3        0.0%
;;STACK                0      0       1       3        0.0%
;;BITSFR3              0      0       0       4        0.0%
;;SFR3                 0      0       0       4        0.0%
;;ABS                  0      0      38       4        0.0%
;;BITBANK0            50      0       0       5        0.0%
;;BITSFR4              0      0       0       5        0.0%
;;SFR4                 0      0       0       5        0.0%
;;BANK0               50      C      2D       6       56.3%
;;BITSFR5              0      0       0       6        0.0%
;;SFR5                 0      0       0       6        0.0%
;;BITBANK1            20      0       0       7        0.0%
;;BITSFR6              0      0       0       7        0.0%
;;SFR6                 0      0       0       7        0.0%
;;BANK1               20      0       0       8        0.0%
;;BITSFR7              0      0       0       8        0.0%
;;SFR7                 0      0       0       8        0.0%
;;BITSFR8              0      0       0       9        0.0%
;;SFR8                 0      0       0       9        0.0%
;;DATA                 0      0      39       9        0.0%
;;BITSFR9              0      0       0      10        0.0%
;;SFR9                 0      0       0      10        0.0%
;;BITSFR10             0      0       0      11        0.0%
;;SFR10                0      0       0      11        0.0%
;;BITSFR11             0      0       0      12        0.0%
;;SFR11                0      0       0      12        0.0%
;;BITSFR12             0      0       0      13        0.0%
;;SFR12                0      0       0      13        0.0%
;;BITSFR13             0      0       0      14        0.0%
;;SFR13                0      0       0      14        0.0%
;;BITSFR14             0      0       0      15        0.0%
;;SFR14                0      0       0      15        0.0%
;;BITSFR15             0      0       0      16        0.0%
;;SFR15                0      0       0      16        0.0%
;;BITSFR16             0      0       0      17        0.0%
;;SFR16                0      0       0      17        0.0%
;;BITSFR17             0      0       0      18        0.0%
;;SFR17                0      0       0      18        0.0%
;;BITSFR18             0      0       0      19        0.0%
;;SFR18                0      0       0      19        0.0%
;;BITSFR19             0      0       0      20        0.0%
;;SFR19                0      0       0      20        0.0%
;;BITSFR20             0      0       0      21        0.0%
;;SFR20                0      0       0      21        0.0%
;;BITSFR21             0      0       0      22        0.0%
;;SFR21                0      0       0      22        0.0%
;;BITSFR22             0      0       0      23        0.0%
;;SFR22                0      0       0      23        0.0%
;;BITSFR23             0      0       0      24        0.0%
;;SFR23                0      0       0      24        0.0%
;;BITSFR24             0      0       0      25        0.0%
;;SFR24                0      0       0      25        0.0%
;;BITSFR25             0      0       0      26        0.0%
;;SFR25                0      0       0      26        0.0%
;;BITSFR26             0      0       0      27        0.0%
;;SFR26                0      0       0      27        0.0%
;;BITSFR27             0      0       0      28        0.0%
;;SFR27                0      0       0      28        0.0%
;;BITSFR28             0      0       0      29        0.0%
;;SFR28                0      0       0      29        0.0%
;;BITSFR29             0      0       0      30        0.0%
;;SFR29                0      0       0      30        0.0%
;;BITSFR30             0      0       0      31        0.0%
;;SFR30                0      0       0      31        0.0%
;;BITSFR31             0      0       0      32        0.0%
;;SFR31                0      0       0      32        0.0%

	global	_main
psect	maintext,global,class=CODE,delta=2
global __pmaintext
__pmaintext:

;; *************** function _main *****************
;; Defined at:
;;		line 84 in file "C:\myData\ZuVo500\Firmware\src\zuvo300_main.c"
;; Parameters:    Size  Location     Type
;;		None
;; Auto vars:     Size  Location     Type
;;  j               4    7[BANK0 ] unsigned long 
;;  i               4    3[BANK0 ] unsigned long 
;;  tCnt            1   11[BANK0 ] unsigned char 
;; Return value:  Size  Location     Type
;;		None               void
;; Registers used:
;;		wreg, status,2, status,0, pclath, cstack
;; Tracked objects:
;;		On entry : 17F/0
;;		On exit  : 0/0
;;		Unchanged: 0/0
;; Data sizes:     COMMON   BANK0   BANK1
;;      Params:         0       0       0
;;      Locals:         0       9       0
;;      Temps:          1       0       0
;;      Totals:         1       9       0
;;Total ram usage:       10 bytes
;; Hardware stack levels required when called:    2
;; This function calls:
;;		_LED_RGB
;; This function is called by:
;;		Startup code after reset
;; This function uses a non-reentrant model
;;
psect	maintext
	file	"C:\myData\ZuVo500\Firmware\src\zuvo300_main.c"
	line	84
	global	__size_of_main
	__size_of_main	equ	__end_of_main-_main
	
_main:	
	opt	stack 16
; Regs used in _main: [wreg+status,2+status,0+pclath+cstack]
	line	88
	
l3224:	
;zuvo300_main.c: 87: static unsigned char tLEDCnt=0;
;zuvo300_main.c: 88: unsigned long i=0,j=0;
	movlw	0
	movlb 0	; select bank0
	movwf	(main@i+3)
	movlw	0
	movwf	(main@i+2)
	movlw	0
	movwf	(main@i+1)
	movlw	0
	movwf	(main@i)

	movlw	0
	movwf	(main@j+3)
	movlw	0
	movwf	(main@j+2)
	movlw	0
	movwf	(main@j+1)
	movlw	0
	movwf	(main@j)

	line	89
	
l3226:	
;zuvo300_main.c: 89: unsigned char tCnt=0;
	clrc
	movlw	0
	btfsc	status,0
	movlw	1
	movwf	(main@tCnt)
	line	94
	
l3228:	
;zuvo300_main.c: 94: OSCCON = 0x72;
	movlw	(072h)
	movlb 1	; select bank1
	movwf	(153)^080h	;volatile
	line	96
	
l3230:	
;zuvo300_main.c: 96: TRISA = ~0x10;
	movlw	(0EFh)
	movwf	(140)^080h	;volatile
	line	97
	
l3232:	
;zuvo300_main.c: 97: TRISC = ~0xff;
	clrf	(142)^080h	;volatile
	line	98
	
l3234:	
;zuvo300_main.c: 98: OPTION_REG = 0x82;
	movlw	(082h)
	movwf	(149)^080h	;volatile
	line	101
	
l3236:	
;zuvo300_main.c: 101: WPUA = 0x24;
	movlw	(024h)
	movlb 4	; select bank4
	movwf	(524)^0200h	;volatile
	line	106
	
l3238:	
;zuvo300_main.c: 106: T0IE = 1;
	bsf	(93/8),(93)&7
	line	107
	
l3240:	
;zuvo300_main.c: 107: INTEDG = 1;
	movlb 1	; select bank1
	bsf	(1198/8)^080h,(1198)&7
	line	108
	
l3242:	
;zuvo300_main.c: 108: INTE = 1;
	bsf	(92/8),(92)&7
	line	109
	
l3244:	
;zuvo300_main.c: 109: GIE = 1;
	bsf	(95/8),(95)&7
	line	113
	
l3246:	
;zuvo300_main.c: 113: mLED_Color = 0x07;
	movlw	(07h)
	movwf	(??_main+0)+0
	movf	(??_main+0)+0,w
	movlb 0	; select bank0
	movwf	(_mLED_Color)
	line	114
	
l3248:	
;zuvo300_main.c: 114: mUVL = 0x01;
	clrf	(_mUVL)
	bsf	status,0
	rlf	(_mUVL),f
	line	115
	
l3250:	
;zuvo300_main.c: 115: mBuzzer = 0x00;
	clrc
	movlw	0
	btfsc	status,0
	movlw	1
	movwf	(_mBuzzer)
	line	120
;zuvo300_main.c: 120: while (1)
	
l1203:	
	line	123
	
l3252:	
# 123 "C:\myData\ZuVo500\Firmware\src\zuvo300_main.c"
clrwdt ;#
psect	maintext
	line	125
	
l3254:	
;zuvo300_main.c: 125: LED_RGB(0x00);
	movlw	(0)
	fcall	_LED_RGB
	line	126
	
l3256:	
;zuvo300_main.c: 126: LED_RGB(0x07);
	movlw	(07h)
	fcall	_LED_RGB
	line	128
	
l3258:	
;zuvo300_main.c: 128: if (mTimer_Flag0)
	movlb 0	; select bank0
	movf	(_mTimer_Flag0),w
	skipz
	goto	u3220
	goto	l1204
u3220:
	line	131
	
l3260:	
;zuvo300_main.c: 129: {
;zuvo300_main.c: 131: mTimer_Flag0 = 0;
	clrc
	movlw	0
	btfsc	status,0
	movlw	1
	movwf	(_mTimer_Flag0)
	line	132
;zuvo300_main.c: 132: mUVL ^=0x01;
	movlw	(01h)
	movwf	(??_main+0)+0
	movf	(??_main+0)+0,w
	xorwf	(_mUVL),f
	line	133
;zuvo300_main.c: 133: mLED_Color ^= 0x07;
	movlw	(07h)
	movwf	(??_main+0)+0
	movf	(??_main+0)+0,w
	xorwf	(_mLED_Color),f
	line	134
;zuvo300_main.c: 134: mBuzzer ^= 0x01;
	movlw	(01h)
	movwf	(??_main+0)+0
	movf	(??_main+0)+0,w
	xorwf	(_mBuzzer),f
	line	155
	
l1204:	
	line	156
	
l1205:	
	line	120
	goto	l1203
	
l1206:	
	line	158
	
l1207:	
	global	start
	ljmp	start
	opt stack 0
GLOBAL	__end_of_main
	__end_of_main:
;; =============== function _main ends ============

	signat	_main,88
	global	_LED_RGB
psect	text182,local,class=CODE,delta=2
global __ptext182
__ptext182:

;; *************** function _LED_RGB *****************
;; Defined at:
;;		line 68 in file "C:\myData\ZuVo500\Firmware\src\zuvo300_main.c"
;; Parameters:    Size  Location     Type
;;  iVal            1    wreg     unsigned char 
;; Auto vars:     Size  Location     Type
;;  iVal            1    1[BANK0 ] unsigned char 
;;  tPort           1    2[BANK0 ] unsigned char 
;;  tLED            1    0[BANK0 ] unsigned char 
;; Return value:  Size  Location     Type
;;		None               void
;; Registers used:
;;		wreg, status,2, status,0
;; Tracked objects:
;;		On entry : 0/0
;;		On exit  : 0/0
;;		Unchanged: 0/0
;; Data sizes:     COMMON   BANK0   BANK1
;;      Params:         0       0       0
;;      Locals:         0       3       0
;;      Temps:          2       0       0
;;      Totals:         2       3       0
;;Total ram usage:        5 bytes
;; Hardware stack levels used:    1
;; Hardware stack levels required when called:    1
;; This function calls:
;;		Nothing
;; This function is called by:
;;		_main
;; This function uses a non-reentrant model
;;
psect	text182
	file	"C:\myData\ZuVo500\Firmware\src\zuvo300_main.c"
	line	68
	global	__size_of_LED_RGB
	__size_of_LED_RGB	equ	__end_of_LED_RGB-_LED_RGB
	
_LED_RGB:	
	opt	stack 15
; Regs used in _LED_RGB: [wreg+status,2+status,0]
;LED_RGB@iVal stored from wreg
	line	70
	movlb 0	; select bank0
	movwf	(LED_RGB@iVal)
	
l3216:	
;zuvo300_main.c: 70: unsigned char tLED=0;
	clrc
	movlw	0
	btfsc	status,0
	movlw	1
	movwf	(LED_RGB@tLED)
	line	77
	
l3218:	
;zuvo300_main.c: 71: unsigned char tPort;
;zuvo300_main.c: 77: tPort = LATC & (~(0x07<<2));
	movlb 2	; select bank2
	movf	(270)^0100h,w
	andlw	0E3h
	movwf	(??_LED_RGB+0)+0
	movf	(??_LED_RGB+0)+0,w
	movlb 0	; select bank0
	movwf	(LED_RGB@tPort)
	line	78
	
l3220:	
;zuvo300_main.c: 78: tPort = tPort | ((~iVal & 0x07)<<2);
	comf	(LED_RGB@iVal),w
	andlw	07h
	movwf	(??_LED_RGB+0)+0
	movlw	(02h)-1
u3215:
	lslf	(??_LED_RGB+0)+0,f
	addlw	-1
	skipz
	goto	u3215
	lslf	(??_LED_RGB+0)+0,w
	iorwf	(LED_RGB@tPort),w
	movwf	(??_LED_RGB+1)+0
	movf	(??_LED_RGB+1)+0,w
	movwf	(LED_RGB@tPort)
	line	79
	
l3222:	
;zuvo300_main.c: 79: LATC = tPort;
	movf	(LED_RGB@tPort),w
	movlb 2	; select bank2
	movwf	(270)^0100h	;volatile
	line	81
	
l1198:	
	return
	opt stack 0
GLOBAL	__end_of_LED_RGB
	__end_of_LED_RGB:
;; =============== function _LED_RGB ends ============

	signat	_LED_RGB,4216
	global	_isr
psect	intentry,class=CODE,delta=2
global __pintentry
__pintentry:

;; *************** function _isr *****************
;; Defined at:
;;		line 162 in file "C:\myData\ZuVo500\Firmware\src\zuvo300_main.c"
;; Parameters:    Size  Location     Type
;;		None
;; Auto vars:     Size  Location     Type
;;  tFS_IN          1    1[COMMON] unsigned char 
;; Return value:  Size  Location     Type
;;		None               void
;; Registers used:
;;		wreg, status,2, status,0
;; Tracked objects:
;;		On entry : 0/0
;;		On exit  : 0/0
;;		Unchanged: 0/0
;; Data sizes:     COMMON   BANK0   BANK1
;;      Params:         0       0       0
;;      Locals:         1       0       0
;;      Temps:          1       0       0
;;      Totals:         2       0       0
;;Total ram usage:        2 bytes
;; Hardware stack levels used:    1
;; This function calls:
;;		Nothing
;; This function is called by:
;;		Interrupt level 1
;; This function uses a non-reentrant model
;;
psect	intentry
	file	"C:\myData\ZuVo500\Firmware\src\zuvo300_main.c"
	line	162
	global	__size_of_isr
	__size_of_isr	equ	__end_of_isr-_isr
	
_isr:	
	opt	stack 14
; Regs used in _isr: [wreg+status,2+status,0]
psect	intentry
	pagesel	$
	line	167
	
i1l2764:	
;zuvo300_main.c: 163: static unsigned char tCntA = 0;
;zuvo300_main.c: 164: static unsigned char pFS_IN= 0;
;zuvo300_main.c: 165: unsigned char tFS_IN;
;zuvo300_main.c: 167: if(T0IF) {
	btfss	(90/8),(90)&7
	goto	u235_21
	goto	u235_20
u235_21:
	goto	i1l1214
u235_20:
	line	168
	
i1l2766:	
;zuvo300_main.c: 168: T0IF = 0;
	bcf	(90/8),(90)&7
	line	170
	
i1l2768:	
;zuvo300_main.c: 170: tFS_IN = (PORTA & (1<<2));
	movlb 0	; select bank0
	movf	(12),w
	andlw	04h
	movwf	(??_isr+0)+0
	movf	(??_isr+0)+0,w
	movwf	(isr@tFS_IN)
	line	171
	
i1l2770:	
;zuvo300_main.c: 171: if ((pFS_IN != tFS_IN) && (tFS_IN !=0))
	movf	(isr@pFS_IN),w
	xorwf	(isr@tFS_IN),w
	skipnz
	goto	u236_21
	goto	u236_20
u236_21:
	goto	i1l1215
u236_20:
	
i1l2772:	
	movf	(isr@tFS_IN),w
	skipz
	goto	u237_20
	goto	i1l1215
u237_20:
	line	172
	
i1l2774:	
;zuvo300_main.c: 172: mFS_Cnt ++;
	movlw	(01h)
	movwf	(??_isr+0)+0
	movf	(??_isr+0)+0,w
	addwf	(_mFS_Cnt),f
	
i1l1215:	
	line	173
	
i1l2776:	
;zuvo300_main.c: 173: pFS_IN = tFS_IN;
	movf	(isr@tFS_IN),w
	movwf	(??_isr+0)+0
	movf	(??_isr+0)+0,w
	movwf	(isr@pFS_IN)
	line	175
	
i1l2778:	
;zuvo300_main.c: 175: tCntA++;
	movlw	(01h)
	movwf	(??_isr+0)+0
	movf	(??_isr+0)+0,w
	addwf	(isr@tCntA),f
	line	176
;zuvo300_main.c: 176: if (tCntA>=10)
	movlw	(0Ah)
	subwf	(isr@tCntA),w
	skipc
	goto	u238_21
	goto	u238_20
u238_21:
	goto	i1l1216
u238_20:
	line	178
	
i1l2780:	
;zuvo300_main.c: 177: {
;zuvo300_main.c: 178: tCntA = 0;
	clrc
	movlw	0
	btfsc	status,0
	movlw	1
	movwf	(isr@tCntA)
	line	179
	
i1l2782:	
;zuvo300_main.c: 179: mTimer_Flag0 = 1;
	clrf	(_mTimer_Flag0)
	bsf	status,0
	rlf	(_mTimer_Flag0),f
	line	180
	
i1l1216:	
	line	181
	
i1l1214:	
	line	183
	
i1l1217:	
	retfie
	opt stack 0
GLOBAL	__end_of_isr
	__end_of_isr:
;; =============== function _isr ends ============

	signat	_isr,88
psect	intentry
	global	btemp
	btemp set 07Eh

	DABS	1,126,2	;btemp
	global	wtemp0
	wtemp0 set btemp
	end
