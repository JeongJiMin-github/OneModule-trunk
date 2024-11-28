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
#ifdef USE_ER_COMM_API

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
/** @brief Weight Unit String Table ũ�� */
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
/** @brief Communication Driver Rx Ring Buffer�� �޾ƿ��� ���� Pointer */
static RING_BUF	*commRxRingBufPtr;
/** @brief Communication Driver Tx Ring Buffer�� �޾ƿ��� ���� Pointer */
static RING_BUF	*commTxRingBufPtr;
/** @brief Communication Interpreter�� ���� ���� */
static INT8U commState;
#ifdef PC_TARE
static INT8U tare_count;
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
static void commTrSendWeight(RING_BUF *txRingBufPtr);
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
* @brief    Communication API Parameter�� default ������ �ʱ�ȭ\n
* @param    type : defualt type
* @return   none
* @remark   Parameter ������ �ʱⰪ�� �ִ´�.\n
*           Cal Mode�� Program ��忡�� ��ü �ʱ�ȭ�� �� �����Ѵ�.
********************************************************************************
*/
void CommSetDefaultParam()
{
	ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_BAUDRATE, BAUD_RATE_9600);
}

/**
********************************************************************************
* @brief    Communication API Init �Լ�\n
*           Serial Communication Speed Init\n
*           Communication Ring Buffer Init\n
* @param    none
* @return   none
* @remark   ó���� �ѹ� ���� �Ǵ� �Լ�\n
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
* @brief    Communication Procedure �Լ�\n
*           Communication Driver Procedure ȣ��\n
*           Communication Driver Ring Buffer Ȯ�� �� Protocol ó��
* @param    none
* @return   none
* @remark   ���� �ð��� �ѹ��� ��� ���� �Ǵ� �Լ�\n
*           ���� Protocol ó�� �κ� ���� �߰� �ʿ� (TO DO)
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
	Comm1DrvProc();
#endif
#ifdef USE_CONTI_SEND_WEIGHT_DATA
	currTimer100ms = TimerGetSysTimer100ms();

	if ((INT16U)(currTimer100ms - prevSendWeightTimer100ms) > 2)
	{
		prevSendWeightTimer100ms = currTimer100ms;
		
		commTrSendWeight(commTxRingBufPtr);
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

/**
********************************************************************************
* @brief    ER TR Send Weight �Լ�\n
* @param    txRingBufPtr : ���� �����͸� ���� Ring Buffer Pointer
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
/**
********************************************************************************
* @brief    ER Send Weight �Լ�\n
* @param    txRingBufPtr : ���� �����͸� ���� Ring Buffer Pointer
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
* @brief    ID-U Send Weight �Լ�\n
* @param    txRingBufPtr : ���� �����͸� ���� Ring Buffer Pointer
* @return   none
* @remark   W5 W4 W3 W2 W1 W0
*           W5 ~ W0 -> Weight Data "  10.25"

********************************************************************************
*/
void commIDSendWeight(void)
{
	if(AdData.underloadFlag) return;

	PrtPutNum(SaleData.weight, 7, SaleData.weightDecPos, 0, 0);
	PrtPutCRLF(1);
}
/**
********************************************************************************
* @brief    ER Send Total Price �Լ�\n
* @param    txRingBufPtr : ���� �����͸� ���� Ring Buffer Pointer
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

/**
********************************************************************************
* @brief    ER Send Unit Price �Լ�\n
* @param    txRingBufPtr : ���� �����͸� ���� Ring Buffer Pointer
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
* @brief    ER Send Unit Price �Լ�\n
* @param    txRingBufPtr : ���� �����͸� ���� Ring Buffer Pointer
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
* @brief    ER communication Interpreter �Լ�\n
* @param    rxRingBufPtr : ���� �����Ͱ� �ִ� Ring Buffer Pointer
*           txRingBufPtr : ���� �����͸� ���� Ring Buffer Pointer
* @return   none
* @remark   Protocol�� ������ ����.
*           1> Scale <-- ENQ(0x05) --  PC
*           2> Scale  -- ACK(0x06) --> PC
*           3> Scale <-- Command   --  PC
*                        DC1(0x11):Request weight data
*                        DC2(0x12):Request all data (weight,price,unit)
*                        DC3(0x13):Request all data + PLU no (TODO : insert func)
*           4> Scale  -- data blocks --> PC
*           "data blocks"�� ������ ����.
*           DC1�� ���� Data Block : SOH STX STA SIGN W5 W4 W3 W2 W1 W0 UN1 UN2 BCC ETX EOT
*           DC2�� ���� Data Block : SOH STX P7 P6 P5 P4 P3 P2 P1 P0 BCC ETX
*                                    STX STA SIGN W5 W4 W3 W2 W1 W0 UN1 UN2 BCC ETX
*                                    STX U7 U6 U5 U4 U3 U2 U1 U0 BCC ETX EOT
*           DC3�� ���� Data Block : ? (TODO)
********************************************************************************
*/
void commErInterpreter(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr)
{
	char byte;
#ifdef PC_TARE
	static INT8U tare_data[4];
	static INT32U send_tare_data;
 

#endif	

	if (RingBufCheckBuf(rxRingBufPtr))
	{
		byte = RingBufGetChar(rxRingBufPtr);
		switch (commState)
		{
			case 0:
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
					saleWeightTareProc();
					commState=0;
				}
#endif
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
					commSendGood(txRingBufPtr, NG); // 0: Not Good('NG'), 1: Good('OK')
					commState=0;
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
#else	
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
				
			default:
				commState = 0;
				break;
		}
	}
}

#ifdef USE_TURKEY_POS_PROTOCOL
/**
********************************************************************************
* @brief    Error Messege ���� �Լ�\n
* @param   none
* @return   none
* @remark   None Error ��� "OK" ����
			Error ��� "NG" ����
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
* @brief    CAS 22 Byte  �Լ�\n
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

