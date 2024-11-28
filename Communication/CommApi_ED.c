/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Communication API Source File\n
* @file    CommApi.c
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
#include "../Common/CommonLib.h"
#include "../Common/RingBuf.h"
#include "../Common/AdData.h"
#include "../Common/SaleData.h"
#include "../Common/UiDefine.h"
#include "../Power/PowerApi.h"
#include "../UI/DispApi.h"
#include "../UI/KeyApi.h"
#include "../DB/StorageApi.h"
#include "../DB/ParamApi.h"
#include "../Ad/AdApi.h"
#include "../Ad/TempCompApi.h"
#include "../Ad/HystCompApi.h"
#include "../Ad/LinearCompApi.h"
#include "../Ad/CreepCompApi.h"
#include "../SystemTimer/TimerApi.h"
#include "CommDrv.h"
#include "CommApi.h"
#include "DebugApi.h"
#include "../Communication/PrtApi.h"
#include "../Application/SaleModeApp.h"

#include "../Hardware/HardwareConfig.h"
#ifdef USE_ED_COMM_API

/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */

/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief Weight Unit String Table 크기 */
#define COMM_WEIGHT_STR_TABLE_SIZE	4
/** @brief Comm Speed Default */
#define BAUD_RATE_DEFAULT BAUD_RATE_9600

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
/** @brief Communication Driver Rx Ring Buffer를 받아오기 위한 Pointer */
static RING_BUF	*commRxRingBufPtr;
/** @brief Communication Driver Tx Ring Buffer를 받아오기 위한 Pointer */
static RING_BUF	*commTxRingBufPtr;
/** @brief Communication Interpreter의 상태 변수 */
static INT8U commState;

static INT8U tareClrChkFlag;

/** @brief Weight Unit String Table */
static const char code commWeightUnitStrTable[COMM_WEIGHT_STR_TABLE_SIZE][2] =
{
	{'k','g'},	{'l', 'b'},	{'o', 'z'},	{' ', 'g'}
};

/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
static void commErInterpreter(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
static void commErSendWeight(RING_BUF *txRingBufPtr);
#ifdef USE_TURKEY_POS_PROTOCOL
static void commSendCas22Byte(RING_BUF *txRingBufPtr);
static void commSendGood(RING_BUF *txRingBufPtr, INT8U resVal);
#endif
/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
/**
********************************************************************************
* @brief    Communication API Parameter를 default 값으로 초기화\n
* @param    type : defualt type
* @return   none
* @remark   Parameter 영역에 초기값을 넣는다.\n
*           Cal Mode나 Program 모드에서 전체 초기화할 때 실행한다.
********************************************************************************
*/
void CommSetDefaultParam(void)
{
	ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_BAUDRATE, BAUD_RATE_9600);
}
/**
********************************************************************************
* @brief    BT module리셋 및 모드변경 함수\n
* @param    none
* @return   none
* @remark   user set mode에서 설정하면 실행.\n
********************************************************************************
*/

/**
********************************************************************************
* @brief    Communication API Init 함수\n
*           Serial Communication Speed Init\n
*           Communication Ring Buffer Init\n
* @param    none
* @return   none
* @remark   처음에 한번 실행 되는 함수\n
********************************************************************************
*/
void CommInit(void)
{
	INT8U tempInt8u;
		
	tempInt8u = ParamReadByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_BAUDRATE);

	CommDrvInit(tempInt8u);
		
	commRxRingBufPtr = &SerialRxRingBuf;
	commTxRingBufPtr = &SerialTxRingBuf;

	commState = 0;
	CommOmInit();
}

/**
********************************************************************************
* @brief    Communication Procedure 함수\n
*           Communication Driver Procedure 호출\n
*           Communication Driver Ring Buffer 확인 후 Protocol 처리
* @param    none
* @return   none
* @remark   일정 시간에 한번씩 계속 실행 되는 함수\n
*           현재 Protocol 처리 부분 없음 추가 필요 (TO DO)
********************************************************************************
*/
void CommProc(void)
{
	while (RingBufCheckBuf(commRxRingBufPtr))
	{
		commErInterpreter(commRxRingBufPtr, commTxRingBufPtr);
	}
	CommDrvProc();
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
/**
********************************************************************************
* @brief    ER Send Weight 함수\n
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   STX STA SIGN W5 W4 W3 W2 W1 W0 UN1 UN2 BCC ETX
*           STA -> stable('S'), unstable('U')
*           SIGN -> Zero or Positive(' '), Negative('-'), Overload('F')
*           W5 ~ W0 -> Weight Data "  1000", if overload, "FFFFFF"
*           UN1 ~ UN2 -> Unit of Weight, if kg, UN1='k', UN2='g'
*           BCC -> Block check character exclusive or(^)
********************************************************************************
*/
void commErSendWeight(RING_BUF *txRingBufPtr)
{
	char strBuf[7];
	char sendByte;
	INT8U bcc;
	INT8U i;

	RingBufPutChar(txRingBufPtr, STX);

	bcc = 0;
	if (AdData.stableFlag)
	{
		sendByte = 'S';
	}
	else
	{
		sendByte = 'U';
	}
	bcc ^= sendByte;
	RingBufPutChar(txRingBufPtr, sendByte);
	if (AdData.overloadFlag)
	{
		for (i = 0; i < 7; i++)
		{
			strBuf[i] = 'F';
			bcc ^= 'F';
		}
		RingBufPutData(txRingBufPtr, strBuf, 7);
	}
	else
	{
		if (SaleData.weight < 0)
		{
			sendByte = '-';
		}
		else
		{
			sendByte = ' ';
		}
		bcc ^= sendByte;
		RingBufPutChar(txRingBufPtr, sendByte);

		Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), 6, ' ', AdData.weightDecPos, '.', 0);
		for (i = 0; i < 6; i++)
		{
			bcc ^= strBuf[i];
		}
		RingBufPutData(txRingBufPtr, strBuf, 6);
	}

	sendByte = commWeightUnitStrTable[AdData.weightUnit][0];
	bcc ^= sendByte;
	RingBufPutChar(txRingBufPtr, sendByte);
	sendByte = commWeightUnitStrTable[AdData.weightUnit][1];
	bcc ^= sendByte;
	RingBufPutChar(txRingBufPtr, sendByte);

	RingBufPutChar(txRingBufPtr, bcc);
	RingBufPutChar(txRingBufPtr, ETX);
}
/**
********************************************************************************
* @brief    ID-U Send Weight 함수\n
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   W5 W4 W3 W2 W1 W0
*           W5 ~ W0 -> Weight Data "  10.25"

********************************************************************************
*/
void commIDSendWeight(void)
{
#ifndef USE_NOT_UNDERLOAD_ERROR
	if(AdData.underloadFlag) return;
#endif
	PrtPutNum(SaleData.weight, 7, SaleData.weightDecPos, 0, 0);
	PrtPutCRLF(1);
}

/**
********************************************************************************
* @brief    ER communication Interpreter 함수\n
* @param    rxRingBufPtr : 받은 데이터가 있는 Ring Buffer Pointer
*           txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   Protocol은 다음과 같다.
*           1> Scale <-- ENQ(0x05) --  PC
*           2> Scale  -- ACK(0x06) --> PC
*           3> Scale <-- Command   --  PC
*                        DC1(0x11):Request weight data
*                        DC2(0x12):Request all data (weight,price,unit)
*                        DC3(0x13):Request all data + PLU no (TODO : insert func)
*           4> Scale  -- data blocks --> PC
*           "data blocks"는 다음과 같다.
*           DC1에 대한 Data Block : SOH STX STA SIGN W5 W4 W3 W2 W1 W0 UN1 UN2 BCC ETX EOT
*           DC2에 대한 Data Block : SOH STX P7 P6 P5 P4 P3 P2 P1 P0 BCC ETX
*                                    STX STA SIGN W5 W4 W3 W2 W1 W0 UN1 UN2 BCC ETX
*                                    STX U7 U6 U5 U4 U3 U2 U1 U0 BCC ETX EOT
*           DC3에 대한 Data Block : ? (TODO)
********************************************************************************
*/
void commErInterpreter(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr)
{
	char byte;
	INT8U tareClrFlag;

	if (RingBufCheckBuf(rxRingBufPtr))
	{
		byte = RingBufGetChar(rxRingBufPtr);
		switch (commState)
		{
			case 0:
				if (byte == ENQ)
				{
					RingBufPutChar(txRingBufPtr, ACK);
					commState++;
				}
				else if (byte == ESC) //for Factory
				{
					commState = 200;
				}
				else if (byte == 's')
				{
					RingBufPutChar(txRingBufPtr, 's');
					commState = 100;
				}
#ifdef USE_TURKEY_POS_PROTOCOL
				else if(byte == 'W')
				{
					commState=10;
				}
				else if(byte == 'T')
				{
					commState=11;
				}
				else if(byte == 'S')
				{
					commState=12;
				}
				else if(byte == 'C')
				{
					commState=13;
				}
				else
				{
					commSendGood(txRingBufPtr, NG);	// 0: Not Good('NG'), 1: Good('OK')
					commState=0;
				}
#endif
				break;
			case 1:
				if(byte == DC1)	//weight data
				{
					RingBufPutChar(txRingBufPtr, SOH);
					commErSendWeight(txRingBufPtr);
					RingBufPutChar(txRingBufPtr, EOT);
				}
				commState=0;
				break;
#ifdef USE_TURKEY_POS_PROTOCOL
			case 10:
				if(byte == 0x0d)
				{
					commState=20;
				}
				else
				{
					commSendGood(txRingBufPtr, NG);
					commState=0;
				}
				break;
			case 11:
				if(byte == 0x0d)
				{
					commState=21;
				}
				else
				{
					commSendGood(txRingBufPtr, NG);
					commState=0;
				}
				break;
			case 12:
				if(byte == 0x0d)
				{
					commState=22;
				}
				else
				{
					commSendGood(txRingBufPtr, NG);
					commState=0;
				}
				break;
			case 13:
				if(byte == 0x0d)
				{
					commState=23;
				}
				else
				{
					commSendGood(txRingBufPtr, NG);
					commState=0;
				}
				break;
			case 20:
				if(byte == LF)
				{
					commSendCas22Byte(txRingBufPtr);
				}
				else
				{
					commSendGood(txRingBufPtr, NG);
				}
				commState=0;
				break;
			case 21:
				if(byte == LF)
				{
					SaleTareExtKeyProc();
					if(saleKeyCommErrFlagState() == 1)
					{
						commSendGood(txRingBufPtr, OK);
					}
					else
					{
						commSendGood(txRingBufPtr, NG);
					}
				}
				else
				{
					commSendGood(txRingBufPtr, NG);
				}
				commState=0;
				break;
			case 22:	// Tare save command
				if(byte == LF)
				{
					SaleTareSaveProc(0);	// 0: tare save, 1: tare clear
					
					if(saleKeyCommErrFlagState() == 1)
					{
						commSendGood(txRingBufPtr, OK);
					}
					else
					{
						commSendGood(txRingBufPtr, NG);
					}
				}
				else
				{
					commSendGood(txRingBufPtr, NG);
				}
				commState=0;
				break;
			case 23:	// Tare clear command
				if(byte == LF)
				{
					SaleTareSaveProc(1);		// 0: tare save, 1: tare clear
					
					if(saleKeyCommErrFlagState() == 1)
					{
						commSendGood(txRingBufPtr, OK);
					}
					else
					{
						commSendGood(txRingBufPtr, NG);
					}
				}
				else
				{
					commSendGood(txRingBufPtr, NG);
				}
				commState=0;
				break;
#endif
			case 200:
				if (!CommOmInterpreter(commTxRingBufPtr, byte)) 
				{
					commState = 0;
				}				
				break;
				
			case 100:
				switch (byte)
				{
					case 'w'://weight data
						commErSendWeight(txRingBufPtr);
						break;
						
					case 'p':
						//commErSendTotalPrice(txRingBufPtr);
						commErSendWeight(txRingBufPtr);
						//commErSendUnitPrice(txRingBufPtr);
						break;
				}
				commState = 0;
				break;
				
			default:
				commState = 0;
				break;
		}
	}
}
#ifdef USE_TURKEY_POS_PROTOCOL
/**
********************************************************************************
* @brief    Error Messege 전송 함수\n
* @param   none
* @return   none
* @remark   None Error 경우 "OK" 전송
			Error 경우 "NG" 전송
********************************************************************************
*/
void commSendGood(RING_BUF *txRingBufPtr, INT8U resVal)
{
	if(resVal == 1)
	{
		RingBufPutChar(txRingBufPtr, 'O');
		RingBufPutChar(txRingBufPtr, 'K');
	}
	else
	{
		RingBufPutChar(txRingBufPtr, 'N');
		RingBufPutChar(txRingBufPtr, 'G'); 
	}
	RingBufPutChar(txRingBufPtr, 0x0D);
	RingBufPutChar(txRingBufPtr, LF);
}

/**
********************************************************************************
* @brief    CAS 22 Byte  함수\n
* @param   none
* @return   none
* @remark   STB , NTB , DIB LSB , WDB   UNB CR LF 
*                 STB -> stable('ST'), unstable('US'), overload('OL')
*                 NTB -> gross('GS'), net('NT')
*                 DIB -> device id('0')
*                 LSB -> 8bit(7:1, 6:stable, 5:1, 4:hold, 3:print, 2:gross, 1:net, 0:zero) 
*                 WDB -> weight data(8byte)
*                 BLANK -> '  '
*                 UNB -> kilo gram('KG'), pound('LB'), once('OZ')
********************************************************************************
*/
void commSendCas22Byte(RING_BUF *txRingBufPtr)
{
	char strBuf[22];
	char sendByte=0xa8;	// Bit3,5,7: 1(Fixed)
	INT8U i;

	Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight),6, ' ', AdData.weightDecPos, '.', 0);
	if(SaleData.saleState == SALE_STATE_HOLD)
	{
		Int32sToDecStr(strBuf, AbsInt32s(SaleData.holdWeight),6, ' ', AdData.weightDecPos, '.', 0);
	}
	for(i=0;i<6;i++)
	{
		strBuf[i+11] = strBuf[i];
	}
	if(AdData.stableFlag)
	{
		strBuf[0] = 'S';
		strBuf[1] = 'T';
		sendByte = sendByte | 0x40;	// Stable Status
	}
	else if(AdData.overloadFlag)
	{
		strBuf[0] = 'O';
		strBuf[1] = 'L';
		sendByte = sendByte | 0x00;
	}
	else
	{
		strBuf[0] = 'U';
		strBuf[1] = 'S';
		sendByte = sendByte | 0x00;
	}

	if (AdData.tareWeight)// && !sendStatus )
	{
		strBuf[3] = 'N';	 
		strBuf[4] = 'T';
		sendByte = sendByte | 0x02;	// Tare Status
	}
	else
	{
		strBuf[3] = 'G';	 
		strBuf[4] = 'S';
		sendByte = sendByte | 0x04;	// Gross Status
	}
	if(SaleData.saleState == SALE_STATE_HOLD)
		sendByte = sendByte | 0x10;	// Hold Status
	else		
		sendByte = sendByte | 0x00;
	if (AdData.grossZeroFlag)
		sendByte = sendByte | 0x01;	// Zero Status
	else
		sendByte = sendByte | 0x00;

	if (SaleData.weight < 0)
		strBuf[9] = 0x2D;
	else
		strBuf[9] = 0x20;

	strBuf[2] = ',';
	strBuf[5] = ',';
	strBuf[6] = 0x30;
	strBuf[7] = sendByte;
	strBuf[8] = ',';
	strBuf[10] = ' ';
	strBuf[17] = ' ';
	strBuf[18] = 'k';
	strBuf[19] = 'g';
	strBuf[20] = 0x0d;
	strBuf[21] = 0x0a;

	RingBufPutData(txRingBufPtr, strBuf, 22);
}
#endif	//USE_TURKEY_POS_PROTOCOL

#endif

