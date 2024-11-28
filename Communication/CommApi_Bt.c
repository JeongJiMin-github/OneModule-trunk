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
/*
#include "../Common/Globals.h"
#include "../Common/CommonLib.h"
#include "../Common/RingBuf.h"
#include "../Common/SaleData.h"
#include "../Common/TimeData.h"
#include "../Common/AdData.h"
#include "../Time/TimeDrv.h"
#include "../Time/TimeApi.h"
#include "../Power/BattDrv.h"
#include "../Power/PowerDrv.h"
#include "../Power/PowerApi.h"
#include "../UI/DispApi.h"
#include "CommDrv.h"
#include "CommApi.h"
*/
#include "../Common/Globals.h"
#include "../Common/AdData.h"
#include "../Common/CommonLib.h"
#include "../Common/RingBuf.h"
#include "../Common/SaleData.h"
#include "../Common/TimeData.h"
#include "../Common/UiDefine.h"
#include "../Time/TimeDrv.h"
#include "../Time/TimeApi.h"
#include "../Power/BattDrv.h"
#include "../Power/PowerDrv.h"
#include "../Power/PowerApi.h"
#include "../UI/DispApi.h"
#include "../DB/StorageApi.h"
#include "../DB/ParamApi.h"
#include "../Ad/AdApi.h"
#include "../Ad/TempCompApi.h"
#include "../Ad/HystCompApi.h"
#include "../Ad/LinearCompApi.h"
#include "../Ad/CreepCompApi.h"
#include "../Application/ScaleModeApp.h"
#include "../Application/SaleModeApp.h"
#include "CommDrv.h"
#include "DebugApi.h"
#include "CommApi.h"
#include "PrtApi.h"
#include "../UI/BuzzerApi.h"

#include "../Hardware/HardwareConfig.h"


#include "../UI/MenuApi.h"

#ifdef USE_BT_COMM_API

#ifdef USE_TWN_FUNCS
/** @brief 현재 BT flag*/
INT8U BtResetFlag;
INT8U BtOnFlag;
INT8U BtErrFlag;
#endif

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
/** @brief Factory Communication Interpreter의 상태 변수 */
static BOOLEAN commDispOnOff;
/** @brief Communication Driver Rx Ring Buffer를 받아오기 위한 Pointer */
static RING_BUF	*commRxRingBufPtr;
/** @brief Communication Driver Tx Ring Buffer를 받아오기 위한 Pointer */
static RING_BUF	*commTxRingBufPtr;
/** @brief Communication Interpreter의 상태 변수 */
static INT8U commState;
static INT8U sendStatus=0;
static INT8U tempInt8u;
//static INT16U zb16uEditData;
static INT8U commDeviceNum;
//static INT8U comoutmode;
static INT8U commZigbeeChannel;
static INT16U commZigbeePanId;
static INT8U commUseZigbee;
static INT8U commKeyTareFlag;
//static btInitOk;
static char buf0;
static char buf1;
static char buf2;
static char buf3;
static char buf4;
static char buf5;

static char buf10;
static char buf11;
static char buf12;
static char buf13;
static char buf14;
static char buf15;

//static BOOLEAN zeroErrFlag;
static BOOLEAN tareErrFlag;

BOOLEAN BtCmdPowerOffFlag;
BOOLEAN BtCmdDispOffFlag;


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
static void commBtInterpreter(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
static void commBtSendWeight(RING_BUF *txRingBufPtr);
static void commBtSendSumWeight(RING_BUF *txRingBufPtr);
static void commBtSendGrossWeight(RING_BUF *txRingBufPtr);
static void commBtSendNetWeight(RING_BUF *txRingBufPtr);
static void commBtSendPrintForm(RING_BUF *txRingBufPtr);
static void commBtSendPrintForm1(RING_BUF *txRingBufPtr);

static void commBtReceiveTimeData(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
static void commBtSendTimeData(RING_BUF *txRingBufPtr);
static void commBtSendinitData(RING_BUF *txRingBufPtr);
static void commBtSendVolData(RING_BUF *txRingBufPtr);
static void commBtReceiveTareData(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);
static void commBtSendTareMsgReturn(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr);

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
* @brief    BT module리셋 및 모드변경 함수\n
* @param    none
* @return   none
* @remark   user set mode에서 설정하면 실행.\n
********************************************************************************
*/
void Btreset(void)
{
	RingBufPutChar(commTxRingBufPtr, 'A');
	RingBufPutChar(commTxRingBufPtr, 'T');
	RingBufPutChar(commTxRingBufPtr, '&');
	RingBufPutChar(commTxRingBufPtr, 'F');
	RingBufPutChar(commTxRingBufPtr, 0x0d);
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
	BtOnFlag = 0;
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
	tempInt8u = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST);
	commDeviceNum = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_DEVICE_NUM);	
	CommDrvInit(BAUD_RATE_DEFAULT);

	commRxRingBufPtr = &SerialRxRingBuf;
	commTxRingBufPtr = &SerialTxRingBuf;

	commState = 0;
	SaleData.sumSaveWeight =0;
	commDispOnOff=1;

#ifdef USE_TWN_FUNCS
	BtResetFlag = 0;
	BtOnFlag = 0;
	BtErrFlag = 0;
	BtCmdDispOffFlag = 1;
	BtCmdPowerOffFlag = 1;
	ZIGBEE_CON = 1;
#endif
}


/**
********************************************************************************
* @brief    Communication API Init 함수\n
*           Serial Communication Speed Init\n
*           Communication Ring Buffer Init\n
* @param    none
* @return   none
* @remark   ZIGBEE 설정 시  실행 되는 함수\n
********************************************************************************
*/
/*	  		//Zigbee 삭제
void ZigbeeInit(void)
{
	INT8U channel1, channel2;
	INT8U panid_1, panid_2, panid_3, panid_4;
	
	tempInt8u = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST);
	commZigbeeChannel = ParamReadByte(PARAM_SALE_AREA_NUM, PRT_NONE);
	commUseZigbee = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_RESERVED);
	
	CommDrvInit(BAUD_RATE_19200);
	commZigbeeChannel = commZigbeeChannel + 10;

	ZIGBEE_CON = 0;	
	
	commRxRingBufPtr = &SerialRxRingBuf;
	commTxRingBufPtr = &SerialTxRingBuf;
	commState = 0;
	SaleData.sumSaveWeight =0;
	commDispOnOff=1;

	//ZIGBEE SETTING
	channel2 = (INT8U)(commZigbeeChannel % 10) + 0x30;
	commZigbeeChannel = commZigbeeChannel / 10;
	channel1 = (INT8U)(commZigbeeChannel % 10) + 0x30;
	//CHANNEL SETTING
	RingBufPutChar(commTxRingBufPtr, 0x02);
	RingBufPutChar(commTxRingBufPtr, 'S');
	RingBufPutChar(commTxRingBufPtr, 'C');
	RingBufPutChar(commTxRingBufPtr, 'H');
	RingBufPutChar(commTxRingBufPtr, 0x30);
	RingBufPutChar(commTxRingBufPtr, 0x30);
	RingBufPutChar(commTxRingBufPtr, 0x30);
	RingBufPutChar(commTxRingBufPtr, channel1);
	RingBufPutChar(commTxRingBufPtr, channel2);
	RingBufPutChar(commTxRingBufPtr, 0x03);	

	
	commUseZigbee = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_RESERVED);
	commZigbeePanId = ParamReadWord(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PCS_VALUE);

	//PAN ID SETTING
	panid_1 = (commZigbeePanId / 1000) + 0x30;
	panid_2 = ((commZigbeePanId / 100) % 10) + 0x30;
	panid_3 = (((commZigbeePanId / 10) % 100) % 10) + 0x30;
	panid_4 = (((commZigbeePanId % 1000) % 100) % 10) + 0x30;
	RingBufPutChar(commTxRingBufPtr, 0x02);
	RingBufPutChar(commTxRingBufPtr, 'S');
	RingBufPutChar(commTxRingBufPtr, 'P');
	RingBufPutChar(commTxRingBufPtr, 'I');
	RingBufPutChar(commTxRingBufPtr, 0x30);
	RingBufPutChar(commTxRingBufPtr, panid_1);
	RingBufPutChar(commTxRingBufPtr, panid_2);
	RingBufPutChar(commTxRingBufPtr, panid_3);
	RingBufPutChar(commTxRingBufPtr, panid_4);
	RingBufPutChar(commTxRingBufPtr, 0x03);		
}
*/



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
   	if(tempInt8u == 1)
	{
		while (RingBufCheckBuf(commRxRingBufPtr))
		{
			commBtInterpreter(commRxRingBufPtr, commTxRingBufPtr);
		}
		CommDrvProc();
	}
	if((tempInt8u == 2) || (tempInt8u == 3))
	{
		commBtInterpreter(commRxRingBufPtr, commTxRingBufPtr);
		CommDrvProc();
	}
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
********************************************************************************
* @brief    Caston Send Weight 함수\n
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark  
********************************************************************************
*/
void commBtSendWeight(RING_BUF *txRingBufPtr)
{
	if(tempInt8u == 1)
	{
		char strBuf[14];
		char sendByte = 0x00;
		BOOLEAN error_check_flag;
		INT8U bcc=0;
		INT8U i;
		INT8U battLowFlag;	

		battLowFlag = BattDrvCheckLow();

		if (AdCheckInitZero())
		{
			if(SaleData.holdWeight) 
			{
				Int32sToDecStr(strBuf, AbsInt32s(SaleData.holdWeight), 5, ' ', 0, 0, 0);
			}
			else 
			{
				Int32sToDecStr(strBuf, AbsInt32s(AdData.weightData), 5, ' ', 0, 0, 0);
			}	
			for(i=0;i<5;i++)
			{
				strBuf[i+6] = strBuf[i];
				if(strBuf[i+6] == 0x20) strBuf[i+6] = 0x30;	 
			}
			strBuf[0] = 0x06;
			strBuf[1] = 'W';
			strBuf[2] = '0';
			strBuf[3] = '0';
			strBuf[4] = '0';
			strBuf[5] = '0';	

			if(AdData.overloadFlag || AdData.initZeroError || battLowFlag) 
			{
				error_check_flag = 1;
				if(AdData.overloadFlag ) sendByte |= 0x80;
				else 			          sendByte |= 0x00;
				if(AdData.initZeroError)  sendByte |= 0x04;
				else                               sendByte |= 0x00;
				if(battLowFlag)              sendByte |= 0x08;
				else                               sendByte |= 0x00;	
			}
			else 
			{
				sendByte = AdData.weightDecPos;
				sendByte |= 0x30;
				error_check_flag = 0;
			}
			strBuf[11] = sendByte;
   	
			sendByte = 0x80;
		
			if (AdData.tareWeight < 0)		sendByte = sendByte | 0x40;
			else                            sendByte = sendByte | 0x00;
			if (AdData.weightData < 0)  	sendByte = sendByte | 0x08;
			else                            sendByte = sendByte | 0x00;
			if (AdData.tareWeight)          sendByte = sendByte | 0x04;
			else                            sendByte = sendByte | 0x00;
			if( AdData.stableFlag )         sendByte = sendByte | 0x02;
			else                            sendByte = sendByte | 0x00;		
			if( AdData.grossZeroFlag )      sendByte = sendByte | 0x01;
			else                            sendByte = sendByte | 0x00;
			if(error_check_flag) 			sendByte = sendByte | 0x20;
			else                            sendByte = sendByte | 0x00;
			if(SaleData.holdWeight	)       sendByte = sendByte | 0x10;
			else                            sendByte = sendByte | 0x00;
		
			strBuf[12] = sendByte;
			for(i=1;i<13;i++) bcc ^= strBuf[i];		
			strBuf[13] = bcc;	
			RingBufPutData(txRingBufPtr, strBuf, 14);	
		}
	}
	if((tempInt8u == 2) || (tempInt8u == 3))	   // CAS 22Byte,
	{
		char strBuf[22];
		char sendByte = 0x00;
   		BOOLEAN error_check_flag;
		INT8U bcc=0;
		INT8U i , status_byte;
		INT8U battLowFlag;
		battLowFlag = BattDrvCheckLow();
	
		if(!sendStatus)	Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight),6, ' ', AdData.weightDecPos, '.', 0);

		else         							
//		Int32sToDecStr(strBuf, AbsInt32s(AdData.tareWeight  + SaleData.weight),6, ' ', 0, 0, 0);
		Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight),6, ' ', AdData.weightDecPos, '.', 0);
        if(SaleData.saleState == SALE_STATE_HOLD)	Int32sToDecStr(strBuf, AbsInt32s(SaleData.holdWeight),6, ' ', AdData.weightDecPos, '.', 0);

		for(i=0;i<6;i++)
		{
			strBuf[i+11] = strBuf[i]; 
		}	
		if(AdData.stableFlag)
		{
			strBuf[0] = 'S';
			strBuf[1] = 'T';
		}
		else if(AdData.overloadFlag)
		{
			strBuf[0] = 'O';
			strBuf[1] = 'L';
		}										
		else
		{
			strBuf[0] = 'U';
			strBuf[1] = 'S';
		}
		strBuf[2] = ',';
		if (AdData.tareWeight)// && !sendStatus )
		{	
			strBuf[3] = 'N';     
			strBuf[4] = 'T';
   		}
		else                            
		{
			strBuf[3] = 'G';     
			strBuf[4] = 'S';
		}
		strBuf[5] = ',';
		strBuf[6] = commDeviceNum +'0';							

		if(AdData.netZeroFlag || AdData.grossZeroFlag)		sendByte = 0x81;	//zero
		else if(SaleData.holdWeight)						sendByte = 0x90;	//HOLD
		else if(AdData.tareWeight)							sendByte = 0x82;	//TARE
		else												sendByte = 0x84;
		strBuf[7] = sendByte;

		strBuf[8] = ',';

		if (SaleData.weight < 0 && !sendStatus)		
		{	strBuf[9] = 0x2D;}
		else if((SaleData.weight) < 0 && sendStatus) 
		{	strBuf[9] = 0x2D;}
		else 
		{	strBuf[9] = 0x20;}	
/*
	if(AdData.weightDecPos)
	{
		strBuf[12+AdData.weightDecPos] = 0x2E;
		if(SaleData.weight<100) 
		{
			strBuf[13] = 0x30;
			strBuf[15] = 0x30;
		}
		else
		{
			for(i=0;i<3;i++)
			{
				strBuf[i+11] = strBuf[i+12]; 			
			}
   		}
	}

*/			
//     	strBuf[2] = ',';	
//		strBuf[5] = ',';
//		strBuf[6] = commDeviceNum +'0';
//		strBuf[7] = ' ';
//		strBuf[8] = ',';
		strBuf[10] = ' ';
		strBuf[17] = ' ';
		if(SaleData.weightUnit == 1)
		{
			strBuf[18] = 'l';     
			strBuf[19] = 'b';
		}
		else
		{
			strBuf[18] = 'k';     
			strBuf[19] = 'g';
		}
		strBuf[20] = 0x0d;
		strBuf[21] = 0x0a;
	
		RingBufPutData(txRingBufPtr, strBuf, 22);

	}

}  	

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
********************************************************************************
* @brief    Caston Send Weight 함수\n
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark  
********************************************************************************
*/
void commBtSendSumWeight(RING_BUF *txRingBufPtr)
{
	if(tempInt8u == 1)
	{
		char strBuf[14];
		char sendByte;
		BOOLEAN error_check_flag;
		INT8U bcc=0;
		INT8U i;	
		INT8U battLowFlag;	
	
		battLowFlag = BattDrvCheckLow();
	
		if(tempInt8u == 1)
		Int32sToDecStr(strBuf, AbsInt32s(SaleData.sumSaveWeight), 5, ' ', 0, 0, 0);
		
		
		for(i=0;i<5;i++)
		{
			strBuf[i+6] = strBuf[i];
			if(strBuf[i+6] == 0x20) strBuf[i+6] = 0x30;	
		}
		strBuf[0] = 0x06;
		strBuf[1] = 'D';
		strBuf[2] = '0';
		strBuf[3] = '0';
		strBuf[4] = '0';
		strBuf[5] = '0';	
	
		if(AdData.overloadFlag || AdData.initZeroError || battLowFlag) 
		{
			error_check_flag = 1;
			if(AdData.overloadFlag ) sendByte |= 0x80;
			else 			          sendByte |= 0x00;
			if(AdData.initZeroError)  sendByte |= 0x04;
			else                               sendByte |= 0x00;
			if(battLowFlag)              sendByte |= 0x08;
			else                               sendByte |= 0x00;	
		}
		else 
		{
			sendByte = AdData.weightDecPos;
			sendByte |= 0x30;
			error_check_flag = 0;
		}
		strBuf[11] = sendByte;
	
		sendByte = 0x80;
		
		if (AdData.tareWeight < 0)		sendByte = sendByte | 0x40;
		else                            sendByte = sendByte | 0x00;
		if(tempInt8u == 1)
		{
			if (AdData.weightData < 0)  	sendByte = sendByte | 0x08;
		}
		if(tempInt8u == 2)
		{
			if (SaleData.weight < 0)  		sendByte = sendByte | 0x08;
		}
		else                            sendByte = sendByte | 0x00;
		if (AdData.tareWeight)          sendByte = sendByte | 0x04;
		else                            sendByte = sendByte | 0x00;
		if( AdData.stableFlag )         sendByte = sendByte | 0x02;
		else                            sendByte = sendByte | 0x00;		
		if( AdData.grossZeroFlag )      sendByte = sendByte | 0x01;
		else                            sendByte = sendByte | 0x00;
		if(error_check_flag) 			sendByte = sendByte | 0x20;
		else                            sendByte = sendByte | 0x00;
		if(SaleData.holdWeight	)       sendByte = sendByte | 0x10;
		else                            sendByte = sendByte | 0x00;
		
		strBuf[12] = sendByte;
		for(i=1;i<13;i++) bcc ^= strBuf[i];		
		strBuf[13] = bcc;	
		RingBufPutData(txRingBufPtr, strBuf, 14);
	}	

}  	

/*
********************************************************************************
* @brief    Caston Send Time & date 함수\n
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark  
********************************************************************************
*/
void commBtSendGrossWeight(RING_BUF *txRingBufPtr)
{
	if(tempInt8u == 1)
	{
		char strBuf[14];
		char sendByte;
		BOOLEAN error_check_flag;
		INT8U bcc=0;
		INT8U i;	
		INT8U battLowFlag;	
	
		battLowFlag = BattDrvCheckLow();
	
		if(tempInt8u == 1)
		Int32sToDecStr(strBuf, AbsInt32s(AdData.tareWeight  + AdData.weightData), 5, ' ', 0, 0, 0);
		
		if(tempInt8u == 2)
		Int32sToDecStr(strBuf, AbsInt32s(AdData.tareWeight  + SaleData.weight), 5, ' ', 0, 0, 0);
		
		for(i=0;i<5;i++)
		{
			strBuf[i+6] = strBuf[i];
			if(strBuf[i+6] == 0x20) strBuf[i+6] = 0x30;	
		}
		strBuf[0] = 0x06;
		strBuf[1] = 'G';
		strBuf[2] = '0';
		strBuf[3] = '0';
		strBuf[4] = '0';
		strBuf[5] = '0';	
	
		if(AdData.overloadFlag || AdData.initZeroError || battLowFlag) 
		{
			error_check_flag = 1;
			if(AdData.overloadFlag ) sendByte |= 0x80;
			else 			          sendByte |= 0x00;
			if(AdData.initZeroError)  sendByte |= 0x04;
			else                               sendByte |= 0x00;
			if(battLowFlag)              sendByte |= 0x08;
			else                               sendByte |= 0x00;	
		}
		else 
		{
			sendByte = AdData.weightDecPos;
			sendByte |= 0x30;
			error_check_flag = 0;
		}
		strBuf[11] = sendByte;
	
		sendByte = 0x80;
		
		if (AdData.tareWeight < 0)		sendByte = sendByte | 0x40;
		else                            sendByte = sendByte | 0x00;
		if(tempInt8u == 1)
		{
			if (AdData.weightData < 0)  	sendByte = sendByte | 0x08;
		}
		if(tempInt8u == 2)
		{
			if (SaleData.weight < 0)  		sendByte = sendByte | 0x08;
		}
		else                            sendByte = sendByte | 0x00;
		if (AdData.tareWeight)          sendByte = sendByte | 0x04;
		else                            sendByte = sendByte | 0x00;
		if( AdData.stableFlag )         sendByte = sendByte | 0x02;
		else                            sendByte = sendByte | 0x00;		
		if( AdData.grossZeroFlag )      sendByte = sendByte | 0x01;
		else                            sendByte = sendByte | 0x00;
		if(error_check_flag) 			sendByte = sendByte | 0x20;
		else                            sendByte = sendByte | 0x00;
		if(SaleData.holdWeight	)       sendByte = sendByte | 0x10;
		else                            sendByte = sendByte | 0x00;
		
		strBuf[12] = sendByte;
		for(i=1;i<13;i++) bcc ^= strBuf[i];		
		strBuf[13] = bcc;	
		RingBufPutData(txRingBufPtr, strBuf, 14);
	}
	if((tempInt8u == 2) ||(tempInt8u == 3)) 
	{
		char strBuf[22];
		char sendByte=0x80;
   		BOOLEAN error_check_flag;
		INT8U bcc=0;
		INT8U i;
		INT8U battLowFlag;
		battLowFlag = BattDrvCheckLow();
	
		
		if(!sendStatus)	Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight),6, ' ', 0, 0, 0);
		else         							
		Int32sToDecStr(strBuf, AbsInt32s(AdData.tareWeight  + SaleData.weight),6, ' ',0, 0, 0);
//		Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight),6, ' ', 0, 0, 0);

		for(i=0;i<6;i++)
		{
			strBuf[i+11] = strBuf[i]; 
		}	
		if(AdData.stableFlag)
		{
			strBuf[0] = 'S';
			strBuf[1] = 'T';
		}
		else if(AdData.overloadFlag)
		{
			strBuf[0] = 'O';
			strBuf[1] = 'L';
		}										
		else
		{
			strBuf[0] = 'U';
			strBuf[1] = 'S';
		}

		if (AdData.tareWeight)// && !sendStatus )
		{	
			strBuf[3] = 'G';     
			strBuf[4] = 'S';
   		}
		else                            
		{
			strBuf[3] = 'G';     
			strBuf[4] = 'S';
		}
//		if (AdData.grossZeroFlag)		sendByte = sendByte | 0x01;
//		else                     		sendByte = sendByte | 0x00;

//		strBuf[7] = sendByte;
//		if(AdData.netZeroFlag || AdData.grossZeroFlag)	strBuf[7] = 0x01;	
//		else if(AdData.stableFlag)						strBuf[7] = 0x40;
//		else if(SaleData.holdWeight)					strBuf[7] = 0x10;
//		else if(AdData.tareWeight)						strBuf[7] = 0x02;
//		else											strBuf[7] = 0x04;

		if (SaleData.weight < 0 && !sendStatus)		
		{
			strBuf[8] = 0x2C;
			strBuf[9] = 0x2D;
		}
		else if((SaleData.weight) < 0 && sendStatus) 
		{
			strBuf[8] = 0x2C;
			strBuf[9] = 0x2D;
		}
		else 
		{
			strBuf[8] = 0x2C;
		   	strBuf[9] = 0x20;	
		}
/*
	if(AdData.weightDecPos)
	{
		strBuf[12+AdData.weightDecPos] = 0x2E;
		if(SaleData.weight<100) 
		{
			strBuf[13] = 0x30;
			strBuf[15] = 0x30;
		}
		else
		{
			for(i=0;i<3;i++)
			{
				strBuf[i+11] = strBuf[i+12]; 			
			}
   		}
	}

*/			
     	strBuf[2] = ',';	
		strBuf[5] = ',';
		strBuf[6] = commDeviceNum +'0';
		//strBuf[7] = ' ';
		strBuf[8] = 0x2C;
		strBuf[10] = ' ';
		strBuf[17] = ' ';
		if(SaleData.weightUnit == 1)
		{
			strBuf[18] = 'l';     
			strBuf[19] = 'b';
		}
		else
		{
			strBuf[18] = 'k';     
			strBuf[19] = 'g';
		}
		strBuf[20] = 0x0d;
		strBuf[21] = 0x0a;
	
		RingBufPutData(txRingBufPtr, strBuf, 22);
	}	
	
}

/*
********************************************************************************
* @brief    Caston Send Time & date 함수\n
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark  
********************************************************************************
*/
void commBtSendPrintForm(RING_BUF *txRingBufPtr)
{
	char strBuf[23];
	int counter=0;
	char sendByte=0xa0;
   	BOOLEAN error_check_flag;
	INT8U bcc=0;
	INT8U i;
	INT8U battLowFlag;
	battLowFlag = BattDrvCheckLow();
	
	if(!sendStatus)	Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight),6, ' ', 0, 0, 0);
	else         							
	Int32sToDecStr(strBuf, AbsInt32s(AdData.tareWeight  + SaleData.weight),6, ' ', 0, 0, 0);
//	Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight),6, ' ', 0, 0, 0);
	for(i=0;i<6;i++)
	{
		strBuf[i+10] = strBuf[i]; 
	}
		
	if (strBuf < 0 && !sendStatus)		strBuf[9] = 0x2D;
	else if((strBuf) < 0 && sendStatus) strBuf[9] = 0x2D;
	else										 strBuf[9] = 0x20;

    strBuf[0] = 'G';	
	strBuf[1] = 'r';
	strBuf[2] = 'o';
	strBuf[3] = 's';
	strBuf[4] = 's';
	strBuf[5] = ':';
	strBuf[6] = ' ';
	strBuf[7] = ' ';     
	strBuf[8] = ' ';
//	strBuf[9] = ' ';
	strBuf[16] = ' ';
	strBuf[17] = 'k';
	strBuf[18] = 'g';
	strBuf[19] = ' ';
	strBuf[20] = ' ';
	strBuf[21] = 0x0d;
	strBuf[22] = 0x0a;	

	RingBufPutData(txRingBufPtr, strBuf, 23);
	strBuf[9] = 0x20;


	Int32sToDecStr(strBuf, AbsInt32s(AdData.tareWeight),6, ' ', 0, 0, 0);
	for(i=0;i<6;i++)
	{
		strBuf[i+10] = strBuf[i]; 
	}

	
	strBuf[0] = 'T';	
	strBuf[1] = 'a';
	strBuf[2] = 'r';
	strBuf[3] = 'e';
	strBuf[4] = ':';
	strBuf[5] = ' ';
	strBuf[6] = ' ';
	strBuf[7] = ' ';     
	strBuf[8] = ' ';
	strBuf[9] = ' ';
	strBuf[16] = ' ';
	strBuf[17] = 'k';
	strBuf[18] = 'g';
	strBuf[19] = ' ';
	strBuf[20] = ' ';
	strBuf[21] = 0x0d;
	strBuf[22] = 0x0a;
	
	RingBufPutData(txRingBufPtr, strBuf, 23);

	if(!sendStatus)	Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight),6, ' ', 0, 0, 0);
	else         							
	Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight),6, ' ', 0, 0, 0);
//	Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight),6, ' ', 0, 0, 0);
	for(i=0;i<6;i++)
	{
		strBuf[i+10] = strBuf[i]; 
	}
	if (SaleData.weight < 0 && !sendStatus)		strBuf[9] = 0x2D;
	else if((SaleData.weight) < 0 && sendStatus) strBuf[9] = 0x2D;
	else										 strBuf[9] = 0x20;

		
	strBuf[0] = 'N';	
	strBuf[1] = 'e';
	strBuf[2] = 't';
	strBuf[3] = ':';
	strBuf[4] = ' ';
	strBuf[5] = ' ';
	strBuf[6] = ' ';
	strBuf[7] = ' ';     
	strBuf[8] = ' ';
//	strBuf[9] = ' ';
	strBuf[16] = ' ';
	strBuf[17] = 'k';
	strBuf[18] = 'g';
	strBuf[19] = ' ';
	strBuf[20] = ' ';
	strBuf[21] = 0x0d;
	strBuf[22] = 0x0a;	

	RingBufPutData(txRingBufPtr, strBuf, 23);
   	strBuf[9] = 0x20;
}	
/*
********************************************************************************
* @brief    Caston Send Time & date 함수\n
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark  
********************************************************************************
*/
void commBtSendPrintForm1(RING_BUF *txRingBufPtr)
{
	char strBuf[23];
	char sendByte=0xa0;
   	BOOLEAN error_check_flag;
	INT8U bcc=0;
	INT8U i;
	INT8U battLowFlag;
	battLowFlag = BattDrvCheckLow();
	
	

	
	if (AdData.grossZeroFlag)		sendByte = sendByte | 0x01;
	else                     		sendByte = sendByte | 0x00;

	if (SaleData.weight < 0 && !sendStatus)		strBuf[9] = 0x2D;
//	else if((AdData.tareWeight  + SaleData.weight) < 0 && sendStatus) strBuf[9] = 0x2D;
	else if((SaleData.weight) < 0 && sendStatus) strBuf[9] = 0x2D;
	else                     	            	strBuf[9] = 0x20;	
	
    
}	

/*
********************************************************************************
* @brief    Caston Send Time & date 함수\n
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark  
********************************************************************************
*/
void commBtSendNetWeight(RING_BUF *txRingBufPtr)
{

	//static INT8U tempInt8u;
	char strBuf[14];
	char sendByte;
	BOOLEAN error_check_flag;
	INT8U bcc=0;
	INT8U i;	
	INT8U battLowFlag;	

	battLowFlag = BattDrvCheckLow();

	if(tempInt8u == 1)
	Int32sToDecStr(strBuf, AbsInt32s(AdData.weightData), 5, ' ', 0, 0, 0);	
	

	for(i=0;i<5;i++)
	{
		strBuf[i+6] = strBuf[i];
		if(strBuf[i+6] == 0x20) strBuf[i+6] = 0x30;	
	}
	strBuf[0] = 0x06;
	strBuf[1] = 'N';
	strBuf[2] = '0';
	strBuf[3] = '0';
	strBuf[4] = '0';
	strBuf[5] = '0';	

	if(AdData.overloadFlag || AdData.initZeroError || battLowFlag) 
	{
		error_check_flag = 1;
		if(AdData.overloadFlag ) sendByte |= 0x80;
		else 			          sendByte |= 0x00;
		if(AdData.initZeroError)  sendByte |= 0x04;
		else                               sendByte |= 0x00;
		if(battLowFlag)              sendByte |= 0x08;
		else                               sendByte |= 0x00;	
	}
	else 
	{
		sendByte = AdData.weightDecPos;
		sendByte |= 0x30;
		error_check_flag = 0;
	}
	strBuf[11] = sendByte;

	sendByte = 0x80;
	
	if (AdData.tareWeight < 0)		sendByte = sendByte | 0x40;
	else                            sendByte = sendByte | 0x00;
	if(tempInt8u == 1)
	{
		if (AdData.weightData < 0)  	sendByte = sendByte | 0x08;
	}
	//if(tempInt8u == 2)
//	{
//		if (SaleData.weight < 0)  		sendByte = sendByte | 0x08;
//	}
	else                            sendByte = sendByte | 0x00;
	if (AdData.tareWeight)          sendByte = sendByte | 0x04;
	else                            sendByte = sendByte | 0x00;
	if( AdData.stableFlag )         sendByte = sendByte | 0x02;
	else                            sendByte = sendByte | 0x00;		
	if( AdData.grossZeroFlag )      sendByte = sendByte | 0x01;
	else                            sendByte = sendByte | 0x00;
	if(error_check_flag) 			sendByte = sendByte | 0x20;
	else                            sendByte = sendByte | 0x00;
	if(SaleData.holdWeight	)       sendByte = sendByte | 0x10;
	else                            sendByte = sendByte | 0x00;
	
	strBuf[12] = sendByte;
	for(i=1;i<13;i++) bcc ^= strBuf[i];		
	strBuf[13] = bcc ;	
	RingBufPutData(txRingBufPtr, strBuf, 14);

}
/*
********************************************************************************
* @brief    Caston Send Time & date 함수\n
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark  
********************************************************************************
*/

void commBtReceiveTimeData(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr)
{
		char i=1, strBuf[14], sendBuf[1];
		INT8U bcc=0;
		strBuf[0] = 0x79;
	    while(i<14)
		{
			if (RingBufCheckBuf(rxRingBufPtr))
			{
				 strBuf[i] =  RingBufGetChar(rxRingBufPtr);
				 i++;
			}

		}
		for(i=0;i<13;i++) bcc ^= strBuf[i];	
		if(strBuf[13] == bcc)
		{
			sendBuf[0] = 0x06;
		    	RingBufPutData(txRingBufPtr, sendBuf, 1);
			TimeData.year = ((strBuf[1]-0x30)*10)+(strBuf[2]-0x30);
			TimeData.month = ((strBuf[3]-0x30)*10)+(strBuf[4]-0x30);
			TimeData.date = ((strBuf[5]-0x30)*10)+(strBuf[6]-0x30);
			TimeData.hour = ((strBuf[7]-0x30)*10)+(strBuf[8]-0x30);
			TimeData.min = ((strBuf[9]-0x30)*10)+(strBuf[10]-0x30);
			TimeData.sec = ((strBuf[11]-0x30)*10)+(strBuf[12]-0x30);
			TimeData.day = 6;		
			TimeWriteData();	
		}
		else
		{
			sendBuf[0] = 0x15;
		    RingBufPutData(txRingBufPtr, sendBuf, 1);
		}
}

/*
********************************************************************************
* @brief    Caston Send Time & date 함수\n
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark  
********************************************************************************
*/
void commBtReceiveTareData(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr)
{
	char i=0, j=0, strBuf[6], sendBuf[1], byte;
	INT32S tare=0, tare1=0, tare_back;
	INT8U bcc=0;

	while(i<5)
	{
		if (RingBufCheckBuf(rxRingBufPtr))
		{
			strBuf[i] =  RingBufGetChar(rxRingBufPtr);
			byte  =  strBuf[0];
			if(byte == 0x0d)
			{
				commKeyTareFlag = 1;
				break;
				
			}
			else if(byte == 0x30 )
			{	
			 	tare1 = ((strBuf[0]-'0')*10000)+((strBuf[1]-'0')*1000)+((strBuf[2]-'0')*100)+((strBuf[3]-'0')*10)+(strBuf[4]-'0'); //+(strBuf[5]-'0');
			 	i++;
				commKeyTareFlag = 0;
			}
			
		}
	}
	i=0;

	if(commKeyTareFlag == 0)	  // Key Tare 값 입력시
	{	
//		if(tare1 == tare)
//		{
//			strBuf[0] = 'N';
//			strBuf[1] = 'G';
//			RingBufPutData(txRingBufPtr, strBuf, 2);
//			BuzzerSetOnTimes(2);
		//	commKeyTareFlag = 1;
//		}
		if ((SaleData.weight <= 0) && (AdData.stableFlag))
		{
			saleWeightTareProc();
			buf0 = strBuf[0];
			buf1 = strBuf[1];
			buf2 = strBuf[2];
			buf3 = strBuf[3];
			buf4 = strBuf[4];
				
			if(tare1 < 10) 
			{
				buf0 = 0x20; buf1 = 0x20; buf2 = 0x20; buf3 = 0x20;// buf4 = 0x20;
			}
			if((tare1 < 100) && (tare1 >= 10)) 
			{
				buf0 = 0x20; buf1 = 0x20; buf2 = 0x20; //buf3 = 0x20; 
			}
			if((tare1 < 1000) && (tare1 >= 100))
			{
				buf0 = 0x20; buf1 = 0x20;// buf2 = 0x20; 
			}
			if((tare1 < 10000) && (tare1 >= 1000))
			{
				buf0 = 0x20; //buf1 = 0x20; 
			}
		}
		else
		{
			strBuf[0] = 'N';
			strBuf[1] = 'G';
			RingBufPutData(txRingBufPtr, strBuf, 2);
			BuzzerSetOnTimes(2);
		}
	}

	tare = tare1;
	if(commKeyTareFlag == 0)
	{								
		if ((SaleData.weight <= 0) && (AdData.stableFlag))
		{
			AdSetPT(tare, 0);
		}
	}
	else	// KT 명령시.(읽기 명령.)
	{

		strBuf[0] = commDeviceNum;	
		strBuf[1] = 'K';
		strBuf[2] = 'T';
		strBuf[3] = ' ';
								
		strBuf[4] = ' ';
		strBuf[5] = buf0;
		strBuf[6] = buf1;
		strBuf[7] = buf2;
		strBuf[8] = buf3;
		strBuf[9] = buf4;
			
		strBuf[10] = ' ';
		strBuf[11] = 0x0d;
		strBuf[12] = 0x0a;
		RingBufPutData(txRingBufPtr, strBuf, 13);
		
		
	}
	i = 0;
	
}

/*
********************************************************************************
* @brief    Caston Send Time & date 함수\n
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark  
********************************************************************************
*/

void commBtSendTimeData(RING_BUF *txRingBufPtr)
{
	if(tempInt8u == 1)
	{
		char strBuf[14], i;
		INT8U bcc=0;
    	TimeReadData();
		strBuf[0] = 0x06;
		strBuf[1] = (TimeData.year/10)+0x30;
		strBuf[2] = (TimeData.year%10)+0x30;
		strBuf[3] = (TimeData.month/10)+0x30;
		strBuf[4] = (TimeData.month%10)+0x30;
		strBuf[5] = (TimeData.date/10)+0x30;
		strBuf[6] = (TimeData.date%10)+0x30;
		strBuf[7] = (TimeData.hour/10)+0x30;
		strBuf[8] = (TimeData.hour%10)+0x30;
		strBuf[9] = (TimeData.min /10)+0x30;
		strBuf[10] = (TimeData.min %10)+0x30;
		strBuf[11] = (TimeData.sec /10)+0x30;
		strBuf[12] = (TimeData.sec %10)+0x30;
		
		for(i=1;i<13;i++) bcc ^= strBuf[i];	
		strBuf[13]	= bcc;
		RingBufPutData(txRingBufPtr, strBuf, 14);	
	}	
}

 
/*
********************************************************************************
* @brief    Caston Send Voltage 함수\n
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark  
********************************************************************************
*/
void commBtSendinitData(RING_BUF *txRingBufPtr)
{
	if(tempInt8u == 1)
	{
		char strBuf[2];
		strBuf[0] = 0x06;
		strBuf[1] = 0x40;	
		RingBufPutData(txRingBufPtr, strBuf, 2);	
	}
}

/*
********************************************************************************
* @brief    Caston Send Voltage 함수\n
* @param    txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark  
********************************************************************************
*/
void commBtSendVolData(RING_BUF *txRingBufPtr)
{
	if(tempInt8u == 1)
	{
		char strBuf[14];
		INT16U tempInt16u;		
		char sendByte;
		BOOLEAN error_check_flag;
		INT8U bcc=0;
		INT8U i;
		INT8U battLowFlag;	

		battLowFlag = BattDrvCheckLow();

		tempInt16u = PowerReadBattVolt();
		Int32sToDecStr(strBuf, (INT32S)tempInt16u, 4, ' ', 0, 0, 0);
		
		for(i=0;i<4;i++)
		{
			strBuf[i+7] = strBuf[i];
			if(strBuf[i+7] == 0x20) strBuf[i+7] = 0x30;	 
		}
		strBuf[0] = 0x06;
		strBuf[1] = 'W';
		strBuf[2] = '0';
		strBuf[3] = '0';
		strBuf[4] = '0';
		strBuf[5] = '0';	
   	
		if(AdData.overloadFlag || AdData.initZeroError || battLowFlag) 
		{
			error_check_flag = 1;
			if(AdData.overloadFlag ) sendByte |= 0x80;
			else 			          sendByte |= 0x00;
			if(AdData.initZeroError)  sendByte |= 0x04;
			else                               sendByte |= 0x00;
			if(battLowFlag)              sendByte |= 0x08;
			else                               sendByte |= 0x00;	
		}
		else 
		{
			sendByte = AdData.weightDecPos;
			sendByte |= 0x30;
			error_check_flag = 0;
		}
		strBuf[11] = sendByte;

		sendByte = 0x80;
		
		if (AdData.tareWeight < 0)		sendByte = sendByte | 0x40;
		else                            sendByte = sendByte | 0x00;
		if (AdData.weightData < 0)  	sendByte = sendByte | 0x08;
		else                            sendByte = sendByte | 0x00;
		if (AdData.tareWeight)          sendByte = sendByte | 0x04;
		else                            sendByte = sendByte | 0x00;
		if( AdData.stableFlag )         sendByte = sendByte | 0x02;
		else                            sendByte = sendByte | 0x00;		
		if( AdData.grossZeroFlag )      sendByte = sendByte | 0x01;
		else                            sendByte = sendByte | 0x00;
		if(error_check_flag) 			sendByte = sendByte | 0x20;
		else                            sendByte = sendByte | 0x00;
		if(SaleData.holdWeight	)       sendByte = sendByte | 0x10;
		else                            sendByte = sendByte | 0x00;
	
		strBuf[12] = sendByte;
		for(i=1;i<13;i++) bcc ^= strBuf[i];		
		strBuf[13] = bcc;	
		RingBufPutData(txRingBufPtr, strBuf, 14);	
	}
}

/**
********************************************************************************
* @brief    Bluetooth communication Interpreter 함수\n
* @param    rxRingBufPtr : 받은 데이터가 있는 Ring Buffer Pointer
*           txRingBufPtr : 보낼 데이터를 넣을 Ring Buffer Pointer
* @return   none
* @remark   
*     
********************************************************************************
*/
void commBtInterpreter(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr)
{
	char byte, strBuf[14];

	if (RingBufCheckBuf(rxRingBufPtr))
	{
		byte = RingBufGetChar(rxRingBufPtr);
		//if(tempInt8u == 2 && (!DP_err_Flag))	   				// RS-232 Commmunication Command Mode
		//if((tempInt8u == 2) && (AdData.stableFlag) && (AdCheckInitZero()))
		if((tempInt8u == 2) && (AdCheckInitZero()))
		{	
			switch(commState)
			{
				case 0:
					if(byte == commDeviceNum) 
					{
						if(AdData.overloadFlag == 1)
						{  	
							strBuf[0] = 'O';
							strBuf[1] = 'V';
							strBuf[2] = 'E';
							strBuf[3] = 'R';
							//RingBufPutData(txRingBufPtr, strBuf, 4); 
							commState ++;
						}
						else
						{
								commState++;
						}  
					}
					else if (byte == ESC) //for Factory
					{
						commState = 200;
					}

					
					break;
				case 1:
					if(byte==0x57)	      // WT
					{
						commState++;
					}
					else if(byte==0x5a)   // Zero
					{
						commState++;
					}
					else if(byte==0x54)	  // Tare
					{
						commState++;
					}
					else if(byte==0x48)   // Hold
					{
						commState++;
					}
					else if(byte==0x4b)	   // Key Tare
					{
						commState = 3;
					}	
					else if(byte==0x50)	  
					{
						commState++;
					}
					else if(byte==0x39)	  
					{
						commState++;
					}
			
					else commState=0;
					break;
				case 2:
					if((byte==0x54) && BtCmdDispOffFlag && BtCmdPowerOffFlag)
					{
						commBtSendWeight(txRingBufPtr);
						commState=0;
					}
					else if(byte==0x54)
					{
						BtCmdDispOffFlag = 1;
					}
					else if(byte==0x4F)					  //pw ON
					{	
						if(!BtCmdDispOffFlag)
						{
							BtCmdDispOffFlag = 1;
						}
					}
					else if(byte==0x4E)					   //pw OFF
					{	
						if(BtCmdDispOffFlag)
						{
							BtCmdDispOffFlag = 0;
						}
					}
					else if(byte==0x30)					   //DP OFF
					{
						if(BtCmdPowerOffFlag)	{BtCmdPowerOffFlag=0;	PowerOffFlag=1; }
						else	{BtCmdPowerOffFlag=1;	PowerOffFlag=0;}
					}
					
					
					else if(byte== 0x45)	  // ZERO
					{
						SaleZeroExtKey(); 
						saleZeroKeyProc();
						if(GetSaleInitZeroErrFlagState() == 0)
						{ 					
							strBuf[0] = 'N';
							strBuf[1] = 'G';
							RingBufPutData(txRingBufPtr, strBuf, 2);
						}
						if(GetSaleInitZeroErrFlagState == 1)
						{ 					
							strBuf[0] = 'O';
							strBuf[1] = 'K';
							RingBufPutData(txRingBufPtr, strBuf, 2);
						}
						
						commState=0;

					}
					else if(byte== 0x52)	  // TARE
					{
						SaleTareExtKey();
						 saleWeightTareProc();
						if(GetSaleInitZeroErrFlagState() == 0)
						{ 					
							strBuf[0] = 'N';
							strBuf[1] = 'G';
							RingBufPutData(txRingBufPtr, strBuf, 2);
						}
						if(GetSaleInitZeroErrFlagState() == 1)
						{ 					
							strBuf[0] = 'O';
							strBuf[1] = 'K';
							RingBufPutData(txRingBufPtr, strBuf, 2);
						}
						commState=0;
					}
					else if(byte== 0x44)   // HOLD
					{
						SaleHoldExtKey();
						strBuf[0] = 'O';
						strBuf[1] = 'K';
						RingBufPutData(txRingBufPtr, strBuf, 2);
						commState=0;
					}
					else if(byte== 0x42)   // HOLD
					{
						
						commState=0;
					}
					else if(byte== 0x51)   // Change ID
					{
						commState++;
					}
					else commState=0;
					break;
				case 3:				
					if(byte== 0x52)			  // PR
					{
						commBtSendPrintForm(txRingBufPtr);
						commState=0;
					}
					else if (byte== 0x54)	  // Key Tare
					{
						commBtReceiveTareData(rxRingBufPtr,txRingBufPtr);
						commState=0;
						if(commKeyTareFlag == 0)
						{
							if ((SaleData.weight <= 0) && (AdData.stableFlag))
							{
								strBuf[0] = commDeviceNum;	
								strBuf[1] = 'K';
								strBuf[2] = 'T';
								strBuf[3] = ' ';
										
								strBuf[4] = ' ';
								strBuf[5] = buf0;
								strBuf[6] = buf1;
								strBuf[7] = buf2;
								strBuf[8] = buf3;
								strBuf[9] = buf4;
					   	
								strBuf[10] = ' ';
								strBuf[11] = 0x0d;
								strBuf[12] = 0x0a;
								RingBufPutData(txRingBufPtr, strBuf, 13);
							}
						}	
					}
					
					else commState=0;
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
		else
		{
//			DP_err_Flag = 0;
		}

		if(tempInt8u == 1)					  // Blue Tooth Commmunication Mode
		{
			switch(commState)
			{
				case 0:
					if(byte==ENQ) 
					{
						commState++;
					}
					else if(byte =='O') 
					{
						commState = 100;
					}
					else if (byte == ESC) //for Factory
					{
						commState = 200;
					}
					break;
				case 1:
					if(byte==0x57)
					{
						commBtSendWeight(txRingBufPtr);

				//	commBtSendNetWeight(txRingBufPtr);

						commState=0;
					}
					else if(byte==0x44) // sum data demand
					{
						commBtSendSumWeight(txRingBufPtr);
						commState=0;
					}
					else if(byte==0x53) // sum 	
					{
						saleSumKey();
						commState=0;
					}
					else if(byte==0x43) // sum clear 	
					{
						saleClearKey();
						commState=0;
					}
					else if(byte==0x47) 
					{
						if(!SaleData.holdWeight) 
						{
							commBtSendGrossWeight(txRingBufPtr);
							commState=0;
						}
					}
					else if(byte==0x4e) 
					{
						if(!SaleData.holdWeight) 
						{
							commBtSendNetWeight(txRingBufPtr);
							commState=0;
						}
					}
					else if(byte==0x5a) 
					{
						SaleZeroExtKey();
						commState=0;
					}
					else if(byte==0x54)
					{
						SaleTareExtKey();
						commState=0;
					}
					else if(byte==0x46)
					{
						commDispOnOff ^= 1;
						DispTurnOnOff(commDispOnOff);
						commState=0;
					}
					else if(byte==0x48) 
					{
						SaleHoldExtKey();
						commState=0;
					}
					else if(byte==0x79) 
					{
						commBtReceiveTimeData(rxRingBufPtr,txRingBufPtr);
						commState=0;
					}
					else if(byte==0x59)
					{
					   	commBtSendTimeData(txRingBufPtr);
						commState=0;
					}
					else if(byte==0x56) 
					{
						commBtSendVolData(txRingBufPtr);
						commState=0;
					}
					else if(byte==0x58) 
					{
						commBtSendinitData(txRingBufPtr);
						commState=0;
					}
					else commState=0;
					break;	
				
				case 100:
				if (byte == 'K') 
				{
//					DispSetConstStrMsg(WIN_WEIGHT, "RESET");
//					DispSetRunMsgPage(10, 1);
					commState = 0;
					BtOnFlag = 1;
					commState = 0;
				}
				else commState = 0;
				break;
				/*
				case 100:
					if (byte == 'K')
					{
						btInitOk = 1;
					}
					else commState = 0;
				*/
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
	else
	{
		if(tempInt8u == 3)
		{
			commBtSendWeight(txRingBufPtr);
			commState=0;
		}
   			
	}
	
}


void commBtSendTareMsgReturn(RING_BUF *rxRingBufPtr, RING_BUF *txRingBufPtr)
{
	char i = 0, strBuf[13];
		
	while(i<6)
	{
		if(RingBufCheckBuf(rxRingBufPtr))
		{
			strBuf[i+4] =  RingBufGetChar(rxRingBufPtr);
//			strBuf[i+4] = strBuf[i];
			i++; 
		}
	}
	i = 0;	
	
	strBuf[0] = commDeviceNum;	
	strBuf[1] = 'K';
	strBuf[2] = 'T';
	strBuf[3] = ' ';
	strBuf[10] = ' ';
	strBuf[11] = 0x0d;
	strBuf[12] = 0x0a;	

	RingBufPutData(txRingBufPtr, strBuf, 13);
}


#endif



