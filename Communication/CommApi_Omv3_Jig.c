/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Driver Source File\n
           Chip : Efm32g840f128 module
* @file    CommApi_Omv3_Jig.c
* @version 1.0
* @date    2012/09/19
* @author  Deok Hyun Kim (KDH)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2012/09/19 created by KDH \n
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
#include "../Common/TimeData.h"
#include "../SystemTimer/TimerApi.h"
#include "../UI/DispApi.h"
#include "../UI/KeyApi.h"
#include "../DB/StorageApi.h"
#include "../DB/ParamApi.h"
#include "../Ad/AdApi.h"
#include "../Ad/TempCompApi.h"
#include "../Ad/HystCompApi.h"
#include "../Ad/LinearCompApi.h"
#include "../Ad/CreepCompApi.h"
#include "../Time/TimeApi.h"
#include "../Power/PowerApi.h"
#include "CommDrv.h"
#include "CommApi.h"
#include "PrtApi.h"
#include "DebugApi.h"

#include "../Power/BattDrv.h"//for BattDrvReadVolt()
#include "../AD/TempDrv.h"//for TempDrvInit()
#include "../Hardware/HardwareDrv.h"//for HwDrvSetFactoryBoot()

#include "../Hardware/HardwareConfig.h"
#ifdef USE_JIG_COMM_API
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
/** @brief Factory Communication Interpreter의 상태 변수 */
static INT8U commStateF;

/** @brief Weight Unit String Table */
static const char code commWeightUnitStrTable[COMM_WEIGHT_STR_TABLE_SIZE][2] =
{
	{'k','g'},	{'l', 'b'},	{'o', 'z'},	{' ', 'g'}
};

#define TEST_SPAN_CONTROL_PIN	P32	
#define TEST_BATT_CONTROL_PIN	P33
#define TEST_PORT_CONN_NUM	42          // duckspg 22
#define EFM32G840F128_TOTAL_PORT        64


//P0 connection : 0-4, 1-5, 2-6, 3-7
//P1 connection : 0-6, 1-7
//P2 connection : 0-2, 1-3
//P3 connection : 2-5, 3-6, 4-7

const INT8U testContable[2][3] =            // duckspg jig
{
    { 0, 3, gpioPortC},
};

#define DEFAULTPIN   0xff


/* { 0, gpioPortA, 2}  == { 포트번호, 포트명칭, 연결되어있는 CPU 핀 숫자 - 1}  -1은 배열이 0부터임으로 CPU 핀은 1부터 나가기 때문
포트번호 포트명칭은 cpu 1번부터 64번까지 순서대로 진행되었음 
DEFAULTPIN : 앞에서 매칭이 되어있거나 다른용도로 쓰고 있는 pin일때 
*/
const INT8U efm32g840PortMap[EFM32G840F128_TOTAL_PORT][3] = 
{
    { 0, gpioPortA, 62}, //0
    { 1, gpioPortA, 63},    //1
    { 2, gpioPortA, 61},   //2
    { 3, gpioPortA, 5},   //3
    { 4, gpioPortA, 6},   //4
    { 5, gpioPortA, DEFAULTPIN},   //5
    { 6, gpioPortA, DEFAULTPIN},   //6 
    { 0, DEFAULTPIN, DEFAULTPIN},  //7
    { 3, gpioPortB, 10},   //8
    { 4, gpioPortB, 11},
    { 5, gpioPortB, DEFAULTPIN},   //10
    { 6, gpioPortB, DEFAULTPIN },   //11
    { 4, gpioPortC, 14},
    { 5, gpioPortC, 15},
    { 7, gpioPortB, DEFAULTPIN},   //14
    { 8, gpioPortB, DEFAULTPIN},   //15
    
    { 12, gpioPortA, 27},  //16
    { 13, gpioPortA, 28},
    { 14, DEFAULTPIN, DEFAULTPIN},
    { 0, DEFAULTPIN, DEFAULTPIN},
    { 11, DEFAULTPIN, DEFAULTPIN},
    { 12, DEFAULTPIN, DEFAULTPIN},
    { 0, DEFAULTPIN, DEFAULTPIN},
    { 13, DEFAULTPIN, DEFAULTPIN},   //23
    { 14, DEFAULTPIN, DEFAULTPIN},  //24
    { 0, DEFAULTPIN, DEFAULTPIN},  //25
    { 0, DEFAULTPIN, DEFAULTPIN},  //26
    { 0, gpioPortD, DEFAULTPIN},   //27
    { 1, gpioPortD, DEFAULTPIN},   //28
    { 2, DEFAULTPIN, DEFAULTPIN},  //29
    { 3, DEFAULTPIN, DEFAULTPIN},  //30
    { 4, gpioPortD, 33},   //31
    
    { 5, gpioPortD, 35},      //32
    { 6, gpioPortD, DEFAULTPIN},  //33
    { 7, gpioPortD, 36},        //34
    { 8, gpioPortD, DEFAULTPIN},   //35
    { 6, gpioPortC, DEFAULTPIN},    //36
    { 7, DEFAULTPIN, DEFAULTPIN },
    { 0, DEFAULTPIN, DEFAULTPIN},
    { 0, DEFAULTPIN, DEFAULTPIN},
    { 4, gpioPortE, 42},   //40
    { 5, gpioPortE, 43},   //41
    { 6, gpioPortE, DEFAULTPIN},   //42
    { 7, gpioPortE, DEFAULTPIN},   //43
    { 12, DEFAULTPIN, DEFAULTPIN},
    { 13, DEFAULTPIN, DEFAULTPIN},
    { 14, DEFAULTPIN, DEFAULTPIN},
    { 15, gpioPortC, DEFAULTPIN},  //47
    
    { 0, DEFAULTPIN, DEFAULTPIN},   //48
    { 1, DEFAULTPIN, DEFAULTPIN},   //49
    { 2, gpioPortF, 52},   //50
    { 3, gpioPortF, 53 },   //51
    { 4, gpioPortF, DEFAULTPIN},   //52
    { 5, gpioPortF, DEFAULTPIN},   //53
    { 0, DEFAULTPIN, DEFAULTPIN},  //54
    { 8, gpioPortE, 59},   //55
    { 9, gpioPortE, 60},   //56
    { 10, DEFAULTPIN },  //57
    { 11, DEFAULTPIN },  //58
    { 12, gpioPortE, DEFAULTPIN},  //59
    { 13, gpioPortE, DEFAULTPIN},   //60
    { 14, gpioPortE, DEFAULTPIN},  //61  duckspg jig 변경 해야함 
    { 15, gpioPortE, DEFAULTPIN},  //62
    { 15, gpioPortA, DEFAULTPIN},  //63
};



const INT8U code TestConnTable[TEST_PORT_CONN_NUM][2][2] =
{
 	{  { 13, gpioPortB}, {  5, gpioPortD}},  //4
 	{  {  4, gpioPortB}, {  4, gpioPortD}},  //5
 	{  {  4, gpioPortD}, {  4, gpioPortB}},  //7
 	{  {  5, gpioPortD}, {  13, gpioPortB}},  //8
 	{  {  6, gpioPortD}, {  8, gpioPortD}},  //10
 	{  {  7, gpioPortD}, {  6, gpioPortC}},  //11
 	{  {  8, gpioPortD}, {  6, gpioPortD}},  //12
 	{  {  6, gpioPortC}, {  7, gpioPortC}},  //13
 	{  {  4, gpioPortE}, { 6, gpioPortE}},  //21
 	{  {  5, gpioPortE}, { 7, gpioPortE}},  //22
 	{  {  6, gpioPortE}, { 4, gpioPortE}},  //23
 	{  {  7, gpioPortE}, { 5, gpioPortE}},  //24
 	{  {  2, gpioPortF}, { 4, gpioPortF}},  //25
 	{  {  3, gpioPortF}, { 5, gpioPortF}},  //26
 	{  {  4, gpioPortF}, { 2, gpioPortF}},  //27
 	{  {  5, gpioPortF}, { 3, gpioPortF}},  //28
 	{  {  8, gpioPortE}, { 12, gpioPortE}},  //29
 	{  {  9, gpioPortE}, { 13, gpioPortE}},  //30
 	{  { 12, gpioPortE}, { 8, gpioPortE}},  //31
 	{  { 13, gpioPortE}, { 9, gpioPortE}},  //32
 	{  { 14, gpioPortE}, { 15, gpioPortE}},  //33
 	{  { 15, gpioPortE}, { 14, gpioPortE}},  //35
 	{  { 15, gpioPortA}, { 1, gpioPortA}},  //36
 	{  {  0, gpioPortA}, { 2, gpioPortA}},  //37
 	{  {  1, gpioPortA}, { 15, gpioPortA}},  //38
 	{  {  2, gpioPortA}, { 0, gpioPortA}},  //39
 	{  {  3, gpioPortA}, { 5, gpioPortA}},  //40
 	{  {  4, gpioPortA}, { 6, gpioPortA}},  //1
 	{  {  5, gpioPortA}, { 3, gpioPortA}},  //2
 	{  {  6, gpioPortA}, { 4, gpioPortA}},  //3
 	{  {  3, gpioPortB}, { 5, gpioPortB}},  //4
 	{  {  4, gpioPortB}, { 6, gpioPortB}},  //5
 	{  {  5, gpioPortB}, { 3, gpioPortB}},  //6
 	{  {  6, gpioPortB}, { 4, gpioPortB}},  //7
 	{  {  0, gpioPortD}, { 12, gpioPortA}},  //11
 	{  {  1, gpioPortD}, { 13, gpioPortA}},  //12
 	{  {  4, gpioPortC}, { 7, gpioPortB}},  //13
 	{  {  5, gpioPortC}, { 8, gpioPortB}},  //14
 	{  {  7, gpioPortB}, { 4, gpioPortC}},  //15
 	{  {  8, gpioPortB}, { 5, gpioPortC}},  //16
 	{  { 12, gpioPortA}, { 0, gpioPortD}},  //17
 	{  { 13, gpioPortA}, { 1, gpioPortD}},  //18
};
/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
static INT16U jigFunc(INT8U funcNum);

static void commErInterpreter(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);

static BOOLEAN commOmJIgInterpreter(RING_BUF *txRingBufPtr, char byte);
static void commOmJigFunc(RING_BUF *txRingBufPtr, INT8U type, INT8U rw, INT32U addr, INT32S data32s);
INT8U portTest(void);
void ad1230Set(INT8U adConData);
void ad1230Clear(void);
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
	CommDrvInit(BAUD_RATE_DEFAULT);

	commRxRingBufPtr = &SerialRxRingBuf;
	commTxRingBufPtr = &SerialTxRingBuf;

	commState = 0;
	commStateF = 0;
}

void comtestprint(void)     // duckspg jig
{
    RingBufPutChar(commTxRingBufPtr, 0xAB);
    CommDrvProc();
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
//	if (RingBufCheckBuf(commRxRingBufPtr))
	while (RingBufCheckBuf(commRxRingBufPtr))
	{
		commErInterpreter(commRxRingBufPtr, commTxRingBufPtr);		//for zig communication
	}
	CommDrvProc();
//	TEST_BATT_CONTROL_PIN = 0;	//BAT_CONTROL prevent power off
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

		Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), 6, ' ', 0, 0, 0);
		for (i = 0; i < 6; i++)
		{
			bcc ^= strBuf[i];
		}
		RingBufPutData(txRingBufPtr, strBuf, 6);
	}

	sendByte = commWeightUnitStrTable[AdData.weightUnit][0];
	bcc ^= sendByte;
	RingBufPutChar(txRingBufPtr, sendByte);
	sendByte = commWeightUnitStrTable[AdData.weightUnit][1];;
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

	Int32sToDecStr(strBuf, SaleData.totalPrice , 7, ' ', 0, 0, 0);
	for (i = 0; i < 7; i++)
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
	Int32sToDecStr(strBuf, SaleData.unitPrice , 7, ' ', 0, 0, 0);
	for (i = 0; i < 7; i++)
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
					commState = 200;            // jig 로 define duckspg 
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
//					case DC3:
//						break;
				}
				commState = 0;
				break;
			case 200:
				if (!commOmJIgInterpreter(commTxRingBufPtr, byte)) 
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
* @brief    
* @param    
* @return   
* @remark   TODO : 채워넣기
********************************************************************************
*/
BOOLEAN commOmJIgInterpreter(RING_BUF *txRingBufPtr, char byte)
{
	static INT8U commDataSize;
	static INT8U commDataRw;
	static INT8U commDataType;
	static UNION_INT32U commDataAddr;
	static UNION_INT32U commData32s;
	static INT8U bcc;

	switch (commStateF)
	{
		case 0:
			switch (byte)
			{
				case 'D':	
				case 'E':
				case 'T':					
					commDataType = byte;
					bcc = byte;
					commDataRw = 0;
					commStateF++;
					break;
				default:
					commStateF = 0;
			}
			break;
		case 1:
			switch (byte)
			{
				case 'R':	
				case 'W':	
					commDataRw = byte;
					bcc ^= byte;
					commDataSize = 0;
					commDataAddr.dword = 0;
					commStateF++;
					break;
				default:
					commStateF = 0;
				}
			break;
		case 2:
			commDataAddr.byteArray.byte[commDataSize] = byte;
			bcc ^= byte;
			commDataSize++;
			if (commDataSize >= 4) 
			{
				commDataSize = 0;
				commData32s.dword = 0;
				commStateF++;
				if (commDataRw == 'R')
				{
					commStateF++;
				}
			}
			break;
		case 3:
			commData32s.byteArray.byte[commDataSize] = byte;
			bcc ^= byte;
			commDataSize++;
			if (commDataSize >= 4) 
			{
				commStateF++;
			}
			break;
		case 4:
			if (byte == bcc)
			{
				commStateF++;
			}
			else
			{
				commStateF = 0;
			}
			break;
		case 5:
			if (byte == 0x0D)
			{
				commOmJigFunc(txRingBufPtr, commDataType, commDataRw, (INT32S)commDataAddr.dword, (INT32S)commData32s.dword);
			}
		default:
			commStateF = 0;
			break;
	}

	if (commStateF == 0) 
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/**
********************************************************************************
* @brief    
* @param    
* @return   
* @remark   TODO : 채워넣기
********************************************************************************
*/
void commOmJigFunc(RING_BUF *txRingBufPtr, INT8U type, INT8U rw, INT32U addr, INT32S data32s)
{
	INT8U error;
	INT32S sendData;
	INT8U bcc;
	INT8U i;
	INT8U *bPtr;
    static INT8U test = 0x01;
    static INT8U test1[10];

	error = 0;
	if (type == 'T') 
	{
		if (rw == 'R') 
		{
			if (addr >= 1 && addr <= 8)
			{
				sendData = (INT32S)jigFunc((INT8U)addr);
			}
			else
			{
				error = 1;
			}
		} 
		else
		{
			error = 1;
		}
	}
	else if (type == 'D') 
	{
		if (rw == 'R') 
		{
			switch (addr)
			{
				case 100:
//					if(KeyCheckCalPort())
//					{
						HwDrvSetFactoryBoot();
						PowerOff();
//					}
					break;
				default:
					error = 1;
					break;
			}
		}
	}
	else if (type == 'E') 
	{
		if (rw == 'R') 
		{
			sendData = (INT32S)StorageReadByte((INT16U)addr);
		}
		else if (rw == 'W')
		{
			StorageWriteByte((INT16U)addr, (INT8U)data32s);
			sendData = (INT32S)StorageReadByte((INT16U)addr);
		}
		else
		{
			error = 1;
		}
	}
	else
	{
		error = 1;
	}

	if (error == 0) 
	{
//        for (i = 0 ; i<10 ; i++)
//            RingBufPutChar(txRingBufPtr, test+i);

		RingBufPutChar(txRingBufPtr, ESC);
		RingBufPutChar(txRingBufPtr, type);
		bcc = type;
		RingBufPutChar(txRingBufPtr, 'W');
		bcc ^= 'W';
		bPtr = (INT8U *)&addr;
		RingBufPutData(txRingBufPtr, bPtr, 4);
		for (i = 0; i < 4; i++)
		{
            test1[i] = bPtr[i];
			bcc ^= bPtr[i];
		}
		bPtr = (INT8U *)&sendData;
		RingBufPutData(txRingBufPtr, bPtr, 4);
		for (i = 0; i < 4; i++)
		{
			bcc ^= bPtr[i];
		}
		RingBufPutChar(txRingBufPtr, bcc);
		RingBufPutChar(txRingBufPtr, 0x0D);
	}
}

INT8U portTest(void)
{
    INT8U portNum;
    INT8U tempPortNum;
    INT8U error = 0;
    
    for (portNum = 0 ; portNum < EFM32G840F128_TOTAL_PORT ; portNum++)
    {
        if (portNum != EFM32G840F128_TOTAL_PORT-1)
        {
            if ((efm32g840PortMap[portNum][1] != DEFAULTPIN) && (efm32g840PortMap[portNum+1][1] != DEFAULTPIN))
            {
    // 주위에 붙어있는 핀 체크
                GPIO_PinModeSet(efm32g840PortMap[portNum][1], efm32g840PortMap[portNum][0], gpioModePushPull, 1);
                if (((GPIO->P[efm32g840PortMap[portNum+1][1]].DIN >> efm32g840PortMap[portNum+1][0]) & 0x1) == 1)
                {
                    GPIO_PinModeSet(efm32g840PortMap[portNum][1], efm32g840PortMap[portNum][0], gpioModeInputPull, 0);
                    error = portNum+1;      // 배열은 0~63까지 cpu port는 1~64번지 따라서 포트번호는 1을 더한다 
                    break;  //connect error return
                }
              
    // 핀출력이 나오는지를 체크                 
                tempPortNum = efm32g840PortMap[portNum][2];
                if (tempPortNum != DEFAULTPIN)
                {
                    if (((GPIO->P[efm32g840PortMap[tempPortNum][1]].DIN >> efm32g840PortMap[tempPortNum][0]) & 0x1) != 1)
                    {
                        GPIO_PinModeSet(efm32g840PortMap[portNum][1], efm32g840PortMap[portNum][0], gpioModeInputPull, 0);
                        error = portNum+1+100;
                        break;  //operation error return 
                    }
                }
                GPIO_PinModeSet(efm32g840PortMap[portNum][1], efm32g840PortMap[portNum][0], gpioModeInputPull, 0);
            }
        }
        
    }
    return error;
}


void ad1230Set(INT8U adConData)
{
    GPIO_PinModeSet(efm32g840PortMap[51][1], efm32g840PortMap[51][0], gpioModePushPull, adConData);
    GPIO_PinModeSet(efm32g840PortMap[53][1], efm32g840PortMap[53][0], gpioModeInputPull, adConData);
    GPIO_PinModeSet(efm32g840PortMap[41][1], efm32g840PortMap[41][0], gpioModePushPull, adConData);
    GPIO_PinModeSet(efm32g840PortMap[43][1], efm32g840PortMap[43][0], gpioModeInputPull, adConData);
}

void ad1230Clear(void)
{
    GPIO_PinModeSet(efm32g840PortMap[51][1], efm32g840PortMap[51][0], gpioModeInputPull, 0);
    GPIO_PinModeSet(efm32g840PortMap[53][1], efm32g840PortMap[53][0], gpioModeInputPull, 0);
    GPIO_PinModeSet(efm32g840PortMap[41][1], efm32g840PortMap[41][0], gpioModeInputPull, 0);
    GPIO_PinModeSet(efm32g840PortMap[43][1], efm32g840PortMap[43][0], gpioModeInputPull, 0);
}



void TestWritePort(INT8U num, INT8U pinData)
{
	switch (num)
	{
/*
    case 0:
			P00 = pinData;
			break;
		case 1:
			P01 = pinData;
			break;
		case 2:
			P02 = pinData;
			break;
		case 3:
			P03 = pinData;
			break;
		case 4:
			P04 = pinData;
			break;
		case 5:
			P05 = pinData;
			break;
		case 6:
			P06 = pinData;
			break;
		case 7:
			P07 = pinData;
			break;

		case 10:
			P10 = pinData;
			break;
		case 11:
			P11 = pinData;
			break;
		case 16:
			P16 = pinData;
			break;
		case 17:
			P17 = pinData;
			break;

		case 20:
			P20 = pinData;
			break;
		case 21:
			P21 = pinData;
			break;
		case 22:
			P22 = pinData;
			break;	
		case 23:
			P23 = pinData;
			break;

		case 32:
			P32 = pinData;
			break;
		case 33:
			P33 = pinData;
			break;
		case 34:
			P34 = pinData;
			break;
		case 35:
			P35 = pinData;
			break;
		case 36:
			P36 = pinData;
			break;
		case 37:
			P37 = pinData;
			break;

*/	}
}

INT8U TestReadPort(INT8U num)
{
	switch (num)
	{
/*
    case 0:
			return P00;
		case 1:
			return P01;
		case 2:
			return P02;
		case 3:
			return P03;
		case 4:
			return P04;
		case 5:
			return P05;
		case 6:
			return P06;
		case 7:
			return P07;

		case 10:
			return P10;
		case 11:
			return P11;
		case 16:
			return P16;
		case 17:
			return P17;

		case 20:
			return P20;
		case 21:
			return P21;
		case 22:
			return P22;
		case 23:
			return P23;

		case 32:
			return P32;
		case 33:
			return P33;
		case 34:
			return P34;
		case 35:
			return P35;
		case 36:
			return P36;
		case 37:
			return P37;

		default:
			return 0xff;
*/
	}
}


INT16U jigFunc(INT8U funcNum)
{
	INT16U tempInt16u;
	INT16U prevJigTimer100ms;
	INT16U currTimer100ms;
	INT16U err;
	INT8U testPinNum;
	INT8U i;
    INT8U stableTempDelay = 0;
    INT8U stableTemp = 0;
    
	err = 0;
	//Port error : 100 + Port number
	//Ex) Error number 122 => port 2.2 or 2.0 error because 2.2 is connected to 2.0 in JIG Base board
	//                        see TestConnTable[][]
	if (funcNum == 1) //Port
	{
        err = portTest();
	}
	else if (funcNum == 2) //AD CH1 Zero Value error : 200 ~ 202
	{
//		TEST_SPAN_CONTROL_PIN = 1;	//SPAN_CONTROL
        ad1230Set(1);
		prevJigTimer100ms = TimerGetSysTimer100ms();
		while((INT16U)(TimerGetSysTimer100ms() - prevJigTimer100ms) < 20)
		{
			AdProc(0, 0);
		}
			
		if(AdData.rawFilteredData < 10000)
		{
			err = 200; // AD CH1 Zero Value under range
			           // or AD Chip or AD CRYSTAL error
		}
		else if(AdData.rawFilteredData > 200000)     // 30000
		{
			err = 201; // AD CH1 Zero Value over range
		}
		else if(stableTemp < 15)
		{
//			err = 202; // AD CH1 Zero Value is unstable
		}
        ad1230Clear();
	}
	else if (funcNum == 3) //AD CH1 Span Value error : 300 ~ 302
	{
//		TEST_SPAN_CONTROL_PIN = 0;	//SPAN_CONTROL
        ad1230Set(0);
        prevJigTimer100ms = TimerGetSysTimer100ms();
		while(TimerGetSysTimer100ms() - prevJigTimer100ms < 20)
        {
            AdProc(0, 0);
        }
		if(AdData.rawFilteredData < 250000)
		{
			err = 300; // AD CH1 Span Value under range
		}
		else if(AdData.rawFilteredData > 600000)
		{
			err = 301; // AD CH1 Span Value under range
		}
		else if(!AdData.rawStableFlag)
		{
//			err = 302; // AD CH1 Span Value is unstable
		}
        ad1230Clear();
	}
	else if (funcNum == 4) //AD CH2 Low Value error : 400 ~ 401
	{
//		TEST_BATT_CONTROL_PIN = 1;	//BAT_CONTROL
        ad1230Set(1);
		prevJigTimer100ms = TimerGetSysTimer100ms();
        currTimer100ms = 0;
		while(TimerGetSysTimer100ms() - prevJigTimer100ms < 20)
        {
            currTimer100ms = TimerGetSysTimer100ms();
            BattDrvProc(currTimer100ms);
            
        }
        tempInt16u = BattDrvReadAd();

		if(tempInt16u < 200)        // 257
		{
			err = 400;// AD CH2 Low Value under range
			          // or AD Chip or AD CRYSTAL error
		}
		else if(tempInt16u > 800)
		{
			err = 401;// AD CH2 Low Value over range
		}
        ad1230Clear();
	}
	else if (funcNum == 5) //AD CH2 High Value error : 500 ~ 501
	{	
//		TEST_BATT_CONTROL_PIN = 0;	//BAT_CONTROL
        ad1230Set(0);
		prevJigTimer100ms = TimerGetSysTimer100ms();
        currTimer100ms = 0;
		while(TimerGetSysTimer100ms() - prevJigTimer100ms < 20)
        {
            currTimer100ms = TimerGetSysTimer100ms();
            BattDrvProc(currTimer100ms);
            
        }
        tempInt16u = BattDrvReadAd();       //440
	
		if(tempInt16u < 800)
		{
			err = 500;// AD CH2 High Value under range
		}
		if(tempInt16u > 1500)
		{
			err = 501;// AD CH2 High Value over range
		}
        ad1230Clear();
	}
	else if (funcNum == 6) // Temp Sensor : 600
	{	
		if (TempDrvInit() == 0)
		{
			err = 600;
		}
	}
	else if (funcNum == 7) // RTC Error : 700 ~ 701
	{	
		TimeData.year = 7;
		TimeData.month = 9;
		TimeData.date = 7;
		TimeData.day = 6;
		TimeData.hour = 11;
		TimeData.min = 52;
		TimeData.sec = 0;

		TimeWriteData();

		prevJigTimer100ms = TimerGetSysTimer100ms();
		while(TimerGetSysTimer100ms() - prevJigTimer100ms < 20)
		{
			AdProc(0, 0);
		}

		TimeReadData();
	
		if((TimeData.year != 7) || (TimeData.hour != 11))
		{
			err = 700; //RTC Error : 700
		} 
		else if(TimeData.sec == 0)
		{
			err = 701; //RTC Crystal Error : 701
		}

	}
	else if (funcNum == 8) //EEPROM Error : 800
	{	
		//TO DO : exp. eeprom
		StorageWriteByte(EXT_STORAGE_JIG_CHECK_ADDR, 0xAA);
		if(StorageReadByte(EXT_STORAGE_JIG_CHECK_ADDR) != 0xAA)
		{
			err = 800;
		}
		StorageWriteByte(EXT_STORAGE_JIG_CHECK_ADDR, 0x55);
		if(StorageReadByte(EXT_STORAGE_JIG_CHECK_ADDR) != 0x55)
		{
			err = 800;
		}
	}
END:
	KeyInit();
	return err;
}

#endif
