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

#include "../Hardware/HardwareConfig.h"	 
#include "../Application/ScaleModeApp.h"	//SaleModeApp.h 와 함께 Include	적용
#include "../Application/SaleModeApp.h" 
#ifdef USE_DP_COMM_API

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
static BOOLEAN commKeyCmdErrFlag;

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
static void commTrSendWeight(RING_BUF *txRingBufPtr);

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
#ifdef USE_CONTI_SEND_WEIGHT_DATA
	static INT16U prevSendWeightTimer100ms = 0;
	INT16U currTimer100ms;
#endif
//	if (RingBufCheckBuf(commRxRingBufPtr))
	while (RingBufCheckBuf(commRxRingBufPtr))
	{
		commErInterpreter(commRxRingBufPtr, commTxRingBufPtr);
	}
	CommDrvProc();
#ifdef USE_CONTI_SEND_WEIGHT_DATA
	currTimer100ms = TimerGetSysTimer100ms();

	if ((INT16U)(currTimer100ms - prevSendWeightTimer100ms) > 2)
	{
		prevSendWeightTimer100ms = currTimer100ms;
		
		
		//RingBufPutChar(commTxRingBufPtr, SOH);
		commTrSendWeight(commTxRingBufPtr);
		//RingBufPutChar(commTxRingBufPtr, EOT);
		RingBufPutChar(commTxRingBufPtr, 0x0D);
		RingBufPutChar(commTxRingBufPtr, LF);
	}
#endif
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
#ifdef USE_EMS_PROTOCOL
/**
********************************************************************************
* @brief    DB-2VFD Send Weight 함수 (중국 EMS) \n
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   0x61('a') -> W5 W4 W3 W2 W1 W0 NULL	//dec point is none
********************************************************************************
*/
void commEMSSendWeight(RING_BUF *txRingBufPtr)
{
	char strBuf[10];
	char sendByte;
	INT8U bcc;
	INT8U i;

	if (!AdData.overloadFlag)
	{
		Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), 5, ' ', 0, '.', 0);
		for (i = 0; i < 6; i++)
		{
			if(strBuf[i] == 0x20)
			{
				strBuf[i] = 0x30;
			}
		}
		RingBufPutData(txRingBufPtr, strBuf, 5);
//		RingBufPutChar(txRingBufPtr, 0x00);
	}
}
#endif

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
void commTrSendWeight(RING_BUF *txRingBufPtr)
{
	char strBuf[10];
	char sendByte;
	INT8U i;

	if (AdData.stableFlag)
	{
		sendByte = 'S';
	}
	else
	{
		sendByte = 'U';
	}
	RingBufPutChar(txRingBufPtr, sendByte);
	
	if (AdData.overloadFlag) // TODO : overload
	{
		for (i = 0; i < 7; i++)
		{
			strBuf[i] = 'F';
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
		RingBufPutChar(txRingBufPtr, sendByte);

		Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), 6, ' ', AdData.weightDecPos, '.', 0);
		RingBufPutData(txRingBufPtr, strBuf, 6);
	}

	sendByte = commWeightUnitStrTable[AdData.weightUnit][0];
	RingBufPutChar(txRingBufPtr, sendByte);

	sendByte = commWeightUnitStrTable[AdData.weightUnit][1];
	RingBufPutChar(txRingBufPtr, sendByte);

}
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
	char strBuf[10];
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
	if (AdData.overloadFlag) // TODO : overload
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

		//Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), 6, ' ', AdData.weightDecPos, SaleData.weightDotChar, 0);
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
* @brief    ER Send Total Price 함수\n
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   STX P7 P6 P5 P4 P3 P2 P1 P0 BCC ETX
*           P7 ~ P0 -> Total Price "   10000", if overload, "FFFFFFFF"
*           BCC -> Block check character exclusive or(^)
********************************************************************************
*/
void commErSendTotalPrice(RING_BUF *txRingBufPtr)
{
	char strBuf[10];
	INT8U bcc;
	INT8U i;

	bcc = 0;
	RingBufPutChar(txRingBufPtr, STX);

	//Int32sToDecStr(strBuf, SaleData.totalPrice , 7, ' ', SaleData.priceDecPos, SaleData.priceDotChar, 0);
	Int32sToDecStr(strBuf, SaleData.totalPrice , 8, ' ', SaleData.priceDecPos, SaleData.priceDotChar, 0);
	//for (i = 0; i < 7; i++)
	for (i = 0; i < 8; i++)
	{
		bcc ^= strBuf[i];
	}
	RingBufPutStr(txRingBufPtr, strBuf);
	RingBufPutChar(txRingBufPtr, bcc);

	RingBufPutChar(txRingBufPtr, ETX);
}

/**
********************************************************************************
* @brief    ER Send Unit Price 함수\n
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   STX U7 U6 U5 U4 U3 U2 U1 U0 BCC ETX
*           U7 ~ U0 -> Unit Price "   10000", if overload, "FFFFFFFF"
*           BCC -> Block check character exclusive or(^)
********************************************************************************
*/
void commErSendUnitPrice(RING_BUF *txRingBufPtr)
{
	char strBuf[10];
	INT8U bcc;
	INT8U i;

	bcc = 0;

	RingBufPutChar(txRingBufPtr, STX);
	//Int32sToDecStr(strBuf, SaleData.unitPrice , 7, ' ', SaleData.priceDecPos, SaleData.priceDotChar, 0);
	Int32sToDecStr(strBuf, SaleData.unitPrice , 8, ' ', SaleData.priceDecPos, SaleData.priceDotChar, 0);
	//for (i = 0; i < 7; i++)
	for (i = 0; i < 8; i++)
	{
		bcc ^= strBuf[i];
	}
	RingBufPutStr(txRingBufPtr, strBuf);
	RingBufPutChar(txRingBufPtr, bcc);

	RingBufPutChar(txRingBufPtr, ETX);
}

/**
********************************************************************************
* @brief    ER Send Unit Price 함수\n
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   STX U7 U6 U5 U4 U3 U2 U1 U0 BCC ETX
*           U7 ~ U0 -> Unit Price "   10000", if overload, "FFFFFFFF"
*           BCC -> Block check character exclusive or(^)
********************************************************************************
*/
void commErSendPluNumber(RING_BUF *txRingBufPtr)
{
	char strBuf[10];
	INT8U bcc;
	INT8U i;

	bcc = 0;

	RingBufPutChar(txRingBufPtr, STX);
	Int32sToDecStr(strBuf, SaleData.pluNum, 5, ' ', 0, 0, 0);
	for (i = 0; i < 5; i++)
	{
		bcc ^= strBuf[i];
	}
	RingBufPutStr(txRingBufPtr, strBuf);
	RingBufPutChar(txRingBufPtr, bcc);

	RingBufPutChar(txRingBufPtr, ETX);
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
	char state, strBuf[2];

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
				else if (byte == 'Z') //0x5A
				{
					state = 'Z';
					commState = 50;
				}
				else if (byte == 'T') //0x54
				{
					state = 'T';
					commState = 50;
				}
				else if (byte == 'H') //0x48
				{
					state = 'H';
					commState = 50;
				}
				else
				{
					strBuf[0] = 'E';
					strBuf[1] = 'R';
					RingBufPutData(txRingBufPtr, strBuf, 2);
				}
				break;
			case 50:
				if (state == 'Z' && byte == 'E') //0x5A + 0x45
				{
					SaleZeroExtKeyProc();
					commState = 0;	
				}
				else if (state == 'T' && byte == 'R') //0x54 + 0x52
				{
					SaleTareExtKeyProc();
					commState = 0;
				}
				else if (state == 'H' && byte == 'D') //0x48 + 0x44
				{
					SaleHoldExtKeyProc();
					commState = 0;
				}
				else
				{
					strBuf[0] = 'E';
					strBuf[1] = 'R';
					RingBufPutData(txRingBufPtr, strBuf, 2);
					commState = 0;
					break;
				}
				if(commKeyCmdErrFlag == 0)
				{ 					
					strBuf[0] = 'O';
					strBuf[1] = 'K';
					RingBufPutData(txRingBufPtr, strBuf, 2);
				}
				if(commKeyCmdErrFlag == 1)
				{ 					
					strBuf[0] = 'N';
					strBuf[1] = 'G';
					RingBufPutData(txRingBufPtr, strBuf, 2);
				}
				break;
				
			case 1:
				switch (byte)
				{
					case DC1://weight data
						RingBufPutChar(txRingBufPtr, SOH);
						commErSendWeight(txRingBufPtr);
						RingBufPutChar(txRingBufPtr, EOT);
						break;
					case DC2:
						RingBufPutChar(txRingBufPtr, SOH);
						commErSendTotalPrice(txRingBufPtr);
						commErSendWeight(txRingBufPtr);
						commErSendUnitPrice(txRingBufPtr);
						RingBufPutChar(txRingBufPtr, EOT);
						break;
					case DC3:	//for greek, DC2 + PLU Number
						RingBufPutChar(txRingBufPtr, SOH);
						commErSendTotalPrice(txRingBufPtr);
						commErSendWeight(txRingBufPtr);
						commErSendUnitPrice(txRingBufPtr);
						commErSendPluNumber(txRingBufPtr);
						RingBufPutChar(txRingBufPtr, EOT);
						break;
				}				
				commState = 0;	
				break;

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
						commErSendTotalPrice(txRingBufPtr);
						commErSendWeight(txRingBufPtr);
						commErSendUnitPrice(txRingBufPtr);
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
/**
********************************************************************************
* @brief    Communication error check 함수\n
* @param    flag: Error ON/OFF flag
* @return   none
* @remark  commKeyCmdErrFlag (Key command error check flag)
*                commKeyCmdErrFlag = 1 -> Invalid condition          
*                commKeyCmdErrFlag = 0 -> Good condition 
********************************************************************************
*/
void CommSetKeyCmdErrFlag(BOOLEAN flag)
{
	if(flag == ON)
		commKeyCmdErrFlag = 1;
	else
		commKeyCmdErrFlag = 0;
}

#endif

