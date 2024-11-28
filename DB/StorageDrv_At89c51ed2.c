/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Storage Drvier Source File\n
           Chip : AT24C16A
* @file    StorageDrv.c
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
#include "StorageDrv.h"

#include "../Hardware/HardwareConfig.h"
#ifdef STORAGE_DRV_AT89C51ED2
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
		eepromWriteByte(dstAddr, src[i]);
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
		dst[i] = eepromReadByte(srcAddr);
		srcAddr++;
	}
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
//extern void TimerDelay10ms(INT16U delay);	//not use

/**
********************************************************************************
* @brief    Read Storage Byte Data\n
* @param    addr : Storage Address
* @return   byte data
* @remark   
********************************************************************************
*/
INT8U eepromReadByte(INT16U addr)
{
  INT8U byte;
  //bit ea_save;
  BOOLEAN eaSave;

  while (EECON & 0x01);	//Eeprom_busy()

  eaSave = EA;
  EA = 0;
  EECON |= 0x02;//Enable eeprom data;
  byte = *(INT8U xdata *)addr;
  EECON &= ~0x02;//Disable eeprom data;
  EA = eaSave;

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
void eepromWriteByte(INT16U addr, INT8U byte)
{
  //bit ea_save;
  BOOLEAN eaSave;

  while (EECON & 0x01);	//wait bit busy

  eaSave = EA;
  EA= 0;
  EECON |= 0x02;//Enable eeprom data
  *(INT8U xdata*)addr = byte;
  EECON &= ~0x02;//Disable eeprom data
  EA = eaSave;
}

#endif//#ifdef STORAGE_DRV_AT89C51ED2