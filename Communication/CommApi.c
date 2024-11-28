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

#include "../Hardware/HardwareConfig.h"
#ifdef USE_STD_COMM_API

/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */

#ifdef USE_BT_CONNECT
/** @brief 현재 BT flag*/
INT8U BtResetFlag;
INT8U BtOnFlag;
INT8U BtSoftResetFlag;
INT8U BtEnableFlag;
#endif
#ifdef USE_CONTI_SEND_WEIGHT_DATA
INT8U sendWeightFlag;
#endif
#ifdef USE_ECR_TYPE_7
INT8U sendType7Flag;
#endif
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
#ifdef USE_UART1
/** @brief Communication Driver Rx Ring Buffer를 받아오기 위한 Pointer */
static RING_BUF	*commRx1RingBufPtr;
/** @brief Communication Driver Tx Ring Buffer를 받아오기 위한 Pointer */
static RING_BUF	*commTx1RingBufPtr;
#endif
/** @brief Communication Interpreter의 상태 변수 */
static INT8U commState;
#ifdef PC_TARE
static INT8U tare_count;
#endif

#ifdef USE_ECR_TYPE_7
/** @brief Parity Type*/
static INT8U parityTypeChk;
/** @brief ECR Number of weight digit*/
static INT8U commSendDigit;
static BOOLEAN commTransFlag;
#endif

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
#ifdef USE_CONTI_SEND_WEIGHT_DATA
static void commTrSendWeight(RING_BUF *txRingBufPtr);
#endif
#ifdef USE_SMART_BT_PROTOCOL
static void commBtInterpreter(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
static void commSmartBtSendData(RING_BUF *txRingBufPtr, INT8U *sendData, INT8U len);
static void commSmartBtSendScaleInformation(RING_BUF *txRingBufPtr);
static void commSmartBtSendWeight(RING_BUF *txRingBufPtr);
static void commSmartBtSendTare(RING_BUF *txRingBufPtr);
static void commSmartBtSendPowerInformation(RING_BUF *txRingBufPtr);
static void commSmrtBtSetPresetTare(RING_BUF *txRingBufPtr, INT8U *rcvData);
#endif
#ifdef USE_ECR_TYPE_7
static void commEcrSendWeight(RING_BUF *txRingBufPtr, INT8U weightUnit, INT8U parityType);
static void commEcrType7(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
static void commSendCharWithParity(INT8U sendData, INT8U parityType, RING_BUF *txRingBufPtr);
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
void CommSetDefaultParam()
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
#ifdef USE_BT_CONNECT
void Btreset(void)
{
	RingBufPutChar(commTxRingBufPtr, 'A');
	RingBufPutChar(commTxRingBufPtr, 'T');
	RingBufPutChar(commTxRingBufPtr, '&');
	RingBufPutChar(commTxRingBufPtr, 'F');
	RingBufPutChar(commTxRingBufPtr, 0x0d);
	BtResetFlag = 1;
	BtSoftResetFlag = 0;
	BtOnFlag = 0;
}

void BtSoftreset(void)
{
	RingBufPutChar(commTxRingBufPtr, 'A');
	RingBufPutChar(commTxRingBufPtr, 'T');
	RingBufPutChar(commTxRingBufPtr, 'Z');
	RingBufPutChar(commTxRingBufPtr, 0x0d);
	BtResetFlag = 0;
	BtSoftResetFlag = 1;
	BtOnFlag = 0;
}

void Btmodeconversion(void)
{
	RingBufPutChar(commTxRingBufPtr, 'A');
	RingBufPutChar(commTxRingBufPtr, 'T');
	RingBufPutChar(commTxRingBufPtr, '+');
	RingBufPutChar(commTxRingBufPtr, 'B');
	RingBufPutChar(commTxRingBufPtr, 'T');
	RingBufPutChar(commTxRingBufPtr, 'M');
	RingBufPutChar(commTxRingBufPtr, 'O');
	RingBufPutChar(commTxRingBufPtr, 'D');
	RingBufPutChar(commTxRingBufPtr, 'E');
	RingBufPutChar(commTxRingBufPtr, ',');
	RingBufPutChar(commTxRingBufPtr, '3');
	RingBufPutChar(commTxRingBufPtr, 0x0d);	
	BtResetFlag = 0;
	BtSoftResetFlag = 0;
	BtOnFlag = 1;
}
#endif
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
#ifdef USE_ECR_TYPE_7
	commTransFlag = 0;
	if(sendType7Flag)
	{
		parityTypeChk = PARITY_TYPE_NONE;
		commSendDigit = 6;
		tempInt8u = BAUD_RATE_9600;
	}
#endif
	CommDrvInit(tempInt8u);
		
	commRxRingBufPtr = &SerialRxRingBuf;
	commTxRingBufPtr = &SerialTxRingBuf;
#ifdef USE_UART1
	commRx1RingBufPtr = &SerialRx1RingBuf;
	commTx1RingBufPtr = &SerialTx1RingBuf;
#endif
	commState = 0;
	CommOmInit();
#ifdef USE_BT_CONNECT
	BtResetFlag = 0;
	BtOnFlag = 0;
	BtSoftResetFlag = 0;
	BtEnableFlag = 0;
#endif
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
#ifdef USE_LONG_KEY_FOR_CHINAMARKET
extern	BOOLEAN 	saleSend_CHNdataFlag;
extern	INT8U		saleSend_CHNdatatype;
#endif

void CommProc(void)
{
#ifdef USE_CONTI_SEND_WEIGHT_DATA 
	static INT16U prevSendWeightTimer100ms = 0;
	INT16U currTimer100ms;
#elif defined (USE_SEND_WEIGHT_DATA)
	static INT16U prevSendWeightTimer100ms = 0;
	static INT8U stablestate = 0;	
	INT16U currTimer100ms;
	char strBuf[10];
#endif
	while (RingBufCheckBuf(commRxRingBufPtr))
	{
		commErInterpreter(commRxRingBufPtr, commTxRingBufPtr);
	}
	CommDrvProc();
#ifdef USE_UART1
	while (RingBufCheckBuf(commRx1RingBufPtr))
	{
		commBtInterpreter(commRx1RingBufPtr, commTx1RingBufPtr);
	}
	Comm1DrvProc();
#endif
#ifdef USE_CONTI_SEND_WEIGHT_DATA
  #if MODEL_NUM == MODEL_OMV4_ER2_LCD	
	if(sendWeightFlag)	//USE_CONTI_SEND_WEIGHT_DATA
  #endif
  	{
		currTimer100ms = TimerGetSysTimer100ms();

		if ((INT16U)(currTimer100ms - prevSendWeightTimer100ms) > 2)
		{
			prevSendWeightTimer100ms = currTimer100ms;
			
			commTrSendWeight(commTxRingBufPtr);
			RingBufPutChar(commTxRingBufPtr, 0x0D);
			RingBufPutChar(commTxRingBufPtr, LF);
		}
  	}
#elif defined (USE_SEND_WEIGHT_DATA)
	if(saleSend_CHNdataFlag && saleSend_CHNdatatype!=0){
		currTimer100ms = TimerGetSysTimer100ms();
		if(!AdData.stableFlag)stablestate =0;
		
		if ((INT16U)(currTimer100ms - prevSendWeightTimer100ms) > 2)
		{
			prevSendWeightTimer100ms = currTimer100ms;
			if(saleSend_CHNdatatype == 2 && stablestate == 0 && AdData.stableFlag || saleSend_CHNdatatype == 1)
			{
				stablestate = 1;
				// Prefix				
				RingBufPutChar(commTxRingBufPtr, LF);				
				RingBufPutChar(commTxRingBufPtr, 0x0D);
				// Weight
				Int32sToDecStr(strBuf, SaleData.weight, 6, ' ', AdData.weightDecPos, '.', 0);
				RingBufPutData(commTxRingBufPtr,strBuf, 6);
				// Seperator
				RingBufPutChar(commTxRingBufPtr, 0x20);
				RingBufPutChar(commTxRingBufPtr, 0x20);			
				// Unit Price
				Int32sToDecStr(strBuf, SaleData.unitPrice, 6, ' ', SaleData.priceDecPos, '.', 0);
				RingBufPutData(commTxRingBufPtr,strBuf, 6);
				// Seperator
				RingBufPutChar(commTxRingBufPtr, 0x20);
				RingBufPutChar(commTxRingBufPtr, 0x20);			
				// Total Price
				Int32sToDecStr(strBuf, SaleData.totalPrice, 7, ' ', SaleData.priceDecPos, '.', 0);
				RingBufPutData(commTxRingBufPtr,strBuf, 7);
			}
		}
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
* @remark   0x61('a') -> W5 W4 W3 W2 W1 W0	//dec point is none
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
	}
}
#endif

#ifdef USE_CONTI_SEND_WEIGHT_DATA
/**
********************************************************************************
* @brief    ER TR Send Weight 함수\n
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   STA SIGN W5 W4 W3 W2 W1 W0 UN1 UN2 
*           STA -> stable('S'), unstable('U')
*           SIGN -> Zero or Positive(' '), Negative('-'), Overload('F')
*           W5 ~ W0 -> Weight Data "  1000", if overload, "FFFFFF"
*           UN1 ~ UN2 -> Unit of Weight, if kg, UN1='k', UN2='g'
********************************************************************************
*/
void commTrSendWeight(RING_BUF *txRingBufPtr)
{
	char strBuf[8];
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
	
	if (AdData.overloadFlag)
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

	Int32sToDecStr(strBuf, SaleData.totalPrice , 8, ' ', SaleData.priceDecPos, SaleData.priceDotChar, 0);
	for (i = 0; i < 8; i++)
	{
		bcc ^= strBuf[i];
	}
	RingBufPutStr(txRingBufPtr, strBuf);
	RingBufPutChar(txRingBufPtr, bcc);

	RingBufPutChar(txRingBufPtr, ETX);
}

#ifdef USE_ECR_TYPE_7
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
void commEcrSendWeight(RING_BUF *txRingBufPtr, INT8U weightUnit,  INT8U parityType)
{
	char strBuf[10];
	INT8U i;
	
	if(sendType7Flag)	//type 7
	{
		parityTypeChk = PARITY_TYPE_NONE;
		commSendDigit = 6;
		
		if(SaleData.weight < 0 || AdData.overloadFlag || !AdData.stableFlag) MakeStrCopy(strBuf, "------", commSendDigit);
		else Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', AdData.weightDecPos, '.', 0);

		//Int32sToDecStr(strBuf, SaleData.weight, 6, ' ', AdData.weightDecPos, '.', 0);
		//RingBufPutData(commTxRingBufPtr,strBuf, 6);
		
		for (i = 0; i < commSendDigit; i++)
		{
			commSendCharWithParity(strBuf[i], parityTypeChk, txRingBufPtr);
		}
	}
}
#endif

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
	Int32sToDecStr(strBuf, SaleData.unitPrice , 8, ' ', SaleData.priceDecPos, SaleData.priceDotChar, 0);
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

#ifdef USE_ECR_TYPE_7
/**
********************************************************************************
* @brief    ER communication Interpreter 함수\n
* @param    rxRingBufPtr : 받은 데이터가 있는 Ring Buffer Pointer
*           txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   Protocol은 다음과 같다.
*           1> Scale <-- $(0x24) --  PC
*           2> Scale  -- data blocks --> PC
*           "data blocks"는 다음과 같다.
*           Data Block : W4 W3 2E W2 W1 W0 CR
********************************************************************************
*/
  #if !defined (USE_SLOVAKIA_PROTOCOL) && !defined (COUNTRY_CZECH)
void commEcrType7(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr)
{
	char byte;

	if (RingBufCheckBuf(rxRingBufPtr))
	{
		byte = RingBufGetChar(rxRingBufPtr);
		switch (commState)
		{
			case 0:
				if (byte == '$')
				{
					commTransFlag = 1; 			
				}
				else if (byte == ESC) //for Factory
				{
					commState = 200;
				}
				break;
			case 200:
				if (!CommOmInterpreter(commTxRingBufPtr, byte)) 
				{
					commState = 0;
				}
				break;
				
			default:
				commState = 0;
				break;
		}
	}
	//if (commTransFlag && AdData.stableFlag && SaleData.weight >= 0 && !AdData.overloadFlag)
	if (commTransFlag)
	{	
		commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk);
		commSendCharWithParity(0x0d, parityTypeChk, txRingBufPtr);
		commTransFlag = 0; 
	}
}
  #endif
#endif

#ifdef USE_ECR_TYPE_7
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
void commSendCharWithParity(INT8U sendData, INT8U parityType, RING_BUF *txRingBufPtr)
{
	INT8U i, numOfOne, parity;
	
	numOfOne = 0;

	if(parityType != PARITY_TYPE_NONE)
	{
		for (i = 0; i < 7; i++)
		{
			if((sendData >> i) & 0x01) numOfOne++;
		}
		numOfOne %= 2;
		if(parityType == numOfOne) parity = 0x00;	//parity = 0;
		else parity = 0x80;	//parity = 1;
		
		sendData |= parity;
	}

	RingBufPutChar(txRingBufPtr, sendData);
	return;
}
#endif

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
#ifdef PC_TARE
	static INT8U tare_data[4];
	static INT32U send_tare_data;
#endif
#ifdef USE_SMART_BT_PROTOCOL
	static INT8U btCmd[10];
	static INT8U btLen;
	static INT8U btIdx;
	static INT8U btBcc;
	static INT8U btNak;
	
	btNak = NAK;
#endif

#ifdef USE_ECR_TYPE_7
	if (RingBufCheckBuf(rxRingBufPtr) || (commTransFlag && AdData.stableFlag))
	{
		if(sendType7Flag)
			commEcrType7(rxRingBufPtr, txRingBufPtr);
	}
#endif

	if (RingBufCheckBuf(rxRingBufPtr))
	{
		byte = RingBufGetChar(rxRingBufPtr);
		switch (commState)
		{
			case 0:
#ifdef USE_PRII_SPAIN_$_PROTOCOL
				if (byte == '$') {
					comm_$_protocol_send_weight(txRingBufPtr);
				}
#else
#ifdef USE_BT_CONNECT
				if(!AdCheckInitZero()) break;
				
				if(BtResetFlag || BtSoftResetFlag || BtOnFlag)
				{
					if(byte == 0x0d)
					{
						commState = 20;
						break;
					}
				}
				if(byte == 0x00)	//Device Number Check (0: Fix)
				{
					commState++;
					break;
				}
#else
				if (byte == ENQ)
				{
	#ifdef USE_DS252_PROTOCOL
					commIDSendWeight();
					break;
	#else 
					RingBufPutChar(txRingBufPtr, ACK);
					commState++;
	#endif
				}
#endif	//USE_BT_CONNECT
				else if (byte == ESC) //for Factory
				{
					commState = 200;
				}
#ifdef USE_EMS_PROTOCOL
				else if (byte == 'a') //0x61
				{
					commEMSSendWeight(txRingBufPtr);
				}
#endif
#ifdef USE_COMMAND_TARE_KEY
				else if ((byte == 't')||(byte == 'T')) //0x74
				{
					SaleTareExtKey();
					RunSaleWeightTareProc();
					commState=0;
				}
#endif
				else if (byte == 's')
				{
					RingBufPutChar(txRingBufPtr, 's');
					commState = 100;
				}
				
#ifdef USE_SMART_BT_PROTOCOL
				else if (byte == STX)
				{
					commState = 110;
				}
#endif
#ifdef USE_ADD_KEY_FOR_CERTIFICATION
				if(SaleData.addState == 2 || SaleData.addState == 4)
				{
					if(byte == 0x00)
					{
						if(SaleData.addState == 2) SaleData.printerState = 2;
						else SaleData.printerState = 4;
					}
					else if(byte == 0x04)
					{
						SaleData.printerState = 3;
					}
					else
					{
						SaleData.printerState = 0;
					}
				}
#endif
#endif /* USE_PRII_SPAIN_$_PROTOCOL */
			break;
				
			case 1:
#ifdef USE_COMMAND_KEY
				if(byte==0x5A)	//'Z'
				{
					commState=2;
					break;
				}
				else if(byte==0x54)	//'T'
				{
					commState=3;
					break;
				}
				else if(byte==0x48)	//'H'
				{
					commState=4;
					break;
				}
				else if((byte != DC1) && (byte != DC2) && (byte != DC3))
				{
					RingBufPutChar(txRingBufPtr, NAK);
					commState=0;
					break;
				}
#endif
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
#ifdef PC_TARE
					case STX: //insert for test
					commState = 10;
					tare_count =0;
					break;
				}
#else
				}		
#endif	//PC_TARE
#ifdef USE_COMMAND_KEY
				if(commState < 2 || commState > 4)
#else
 #ifdef PC_TARE
				if(commState != 10)
 #endif
#endif
				commState = 0;
				break;
#ifdef USE_COMMAND_KEY
			case 2:
				if(byte==0x45) //'E'
				{
					SaleZeroExtKeyProc();
					if(saleKeyCommErrFlagState() == 1)
					{
						RingBufPutChar(txRingBufPtr, 'O');
						RingBufPutChar(txRingBufPtr, 'K');
					}
					else
					{
						RingBufPutChar(txRingBufPtr, 'N');
						RingBufPutChar(txRingBufPtr, 'G');
					}
					commState = 0;
				}
				else
				{
					RingBufPutChar(txRingBufPtr, NAK);
					commState = 0;
				}
				break;
			case 3: 
				if(byte==0x52) //'R'
				{
					SaleTareExtKeyProc();
					if(saleKeyCommErrFlagState() == 1)
					{
						RingBufPutChar(txRingBufPtr, 'O');
						RingBufPutChar(txRingBufPtr, 'K');
					}
					else
					{
						RingBufPutChar(txRingBufPtr, 'N');
						RingBufPutChar(txRingBufPtr, 'G');
					}
					commState = 0;
				}
				else
				{
					RingBufPutChar(txRingBufPtr, NAK);
					commState = 0;
				}
				break;
			case 4:
				if(byte==0x44) //'D'
				{
					SaleHoldExtKeyProc();
					if(saleKeyCommErrFlagState() == 1)
					{
						RingBufPutChar(txRingBufPtr, 'O');
						RingBufPutChar(txRingBufPtr, 'K');
					}
					else
					{
						RingBufPutChar(txRingBufPtr, 'N');
						RingBufPutChar(txRingBufPtr, 'G');
					}
					commState = 0;
				}
				else
				{
					RingBufPutChar(txRingBufPtr, NAK);
					commState = 0;
				}
				break;
#endif				

#ifdef PC_TARE //insert for test
			case 10: //insert for test

				tare_data[tare_count] = byte;
				
				if(tare_count == 3)
				{	
					 commState = 11;
				}
				else
				{
					commState = 10;
					tare_count++;
				}
				break;

			case 11:
				if(byte == ETX)
				{
					commState = 0;
					send_tare_data =0;
					for (tare_count = 0; tare_count <4 ; tare_count++)
					{
						send_tare_data += (INT32U)(tare_data[tare_count]-0x30) * DecPowerConvTable[3 - tare_count];
					}
					if(AdData.stableFlag)
					{
						RingBufPutChar(txRingBufPtr, ACK);
						AdSetPT(send_tare_data, 1);
					}

				}

				else
				{
					RingBufPutChar(txRingBufPtr, NAK);
					commState = 0;
				}
				break;
#endif	
#ifdef USE_BT_CONNECT
			case 20:
				if(byte == LF)
				{
					commState++;
				}
			case 21:
				if(byte == 'O')
				{
					commState++;
				}
			case 22:
				if(byte == 'K')
				{
					commState++;
				}
			case 23:
				if(byte == 0x0d)
				{
					commState++;
				}
			case 24:
				if(byte == 0x0a)
				{										
					if(BtResetFlag && !BtOnFlag && !BtSoftResetFlag)
					{
						commState++;
					}
					else if(BtOnFlag && !BtResetFlag && !BtSoftResetFlag)
					{
						commState = 26;
					}
					else	// BtSoftResetFlag = 1
					{
						BtSoftResetFlag = 0;
						commState = 0;
						break;
					}
				}
			case 25:
				if(BtResetFlag)
				{
					Btmodeconversion();
					commState = 0;
				}
				break;							
			case 26:
				if(BtOnFlag)
				{
					BtSoftreset();
					BtEnableFlag = 1;
					commState = 0;
				}
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
						commErSendTotalPrice(txRingBufPtr);
						commErSendWeight(txRingBufPtr);
						commErSendUnitPrice(txRingBufPtr);
						break;
				}
				commState = 0;
				break;
#ifdef USE_SMART_BT_PROTOCOL
			case 110:
				btLen = byte;
				btIdx = 0;
				btBcc = byte;
				commState++;
				break;
			case 111:
				if (btIdx >= 10) commState = 0;
				btCmd[btIdx++] = byte;
				btBcc ^= byte;
				if (btIdx >= btLen) commState++;
				break;
			case 112:
				if (byte == btBcc) commState++;
				else commState = 0;
				break;
			case 113:
				if (byte == ETX)
				{
					if (btCmd[0] == 'R')
					{
						if (btCmd[1] == 'I') commSmartBtSendScaleInformation(txRingBufPtr);
						else if (btCmd[1] == 'W') commSmartBtSendWeight(txRingBufPtr);
						else if (btCmd[1] == 'T') commSmartBtSendTare(txRingBufPtr);
						else if (btCmd[1] == 'P') commSmartBtSendPowerInformation(txRingBufPtr);
					}
					else if (btCmd[0] == 'K') 
					{
						if (btCmd[1] == 'T')
						{
							if (AdSetTare() == ERR_NONE)
							{
								commSmartBtSendData(txRingBufPtr, btCmd, 2);
							}
							else 
							{
								commSmartBtSendData(txRingBufPtr, &btNak, 1);
								BuzzerSetOnTimes(2);
							}
						}
						else if (btCmd[1] == 'Z') 
						{
							if (AdSetZero() == ERR_NONE)
							{
								commSmartBtSendData(txRingBufPtr, btCmd, 2);
							}
							else 
							{
								commSmartBtSendData(txRingBufPtr, &btNak, 1);
								BuzzerSetOnTimes(2);
							}
						}
					}
					else if (btCmd[0] == 'P')
					{
						if (btCmd[1] == 'T') commSmrtBtSetPresetTare(txRingBufPtr, &btCmd[2]);
					}
				}
				commState = 0;
				break;
#endif
			default:
				commState = 0;
				break;
		}
	}
}

#ifdef USE_SMART_BT_PROTOCOL
void commBtInterpreter(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr)
{
	char byte;
#ifdef PC_TARE
	static INT8U tare_data[4];
	static INT32U send_tare_data;
#endif
#ifdef USE_SMART_BT_PROTOCOL
	static INT8U btCmd[10];
	static INT8U btLen;
	static INT8U btIdx;
	static INT8U btBcc;
	static INT8U btNak;
	
	btNak = NAK;
#endif

	if (RingBufCheckBuf(rxRingBufPtr))
	{
		byte = RingBufGetChar(rxRingBufPtr);
		switch (commState)
		{
			case 0:
#ifdef USE_BT_CONNECT
				if(!AdCheckInitZero()) break;
				
				if(BtResetFlag || BtSoftResetFlag || BtOnFlag)
				{
					if(byte == 0x0d)
					{
						commState = 20;
						break;
					}
				}
				if(byte == 0x00)	//Device Number Check (0: Fix)
				{
					commState++;
					break;
				}
#else
				if (byte == ENQ)
				{
	#ifdef USE_DS252_PROTOCOL
					commIDSendWeight();
					break;
	#else 
					RingBufPutChar(txRingBufPtr, ACK);
					commState++;
	#endif
				}
#endif	//USE_BT_CONNECT
				else if (byte == ESC) //for Factory
				{
					commState = 200;
				}
#ifdef USE_EMS_PROTOCOL
				else if (byte == 'a') //0x61
				{
					commEMSSendWeight(txRingBufPtr);
				}
#endif
#ifdef USE_COMMAND_TARE_KEY
				else if ((byte == 't')||(byte == 'T')) //0x74
				{
					SaleTareExtKey();
					RunSaleWeightTareProc();
					commState=0;
				}
#endif
				else if (byte == 's')
				{
					RingBufPutChar(txRingBufPtr, 's');
					commState = 100;
				}
				
#ifdef USE_SMART_BT_PROTOCOL
				else if (byte == STX)
				{
					commState = 110;
				}
#endif
#ifdef USE_ADD_KEY_FOR_CERTIFICATION
				if(SaleData.addState == 2 || SaleData.addState == 4)
				{
					if(byte == 0x00)
					{
						if(SaleData.addState == 2) SaleData.printerState = 2;
						else SaleData.printerState = 4;
					}
					else if(byte == 0x04)
					{
						SaleData.printerState = 3;
					}
					else
					{
						SaleData.printerState = 0;
					}
				}
#endif
			break;
				
			case 1:
#ifdef USE_COMMAND_KEY
				if(byte==0x5A)	//'Z'
				{
					commState=2;
					break;
				}
				else if(byte==0x54)	//'T'
				{
					commState=3;
					break;
				}
				else if(byte==0x48)	//'H'
				{
					commState=4;
					break;
				}
				else if((byte != DC1) && (byte != DC2) && (byte != DC3))
				{
					RingBufPutChar(txRingBufPtr, NAK);
					commState=0;
					break;
				}
#endif
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
#ifdef PC_TARE
					case STX: //insert for test
					commState = 10;
					tare_count =0;
					break;
				}
#else
				}		
#endif	//PC_TARE
#ifdef USE_COMMAND_KEY
				if(commState < 2 || commState > 4)
#else
 #ifdef PC_TARE
				if(commState != 10)
 #endif
#endif
				commState = 0;
				break;
#ifdef USE_COMMAND_KEY
			case 2:
				if(byte==0x45) //'E'
				{
					SaleZeroExtKeyProc();
					if(saleKeyCommErrFlagState() == 1)
					{
						RingBufPutChar(txRingBufPtr, 'O');
						RingBufPutChar(txRingBufPtr, 'K');
					}
					else
					{
						RingBufPutChar(txRingBufPtr, 'N');
						RingBufPutChar(txRingBufPtr, 'G');
					}
					commState = 0;
				}
				else
				{
					RingBufPutChar(txRingBufPtr, NAK);
					commState = 0;
				}
				break;
			case 3: 
				if(byte==0x52) //'R'
				{
					SaleTareExtKeyProc();
					if(saleKeyCommErrFlagState() == 1)
					{
						RingBufPutChar(txRingBufPtr, 'O');
						RingBufPutChar(txRingBufPtr, 'K');
					}
					else
					{
						RingBufPutChar(txRingBufPtr, 'N');
						RingBufPutChar(txRingBufPtr, 'G');
					}
					commState = 0;
				}
				else
				{
					RingBufPutChar(txRingBufPtr, NAK);
					commState = 0;
				}
				break;
			case 4:
				if(byte==0x44) //'D'
				{
					SaleHoldExtKeyProc();
					if(saleKeyCommErrFlagState() == 1)
					{
						RingBufPutChar(txRingBufPtr, 'O');
						RingBufPutChar(txRingBufPtr, 'K');
					}
					else
					{
						RingBufPutChar(txRingBufPtr, 'N');
						RingBufPutChar(txRingBufPtr, 'G');
					}
					commState = 0;
				}
				else
				{
					RingBufPutChar(txRingBufPtr, NAK);
					commState = 0;
				}
				break;
#endif				

#ifdef PC_TARE //insert for test
			case 10: //insert for test

				tare_data[tare_count] = byte;
				
				if(tare_count == 3)
				{	
					 commState = 11;
				}
				else
				{
					commState = 10;
					tare_count++;
				}
				break;

			case 11:
				if(byte == ETX)
				{
					commState = 0;
					send_tare_data =0;
					for (tare_count = 0; tare_count <4 ; tare_count++)
					{
						send_tare_data += (INT32U)(tare_data[tare_count]-0x30) * DecPowerConvTable[3 - tare_count];
					}
					if(AdData.stableFlag)
					{
						RingBufPutChar(txRingBufPtr, ACK);
						AdSetPT(send_tare_data, 1);
					}

				}

				else
				{
					RingBufPutChar(txRingBufPtr, NAK);
					commState = 0;
				}
				break;
#endif	
#ifdef USE_BT_CONNECT
			case 20:
				if(byte == LF)
				{
					commState++;
				}
			case 21:
				if(byte == 'O')
				{
					commState++;
				}
			case 22:
				if(byte == 'K')
				{
					commState++;
				}
			case 23:
				if(byte == 0x0d)
				{
					commState++;
				}
			case 24:
				if(byte == 0x0a)
				{										
					if(BtResetFlag && !BtOnFlag && !BtSoftResetFlag)
					{
						commState++;
					}
					else if(BtOnFlag && !BtResetFlag && !BtSoftResetFlag)
					{
						commState = 26;
					}
					else	// BtSoftResetFlag = 1
					{
						BtSoftResetFlag = 0;
						commState = 0;
						break;
					}
				}
			case 25:
				if(BtResetFlag)
				{
					Btmodeconversion();
					commState = 0;
				}
				break;							
			case 26:
				if(BtOnFlag)
				{
					BtSoftreset();
					BtEnableFlag = 1;
					commState = 0;
				}
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
						commErSendTotalPrice(txRingBufPtr);
						commErSendWeight(txRingBufPtr);
						commErSendUnitPrice(txRingBufPtr);
						break;
				}
				commState = 0;
				break;
#ifdef USE_SMART_BT_PROTOCOL
			case 110:
				btLen = byte;
				btIdx = 0;
				btBcc = byte;
				commState++;
				break;
			case 111:
				if (btIdx >= 10) commState = 0;
				btCmd[btIdx++] = byte;
				btBcc ^= byte;
				if (btIdx >= btLen) commState++;
				break;
			case 112:
				if (byte == btBcc) commState++;
				else commState = 0;
				break;
			case 113:
				if (byte == ETX)
				{
					if (btCmd[0] == 'R')
					{
						if (btCmd[1] == 'I') commSmartBtSendScaleInformation(txRingBufPtr);
						else if (btCmd[1] == 'W') commSmartBtSendWeight(txRingBufPtr);
						else if (btCmd[1] == 'T') commSmartBtSendTare(txRingBufPtr);
						else if (btCmd[1] == 'P') commSmartBtSendPowerInformation(txRingBufPtr);
					}
					else if (btCmd[0] == 'K') 
					{
						if (btCmd[1] == 'T')
						{
							if (AdSetTare() == ERR_NONE)
							{
								commSmartBtSendData(txRingBufPtr, btCmd, 2);
							}
							else 
							{
								commSmartBtSendData(txRingBufPtr, &btNak, 1);
								BuzzerSetOnTimes(2);
							}
						}
						else if (btCmd[1] == 'Z') 
						{
							if (AdSetZero() == ERR_NONE)
							{
								commSmartBtSendData(txRingBufPtr, btCmd, 2);
							}
							else 
							{
								commSmartBtSendData(txRingBufPtr, &btNak, 1);
								BuzzerSetOnTimes(2);
							}
						}
					}
					else if (btCmd[0] == 'P')
					{
						if (btCmd[1] == 'T') commSmrtBtSetPresetTare(txRingBufPtr, &btCmd[2]);
					}
				}
				commState = 0;
				break;
#endif
				
			default:
				commState = 0;
				break;
		}
	}
}
/**
********************************************************************************
* @brief    BT Send scale information											**need to modify
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   STX U7 U6 U5 U4 U3 U2 U1 U0 BCC ETX
*           U7 ~ U0 -> Unit Price "   10000", if overload, "FFFFFFFF"
*           BCC -> Block check character exclusive or(^)
********************************************************************************
*/
void commSmartBtSendData(RING_BUF *txRingBufPtr, INT8U *sendData, INT8U len)
{
	INT8U bcc;
	INT8U i;
	
	bcc = len;
	
	RingBufPutChar(txRingBufPtr, STX);
	RingBufPutChar(txRingBufPtr, len);
	for (i = 0 ; i < len ; i++)
	{
		bcc ^= sendData[i];
		RingBufPutChar(txRingBufPtr, sendData[i]);
	}
	
	RingBufPutChar(txRingBufPtr, bcc);
	RingBufPutChar(txRingBufPtr, ETX);
}

/**
********************************************************************************
* @brief    BT Send scale information											**need to modify
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   STX U7 U6 U5 U4 U3 U2 U1 U0 BCC ETX
*           U7 ~ U0 -> Unit Price "   10000", if overload, "FFFFFFFF"
*           BCC -> Block check character exclusive or(^)
********************************************************************************
*/
void commSmartBtSendScaleInformation(RING_BUF *txRingBufPtr)
{
	INT8U strBuf[80];
	INT8U cnt;
	
	cnt = 0;
	MakeStrCopy(&strBuf[cnt], "RI,", 3);
	cnt += 3;
	MakeStrCopy(&strBuf[cnt], "NAME:", 5);
	cnt += 5;
#if MODEL_NUM == MODEL_OMV4_PB_LCD
	MakeStrCopy(&strBuf[cnt], "PB(OMV4) LCD    ", 16);
#endif
	cnt += 16;
	MakeStrCopy(&strBuf[cnt], ",VERSION:", 9);
	cnt += 9;
	MakeStrCopy(&strBuf[cnt], VERSION_STR, 2);
	cnt += 2;
	Int32sToDecStr(&strBuf[cnt], VERSION_NUM, 3, '0', 0, 0, 0);
	cnt += 3;
	MakeStrCopy(&strBuf[cnt], ",REVISION:", 10);
	cnt += 10;
	Int32sToDecStr(&strBuf[cnt], REVISION_NUM, 4, '0', 0, 0, 0);
	cnt += 4;
	MakeStrCopy(&strBuf[cnt], ",", 1);
	cnt += 1;
	
	commSmartBtSendData(txRingBufPtr, strBuf, cnt);
}

/**
********************************************************************************
* @brief    BT Send weight											**need to modify
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   STX U7 U6 U5 U4 U3 U2 U1 U0 BCC ETX
*           U7 ~ U0 -> Unit Price "   10000", if overload, "FFFFFFFF"
*           BCC -> Block check character exclusive or(^)
********************************************************************************
*/
void commSmartBtSendWeight(RING_BUF *txRingBufPtr)
{
	INT8U strBuf[10];
	INT8U cnt;

	cnt = 0;
	MakeStrCopy(&strBuf[cnt], "RW,", 3);
	cnt += 3;
	if (AdData.overloadFlag)
	{
		MakeStrCopy(&strBuf[cnt], "O-------", 7);
		cnt += 7;
	}
	else if (AdData.underloadFlag)
	{
		MakeStrCopy(&strBuf[cnt], "N-------", 7);
		cnt += 7;
	}
	else
	{
		if (AdData.stableFlag)
		{
			strBuf[cnt++] = 'S';
		}
		else
		{
			strBuf[cnt++] = 'U';
		}
		
		if (SaleData.weight < 0)
		{
			strBuf[cnt++] = '-';
		}
		else
		{
			strBuf[cnt++] = ' ';
		}
		Int32sToDecStr(&strBuf[cnt], AbsInt32s(SaleData.weight), 6, ' ', AdData.weightDecPos, '.', 0);
		cnt += 6;
	}

	commSmartBtSendData(txRingBufPtr, strBuf, cnt);
}

/**
********************************************************************************
* @brief    BT Send Tare											**need to modify
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   STX U7 U6 U5 U4 U3 U2 U1 U0 BCC ETX
*           U7 ~ U0 -> Unit Price "   10000", if overload, "FFFFFFFF"
*           BCC -> Block check character exclusive or(^)
********************************************************************************
*/
void commSmartBtSendTare(RING_BUF *txRingBufPtr)
{
	INT8U strBuf[10];
	INT8U cnt;
	
	cnt = 0;
	MakeStrCopy(strBuf, "RT,", 3);
	cnt += 3;
	Int32sToDecStr(&strBuf[cnt], SaleData.keyTare, 6, ' ', AdData.weightDecPos, '.', 0);
	cnt += 6;
	commSmartBtSendData(txRingBufPtr, strBuf, cnt);
}

/**
********************************************************************************
* @brief    BT Send Power inofrmation											**need to modify
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   STX U7 U6 U5 U4 U3 U2 U1 U0 BCC ETX
*           U7 ~ U0 -> Unit Price "   10000", if overload, "FFFFFFFF"
*           BCC -> Block check character exclusive or(^)
********************************************************************************
*/
void commSmartBtSendPowerInformation(RING_BUF *txRingBufPtr)
{
	INT8U strBuf[10];
	INT8U cnt;
    INT8U battType;
    INT8U battAmount;
	
	cnt = 0;
	MakeStrCopy(&strBuf[cnt], "RP,", 3);
	cnt += 3;
//    battType = PowerTypeCheck(); //v2기준으로 v4에서는 구현되어있지 않음.
	MakeStrCopy(&strBuf[cnt], "UN,", 3); //unknown 표시.
	cnt += 3;
    battAmount = BattDrvGetUseablePercent();
	Int32sToDecStr(&strBuf[cnt], battAmount, 3, ' ', 0, 0, 0);
	cnt += 3;
	
	commSmartBtSendData(txRingBufPtr, strBuf, cnt);
}

void commSmrtBtSetPresetTare(RING_BUF *txRingBufPtr, INT8U *rcvData)
{
	INT32S rcvTareData;
	INT8U i;
	INT8U byte;
	INT8U strBuf[2];
	INT8U cnt;
	
	cnt = 0;
	rcvTareData = 0;
	for (i = 0; i < 6; i++)
	{
		byte = *(rcvData++);
		if (byte != '.') rcvTareData *= 10;
		if (byte > '0' && byte <= '9') 
			rcvTareData += (INT32S)(byte - 0x30);
	}
	
	if (AdCheckPTInterval(rcvTareData))
	{
		strBuf[cnt++] = NAK;
	}
	else if (AdSetPT(rcvTareData,1))
	{
		strBuf[cnt++] = NAK;
	}
	else
	{
		MakeStrCopy(&strBuf[cnt], "PT", 2);
		cnt += 2;
	}

	commSmartBtSendData(txRingBufPtr, strBuf, cnt);
}

#endif	//#ifdef USE_SMART_BT_PROTOCOL

#ifdef USE_PRII_SPAIN_$_PROTOCOL
/**
 * @brief 	$-Protocol 무게 정보 전송
 * @param 	txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
 * @return 	none
 * @remark 	7자로 구성된 무게 전송 & 끝에 제어문자 [CR]
 * 			ex1) 0000000[CR]
 * 			ex2) 000.504[CR]
 */
void comm_$_protocol_send_weight(RING_BUF *txRingBufPtr)
{
	char strBuf[12] = {0};

	if (AdData.stableFlag) {
		if (SaleData.weight < 0) {
			Memset(strBuf, '-', 7);
		} else if (SaleData.weight == 0) {
			Memset(strBuf, '0', 7);
		} else if (AdData.overloadFlag) {
			Memset(strBuf, 'F', 7);
		} else {
			Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), 7, '0', AdData.weightDecPos, '.', 0);
		}
	} else {
		Memset(strBuf, '-', 7);
	}
	strBuf[7] = ASCII_CR;

	RingBufPutStr(txRingBufPtr, strBuf);
}
#endif /* USE_PRII_SPAIN_$_PROTOCOL */
#endif
