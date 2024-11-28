/**
********************************************************************************
* Copyright (c) 2020 CAS
* @brief   Communication API Source File\n
* @file    CommApi_PDN_Ecr.c
* @version 1.0
* @date    2020/04/17
* @author  Lee Dong Won (chamchida)
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
#include "../Communication/PrtApi.h"
#ifdef USE_PDN_ECR_COMM_API
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
#ifdef USE_TURKEY_ECR_15_PROTOCOL
	#define STREAM_MODE_RESPONSE_DELAY		50
#else
	#define STREAM_MODE_RESPONSE_DELAY		10
#endif

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
#define PORT_TYPE_USB 						0 //used to the responseDelayCtrl variable for usb communication.
#define PORT_TYPE_RS232 						1 //used to the responseDelayCtrl variable for RS232 communication.

#define USE_TARE_KEY_PROTOCOL	250			// 'T' or 't'를 통신으로 Tare Key 잡는 기능
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
/** @brief USB ECR Type*/
static INT8U commUsbEcrType;
/** @brief RS232 ECR Type*/
static INT8U commRs232EcrType;
/** @brief Parity Type(UART 0)*/
static INT8U parityTypeChk0;
/** @brief Parity Type(UART 1)*/
static INT8U parityTypeChk1;

/** @brief ECR Number of weight digit(UART 0)*/
static INT8U commSendDigit0;
/** @brief ECR Number of weight digit(UART 1)*/
static INT8U commSendDigit1;

static INT32U commWeightBackup;
static BOOLEAN commTransFlag;
static BOOLEAN commInterLockFlag;
/** @brief Real time transaction delay at ECR Protocol type14 OR Dialog6 Status Information ECR Protocol type12*/
static INT8U responseDelayCtrl;//ECR type12에서 dialog6 Status information variable로 사용(Type12_StsInfo)
static INT8U responseDelayCtrl_usb;//usb와 rs232 responseDelayCtrl 사용 분리를 위한 변수
/** @brief ECR#12 command check 상태 변수 */
static INT8U commandState;
#ifdef USE_SLOVAKIA_PROTOCOL
extern BOOLEAN saleSendSKProtocolflag;
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
static void commEcrInterpreter(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType, INT8U commPortType);
static void commEcrSendWeight(RING_BUF *txRingBufPtr, INT8U weightUnit, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType);
static void commEcrSendPrice(RING_BUF *txRingBufPtr, INT8U priceType, INT8U parityTypeChk);
static void commEcrSendStatus(RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commEcrType);
static void commEcrType0(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType);
static void commEcrType2(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType);
static void commEcrType4(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType);
static void commEcrType6(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType);
#if !defined (USE_SLOVAKIA_PROTOCOL) && !defined (COUNTRY_CZECH)
static void commEcrType7(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType);
static void commEcrType8(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType);
static void commEcrType9(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType);
#else
static void commEcrType8(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType);
#endif
//static void commEcrType11(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
#ifdef DIALOG6
static void commEcrType12(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType, INT8U commPortType);
static void commEcrType13(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType);
static void commEcrType14(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType, INT8U commPortType);
static void commEcrSendTare(RING_BUF *txRingBufPtr, INT32U tareType, INT8U parityType);
#endif
#ifdef USE_SLOVAKIA_PROTOCOL
static void commEcrType16(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType);
#endif
#if !defined (USE_WEIGHT_ONLY_ECR) && !defined (USE_PDN_ECR) && defined(USE_ECR_ANKER_PROTOCOL)
static void commEcrType17(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType);
#endif
#ifdef USE_PD2_ECR
static void commEcrType33(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType);
static void commEcrType35(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType);
#endif
static void commEcrType36(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType);
static void commSendCharWithParity(INT8U sendData, INT8U parityType, RING_BUF *txRingBufPtr);
static INT8U commCheckCharWithParity(INT8U receiveData, INT8U parityType);
void comm2byteChecksum(RING_BUF *txRingBufPtr, INT8U size, INT8U parityType);
#ifdef USE_SMART_BT_PROTOCOL
static void commBtInterpreter(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
static void commSmartBtSendData(RING_BUF *txRingBufPtr, INT8U *sendData, INT8U len);
static void commSmartBtSendScaleInformation(RING_BUF *txRingBufPtr);
static void commSmartBtSendWeight(RING_BUF *txRingBufPtr);
static void commSmartBtSendTare(RING_BUF *txRingBufPtr);
static void commSmartBtSendPowerInformation(RING_BUF *txRingBufPtr);
static void commSmrtBtSetPresetTare(RING_BUF *txRingBufPtr, INT8U *rcvData);
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
	StorageWriteByte(STORAGE_EXT_PARAM_BASE_ADDR, ECR_TYPE_6); // RS232 ECR TYPE
	ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_1ST, 0);
	ParamWriteByte(PARAM_COMM_AREA_NUM,COMM_PARAM_OFFSET_SELECT, RS232_ECR); // USB ECR + RS232 ECR
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
#ifdef USE_PDN_ECR
	INT8U uart1Baudrate;
	INT8U printerType;
	INT8U commType;
	VERSION version;

	printerType = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH) & 0x03;
	commType = ParamReadByte(PARAM_COMM_AREA_NUM,COMM_PARAM_OFFSET_SELECT);

  #ifdef USE_PRINTER
  #else
  	if (printerType) // PRT_DEP50 or PRT_DLP50. PRT_NONE : 0;
  	{
  		printerType = PRT_NONE;
  		ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, printerType);
  	}

	if ((commType == RS232_DEP) || (commType == RS232_DLP))
	{
		commType = RS232_ECR; // default
		ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_SELECT, commType);
	}
  #endif

  // PDN - 국가셋팅 미국으로 설정 시 Tare Key 사용안함 / 
  	ParamReadData(PARAM_VERSION_AREA_NUM, 0, (INT8U *)&version, sizeof(version));
	if(version.countryNum == NTEP) Pd2ECRProtocol = 1;
	else Pd2ECRProtocol = 0;
	
#endif
	commUsbEcrType = ParamReadByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_ECR_TYPE);
	commRs232EcrType = StorageReadByte(STORAGE_EXT_PARAM_BASE_ADDR); // RS232 ECR TYPE
	commInterLockFlag = (BOOLEAN)(ParamReadByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_1ST) & 0x01);
	commTransFlag = 0;
	commWeightBackup = 0;
	// USB ECR Setting
	switch (commUsbEcrType)
	{
		case ECR_TYPE_0:
		case ECR_TYPE_1:
		case ECR_TYPE_3:
		case ECR_TYPE_8:
#if defined(COUNTRY_MEXICO)
			parityTypeChk0 = PARITY_TYPE_NONE;
			commSendDigit0 = 6;
			tempInt8u = BAUD_RATE_9600;
			break;
#elif defined(USE_DIALOG2_5DIGIT_PRICE)
			parityTypeChk0 = PARITY_TYPE_NONE;
			commSendDigit0 = 5;
			tempInt8u = BAUD_RATE_9600;
			break;
#else
			parityTypeChk0 = PARITY_TYPE_EVEN;
			commSendDigit0 = 5;
			tempInt8u = BAUD_RATE_9600;
			break;
#endif
		case ECR_TYPE_2:
#if defined(COUNTRY_MEXICO) ||  defined(USE_DIALOG2_5DIGIT_PRICE)
			parityTypeChk0 = PARITY_TYPE_NONE;
			commSendDigit0 = 6;
			tempInt8u = BAUD_RATE_9600;
			break;
#else
			parityTypeChk0 = PARITY_TYPE_EVEN;
#if defined(USE_WEIGHT_ONLY_ECR) || defined(USE_PDN_ECR)
			if(Pd2ECRProtocol == 1)
			{
				commSendDigit0 = 5;
			}
			else
			{
				commSendDigit0 = 6;
			}
#else
			commSendDigit0 = 6;
#endif
			tempInt8u = BAUD_RATE_9600;
			break;				
#endif
		case ECR_TYPE_4:
#if defined(COUNTRY_MEXICO) ||  defined(USE_DIALOG2_5DIGIT_PRICE)
			parityTypeChk0 = PARITY_TYPE_NONE;
			commSendDigit0 = 6;
			tempInt8u = BAUD_RATE_9600;
			break;
#endif
		case ECR_TYPE_5:
#ifdef USE_DIALOG2_5DIGIT_PRICE
			parityTypeChk0 = PARITY_TYPE_NONE;
			commSendDigit0 = 5;
			tempInt8u = BAUD_RATE_9600;
			break;
#else
			parityTypeChk0 = PARITY_TYPE_EVEN;
			commSendDigit0 = 6;
			tempInt8u = BAUD_RATE_9600;
			break;
#endif
		case ECR_TYPE_6:
		case ECR_TYPE_7:
		case ECR_TYPE_10:
			parityTypeChk0 = PARITY_TYPE_NONE;
			commSendDigit0 = 6;
			tempInt8u = BAUD_RATE_9600;
			break;
		case ECR_TYPE_9:
			parityTypeChk0 = PARITY_TYPE_NONE;
			commSendDigit0 = 5;
			tempInt8u = BAUD_RATE_2400;
			break;	
#ifdef DIALOG6
		case ECR_TYPE_12:
#ifdef USE_DIALOG2_5DIGIT_PRICE
			parityTypeChk0 = PARITY_TYPE_NONE;
#else
			parityTypeChk0 = PARITY_TYPE_ODD;
#endif
			commSendDigit0 = 5;
			tempInt8u = BAUD_RATE_9600;
			responseDelayCtrl_usb = ECR_TYPE12_STATUS_INFORM_00;//Type12_StsInfo
			break;
		case ECR_TYPE_13:
			parityTypeChk0 = PARITY_TYPE_NONE;
			commSendDigit0 = 5;
			tempInt8u = BAUD_RATE_9600;
			break;				
		case ECR_TYPE_14:		//wcm0112
		case ECR_TYPE_15:
			parityTypeChk0 = PARITY_TYPE_NONE;
			commSendDigit0 = 5;
			tempInt8u = BAUD_RATE_9600;
			//commTransFlag = 1;
			responseDelayCtrl_usb = 0;
			break;
#endif
#ifdef USE_SLOVAKIA_PROTOCOL
		case ECR_TYPE_16:
			parityTypeChk0 = PARITY_TYPE_NONE;
			commSendDigit0 = 5;
			tempInt8u = BAUD_RATE_9600;
			break;			
#endif
		case ECR_TYPE_17:
			parityTypeChk0 = PARITY_TYPE_NONE;
			commSendDigit0 = 5;
			tempInt8u = BAUD_RATE_9600;
			break;
		case ECR_TYPE_20:
			parityTypeChk0 = PARITY_TYPE_EVEN;
			commSendDigit0 = 5;
			tempInt8u = BAUD_RATE_9600;
			break;
#ifdef USE_PD2_ECR
		case ECR_TYPE_18:
		case ECR_TYPE_19:
			parityTypeChk0 = PARITY_TYPE_EVEN;
			commSendDigit0 = 5;
			tempInt8u = BAUD_RATE_9600;
			break;
		case ECR_TYPE_22:
		case ECR_TYPE_23:
			parityTypeChk0 = PARITY_TYPE_EVEN;
			commSendDigit0 = 5;
			tempInt8u = BAUD_RATE_9600;
			break;
		case ECR_TYPE_21:
		case ECR_TYPE_24:
		case ECR_TYPE_25:
		case ECR_TYPE_32:
			parityTypeChk0 = PARITY_TYPE_EVEN;
			commSendDigit0 = 6;
			tempInt8u = BAUD_RATE_9600;
			break;
		// Databit 8bit, Parity None
		case ECR_TYPE_26:
		case ECR_TYPE_27:
		case ECR_TYPE_28:
		case ECR_TYPE_31:
			parityTypeChk0 = PARITY_TYPE_NONE;
			commSendDigit0 = 6;
			tempInt8u = BAUD_RATE_9600;
			break;
		case ECR_TYPE_29:
		case ECR_TYPE_33:
			parityTypeChk0 = PARITY_TYPE_NONE;
			commSendDigit0 = 5;
			tempInt8u = BAUD_RATE_9600;
			break;
		case ECR_TYPE_30:
			parityTypeChk0 = PARITY_TYPE_NONE;
			commSendDigit0 = 7;
			tempInt8u = BAUD_RATE_9600;
			break;
		case ECR_TYPE_34:
			parityTypeChk0 = PARITY_TYPE_ODD;
			commSendDigit0 = 5;
			tempInt8u = BAUD_RATE_9600;
			break;
		case ECR_TYPE_35:
			parityTypeChk0 = PARITY_TYPE_ODD;
			commSendDigit0 = 7;
			tempInt8u = BAUD_RATE_2400;
			break;
#endif
		case ECR_TYPE_36:
		case ECR_TYPE_37:
			parityTypeChk0 = PARITY_TYPE_EVEN;
			commSendDigit0 = 5;
			tempInt8u = BAUD_RATE_9600;
			break;
		default :
			parityTypeChk0 = PARITY_TYPE_NONE;
			commSendDigit0 = 6;
			tempInt8u = BAUD_RATE_9600;
			break;
	}

	// RS232 ECR Setting
	switch (commRs232EcrType)
	{
		case ECR_TYPE_0:
		case ECR_TYPE_1:
		case ECR_TYPE_3:
		case ECR_TYPE_8:
#if defined(COUNTRY_MEXICO)
			parityTypeChk1 = PARITY_TYPE_NONE;
			commSendDigit1 = 6;
			uart1Baudrate = BAUD_RATE_9600;
			break;
#elif defined(USE_DIALOG2_5DIGIT_PRICE)
			parityTypeChk1 = PARITY_TYPE_NONE;
			commSendDigit1 = 5;
			uart1Baudrate = BAUD_RATE_9600;
			break;
#else
			parityTypeChk1 = PARITY_TYPE_EVEN;
			commSendDigit1 = 5;
			uart1Baudrate = BAUD_RATE_9600;
			break;
#endif
		case ECR_TYPE_2:
#if defined(COUNTRY_MEXICO) ||  defined(USE_DIALOG2_5DIGIT_PRICE)
			parityTypeChk1 = PARITY_TYPE_NONE;
			commSendDigit1 = 6;
			uart1Baudrate = BAUD_RATE_9600;
			break;
#else
			parityTypeChk1 = PARITY_TYPE_EVEN;
#if defined(USE_WEIGHT_ONLY_ECR) || defined(USE_PDN_ECR)
			if(Pd2ECRProtocol == 1)
			{
				commSendDigit1 = 5;
			}
			else
			{
				commSendDigit1 = 6;
			}
#else
			commSendDigit1 = 6;
#endif
			uart1Baudrate = BAUD_RATE_9600;
			break;				
#endif
		case ECR_TYPE_4:
#if defined(COUNTRY_MEXICO) ||  defined(USE_DIALOG2_5DIGIT_PRICE)
			parityTypeChk1 = PARITY_TYPE_NONE;
			commSendDigit1 = 6;
			uart1Baudrate = BAUD_RATE_9600;
			break;
#endif
		case ECR_TYPE_5:
#ifdef USE_DIALOG2_5DIGIT_PRICE
			parityTypeChk1 = PARITY_TYPE_NONE;
			commSendDigit1 = 5;
			uart1Baudrate = BAUD_RATE_9600;
			break;
#else
			parityTypeChk1 = PARITY_TYPE_EVEN;
			commSendDigit1 = 6;
			uart1Baudrate = BAUD_RATE_9600;
			break;
#endif
		case ECR_TYPE_6:
		case ECR_TYPE_7:
		case ECR_TYPE_10:
			parityTypeChk1 = PARITY_TYPE_NONE;
			commSendDigit1 = 6;
			uart1Baudrate = BAUD_RATE_9600;
			break;
		case ECR_TYPE_9:
			parityTypeChk1 = PARITY_TYPE_NONE;
			commSendDigit1 = 5;
			uart1Baudrate = BAUD_RATE_2400;
			break;
#ifdef DIALOG6
		case ECR_TYPE_12:
#ifdef USE_DIALOG2_5DIGIT_PRICE
			parityTypeChk1 = PARITY_TYPE_NONE;
#else
			parityTypeChk1 = PARITY_TYPE_ODD;
#endif
			commSendDigit1 = 5;
			uart1Baudrate = BAUD_RATE_9600;
			responseDelayCtrl = ECR_TYPE12_STATUS_INFORM_00;//Type12_StsInfo
			break;
		case ECR_TYPE_13:
			parityTypeChk1 = PARITY_TYPE_NONE;
			commSendDigit1 = 5;
			uart1Baudrate = BAUD_RATE_9600;
			break;				
		case ECR_TYPE_14:		//wcm0112
		case ECR_TYPE_15:
			parityTypeChk1 = PARITY_TYPE_NONE;
			commSendDigit1 = 5;
			uart1Baudrate = BAUD_RATE_9600;
			//commTransFlag = 1;
			responseDelayCtrl = 0;
			break;
#endif
#ifdef USE_SLOVAKIA_PROTOCOL
		case ECR_TYPE_16:
			parityTypeChk1 = PARITY_TYPE_NONE;
			commSendDigit1 = 5;
			uart1Baudrate = BAUD_RATE_9600;
			break;			
#endif
		case ECR_TYPE_17:
			parityTypeChk1 = PARITY_TYPE_NONE;
			commSendDigit1 = 5;
			uart1Baudrate = BAUD_RATE_9600;
			break;
		case ECR_TYPE_20:
			parityTypeChk1 = PARITY_TYPE_EVEN;
			commSendDigit1 = 5;
			uart1Baudrate = BAUD_RATE_9600;
			break;
#ifdef USE_PD2_ECR
		case ECR_TYPE_18:
		case ECR_TYPE_19:
			parityTypeChk1 = PARITY_TYPE_EVEN;
			commSendDigit1 = 5;
			uart1Baudrate = BAUD_RATE_9600;
			break;
		case ECR_TYPE_22:
		case ECR_TYPE_23:
			parityTypeChk1 = PARITY_TYPE_EVEN;
			commSendDigit1 = 5;
			uart1Baudrate = BAUD_RATE_9600;
			break;
		case ECR_TYPE_21:
		case ECR_TYPE_24:
		case ECR_TYPE_25:
		case ECR_TYPE_32:
			parityTypeChk1 = PARITY_TYPE_EVEN;
			commSendDigit1 = 6;
			uart1Baudrate = BAUD_RATE_9600;
			break;
		// Databit 8bit, Parity None
		case ECR_TYPE_26:
		case ECR_TYPE_27:
		case ECR_TYPE_28:
		case ECR_TYPE_31:
			parityTypeChk1 = PARITY_TYPE_NONE;
			commSendDigit1 = 6;
			uart1Baudrate = BAUD_RATE_9600;
			break;
		case ECR_TYPE_29:
		case ECR_TYPE_33:
			parityTypeChk1 = PARITY_TYPE_NONE;
			commSendDigit1 = 5;
			uart1Baudrate = BAUD_RATE_9600;
			break;
		case ECR_TYPE_30:
			parityTypeChk1 = PARITY_TYPE_NONE;
			commSendDigit1 = 7;
			uart1Baudrate = BAUD_RATE_9600;
			break;
		case ECR_TYPE_34:
			parityTypeChk1 = PARITY_TYPE_ODD;
			commSendDigit1 = 5;
			uart1Baudrate = BAUD_RATE_9600;
			break;
		case ECR_TYPE_35:
			parityTypeChk1 = PARITY_TYPE_ODD;
			commSendDigit1 = 7;
			uart1Baudrate = BAUD_RATE_2400;
			break;
#endif
		case ECR_TYPE_36:
		case ECR_TYPE_37:
			parityTypeChk1 = PARITY_TYPE_EVEN;
			commSendDigit1 = 5;
			tempInt8u = BAUD_RATE_9600;
			break;
		default :
			parityTypeChk1 = PARITY_TYPE_NONE;
			commSendDigit1 = 6;
			uart1Baudrate = BAUD_RATE_9600;
			break;
	}
#ifdef USE_PDN_ECR
	if (printerType == 0)
	{
		if (commType == RS232_BLE)
		{
			uart1Baudrate = BAUD_RATE_9600;
			CommDrvInit(tempInt8u, uart1Baudrate);
		}
		else // ECR
		{
			CommDrvInit(tempInt8u, uart1Baudrate);
		}
	}
	else // DEP, DLP
	{
		uart1Baudrate = BAUD_RATE_9600;
		CommDrvInit(tempInt8u, uart1Baudrate);
	}
#else
	CommDrvInit(tempInt8u);
#endif

	commRxRingBufPtr = &SerialRxRingBuf;
	commTxRingBufPtr = &SerialTxRingBuf;
#ifdef USE_UART1
	commRx1RingBufPtr = &SerialRx1RingBuf;
	commTx1RingBufPtr = &SerialTx1RingBuf;
#endif	

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
#ifdef USE_PDN_ECR
	INT8U commType;
	
	commType = ParamReadByte(PARAM_COMM_AREA_NUM,COMM_PARAM_OFFSET_SELECT);

#ifndef DIALOG6
	if (RingBufCheckBuf(commRxRingBufPtr) || (commTransFlag && AdData.stableFlag))
#else
	if (RingBufCheckBuf(commRxRingBufPtr) || (commTransFlag && AdData.stableFlag) || ((commUsbEcrType == ECR_TYPE_14) || (commUsbEcrType == ECR_TYPE_15)))
#endif
	{
		commEcrInterpreter(commRxRingBufPtr, commTxRingBufPtr, parityTypeChk0, commSendDigit0, commUsbEcrType, PORT_TYPE_USB);
	}
	
	if (commType == RS232_ECR)
	{
#ifndef DIALOG6
		if (RingBufCheckBuf(commRx1RingBufPtr) || (commTransFlag && AdData.stableFlag))
#else
		if (RingBufCheckBuf(commRx1RingBufPtr) || (commTransFlag && AdData.stableFlag) || ((commRs232EcrType == ECR_TYPE_14) || (commRs232EcrType == ECR_TYPE_15)))
#endif
		{
			commEcrInterpreter(commRx1RingBufPtr, commTx1RingBufPtr, parityTypeChk1, commSendDigit1, commRs232EcrType, PORT_TYPE_RS232);
		}
	}
	else if (commType == RS232_BLE)
	{
#ifdef USE_UART1
#ifdef USE_SMART_BT_PROTOCOL
		while (RingBufCheckBuf(commRx1RingBufPtr))
		{
			commBtInterpreter(commRx1RingBufPtr, commTx1RingBufPtr);
		}
#endif
#endif
	}
#else // USE_PDN_ECR
#ifndef DIALOG6
	if (RingBufCheckBuf(commRxRingBufPtr) || (commTransFlag && AdData.stableFlag))
#else
#ifndef USE_SLOVAKIA_PROTOCOL
	if (RingBufCheckBuf(commRxRingBufPtr) || (commTransFlag && AdData.stableFlag) || ((commEcrType == ECR_TYPE_14) || (commEcrType == ECR_TYPE_15)))
#else
	if (RingBufCheckBuf(commRxRingBufPtr) || (commTransFlag && AdData.stableFlag) || ((commEcrType == ECR_TYPE_14) || (commEcrType == ECR_TYPE_15) || (commEcrType == ECR_TYPE_16)))
#endif
#endif
	{
		commEcrInterpreter(commRxRingBufPtr, commTxRingBufPtr);
	}
#endif // USE_PDN_ECR
	CommDrvProc();
#ifdef USE_UART1
	Comm1DrvProc();
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
void commEcrSendWeight(RING_BUF *txRingBufPtr, INT8U weightUnit, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType)
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
#ifdef USE_PDN_ECR
	INT8U statusBuf[12];
#endif
	INT8U sendSize;

	bcc = 0;
	scaleTypeID = 0;
	switch(commEcrType)
	{
		case ECR_TYPE_0:	
		case ECR_TYPE_1:
#ifdef USE_PD2_ECR
		case ECR_TYPE_18:
		case ECR_TYPE_19:
#endif
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
#ifdef USE_PD2_ECR
			if (commEcrType == ECR_TYPE_19)
				Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, 0, 0, 0, 0);
			else
				Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', 0, 0, 0);
#else
			Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', 0, 0, 0);
#endif
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
#ifdef USE_PD2_ECR
		case ECR_TYPE_26:
		case ECR_TYPE_27:
		case ECR_TYPE_28:
#endif
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
			else if (AdData.overloadFlag)
			{
				sendByte1 = 'F'; 	//"F", Overflow(0x46)
			}
			else if (AdData.grossZeroFlag || AdData.netZeroFlag || SaleData.weight >= 0)
			{
				sendByte1 = ' ';		//"Space", Zero(0x20)
			}

			commSendCharWithParity(sendByte, parityTypeChk, txRingBufPtr);	
			commSendCharWithParity(sendByte1, parityTypeChk, txRingBufPtr);	
			
			bcc ^= sendByte;			
			bcc ^= sendByte1;
#ifdef USE_PD2_ECR
			if (commEcrType == ECR_TYPE_26)
			{
				Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', AdData.weightDecPos, '.', 0); // fillchar 0x30
			}
			else if (commEcrType == ECR_TYPE_27)
			{
				Int32sToDecStr(strBuf, SaleData.weight, commSendDigit, 0, AdData.weightDecPos, '.', 0); // fillchar 0x20, weight에 (-)포함
			}
			else if (commEcrType == ECR_TYPE_28)
			{
				if ((SaleData.weight < 0) || (AdData.overloadFlag)) MakeStrCopy(strBuf, "------", commSendDigit);
				else Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', AdData.weightDecPos, '.', 0); // fillchar 0x30
			}
			else
#endif
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
			commSendCharWithParity(commWeightUnitStrTable[weightUnit][0], parityTypeChk, txRingBufPtr);	
			commSendCharWithParity(commWeightUnitStrTable[weightUnit][1], parityTypeChk, txRingBufPtr);
			bcc ^= commWeightUnitStrTable[weightUnit][0];
			bcc ^= commWeightUnitStrTable[weightUnit][1];

#ifndef USE_2BYPE_CHECKSUM_PROTOCOL
			commSendCharWithParity(bcc, parityTypeChk, txRingBufPtr);
#endif
			break;
			
		case ECR_TYPE_2:
		case ECR_TYPE_37:
			Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', 0, 0, 0);

			for (i = 0; i < commSendDigit; i++)
			{
				commSendCharWithParity(strBuf[i], parityTypeChk, txRingBufPtr);
			}
			break;
			
		case ECR_TYPE_3:
		case ECR_TYPE_9:
		case ECR_TYPE_10:
			if(commEcrType == ECR_TYPE_10) Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', AdData.weightDecPos, '.', 0);
			else Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', 0, 0, 0);

			for (i = 0; i < commSendDigit; i++)
			{
				commSendCharWithParity(strBuf[i], parityTypeChk, txRingBufPtr);
			}
			break;
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
		case ECR_TYPE_13:
			Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', 0, 0, 0);

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
		case ECR_TYPE_14:
		case ECR_TYPE_15:
  #ifdef USE_TURKEY_ECR_15_PROTOCOL
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

			sendByte = commWeightUnitSmallStrTable[AdData.weightUnit][0];
			RingBufPutChar(txRingBufPtr, sendByte);

			sendByte = commWeightUnitSmallStrTable[AdData.weightUnit][1];
			RingBufPutChar(txRingBufPtr, sendByte);	
				
			RingBufPutChar(txRingBufPtr, ASCII_CR);
			RingBufPutChar(txRingBufPtr, LF);
			break;
  #else
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
#ifdef USE_PD2_ECR
		case ECR_TYPE_31:
//			if(commEcrType == ECR_TYPE_8) Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', 2, '.', 0);
//			else Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', AdData.weightDecPos, '.', 0);
			if(commEcrType == ECR_TYPE_4 || commEcrType == ECR_TYPE_5 || commEcrType == ECR_TYPE_31)
#else
			if(commEcrType == ECR_TYPE_4 || commEcrType == ECR_TYPE_5) 
#endif
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
		case ECR_TYPE_17:
		case ECR_TYPE_20:
			Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', 0, 0, 0);
			for (i = 0; i < commSendDigit; i++)
			{
				commSendCharWithParity(strBuf[i], parityTypeChk, txRingBufPtr);
			}
			break;
#ifdef USE_PD2_ECR
		// 소수점 출력, 빈 공간 0x30
		case ECR_TYPE_21:
		case ECR_TYPE_23:
		case ECR_TYPE_32:
			Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', AdData.weightDecPos, '.', 0);

			for (i = 0; i < commSendDigit; i++)
			{
				commSendCharWithParity(strBuf[i], parityTypeChk, txRingBufPtr);
			}
			break;

		// 소수점 출력X, 빈 공간 0x30
		case ECR_TYPE_22:
		case ECR_TYPE_29:
		case ECR_TYPE_33:
		case ECR_TYPE_34:
			Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', 0, 0, 0);
			
			for (i = 0; i < commSendDigit; i++)
			{
				commSendCharWithParity(strBuf[i], parityTypeChk, txRingBufPtr);
			}
			break;
		case ECR_TYPE_24:
		case ECR_TYPE_25:
			if(SaleData.weight < 0 || AdData.overloadFlag)
				Int32sToDecStr(strBuf, 0, commSendDigit, '0', AdData.weightDecPos, '.', 0);
			else
				Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', AdData.weightDecPos, '.', 0);
			
			for (i = 0; i < commSendDigit; i++)
			{
				commSendCharWithParity(strBuf[i], parityTypeChk, txRingBufPtr);
			}
			break;
		case ECR_TYPE_30: // 소수점 출력, 빈 공간 0x20
			Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, 0, AdData.weightDecPos, '.', 0);
			
			for (i = 0; i < commSendDigit; i++)
			{
				commSendCharWithParity(strBuf[i], parityTypeChk, txRingBufPtr);
			}
			break;
		case ECR_TYPE_35:
			if(SaleData.weight < 0 || AdData.overloadFlag || !AdData.stableFlag)
				Int32sToDecStr(strBuf, 0, commSendDigit, '0', AdData.weightDecPos, ',', 0);
			else
				Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), commSendDigit, '0', AdData.weightDecPos, ',', 0);

			bcc = 'w';
			
			for (i = 0; i < commSendDigit; i++)
			{
				commSendCharWithParity(strBuf[i], parityTypeChk, txRingBufPtr);
				bcc ^= strBuf[i];
			}

			commSendCharWithParity(commWeightUnitSmallStrTable[weightUnit][0], parityTypeChk, txRingBufPtr);
			commSendCharWithParity(commWeightUnitSmallStrTable[weightUnit][1], parityTypeChk, txRingBufPtr);
			bcc ^= commWeightUnitSmallStrTable[weightUnit][0];
			bcc ^= commWeightUnitSmallStrTable[weightUnit][1];

			for (i = 0; i < 12; i++)
				statusBuf[i] = 0x30;

			// 0x30 = state not appear, 0x39 = state appear
			if (!AdData.stableFlag) statusBuf[0] = 0x39;
			if (SaleData.weight == 0) statusBuf[1] = 0x39;
			if (AdData.overloadFlag) statusBuf[5] = 0x39;
			if (SaleData.weight < 0)
			{
				statusBuf[2] = 0x39;
				statusBuf[4] = 0x39;
			}

			for (i = 0; i < 12; i++)
			{
				commSendCharWithParity(statusBuf[i], parityTypeChk, txRingBufPtr);
				bcc ^= statusBuf[i];
			}

			commSendCharWithParity(bcc, parityTypeChk, txRingBufPtr);
			break;
#endif
		case ECR_TYPE_36:
			// commSendDigit default : 5
			//ex) weight 0.200 -> "02000"전송, 12.456 -> "124560" 전송 (독일 법인 요청)
			if(AbsInt32s(SaleData.weight) > 9999l) sendSize = commSendDigit; // commSendDigit 5
			else
			{
				sendSize = commSendDigit - 1;
			}
			Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), sendSize, '0', 0, 0, 0);
			for (i = 0; i < sendSize; i++)
			{
				commSendCharWithParity(strBuf[i], parityTypeChk, txRingBufPtr);
			}
			commSendCharWithParity('0', parityTypeChk, txRingBufPtr);
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
void commEcrSendStatus(RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commEcrType)
{
	INT8U sendByte1;
	INT8U sendByte2;

	switch (commEcrType)
	{
		case ECR_TYPE_2:
		case ECR_TYPE_3:
		case ECR_TYPE_10:
		case ECR_TYPE_20:
#ifdef USE_PD2_ECR
		case ECR_TYPE_21:
		case ECR_TYPE_22:
		case ECR_TYPE_23:
		case ECR_TYPE_29:
#endif	
		case ECR_TYPE_36:
		case ECR_TYPE_37:
			sendByte1 = 0x40;
			if(AdData.grossZeroFlag || AdData.netZeroFlag) sendByte1 |= 0x10;
			if(SaleData.weight < 0) sendByte1 |= 0x04;
			if(AdData.overloadFlag) sendByte1 |= 0x02;
			if(!AdData.stableFlag) sendByte1 |= 0x01;
			commSendCharWithParity(sendByte1, parityTypeChk, txRingBufPtr);
			break;
			
		case ECR_TYPE_4:
		case ECR_TYPE_5:
#ifdef USE_PD2_ECR
		case ECR_TYPE_24:
		case ECR_TYPE_25:
#endif
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
void commEcrSendPrice(RING_BUF *txRingBufPtr, INT8U priceType, INT8U parityTypeChk)
{
	char strBuf[6];
	INT8U i;

#ifdef	USE_DIALOG2_5DIGIT_PRICE
	if(priceType == UNIT_PRICE) Int32sToDecStr(strBuf, SaleData.unitPrice , 5, '0', 0, 0, 0);
	else Int32sToDecStr(strBuf, SaleData.totalPrice , 6, '0', 0, 0, 0);
#else
#ifdef USE_WEIGHT_ONLY_ECR
	if(priceType == UNIT_PRICE) Int32sToDecStr(strBuf, 0, 6, '0', 0, 0, 0);
	else Int32sToDecStr(strBuf, 0, 6, '0', 0, 0, 0);
#else

	if(priceType == UNIT_PRICE) Int32sToDecStr(strBuf, SaleData.unitPrice , 6, '0', 0, 0, 0);
	else Int32sToDecStr(strBuf, SaleData.totalPrice , 6, '0', 0, 0, 0);
#endif
#endif
	for (i = 0; i < 6; i++)
	{
		if (strBuf[i] == '\0') break;
		commSendCharWithParity(strBuf[i], parityTypeChk, txRingBufPtr);
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
void commEcrInterpreter(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType, INT8U commPortType)
{
	switch (commEcrType)
	{
		case ECR_TYPE_0:
//wcm0816 code optimaization
//			commEcrType0(rxRingBufPtr, txRingBufPtr);
//			break;
		case ECR_TYPE_1:
#ifdef USE_PD2_ECR
		case ECR_TYPE_18:
		case ECR_TYPE_19:
#endif
			commEcrType0(rxRingBufPtr, txRingBufPtr, parityTypeChk, commSendDigit, commEcrType);
			break;
		case ECR_TYPE_2:
		case ECR_TYPE_20:
#ifdef USE_PD2_ECR
		case ECR_TYPE_21:
		case ECR_TYPE_29:
#endif
//wcm0816 code optimaization
//			commEcrType2(rxRingBufPtr, txRingBufPtr);
//			break;
		case ECR_TYPE_3:	
		case ECR_TYPE_10:
#ifdef USE_PD2_ECR
		case ECR_TYPE_22:
		case ECR_TYPE_23:
#endif
		case ECR_TYPE_37:
			commEcrType2(rxRingBufPtr, txRingBufPtr, parityTypeChk, commSendDigit, commEcrType);
			break;
		case ECR_TYPE_4:
//wcm0816 code optimaization
//			commEcrType4(rxRingBufPtr, txRingBufPtr);
//			break;
		case ECR_TYPE_5:
#ifdef USE_PD2_ECR
		case ECR_TYPE_24:
		case ECR_TYPE_25:
#endif
			commEcrType4(rxRingBufPtr, txRingBufPtr, parityTypeChk, commSendDigit, commEcrType);
			break;
		case ECR_TYPE_6:
#ifdef USE_PD2_ECR
		case ECR_TYPE_26:
		case ECR_TYPE_27:
		case ECR_TYPE_28:
#endif
			commEcrType6(rxRingBufPtr, txRingBufPtr, parityTypeChk, commSendDigit, commEcrType);
			break;
#if !defined (USE_SLOVAKIA_PROTOCOL) && !defined (COUNTRY_CZECH)
		case ECR_TYPE_7:
#ifdef USE_PD2_ECR
		case ECR_TYPE_30:
		case ECR_TYPE_31:
#endif
			commEcrType7(rxRingBufPtr, txRingBufPtr, parityTypeChk, commSendDigit, commEcrType);
			break;
		case ECR_TYPE_8:
#ifdef USE_PD2_ECR
		case ECR_TYPE_32:
#endif
			commEcrType8(rxRingBufPtr, txRingBufPtr, parityTypeChk, commSendDigit, commEcrType);
			break;
		case ECR_TYPE_9:
#ifdef USE_PD2_ECR
		case ECR_TYPE_34:
#endif
			commEcrType9(rxRingBufPtr, txRingBufPtr, parityTypeChk, commSendDigit, commEcrType);
			break;
#else
		case ECR_TYPE_8:
			commEcrType8(rxRingBufPtr, txRingBufPtr, parityTypeChk, commSendDigit, commEcrType);
			break;
#endif
		//case ECR_TYPE_11:
			//commEcrType11(rxRingBufPtr, txRingBufPtr);
			//break;		
#ifdef DIALOG6
		case ECR_TYPE_12:
			commEcrType12(rxRingBufPtr, txRingBufPtr, parityTypeChk, commSendDigit, commEcrType, commPortType);
			break;
		case ECR_TYPE_13:
			commEcrType13(rxRingBufPtr, txRingBufPtr, parityTypeChk, commSendDigit, commEcrType);
			break;
		case ECR_TYPE_14:
		case ECR_TYPE_15:
			commEcrType14(rxRingBufPtr, txRingBufPtr, parityTypeChk, commSendDigit, commEcrType, commPortType);
			break;
#endif
#ifdef USE_SLOVAKIA_PROTOCOL
		case ECR_TYPE_16:
			commEcrType16(rxRingBufPtr, txRingBufPtr, parityTypeChk, commSendDigit, commEcrType);
			break;
#endif
#if !defined (USE_WEIGHT_ONLY_ECR) && !defined (USE_PDN_ECR) && defined(USE_ECR_ANKER_PROTOCOL)
		case ECR_TYPE_17:
			commEcrType17(rxRingBufPtr, txRingBufPtr, parityTypeChk, commSendDigit, commEcrType);
			break;
#endif
#ifdef USE_PD2_ECR
		case ECR_TYPE_33:
			commEcrType33(rxRingBufPtr, txRingBufPtr, parityTypeChk, commSendDigit, commEcrType);
			break;
		case ECR_TYPE_35:
			commEcrType35(rxRingBufPtr, txRingBufPtr, parityTypeChk, commSendDigit, commEcrType);
			break;
#endif
		case ECR_TYPE_36:
			commEcrType36(rxRingBufPtr, txRingBufPtr, parityTypeChk, commSendDigit, commEcrType);
			break;
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
void commEcrType0(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType)
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
#ifdef USE_PD2_ECR
						else if (SaleData.weight < 0)
						{
							if ((commEcrType == ECR_TYPE_18) || (commEcrType == ECR_TYPE_19))
							{
								commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);	//0x02
								commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk, commSendDigit, commEcrType);
								commSendCharWithParity(ETX, parityTypeChk, txRingBufPtr);	//0x03
							}
							else {
								commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15
							}
						}
						else if (!AdData.stableFlag)
						{
							if (commEcrType == ECR_TYPE_18)
							{
								commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);	//0x02
								commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk, commSendDigit, commEcrType);
								commSendCharWithParity(ETX, parityTypeChk, txRingBufPtr);	//0x03
							}
							else {
								commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15
							}
						}
#else
						else if (SaleData.weight < 0 || !AdData.stableFlag) //csh 20120904
						{
							commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x07
						}
#endif
						else
						{
							commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);	//0x02
							commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk, commSendDigit, commEcrType);
							commSendCharWithParity(ETX, parityTypeChk, txRingBufPtr);	//0x03
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
void commEcrType2(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType)
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
				else if(byte == 0x0d)
				{
#ifdef USE_PD2_ECR
					if ((commEcrType == ECR_TYPE_3) || (commEcrType == ECR_TYPE_22) || (commEcrType == ECR_TYPE_23))
#else
					if (commEcrType == ECR_TYPE_3)
#endif
						commTransFlag = 0;
				}
				else 
				{
					commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);  //0x02					
#ifdef COUNTRY_MEXICO
					if((byte == 'W') || (byte == 'I') || commTransFlag)
#else					
					if((byte == 'W') || commTransFlag)
#endif
					{
						if(!AdData.grossZeroFlag && !AdData.netZeroFlag && AdData.stableFlag && SaleData.weight >= 0 && !AdData.overloadFlag)
						{
#ifdef USE_WEIGHT_ONLY_ECR
								if(Pd2ECRProtocol != 1 && AdData.weightUnit != 0)
								{
									commSendCharWithParity('0', parityTypeChk, txRingBufPtr);  //0x30
								}
								else
								{
									commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk, commSendDigit, commEcrType);
								}
#else
								if(Pd2ECRProtocol != 1 && AdData.weightUnit != 0)
								{
#ifdef USE_PD2_ECR // USE_PDN_ECR
									if ((commEcrType == ECR_TYPE_2) || (commEcrType == ECR_TYPE_3))
#endif
										commSendCharWithParity('0', parityTypeChk, txRingBufPtr);  //0x30
								}
								commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk, commSendDigit, commEcrType);		//무게 전송 추가(버그)
#endif
						}
						else 
						{						
							commSendCharWithParity('?', parityTypeChk, txRingBufPtr);  //0x3F
							commEcrSendStatus(txRingBufPtr, parityTypeChk, commEcrType);
						}				
#ifdef USE_PD2_ECR
						if ((commEcrType == ECR_TYPE_3) || (commEcrType == ECR_TYPE_22) || (commEcrType == ECR_TYPE_23))
#else
						if (commEcrType == ECR_TYPE_3)
#endif
						{
							commTransFlag = 1;
							commWeightBackup = SaleData.weight;
						}
					}
					else
					{
						commSendCharWithParity('?', parityTypeChk, txRingBufPtr);  //0x3F					
						commSendCharWithParity('X', parityTypeChk, txRingBufPtr);  //0x58					
					}

					commSendCharWithParity(0x0d, parityTypeChk, txRingBufPtr);  //CR->0x0d
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
void commEcrType4(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType)
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
					commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
					commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk, commSendDigit, commEcrType);
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
					commSendCharWithParity(commWeightUnitStrTable[AdData.weightUnit][0], parityTypeChk, txRingBufPtr);
					commSendCharWithParity(commWeightUnitStrTable[AdData.weightUnit][1], parityTypeChk, txRingBufPtr);
#elif defined(USE_PD2_ECR)
					if ((commEcrType == ECR_TYPE_24) || (commEcrType == ECR_TYPE_25))
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
					commSendCharWithParity(commWeightUnitStrTable[AdData.weightUnit][0], parityTypeChk, txRingBufPtr);
					commSendCharWithParity(commWeightUnitStrTable[AdData.weightUnit][1], parityTypeChk, txRingBufPtr);
#endif
					commSendCharWithParity(0x0d, parityTypeChk, txRingBufPtr);	//CR-0x0d
					commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);		//0x0A
#ifdef USE_PD2_ECR
					if ((commEcrType == ECR_TYPE_4)	|| (commEcrType == ECR_TYPE_24))
#else
					if (commEcrType == ECR_TYPE_4)	
#endif
						commSendCharWithParity('S', parityTypeChk, txRingBufPtr);	//0x53
					commEcrSendStatus(txRingBufPtr, parityTypeChk, commEcrType);								//send 2 bytes
					commSendCharWithParity(0x0d, parityTypeChk, txRingBufPtr);	//CR-0x0d
					commSendCharWithParity(ETX, parityTypeChk, txRingBufPtr);	//0x03

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
void commEcrType6(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType)
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
#ifdef USE_COMMAND_TARE_KEY
				else if ((byte == 't')||(byte == 'T')) //0x74
				{
					SaleTareExtKey();
					RunSaleWeightTareProc();
					commState=0;
				}
#endif				
				break;
			
			case 1:
				if(byte == DC1)
				{			
					commSendCharWithParity(SOH, parityTypeChk, txRingBufPtr);
					commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);
			
					commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk, commSendDigit, commEcrType);

					commSendCharWithParity(ETX, parityTypeChk, txRingBufPtr);

#ifdef USE_2BYPE_CHECKSUM_PROTOCOL
					comm2byteChecksum(txRingBufPtr, 13, parityTypeChk);
					commSendCharWithParity(0x0D, parityTypeChk, txRingBufPtr);
					commSendCharWithParity(LF, parityTypeChk, txRingBufPtr);
#else					
					commSendCharWithParity(EOT, parityTypeChk, txRingBufPtr);
#endif
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
void commEcrType7(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType)
{
	char byte;
#ifdef USE_PD2_ECR
	if (commEcrType == ECR_TYPE_30)
	{
		if(SaleData.weight < 0 || AdData.overloadFlag || !AdData.stableFlag)
			return;
	}
#endif

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
		commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk, commSendDigit, commEcrType);
		commSendCharWithParity(0x0d, parityTypeChk, txRingBufPtr);
		commTransFlag = 0; 
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
void commEcrType8(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType)
{
	char byte;
	INT8U error;
#ifdef USE_PD2_ECR
	if (commEcrType == ECR_TYPE_32)
	{
		if(SaleData.weight < 0 || AdData.overloadFlag || !AdData.stableFlag)
			return;
	}
#endif

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
			
						commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk, commSendDigit, commEcrType);
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
void commEcrType9(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType)
{
	char byte, error;

	if (RingBufCheckBuf(rxRingBufPtr))
	{
		byte = RingBufGetCharWithParity(rxRingBufPtr, parityTypeChk, &error);
		switch (commState)
		{
			case 0:
				if (byte == ETX)
				{
					commSendCharWithParity(ACK, parityTypeChk, txRingBufPtr);
					commState++;
				}
				else if (byte == ESC) //for Factory
				{
					commState = 200;
				}
				else
				{
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
				}
				break;
			case 1:
				if(byte == ENQ)
				{
					if (!AdData.stableFlag) //csh 20120904
					{
						commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					}
#ifdef USE_PD2_ECR
					else if ((SaleData.weight < 0) && (commEcrType != ECR_TYPE_34)) //csh 20120904
#else
					else if (SaleData.weight < 0) //csh 20120904
#endif
					{
						commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					}	
					else
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
						commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk, commSendDigit, commEcrType);
						commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
#ifdef USE_PD2_ECR
						if (commEcrType != ECR_TYPE_34)
						{
							commEcrSendPrice(txRingBufPtr, UNIT_PRICE, parityTypeChk);
							commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
						
							commEcrSendPrice(txRingBufPtr, TOTAL_PRICE, parityTypeChk);
							commSendCharWithParity(ETX, parityTypeChk, txRingBufPtr);
						}
#else
						commEcrSendPrice(txRingBufPtr, UNIT_PRICE, parityTypeChk);
						commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
						
						commEcrSendPrice(txRingBufPtr, TOTAL_PRICE, parityTypeChk);
						commSendCharWithParity(ETX, parityTypeChk, txRingBufPtr);
#endif
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
					
					commEcrSendPrice(txRingBufPtr, UNIT_PRICE, parityTypeChk);
					
					commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
					
					commEcrSendPrice(txRingBufPtr, TOTAL_PRICE, parityTypeChk);
					
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
#ifdef DIALOG6
void commEcrType12(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType, INT8U commPortType)
{
	INT8U byte,i,count,error, S1, S0;
	static INT8U	txBufArray[10];
	static INT8U	rxTextArray[13];
	INT32U	rcvdata_price, rcvdata_tare;
	INT32U	keytarebuf;
	static INT8U	recordState, ref_Offset;
	char	sendByte;
	INT8U *delay_ctr_ptr;
	if(commPortType == PORT_TYPE_USB)
		delay_ctr_ptr = &responseDelayCtrl_usb;
	else 
		delay_ctr_ptr = &responseDelayCtrl;

	while(RingBufCheckBuf(rxRingBufPtr))
	{
		byte = RingBufGetCharWithParity(rxRingBufPtr, parityTypeChk, &error);
		if (error == 1)
		{
			*delay_ctr_ptr = ECR_TYPE12_STATUS_INFORM_02; //Type12_StsInfo
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
						*delay_ctr_ptr = ECR_TYPE12_STATUS_INFORM_20;//Type12_StsInfo
					}
					//min range?(해당 변수 없음)

					//underload or negative weight?
#ifdef USE_NOT_UNDERLOAD_ERROR
					else if(AdData.grossZeroFlag || AdData.netZeroFlag)
#else
					else if((SaleData.weight < 0)|| AdData.grossZeroFlag || AdData.netZeroFlag)//else if(AdData.underloadFlag || AdData.grossZeroFlag || AdData.netZeroFlag)
#endif
					{
						*delay_ctr_ptr  = ECR_TYPE12_STATUS_INFORM_31;//Type12_StsInfo
					}
					//overload?
					else if(AdData.overloadFlag)
					{
						*delay_ctr_ptr = ECR_TYPE12_STATUS_INFORM_32;//Type12_StsInfo
					}
#ifdef COUNTRY_CZECH
					else if(SaleData.weight == recentlySendWeight || recentlySendWeight != 0)
					{
						*delay_ctr_ptr = ECR_TYPE12_STATUS_INFORM_34;//Type12_StsInfo
					}
#endif
					else
					{
						*delay_ctr_ptr = ECR_TYPE12_STATUS_INFORM_00;//Type12_StsInfo

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
						
						commEcrSendWeight(txRingBufPtr, SaleData.weightUnit, parityTypeChk, commSendDigit, commEcrType);
						
						commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
						
						commEcrSendPrice(txRingBufPtr, UNIT_PRICE, parityTypeChk);
						
						commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
						
						commEcrSendPrice(txRingBufPtr, TOTAL_PRICE, parityTypeChk);
						
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
					
					if( *delay_ctr_ptr) commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);//Type12_StsInfo
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
				else if(byte == 0x33)
				{
					commState = 40;
				}
				else
				{
					*delay_ctr_ptr = ECR_TYPE12_STATUS_INFORM_10;//Type12_StsInfo
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					commState = 0;
				}
				break;

			case 30: //Record No. Second byte
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
				else if(recordState == 8)
				{
					commandState = 0;
					ref_Offset = 151;
				}
				else
				{
					*delay_ctr_ptr = ECR_TYPE12_STATUS_INFORM_10;//Type12_StsInfo
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
								*delay_ctr_ptr = ECR_TYPE12_STATUS_INFORM_10;//Type12_StsInfo
								commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
								commState = 0;
								break;
							}
							commState = ref_Offset;
							break;
#ifdef USE_WEIGHT_ONLY_ECR

//ESC
			case 31:
			case 91:	
			case 98:
				*delay_ctr_ptr = ECR_TYPE12_STATUS_INFORM_01;//Type12_StsInfo
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
					*delay_ctr_ptr = ECR_TYPE12_STATUS_INFORM_02;//Type12_StsInfo
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					commState = 0;
				}
				break;
#else
//ESC
			case 31:
			case 61:	
			case 68:
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
					*delay_ctr_ptr = ECR_TYPE12_STATUS_INFORM_02;//Type12_StsInfo
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
					*delay_ctr_ptr = ECR_TYPE12_STATUS_INFORM_11;//Type12_StsInfo
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
					*delay_ctr_ptr = ECR_TYPE12_STATUS_INFORM_11;//Type12_StsInfo
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
					*delay_ctr_ptr = ECR_TYPE12_STATUS_INFORM_13;//Type12_StsInfo
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					commState = 0;
				}
				break;			
//ETX 
			case 38:// Dialog 6 (Unit Price) : EOT STX '0' '1' 'ESC' U5 U4 U3 U2 U1 U0 'ESC' 'EXT' ,
				if(byte == ESC)//Dialog 6 protocol에 존재하지만, 'ESC' 구현이 누락되어서 추가함.
					break;
			case 73:
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

						switch(*delay_ctr_ptr)//Type12_StsInfo
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
						*delay_ctr_ptr = ECR_TYPE12_STATUS_INFORM_00;//Type12_StsInfo
					}
					else if(recordState == 0)
					{
						commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
						commEcrSendTare(txRingBufPtr, SaleData.keyTare, parityTypeChk);
						commSendCharWithParity(ETX, parityTypeChk, txRingBufPtr);
						commState = 0;
					}
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
							*delay_ctr_ptr = ECR_TYPE12_STATUS_INFORM_12;//Type12_StsInfo
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
#elif defined(USE_PDN_ECR)
						AdSetPT(rcvdata_tare,1);
#else
						//receivedAllPluDataApply(rxTextArray,rcvdata_price,rcvdata_tare); 
#ifndef COUNTRY_CZECH
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
						//everything is OK.	
						commSendCharWithParity(ACK, parityTypeChk, txRingBufPtr); //데이터 반영 후 ACK 응답 보냄.
					}
				}
				else 
				{					
					*delay_ctr_ptr = ECR_TYPE12_STATUS_INFORM_02;//Type12_StsInfo
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
void commEcrType13(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType)
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
						commEcrSendWeight(txRingBufPtr, SaleData.weightUnit, parityTypeChk, commSendDigit, commEcrType);
						commSendCharWithParity(RS, parityTypeChk, txRingBufPtr);
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
void commEcrType14(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType, INT8U commPortType)
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
#ifdef USE_TURKEY_ECR_15_PROTOCOL
			else if ((byte == 't')||(byte == 'T')) 
			{
				commState = USE_TARE_KEY_PROTOCOL;
			}
#endif
		}
		else if(commState == 200)
		{
			if (!CommOmInterpreter(commTxRingBufPtr, byte)) 
			{
				commState = 0;
			}
		}
#ifdef USE_TURKEY_ECR_15_PROTOCOL
		else if(commState == USE_TARE_KEY_PROTOCOL)
		{
			SaleTareExtKey();
			RunSaleWeightTareProc();
			commState=0;
		}
#endif
		else
		{
			if (commPortType == PORT_TYPE_USB)
			{
				if(responseDelayCtrl_usb == 0)
				{
					commEcrSendWeight(txRingBufPtr, SaleData.weightUnit, parityTypeChk, commSendDigit, commEcrType);
				}

				responseDelayCtrl_usb++;
				responseDelayCtrl_usb %= STREAM_MODE_RESPONSE_DELAY;

			}
			else {
				if(responseDelayCtrl == PORT_TYPE_RS232)
				{
					commEcrSendWeight(txRingBufPtr, SaleData.weightUnit, parityTypeChk, commSendDigit, commEcrType);
				}
	
				responseDelayCtrl++;
				responseDelayCtrl %= STREAM_MODE_RESPONSE_DELAY;
			}
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
#ifdef USE_SLOVAKIA_PROTOCOL
void commEcrType16(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType)
{
	if(saleSendSKProtocolflag)
	{
		commEcrSendWeight(txRingBufPtr, SaleData.weightUnit, parityTypeChk, commSendDigit, commEcrType);
		saleSendSKProtocolflag = 0;
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
*			1> Scale  <-- ENQ STX "data blocks" ETX -- PC
*				"data blocks"는 다음과 같다.
*				: 0 1 ESC U4 U3 U2 U1 U0 
*			2> Scale  ---        ACK(0x06)         --> PC
*			3> Scale  <-- ENQ STX "data blocks" ETX -- PC 
*			   Scale  <-- ENQ STX "data blocks" ETX -- PC // send 2 times
*			4> Scale  ---   STX "data blocks" ETX  --> PC
*			"data blocks"는 다음과 같다.
*				: 0 2 ESC STA ESC W4 W3 W2 W1 W0 ESC U4 U3 U2 U1 U0 ESC 
*				  U4 U3 U2 U1 U0 ESC P5 P4 P3 P2 P1 P0 ETX
********************************************************************************
*/
#if !defined(USE_WEIGHT_ONLY_ECR) || !defined(USE_PDN_ECR) && defined(USE_ECR_ANKER_PROTOCOL)
void commEcrType17(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType)
{
	char byte;
	INT8U error;
	INT8U sendDta, i;
	static INT8U sndDataState = 0;//comm state flag
	static INT8U txBufArray[10];
	INT32U rcvdata_price;
	static INT32U prercvdata_price;

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
				if (byte == ENQ) // start with ENQ
					commState++;
				else if (byte == ESC)
					commState = 200;
				else
				{
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15
					commState = 0;
					sndDataState = 0;
				}
				break;
			case 1:
				if (byte == STX)
					commState++;
				else
				{
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15
					commState = 0;
					sndDataState = 0;
				}
				break;
			case 2:
				if (byte == '0')
					commState ++;
				else
				{
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15
					commState = 0;
					sndDataState = 0;
				}
				break;
			case 3:
				if (byte == '1')
				{
					commState = 10;
				}
				else
				{
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15
					commState = 0;
					sndDataState = 0;
				}
				break;
			case 10:
				if (byte == ESC)
				{
					commState = 20; // UnitPrice
				}
				else
				{
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15
					commState = 0;
					sndDataState = 0;
				}
				break;
				
			case 20:
			case 21:
			case 22:
			case 23:
			case 24:	// UnitPrice 5 digit		
				if(isNum(byte))
				{
					txBufArray[commState - 20] = byte;
					commState++;
				}
				else
				{
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);
					commState = 0;
					sndDataState = 0;
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

					if(sndDataState == 0) // ready 'set unitprice cmd' state
					{
						receivedPluDataApply(0, rcvdata_price,0xFFFFFFFF);
							
						if(SaleData.unitPrice == rcvdata_price)// set unit price success
						{
							commSendCharWithParity(ACK, parityTypeChk, txRingBufPtr);
							prercvdata_price = rcvdata_price;
							sndDataState = 1;							
						}
						else
						{
							commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15
							sndDataState = 0;
						}
					}
					else if(sndDataState == 1) // ready 'send current data' state (wait final cmd)
					{
						if(prercvdata_price == rcvdata_price)
						{
							sndDataState = 2;
						}
						else
						{
							commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15
							sndDataState = 0;
						}
					}
					else if(sndDataState == 2) // send current data state2 (received final cmd)
					{
						if((prercvdata_price == rcvdata_price) && (rcvdata_price == SaleData.unitPrice))
						{
							if (AdData.stableFlag)
							{
								commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);
								commSendCharWithParity('0', parityTypeChk, txRingBufPtr);
								commSendCharWithParity('2', parityTypeChk, txRingBufPtr);
								commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);

								sendDta = (AdData.weightUnit + 3) % 4 + '0';
								commSendCharWithParity(sendDta, parityTypeChk, txRingBufPtr);
								commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
								
								commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk, commSendDigit, commEcrType);
								commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);

								commEcrSendPrice(txRingBufPtr, UNIT_PRICE, parityTypeChk);
								commSendCharWithParity(ESC, parityTypeChk, txRingBufPtr);
									
								commEcrSendPrice(txRingBufPtr, TOTAL_PRICE, parityTypeChk);
								commSendCharWithParity(ETX, parityTypeChk, txRingBufPtr);
							}
							else //scale unstable
							{
								commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15
							}
						}
						else // protocol error
						{
							commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15
						}
						prercvdata_price = 0;
						sndDataState = 0; // ready 'set unitprice cmd' state
					}
				}
				else // protocol error
				{
					commSendCharWithParity(NAK, parityTypeChk, txRingBufPtr);	//0x15
					sndDataState = 0;
				}
				commState = 0; // end of protocol
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


#ifdef USE_PD2_ECR
/**
********************************************************************************
* @brief    PDN communication Interpreter 함수\n
* @param    rxRingBufPtr : 받은 데이터가 있는 Ring Buffer Pointer
*           txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   Protocol은 다음과 같다.
*           1> Scale <-- $(0x24) --  PC
*           2> Scale  -- STX "data blocks" ETX --> PC
*           "data blocks"는 다음과 같다.
*           Data Block : W4 W3 W2 W1 W0 CR
********************************************************************************
*/

void commEcrType33(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType)
{
	char byte;

	if(SaleData.weight <= 0 || AdData.overloadFlag || !AdData.stableFlag)
		return;

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

	if (commTransFlag)
	{
		commSendCharWithParity(0x02, parityTypeChk, txRingBufPtr);
		commSendCharWithParity(0x20, parityTypeChk, txRingBufPtr);
		commSendCharWithParity(0x20, parityTypeChk, txRingBufPtr);
		commSendCharWithParity(0x20, parityTypeChk, txRingBufPtr);
		commSendCharWithParity(0x20, parityTypeChk, txRingBufPtr);
		commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk, commSendDigit, commEcrType);
		commSendCharWithParity(0x03, parityTypeChk, txRingBufPtr);
		commTransFlag = 0; 
	}
}

/**
********************************************************************************
* @brief    PDN communication Interpreter 함수\n
* @param    rxRingBufPtr : 받은 데이터가 있는 Ring Buffer Pointer
*           txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   Protocol은 다음과 같다.
*           1> Scale <-- STX(0x02) --  PC
*           2> Scale <-- W(0x57) -- PC
*           3> Scale <-- ETX(0x03) -- PC
*           4> Scale  -- STX w "data blocks" ETX --> PC
*           "data blocks"는 다음과 같다.
*           Data Block : W5 W4 W3 ',' W2 W1 W0 UN1 UN0 S11 S10 S09 S08 S07 S06 S05 S04 S03 S02 S01 S00 bcc
********************************************************************************
*/

void commEcrType35(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType)
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
				if (byte == STX)
				{
					commState++;
				}
				else if (byte == ESC) //for Factory
				{
					commState = 200;
				}
				else
				{
					commSendCharWithParity(ACK, parityTypeChk, txRingBufPtr);	//0x15 //csh 20120904
				}
				break;
			case 1:
				if(byte == 'W')
				{
					commState++;
				}
				else if (byte == ESC) //for Factory
				{
					commState = 200;
				}
				else
				{
					commSendCharWithParity(ACK, parityTypeChk, txRingBufPtr);	//0x15 //csh 20120904
				}
				break;
			case 2:
				if (byte == ETX)
				{
					commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);
					commSendCharWithParity('w', parityTypeChk, txRingBufPtr);
			
					commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk, commSendDigit, commEcrType);

					commSendCharWithParity(ETX, parityTypeChk, txRingBufPtr);
				}
				else
				{
					commSendCharWithParity(ACK, parityTypeChk, txRingBufPtr);	//0x15 //csh 20120904
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
* @brief    PDN communication Interpreter 함수\n
* @param    rxRingBufPtr : 받은 데이터가 있는 Ring Buffer Pointer
*           txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
*           1> Scale <-- W(0x57) --  PC
*           2> Scale  -- data blocks --> PC
*           "data blocks"는 다음과 같다.
*           Data Block : 
*			1) Weight 5 digit : STX W4 W3 W2 W1 W0 "0" CR
*			2) Weight 6 digit : STX W5 W4 W3 W2 W1 W0 "0" CR (무게 뒤에 '0' 붙여서 전송)
********************************************************************************
*/

void commEcrType36(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr, INT8U parityTypeChk, INT8U commSendDigit, INT8U commEcrType)
{
	char byte;
	INT8U error, sendDigit;
	
	if(RingBufCheckBuf(rxRingBufPtr))
	{
		byte = RingBufGetCharWithParity(rxRingBufPtr, parityTypeChk, &error);
		if (error == 1)
		{
			commState = 0;
			return;
		}
		switch(commState)
		{
			case 0:
				if(byte == ESC)
				{
					commState = 200;
				}
				else if(byte == 'W')
				{
					commSendCharWithParity(STX, parityTypeChk, txRingBufPtr);  //0x02	
					
					if(!AdData.grossZeroFlag && !AdData.netZeroFlag && AdData.stableFlag && SaleData.weight >= 0 && !AdData.overloadFlag)
					{
						if(AdData.weightUnit != UNIT_KG)
						{
							commSendCharWithParity('0', parityTypeChk, txRingBufPtr);  //0x30
						}
						commEcrSendWeight(txRingBufPtr, AdData.weightUnit, parityTypeChk, commSendDigit, commEcrType);		//무게 전송 추가(버그)
					}
					else // Error Value Data format
					{						
						commSendCharWithParity('?', parityTypeChk, txRingBufPtr);  //0x3F
						commEcrSendStatus(txRingBufPtr, parityTypeChk, commEcrType);
					}
					
					commSendCharWithParity(0x0d, parityTypeChk, txRingBufPtr);  //CR->0x0d
					commState = 0;	
				}
				else
				{
					commSendCharWithParity('?', parityTypeChk, txRingBufPtr);  //0x3F					
					commSendCharWithParity('X', parityTypeChk, txRingBufPtr);  //0x58	
					commSendCharWithParity(0x0d, parityTypeChk, txRingBufPtr);  //CR->0x0d
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
	commSendCharWithParity((INT8U)((sumData >> 8) & 0x7F), parityType, txRingBufPtr);
	commSendCharWithParity((INT8U)(sumData & 0x007F), parityType, txRingBufPtr);
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
						//commErSendWeight(txRingBufPtr);
						RingBufPutChar(txRingBufPtr, EOT);
						break;
					case DC2:
						RingBufPutChar(txRingBufPtr, SOH);
						//commErSendTotalPrice(txRingBufPtr);
						//commErSendWeight(txRingBufPtr);
						//commErSendUnitPrice(txRingBufPtr);
						RingBufPutChar(txRingBufPtr, EOT);
						break;
					case DC3:	//for greek, DC2 + PLU Number
						RingBufPutChar(txRingBufPtr, SOH);
						//commErSendTotalPrice(txRingBufPtr);
						//commErSendWeight(txRingBufPtr);
						//commErSendUnitPrice(txRingBufPtr);
						//ommErSendPluNumber(txRingBufPtr);
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
						//commErSendWeight(txRingBufPtr);
						break;
						
					case 'p':
						//commErSendTotalPrice(txRingBufPtr);
						//commErSendWeight(txRingBufPtr);
						//commErSendUnitPrice(txRingBufPtr);
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
						if (btCmd[1] == 'I')
							commSmartBtSendScaleInformation(txRingBufPtr);
						else if (btCmd[1] == 'W')
							commSmartBtSendWeight(txRingBufPtr);
						else if (btCmd[1] == 'T')
							commSmartBtSendTare(txRingBufPtr);
						else if (btCmd[1] == 'P')
							commSmartBtSendPowerInformation(txRingBufPtr);
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
	
	cnt = 0;
	MakeStrCopy(&strBuf[cnt], "RP,", 3);
	cnt += 3;
// need to develop check power type function
	MakeStrCopy(&strBuf[cnt], "AD,", 3);
	cnt += 3;
// need to develop check power gage
	Int32sToDecStr(&strBuf[cnt], 83, 3, ' ', 0, 0, 0);
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
#endif//#ifdef USE_PDN_ECR_COMM_API

