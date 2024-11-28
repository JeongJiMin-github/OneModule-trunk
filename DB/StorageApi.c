/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Storage API Source File\n
* @file    StorageApi.c
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
#include "StorageDrv.h"
#include "StorageApi.h"

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
* @brief    Read Storage Data\n
* @param    addr : Storage Address
*           dst : destination data pointer
*           size : data size
* @return   none
* @remark   
********************************************************************************
*/
void StorageReadData(INT16U addr, INT8U *dst, INT16U size)
{
	StorageDrvReadData(addr, dst, size);
}

/**
********************************************************************************
* @brief    Write Storage Data\n
* @param    addr : Storage Address
*           src : source data pointer
*           size : data size
* @return   none
* @remark   
********************************************************************************
*/
void StorageWriteData(INT16U addr, INT8U *src, INT16U size)
{	
	StorageDrvWriteData(addr, src, size);
}

/**
********************************************************************************
* @brief    Read Storage Byte Data\n
* @param    addr : Storage Address
* @return   byte data
* @remark   
********************************************************************************
*/
INT8U StorageReadByte(INT16U addr)
{
	INT8U byte;

	StorageDrvReadData(addr, &byte, 1);
	return byte;
}

/**
********************************************************************************
* @brief    Write Storage Byte Data\n
* @param    addr : Storage Address
*           byte : data for writing
* @return   none
* @remark   
********************************************************************************
*/
void StorageWriteByte(INT16U addr, INT8U byte)
{
	StorageDrvWriteData(addr, &byte, 1);
}

/**
********************************************************************************
* @brief    Read Storage word(INT16U) Data \n
* @param    addr : Storage Address
* @return   word data(INT16U)
* @remark   
********************************************************************************
*/
INT16U StorageReadWord(INT16U addr)
{
	INT16U word;

	StorageDrvReadData(addr, (INT8U *)(&word), 2);
	return word;
}

/**
********************************************************************************
* @brief    Write Storage word(INT16U) Data \n
* @param    addr : Storage Address
*           word : word data(INT16U)
* @return   none
* @remark   
********************************************************************************
*/
void StorageWriteWord(INT16U addr, INT16U word)
{
	StorageDrvWriteData(addr, (INT8U *)(&word), 2);
}

/**
********************************************************************************
* @brief    Read Storage double word(INT32U) Data \n
* @param    addr : Storage Address
* @return   double word data(INT32U)
* @remark   
********************************************************************************
*/
INT32U StorageReadDword(INT16U addr)
{
	INT32U dword;

	StorageDrvReadData(addr, (INT8U *)(&dword), 4);
	return dword;
}

/**
********************************************************************************
* @brief    Write Storage double word(INT32U) Data \n
* @param    addr : Storage Address
*           dword : double word data(INT32U)
* @return   none
* @remark   
********************************************************************************
*/
void StorageWriteDword(INT16U addr, INT32U dword)
{
	StorageDrvWriteData(addr, (INT8U *)(&dword), 4);
}

/*
#include <stdio.h>//for test
void StorageTest(void)
{
	char testStr[32];

	//0x2e8 ~ 0x318
	//StorageReadData(0x00, testStr, 20);
	StorageReadData(0x2f0, testStr, 20);
	DebugDataOut(testStr, 20);
	sprintf(testStr, "123456789A123456789B");//20 byte
	//StorageDrvWriteData(0x00, testStr, 20);
	StorageWriteData(0x2f0, testStr, 20);
}
*/
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
