/**
********************************************************************************
* Copyright (c) 2012 CAS
* @brief   Storage Drvier Source File\n
           Chip : STM8L151C8
* @file    StorageDrv_Stm8l151c8.c
* @version 1.0
* @date    2012/09/24
* @author  Young Hoon Lee (LYH)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2012/09/24 created by LYH \n
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
#include "../Hardware/HardwareConfig.h"
#include "../STM8L151C8/stm8l15x_conf.h"
#ifdef STORAGE_DRV_STM8L151C8
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
/** @brief eeprom ¿ë·® */
#define INT_EEPROM_CAPACITY	0x0800

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
static INT8U eepromReadByte(INT16U addr);
static void eepromWriteByte(INT16U addr, INT8U byte);

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */

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
void StorageDrvWriteData(INT16U addr, INT8U *src, INT16U size)
{
	INT16U dstAddr;
	INT16U i;

	dstAddr = addr;
	for (i = 0; i < size; i++)
	{
		FLASH_ProgramByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+dstAddr, src[i]);
		dstAddr++;
	}
}

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
void StorageDrvReadData(INT16U addr, INT8U *dst, INT16U size)
{
	INT16U srcAddr;
	INT16U i;

	srcAddr = addr;
	for (i = 0; i < size; i++)
	{
		dst[i] = FLASH_ReadByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+srcAddr);
		srcAddr++;
	}
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

#endif//#ifdef STORAGE_DRV_STM8L151C8