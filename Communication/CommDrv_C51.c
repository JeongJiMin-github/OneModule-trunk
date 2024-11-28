/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Communication Driver Source File\n
* @file    CommDrv.c
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

/*
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
/* Insert include files here */
#include "../Common/Globals.h"
#include "../Common/RingBuf.h"
#include "CommDrv.h"

#include "../Hardware/HardwareConfig.h"
#ifdef COMM_DRV_C51
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief Communication Driver Rx Ring Buffer */
RING_BUF SerialRxRingBuf;
/** @brief Communication Driver Tx Ring Buffer */
RING_BUF SerialTxRingBuf;

/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
#ifdef X2_MODE
/** @brief clock divider 값을 계산하기 위한 상수 */
#define BAUD_MULTI_CONST	16L
#else
/** @brief clock divider 값을 계산하기 위한 상수 */
#define BAUD_MULTI_CONST	32L
#endif

/** @brief Communication Driver Rx Ring Buffer Size */
#define SERIAL_RX_BUF_SIZE	40
/** @brief Communication Driver Tx Ring Buffer Size */
//#define SERIAL_TX_BUF_SIZE	72
#define SERIAL_TX_BUF_SIZE	62

/*
********************************************************************************
*                       LOCAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert local typedef & structure here */


/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/
/* Insert file scope variable & tables here */
/** @brief Communication Driver Rx Ring Buffer에서 사용할 byte buffer arrary */
static char rxData[SERIAL_RX_BUF_SIZE];

/** @brief Communication Driver Tx Ring Buffer에서 사용할 byte buffer arrary */
static char txData[SERIAL_TX_BUF_SIZE];

/** @brief Tx Data Transmit 중임을 알리는 Flag, 전송중 전송 방지  */
static BOOLEAN serialTxing;
/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */


/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/

/* Insert global functions here */
/**
********************************************************************************
* @brief    Communication Driver Init 함수\n
*           Serial Communication Ring Buffer Init\n
*           Serial Communication Speed Setting\n
* @param    none
* @return   none
* @remark   일정 시간에 한번씩 계속 실행 되는 함수\n
********************************************************************************
*/
void CommDrvInit(INT8U baud)
{
	INT32U	divider;

	RingBufInit(&SerialRxRingBuf, &rxData[0], SERIAL_RX_BUF_SIZE);
	RingBufInit(&SerialTxRingBuf, &txData[0], SERIAL_TX_BUF_SIZE);

 	switch (baud) 
	{
		case BAUD_RATE_9600:
		default:
			divider = OSC/(BAUD_MULTI_CONST*9600);
			break;
		case BAUD_RATE_19200:
		    divider = OSC/(BAUD_MULTI_CONST*19200);
			break;
		case BAUD_RATE_38400:
		    divider = OSC/(BAUD_MULTI_CONST*38400);
			break;
		case BAUD_RATE_57600:
		    divider = OSC/(BAUD_MULTI_CONST*57600);
			break;
		case BAUD_RATE_115200:
		    divider = OSC/(BAUD_MULTI_CONST*115200);
			break;
		case BAUD_RATE_2400:
		    divider = OSC/(BAUD_MULTI_CONST*2400);
			break;
	}
	//Using Timer 2 to generate buad rates
	// Baud Rate = OSC / (32 x (65536 - (RCAP2H,RCAP2L))
	// RCAP2H,RCAP2L = 65536 - (OSC / (32 * Baud Rate)
	// for example 9600 baud
	// 65536 - (11059200 / (32 * 9600)) => 0xffdc
	// RCAP2H = 0xff, RCAP2L = 0xdc
	//if X2_MODE, 32L --> 16L
	TL2 = RCAP2L = -divider;
    	TH2 = RCAP2H = -divider >> 8;

	// Timer/Counter 2 control Register
	// T2CON = [TF2|EXF2|RCLK|TCLK|EXEN2|ER2|C/T2|C{/RL2] 7..0
	// TF2 = Timer 2 Overflow Flag
	// EXF2 = Timer 2 External Flag
	// RCLK = Receive Clock Flag (Serial Port)
	// TCLK = Transmit Clock Flag (Serial Port)
	// EXEN2 = Timer 2 External Enable Flag
	// TR2 = Software Start/Stop Control for Timer 2
	// C/(/T2) = Timer or Counter Select
	// CP/(/RL2) = Captuer/Reload Flag
	T2CON = 0x3c;     // Use for Serial Port
	// Serial Port Control Register
	// SCON = [SM0|SM1|SM2|REN|TB8|RB8|TI|RI]
	// SM0 SM1 = Description, Baud Rate
	// 0   0   = Shift Register, Fosc/12 (mode 0)
	// 0   1   = 8-bit UART, Variable (mode 1)
	// 1   0   = 9-bit UART, Fosc/64 or Fosc/32 (mode 2)
	// 1   1   = 9-bit UART, Variable (mode 3)
	// SM2 = Enable Multi Processor Communication
	// REN = Enable Reception
	// TB8 = 9th bit that will be transmitted in modes 2 & 3
	// RB8 = In mode 2 & 3, 9th bit that was received
	// TI = Transmit Interrupt Flag, Must be cleared by software
	// RI = Receive Interrupt Flag, Must be cleared by software
    SCON = 0x50;      // variable 8 bit UART(mode 1)/enable reception
    // Power Control Register(not bit addressable)
	// PCON = [SMOD|-|-|-|GF1|GF0|PD|IDL] 7..0
	// SMOD = Double baud rate bit
	// GF1 = Gerneral purpose flag bit
	// GF0 = Gerneral purpose flag bit
	// PD = Power Down bit
	// IDL = Idle Mode bit
	PCON |= 0x80;     // SMOD = 1 (when timer 1 use for Serial)

	ES = 1;//Enable Serial Port Interrupt(in TCON)

	serialTxing = OFF;
}

/**
********************************************************************************
* @brief    Communication Driver Procedure 함수\n
*           Communication Driver Tx Ring Buffer 확인 후 Data 보냄
*           (Serial Interrupt Flag On 시킴)
* @param    none
* @return   none
* @remark   일정 시간에 한번씩 계속 실행 되는 함수\n
********************************************************************************
*/
void  CommDrvProc(void)
{  
	if (RingBufCheckBuf(&SerialTxRingBuf) && (serialTxing == OFF))
	{
		serialTxing = ON;
		TI = 1;
	}
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
/**
********************************************************************************
* @brief    Communication Driver Interrupt 함수\n
* @param    none
* @return   none
* @remark   8051계열의 Serial Interrpt 처리 (Tx, Rx)\n
*           받은 Data가 있으면 Rx Ring Buffer에 저장\n
*           보낼 Data가 있으면 Tx Ring Buffer에서 가져와 전송
********************************************************************************
*/
//interrupt [0x23] void serialInt (void)    /* Serial Port */ 
 void serialInt (void) interrupt 4  /* Serial Port */ 
{
	INT8U temp;

	if (RI) 
	{
		temp = SBUF;
		RingBufPutCharForCommInterrupt(&SerialRxRingBuf, temp);
		RI = 0;
	}

	if (TI)
	{
		if (RingBufCheckBufForCommInterrupt(&SerialTxRingBuf))
		{
			SBUF = RingBufGetCharForCommInterrupt(&SerialTxRingBuf);
		}
		else
		{
			serialTxing = OFF;
		}
		TI = 0;
	}
}
#endif//#ifdef COMM_DRV_C51
