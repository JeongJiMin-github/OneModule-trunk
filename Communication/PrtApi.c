/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Printer API Source File\n
* @file    PrtApi.c
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
#include "../Common/UiDefine.h"
#include "../Db/StorageApi.h"
#include "../Db/ParamApi.h"
#include "CommDrv.h"
#include "PrtApi.h"

#if defined(USE_PRINTER) || defined(USE_SATO_PRINTER_PROTOCOL)
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
PRT_SETTING	PrtSetting;


/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */


/*
********************************************************************************
*                       LOCAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert local typedef & structure here */
#ifndef PRT_PUT_NUM_FILL_CHAR
#define PRT_PUT_NUM_FILL_CHAR 0
#endif


/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/
/* Insert file scope variable & tables here */
/** @brief Printer Driver Rx Ring Buffer를 받아오기 위한 Pointer */
static RING_BUF	*prtRxRingBufPtr;
/** @brief Printer Driver Tx Ring Buffer를 받아오기 위한 Pointer */
static RING_BUF	*prtTxRingBufPtr;
/** @brief Printer의 한줄에 쓸 수 있는 최대 글자수 */
static INT8U prtMaxTxBufSize;
/** @brief Printer의 한줄에 쓸 수 있는 최대 글자수 */
static INT8U prtMaxCharNumPerLine;

/** @brief Maximun Tab table size */
#define PRT_TAB_TABLE_MAX_SIZE	10
/** @brief 현재 Tab 번호 */
static INT8U prtTabNum;
/** @brief Tab 번호에 따른 위치 값 테이블 */
static INT8U prtTabTable[PRT_TAB_TABLE_MAX_SIZE];
/** @brief 현재 x좌표 위치 */
static INT8U prtCurrPos;

/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
static void prtCheckBufAndSend(void);

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
/**
********************************************************************************
* @brief    Printer API Parameter를 default 값으로 초기화\n
* @param    type : defualt type
* @return   none
* @remark   Parameter 영역에 초기값을 넣는다.\n
*           Cal Mode나 Program 모드에서 전체 초기화할 때 실행한다.
********************************************************************************
*/
void PrtSetDefaultParam(void)
{
	ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST, 0x78);
	//pgmPrintOffet = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST);
	//pgmPrintMethod = pgmPrintOffet >> 6;	//00: off, 01: key, 10: auto, 11: stream (Use two bits)
	//pgmDateOnOffFlag = (pgmPrintOffet & 0x20) >> 5;	//0: off, 1: on
	//pgmTimeOnOffFlag = (pgmPrintOffet & 0x10) >> 4;	//0: off, 1: on			
	//pgmLabelOnOffFlag = (pgmPrintOffet & 0x08) >> 3;	//0: off, 1: on
	ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_LF_SIZE, 9);
	ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_BARCODE_TYPE, TYPE_WEIGHT);
}

/**
********************************************************************************
* @brief    Printer API Init 함수\n
*              Printer Ring Buffer Init\n
* @param    none
* @return   none
* @remark   처음에 한번 실행 되는 함수\n
********************************************************************************
*/
void PrtInit(void)
{
	INT8U i;
	//CommDrvInit(BAUD_RATE_9600);//Same ad CommApi

#ifdef USE_UART1_PRINTER
	prtRxRingBufPtr = &SerialRx1RingBuf;
	prtTxRingBufPtr = &SerialTx1RingBuf;
#else
	prtRxRingBufPtr = &SerialRxRingBuf;
	prtTxRingBufPtr = &SerialTxRingBuf;
#endif
	prtMaxCharNumPerLine = 30;// TODO : 추후 프린터 종류에 따라 변경
	prtMaxTxBufSize = RingBufCheckMaxBufSize(prtTxRingBufPtr);

	prtTabNum = 0;
	prtCurrPos = 0;
	for (i = 0; i < PRT_TAB_TABLE_MAX_SIZE; i++)
	{
		prtTabTable[i] = 0;
	}
	i = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST);

#if defined(USE_PGM_SW) || defined(USE_PGM_PB) || defined(USE_PGM_FW)|| defined(USE_PGM_PDN)
	PrtSetting.method = i & 0x07;
  #ifdef USE_PGM_PB
	PrtSetting.dateUse = 0;
	PrtSetting.timeUse = 0;
  #endif
#ifdef USE_STABLE_ZERO_WEIGHT_PRT 
	// Print method (When Use S-send that means when stable, send weight)
	if(PrtSetting.method < PRT_USE_BY_AUTO || PrtSetting.method > PRT_USE_BY_STREAM)
	{
		PrtSetting.method = PRT_USE_BY_STREAM;
		i &= ~(0x07);
		i |= PRT_USE_BY_STREAM;
		ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST, i);
	}
#endif
#else
	PrtSetting.method = (i & 0xC0) >> 6;
	PrtSetting.dateUse = (i & 0x20) >> 5;
	PrtSetting.timeUse = (i & 0x10) >> 4;
	PrtSetting.lineFeedSize = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_LF_SIZE) + 1;
	PrtSetting.barcodeType = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_BARCODE_TYPE);
#endif	
	PrtSetting.barcodeOnOffFlag = (i & 0x08) >> 3;
}

/**
********************************************************************************
* @brief    Printer Procedure 함수\n
*           Printer Driver Procedure 호출\n
*           Printer Driver Ring Buffer 확인 후 처리
* @param    none
* @return   none
* @remark   일정 시간에 한번씩 계속 실행 되는 함수\n
*           현재 Protocol 처리 부분 없음 추가 필요 (TO DO)
********************************************************************************
*/
void PrtProc(void)
{
	prtCheckBufAndSend();
}

/**
********************************************************************************
* @brief    Printer Tab 설정 함수\n
* @param    tabNum : tab table 번호
*           tabPos : tab 위치 (012345...)
* @return   none
* @remark   일정 시간에 한번씩 계속 실행 되는 함수\n
*           현재 Protocol 처리 부분 없음 추가 필요 (TO DO)
********************************************************************************
*/
void PrtSetTabPos(INT8U tabNum, INT8U tabPos)
{
	if (prtTabNum >= PRT_TAB_TABLE_MAX_SIZE)
	{
		return;
	}
	prtTabTable[tabNum] = tabPos;
}

/**
********************************************************************************
* @brief    Printer Tab 설정 함수\n
* @param    tabNum : tab table 번호
*           tabPos : tab 위치 (012345...)
* @return   none
* @remark   일정 시간에 한번씩 계속 실행 되는 함수\n
*           현재 Protocol 처리 부분 없음 추가 필요 (TO DO)
********************************************************************************
*/
void PrtPutTab(INT8U tabCount)
{
	INT8U tabPos;
	INT8U i;

	prtTabNum = prtTabNum + tabCount;
	if (prtTabNum >= PRT_TAB_TABLE_MAX_SIZE)
	{
		return;
	}

	tabPos = prtTabTable[prtTabNum];
	if (prtCurrPos < tabPos)
	{
		for (i = prtCurrPos; i < tabPos; i++)
		{
			RingBufPutChar(prtTxRingBufPtr, ' ');
		}
		prtCurrPos = tabPos;
	}	
}

/**
********************************************************************************
* @brief    Write command to Printer Buffer\n
* @param    ch : source data
* @return   none
* @remark   
********************************************************************************
*/
void PrtPutCmd(char cmd)
{
    RingBufPutChar(prtTxRingBufPtr, cmd);
	prtCheckBufAndSend();
}

/**
********************************************************************************
* @brief    Write ch to Printer Buffer\n
* @param    ch : source data
* @return   none
* @remark   
********************************************************************************
*/
void PrtPutChar(char ch)
{
    RingBufPutChar(prtTxRingBufPtr, ch);
	prtCurrPos++;
	prtCheckBufAndSend();
}

/**
********************************************************************************
* @brief    Write string to Printer Buffer\n
* @param    src : source string pointer
* @return   none
* @remark   
********************************************************************************
*/
void PrtPutStr(char *src)
{
	while (*src != '\0') 
	{
		RingBufPutChar(prtTxRingBufPtr, *src);
		src++;
		prtCurrPos++;
    	}
	prtCheckBufAndSend();
}

/**
********************************************************************************
* @brief    Write data to Printer Buffer\n
* @param    src : source data pointer
*           size : Data size
* @return   none
* @remark   
********************************************************************************
*/
void PrtPutData(char *src, INT8U size)
{
    	RingBufPutData(prtTxRingBufPtr, src, size);
	prtCurrPos = prtCurrPos + size;
	prtCheckBufAndSend();
}

/**
********************************************************************************
* @brief    Write const string to Printer Buffer\n
* @param    src : source string pointer
* @return   none
* @remark   CR, LF 마지막에 보냄
********************************************************************************
*/
void PrtPutConstStr(const char *src)
{
    while (*src != '\0') 
	{
		RingBufPutChar(prtTxRingBufPtr, *src);
		src++;
		prtCurrPos++;
    }
	prtCheckBufAndSend();
}

/**
********************************************************************************
* @brief    Write string to Printer Buffer with LineFeed\n
* @param    src : source string pointer
* @return   none
* @remark   CR, LF 마지막에 보냄
********************************************************************************
*/
/*
void PrtPutStrWithCRLF(char *src)
{
    PrtPutStr(src);
	PrtPutCRLF(1);
}
*/
/**
********************************************************************************
* @brief    Write const string to Printer Buffer with LineFeed\n
* @param    src : source string pointer
* @return   none
* @remark   CR, LF 마지막에 보냄
********************************************************************************
*/
/*
void PrtPutConstStrWithCRLF(const char *src)
{
    PrtPutConstStr(src);
	PrtPutCRLF(1);
}
*/

/**
********************************************************************************
* @brief    Write Number Data to Printer Buffer\n
* @param    num : number data
* @return   none
* @remark   
********************************************************************************
*/
void PrtPutNum(INT32S num, INT8U size, INT8U decPos, INT8U decType, INT8U useThousandPt)
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

	Int32sToDecStr(str, num, size, PRT_PUT_NUM_FILL_CHAR, decPos, decChar, useThousandPt);
	PrtPutStr(str);
}

/**
********************************************************************************
* @brief    Write Decimal Number Data to Printer Buffer\n
* @param    num : number data, size : string size, type : number type (PRT_NUM_TYPE_WEIGHT...)
* @return   none
* @remark   
********************************************************************************
*/
void PrtPutDecNum(INT32S num, INT8U size, INT8U type)
{
	char str[10];
	char decChar;
	INT8U decPos;
	INT8U useThousandPt;
	INT8U decType;
	char fillChar;
		
	if (size > 10)
	{
		return;
	}
	
	switch (type)
	{
		case PRT_NUM_TYPE_WEIGHT: //TO DO : PrtSetWeightType, init variable
			decPos = 0;
			decType = 0;
			useThousandPt = 0;
			fillChar = 0;
			break;
		case PRT_NUM_TYPE_PRICE: //TO DO : PrtSetPriceType, init variable
			decPos = 0;
			decType = 0;
			useThousandPt = 0;
			fillChar = 0;
			break;
		case PRT_NUM_TYPE_FILL_ZERO:
			decPos = 0;
			decType = 0;
			useThousandPt = 0;
			fillChar = '0';
			break;
		default:
		case PRT_NUM_TYPE_FILL_SPACE:
			decPos = 0;
			decType = 0;
			useThousandPt = 0;
			fillChar = ' ';
			break;			
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
	PrtPutStr(str);
}


/**
********************************************************************************
* @brief    Write LineFeed to Printer Buffer\n
* @param    none
* @return   none
* @remark   CR, LF 마지막에 보냄
********************************************************************************
*/
void PrtPutCRLF(INT8U lineNum)
{
	INT8U i;
	
	for (i = 0; i < lineNum; i++)
	{
		PrtPutChar(0x0D);
		PrtPutChar(LF);
	}

	prtTabNum = 0;//Reset tab number
	prtCurrPos = 0;
}

/**
********************************************************************************
* @brief    Write Line string to Printer Buffer with LineFeed\n
* @param    none
* @return   none
* @remark   CR, LF 마지막에 보냄
********************************************************************************
*/
void PrtPutLine(void)
{
	INT8U i;

	for (i = 0; i < prtMaxCharNumPerLine; i++)
	{
		RingBufPutChar(prtTxRingBufPtr, '=');
	}
	PrtPutCRLF(1);
}

/**
********************************************************************************
* @brief    Write Printer Command to Printer Buffer\n
* @param    cmdType : Printer Command Type
* @return   none
* @remark   
********************************************************************************
*/
/*
void PrtPutCmd(char *cmdType)
{
    //RingBufPutStr(prtTxRingBufPtr, src);
}
*/

/**
********************************************************************************
* @brief    프린트 테스트 함수
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
/*
void PrtTest(void)
{
	PrtSetTabPos(0, 0);
	PrtSetTabPos(1, 2);
	PrtSetTabPos(2, 10);
	PrtSetTabPos(3, 19);

	PrtPutLine();
	PrtPutTab(1);//1
	PrtPutConstStr("PLU");
	PrtPutTab(1);//2
	PrtPutConstStr("UNIT");
	PrtPutTab(1);//3
	PrtPutConstStr("TOTAL");
	PrtPutCRLF(1);
	PrtPutTab(1);//1
	PrtPutStr("PLU 01");
	PrtPutTab(1);//2
	PrtPutNum(123456, 8, 2, DEC_TYPE_DOT, NO_USE_THOUSAND_PT);//" 1234.56"
	PrtPutTab(1);//3
	PrtPutNum(-1, 8, 1, DEC_TYPE_COMMA, NO_USE_THOUSAND_PT);  //"    -0.1"
	PrtPutCRLF(1);
	PrtPutTab(1);//1
	PrtPutStr("PLU 02");
	PrtPutTab(1);//2
	PrtPutNum(123456, 8, 2, DEC_TYPE_DOT, USE_THOUSAND_PT);    //" 1,234.56"
	PrtPutTab(1);//3
	PrtPutNum(-10000, 8, 1, DEC_TYPE_COMMA, USE_THOUSAND_PT);  //" -1,000.0"
	PrtPutCRLF(2);
	PrtPutLine();
	PrtPutConstStr("test string");
	PrtPutConstStrWithCRLF(" with CRLF");
	PrtPutStr("test string");
	PrtPutStrWithCRLF(" with CRLF");
	PrtPutLine();
}
*/
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
/**
********************************************************************************
* @brief    Printer로 보낼 데이터 Check 후 보내는 함수 \n
* @param    none
* @return   none
* @remark   실제 전송은 Interrupt에서 보냄
********************************************************************************
*/
void prtCheckBufAndSend(void)
{
	while (RingBufCountSize(prtTxRingBufPtr) > (prtMaxTxBufSize/2));	//대기
#ifdef USE_UART1_PRINTER
	Comm1DrvProc();
#else
	CommDrvProc();
#endif
}
#endif//#ifdef USE_PRINTER
