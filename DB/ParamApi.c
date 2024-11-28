/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Parameter Data API Source File\n
           for One Module
* @file    ParamApi.c
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
#include "../Communication/DebugApi.h"
#include "StorageApi.h"
#include "ParamApi.h"

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
/** 
* @brief Parameter Area Number로 부터 주소를 가져오기 위한 Table
* @remark 위치 변경은 함부로 하면 안됨, 정의 된 번호가 있음\n
*         see PARAM_AD_AREA_NUM...
*         Area 개수 변경 시 반드시 변경 할 것
*/
const INT16U code ParamBaseAddr[PARAM_MAX_AREA_NUM] = 
{
	PARAM_AD_BASE_ADDR,
	PARAM_HEAD_BASE_ADDR,
	PARAM_SALE_BASE_ADDR,
	PARAM_KEY_BASE_ADDR,
	PARAM_DISP_BASE_ADDR,
	PARAM_LOG_BASE_ADDR,
	PARAM_COMM_BASE_ADDR,
	PARAM_PRT_BASE_ADDR,
	PARAM_POWER_BASE_ADDR,
	PARAM_BUZZER_BASE_ADDR,
	PARAM_AUX_BASE_ADDR,
	PARAM_VERSION_BASE_ADDR
};

/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/
/* Insert file scope variable & tables here */


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
* @brief    Read Parameter Data\n
* @param    ParamAreaNum : Parameter Area Number (PARAM_AD_AREA_NUM ~ PARAM_AUX_AREA_NUM)
*           ParamOffset : byte offset from parameter area start address
*           dst : destination data pointer
*           size : data size
* @return   none
* @remark   
********************************************************************************
*/
void ParamReadData(INT16U ParamAreaNum, INT8U ParamOffset, INT8U *dst, INT16U size)
{
	StorageReadData(ParamBaseAddr[ParamAreaNum] + ParamOffset, dst, size);
}

/**
********************************************************************************
* @brief    Write Parameter Data\n
* @param    ParamAreaNum : Parameter Area Number (PARAM_AD_AREA_NUM ~ PARAM_AUX_AREA_NUM)
*           ParamOffset : byte offset from parameter area start address
*           src : source data pointer
*           size : data size
* @return   none
* @remark   
********************************************************************************
*/
void ParamWriteData(INT16U ParamAreaNum, INT8U ParamOffset, INT8U *src, INT16U size)
{
	StorageWriteData(ParamBaseAddr[ParamAreaNum] + ParamOffset, src, size);
}

/**
********************************************************************************
* @brief    Read Parameter Byte Data\n
* @param    ParamAreaNum : Parameter Area Number (PARAM_AD_AREA_NUM ~ PARAM_AUX_AREA_NUM)
*           ParamOffset : byte offset from parameter area start address
* @return   byte data
* @remark   
********************************************************************************
*/
INT8U ParamReadByte(INT16U ParamAreaNum, INT8U ParamOffset)
{
	return StorageReadByte(ParamBaseAddr[ParamAreaNum] + ParamOffset);
}

/**
********************************************************************************
* @brief    Write Parameter Byte Data\n
* @param    ParamAreaNum : Parameter Area Number (PARAM_AD_AREA_NUM ~ PARAM_AUX_AREA_NUM)
*           ParamOffset : byte offset from parameter area start address
*           byte : data for writing
* @return   none
* @remark   
********************************************************************************
*/
void ParamWriteByte(INT16U ParamAreaNum, INT8U ParamOffset, INT8U byte)
{
	StorageWriteByte(ParamBaseAddr[ParamAreaNum] + ParamOffset, byte);
}

/**
********************************************************************************
* @brief    Read Parameter word(INT16U) Data \n
* @param    ParamAreaNum : Parameter Area Number (PARAM_AD_AREA_NUM ~ PARAM_AUX_AREA_NUM)
*           ParamOffset : byte offset from parameter area start address
* @return   word data(INT16U)
* @remark   
********************************************************************************
*/
INT16U ParamReadWord(INT16U ParamAreaNum, INT8U ParamOffset)
{
	return StorageReadWord(ParamBaseAddr[ParamAreaNum] + ParamOffset);
}

/**
********************************************************************************
* @brief    Write Parameter word(INT16U) Data \n
* @param    ParamAreaNum : Parameter Area Number (PARAM_AD_AREA_NUM ~ PARAM_AUX_AREA_NUM)
*           ParamOffset : byte offset from parameter area start address
*           word : word data(INT16U)
* @return   none
* @remark   
********************************************************************************
*/
void ParamWriteWord(INT16U ParamAreaNum, INT8U ParamOffset, INT16U word)
{
	StorageWriteWord(ParamBaseAddr[ParamAreaNum] + ParamOffset, word);
}

/**
********************************************************************************
* @brief    Read Parameter double word(INT32U) Data \n
* @param    ParamAreaNum : Parameter Area Number (PARAM_AD_AREA_NUM ~ PARAM_AUX_AREA_NUM)
*           ParamOffset : byte offset from parameter area start address
* @return   double word data(INT32U)
* @remark   
********************************************************************************
*/
INT32U ParamReadDword(INT16U ParamAreaNum, INT8U ParamOffset)
{
	return StorageReadDword(ParamBaseAddr[ParamAreaNum] + ParamOffset);
}

/**
********************************************************************************
* @brief    Write Parameter double word(INT32U) Data \n
* @param    ParamAreaNum : Parameter Area Number (PARAM_AD_AREA_NUM ~ PARAM_AUX_AREA_NUM)
*           ParamOffset : byte offset from parameter area start address
*           dword : double word data(INT32U)
* @return   none
* @remark   
********************************************************************************
*/
void ParamWriteDword(INT16U ParamAreaNum, INT8U ParamOffset, INT32U dword)
{
	StorageWriteDword(ParamBaseAddr[ParamAreaNum] + ParamOffset, dword);
}
/*
#include <stdio.h>//for test
void ParamTest(void)
{
	char testStr[32];

	ParamReadData(PARAM_AUX_AREA_NUM, 0, testStr, 20);
	DebugDataOut(testStr, 20);
	sprintf(testStr, "A123456789B123456789");//20 byte
	ParamWriteData(PARAM_AUX_AREA_NUM, 0, testStr, 20);
}
*/
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
