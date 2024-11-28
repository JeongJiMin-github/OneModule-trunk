/**
********************************************************************************
* Copyright (c) 2014 CAS
* @brief   Communication Driver Source File \n
* @file    CommDrv_Sm59r16a5.c
* @version 1.0
* @date    2014/02/24
* @author  Keum Young-Kwang (KYK)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2014/02/24 created by KYK \n
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
#ifdef COMM_DRV_SM59R16A5
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
/** @brief clock divider ���� ����ϱ� ���� ��� */
#define BAUD_MULTI_CONST	32L
#else
/** @brief clock divider ���� ����ϱ� ���� ��� */
#define BAUD_MULTI_CONST	32L
#endif

/** @brief Communication Driver Rx Ring Buffer Size */
#define SERIAL_RX_BUF_SIZE	32
/** @brief Communication Driver Tx Ring Buffer Size */
#define SERIAL_TX_BUF_SIZE	72

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
/** @brief Communication Driver Rx Ring Buffer���� ����� byte buffer arrary */
static char rxData[SERIAL_RX_BUF_SIZE];

/** @brief Communication Driver Tx Ring Buffer���� ����� byte buffer arrary */
static char txData[SERIAL_TX_BUF_SIZE];

/** @brief Tx Data Transmit ������ �˸��� Flag, ������ ���� ����  */
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
* @brief    Communication Driver Init �Լ�\n
*           Serial Communication Ring Buffer Init\n
*           Serial Communication Speed Setting\n
* @param    none
* @return   none
* @remark   ���� �ð��� �ѹ��� ��� ���� �Ǵ� �Լ�\n
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
	// Baud Rate = (2^SMOD * OSC) / (64 x (1024 - (S0RELH,S0RELL))
	// S0RELH,S0RELL = 1024 - (2^SMOD * OSC / (64 * Baud Rate)
	// for example 9600 baud
	// 1024 - (2^1 * 11059200 / (64 * 9600)) => 988 = 0x3dc
	// S0RELH = 0x03,S0RELL = 0xdc
	//if X2_MODE, 32L is same 
	S0RELL = 1024 - divider;
	S0RELH = (1024 - divider) >> 8;
	AUX |= 0x80;			// BRS = 1
	PCON |= 0x80;			// SMOD =1
	IEN0 |= 0x90;			// EA=1, ES0=1, interrupt enable
	S0CON = 0x50;			// mode 1, SM20=0, Revice enable
	TI=0;

	serialTxing = OFF;
}

/**
********************************************************************************
* @brief    Communication Driver Procedure �Լ�\n
*           Communication Driver Tx Ring Buffer Ȯ�� �� Data ����
*           (Serial Interrupt Flag On ��Ŵ)
* @param    none
* @return   none
* @remark   ���� �ð��� �ѹ��� ��� ���� �Ǵ� �Լ�\n
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
* @brief    Communication Driver Interrupt �Լ�\n
* @param    none
* @return   none
* @remark   8051�迭�� Serial Interrpt ó�� (Tx, Rx)\n
*           ���� Data�� ������ Rx Ring Buffer�� ����\n
*           ���� Data�� ������ Tx Ring Buffer���� ������ ����
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
#endif//#ifdef COMM_DRV_SM59R16A5
