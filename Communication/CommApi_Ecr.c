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
#include "CommDrv.h"
#include "CommApi.h"
#include "DebugApi.h"

#include "../Application/ScaleModeApp.h"
#include "../Application/SaleModeApp.h"

#include "../Hardware/HardwareConfig.h"
#include "../SystemTimer/TimerApi.h"
#ifdef USE_ECR_COMM_API
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
//wcm0816 code optimaization
/** @brief ECR type#0,#1 Scale type ID Table Size */
#define COMM_SCALE_TYPE_ID_TABLE_SIZE 17
/** @brief Comm Speed Default */
#define BAUD_RATE_DEFAULT 				BAUD_RATE_9600
/** @brief ECR send Unit Price */
#define UNIT_PRICE 						0
/** @brief ECR send Total price */
#define TOTAL_PRICE 						1
/** @brief ECR stream mode send response delay */
#define STREAM_MODE_RESPONSE_DELAY		10

/** @brief ECR Protocol type12(Dialog6) Status Information */
#define ECR_TYPE12_STATUS_INFORM_00		0		//There is no error present
#ifdef USE_WEIGHT_ONLY_ECR
#define ECR_TYPE12_STATUS_INFORM_01		1		//General error on scale
#endif
#define ECR_TYPE12_STATUS_INFORM_02		2		//Parity error, or more charactoers than permitted
#define ECR_TYPE12_STATUS_INFORM_10		3		//incorrect record number detected
#define ECR_TYPE12_STATUS_INFORM_11		4		//no valid unit price
#define ECR_TYPE12_STATUS_INFORM_12		5		//no valid tare value received
#define ECR_TYPE12_STATUS_INFORM_13		6		//no valid text received
#define ECR_TYPE12_STATUS_INFORM_20		7		//scale still in motion(no equilibrium)-unstable
//#define ECR_TYPE12_STATUS_INFORM_21		8		//no motion since last weighing operation
//#define ECR_TYPE12_STATUS_INFORM_22		9		//price calculation not yet available
//#define ECR_TYPE12_STATUS_INFORM_30		10		//scale in MIN range
#define ECR_TYPE12_STATUS_INFORM_31		11		//scale in underload range or negative weight display
#define ECR_TYPE12_STATUS_INFORM_32		12		//scale in overload range
//#define ECR_TYPE12_STATUS_INFORM_33		13		//scale was not unloaded for approx. 2 minutes
#ifdef COUNTRY_CZECH
#define ECR_TYPE12_STATUS_INFORM_34		14		//weight on the platform is not modified
#endif
#ifdef USE_SATO_PRINTER_PROTOCOL
#define ECR_TYPE12_STATUS_INFORM_40		15		//load exceeds zero setting range
#define ECR_TYPE12_STATUS_INFORM_42		16		//tare setting error a) outside of tare range b) tare already set c) stack tare not allowed
#define ECR_TYPE12_STATUS_INFORM_52		17		// function not possible
#endif

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
/** @brief ECR Type*/
static INT8U commEcrType;
/** @brief Parity Type*/
static INT8U parityTypeChk;

/** @brief ECR Number of weight digit*/
static INT8U commSendDigit;

static INT32U commWeightBackup;
static BOOLEAN commTransFlag;
static BOOLEAN commInterLockFlag;
/** @brief Real time transaction delay at ECR Protocol type14 OR Dialog6 Status Information ECR Protocol type12*/
static INT8U responseDelayCtrl;//ECR type12에서 dialog6 Status information variable로 사용(Type12_StsInfo)
/** @brief ECR#12 command check 상태 변수 */
static INT8U commandState;
#ifdef USE_SLOVAKIA_PROTOCOL
extern BOOLEAN saleSendSKProtocolflag;
#endif
#ifdef USE_SATO_PRINTER_PROTOCOL
INT8U RecordeTareflag = 0;
#endif

/** @brief Weight Unit String Table */
static const char code commWeightUnitStrTable[COMM_WEIGHT_STR_TABLE_SIZE][2] =
{
	{'K','G'},	{'L', 'B'},	{'O', 'Z'},	{'G', ' '}
};

static const char code commWeightUnitSmallStrTable[COMM_WEIGHT_STR_TABLE_SIZE][2] =
{
	{'k','g'},	{'l', 'b'},	{'o', 'z'},	{'g', ' '}
};
//wcm0816 code optimaization
/** @brief ECR type#0,#1 Scale type ID Table */
static const char code commScaleIdTable[COMM_SCALE_TYPE_ID_TABLE_SIZE][2] =
{
	{15,'A'},	{30, 'B'},	{6, 'C'},	{2, 'G'}, {5, 'H'},
	{10,'I'},	{20, 'J'},	{60, 'O'},	{25, 'P'}, {0, ' '},
	{30,'D'},	{60, 'E'},	{15, 'F'},	{5, 'K'}, {10, 'L'},
	{20,'M'},	{50, 'N'}
};



/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
static void commEcrInterpreter(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
static void commEcrSendWeight(RING_BUF *txRingBufPtr, INT8U weightUnit, INT8U parityType);
static void commEcrSendPrice(RING_BUF *txRingBufPtr, INT8U priceType, INT8U parityType, INT8U priceSize);
static void commEcrSendStatus(RING_BUF *txRingBufPtr);
static void commEcrType0(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
static void commEcrType2(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
static void commEcrType4(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
static void commEcrType6(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
#if !defined (USE_SLOVAKIA_PROTOCOL) && !defined (COUNTRY_CZECH)
static void commEcrType7(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
static void commEcrType8(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
static void commEcrType9(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
#else
static void commEcrType8(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
#endif
//static void commEcrType11(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
#ifdef DIALOG6
static void commEcrType12(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
#ifndef USE_ECR_12_ONLY
static void commEcrType13(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
static void commEcrType14(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
#endif
static void commEcrSendTare(RING_BUF *txRingBufPtr, INT32U tareType, INT8U parityType);
#endif
#ifdef USE_SATO_PRINTER_PROTOCOL
static void commEcrType12(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
static void commEcrSendTare(RING_BUF *txRingBufPtr, INT32U tareType, INT8U parityType);
void sendDLP50Protocol(INT8U parityType, RING_BUF *txRingBufPtr);
#endif
#ifdef USE_SLOVAKIA_PROTOCOL
static void commEcrType16(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
#endif
#if !defined(USE_WEIGHT_ONLY_ECR) && defined(USE_ECR_ANKER_PROTOCOL)
static void commEcrType17(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
#endif
static void commSendCharWithParity(INT8U sendData, INT8U parityType, RING_BUF *txRingBufPtr);
//static INT8U commCheckCharWithParity(INT8U receiveData, INT8U parityType);
#ifdef USE_2BYPE_CHECKSUM_PROTOCOL
void comm2byteChecksum(RING_BUF *txRingBufPtr, INT8U size, INT8U parityType);
#endif
#ifdef USE_SEND_SAME_WEIGHT_ONCE
INT8U isSamePreviousWeight(void);
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
	ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_ECR_TYPE, ECR_TYPE_6);
	ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_1ST, 0);		// PRINT METHOD
	ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_SELECT, 1);	// 0:PRT, 1:ECR
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

	commEcrType = ParamReadByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_ECR_TYPE);
	commInterLockFlag = (BOOLEAN)(ParamReadByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_1ST) & 0x01);
	commTransFlag = 0;
	commWeightBackup = 0;
	switch (commEcrType)
	{
		case ECR_TYPE_0:
		case ECR_TYPE_1:
		case ECR_TYPE_3:
		case ECR_TYPE_8:
#ifdef COUNTRY_MEXICO
			parityTypeChk = PARITY_TYPE_NONE;
			commSendDigit = 6;
			tempInt8u = BAUD_RATE_9600;
			break;
#else
			parityTypeChk = PARITY_TYPE_EVEN;
			commSendDigit = 5;
			tempInt8u = BAUD_RATE_9600;
			break;
#endif
		case ECR_TYPE_2:
#if defined(COUNTRY_MEXICO)
			parityTypeChk = PARITY_TYPE_NONE;
			commSendDigit = 6;
			tempInt8u = BAUD_RATE_9600;
			break;
#else
			parityTypeChk = PARITY_TYPE_EVEN;
#ifdef USE_WEIGHT_ONLY_ECR
			if(Pd2ECRProtocol == 1)
			{
				commSendDigit = 5;
			}
			else
			{
				commSendDigit = 6;
			}
#else
			commSendDigit = 6;
#endif
			tempInt8u = BAUD_RATE_9600;
			break;				
#endif
//code optimaization
/*
		case ECR_TYPE_3:
			parityTypeChk = PARITY_TYPE_EVEN;
			commSendDigit = 5;
			tempInt8u = BAUD_RATE_9600;
			break;
*/
		case ECR_TYPE_4:
#if defined(COUNTRY_MEXICO)
			parityTypeChk = PARITY_TYPE_NONE;
			commSendDigit = 6;
			tempInt8u = BAUD_RATE_9600;
			break;
#elif defined(USE_DIALOG2_5DIGIT_PRICE)
			parityTypeChk = PARITY_TYPE_EVEN;
			commSendDigit = 6;
			tempInt8u = BAUD_RATE_9600;
			break;
#endif
		case ECR_TYPE_5:
#ifdef USE_DIALOG2_5DIGIT_PRICE
			parityTypeChk = PARITY_TYPE_EVEN;
			commSendDigit = 5;
			tempInt8u = BAUD_RATE_9600;
			break;
#else
			parityTypeChk = PARITY_TYPE_EVEN;
			commSendDigit = 6;
			tempInt8u = BAUD_RATE_9600;
			break;
#endif
		case ECR_TYPE_6:
		case ECR_TYPE_7:
		case ECR_TYPE_10:
			parityTypeChk = PARITY_TYPE_NONE;
			commSendDigit = 6;
			tempInt8u = BAUD_RATE_9600;
			break;
//code optimaization
/*
		case ECR_TYPE_8:
			parityTypeChk = PARITY_TYPE_EVEN;
			commSendDigit = 5;
			tempInt8u = BAUD_RATE_9600;
			break;
*/
		case ECR_TYPE_9:
			parityTypeChk = PARITY_TYPE_NONE;
			commSendDigit = 5;
			tempInt8u = BAUD_RATE_2400;
			break;
//code optimaization
/*
		case ECR_TYPE_10:
			parityTypeChk = PARITY_TYPE_NONE;
			commSendDigit = 6;
			tempInt8u = BAUD_RATE_9600;
			break;				
*/
		//case ECR_TYPE_11:
			//parityTypeChk = PARITY_TYPE_NONE;
			//commSendDigit = 5;
			//tempInt8u = BAUD_RATE_9600;
			//break;		
#ifdef DIALOG6
		case ECR_TYPE_12:
#ifdef USE_DIALOG2_5DIGIT_PRICE
			parityTypeChk = PARITY_TYPE_EVEN;
#else
			parityTypeChk = PARITY_TYPE_ODD;
#endif
			commSendDigit = 5;
			tempInt8u = BAUD_RATE_9600;
			responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_00;//Type12_StsInfo
			break;
  #ifndef USE_ECR_12_ONLY
		case ECR_TYPE_13:
			parityTypeChk = PARITY_TYPE_NONE;
			commSendDigit = 5;
			tempInt8u = BAUD_RATE_9600;
			break;				
		case ECR_TYPE_14:		//wcm0112
		case ECR_TYPE_15:
			parityTypeChk = PARITY_TYPE_NONE;
			commSendDigit = 5;
			tempInt8u = BAUD_RATE_9600;
			//commTransFlag = 1;
			responseDelayCtrl = 0;
			break;
  #endif
#endif
#ifdef USE_SATO_PRINTER_PROTOCOL
		case ECR_TYPE_12:
			parityTypeChk = PARITY_TYPE_ODD;
			commSendDigit = 5;
			tempInt8u = BAUD_RATE_9600;
			responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_00;//Type12_StsInfo
			break;
#endif
#ifdef USE_SLOVAKIA_PROTOCOL
		case ECR_TYPE_16:
			parityTypeChk = PARITY_TYPE_NONE;
			commSendDigit = 5;
			tempInt8u = BAUD_RATE_9600;
			break;			
#endif
#ifdef USE_ECR_ANKER_PROTOCOL
		case ECR_TYPE_17:
			parityTypeChk = PARITY_TYPE_NONE;
			commSendDigit = 5;
			tempInt8u = BAUD_RATE_9600;
			break;
#endif
		default :
			parityTypeChk = PARITY_TYPE_NONE;
			commSendDigit = 6;
			tempInt8u = BAUD_RATE_9600;
			break;
	}
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
#ifndef DIALOG6
	if (RingBufCheckBuf(commRxRingBufPtr) || (commTransFlag && AdData.stableFlag))
#else
#ifndef USE_SLOVAKIA_PROTOCOL
	if (RingBufCheckBuf(commRxRingBufPtr) || (commTransFlag && AdData.stableFlag) || ((commEcrType == ECR_TYPE_14) || (commEcrType == ECR_TYPE_15)))
#else
	if (RingBufCheckBuf(commRxRingBufPtr) || (commTransFlag && AdData.stableFlag) || ((commEcrType == ECR_TYPE_14) || (commEcrType == ECR_TYPE_15) || (commEcrType == ECR_TYPE_16)))
#endif
#endif

#ifdef USE_COUNT_COMPARE
	// COUNT MODE일 경우 ECR 전송안함
	if(SaleData.saleState == SALE_STATE_COUNT || SaleData.saleState == SALE_STATE_PRECOUNT)
	{
		(*commRxRingBufPtr).InPtr = (*commRxRingBufPtr).OutPtr; // Rx 링버퍼 초기화 
	}
	else
#endif
	{
		commEcrInterpreter(commRxRingBufPtr, commTxRingBufPtr);
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
void commEcrSendWeight(RING_BUF *txRingBufPtr, INT8U weightUnit, INT8U parityType)
{
	char strBuf[10];
	char sendByte, sendByte1;
	INT8U bcc;
	INT8U i;
	char scaleTypeID;
//	INT8U strStart;
//	INT8U strEnd;
#ifdef USE_SLOVAKIA_PROTOCOL
	INT8U EAN13Buf[13];
#endif

	bcc = 0;
	scaleTypeID = 0;
	switch(commEcrType)
	{
		case ECR_TYPE_0:	
		case ECR_TYPE_1:	
			scaleTypeID = 0;
			if(CalParam.calUnit == 0)
			{
				for(i = 0; i < 9; i++)
				{
					if(commScaleIdTable[i][0] == CalParam.capa)
					{
						scaleTypeID = commScaleIdTable[i][1];
					}
				}
			}
			else if(CalParam.calUnit == 1)
			{
				for(i = 10; i < COMM_SCALE_TYPE_ID_TABLE_SIZE; i++)
				{
					if(commScaleIdTable[i][0] == CalParam.capa)
					{
						scaleTypeID = commScaleIdTable[i][1];
					}
				}
			}
//wcm0816 code optimaization
/*
			switch(CalParam.capa)
			{
				case 2:
					scaleTypeID = 'G';						//2kg 0x47
					break;
				case 5:
					if(CalParam.calUnit == 0)
					{
						scaleTypeID = 'H';						//5kg 0x48
					}
					if(CalParam.calUnit == 1)
					{
						scaleTypeID = 'K';						//5lb 0x4B
					}
					break;	
				case 6:
					scaleTypeID = 'C';						//6kg 0x43
					break;
				case 10:
					if(CalParam.calUnit == 0)
					{
						scaleTypeID = 'I';						//10kg 0x49
					}
					if(CalParam.calUnit == 1)
					{
						scaleTypeID = 'L';						//10lb 0x4C
					}
					break;	
				case 15:
					if(CalParam.calUnit == 0)
					{
						scaleTypeID = 'A';						//15kg 0x41
					}
					else if(CalParam.calUnit == 1) 
					{
						scaleTypeID = 'F';						//15lb 0x46
					}
					break;
				case 20:
					if(CalParam.calUnit == 0)
					{
						scaleTypeID = 'J';						//20kg 0x4A
					}
					if(CalParam.calUnit == 1)
					{
						scaleTypeID = 'M';						//20lb 0x4D
					}
					break;	
				case 25:
					scaleTypeID = 'P';							//25kg 0x50
					break;									
				case 30:
					if(CalParam.calUnit == 0) 
					{
						scaleTypeID = 'B';						//30kg 0x42
					}
					else if(CalParam.calUnit == 1)
					{
						scaleTypeID = 'D';						//30lb 0x44
					}
					break;
				case 50:
					if(CalParam.calUnit == 1)
					{
						scaleTypeID = 'N';						//50lb 0x4E
					}
					break;
				case 60:
					if(CalParam.calUnit == 0) 
					{
						scaleTypeID = 'O';						//60kg 0x4F				
					}
					else if(CalParam.calUnit == 1)
					{
						scaleTypeID = 'E';						//60lb 0x45
					}
					break;
				default:
					break;
			}
*/
//			if (scaleTypeID) //왜 필요한지 모르겠음. ScaleTypeID가 없으면 프로토콜 에러남
//			{
				commSendCharWithParity(scaleTypeID, parityTypeChk, txRingBufPtr);
				bcc ^= scaleTypeID;		
//			}
			Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', 0, 0, 0);
//wcm0816 code optimaization
/*
			for (i = 0; i < commSendDigit; i++)
			{
				bcc ^= strBuf[i];
			}
*/
			for (i = 0; i < commSendDigit; i++)
			{
				commSendCharWithParity(strBuf[i], parityTypeChk, txRingBufPtr);
				bcc ^= strBuf[i];
			}
			commSendCharWithParity(bcc, parityTypeChk, txRingBufPtr);	//0x03
			break;
			
		case ECR_TYPE_6:
			if(AdData.stableFlag)
			{
				sendByte = 'S';		//"S", Stable(0x53)
			}
			else
			{
				sendByte = 'U';		//"U", Unstable(0x55)
			}

			if (SaleData.weight < 0)
			{
				sendByte1 = '-';		//"-", Negative(0x2D)
			}
			else if (AdData.grossZeroFlag || AdData.netZeroFlag || SaleData.weight >= 0)
			{
				sendByte1 = ' ';		//"Space", Zero(0x20)
			}
			else if (AdData.overloadFlag)
			{
				sendByte1 = 'F'; 	//"F", Overflow(0x46)
			}

			commSendCharWithParity(sendByte, parityTypeChk, txRingBufPtr);	
			commSendCharWithParity(sendByte1, parityTypeChk, txRingBufPtr);	
			
			bcc ^= sendByte;			
			bcc ^= sendByte1;
			
			Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, 0, AdData.weightDecPos, '.', 0);
//wcm0816 code optimaization
/*
			for (i = 0; i < commSendDigit; i++)
			{
				bcc ^= strBuf[i];
			}
*/
			for (i = 0; i < commSendDigit; i++)
			{
				commSendCharWithParity(strBuf[i], parityTypeChk, txRingBufPtr);
				bcc ^= strBuf[i];
			}
/*
			if(commSendDigit == 6)
			{
				if(weightUnit == UNIT_KG)
				{
					commSendCharWithParity('k', parityTypeChk, txRingBufPtr);	
					commSendCharWithParity('g', parityTypeChk, txRingBufPtr);	
					bcc ^= 'k';
					bcc ^= 'g';
				}
				else if(weightUnit == UNIT_GRAM)
				{
					commSendCharWithParity('g', parityTypeChk, txRingBufPtr);
					commSendCharWithParity(' ', parityTypeChk, txRingBufPtr);	
					bcc ^= 'g';
					bcc ^= ' ';
				}
				else if(weightUnit == UNIT_LB)
				{
					commSendCharWithParity('l', parityTypeChk, txRingBufPtr);	
					commSendCharWithParity('b', parityTypeChk, txRingBufPtr);
					bcc ^= 'l';
					bcc ^= 'b';
				}
				else if(weightUnit == UNIT_OZ)
				{
					commSendCharWithParity('o', parityTypeChk, txRingBufPtr);	
					commSendCharWithParity('z', parityTypeChk, txRingBufPtr);
					bcc ^= 'o';
					bcc ^= 'z';
				}
			}
*/
			commSendCharWithParity(commWeightUnitSmallStrTable[weightUnit][0], parityTypeChk, txRingBufPtr);	
			commSendCharWithParity(commWeightUnitSmallStrTable[weightUnit][1], parityTypeChk, txRingBufPtr);
			bcc ^= commWeightUnitSmallStrTable[weightUnit][0];
			bcc ^= commWeightUnitSmallStrTable[weightUnit][1];

#ifndef USE_2BYPE_CHECKSUM_PROTOCOL
			commSendCharWithParity(bcc, parityTypeChk, txRingBufPtr);
#endif
			break;
			
		case ECR_TYPE_2:
			Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', 0, 0, 0);

			for (i = 0; i < commSendDigit; i++)
			{
				commSendCharWithParity(strBuf[i], parityTypeChk, txRingBufPtr);
			}
			break;
			
		case ECR_TYPE_3:
		case ECR_TYPE_9:
		case ECR_TYPE_10:
/*		case ECR_TYPE_11:
//			if(weightUnit == UNIT_LB || weightUnit == UNIT_OZ) commSendDigit = 4;
//			else						commSendDigit = 5;
			if(commEcrType == ECR_TYPE_10) 
			{
				commSendDigit = 6;
				Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', 3, '.', 0);
			}
			else Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', 0, 0, 0);
//			if(parityType != PARITY_TYPE_NONE)
//			{
				//RingBufPutData(txRingBufPtr, strBuf, commSendDigit);
				for (i = 0; i < commSendDigit; i++)
				{
					//strBuf[i] = commMakeCharWithParity(strBuf[i], parityType);
					commSendCharWithParity(strBuf[i], parityTypeChk, txRingBufPtr);
				}
				//RingBufPutData(txRingBufPtr, strBuf, commSendDigit);
//			}			
//			else RingBufPutData(txRingBufPtr, strBuf, commSendDigit);			
			break;
*/
#ifdef DIALOG6
		case ECR_TYPE_12:
  #ifndef USE_ECR_12_ONLY
		case ECR_TYPE_13:
  #endif
			if(commEcrType == ECR_TYPE_10) Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', AdData.weightDecPos, '.', 0);
			else Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', 0, 0, 0);

			for (i = 0; i < commSendDigit; i++)
			{
				commSendCharWithParity(strBuf[i], parityTypeChk, txRingBufPtr);
			}
			break;
//code optimaization
/*
		case ECR_TYPE_13:
			Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', 0, 0, 0);

			for (i = 0; i < commSendDigit; i++)
			{
				commSendCharWithParity(strBuf[i], parityTypeChk, txRingBufPtr);
			}
			break;		
*/
  #ifndef USE_ECR_12_ONLY
		case ECR_TYPE_14:
		case ECR_TYPE_15:
			//type15 STA1

			if(commEcrType == ECR_TYPE_15)
			{
				if(AdData.tareWeight) sendByte = 0x4E;
				else sendByte = 0x20;

				commSendCharWithParity(sendByte, parityTypeChk, txRingBufPtr);
			}
			//type14 STA / type15 STA2
			if(AdData.overloadFlag) sendByte = 0x4F;		//"O", Over weight
			else if((SaleData.weight < 0) && (commEcrType != ECR_TYPE_15)) sendByte = 0x2D;		//"-", Negative
			else if((AdData.grossZeroFlag || AdData.netZeroFlag) && (commEcrType == ECR_TYPE_15)) sendByte = 0x5A;	//"Z", Zero
			else if((commEcrType == ECR_TYPE_15) && (AdData.rawSetZeroData < 0)) sendByte = 0x4C;	//"L", Below Real Zero
			else {
				if(AdData.stableFlag) sendByte = 0x53;		//"S", Stable
				else sendByte = 0x55;		//"U", Unstable
			}

			commSendCharWithParity(sendByte, parityTypeChk, txRingBufPtr);	
			//type15 STA3			
			if(commEcrType == ECR_TYPE_15)
			{
				if(SaleData.weight < 0) sendByte = 0x2D;
				else sendByte = 0x20;

				commSendCharWithParity(sendByte, parityTypeChk, txRingBufPtr);
			}
			
			//if((commEcrType == ECR_TYPE_15) && ((AdData.rawSetZeroData < 0) || (AdData.overloadFlag))) MakeStrCopy(strBuf, "-----", commSendDigit);
			if((SaleData.weight < 0) || (AdData.overloadFlag)) MakeStrCopy(strBuf, "-----", commSendDigit); 
			else Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', 0, 0, 0);
			for (i = 0; i < commSendDigit; i++)
			{
				commSendCharWithParity(strBuf[i], parityTypeChk, txRingBufPtr);
			}
			commSendCharWithParity(RS, parityTypeChk, txRingBufPtr);
			break;
  #endif

#endif
		case ECR_TYPE_4:
		case ECR_TYPE_5:
		case ECR_TYPE_7:
		case ECR_TYPE_8:
//			if(commEcrType == ECR_TYPE_8) Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', 2, '.', 0);
//			else Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', AdData.weightDecPos, '.', 0);
			if(commEcrType == ECR_TYPE_4 || commEcrType == ECR_TYPE_5) 
			{
				if(SaleData.weight < 0 || AdData.overloadFlag) MakeStrCopy(strBuf, "------", commSendDigit);
				else Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', AdData.weightDecPos, '.', 0);
			}
			else
			{
				if(SaleData.weight < 0 || AdData.overloadFlag || !AdData.stableFlag) MakeStrCopy(strBuf, "------", commSendDigit);
				else Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', AdData.weightDecPos, '.', 0);
			}
			for (i = 0; i < commSendDigit; i++)
			{
				commSendCharWithParity(strBuf[i], parityTypeChk, txRingBufPtr);
			}
			break;
#ifdef USE_SLOVAKIA_PROTOCOL
		case ECR_TYPE_16:
			if(AdData.overloadFlag) 
				break;
			if(AdData.stableFlag && saleSendSKProtocolflag)
			{
				sendByte = '2';
				sendByte1= '8';
				commSendCharWithParity(sendByte, parityTypeChk, txRingBufPtr);
				commSendCharWithParity(sendByte1, parityTypeChk, txRingBufPtr);
				bcc += 2;
				bcc += 8*3;				
				Int32sToDecStr(strBuf, AbsInt32s(SaleData.unitPrice), 4, '0', 0, 0, 0);
				for(i = 0; i < 4; i++) // item code number(not plu number) is 4digit
				{
					if(i % 2 == 0)
						bcc += (strBuf[i] - '0');
					else
						bcc += (strBuf[i] - '0') * 3;	
					commSendCharWithParity(strBuf[i], parityTypeChk, txRingBufPtr);
				}
				bcc %= 10;
				if(bcc != 0)
					bcc = 10 - bcc;
				bcc += '0';
				commSendCharWithParity(bcc, parityTypeChk, txRingBufPtr);

				bcc=0;
				Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), 5, '0', 0, 0, 0);
				for(i = 0; i < 5; i++)
				{
					if(i % 2 == 0)
						bcc += (strBuf[i] - '0');
					else
						bcc += (strBuf[i] - '0') * 3;	
					commSendCharWithParity(strBuf[i], parityTypeChk, txRingBufPtr);
				}
				bcc %= 10;
				if(bcc != 0)
					bcc = 10 - bcc;
				bcc += '0';
				commSendCharWithParity(bcc, parityTypeChk, txRingBufPtr);
				commSendCharWithParity(0x0d, parityTypeChk, txRingBufPtr);  //CR->0x0d
				commSendCharWithParity(0x0a, parityTypeChk, txRingBufPtr);  //Lf->0x0A
			}
			break;
#endif
#ifdef USE_ECR_ANKER_PROTOCOL
		case ECR_TYPE_17:
			Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', 0, 0, 0);
			for (i = 0; i < commSendDigit; i++)
			{
				commSendCharWithParity(strBuf[i], parityTypeChk, txRingBufPtr);
			}
			break;
#endif
		
	}
}

/**
********************************************************************************
* @brief    ECR Send Total Price 함수\n
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   STX P7 P6 P5 P4 P3 P2 P1 P0 BCC ETX
*           P7 ~ P0 -> Total Price "   10000", if overload, "FFFFFFFF"
*           BCC -> Block check character exclusive or(^)
********************************************************************************
*/
void commEcrSendStatus(RING_BUF *txRingBufPtr)
{
	INT8U sendByte1;
	INT8U sendByte2;

	switch (commEcrType)
	{
		case ECR_TYPE_2:
		case ECR_TYPE_3:
		case ECR_TYPE_10:	
			sendByte1 = 0x40;
			if(AdData.grossZeroFlag || AdData.netZeroFlag) sendByte1 |= 0x10;
			if(SaleData.weight < 0) sendByte1 |= 0x04;
			if(AdData.overloadFlag) sendByte1 |= 0x02;
			if(!AdData.stableFlag) sendByte1 |= 0x01;
			commSendCharWithParity(sendByte1, parityTypeChk, txRingBufPtr);
			break;
			
		case ECR_TYPE_4:
		case ECR_TYPE_5:
			sendByte1 = 0x30;			//"0", OK
			sendByte2 = 0x30;			//"0", OK
			if(AdData.overloadFlag)
			{
				sendByte2 = 0x32;		//"2", OverFlow
			}
			else if(SaleData.weight < 0)
			{
				sendByte2 = 0x31;		//"1", Negative
			}
			else if(AdData.grossZeroFlag || AdData.netZeroFlag)
			{
				sendByte1 = 0x32;		//"2", Zero
			}
			else if(!AdData.stableFlag) 
			{
				sendByte1 = 0x31;		//"1", motion
			}		
			commSendCharWithParity(sendByte1, parityTypeChk, txRingBufPtr);			
			commSendCharWithParity(sendByte2, parityTypeChk, txRingBufPtr);
			break;
	}
}

/**
********************************************************************************
* @brief    ECR Send Total Price 함수\n
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   STX P7 P6 P5 P4 P3 P2 P1 P0 BCC ETX
*           P7 ~ P0 -> Total Price "   10000", if overload, "FFFFFFFF"
*           BCC -> Block check character exclusive or(^)
********************************************************************************
*/
void commEcrSendPrice(RING_BUF *txRingBufPtr, INT8U priceType, INT8U parityType, INT8U priceSize)
{
	char strBuf[6];
	INT8U i;

	if (priceSize > 6){
		priceSize = 6;
	}

#ifdef	USE_DIALOG2_5DIGIT_PRICE
	if(priceType == UNIT_PRICE)
	{
		priceSize = 5;
		Int32sToDecStr(strBuf, SaleData.unitPrice , priceSize, '0', 0, 0, 0);
	}
	else Int32sToDecStr(strBuf, SaleData.totalPrice , priceSize, '0', 0, 0, 0);
#else
  #ifdef USE_WEIGHT_ONLY_ECR
	Int32sToDecStr(strBuf, 0, priceSize, '0', 0, 0, 0);
  #else
	if(priceType == UNIT_PRICE) Int32sToDecStr(strBuf, SaleData.unitPrice , priceSize, '0', 0, 0, 0);
	else Int32sToDecStr(strBuf, SaleData.totalPrice , priceSize, '0', 0, 0, 0);
  #endif
#endif
	for (i = 0; i < priceSize; i++)
	{
		if (strBuf[i] == '\0') break;
		commSendCharWithParity(strBuf[i], parityType, txRingBufPtr);
	}
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
void commEcrInterpreter(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr)
{
	switch (commEcrType)
	{
		case ECR_TYPE_0:
//wcm0816 code optimaization
//			commEcrType0(rxRingBufPtr, txRingBufPtr);
//			break;
		case ECR_TYPE_1:
			commEcrType0(rxRingBufPtr, txRingBufPtr);
			break;
		case ECR_TYPE_2:
//wcm0816 code optimaization
//			commEcrType2(rxRingBufPtr, txRingBufPtr);
//			break;
		case ECR_TYPE_3:	
//wcm0816 code optimaization
//			commEcrType2(rxRingBufPtr, txRingBufPtr);
//			break;
		case ECR_TYPE_10:
			commEcrType2(rxRingBufPtr, txRingBufPtr);
			break;
		case ECR_TYPE_4:
//wcm0816 code optimaization
//			commEcrType4(rxRingBufPtr, txRingBufPtr);
//			break;
		case ECR_TYPE_5:
			commEcrType4(rxRingBufPtr, txRingBufPtr);
			break;
		case ECR_TYPE_6:
			commEcrType6(rxRingBufPtr, txRingBufPtr);
			break;
#if !defined (USE_SLOVAKIA_PROTOCOL) && !defined (COUNTRY_CZECH)
		case ECR_TYPE_7:
			commEcrType7(rxRingBufPtr, txRingBufPtr);
			break;
		case ECR_TYPE_8:
			commEcrType8(rxRingBufPtr, txRingBufPtr);
			break;
		case ECR_TYPE_9:
			commEcrType9(rxRingBufPtr, txRingBufPtr);
			break;
#else
		case ECR_TYPE_8:
			commEcrType8(rxRingBufPtr, txRingBufPtr);
			break;
#endif
		//case ECR_TYPE_11:
			//commEcrType11(rxRingBufPtr, txRingBufPtr);
			//break;		
#ifdef DIALOG6
		case ECR_TYPE_12:
			commEcrType12(rxRingBufPtr, txRingBufPtr);
			break;
  #ifndef USE_ECR_12_ONLY
		case ECR_TYPE_13:
			commEcrType13(rxRingBufPtr, txRingBufPtr);
			break;
		case ECR_TYPE_14:
		case ECR_TYPE_15:
			commEcrType14(rxRingBufPtr, txRingBufPtr);
			break;
  #endif
#endif
#ifdef USE_SATO_PRINTER_PROTOCOL
		case ECR_TYPE_12:
			commEcrType12(rxRingBufPtr, txRingBufPtr);
			break;
#endif
#ifdef USE_SLOVAKIA_PROTOCOL
		case ECR_TYPE_16:
			commEcrType16(rxRingBufPtr, txRingBufPtr);
			break;
#endif
#if !defined(USE_WEIGHT_ONLY_ECR) && defined(USE_ECR_ANKER_PROTOCOL)
		case ECR_TYPE_17:
			commEcrType17(rxRingBufPtr, txRingBufPtr);
			break;
#endif
	}
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
*           3> Scale <-- DC2(0x12)  --  PC
*           4> Scale  -- ID(CAPA) --> PC
*           "ID(CAPA)"는 다음과 같다.
*           ID(CAPA) : 6kg => 0x43, 15lb => 0x46, 
*                            15kg => 0x41, 30lb => 0x44,
*                            25kg => 0x42, 50lb => 0x45(PD-II)
*                            30kg => 0x42, 60lb => 0x45(ER-PLUS)
*           5> Scale  -- data blocks --> PC
*           "data blocks"는 다음과 같다.
*           Data Block : SOH STX STA SIGN W5 W4 W3 W2 W1 W0 UN1 UN2 BCC ETX EOT
********************************************************************************
*/
void commEcrType0(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr)
{	
	char byte;
	INT8U error;

	if (RingBufCheckBuf(rxRingBufPtr))
	{
		byte = RingBufGetCharWithParity(rxRingBufPtr, parityTypeChk, &error);
		if (error == 1)
		{
			commState = 0;
			return;
		}		
		switch (commState)
		{
			case 0:
				if (byte == ENQ)
				{
					commSendCharWithParity(ACK, parityTypeChk, txRingBufPtr);	//0x06
					commState++;
				}
				else if (byte == ESC) //for Factory
				{
					commState = 200;
				}
				else
				{
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15
					commState = 0;
				}
				break;
			case 1:
//wcm0816 code optimaization
/*				switch (byte)
				{
					case DC2:	//weight data
						if(SaleData.weight == 0)
						{
							commSendCharWithParity(BEL, parityTypeChk, txRingBufPtr);	//0x07
						}
						else
						{
							commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);	//0x02
							commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk);
							commSendCharWithParity(ETX, parityTypeChk, txRingBufPtr);	//0x03
						}
						break;
				}
*/
				if(byte == DC2)
				{
						if(SaleData.weight == 0)
						{
							commSendCharWithParity(BEL, parityTypeChk, txRingBufPtr);	//0x07
						}
						else if (SaleData.weight < 0 || !AdData.stableFlag) //csh 20120904
						{
							commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x07
						}						
						else
						{
#ifdef USE_SEND_SAME_WEIGHT_ONCE
							if (isSamePreviousWeight() != SAME_WEIGHT)
#endif
							{
								commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);	//0x02
								commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk);
								commSendCharWithParity(ETX, parityTypeChk, txRingBufPtr);	//0x03
							}
						}
				}
				else
				{
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15 //csh 20120904
				}

				commState = 0;
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
}

/**
********************************************************************************
* @brief    ER communication Interpreter 함수\n
* @param    rxRingBufPtr : 받은 데이터가 있는 Ring Buffer Pointer
*           txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   Protocol은 다음과 같다.
			(I) type2
*           1> Scale <-- W(0x57) --  PC
*           2> Scale  -- data blocks --> PC
*           "data blocks"는 다음과 같다.
*           Data Block : STX W5 W4 W3 W2 W1 W0 CR
*			(II) type3
*			1> Scale <-- W(0x57) --  PC
*           2> Scale  -- data blocks --> PC
*			3> data blocks를 연속으로 전송
*           "data blocks"는 다음과 같다.
*           Data Block : STX W5 W4 W3 W2 W1 W0 CR
*			(III) type10
*			1> Scale <-- W(0x57) --  PC
*           2> Scale  -- data blocks --> PC
*           "data blocks"는 다음과 같다.
*           Data Block : STX W4 W3 W2 2E W1 W0 CR
*********************************************************************************
*/
void commEcrType2(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr)
{
	char byte;
	INT8U error;

	if (RingBufCheckBuf(rxRingBufPtr) || commTransFlag)
	{
		if(RingBufCheckBuf(rxRingBufPtr))
		{
			byte = RingBufGetCharWithParity(rxRingBufPtr, parityTypeChk, &error);
			if (error == 1)
			{
				commState = 0;
				return;
			}
		}
		else
		{
			byte = 0;
		}
//wcm0816 code optimaization
/*		switch (commState)
		{
			case 0:
				if (byte == 'W')	//0x57
				{
					commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);  //0x02
					if(!AdData.grossZeroFlag && !AdData.netZeroFlag && AdData.stableFlag && SaleData.weight >= 0 && !AdData.overloadFlag)
					{
							commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk);	
					}
					else 
					{						
						commSendCharWithParity('?', parityTypeChk, txRingBufPtr);  //0x3F
						commEcrSendStatus(txRingBufPtr);
					}					
					commSendCharWithParity(0x0d, parityTypeChk, txRingBufPtr);  //CR
					commState = 0;
					if (commEcrType == ECR_TYPE_3) 
					{
						commTransFlag = 1;
						commWeightBackup = SaleData.weight;
					}
				}
				else if (byte == ESC) //for Factory
				{
					commState = 200;
				}
				else if(byte == 0x0d)	//CR
				{
					commState = 0;
					if (commEcrType == ECR_TYPE_3) commTransFlag = 0;
				}
				else
				{
					commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);  //0x02					
					commSendCharWithParity('?', parityTypeChk, txRingBufPtr);  //0x3F					
					commSendCharWithParity('X', parityTypeChk, txRingBufPtr);  //0x58					
					commSendCharWithParity(0x0d, parityTypeChk, txRingBufPtr);  //CR
					commState = 0;
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
	if (commTransFlag && (commWeightBackup != SaleData.weight))
	{		
		commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);  //0x02
		if(!AdData.grossZeroFlag && !AdData.netZeroFlag && AdData.stableFlag && SaleData.weight >= 0 && !AdData.overloadFlag)
		{
			if(AdData.weightUnit != 0)
			{
				commSendCharWithParity('0', parityTypeChk, txRingBufPtr);  //0x30
			}
			commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk);
		}
		else 
		{						
			commSendCharWithParity('?', parityTypeChk, txRingBufPtr);  //0x3F
			commEcrSendStatus(txRingBufPtr);
		}				
		commSendCharWithParity(0x0d, parityTypeChk, txRingBufPtr);  //CR
		commWeightBackup = SaleData.weight;
	}

*/
		if((commState == 0) || (commWeightBackup != SaleData.weight))
		{
				if(byte == ESC)
				{
					commState = 200;
				}
				else if(byte == 0x0d && commEcrType == ECR_TYPE_3)
				{
					commTransFlag = 0;
				}
				else 
				{
				//	commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);  //0x02					
#ifdef COUNTRY_MEXICO
					if((byte == 'W') || (byte == 'I') || commTransFlag)
#else					
					if((byte == 'W') || commTransFlag)
#endif
					{
						if(!AdData.grossZeroFlag && !AdData.netZeroFlag && AdData.stableFlag && SaleData.weight >= 0 && !AdData.overloadFlag)
						{
#ifdef USE_SEND_SAME_WEIGHT_ONCE
							if (isSamePreviousWeight() != SAME_WEIGHT)
#endif
							{
								commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);  //0x02
#ifdef USE_WEIGHT_ONLY_ECR
								if(Pd2ECRProtocol != 1 && AdData.weightUnit != 0)
								{
									commSendCharWithParity('0', parityTypeChk, txRingBufPtr);  //0x30
								}
								else
								{								
									commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk);									
								}
#else
								if(AdData.weightUnit != 0)
								{
									commSendCharWithParity('0', parityTypeChk, txRingBufPtr);  //0x30
								}								
								commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk);		//무게 전송 추가(버그)							
#endif
								commSendCharWithParity(0x0d, parityTypeChk, txRingBufPtr);  //CR->0x0d
							}
						}
						else 
						{		
							commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);  //0x02				
							commSendCharWithParity('?', parityTypeChk, txRingBufPtr);  //0x3F
							commEcrSendStatus(txRingBufPtr);
							commSendCharWithParity(0x0d, parityTypeChk, txRingBufPtr);  //CR->0x0d
						}				

						if (commEcrType == ECR_TYPE_3) 
						{
							commTransFlag = 1;
							commWeightBackup = SaleData.weight;
						}
					}
					else
					{
						commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);  //0x02
						commSendCharWithParity('?', parityTypeChk, txRingBufPtr);  //0x3F					
						commSendCharWithParity('X', parityTypeChk, txRingBufPtr);  //0x58		
						commSendCharWithParity(0x0d, parityTypeChk, txRingBufPtr);  //CR->0x0d			
					}

				//	commSendCharWithParity(0x0d, parityTypeChk, txRingBufPtr);  //CR->0x0d
				}
		}
		else if(commState == 200)
		{
				if (!CommOmInterpreter(commTxRingBufPtr, byte)) 
				{
					commState = 0;
				}
		}
		else
		{
			commState = 0;
		}
	}
}

/**
********************************************************************************
* @brief    ER communication Interpreter 함수\n
* @param    rxRingBufPtr : 받은 데이터가 있는 Ring Buffer Pointer
*           txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   Protocol은 다음과 같다.
*			(I) type4
*           1> Scale <-- W(0x57) --  PC
*           2> Scale <-- CR(0x0D) -- PC
*           3> Scale  -- data blocks --> PC
*           "data blocks"는 다음과 같다.
*           Data Block	: LF W5 W4 W3 W2 W1 W0 UN1 UN2 CR LF S STATUS CR ETX 
*           Status(2bytes): 0 0 : OK
*						   	0 2	: OverFlow
*							2 0 : Zero
*							0 1 : Negative
*							1 0 : motion
*			(II) type5
*           1> Scale <-- W(0x57) --  PC
*           2> Scale  -- CR(0x0D) --> PC
*           3> Scale  -- data blocks --> PC
*           "data blocks"는 다음과 같다.
*           Data Block	: LF W5 W4 W3 W2 W1 W0 UN1 UN2 CR LF STATUS CR ETX 
*           Status(2bytes): 0 0 : OK
*						   	0 2	: OverFlow
*							2 0 : Zero
*							0 1 : Negative
*							1 0 : motion
*********************************************************************************
*/
void commEcrType4(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr)
{
	char byte;
	INT8U error;

	if (RingBufCheckBuf(rxRingBufPtr))
	{
		byte = RingBufGetCharWithParity(rxRingBufPtr, parityTypeChk, &error);
		if (error == 1)
		{
			commState = 0;
			return;
		}
		switch (commState)
		{
			case 0:
#ifdef COUNTRY_MEXICO
				if ((byte == 'W') || (byte == 'I'))
#else
				if (byte == 'W')
#endif
				{
					commState++;
				}
				else if (byte == ESC) //for Factory
				{
					commState = 200;
				}
				break;
			
			case 1:
				if(byte == 0x0d)	//CR
				{
#ifdef USE_SEND_SAME_WEIGHT_ONCE
					if (isSamePreviousWeight() != SAME_WEIGHT)
#endif				
					{
						commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
						commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk);
//wcm0816 code optimaization
/*
						if(AdData.weightUnit == UNIT_KG) //commState = 10;
						{
							commSendCharWithParity('K', parityTypeChk, txRingBufPtr);	//0x4B
							commSendCharWithParity('G', parityTypeChk, txRingBufPtr);	//0x47
						}
						else if(AdData.weightUnit == UNIT_GRAM) //commState = 10;
						{
							commSendCharWithParity('G', parityTypeChk, txRingBufPtr);	//0x47
							commSendCharWithParity(' ', parityTypeChk, txRingBufPtr);	//0x20 Space
						}
						else if(AdData.weightUnit == UNIT_LB) //commState = 20;
						{
							commSendCharWithParity('L', parityTypeChk, txRingBufPtr);	//0x4C
							commSendCharWithParity('B', parityTypeChk, txRingBufPtr);	//0x42
						}
						else if(AdData.weightUnit == UNIT_OZ) //commState = 30;
						{
							commSendCharWithParity('O', parityTypeChk, txRingBufPtr);	//0x4C
							commSendCharWithParity('Z', parityTypeChk, txRingBufPtr);	//0x42
						}
*/
#ifdef USE_WEIGHT_ONLY_ECR
						if(Pd2ECRProtocol == 1)
						{
							commSendCharWithParity(commWeightUnitStrTable[AdData.weightUnit][0], parityTypeChk, txRingBufPtr);
							commSendCharWithParity(commWeightUnitStrTable[AdData.weightUnit][1], parityTypeChk, txRingBufPtr);
						}
						else
						{
							commSendCharWithParity(commWeightUnitSmallStrTable[AdData.weightUnit][0], parityTypeChk, txRingBufPtr);
							commSendCharWithParity(commWeightUnitSmallStrTable[AdData.weightUnit][1], parityTypeChk, txRingBufPtr);
						}
#else
						commSendCharWithParity(commWeightUnitSmallStrTable[AdData.weightUnit][0], parityTypeChk, txRingBufPtr);
						commSendCharWithParity(commWeightUnitSmallStrTable[AdData.weightUnit][1], parityTypeChk, txRingBufPtr);
#endif
						commSendCharWithParity(0x0d, parityTypeChk, txRingBufPtr);	//CR-0x0d
						commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);		//0x0A
						if (commEcrType == ECR_TYPE_4)
							commSendCharWithParity('S', parityTypeChk, txRingBufPtr);	//0x53
						commEcrSendStatus(txRingBufPtr);								//send 2 bytes
						commSendCharWithParity(0x0d, parityTypeChk, txRingBufPtr);	//CR-0x0d
						commSendCharWithParity(ETX, parityTypeChk, txRingBufPtr);	//0x03
					}
					commState = 0;
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
*           4> Scale  -- data blocks --> PC
*           "data blocks"는 다음과 같다.
*           Data Block : SOH STX STA SIGN W5 W4 W3 W2 W1 W0 UN1 UN0 BCC ETX EOT
********************************************************************************
*/
void commEcrType6(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr)
{
	char byte;
	INT8U error;

	if (RingBufCheckBuf(rxRingBufPtr))
	{
		byte = RingBufGetCharWithParity(rxRingBufPtr, parityTypeChk, &error);
		if (error == 1)
		{
			commState = 0;
			return;
		}		
		switch (commState)
		{
			case 0:
				if (byte == ENQ)
				{
					commSendCharWithParity(ACK, parityTypeChk, txRingBufPtr);
					commState++;
				}
				else if (byte == ESC) //for Factory
				{
					commState = 200;
				}
				break;
			
			case 1:
				if(byte == DC1)
				{
#ifdef USE_SEND_SAME_WEIGHT_ONCE
					if (isSamePreviousWeight() != SAME_WEIGHT)
#endif
					{
						commSendCharWithParity(SOH, parityTypeChk, txRingBufPtr);
						commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);
						commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk);
						commSendCharWithParity(ETX, parityTypeChk, txRingBufPtr);
#ifdef USE_2BYPE_CHECKSUM_PROTOCOL
						comm2byteChecksum(txRingBufPtr, 13, parityTypeChk);
						commSendCharWithParity(0x0D, parityTypeChk, txRingBufPtr);
						commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
#else					
						commSendCharWithParity(EOT, parityTypeChk, txRingBufPtr);
#endif
					}
				}
				commState = 0;
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
	
}

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
#ifdef USE_SEND_SAME_WEIGHT_ONCE
		if (isSamePreviousWeight() != SAME_WEIGHT)
#endif
		{
			commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk);
			commSendCharWithParity(0x0d, parityTypeChk, txRingBufPtr);
			commTransFlag = 0;
		}		 
	}
}
#endif

/**
********************************************************************************
* @brief    ER communication Interpreter 함수\n
* @param    rxRingBufPtr : 받은 데이터가 있는 Ring Buffer Pointer
*           txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   Protocol은 다음과 같다.
*           1> Scale <-- W(0x57) --  PC
*           2> Scale <-- CR(0x0D) -- PC
*           3> Scale  -- data blocks --> PC
*           "data blocks"는 다음과 같다.
*           Data Block : LF W3 W2 2E W1 W0 UN1 UN2 OD LF S 0 0 CR ETX
********************************************************************************
*/
void commEcrType8(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr)
{
	char byte;
	INT8U error;

	if (RingBufCheckBuf(rxRingBufPtr))
	{
		byte = RingBufGetCharWithParity(rxRingBufPtr, parityTypeChk, &error);
		if (error == 1)
		{
			commState = 0;
			return;
		}		
		switch (commState)
		{
			case 0:
				if (byte == 'W')
				{
					commState++;
				}
				else if (byte == ESC) //for Factory
				{
					commState = 200;
				}
				else
				{
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15 //csh 20120904
				}
				break;
			case 1:
				if(byte == 0x0d)	//CR
				{
#ifdef USE_SEND_SAME_WEIGHT_ONCE
					if (isSamePreviousWeight() != SAME_WEIGHT)
#endif
					{
//**************(ECRSendWeight에 조건 중복됨, "------kg" 로 변경)**********************************//
/*
					if (SaleData.weight < 0 || !AdData.stableFlag || AdData.overloadFlag) //csh 20120904
					{
						commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					}	
					else
					{
*/
						commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
			
						commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk);
	//wcm0816 code optimaization
	//					commSendCharWithParity('K', parityTypeChk, txRingBufPtr);
	//					commSendCharWithParity('G', parityTypeChk, txRingBufPtr);
						commSendCharWithParity(commWeightUnitStrTable[AdData.weightUnit][0], parityTypeChk, txRingBufPtr);
						commSendCharWithParity(commWeightUnitStrTable[AdData.weightUnit][1], parityTypeChk, txRingBufPtr);

						commSendCharWithParity(0x0d, parityTypeChk, txRingBufPtr);
						commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
						commSendCharWithParity('S', parityTypeChk, txRingBufPtr);
						commSendCharWithParity('0', parityTypeChk, txRingBufPtr);
						commSendCharWithParity('0', parityTypeChk, txRingBufPtr);
						commSendCharWithParity(0x0d, parityTypeChk, txRingBufPtr);
						commSendCharWithParity(ETX, parityTypeChk, txRingBufPtr);
					//}
					}
				}
				else
				{
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15 //csh 20120904
				}
				commState = 0;
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
	
}

/**
********************************************************************************
* @brief    ER communication Interpreter 함수\n
* @param    rxRingBufPtr : 받은 데이터가 있는 Ring Buffer Pointer
*           txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   Protocol은 다음과 같다.
*           1> Scale <-- ETX(0x03) --  PC
*           2> Scale  -- ACK(0x06) --> PC
*           3> Scale <-- ENQ(0x05)   --  PC
*           4> Scale  -- data blocks --> PC
*           "data blocks"는 다음과 같다.
*           Data Block : STX 0 2 ESC [3 or 0] ESC W4 W3 W2 W1 W0 ESC
*						 U4 U3 U2 U1 U0 ESC P5 P4 P3 P2 P1 P0 ETX
*			@@ TYPE 9의 BAUD RATE는 2400임
********************************************************************************
*/
#if !defined (USE_SLOVAKIA_PROTOCOL) && !defined (COUNTRY_CZECH)
void commEcrType9(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr)
{
	char byte;

	if (RingBufCheckBuf(rxRingBufPtr))
	{
		byte = RingBufGetChar(rxRingBufPtr);
		switch (commState)
		{
			case 0:
				if (byte == ETX)
				{
					RingBufPutChar(txRingBufPtr, ACK);
					commState++;
				}
				else if (byte == ESC) //for Factory
				{
					commState = 200;
				}
				else
				{
					RingBufPutChar(txRingBufPtr, NAK);	//0x15 //csh 20120904
				}
				break;
			case 1:
				if(byte == ENQ)
				{
					if (SaleData.weight < 0 || !AdData.stableFlag) //csh 20120904
					{
						commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					}	
					else
					{
#ifdef USE_SEND_SAME_WEIGHT_ONCE
						if (isSamePreviousWeight() != SAME_WEIGHT)
#endif
						{
							commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);
							commSendCharWithParity('0', parityTypeChk, txRingBufPtr);
							commSendCharWithParity('2', parityTypeChk, txRingBufPtr);
							commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
		//wcm0816 code optimaization
		/*
							if(AdData.weightUnit == UNIT_KG) commSendCharWithParity('3', parityTypeChk, txRingBufPtr);
							else if(AdData.weightUnit == UNIT_LB) commSendCharWithParity('0', parityTypeChk, txRingBufPtr);
							else if(AdData.weightUnit == UNIT_OZ) commSendCharWithParity('1', parityTypeChk, txRingBufPtr);
							else if(AdData.weightUnit == UNIT_GRAM) commSendCharWithParity('2', parityTypeChk, txRingBufPtr);
		*/
							byte = (AdData.weightUnit + 3) % 4 + '0';
							commSendCharWithParity(byte, parityTypeChk, txRingBufPtr);

							commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
							commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk);
							commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);

							commEcrSendPrice(txRingBufPtr, UNIT_PRICE, parityTypeChk, 6);
							commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
							
							commEcrSendPrice(txRingBufPtr, TOTAL_PRICE, parityTypeChk, 6);
							commSendCharWithParity(ETX, parityTypeChk, txRingBufPtr);
						}				
					}
				}
				else
				{
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15 //csh 20120904
				}
				commState = 0;
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
}
#endif
/**
********************************************************************************
* @brief    ER communication Interpreter 함수\n
* @param    rxRingBufPtr : 받은 데이터가 있는 Ring Buffer Pointer
*           txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   Protocol은 다음과 같다.
*           1> Scale <-- EOT STX "data blocks" ETX --  PC
*			"data blocks"는 다음과 같다.
*			 : 0 5 ESC U5 U4 U3 U2 U1 U0 ESC T3 T2 T1 T0 ESC TEXT13 BYTE [TXT12 ~ TXT0] ETX 
*           2> Scale  -- ACK(0x06) --> PC
*           3> Scale <-- EOT ENQ   --  PC
*			4> Scale  -- STX "data blocks" ETX --> PC
*           "data blocks"는 다음과 같다.
*            : 0 2 ESC STATUS ESC W4 W3 W2 W1 W0 ESC U5 U4 U3 U2 U1 U0 ESC P5 P4 P3 P2 P1 P0
*			STATUS는 다음과 같다.
*			Unit이 KG 			: 3
*			Unit이 LB(d=0.01) 	: 1
*			Unit이 LB(d=0.005)	: 2 
********************************************************************************
*/
/*
void commEcrType11(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr)
{
	char byte, i;
	INT8U	txBufArray[10];
	INT32U	rcvdata_price, rcvdata_tare;
	
	while(RingBufCheckBuf(rxRingBufPtr))
	{
		byte = RingBufGetChar(rxRingBufPtr);
		switch (commState)
		{
			case 0:
				if (byte == EOT)
				{
					commState++;
				}
				else if (byte == ESC) //for Factory
				{
					commState = 200;
				}
				break;
			case 1:
				if(byte == STX)
				{
 					commState = 10;
				}
				else if(byte == ENQ)
				{
					commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);
					commSendCharWithParity('0', parityTypeChk, txRingBufPtr);
					commSendCharWithParity('2', parityTypeChk, txRingBufPtr);
					commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
					if(AdData.weightUnit == UNIT_KG) 
					{
						commSendCharWithParity('3', parityTypeChk, txRingBufPtr);
					}
					else if(AdData.weightUnit == UNIT_LB)
					{
						if(AdData.maxCapa == 15)
						{
							commSendCharWithParity('2', parityTypeChk, txRingBufPtr);
						}
						else
						{
							commSendCharWithParity('1', parityTypeChk, txRingBufPtr);
						}
					}

					commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
					
					commEcrSendWeight(txRingBufPtr, SaleData.weightUnit, parityTypeChk);
					
					commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
					
					commEcrSendPrice(txRingBufPtr, UNIT_PRICE, parityTypeChk, 6);
					
					commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
					
					commEcrSendPrice(txRingBufPtr, TOTAL_PRICE, parityTypeChk, 6);
					
					commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
					
					commSendCharWithParity(ETX, parityTypeChk, txRingBufPtr);
					
					commState = 0;
				}
				break;
			case 10:
				if(byte == 0x30)
				{
					commState = 30;
				}
				else
				{
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					commState = 0;
				}
				break;
			case 30:
				if(byte == 0x35)
				{
					commState = 31;
				}
				else
				{
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					commState = 0;
				}
				break;
			case 31:
				if (byte == ESC) commState = 32;
				else 
				{					
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					commState = 0;
				}
				break;
			case 32:
				txBufArray[0] = byte;
				commState = 33;					
				break;
			case 33:
				txBufArray[1] = byte;
				commState = 34;					
				break;
			case 34:
				txBufArray[2] = byte;
				commState = 35;					
				break;
			case 35:
				txBufArray[3] = byte;
				commState = 36;					
				break;
#ifdef	USE_DIALOG2_5DIGIT_PRICE
			case 36:
				txBufArray[4] = byte;
				commState = 38;					
				break;
			case 38:				
				if (byte == ESC) 
				{
					commState = 39;
				}
				else 
				{					
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					commState = 0;
				}
				break;
			case 39:
				txBufArray[5] = byte;
				commState = 40;					
				break;
			case 40:
				txBufArray[6] = byte;
				commState = 41;					
				break;
			case 41:
				txBufArray[7] = byte;
				commState = 42;					
				break;
			case 42:
				txBufArray[8] = byte;
				commState = 43;					
				break;	
#else
			case 36:
				txBufArray[4] = byte;
				commState = 37;					
				break;
			case 37:
				txBufArray[5] = byte;
				commState = 38;					
				break;
			case 38:				
				if (byte == ESC) 
				{
					commState = 39;
				}
				else 
				{					
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					commState = 0;
				}
				break;
			case 39:
				txBufArray[6] = byte;
				commState = 40;					
				break;
			case 40:
				txBufArray[7] = byte;
				commState = 41;					
				break;
			case 41:
				txBufArray[8] = byte;
				commState = 42;					
				break;
			case 42:
				txBufArray[9] = byte;
				commState = 43;					
				break;
#endif

			case 43:
				if(byte == ESC)
				{
					commState = 44;
				}
				else
				{
					commState = 0;
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
				}
				break;
			case 44:
//				txBufArray[0] = byte;
				commState = 45;					
				break;
			case 45:
//				txBufArray[1] = byte;
				commState = 46;					
				break;
			case 46:
//				txBufArray[2] = byte;
				commState = 47;					
				break;
			case 47:
//				txBufArray[3] = byte;
				commState = 48;					
				break;
			case 48:
//				txBufArray[4] = byte;
				commState = 49;					
				break;
			case 49:
//				txBufArray[5] = byte;
				commState = 50;					
				break;
			case 50:
//				txBufArray[6] = byte;
				commState = 51;					
				break;
			case 51:
//				txBufArray[7] = byte;
				commState = 52;					
				break;
			case 52:
//				txBufArray[8] = byte;
				commState = 53;					
				break;
			case 53:
//				txBufArray[9] = byte;
				commState = 54;					
				break;
			case 54:
//				txBufArray[10] = byte;
				commState = 55;					
				break;
			case 55:
//				txBufArray[11] = byte;
				commState = 56;					
				break;
			case 56:
//				txBufArray[12] = byte;
				commState = 57;					
				break;
			case 57:				
				if(byte == ETX)
				{
					//SalePluNameCopyExt(txBufArray); 						
					commState = 0;							
					commSendCharWithParity(ACK, parityTypeChk, txRingBufPtr);
					
					rcvdata_price= 0;
					rcvdata_tare = 0;
#ifdef USE_DIALOG2_5DIGIT_PRICE
					for (i = 0; i < 5; i++)
					{
						rcvdata_price += (INT32U)(txBufArray[i]-0x30) * DecPowerConvTable[4 - i];
					}
					SaleData.unitPrice = rcvdata_price;

					for (i = 5; i < 9; i++)
					{
						rcvdata_tare += (INT32U)(txBufArray[i]-0x30) * DecPowerConvTable[3 - i];
					}
					AdSetPT(rcvdata_tare, 0);
#else
					for (i = 0; i < 6; i++)
					{
						rcvdata_price += (INT32U)(txBufArray[i]-0x30) * DecPowerConvTable[5 - i];
					}
					SaleData.unitPrice = rcvdata_price;
					
					for (i = 6; i < 10; i++)
					{
						rcvdata_tare += (INT32U)(txBufArray[i]-0x30) * DecPowerConvTable[3 + 6 - i];
					}
					AdSetPT(rcvdata_tare, 0);
#endif
				}
				else
				{							
					commState = 0;
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
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

}
*/
/**
********************************************************************************
* @brief    ER communication Interpreter 함수\n
* @param    rxRingBufPtr : 받은 데이터가 있는 Ring Buffer Pointer
*           txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   Protocol은 다음과 같다.
*           1> Scale <-- EOT STX "data blocks" ETX --  PC
*			"data blocks"는 다음과 같다.
*			 : 0 5 ESC U5 U4 U3 U2 U1 U0 ESC T3 T2 T1 T0 ESC TEXT13 BYTE [TXT12 ~ TXT0] ETX 
*           2> Scale  -- ACK(0x06) --> PC
*           3> Scale <-- EOT ENQ   --  PC
*			4> Scale  -- STX "data blocks" ETX --> PC
*           "data blocks"는 다음과 같다.
*            : 0 2 ESC STATUS ESC W4 W3 W2 W1 W0 ESC U5 U4 U3 U2 U1 U0 ESC P5 P4 P3 P2 P1 P0
*			STATUS는 다음과 같다.
*			Unit이 KG 			: 3
*			Unit이 LB(d=0.01) 	: 1
*			Unit이 LB(d=0.005)	: 2 
********************************************************************************
*/
#if defined(DIALOG6) || defined(USE_SATO_PRINTER_PROTOCOL)
void commEcrType12(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr)
{
	INT8U byte,i,count,error, S1, S0;
	static INT8U	txBufArray[10];
	static INT8U	rxTextArray[13];
	INT32U	rcvdata_price, rcvdata_tare;
	INT32U	keytarebuf;
	static INT8U	recordState, ref_Offset;
	char	sendByte;
#ifdef USE_SATO_PRINTER_PROTOCOL
	INT8U returnData;
#endif

	while(RingBufCheckBuf(rxRingBufPtr))
	{
		byte = RingBufGetCharWithParity(rxRingBufPtr, parityTypeChk, &error);
		if (error == 1)
		{
			responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_02;//Type12_StsInfo
			commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
			commState = 0;
			return;
		}		
		switch (commState)
		{
			case 0:
				if (byte == EOT)  //ox04
				{
					commState++;
				}
				else if (byte == ESC) //for Factory
				{
					commState = 200;
				}
				break;
			case 1:
				if(byte == STX) //0x02
				{
 					commState = 10;
				}
				else if(byte == ENQ)
				{
					//weight check-------------------
					//unstable?
					if(!AdData.stableFlag)
					{
						responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_20;//Type12_StsInfo
					}
					//min range?(해당 변수 없음)

					//underload or negative weight?
#ifdef USE_NOT_UNDERLOAD_ERROR
					else if(AdData.grossZeroFlag || AdData.netZeroFlag)
#else
					else if((SaleData.weight < 0)|| AdData.grossZeroFlag || AdData.netZeroFlag)//else if(AdData.underloadFlag || AdData.grossZeroFlag || AdData.netZeroFlag)
#endif
					{
						responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_31;//Type12_StsInfo
					}
					//overload?
					else if(AdData.overloadFlag)
					{
						responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_32;//Type12_StsInfo
					}
#ifdef COUNTRY_CZECH
					else if(SaleData.weight == recentlySendWeight || recentlySendWeight != 0)
					{
						responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_34;//Type12_StsInfo
					}
#endif
					else
					{
#ifdef USE_SEND_SAME_WEIGHT_ONCE
						if (isSamePreviousWeight() != SAME_WEIGHT)
#endif
						{
							responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_00;//Type12_StsInfo

							commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);
							commSendCharWithParity('0', parityTypeChk, txRingBufPtr);
							commSendCharWithParity('2', parityTypeChk, txRingBufPtr);
							commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
							if(AdData.weightUnit == UNIT_KG) 
							{
								commSendCharWithParity('3', parityTypeChk, txRingBufPtr);
							}
							else if(AdData.weightUnit == UNIT_LB)
							{
								/*if(AdData.maxCapa == 15)
								{
									commSendCharWithParity('2', parityTypeChk, txRingBufPtr);
								}
								else
								{
									commSendCharWithParity('1', parityTypeChk, txRingBufPtr);
								}*/
								commSendCharWithParity('0', parityTypeChk, txRingBufPtr);
							}
							else if(AdData.weightUnit == UNIT_OZ) 
							{
								commSendCharWithParity('1', parityTypeChk, txRingBufPtr);
							}
							else if(AdData.weightUnit == UNIT_GRAM) 
							{
								commSendCharWithParity('2', parityTypeChk, txRingBufPtr);
							}

							commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
							
							commEcrSendWeight(txRingBufPtr, SaleData.weightUnit, parityTypeChk);
							
							commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
							
							commEcrSendPrice(txRingBufPtr, UNIT_PRICE, parityTypeChk, 6);
							
							commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
							
							commEcrSendPrice(txRingBufPtr, TOTAL_PRICE, parityTypeChk, 6);
							
							//commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
							
							commSendCharWithParity(ETX, parityTypeChk, txRingBufPtr);

#ifdef USE_2BYPE_CHECKSUM_PROTOCOL
							comm2byteChecksum(txRingBufPtr, 26, parityTypeChk);
							commSendCharWithParity(0x0D, parityTypeChk, txRingBufPtr);
							commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);

#endif
#ifdef COUNTRY_CZECH
							recentlySendWeight = SaleData.weight;
#endif
						}
					}					
					
					if(responseDelayCtrl) commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);//Type12_StsInfo
					commState = 0;
				}
				break;
			case 10: //Recrod No. First byte
				if(byte == 0x30)
				{
					commState = 30;
					recordState = 0;
					for(i=0; i<sizeof(txBufArray); i++)
						txBufArray[i] = 0;
					for(i=0; i<sizeof(rxTextArray); i++)
						rxTextArray[i] = 0;
					/*
					memset(txBufArray,0,sizeof(txBufArray));
					memset(rxTextArray,0,sizeof(rxTextArray));
					*/
				}
#ifdef USE_SATO_PRINTER_PROTOCOL
				else if(byte == 0x32) // Record No. 2x
				{
					commState = 50;
					recordState = 0;
				}
#endif
				else if(byte == 0x33)
				{
					commState = 40;
				}
				else
				{
					responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_10;//Type12_StsInfo
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					commState = 0;
				}
				break;

			case 30: //Record No. Second byte, Record No : 0X
				recordState = byte - '0';
				if(recordState == 1)
				{
					commandState = 1;	// unitprice
					ref_Offset = 31;
				}
				else if(recordState == 3)
				{
					commandState = 3;	// unit price, tare value
					ref_Offset = 61;
				}
				else if(recordState == 4)
				{
					commandState = 4;	// unit price, PLU name
					ref_Offset = 91;
				}
				else if(recordState == 5)
				{
					commandState = 5;	// unit price, PLU name, tare value
					ref_Offset = 121;
				}
#ifdef USE_SATO_PRINTER_PROTOCOL
				else if(recordState == 6)
				{
					commandState = 6;	// set tare current weight
					ref_Offset = 81;
				}
				else if(recordState == 7)
				{
					commandState = 7;	// set zero
					ref_Offset = 151;
				}
#endif
				else if(recordState == 8)
				{
					commandState = 0;
					ref_Offset = 151;
				}
				else
				{
					responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_10;//Type12_StsInfo
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					commState = 0;
					break;
				}
				commState = ref_Offset;
				break;
			
			case 40: //Record No. Second byte
							recordState = byte - '0';
							if(recordState == 0)		  //Request Tare Value
							{
								ref_Offset = 151;
							}
							else if(recordState == 1)	  //Opos Set Zero
							{
								SetSaleZeroKey();
								SaleData.zeroErrFlag=0;
								if (AdSetZero() != ERR_NONE)
								{
									SaleData.zeroErrFlag=1;
								}
			//					AdProc(0, 0);
								
								if(SaleData.zeroErrFlag)									sendByte = 0x15;	//"Z", Zero
			//					if(AdData.grossZeroFlag || AdData.netZeroFlag)	sendByte = 0x06;	//"Z", Zero
								else											 sendByte = 0x06;	
								commSendCharWithParity(sendByte, parityTypeChk, txRingBufPtr);
									
								commState = 0;
								break;
							}
							else if(recordState == 2)	   //Request Status
							{
								
								if(AdData.tareWeight) sendByte = 0x4E;
								else sendByte = 0x20;
								commSendCharWithParity(sendByte, parityTypeChk, txRingBufPtr);
							 //STA1
								if(AdData.overloadFlag) sendByte = 0x4F;		//"O", Over weight
								else if(AdData.grossZeroFlag || AdData.netZeroFlag)  sendByte = 0x5A;	//"Z", Zero
								else if(AdData.rawSetZeroData < 0) sendByte = 0x4C;	//"L", Below Real Zero
								else {
									if(AdData.stableFlag) sendByte = 0x53;		//"S", Stable
									else sendByte = 0x55;		//"U", Unstable
									}
								commSendCharWithParity(sendByte, parityTypeChk, txRingBufPtr);
							//STA2
								if(SaleData.weight < 0) sendByte = 0x2D;
								else sendByte = 0x20;
								commSendCharWithParity(sendByte, parityTypeChk, txRingBufPtr);
							//STA3
								commSendCharWithParity(ETX, parityTypeChk, txRingBufPtr);
			
								commState = 0;
								break;
							}
							
							else
							{
								responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_10;//Type12_StsInfo
								commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
								commState = 0;
								break;
							}
							commState = ref_Offset;
							break;
#ifdef USE_SATO_PRINTER_PROTOCOL
			case 50: //Record No. Second byte, Record No : 2X
				if(byte == 0x35) // Record No. 25
				{
					recordState = 25;
				}
				else
				{
					responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_10;//Type12_StsInfo
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					commState = 0;
					break;
				}
				commState++;
			break;
#endif
#ifdef USE_WEIGHT_ONLY_ECR

//ESC
			case 31:
			case 91:	
			case 98:
				responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_01;//Type12_StsInfo
				commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);				
				commState = 0;
				break;
				
			case 61:	
			case 68:
			case 121:
			case 128:
			case 133:
				if (byte == ESC) //0x1B
				{
					if(commState == 98) ref_Offset = 99;
					else if(commState == 133) ref_Offset = 134;

					commState++;
				}
				else 
				{					
					responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_02;//Type12_StsInfo
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					commState = 0;
				}
				break;
#else
//ESC
			case 31:
			case 61:	
			case 68:
			case 81:
			case 91:	
			case 98:
			case 121:
			case 128:
			case 133:
				if (byte == ESC) //0x1B
				{
					if(commState == 98) ref_Offset = 99;
					else if(commState == 133) ref_Offset = 134;

					commState++;
				}
				else 
				{					
					responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_02;//Type12_StsInfo
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					commState = 0;
				}
				break;
#endif 

//Unit Price
			case 32://ref. offset 31 -> 0
			case 33:
			case 34:
			case 35:
			case 36:
			case 37:

			case 62://ref. offset 61 -> 0
			case 63:
			case 64:
			case 65:
			case 66:
			case 67:

			case 82://ref. offset 82 -> 0

			case 92://ref. offset 91 -> 0
			case 93:
			case 94:
			case 95:
			case 96:
			case 97:

			case 122://ref. offset 121 -> 0
			case 123:
			case 124:
			case 125:
			case 126:
			case 127:
				if(isNum(byte))
				{
					txBufArray[commState-ref_Offset-1] = byte;
					commState++;
				}
				else
				{
					responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_11;//Type12_StsInfo
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					commState = 0;
				}
				break;
//Tare
			case 69://ref offset 61 -> 6
			case 70:		
			case 71:		
			case 72:		

			case 129://ref offset 121 -> 6
			case 130:		
			case 131:		
			case 132:		
				if(isNum(byte))
				{
					txBufArray[commState - ref_Offset - 2] = byte;
					commState++;
				}
				else
				{
					responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_11;//Type12_StsInfo
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					commState = 0;
				}
				break;
//Text for PLU name
			case 99://ref. offset 91 -> 0
			case 100:
			case 101:
			case 102:
			case 103:
			case 104:
			case 105:
			case 106:
			case 107:
			case 108:
			case 109:
			case 110:
			case 111:

			case 134://ref. offset 121 -> 0
			case 135:
			case 136:
			case 137:
			case 138:
			case 139:
			case 140:
			case 141:
			case 142:
			case 143:
			case 144:
			case 145:
			case 146:
				if(byte >= 0x20)
				{
					rxTextArray[commState - ref_Offset] = byte;
					commState++;
				}
				else
				{
					responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_13;//Type12_StsInfo
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					commState = 0;
				}
				break;			
//ETX 
			case 38:// Dialog 6 (Unit Price) : EOT STX '0' '1' 'ESC' U5 U4 U3 U2 U1 U0 'ESC' 'EXT' ,
				if(byte == ESC)//Dialog 6 protocol에 존재하지만, 'ESC' 구현이 누락되어서 추가함.
					break;
			case 51:
			case 73:
			case 83:
			case 112:
			case 147:
			case 151:
				if (byte == ETX)
				{
					if(recordState == 8)
					{
						commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);
						commSendCharWithParity('0', parityTypeChk, txRingBufPtr);
						commSendCharWithParity('9', parityTypeChk, txRingBufPtr);
						commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);

						switch(responseDelayCtrl)//Type12_StsInfo
						{
#ifdef USE_WEIGHT_ONLY_ECR
							case ECR_TYPE12_STATUS_INFORM_01:
								S1 = '0';
								S0 = '1';
								break;
#endif
							case ECR_TYPE12_STATUS_INFORM_02:
								S1 = '0';
								S0 = '2';
								break;
							case ECR_TYPE12_STATUS_INFORM_10:
								S1 = '1';
								S0 = '0';
								break;
							case ECR_TYPE12_STATUS_INFORM_11:
								S1 = '1';
								S0 = '1';
								break;
							case ECR_TYPE12_STATUS_INFORM_12:
								S1 = '1';
								S0 = '2';
								break;
							case ECR_TYPE12_STATUS_INFORM_13:
								S1 = '1';
								S0 = '3';
								break;
							case ECR_TYPE12_STATUS_INFORM_20:
								S1 = '2';
								S0 = '0';
								break;
							case ECR_TYPE12_STATUS_INFORM_31:
								S1 = '3';
								S0 = '1';
								break;
							case ECR_TYPE12_STATUS_INFORM_32:
								S1 = '3';
								S0 = '2';
								break;
#ifdef USE_SATO_PRINTER_PROTOCOL
							case ECR_TYPE12_STATUS_INFORM_40:
								S1 = '4';
								S0 = '0';
								break;
							case ECR_TYPE12_STATUS_INFORM_42:
								S1 = '4';
								S0 = '2';
								break;
							case ECR_TYPE12_STATUS_INFORM_52:
								S1 = '5';
								S0 = '2';
								break;
#endif
							case ECR_TYPE12_STATUS_INFORM_00:
							default:
								S1 = '0';
								S0 = '0';
								break;
						}
						commSendCharWithParity(S1, parityTypeChk, txRingBufPtr);
						commSendCharWithParity(S0, parityTypeChk, txRingBufPtr);
						commSendCharWithParity(ETX, parityTypeChk, txRingBufPtr);

#ifdef USE_2BYPE_CHECKSUM_PROTOCOL
						comm2byteChecksum(txRingBufPtr, 7, parityTypeChk);
						commSendCharWithParity(0x0D, parityTypeChk, txRingBufPtr);
						commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);

#endif


						commState = 0;
						responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_00;//Type12_StsInfo
					}
					else if(recordState == 0)
					{
						commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
						commEcrSendTare(txRingBufPtr, SaleData.keyTare, parityTypeChk);
						commSendCharWithParity(ETX, parityTypeChk, txRingBufPtr);
						commState = 0;
					}
#ifdef USE_SATO_PRINTER_PROTOCOL
					else if(recordState == 6)// Record 06 : Set Tare current weight
					{
						if(txBufArray[0] == 0x30) // do not save tare value
						{
							if(RecordeTareflag) // tare renew is not allowed when 
								returnData = ERR_ALREADY_TARE;
							else
							{
								if(GetSaleTareClearFlag() == OFF)
									SetScaleTareClearFlag(ON);
								returnData = AdSetTare();
							}
						}
						else if(txBufArray[0] == 0x31) // save tare value
						{
							if(RecordeTareflag)
								returnData = ERR_ALREADY_TARE;
							else
							{
								if(GetSaleTareClearFlag() == ON)
									SetScaleTareClearFlag(OFF);
								returnData = AdSetTare();
							}
						}
						else if(txBufArray[0] == 0x32)  // clear tare value
						{
							if(GetSaleTareClearFlag() == ON)
								SetScaleTareClearFlag(OFF);
							returnData = AdSetTareClear();
							if(returnData == ERR_NONE)
								RecordeTareflag = OFF;
						}
						else
						{
							responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_10;//Type12_StsInfo
						}
						switch(returnData)
						{
							case ERR_NONE:
								responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_00;//Type12_StsInfo
								SetSaleTransactionWeightCheck(FALSE);
								break;
							case ERR_UNSTABLE:
								responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_20;//Type12_StsInfo
								break;
							case ERR_ALREADY_TARE:
								responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_42;//Type12_StsInfo
								break;
							case ERR_OVER_RANGE:
								responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_40;//Type12_StsInfo
								break;
							case ERR_NO_TARE:
							case ERR_TARE_TYPE:	
								responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_52;//Type12_StsInfo
								break;
						}
						if(returnData == ERR_NONE)
							sendByte = ACK;
						else
							sendByte = NAK;
						commSendCharWithParity(sendByte, parityTypeChk, txRingBufPtr);
						commState = 0;
					}
					else if(recordState == 7) // Record 07 : Set Zero
					{
						SaleData.zeroErrFlag=0;
						switch(AdSetZero())
						{
							case ERR_NONE:
								responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_00;//Type12_StsInfo
								break;
							case ERR_UNSTABLE:
								responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_20;//Type12_StsInfo
								SaleData.zeroErrFlag=1;
								break;
							case ERR_ALREADY_TARE:
								responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_42;//Type12_StsInfo
								SaleData.zeroErrFlag=1;
								break;
							case ERR_OVER_RANGE:
								responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_40;//Type12_StsInfo
								SaleData.zeroErrFlag=1;
								break;
						}
						if(SaleData.zeroErrFlag)
							sendByte = NAK;
						else
							sendByte = ACK;	
						commSendCharWithParity(sendByte, parityTypeChk, txRingBufPtr);
						commState = 0;
						break;
					}
					else if(recordState == 25) // Record 25 : Remote Print-Key
					{
						// sendig dlp protocol
						sendDLP50Protocol(parityTypeChk, txRingBufPtr);
						commState = 0;
						break;
					}
#endif
					else
					{
						commState = 0;							
						rcvdata_price= 0;
						rcvdata_tare= 0;					
						//unitprice save
						for (i = 0; i < 6; i++)
						{
							rcvdata_price += (INT32U)(txBufArray[i]-0x30) * DecPowerConvTable[5 - i];
						}
						//tare save(recode No. #3, #5)
						if(recordState != 1 && recordState !=4)
						{
							count = 0;
							for (i = 6; i < 10; i++)
							{
								rcvdata_tare += (INT32U)(txBufArray[i]-0x30) * DecPowerConvTable[3 - count];
								count++;
								if(i == 9)
								{
									keytarebuf =  rcvdata_tare;
								}
								else
								{
									keytarebuf = 0; 
								}
							}
						}
						//tare Check?
						if(AdCheckPTInterval(rcvdata_tare))
						{
							responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_12;//Type12_StsInfo
							commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
							commState = 0;
							break;
						}
						//everything is OK.	
						//commSendCharWithParity(ACK, parityTypeChk, txRingBufPtr); 데이터 처리후 응답을 보내는 것으로 수정
						//SalePluDataCopyExt(rxTextArray,rcvdata_price,rcvdata_tare); 
#ifdef USE_WEIGHT_ONLY_ECR
						//if(NoUseTarekey  == 0) AdSetPT(rcvdata_tare,1);
						AdSetPT(rcvdata_tare,1);
#else
						//receivedAllPluDataApply(rxTextArray,rcvdata_price,rcvdata_tare); 
#if !defined(COUNTRY_CZECH) && !defined(COUNTRY_FRANCE) && !defined(USE_SATO_DEFAULT)
						receivedPluDataApply(rxTextArray,rcvdata_price,rcvdata_tare);
#endif
						if(commandState == 1)
						{
							//receivedUnitPriceDataApply(rcvdata_price);
							receivedPluDataApply(0, rcvdata_price,0xFFFFFFFF);
						}
						if(commandState == 3)
						{
							//receivedUnitPriceDataApply(rcvdata_price);
							//receivedTareValueApply(rcvdata_tare);
							receivedPluDataApply(0, rcvdata_price, rcvdata_tare);
						}
						if(commandState == 4)
						{
							//receivedUnitPriceDataApply(rcvdata_price);
							//receivedPluNameApply(rxTextArray);
							receivedPluDataApply(rxTextArray, rcvdata_price, 0xFFFFFFFF);
						}
						if(commandState == 5)
						{
							//receivedUnitPriceDataApply(rcvdata_price);
							//receivedPluNameApply(rxTextArray);
							//receivedTareValueApply(rcvdata_tare);
							receivedPluDataApply(rxTextArray, rcvdata_price, rcvdata_tare);
						}
#endif
#ifdef USE_SATO_PRINTER_PROTOCOL
						if(commandState == 3 || commandState == 5 )
							RecordeTareflag = ON;
						else
							RecordeTareflag = OFF;
#endif
						//everything is OK.	
						commSendCharWithParity(ACK, parityTypeChk, txRingBufPtr); //데이터 반영 후 ACK 응답 보냄.
					}
				}
				else 
				{					
					responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_02;//Type12_StsInfo
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					commState = 0;
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
}

#ifndef USE_ECR_12_ONLY
/**
********************************************************************************
* @brief    ER communication Interpreter 함수\n
* @param    rxRingBufPtr : 받은 데이터가 있는 Ring Buffer Pointer
*           txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   Protocol은 다음과 같다.
*           1> Scale <-- EOT STX "data blocks" ETX --  PC
*			"data blocks"는 다음과 같다.
*			 : 0 5 ESC U5 U4 U3 U2 U1 U0 ESC T3 T2 T1 T0 ESC TEXT13 BYTE [TXT12 ~ TXT0] ETX 
*           2> Scale  -- ACK(0x06) --> PC
*           3> Scale <-- EOT ENQ   --  PC
*			4> Scale  -- STX "data blocks" ETX --> PC
*           "data blocks"는 다음과 같다.
*            : 0 2 ESC STATUS ESC W4 W3 W2 W1 W0 ESC U5 U4 U3 U2 U1 U0 ESC P5 P4 P3 P2 P1 P0
*			STATUS는 다음과 같다.
*			Unit이 KG 			: 3
*			Unit이 LB(d=0.01) 	: 1
*			Unit이 LB(d=0.005)	: 2 
********************************************************************************
*/
void commEcrType13(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr)
{
	char byte;
	while(RingBufCheckBuf(rxRingBufPtr))
	{
		byte = RingBufGetChar(rxRingBufPtr);
		switch (commState)
		{
			case 0:
				if (byte == ENQ && AdData.stableFlag)
				{
					if (SaleData.weight < 0) //csh 20120904
					{
						commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					}	
					else
					{
#ifdef USE_SEND_SAME_WEIGHT_ONCE
						if (isSamePreviousWeight() != SAME_WEIGHT)
#endif
						{
							commEcrSendWeight(txRingBufPtr, SaleData.weightUnit, parityTypeChk);
							commSendCharWithParity(RS, parityTypeChk, txRingBufPtr);
						}						
					}
					commState =0;
				}
				else if (byte == ESC) //for Factory
				{
					commState = 200;
				}
				else
				{
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					commState = 0;
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
}
/**
********************************************************************************
* @brief    ER communication Interpreter 함수\n
* @param    rxRingBufPtr : 받은 데이터가 있는 Ring Buffer Pointer
*           txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   Protocol은 다음과 같다.
*			1> Scale  -- "data blocks" RS --> PC
*           "data blocks"는 다음과 같다.
*            : W4 W3 W2 W1 W0 <S>
*			<S>;STATUS는 다음과 같다.
*			Over weight			: 'O'
*			Lower weight	 	: '-'
*			Stable				: 'S' 
*			Unstable			: 'U' 
********************************************************************************
*/
void commEcrType14(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr)
{
	char byte;

	if(commTransFlag == 0)
	{
		if(AdData.stableFlag && SaleData.weight == 0) commTransFlag = 1;
	}
	else
	{
		if(commState == 0 && RingBufCheckBuf(rxRingBufPtr))
		{
			byte = RingBufGetChar(rxRingBufPtr);

			if(byte == ESC) commState = 200;
		}
		else if(commState == 200)
		{
			if (!CommOmInterpreter(commTxRingBufPtr, byte)) 
			{
				commState = 0;
			}
		}
		else
		{
			if(responseDelayCtrl == 0)
			{
				commEcrSendWeight(txRingBufPtr, SaleData.weightUnit, parityTypeChk);
			}

			responseDelayCtrl++;
			responseDelayCtrl %= STREAM_MODE_RESPONSE_DELAY;
		}
	}
}
#endif
/**
********************************************************************************
* @brief    ER communication Interpreter 함수\n
* @param    rxRingBufPtr : 받은 데이터가 있는 Ring Buffer Pointer
*           txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   Protocol은 다음과 같다.
*			1> Scale  -- "data blocks" RS --> PC
*           "data blocks"는 다음과 같다.
*            : W4 W3 W2 W1 W0 <S>
*			<S>;STATUS는 다음과 같다.
*			Over weight			: 'O'
*			Lower weight	 	: '-'
*			Stable				: 'S' 
*			Unstable			: 'U' 
********************************************************************************
*/
#ifdef USE_SLOVAKIA_PROTOCOL
void commEcrType16(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr)
{
	if(saleSendSKProtocolflag)
	{		
  #ifdef USE_SEND_SAME_WEIGHT_ONCE
		if (isSamePreviousWeight() != SAME_WEIGHT)
  #endif
		{
			commEcrSendWeight(txRingBufPtr, SaleData.weightUnit, parityTypeChk);
			saleSendSKProtocolflag = 0;
		}
	}
}
#endif
/**
********************************************************************************
* @brief    ER communication Interpreter 함수\n
* @param    rxRingBufPtr : 받은 데이터가 있는 Ring Buffer Pointer
*           txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   Anker Protocol은 다음과 같다.
*			1> Scale  <--   STX "data blocks" ETX  --- PC
*				"data blocks"는 다음과 같다.
*				: 0 1 ESC U4 U3 U2 U1 U0 
*			2> Scale  ---        ACK(0x06)         --> PC
*			3> Scale  <--        ENQ(0x05)         --- PC 
*			4> Scale  ---   STX "data blocks" ETX  --> PC
*			"data blocks"는 다음과 같다.
*				: 0 2 ESC STA ESC W4 W3 W2 W1 W0 ESC U4 U3 U2 U1 U0 ESC 
*				  U4 U3 U2 U1 U0 ESC P5 P4 P3 P2 P1 P0 ETX
********************************************************************************
*/
#if !defined(USE_WEIGHT_ONLY_ECR) && defined(USE_ECR_ANKER_PROTOCOL)
void commEcrType17(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr)
{
	char byte;
	INT8U error;
	INT8U sta, i;
	static INT8U txBufArray[10];
	INT32U rcvdata_price;
		
	while (RingBufCheckBuf(rxRingBufPtr))
	{
		byte = RingBufGetCharWithParity(rxRingBufPtr, parityTypeChk, &error);
		if (error == 1)
		{
			commState = 0;
			return;
		}		
		switch (commState)
		{
			case 0:
				if (byte == ENQ && AdData.stableFlag)
				{
#ifdef USE_SEND_SAME_WEIGHT_ONCE
					if (isSamePreviousWeight() != SAME_WEIGHT)
#endif
					{
						commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);
						commSendCharWithParity('0', parityTypeChk, txRingBufPtr);
						commSendCharWithParity('2', parityTypeChk, txRingBufPtr);
						commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);

						sta = (AdData.weightUnit + 3) % 4 + '0';
						commSendCharWithParity(sta, parityTypeChk, txRingBufPtr);
						commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
						
						commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk);
						commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);

						commEcrSendPrice(txRingBufPtr, UNIT_PRICE, parityTypeChk, 5);
						commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
							
						commEcrSendPrice(txRingBufPtr, TOTAL_PRICE, parityTypeChk, 6);
						commSendCharWithParity(ETX, parityTypeChk, txRingBufPtr);
					}
					commState = 0;
				}
				else if (byte == STX)
					commState++;
				else if (byte == ESC)
					commState = 200;
				else
				{
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15
					commState = 0;
				}
				break;
			case 1:
				if (byte == '0')
					commState ++;
				else
				{
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15
					commState = 0;
				}
				break;
			case 2:
				if (byte == '1')
					commState = 10;
				else
				{
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15
					commState = 0;
				}
				break;
			case 10:
				if (byte == ESC)
					commState = 20; //UnitPrice
				else
				{
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15
					commState = 0;
				}
				break;
			case 20:
			case 21:
			case 22:
			case 23:
			case 24:		
				if(isNum(byte))
				{
					txBufArray[commState - 20] = byte;
					commState++;
				}
				else
				{
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					commState = 0;
				}
				break;
			case 25:
				if (byte == ETX)
				{
					rcvdata_price = 0;
					for (i = 0; i < 5; i++)
					{
						rcvdata_price += (INT32U)(txBufArray[i] - '0') * DecPowerConvTable[4 - i];
					}

					receivedPluDataApply(0, rcvdata_price,0xFFFFFFFF);

					if (SaleData.unitPrice == rcvdata_price)
					{
						commSendCharWithParity(ACK, parityTypeChk, txRingBufPtr);
						commState = 0;
					}
					else
					{
						commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15
						commState = 0;
					}
				}
				else
				{
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15
					commState = 0;
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

}
#endif

/**
********************************************************************************
* @brief    ECR Send Tare value 함수\n
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   STX P7 P6 P5 P4 P3 P2 P1 P0 BCC ETX
*           P7 ~ P0 -> Total Price "   10000", if overload, "FFFFFFFF"
*           BCC -> Block check character exclusive or(^)
********************************************************************************
*/
void commEcrSendTare(RING_BUF *txRingBufPtr, INT32U tareType, INT8U parityType)
{
	char strBuf[4];
	INT8U i;


	Int32sToDecStr(strBuf, SaleData.keyTare, 4, '0', 0, 0, 0);
	//Int32sToDecStr(strBuf, tareBuf, 4, '0', 0, 0, 0);
	
	for (i = 0; i < 4; i++)
	{
		if (strBuf[i] == '\0') break;
		commSendCharWithParity(strBuf[i], parityType, txRingBufPtr);
	}
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

#ifdef USE_2BYPE_CHECKSUM_PROTOCOL
void comm2byteChecksum(RING_BUF *txRingBufPtr, INT8U size, INT8U parityType) 
{
	INT8U i, checksumData, out;
	INT16U sumData;

	
	sumData = 0;
	out = 0;
	for(i = 0; i < size; i++)
	{	
		if ((*txRingBufPtr).OutPtr + i >= (*txRingBufPtr).Size)
			out = (*txRingBufPtr).OutPtr + i - (*txRingBufPtr).Size;
		else
			out = (*txRingBufPtr).OutPtr + i;
		
		checksumData = (*txRingBufPtr).Buf[out];

		if(checksumData != SOH)
			sumData += checksumData;
	}
	commSendCharWithParity((INT8U)((sumData >> 8) & 0x7F), parityTypeChk, txRingBufPtr);
	commSendCharWithParity((INT8U)(sumData & 0x007F), parityTypeChk, txRingBufPtr);
}
#endif

#ifdef USE_SEND_SAME_WEIGHT_ONCE
INT8U isSamePreviousWeight(void)
{
	if (checkSamePreviousWeight != SAME_WEIGHT)
	{
		checkSamePreviousWeight = SAME_WEIGHT;
		return DIFFERENCE_WEIGHT;
	}

		return SAME_WEIGHT;
}
#endif

#ifdef USE_SATO_PRINTER_PROTOCOL
void CheckBufAndSend(void)
{
	while (RingBufCountSize(commTxRingBufPtr) > (RingBufCheckMaxBufSize(commTxRingBufPtr)/2)); // MAX BUFF SIZE
	CommDrvProc();
}

void commStrWithParity(INT8U* str, INT8U parityType, RING_BUF *txRingBufPtr) 
{
	while(*str != NULL)
	{
		commSendCharWithParity(*str, parityTypeChk, txRingBufPtr);
		str++;
	}
	CheckBufAndSend();
}
/**
********************************************************************************
* @brief    Write Number Data to Printer Buffer with Parity\n
* @param    num : number data
* @return   none
* @remark   
********************************************************************************
*/
void commNumwithParity(INT32S num, INT8U size, char fillChar, INT8U decPos, INT8U decType, INT8U useThousandPt, INT8U parityType, RING_BUF *txRingBufPtr)
{
	char str[10];
	char decChar;

	if (size > 10)
	{
		return;
	}
	if (decType == DEC_TYPE_DOT)
	{
		decChar = '.';
	}
	else
	{
		decChar = ',';	//(decType == DEC_TYPE_COMMA)
	}

	Int32sToDecStr(str, num, size, fillChar, decPos, decChar, useThousandPt);
	commStrWithParity(str,parityTypeChk, txRingBufPtr);
	CheckBufAndSend();
}

/**
********************************************************************************
* @brief   DLP 프로토콜 전송 하는 하는 함수
* @param  none
* @return   none
* @remark   
********************************************************************************
*/
void sendDLP50Protocol(INT8U parityType, RING_BUF	*txRingBufPtr)
{
	INT8U PluName[16];

	// Label formant CMD
	commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
	commStrWithParity("FR", parityTypeChk, txRingBufPtr);
	commSendCharWithParity(0x22, parityTypeChk, txRingBufPtr); // " 
	commStrWithParity("ERW", parityTypeChk, txRingBufPtr);
	commSendCharWithParity(0x22, parityTypeChk, txRingBufPtr); // " 
	commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
	commSendCharWithParity(0x3F, parityTypeChk, txRingBufPtr); // " 
	commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);

	//V00 : weight
	commNumwithParity(SaleData.weight, 6, ' ', SaleData.weightDecPos, 0, 0, parityTypeChk, txRingBufPtr);
	commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
	//V01 : Weight Unit
	commSendCharWithParity(commWeightUnitSmallStrTable[AdData.weightUnit][0], parityTypeChk, txRingBufPtr);	
	commSendCharWithParity(commWeightUnitSmallStrTable[AdData.weightUnit][1], parityTypeChk, txRingBufPtr);
	commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
	//V02 : Unit Price
	commNumwithParity(SaleData.unitPrice, 7, ' ', SaleData.priceDecPos, 0, 0, parityTypeChk, txRingBufPtr);
	commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
	//V03 : Total Price
	commNumwithParity(SaleData.totalPrice, 7, ' ', SaleData.priceDecPos, 0, 0, parityTypeChk, txRingBufPtr);
	commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
	//V04 : Weight(Barcode)
	commNumwithParity(SaleData.weight, 5, '0', 0, 0, 0, parityTypeChk, txRingBufPtr);
	commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
	//V05 : Unit Price(Barcode)
	commNumwithParity(SaleData.unitPrice, 6, '0', 0, 0, 0, parityTypeChk, txRingBufPtr);
	commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
	//V06 : Total Price(Barcode)
	commNumwithParity(SaleData.totalPrice, 5, '0', 0, 0, 0, parityTypeChk, txRingBufPtr);
	commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
	//V07 : PLU Name
	//V08 : PLU Number
#ifdef USE_SALE_PRICE_EXTERNAL_PLU
	if(SaleData.pluNum)
	{
		if(SaleData.pluNum<=200)
		{
			//V07 : PLU Name
			getSaleDefaultName(PluName,0);
			if(PluName[0] != 0x00)
				commStrWithParity(PluName, parityTypeChk, txRingBufPtr);
			else 
			{
				getSaleDefaultName(PluName,1);
				commStrWithParity(PluName, parityTypeChk, txRingBufPtr);
			}
			commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);

			//V08 : PLU Number
			commNumwithParity(SaleData.pluNum, 3, ' ', 0, 0, 0, parityTypeChk, txRingBufPtr);
			commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
		}
		else
		{
			commStrWithParity("NO_PLU          ", parityTypeChk, txRingBufPtr);
			commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
			//V08 : PLU Number
			commNumwithParity(SaleData.pluNum, 3, ' ', 0, 0, 0, parityTypeChk, txRingBufPtr);
			commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
		}
		
	}
	else
	{
		commStrWithParity("NO_PLU          ", parityTypeChk, txRingBufPtr);
		commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
		//V08 : PLU Number
		commStrWithParity("   ", parityTypeChk, txRingBufPtr);
		commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
	}
	//V09 : unit Price unit
	commStrWithParity("$/", parityTypeChk, txRingBufPtr);
	commSendCharWithParity(commWeightUnitSmallStrTable[AdData.weightUnit][0], parityTypeChk, txRingBufPtr);	
	commSendCharWithParity(commWeightUnitSmallStrTable[AdData.weightUnit][1], parityTypeChk, txRingBufPtr);
	commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);

	//V10 : total price unit
	commStrWithParity("$", parityTypeChk, txRingBufPtr);
	commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);

	//V11 & V12 : Packed on date and Sell by date -> send 00.00.00
	commStrWithParity("00.00.00", parityTypeChk, txRingBufPtr);
	commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
	//V12
	commStrWithParity("00.00.00", parityTypeChk, txRingBufPtr);
	commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
	//V13 : Prefix(Barcode)	
	commNumwithParity(getSaleWeightPrefix(), 2, ' ', 0, 0, 0, parityTypeChk, txRingBufPtr);
	commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
	//V14 : itemCode(Barcode) send 00000
	commNumwithParity(0, 5, '0', 0, 0, 0, parityTypeChk, txRingBufPtr);
	commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
	//V15 : Tare Weight
	commNumwithParity(AdData.tareWeight, 6, ' ', SaleData.weightDecPos, 0, 0, parityTypeChk, txRingBufPtr);
	commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
#else
	//V09 : unit Price unit
	commStrWithParity("$/", parityTypeChk, txRingBufPtr);
	commSendCharWithParity(commWeightUnitSmallStrTable[AdData.weightUnit][0], parityTypeChk, txRingBufPtr);	
	commSendCharWithParity(commWeightUnitSmallStrTable[AdData.weightUnit][1], parityTypeChk, txRingBufPtr);
	commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);

	//V10 : total price unit
	commStrWithParity("$", parityTypeChk, txRingBufPtr);
	commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);

	//V11 : Tare Weight
	commNumwithParity(AdData.tareWeight, 6, ' ', SaleData.weightDecPos, 0, 0, parityTypeChk, txRingBufPtr);
	commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
#endif
	//End command
	commStrWithParity("P1,1", parityTypeChk, txRingBufPtr);
	commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
}


/**
********************************************************************************
* @brief    외부에서 sendDLP50Protocol 접근 하는 함수
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void sendDLP50withParity(void)
{
    sendDLP50Protocol(parityTypeChk, commTxRingBufPtr);
}
#endif//#ifdef USE_SATO_PRINTER_PROTOCOL
#endif//#ifdef USE_ECR_COMM_API

