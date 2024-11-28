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

//#ifdef USE_SALE_PRICE_EXTERNAL_PLU
#if defined (USE_SALE_PRICE_EXTERNAL_PLU) || defined (USE_SALE_PRICE_EXTERNAL_PLU_AP)
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

	directPluData.unitPrice = 0;
	directPluData.tare = 0;
	directPluData.validDate = 0;
	directPluData.itemCode = 0;
	
	for (j = 0; j < PLU_NAME_SIZE; j++)
	{
		directPluData.name[j] = 0x20;
	}

	for (i = 1; i <= DIRECT_PLU_MAX; i++)
	{
		PluWriteDirectPlu(i, &directPluData);
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
	INT8U i;

	if (pluNum == 0)
	{
		return;
	}
	addr = EXT_STORAGE_PLU_BASE_ADDR + (PLU_SIZE * (pluNum - 1));
	for (i = 0; i < PLU_SIZE; i++) 
	{
		((INT8U *)dst)[i] = StorageReadByte(addr + i);
	}	
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
	INT8U i;
	
	if (pluNum == 0)
	{
		return;
	}
	addr = EXT_STORAGE_PLU_BASE_ADDR + (PLU_SIZE * (pluNum - 1));
	for (i = 0; i < PLU_SIZE; i++) StorageWriteByte(addr + i, ((INT8U *)src)[i]);
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
#if defined(USE_SALE_PRICE) || defined(USE_SALE_PRICE_COMPARE) || defined(USE_SALE_PR) || defined(USE_SALE_ER_MLED)
void PluReadIndirectPlu(INT16U pluNum, INDIRECT_PLU_DATA *dst)
{

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

}
#endif
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
	INT8U i;
	
	if (reportNum == 0)
	{
		return;
	}
	addr = EXT_STORAGE_PLU_AREA_SIZE + (REPORT_SIZE * (reportNum - 1));
	for (i = 0; i < REPORT_SIZE; i++)
	{
		((INT8U *)dst)[i] = StorageReadByte(addr + i);
	}
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
	INT8U i;

	if (reportNum == 0)
	{
		return;
	}
	addr = EXT_STORAGE_PLU_AREA_SIZE + (REPORT_SIZE * (reportNum - 1));
	for (i = 0; i < REPORT_SIZE; i++) StorageWriteByte(addr + i, ((INT8U *)src)[i]);
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
#endif //#ifdef USE_SALE_PRICE_EXTERNAL_PLU
