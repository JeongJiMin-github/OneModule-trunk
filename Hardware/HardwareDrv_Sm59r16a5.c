/**
********************************************************************************
* Copyright (c) 2014 CAS
* @brief   Hardware Driver Source File\n
           for ER-1 LED
* @file    HardwareDrv_Sm59r16a5.c
* @version 1.0
* @date    2014/02/24
* @author  Keum Young-Kwang (KYK)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2014/02/24 created by KYK \n
********************************************************************************
*/


/*
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
/* Insert include files here */
#include "../Common/Globals.h"

#include "HardwareConfig.h"
#ifdef CPU_SM59R16A5
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief I2C Port 중복 사용을 제한하기 위한 Flag */
INT8U I2cPortUseFlag;

/** @brief Key output port 상태, Display output port 와 공용 사용시 사용 */
INT8U  KeyOutputPortStatus;
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
* @brief    Hardware Dirver Init 함수\n
*           CPU Init\n
*           Hardware variable Init\n
* @param    none
* @return   none
* @remark   초기에 한번 실행 되는 함수\n
********************************************************************************
*/
void HwDrvInit(void)
{
#ifndef X2_MODE
	IFCON = 0x00; //[ITS|CDPR|-|-|ALEC1|ALEC0|EMEN|ISPE]: 2T Set 
	//ITS: 0=2T, 1=1T
	//EMEN: internal expanded memory, 0=enable, 1=disable 
#else
	IFCON = 0x80;
#endif

	P0 = 0xff;
	P1 = 0xff;
	P2 = 0xef;		// P2.4  Backlight OFF
	P3 = 0xff;

	//AUXR 
	//7		 6		   5		     4		3	  2		1		0
	//DPU    Reserved  Pulse length  XRS2   XRS1  XRS0  EXTRAM  A0
//	AUXR = 0x10;//00010000 //Interal XRAM Size XRS2 | XRS1 | XRS0 = 1 | 0 | 0 => 1792 bytes
//	RCON: syncmos, internal RAM control register (addr=0x86)
	RCON = 3; //2048 bytes

	I2cPortUseFlag = OFF;
}

/**
********************************************************************************
* @brief    다음에 켤 때 Factory Boot가 무조건 실행하도록 하는 함수\n
* @param    none
* @return   none
* @remark   ENBOOT = 1, BSB = 0xFF, SBV = 0xFC 로 만든다.\n
********************************************************************************
*/
void HwDrvSetFactoryBoot(void)
{
#if 0
	EA = 0;
	AUXR1 |= 0x20;
	__api_wr_BSB(0xFF);
	__api_wr_SBV(0xFC);
	EA = 1;
#endif
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

#endif//#ifdef CPU_AT89C51ED2
