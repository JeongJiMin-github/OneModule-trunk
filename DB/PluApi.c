/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   PLU API Source File\n
* @file    PluApi.c
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
#include "../Common/PluData.h"
#include "../Common/CommonLib.h"
#include "../Communication/DebugApi.h"
#include "StorageApi.h"
#include "PluApi.h"

#if defined (USE_SALE_PRICE) || defined (USE_SALE_ER_MLED) || defined (USE_SALE_PRICE_COMPARE_PLU) || defined (USE_SALE_PR)
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
* @brief    Erase(Clear) All PLU data and Report
* @param    none
* @return   none
* @remark   PLU data 영역, 정산 영역 모두 0x00으로 초기화 한다.
********************************************************************************
*/
void PluClearAllPlu(void)
{
	INT16U i;
	INT8U j;
	DIRECT_PLU_DATA directPluData;
	INDIRECT_PLU_DATA indirectPluData;

	directPluData.unitPrice = 0;
	directPluData.tare = 0;
	
	for (j = 0; j < PLU_NAME_SIZE; j++)
	{
		directPluData.name[j] = 0x20;
	}

	for (i = 1; i <= DIRECT_PLU_MAX; i++)
	{
		PluWriteDirectPlu(i, &directPluData);
	}
		
	indirectPluData.unitPrice = 0;

	for (i = DIRECT_PLU_MAX + 1; i <= INDIRECT_PLU_MAX; i++)
	{
		PluWriteIndirectPlu(i, &indirectPluData);
	}
	ReportClearAll();
}

/**
********************************************************************************
* @brief    Read Direct PLU\n
* @param    pluNum : PLU number
*           dst : destination Direct PLU struct pointer
* @return   none
* @remark   
********************************************************************************
*/
void PluReadDirectPlu(INT16U pluNum, DIRECT_PLU_DATA *dst)
{
	INT16U addr;

	if (pluNum == 0)
	{
		return;
	}
	addr = STORAGE_DIRECT_PLU_BASE_ADDR + (sizeof(DIRECT_PLU_DATA) * (pluNum - 1));
	StorageReadData(addr, (INT8U *)dst, sizeof(DIRECT_PLU_DATA));
}

/**
********************************************************************************
* @brief    Write Direct PLU\n
* @param    pluNum : PLU number
*           src : source Direct PLU struct pointer
* @return   none
* @remark   
********************************************************************************
*/
void PluWriteDirectPlu(INT16U pluNum, DIRECT_PLU_DATA *src)
{
	INT16U addr;

	if (pluNum == 0)
	{
		return;
	}
	addr = STORAGE_DIRECT_PLU_BASE_ADDR + (sizeof(DIRECT_PLU_DATA) * (pluNum - 1));
	StorageWriteData(addr, (INT8U *)src, sizeof(DIRECT_PLU_DATA));
}

/**
********************************************************************************
* @brief    Read Indirect PLU\n
* @param    pluNum : PLU number
*           dst : destination Indirect PLU struct pointer
* @return   none
* @remark   
********************************************************************************
*/
void PluReadIndirectPlu(INT16U pluNum, INDIRECT_PLU_DATA *dst)
{
	INT16U addr;

	if (pluNum == 0)
	{
		return;
	}
	addr = STORAGE_INDIRECT_PLU_BASE_ADDR + sizeof(INDIRECT_PLU_DATA) * (pluNum - 29);
	StorageReadData(addr, (INT8U *)dst, sizeof(INDIRECT_PLU_DATA));
}

/**
********************************************************************************
* @brief    Write Indirect PLU\n
* @param    pluNum : PLU number
*           src : source Indirect PLU struct pointer
* @return   none
* @remark   
********************************************************************************
*/
void PluWriteIndirectPlu(INT16U pluNum, INDIRECT_PLU_DATA *src)
{
	INT16U addr;

	if (pluNum == 0)
	{
		return;
	}
	addr = STORAGE_INDIRECT_PLU_BASE_ADDR + sizeof(INDIRECT_PLU_DATA) * (pluNum - 29);
	StorageWriteData(addr, (INT8U *)src, sizeof(INDIRECT_PLU_DATA));
}

/**
********************************************************************************
* @brief    Erase(Clear) All PLU Report
* @param    none
* @return   none
* @remark   PLU 정산 영역 모두 0x00으로 초기화 한다.
********************************************************************************
*/
void ReportClearAll(void)
{
	INT16U i;
	PLU_REPORT_DATA pluReport;

	pluReport.price = 0;
	pluReport.count = 0;

	for (i = 1; i <= PLU_REPORT_MAX_NUM; i++)
	{
		ReportWriteData(i, &pluReport);
	}
}

/**
********************************************************************************
* @brief    Read PLU Report\n
* @param    pluNum : PLU number
*           dst : destination PLU Report struct pointer
* @return   none
* @remark   
********************************************************************************
*/
void ReportReadData(INT16U reportNum, PLU_REPORT_DATA *dst)
{
	INT16U addr;
	
	if (reportNum == 0)
	{
		return;
	}
	addr = STORAGE_REPORT_BASE_ADDR + sizeof(PLU_REPORT_DATA) * (reportNum - 1);
	StorageReadData(addr, (INT8U *)dst, sizeof(PLU_REPORT_DATA));
}

/**
********************************************************************************
* @brief    Write PLU Report\n
* @param    pluNum : PLU number
*           src : source PLU Report struct pointer
* @return   none
* @remark   
********************************************************************************
*/
void ReportWriteData(INT16U reportNum, PLU_REPORT_DATA *src)
{
	INT16U addr;

	if (reportNum == 0)
	{
		return;
	}
	addr = STORAGE_REPORT_BASE_ADDR + sizeof(PLU_REPORT_DATA) * (reportNum - 1);
	StorageWriteData(addr, (INT8U *)src, sizeof(PLU_REPORT_DATA));
}

/*
#include <stdio.h>//for test
void PluTest(void)
{
	char testStr[20];
	DIRECT_PLU_DATA testPlu;

	PluReadDirectPlu(10, &testPlu);
	sprintf(testStr, "name:");
	DebugDataOut(testStr, 5);
	DebugDataOut(testPlu.name, 8);
	sprintf(testStr, "\r\n");
	DebugDataOut(testStr, 2);
	sprintf(testStr, "ta:%3d\r\n", (INT16U)testPlu.tare);
	DebugDataOut(testStr, 8);
	sprintf(testStr, "up:%3d\r\n", (INT16U)testPlu.unitPrice);
	DebugDataOut(testStr, 8);

	sprintf(testPlu.name, "PLU 10  ");
	testPlu.tare = 321;
	testPlu.unitPrice = 654;
	PluWriteDirectPlu(10, &testPlu);
}
*/

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
#endif	//#ifdef USE_SALE_PRICE
