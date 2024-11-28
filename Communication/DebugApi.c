/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Debug Api Source File\n
* @file    DebugApi.c
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
#include "../Common/RingBuf.h"
#include "../Communication/CommDrv.h"

#ifdef USE_DEBUG_API
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
/** @brief Debug App.에서 사용할 Communication Driver Tx Ring Buffer Pointer */
static RING_BUF	*debugTxRingBufPtr;

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
* @brief    Debug Application Init 함수\n
*           Communication Ring Buffer 가져옴\n
* @param    none
* @return   none
* @remark   초기에 한번 실행 되는 함수\n
********************************************************************************
*/
void DebugInit(void)
{
	debugTxRingBufPtr = &SerialTxRingBuf;
}

/**
********************************************************************************
* @brief    Debug용 통신 포트로 Data를 보내는 함수
* @param    debugStr : Debug 통신 포트로 보낼 Data Pointer
*           size : Data size
* @return   none
* @remark   
********************************************************************************
*/
void DebugDataOut(char *debugData, INT16U size)
{
	RingBufPutData(debugTxRingBufPtr, debugData, size);
}



/**
********************************************************************************
* @brief    Debug용 통신 포트로 String을 보내는 함수
* @param    debugStr : Debug 통신 포트로 보낼 String
* @return   none
* @remark   
********************************************************************************
*/
void DebugStrOut(char *debugStr)
{
	RingBufPutStr(debugTxRingBufPtr, debugStr);
}
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

#endif//#ifdef USE_DEBUG_API