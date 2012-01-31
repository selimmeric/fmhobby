
;CodeVisionAVR C Compiler V2.03.4 Standard
;(C) Copyright 1998-2008 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com

;Chip type              : ATmega8
;Program type           : Application
;Clock frequency        : 1.000000 MHz
;Memory model           : Small
;Optimize for           : Size
;(s)printf features     : int, width
;(s)scanf features      : int, width
;External RAM size      : 0
;Data Stack size        : 256 byte(s)
;Heap size              : 0 byte(s)
;Promote char to int    : Yes
;char is unsigned       : Yes
;global const stored in FLASH  : No
;8 bit enums            : Yes
;Enhanced core instructions    : On
;Smart register allocation : On
;Automatic register allocation : On

	#pragma AVRPART ADMIN PART_NAME ATmega8
	#pragma AVRPART MEMORY PROG_FLASH 8192
	#pragma AVRPART MEMORY EEPROM 512
	#pragma AVRPART MEMORY INT_SRAM SIZE 1024
	#pragma AVRPART MEMORY INT_SRAM START_ADDR 0x60

	.LISTMAC
	.EQU UDRE=0x5
	.EQU RXC=0x7
	.EQU USR=0xB
	.EQU UDR=0xC
	.EQU SPSR=0xE
	.EQU SPDR=0xF
	.EQU EERE=0x0
	.EQU EEWE=0x1
	.EQU EEMWE=0x2
	.EQU EECR=0x1C
	.EQU EEDR=0x1D
	.EQU EEARL=0x1E
	.EQU EEARH=0x1F
	.EQU WDTCR=0x21
	.EQU MCUCR=0x35
	.EQU GICR=0x3B
	.EQU SPL=0x3D
	.EQU SPH=0x3E
	.EQU SREG=0x3F

	.DEF R0X0=R0
	.DEF R0X1=R1
	.DEF R0X2=R2
	.DEF R0X3=R3
	.DEF R0X4=R4
	.DEF R0X5=R5
	.DEF R0X6=R6
	.DEF R0X7=R7
	.DEF R0X8=R8
	.DEF R0X9=R9
	.DEF R0XA=R10
	.DEF R0XB=R11
	.DEF R0XC=R12
	.DEF R0XD=R13
	.DEF R0XE=R14
	.DEF R0XF=R15
	.DEF R0X10=R16
	.DEF R0X11=R17
	.DEF R0X12=R18
	.DEF R0X13=R19
	.DEF R0X14=R20
	.DEF R0X15=R21
	.DEF R0X16=R22
	.DEF R0X17=R23
	.DEF R0X18=R24
	.DEF R0X19=R25
	.DEF R0X1A=R26
	.DEF R0X1B=R27
	.DEF R0X1C=R28
	.DEF R0X1D=R29
	.DEF R0X1E=R30
	.DEF R0X1F=R31

	.MACRO __CPD1N
	CPI  R30,LOW(@0)
	LDI  R26,HIGH(@0)
	CPC  R31,R26
	LDI  R26,BYTE3(@0)
	CPC  R22,R26
	LDI  R26,BYTE4(@0)
	CPC  R23,R26
	.ENDM

	.MACRO __CPD2N
	CPI  R26,LOW(@0)
	LDI  R30,HIGH(@0)
	CPC  R27,R30
	LDI  R30,BYTE3(@0)
	CPC  R24,R30
	LDI  R30,BYTE4(@0)
	CPC  R25,R30
	.ENDM

	.MACRO __CPWRR
	CP   R@0,R@2
	CPC  R@1,R@3
	.ENDM

	.MACRO __CPWRN
	CPI  R@0,LOW(@2)
	LDI  R30,HIGH(@2)
	CPC  R@1,R30
	.ENDM

	.MACRO __ADDB1MN
	SUBI R30,LOW(-@0-(@1))
	.ENDM

	.MACRO __ADDB2MN
	SUBI R26,LOW(-@0-(@1))
	.ENDM

	.MACRO __ADDW1MN
	SUBI R30,LOW(-@0-(@1))
	SBCI R31,HIGH(-@0-(@1))
	.ENDM

	.MACRO __ADDW2MN
	SUBI R26,LOW(-@0-(@1))
	SBCI R27,HIGH(-@0-(@1))
	.ENDM

	.MACRO __ADDW1FN
	SUBI R30,LOW(-2*@0-(@1))
	SBCI R31,HIGH(-2*@0-(@1))
	.ENDM

	.MACRO __ADDD1FN
	SUBI R30,LOW(-2*@0-(@1))
	SBCI R31,HIGH(-2*@0-(@1))
	SBCI R22,BYTE3(-2*@0-(@1))
	.ENDM

	.MACRO __ADDD1N
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	SBCI R22,BYTE3(-@0)
	SBCI R23,BYTE4(-@0)
	.ENDM

	.MACRO __ADDD2N
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	SBCI R24,BYTE3(-@0)
	SBCI R25,BYTE4(-@0)
	.ENDM

	.MACRO __SUBD1N
	SUBI R30,LOW(@0)
	SBCI R31,HIGH(@0)
	SBCI R22,BYTE3(@0)
	SBCI R23,BYTE4(@0)
	.ENDM

	.MACRO __SUBD2N
	SUBI R26,LOW(@0)
	SBCI R27,HIGH(@0)
	SBCI R24,BYTE3(@0)
	SBCI R25,BYTE4(@0)
	.ENDM

	.MACRO __ANDBMNN
	LDS  R30,@0+@1
	ANDI R30,LOW(@2)
	STS  @0+@1,R30
	.ENDM

	.MACRO __ANDWMNN
	LDS  R30,@0+@1
	ANDI R30,LOW(@2)
	STS  @0+@1,R30
	LDS  R30,@0+@1+1
	ANDI R30,HIGH(@2)
	STS  @0+@1+1,R30
	.ENDM

	.MACRO __ANDD1N
	ANDI R30,LOW(@0)
	ANDI R31,HIGH(@0)
	ANDI R22,BYTE3(@0)
	ANDI R23,BYTE4(@0)
	.ENDM

	.MACRO __ANDD2N
	ANDI R26,LOW(@0)
	ANDI R27,HIGH(@0)
	ANDI R24,BYTE3(@0)
	ANDI R25,BYTE4(@0)
	.ENDM

	.MACRO __ORBMNN
	LDS  R30,@0+@1
	ORI  R30,LOW(@2)
	STS  @0+@1,R30
	.ENDM

	.MACRO __ORWMNN
	LDS  R30,@0+@1
	ORI  R30,LOW(@2)
	STS  @0+@1,R30
	LDS  R30,@0+@1+1
	ORI  R30,HIGH(@2)
	STS  @0+@1+1,R30
	.ENDM

	.MACRO __ORD1N
	ORI  R30,LOW(@0)
	ORI  R31,HIGH(@0)
	ORI  R22,BYTE3(@0)
	ORI  R23,BYTE4(@0)
	.ENDM

	.MACRO __ORD2N
	ORI  R26,LOW(@0)
	ORI  R27,HIGH(@0)
	ORI  R24,BYTE3(@0)
	ORI  R25,BYTE4(@0)
	.ENDM

	.MACRO __DELAY_USB
	LDI  R24,LOW(@0)
__DELAY_USB_LOOP:
	DEC  R24
	BRNE __DELAY_USB_LOOP
	.ENDM

	.MACRO __DELAY_USW
	LDI  R24,LOW(@0)
	LDI  R25,HIGH(@0)
__DELAY_USW_LOOP:
	SBIW R24,1
	BRNE __DELAY_USW_LOOP
	.ENDM

	.MACRO __CLRD1S
	LDI  R30,0
	STD  Y+@0,R30
	STD  Y+@0+1,R30
	STD  Y+@0+2,R30
	STD  Y+@0+3,R30
	.ENDM

	.MACRO __GETD1S
	LDD  R30,Y+@0
	LDD  R31,Y+@0+1
	LDD  R22,Y+@0+2
	LDD  R23,Y+@0+3
	.ENDM

	.MACRO __PUTD1S
	STD  Y+@0,R30
	STD  Y+@0+1,R31
	STD  Y+@0+2,R22
	STD  Y+@0+3,R23
	.ENDM

	.MACRO __PUTD2S
	STD  Y+@0,R26
	STD  Y+@0+1,R27
	STD  Y+@0+2,R24
	STD  Y+@0+3,R25
	.ENDM

	.MACRO __POINTB1MN
	LDI  R30,LOW(@0+@1)
	.ENDM

	.MACRO __POINTW1MN
	LDI  R30,LOW(@0+@1)
	LDI  R31,HIGH(@0+@1)
	.ENDM

	.MACRO __POINTD1M
	LDI  R30,LOW(@0)
	LDI  R31,HIGH(@0)
	LDI  R22,BYTE3(@0)
	LDI  R23,BYTE4(@0)
	.ENDM

	.MACRO __POINTW1FN
	LDI  R30,LOW(2*@0+@1)
	LDI  R31,HIGH(2*@0+@1)
	.ENDM

	.MACRO __POINTD1FN
	LDI  R30,LOW(2*@0+@1)
	LDI  R31,HIGH(2*@0+@1)
	LDI  R22,BYTE3(2*@0+@1)
	LDI  R23,BYTE4(2*@0+@1)
	.ENDM

	.MACRO __POINTB2MN
	LDI  R26,LOW(@0+@1)
	.ENDM

	.MACRO __POINTW2MN
	LDI  R26,LOW(@0+@1)
	LDI  R27,HIGH(@0+@1)
	.ENDM

	.MACRO __POINTBRM
	LDI  R@0,LOW(@1)
	.ENDM

	.MACRO __POINTWRM
	LDI  R@0,LOW(@2)
	LDI  R@1,HIGH(@2)
	.ENDM

	.MACRO __POINTBRMN
	LDI  R@0,LOW(@1+@2)
	.ENDM

	.MACRO __POINTWRMN
	LDI  R@0,LOW(@2+@3)
	LDI  R@1,HIGH(@2+@3)
	.ENDM

	.MACRO __POINTWRFN
	LDI  R@0,LOW(@2*2+@3)
	LDI  R@1,HIGH(@2*2+@3)
	.ENDM

	.MACRO __GETD1N
	LDI  R30,LOW(@0)
	LDI  R31,HIGH(@0)
	LDI  R22,BYTE3(@0)
	LDI  R23,BYTE4(@0)
	.ENDM

	.MACRO __GETD2N
	LDI  R26,LOW(@0)
	LDI  R27,HIGH(@0)
	LDI  R24,BYTE3(@0)
	LDI  R25,BYTE4(@0)
	.ENDM

	.MACRO __GETD2S
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	LDD  R24,Y+@0+2
	LDD  R25,Y+@0+3
	.ENDM

	.MACRO __GETB1MN
	LDS  R30,@0+@1
	.ENDM

	.MACRO __GETB1HMN
	LDS  R31,@0+@1
	.ENDM

	.MACRO __GETW1MN
	LDS  R30,@0+@1
	LDS  R31,@0+@1+1
	.ENDM

	.MACRO __GETD1MN
	LDS  R30,@0+@1
	LDS  R31,@0+@1+1
	LDS  R22,@0+@1+2
	LDS  R23,@0+@1+3
	.ENDM

	.MACRO __GETBRMN
	LDS  R@0,@1+@2
	.ENDM

	.MACRO __GETWRMN
	LDS  R@0,@2+@3
	LDS  R@1,@2+@3+1
	.ENDM

	.MACRO __GETWRZ
	LDD  R@0,Z+@2
	LDD  R@1,Z+@2+1
	.ENDM

	.MACRO __GETD2Z
	LDD  R26,Z+@0
	LDD  R27,Z+@0+1
	LDD  R24,Z+@0+2
	LDD  R25,Z+@0+3
	.ENDM

	.MACRO __GETB2MN
	LDS  R26,@0+@1
	.ENDM

	.MACRO __GETW2MN
	LDS  R26,@0+@1
	LDS  R27,@0+@1+1
	.ENDM

	.MACRO __GETD2MN
	LDS  R26,@0+@1
	LDS  R27,@0+@1+1
	LDS  R24,@0+@1+2
	LDS  R25,@0+@1+3
	.ENDM

	.MACRO __PUTB1MN
	STS  @0+@1,R30
	.ENDM

	.MACRO __PUTW1MN
	STS  @0+@1,R30
	STS  @0+@1+1,R31
	.ENDM

	.MACRO __PUTD1MN
	STS  @0+@1,R30
	STS  @0+@1+1,R31
	STS  @0+@1+2,R22
	STS  @0+@1+3,R23
	.ENDM

	.MACRO __PUTB1EN
	LDI  R26,LOW(@0+@1)
	LDI  R27,HIGH(@0+@1)
	RCALL __EEPROMWRB
	.ENDM

	.MACRO __PUTW1EN
	LDI  R26,LOW(@0+@1)
	LDI  R27,HIGH(@0+@1)
	RCALL __EEPROMWRW
	.ENDM

	.MACRO __PUTD1EN
	LDI  R26,LOW(@0+@1)
	LDI  R27,HIGH(@0+@1)
	RCALL __EEPROMWRD
	.ENDM

	.MACRO __PUTBR0MN
	STS  @0+@1,R0
	.ENDM

	.MACRO __PUTDZ2
	STD  Z+@0,R26
	STD  Z+@0+1,R27
	STD  Z+@0+2,R24
	STD  Z+@0+3,R25
	.ENDM

	.MACRO __PUTBMRN
	STS  @0+@1,R@2
	.ENDM

	.MACRO __PUTWMRN
	STS  @0+@1,R@2
	STS  @0+@1+1,R@3
	.ENDM

	.MACRO __PUTBZR
	STD  Z+@1,R@0
	.ENDM

	.MACRO __PUTWZR
	STD  Z+@2,R@0
	STD  Z+@2+1,R@1
	.ENDM

	.MACRO __GETW1R
	MOV  R30,R@0
	MOV  R31,R@1
	.ENDM

	.MACRO __GETW2R
	MOV  R26,R@0
	MOV  R27,R@1
	.ENDM

	.MACRO __GETWRN
	LDI  R@0,LOW(@2)
	LDI  R@1,HIGH(@2)
	.ENDM

	.MACRO __PUTW1R
	MOV  R@0,R30
	MOV  R@1,R31
	.ENDM

	.MACRO __PUTW2R
	MOV  R@0,R26
	MOV  R@1,R27
	.ENDM

	.MACRO __ADDWRN
	SUBI R@0,LOW(-@2)
	SBCI R@1,HIGH(-@2)
	.ENDM

	.MACRO __ADDWRR
	ADD  R@0,R@2
	ADC  R@1,R@3
	.ENDM

	.MACRO __SUBWRN
	SUBI R@0,LOW(@2)
	SBCI R@1,HIGH(@2)
	.ENDM

	.MACRO __SUBWRR
	SUB  R@0,R@2
	SBC  R@1,R@3
	.ENDM

	.MACRO __ANDWRN
	ANDI R@0,LOW(@2)
	ANDI R@1,HIGH(@2)
	.ENDM

	.MACRO __ANDWRR
	AND  R@0,R@2
	AND  R@1,R@3
	.ENDM

	.MACRO __ORWRN
	ORI  R@0,LOW(@2)
	ORI  R@1,HIGH(@2)
	.ENDM

	.MACRO __ORWRR
	OR   R@0,R@2
	OR   R@1,R@3
	.ENDM

	.MACRO __EORWRR
	EOR  R@0,R@2
	EOR  R@1,R@3
	.ENDM

	.MACRO __GETWRS
	LDD  R@0,Y+@2
	LDD  R@1,Y+@2+1
	.ENDM

	.MACRO __PUTWSR
	STD  Y+@2,R@0
	STD  Y+@2+1,R@1
	.ENDM

	.MACRO __MOVEWRR
	MOV  R@0,R@2
	MOV  R@1,R@3
	.ENDM

	.MACRO __INWR
	IN   R@0,@2
	IN   R@1,@2+1
	.ENDM

	.MACRO __OUTWR
	OUT  @2+1,R@1
	OUT  @2,R@0
	.ENDM

	.MACRO __CALL1MN
	LDS  R30,@0+@1
	LDS  R31,@0+@1+1
	ICALL
	.ENDM

	.MACRO __CALL1FN
	LDI  R30,LOW(2*@0+@1)
	LDI  R31,HIGH(2*@0+@1)
	RCALL __GETW1PF
	ICALL
	.ENDM

	.MACRO __CALL2EN
	LDI  R26,LOW(@0+@1)
	LDI  R27,HIGH(@0+@1)
	RCALL __EEPROMRDW
	ICALL
	.ENDM

	.MACRO __GETW1STACK
	IN   R26,SPL
	IN   R27,SPH
	ADIW R26,@0+1
	LD   R30,X+
	LD   R31,X
	.ENDM

	.MACRO __NBST
	BST  R@0,@1
	IN   R30,SREG
	LDI  R31,0x40
	EOR  R30,R31
	OUT  SREG,R30
	.ENDM


	.MACRO __PUTB1SN
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SN
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SN
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1SNS
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	ADIW R26,@1
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SNS
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	ADIW R26,@1
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SNS
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	ADIW R26,@1
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1PMN
	LDS  R26,@0
	LDS  R27,@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1PMN
	LDS  R26,@0
	LDS  R27,@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1PMN
	LDS  R26,@0
	LDS  R27,@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1PMNS
	LDS  R26,@0
	LDS  R27,@0+1
	ADIW R26,@1
	ST   X,R30
	.ENDM

	.MACRO __PUTW1PMNS
	LDS  R26,@0
	LDS  R27,@0+1
	ADIW R26,@1
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1PMNS
	LDS  R26,@0
	LDS  R27,@0+1
	ADIW R26,@1
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1RN
	MOVW R26,R@0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1RN
	MOVW R26,R@0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1RN
	MOVW R26,R@0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1RNS
	MOVW R26,R@0
	ADIW R26,@1
	ST   X,R30
	.ENDM

	.MACRO __PUTW1RNS
	MOVW R26,R@0
	ADIW R26,@1
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1RNS
	MOVW R26,R@0
	ADIW R26,@1
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1RON
	MOV  R26,R@0
	MOV  R27,R@1
	SUBI R26,LOW(-@2)
	SBCI R27,HIGH(-@2)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1RON
	MOV  R26,R@0
	MOV  R27,R@1
	SUBI R26,LOW(-@2)
	SBCI R27,HIGH(-@2)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1RON
	MOV  R26,R@0
	MOV  R27,R@1
	SUBI R26,LOW(-@2)
	SBCI R27,HIGH(-@2)
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1RONS
	MOV  R26,R@0
	MOV  R27,R@1
	ADIW R26,@2
	ST   X,R30
	.ENDM

	.MACRO __PUTW1RONS
	MOV  R26,R@0
	MOV  R27,R@1
	ADIW R26,@2
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1RONS
	MOV  R26,R@0
	MOV  R27,R@1
	ADIW R26,@2
	RCALL __PUTDP1
	.ENDM


	.MACRO __GETB1SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R30,Z
	.ENDM

	.MACRO __GETB1HSX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R31,Z
	.ENDM

	.MACRO __GETW1SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R0,Z+
	LD   R31,Z
	MOV  R30,R0
	.ENDM

	.MACRO __GETD1SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R0,Z+
	LD   R1,Z+
	LD   R22,Z+
	LD   R23,Z
	MOVW R30,R0
	.ENDM

	.MACRO __GETB2SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R26,X
	.ENDM

	.MACRO __GETW2SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R27,X
	MOV  R26,R0
	.ENDM

	.MACRO __GETD2SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R1,X+
	LD   R24,X+
	LD   R25,X
	MOVW R26,R0
	.ENDM

	.MACRO __GETBRSX
	MOVW R30,R28
	SUBI R30,LOW(-@1)
	SBCI R31,HIGH(-@1)
	LD   R@0,Z
	.ENDM

	.MACRO __GETWRSX
	MOVW R30,R28
	SUBI R30,LOW(-@2)
	SBCI R31,HIGH(-@2)
	LD   R@0,Z+
	LD   R@1,Z
	.ENDM

	.MACRO __LSLW8SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R31,Z
	CLR  R30
	.ENDM

	.MACRO __PUTB1SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	ST   X+,R30
	ST   X+,R31
	ST   X+,R22
	ST   X,R23
	.ENDM

	.MACRO __CLRW1SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	CLR  R0
	ST   Z+,R0
	ST   Z,R0
	.ENDM

	.MACRO __CLRD1SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	CLR  R0
	ST   Z+,R0
	ST   Z+,R0
	ST   Z+,R0
	ST   Z,R0
	.ENDM

	.MACRO __PUTB2SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	ST   Z,R26
	.ENDM

	.MACRO __PUTW2SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	ST   Z+,R26
	ST   Z,R27
	.ENDM

	.MACRO __PUTD2SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	ST   Z+,R26
	ST   Z+,R27
	ST   Z+,R24
	ST   Z,R25
	.ENDM

	.MACRO __PUTBSRX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	ST   Z,R@1
	.ENDM

	.MACRO __PUTWSRX
	MOVW R30,R28
	SUBI R30,LOW(-@2)
	SBCI R31,HIGH(-@2)
	ST   Z+,R@0
	ST   Z,R@1
	.ENDM

	.MACRO __PUTB1SNX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R27,X
	MOV  R26,R0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SNX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R27,X
	MOV  R26,R0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SNX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R27,X
	MOV  R26,R0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X+,R31
	ST   X+,R22
	ST   X,R23
	.ENDM

	.MACRO __MULBRR
	MULS R@0,R@1
	MOVW R30,R0
	.ENDM

	.MACRO __MULBRRU
	MUL  R@0,R@1
	MOVW R30,R0
	.ENDM

	.MACRO __MULBRR0
	MULS R@0,R@1
	.ENDM

	.MACRO __MULBRRU0
	MUL  R@0,R@1
	.ENDM

	.MACRO __MULBNWRU
	LDI  R26,@2
	MUL  R26,R@0
	MOVW R30,R0
	MUL  R26,R@1
	ADD  R31,R0
	.ENDM

;NAME DEFINITIONS FOR GLOBAL VARIABLES ALLOCATED TO REGISTERS
	.DEF _CodeCnt=R5
	.DEF _IrCode=R6
	.DEF _tx_wr_index=R4
	.DEF _tx_rd_index=R9
	.DEF _tx_counter=R8

	.CSEG
	.ORG 0x00

;INTERRUPT VECTORS
	RJMP __RESET
	RJMP _ext_int0_isr
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP _usart_tx_isr
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00

_tbl10_G100:
	.DB  0x10,0x27,0xE8,0x3,0x64,0x0,0xA,0x0
	.DB  0x1,0x0
_tbl16_G100:
	.DB  0x0,0x10,0x0,0x1,0x10,0x0,0x1,0x0

__RESET:
	CLI
	CLR  R30
	OUT  EECR,R30

;INTERRUPT VECTORS ARE PLACED
;AT THE START OF FLASH
	LDI  R31,1
	OUT  GICR,R31
	OUT  GICR,R30
	OUT  MCUCR,R30

;DISABLE WATCHDOG
	LDI  R31,0x18
	OUT  WDTCR,R31
	OUT  WDTCR,R30

;CLEAR R2-R14
	LDI  R24,(14-2)+1
	LDI  R26,2
	CLR  R27
__CLEAR_REG:
	ST   X+,R30
	DEC  R24
	BRNE __CLEAR_REG

;CLEAR SRAM
	LDI  R24,LOW(0x400)
	LDI  R25,HIGH(0x400)
	LDI  R26,0x60
__CLEAR_SRAM:
	ST   X+,R30
	SBIW R24,1
	BRNE __CLEAR_SRAM

;STACK POINTER INITIALIZATION
	LDI  R30,LOW(0x45F)
	OUT  SPL,R30
	LDI  R30,HIGH(0x45F)
	OUT  SPH,R30

;DATA STACK POINTER INITIALIZATION
	LDI  R28,LOW(0x160)
	LDI  R29,HIGH(0x160)

	RJMP _main

	.ESEG
	.ORG 0

	.DSEG
	.ORG 0x160

	.CSEG
;/*****************************************************
;This program was produced by the
;CodeWizardAVR V2.03.4 Standard
;Automatic Program Generator
;© Copyright 1998-2008 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com
;
;Project : Decode For PT2262
;Version : v3.0 ÔÚDemo2»ù´¡ÉÏ²ÉÓÃ¶à´ÎÏÂÑØÖÐ¶Ï½âÂë(25´Î)
;Date    : 2008-8-26
;Author  : Caijianqiang
;Company : STEP
;Comments: PT2262Õðµ´µç×èÎª4.7M£¬Èçµç×è²»Ò»ÑùÐè¸ÄÒ»ÏÂ³ÌÐòÊ±¼ä
;          Á¬½Ó·½·¨ÊÇ½«PT2262µÄÊý¾ÝÊä³ö¶Ë¾­¹ýÈý¼«¹Ü·´Ïòºó
;          Ö±½Ó½Óµ½MEGA8µÄINT0(PIND.2), ÐÅºÅÓëPT2262·´Ïò
;          ½âÂë½á¹û·ÅÔÚReceive[]Àï, LLMMHH
;
;Chip type           : ATmega8
;Program type        : Application
;Clock frequency     : 1.000000 MHz
;Memory model        : Small
;External RAM size   : 0
;Data Stack size     : 256
;*****************************************************/
;
;#include <mega8.h>
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x80
	.EQU __sm_mask=0x70
	.EQU __sm_powerdown=0x20
	.EQU __sm_powersave=0x30
	.EQU __sm_standby=0x60
	.EQU __sm_ext_standby=0x70
	.EQU __sm_adc_noise_red=0x10
	.SET power_ctrl_reg=mcucr
	#endif
;#include <delay.h>
;
;/*--------------------------------------------------*/
;#define RUN   PORTB.0
;#define LEDA  PORTB.1
;#define LEDB  PORTB.2
;#define IrDat PIND.2
;
;/*--------------------------------------------------*/
;#define MinSync	12000	// Í¬²½ÂëÏÂÏÞ
;#define MaxSync 18000	// Í¬²½ÂëÉÏÏÞ
;#define MinNarr 400		// Õ­Âö³åÏÂÏÞ
;#define MaxNarr 612		// Õ­Âö³åÉÏÏÞ
;#define MinWide 1200	// ¿íÂö³åÉÏÏÞ
;#define MaxWide 1800	// ¿íÂö³åÉÏÏÞ
;
;/*--------------------------------------------------*/
;// Declare your global variables here
;unsigned char CodeCnt;
;unsigned char Receive[3];
;unsigned int  IrCode;
;unsigned char ValStr[5];
;
;/*--------------------------------------------------*/
;// External Interrupt 0 service routine
;interrupt [EXT_INT0] void ext_int0_isr(void)
; 0000 0035 {

	.CSEG
_ext_int0_isr:
	ST   -Y,R0
	ST   -Y,R1
	ST   -Y,R25
	ST   -Y,R26
	ST   -Y,R27
	ST   -Y,R30
	ST   -Y,R31
	IN   R30,SREG
	ST   -Y,R30
; 0000 0036  // Place your code here
; 0000 0037  unsigned char i;
; 0000 0038 
; 0000 0039  TCNT1=0;
	ST   -Y,R17
;	i -> R17
	RCALL SUBOPT_0x0
; 0000 003A  TCCR1B=0x01;						// ²âµÍµçÆ½¿í¶È
	LDI  R30,LOW(1)
	OUT  0x2E,R30
; 0000 003B 
; 0000 003C  if(CodeCnt==0)
	TST  R5
	BRNE _0x3
; 0000 003D  {									// ÕÒÍ¬²½Âë
; 0000 003E   TCNT1=0;
	RCALL SUBOPT_0x0
; 0000 003F   while(!IrDat);					// µÈ´ý¸ßµçÆ½
_0x4:
	SBIS 0x10,2
	RJMP _0x4
; 0000 0040   TCCR1B=0x01;						// ²â¸ßµçÆ½¿í¶È
	LDI  R30,LOW(1)
	OUT  0x2E,R30
; 0000 0041   while (IrDat)						// µÈ´ýµÍµçÆ½
_0x7:
	SBIS 0x10,2
	RJMP _0x9
; 0000 0042    if(TIFR&0x04) goto RemExit;		// ³¬Ê±¾ÍÍË³ö
	RCALL SUBOPT_0x1
	BREQ _0xA
	RJMP _0xB
; 0000 0043   TCCR1B=0x00;						// ÖÐÖ¹²âÁ¿
_0xA:
	RJMP _0x7
_0x9:
	RCALL SUBOPT_0x2
; 0000 0044 
; 0000 0045   if(TCNT1>MaxSync||TCNT1<MinSync) 	// ÅÐ¶ÏÍ¬²½ÂëµÄ¿í¶È
	RCALL SUBOPT_0x3
	CPI  R30,LOW(0x4651)
	LDI  R26,HIGH(0x4651)
	CPC  R31,R26
	BRSH _0xD
	RCALL SUBOPT_0x3
	CPI  R30,LOW(0x2EE0)
	LDI  R26,HIGH(0x2EE0)
	CPC  R31,R26
	BRSH _0xC
_0xD:
; 0000 0046   {									// Í¬²½ÐÅºÅ´íÎó
; 0000 0047    goto RemExit;
	RJMP _0xB
; 0000 0048   }
; 0000 0049   // Í¬²½ÐÅºÅ¼ì²â³É¹¦£¬×¼±¸¼ì²â24Î»Êý¾Ý(16770us)
; 0000 004A   CodeCnt=1;
_0xC:
	LDI  R30,LOW(1)
	MOV  R5,R30
; 0000 004B   IrCode=0;
	CLR  R6
	CLR  R7
; 0000 004C  }
; 0000 004D  else
	RJMP _0xF
_0x3:
; 0000 004E  {
; 0000 004F   if(CodeCnt<25)
	LDI  R30,LOW(25)
	CP   R5,R30
	BRLO PC+2
	RJMP _0x10
; 0000 0050   {
; 0000 0051    // ¼ì²â24Î»Êý¾Ý
; 0000 0052    while(!IrDat)  					// µÈ´ý±ä¸ßµçÆ½
_0x11:
	SBIC 0x10,2
	RJMP _0x13
; 0000 0053     if(TIFR&0x04)
	RCALL SUBOPT_0x1
	BREQ _0x14
; 0000 0054     {
; 0000 0055      CodeCnt=0;
	CLR  R5
; 0000 0056      goto RemExit;					// ³¬Ê±¾ÍÍË³ö
	RJMP _0xB
; 0000 0057     }
; 0000 0058    TCCR1B=0x00;						// ÖÐÖ¹²âÁ¿
_0x14:
	RJMP _0x11
_0x13:
	RCALL SUBOPT_0x2
; 0000 0059    i=(CodeCnt-1)/8;
	MOV  R26,R5
	RCALL SUBOPT_0x4
	SBIW R26,1
	LDI  R30,LOW(8)
	LDI  R31,HIGH(8)
	RCALL __DIVW21
	MOV  R17,R30
; 0000 005A 
; 0000 005B    if (TCNT1>MinWide && TCNT1<MaxWide)
	RCALL SUBOPT_0x3
	CPI  R30,LOW(0x4B1)
	LDI  R26,HIGH(0x4B1)
	CPC  R31,R26
	BRLO _0x16
	RCALL SUBOPT_0x3
	CPI  R30,LOW(0x708)
	LDI  R26,HIGH(0x708)
	CPC  R31,R26
	BRLO _0x17
_0x16:
	RJMP _0x15
_0x17:
; 0000 005C    {
; 0000 005D     Receive[i]=Receive[i]>>1;
	RCALL SUBOPT_0x5
	ASR  R31
	ROR  R30
	ST   X,R30
; 0000 005E     Receive[i]=Receive[i]+0x80;		// ±£´æ1(1471us)
	RCALL SUBOPT_0x5
	SUBI R30,LOW(-128)
	SBCI R31,HIGH(-128)
	ST   X,R30
; 0000 005F    }
; 0000 0060    else
	RJMP _0x18
_0x15:
; 0000 0061     if(TCNT1>MinNarr && TCNT1<MaxNarr)
	RCALL SUBOPT_0x3
	CPI  R30,LOW(0x191)
	LDI  R26,HIGH(0x191)
	CPC  R31,R26
	BRLO _0x1A
	RCALL SUBOPT_0x3
	CPI  R30,LOW(0x264)
	LDI  R26,HIGH(0x264)
	CPC  R31,R26
	BRLO _0x1B
_0x1A:
	RJMP _0x19
_0x1B:
; 0000 0062     {
; 0000 0063      Receive[i]=Receive[i]>>1;		// ±£´æ0(511us)
	RCALL SUBOPT_0x5
	ASR  R31
	ROR  R30
	ST   X,R30
; 0000 0064     }
; 0000 0065     else
	RJMP _0x1C
_0x19:
; 0000 0066     {
; 0000 0067      CodeCnt=0;
	CLR  R5
; 0000 0068      goto RemExit;
	RJMP _0xB
; 0000 0069     }
_0x1C:
_0x18:
; 0000 006A    if((CodeCnt&1)==0)
	RCALL SUBOPT_0x6
	ANDI R30,LOW(0x1)
	BRNE _0x1D
; 0000 006B    {								// È¡Êý¾ÝÂë
; 0000 006C    	IrCode>>=1;
	LSR  R7
	ROR  R6
; 0000 006D    	if((Receive[i]&0xC0)==0xC0)
	MOV  R30,R17
	RCALL SUBOPT_0x7
	SUBI R30,LOW(-_Receive)
	SBCI R31,HIGH(-_Receive)
	LD   R30,Z
	RCALL SUBOPT_0x7
	ANDI R30,LOW(0xC0)
	CPI  R30,LOW(0xC0)
	BRNE _0x1E
; 0000 006E    	{								// 1
; 0000 006F    	 IrCode|=0x800;
	LDI  R30,LOW(8)
	OR   R7,R30
; 0000 0070    	}
; 0000 0071    }
_0x1E:
; 0000 0072    CodeCnt+=1;
_0x1D:
	RCALL SUBOPT_0x6
	ADIW R30,1
	MOV  R5,R30
; 0000 0073   }
; 0000 0074  }
_0x10:
_0xF:
; 0000 0075 
; 0000 0076  RemExit:
_0xB:
; 0000 0077  TCCR1B=0x00;
	RCALL SUBOPT_0x2
; 0000 0078  TIFR|=0x04;
	IN   R30,0x38
	RCALL SUBOPT_0x7
	ORI  R30,4
	OUT  0x38,R30
; 0000 0079 }
	LD   R17,Y+
	LD   R30,Y+
	OUT  SREG,R30
	LD   R31,Y+
	LD   R30,Y+
	LD   R27,Y+
	LD   R26,Y+
	LD   R25,Y+
	LD   R1,Y+
	LD   R0,Y+
	RETI
;
;/*--------------------------------------------------*/
;#define RXB8 1
;#define TXB8 0
;#define UPE  2
;#define OVR  3
;#define FE   4
;#define UDRE 5
;#define RXC  7
;
;#define FRAMING_ERROR       (1<<FE)
;#define PARITY_ERROR        (1<<UPE)
;#define DATA_OVERRUN        (1<<OVR)
;#define DATA_REGISTER_EMPTY (1<<UDRE)
;#define RX_COMPLETE         (1<<RXC)
;
;// USART Transmitter buffer
;#define TX_BUFFER_SIZE 8
;char tx_buffer[TX_BUFFER_SIZE];
;
;#if TX_BUFFER_SIZE<256
;unsigned char tx_wr_index,tx_rd_index,tx_counter;
;#else
;unsigned int tx_wr_index,tx_rd_index,tx_counter;
;#endif
;
;// USART Transmitter interrupt service routine
;interrupt [USART_TXC] void usart_tx_isr(void)
; 0000 0096 {
_usart_tx_isr:
	ST   -Y,R30
	ST   -Y,R31
	IN   R30,SREG
	ST   -Y,R30
; 0000 0097  if (tx_counter)
	TST  R8
	BREQ _0x1F
; 0000 0098  {
; 0000 0099   --tx_counter;
	DEC  R8
; 0000 009A   UDR=tx_buffer[tx_rd_index];
	MOV  R30,R9
	RCALL SUBOPT_0x7
	SUBI R30,LOW(-_tx_buffer)
	SBCI R31,HIGH(-_tx_buffer)
	LD   R30,Z
	OUT  0xC,R30
; 0000 009B   if (++tx_rd_index == TX_BUFFER_SIZE) tx_rd_index=0;
	INC  R9
	LDI  R30,LOW(8)
	CP   R30,R9
	BRNE _0x20
	CLR  R9
; 0000 009C  };
_0x20:
_0x1F:
; 0000 009D }
	LD   R30,Y+
	OUT  SREG,R30
	LD   R31,Y+
	LD   R30,Y+
	RETI
;
;#ifndef _DEBUG_TERMINAL_IO_
;// Write a character to the USART Transmitter buffer
;#define _ALTERNATE_PUTCHAR_
;#pragma used+
;void putchar(char c)
; 0000 00A4 {
_putchar:
; 0000 00A5  while (tx_counter == TX_BUFFER_SIZE);
;	c -> Y+0
_0x21:
	LDI  R30,LOW(8)
	CP   R30,R8
	BREQ _0x21
; 0000 00A6  #asm("cli")
	cli
; 0000 00A7  if (tx_counter || ((UCSRA & DATA_REGISTER_EMPTY)==0))
	TST  R8
	BRNE _0x25
	IN   R30,0xB
	RCALL SUBOPT_0x7
	ANDI R30,LOW(0x20)
	BRNE _0x24
_0x25:
; 0000 00A8  {
; 0000 00A9   tx_buffer[tx_wr_index]=c;
	MOV  R30,R4
	RCALL SUBOPT_0x7
	SUBI R30,LOW(-_tx_buffer)
	SBCI R31,HIGH(-_tx_buffer)
	LD   R26,Y
	STD  Z+0,R26
; 0000 00AA   if (++tx_wr_index == TX_BUFFER_SIZE) tx_wr_index=0;
	INC  R4
	LDI  R30,LOW(8)
	CP   R30,R4
	BRNE _0x27
	CLR  R4
; 0000 00AB   ++tx_counter;
_0x27:
	INC  R8
; 0000 00AC  }
; 0000 00AD  else
	RJMP _0x28
_0x24:
; 0000 00AE   UDR=c;
	LD   R30,Y
	OUT  0xC,R30
; 0000 00AF  #asm("sei")
_0x28:
	sei
; 0000 00B0 }
	ADIW R28,1
	RET
;#pragma used-
;#endif
;
;// Standard Input/Output functions
;#include <stdio.h>
;
;/*--------------------------------------------------*/
;void ValToStr(unsigned int Val)
; 0000 00B9 {
_ValToStr:
; 0000 00BA  unsigned int i;
; 0000 00BB 
; 0000 00BC  i=Val;
	RCALL __SAVELOCR2
;	Val -> Y+2
;	i -> R16,R17
	__GETWRS 16,17,2
; 0000 00BD  ValStr[0]=i/1000+0x30;
	MOVW R26,R16
	LDI  R30,LOW(1000)
	LDI  R31,HIGH(1000)
	RCALL __DIVW21U
	ADIW R30,48
	STS  _ValStr,R30
; 0000 00BE  i=i%1000;
	MOVW R26,R16
	LDI  R30,LOW(1000)
	LDI  R31,HIGH(1000)
	RCALL __MODW21U
	MOVW R16,R30
; 0000 00BF  ValStr[1]=i/100+0x30;
	MOVW R26,R16
	LDI  R30,LOW(100)
	LDI  R31,HIGH(100)
	RCALL __DIVW21U
	ADIW R30,48
	__PUTB1MN _ValStr,1
; 0000 00C0  i=i%100;
	MOVW R26,R16
	LDI  R30,LOW(100)
	LDI  R31,HIGH(100)
	RCALL __MODW21U
	MOVW R16,R30
; 0000 00C1  ValStr[2]=i/10+0x30;
	MOVW R26,R16
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	RCALL __DIVW21U
	ADIW R30,48
	__PUTB1MN _ValStr,2
; 0000 00C2  ValStr[3]=i%10+0x30;
	MOVW R26,R16
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	RCALL __MODW21U
	ADIW R30,48
	__PUTB1MN _ValStr,3
; 0000 00C3  ValStr[4]=0;
	LDI  R30,LOW(0)
	__PUTB1MN _ValStr,4
; 0000 00C4 }
	RCALL __LOADLOCR2
	ADIW R28,4
	RET
;
;/*--------------------------------------------------*/
;void main(void)
; 0000 00C8 {
_main:
; 0000 00C9  // Declare your local variables here
; 0000 00CA 
; 0000 00CB  // Input/Output Ports initialization
; 0000 00CC  // Port B initialization
; 0000 00CD  // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=Out Func1=Out Func0=Out
; 0000 00CE  // State7=T State6=T State5=T State4=T State3=T State2=0 State1=0 State0=0
; 0000 00CF  PORTB=0x00;
	LDI  R30,LOW(0)
	OUT  0x18,R30
; 0000 00D0  DDRB=0x07;
	LDI  R30,LOW(7)
	OUT  0x17,R30
; 0000 00D1 
; 0000 00D2  // Port C initialization
; 0000 00D3  // Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
; 0000 00D4  // State6=T State5=T State4=T State3=T State2=T State1=T State0=T
; 0000 00D5  PORTC=0x00;
	LDI  R30,LOW(0)
	OUT  0x15,R30
; 0000 00D6  DDRC=0x00;
	OUT  0x14,R30
; 0000 00D7 
; 0000 00D8  // Port D initialization
; 0000 00D9  // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
; 0000 00DA  // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
; 0000 00DB  PORTD=0x00;
	OUT  0x12,R30
; 0000 00DC  DDRD=0x00;
	OUT  0x11,R30
; 0000 00DD 
; 0000 00DE  // Timer/Counter 0 initialization
; 0000 00DF  // Clock source: System Clock
; 0000 00E0  // Clock value: Timer 0 Stopped
; 0000 00E1  TCCR0=0x00;
	OUT  0x33,R30
; 0000 00E2  TCNT0=0x00;
	OUT  0x32,R30
; 0000 00E3 
; 0000 00E4  // Timer/Counter 1 initialization
; 0000 00E5  // Clock source: System Clock
; 0000 00E6  // Clock value: Timer 1 Stopped
; 0000 00E7  // Mode: Normal top=FFFFh
; 0000 00E8  // OC1A output: Discon.
; 0000 00E9  // OC1B output: Discon.
; 0000 00EA  // Noise Canceler: Off
; 0000 00EB  // Input Capture on Falling Edge
; 0000 00EC  // Timer 1 Overflow Interrupt: Off
; 0000 00ED  // Input Capture Interrupt: Off
; 0000 00EE  // Compare A Match Interrupt: Off
; 0000 00EF  // Compare B Match Interrupt: Off
; 0000 00F0  TCCR1A=0x00;
	OUT  0x2F,R30
; 0000 00F1  TCCR1B=0x00;
	RCALL SUBOPT_0x2
; 0000 00F2  TCNT1H=0x00;
	LDI  R30,LOW(0)
	OUT  0x2D,R30
; 0000 00F3  TCNT1L=0x00;
	OUT  0x2C,R30
; 0000 00F4  ICR1H=0x00;
	OUT  0x27,R30
; 0000 00F5  ICR1L=0x00;
	OUT  0x26,R30
; 0000 00F6  OCR1AH=0x00;
	OUT  0x2B,R30
; 0000 00F7  OCR1AL=0x00;
	OUT  0x2A,R30
; 0000 00F8  OCR1BH=0x00;
	OUT  0x29,R30
; 0000 00F9  OCR1BL=0x00;
	OUT  0x28,R30
; 0000 00FA 
; 0000 00FB  // Timer/Counter 2 initialization
; 0000 00FC  // Clock source: System Clock
; 0000 00FD  // Clock value: Timer 2 Stopped
; 0000 00FE  // Mode: Normal top=FFh
; 0000 00FF  // OC2 output: Disconnected
; 0000 0100  ASSR=0x00;
	OUT  0x22,R30
; 0000 0101  TCCR2=0x00;
	OUT  0x25,R30
; 0000 0102  TCNT2=0x00;
	OUT  0x24,R30
; 0000 0103  OCR2=0x00;
	OUT  0x23,R30
; 0000 0104 
; 0000 0105  // External Interrupt(s) initialization
; 0000 0106  // INT0: On
; 0000 0107  // INT0 Mode: Falling Edge
; 0000 0108  // INT1: Off
; 0000 0109  GICR|=0x40;
	IN   R30,0x3B
	RCALL SUBOPT_0x7
	ORI  R30,0x40
	OUT  0x3B,R30
; 0000 010A  MCUCR=0x02;
	LDI  R30,LOW(2)
	OUT  0x35,R30
; 0000 010B  GIFR=0x40;
	LDI  R30,LOW(64)
	OUT  0x3A,R30
; 0000 010C 
; 0000 010D  // Timer(s)/Counter(s) Interrupt(s) initialization
; 0000 010E  TIMSK=0x00;
	LDI  R30,LOW(0)
	OUT  0x39,R30
; 0000 010F 
; 0000 0110  // USART initialization
; 0000 0111  // Communication Parameters: 8 Data, 1 Stop, No Parity
; 0000 0112  // USART Receiver: Off
; 0000 0113  // USART Transmitter: On
; 0000 0114  // USART Mode: Asynchronous
; 0000 0115  // USART Baud Rate: 4800
; 0000 0116  UCSRA=0x00;
	OUT  0xB,R30
; 0000 0117  UCSRB=0x48;
	LDI  R30,LOW(72)
	OUT  0xA,R30
; 0000 0118  UCSRC=0x86;
	LDI  R30,LOW(134)
	OUT  0x20,R30
; 0000 0119  UBRRH=0x00;
	LDI  R30,LOW(0)
	OUT  0x20,R30
; 0000 011A  UBRRL=0x0C;
	LDI  R30,LOW(12)
	OUT  0x9,R30
; 0000 011B 
; 0000 011C  // Analog Comparator initialization
; 0000 011D  // Analog Comparator: Off
; 0000 011E  // Analog Comparator Input Capture by Timer/Counter 1: Off
; 0000 011F  ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
; 0000 0120  SFIOR=0x00;
	LDI  R30,LOW(0)
	OUT  0x30,R30
; 0000 0121 
; 0000 0122  // Global enable interrupts
; 0000 0123  #asm("sei")
	sei
; 0000 0124 
; 0000 0125  CodeCnt=0;
	CLR  R5
; 0000 0126 
; 0000 0127  putchar('A');
	LDI  R30,LOW(65)
	RCALL SUBOPT_0x8
; 0000 0128  putchar('B');
	LDI  R30,LOW(66)
	RCALL SUBOPT_0x8
; 0000 0129  putchar('C');
	LDI  R30,LOW(67)
	RCALL SUBOPT_0x8
; 0000 012A 
; 0000 012B  while (1)
_0x29:
; 0000 012C  {
; 0000 012D   // Place your code here
; 0000 012E   delay_ms(500);
	LDI  R30,LOW(500)
	LDI  R31,HIGH(500)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _delay_ms
; 0000 012F   if(RUN) RUN=0; else RUN=1;
	SBIS 0x18,0
	RJMP _0x2C
	CBI  0x18,0
	RJMP _0x2F
_0x2C:
	SBI  0x18,0
; 0000 0130 
; 0000 0131   if(CodeCnt>24)
_0x2F:
	LDI  R30,LOW(24)
	CP   R30,R5
	BRSH _0x32
; 0000 0132   {
; 0000 0133    CodeCnt=0;
	CLR  R5
; 0000 0134    // ½â³öÊý¾ÝÂë, Ðü¿Õ×÷0
; 0000 0135    ValToStr(IrCode);
	ST   -Y,R7
	ST   -Y,R6
	RCALL _ValToStr
; 0000 0136    puts(ValStr);
	LDI  R30,LOW(_ValStr)
	LDI  R31,HIGH(_ValStr)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _puts
; 0000 0137    if(IrCode==0x123)
	LDI  R30,LOW(291)
	LDI  R31,HIGH(291)
	CP   R30,R6
	CPC  R31,R7
	BRNE _0x33
; 0000 0138    	if(LEDA) LEDA=0; else LEDA=1;
	SBIS 0x18,1
	RJMP _0x34
	CBI  0x18,1
	RJMP _0x37
_0x34:
	SBI  0x18,1
; 0000 0139    if(IrCode==0x321)
_0x37:
_0x33:
	LDI  R30,LOW(801)
	LDI  R31,HIGH(801)
	CP   R30,R6
	CPC  R31,R7
	BRNE _0x3A
; 0000 013A    	if(LEDB) LEDB=0; else LEDB=1;
	SBIS 0x18,2
	RJMP _0x3B
	CBI  0x18,2
	RJMP _0x3E
_0x3B:
	SBI  0x18,2
; 0000 013B   }
_0x3E:
_0x3A:
; 0000 013C  };
_0x32:
	RJMP _0x29
; 0000 013D }
_0x41:
	RJMP _0x41
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x80
	.EQU __sm_mask=0x70
	.EQU __sm_powerdown=0x20
	.EQU __sm_powersave=0x30
	.EQU __sm_standby=0x60
	.EQU __sm_ext_standby=0x70
	.EQU __sm_adc_noise_red=0x10
	.SET power_ctrl_reg=mcucr
	#endif

	.CSEG
_puts:
	ST   -Y,R17
_0x2000003:
	LDD  R26,Y+1
	LDD  R27,Y+1+1
	LD   R30,X+
	STD  Y+1,R26
	STD  Y+1+1,R27
	MOV  R17,R30
	CPI  R30,0
	BREQ _0x2000005
	ST   -Y,R17
	RCALL _putchar
	RJMP _0x2000003
_0x2000005:
	LDI  R30,LOW(10)
	RCALL SUBOPT_0x8
	LDD  R17,Y+0
	ADIW R28,3
	RET

	.CSEG

	.CSEG

	.DSEG
_Receive:
	.BYTE 0x3
_ValStr:
	.BYTE 0x5
_tx_buffer:
	.BYTE 0x8
_p_S1020024:
	.BYTE 0x2

	.CSEG
;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x0:
	LDI  R30,LOW(0)
	LDI  R31,HIGH(0)
	OUT  0x2C+1,R31
	OUT  0x2C,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:2 WORDS
SUBOPT_0x1:
	IN   R30,0x38
	LDI  R31,0
	ANDI R30,LOW(0x4)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x2:
	LDI  R30,LOW(0)
	OUT  0x2E,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES, CODE SIZE REDUCTION:3 WORDS
SUBOPT_0x3:
	IN   R30,0x2C
	IN   R31,0x2C+1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:4 WORDS
SUBOPT_0x4:
	LDI  R27,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:24 WORDS
SUBOPT_0x5:
	MOV  R26,R17
	RCALL SUBOPT_0x4
	SUBI R26,LOW(-_Receive)
	SBCI R27,HIGH(-_Receive)
	MOV  R30,R17
	LDI  R31,0
	SUBI R30,LOW(-_Receive)
	SBCI R31,HIGH(-_Receive)
	LD   R30,Z
	LDI  R31,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x6:
	MOV  R30,R5
	LDI  R31,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 7 TIMES, CODE SIZE REDUCTION:10 WORDS
SUBOPT_0x7:
	LDI  R31,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES, CODE SIZE REDUCTION:1 WORDS
SUBOPT_0x8:
	ST   -Y,R30
	RJMP _putchar


	.CSEG
_delay_ms:
	ld   r30,y+
	ld   r31,y+
	adiw r30,0
	breq __delay_ms1
__delay_ms0:
	__DELAY_USW 0xFA
	wdr
	sbiw r30,1
	brne __delay_ms0
__delay_ms1:
	ret

__ANEGW1:
	NEG  R31
	NEG  R30
	SBCI R31,0
	RET

__DIVW21U:
	CLR  R0
	CLR  R1
	LDI  R25,16
__DIVW21U1:
	LSL  R26
	ROL  R27
	ROL  R0
	ROL  R1
	SUB  R0,R30
	SBC  R1,R31
	BRCC __DIVW21U2
	ADD  R0,R30
	ADC  R1,R31
	RJMP __DIVW21U3
__DIVW21U2:
	SBR  R26,1
__DIVW21U3:
	DEC  R25
	BRNE __DIVW21U1
	MOVW R30,R26
	MOVW R26,R0
	RET

__DIVW21:
	RCALL __CHKSIGNW
	RCALL __DIVW21U
	BRTC __DIVW211
	RCALL __ANEGW1
__DIVW211:
	RET

__MODW21U:
	RCALL __DIVW21U
	MOVW R30,R26
	RET

__CHKSIGNW:
	CLT
	SBRS R31,7
	RJMP __CHKSW1
	RCALL __ANEGW1
	SET
__CHKSW1:
	SBRS R27,7
	RJMP __CHKSW2
	COM  R26
	COM  R27
	ADIW R26,1
	BLD  R0,0
	INC  R0
	BST  R0,0
__CHKSW2:
	RET

__SAVELOCR2:
	ST   -Y,R17
	ST   -Y,R16
	RET

__LOADLOCR2:
	LDD  R17,Y+1
	LD   R16,Y
	RET

;END OF CODE MARKER
__END_OF_CODE:
