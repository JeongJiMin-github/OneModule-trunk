/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Hardware Driver Source File\n
           for ER-1 LED
* @file    HwDrv.c
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
#include "../Flash_API_RD2_0_1_0/config.h"

#include "HardwareConfig.h"
#ifdef CPU_AT89C51ED2
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief I2C Port �ߺ� ����� �����ϱ� ���� Flag */
INT8U I2cPortUseFlag;

/** @brief Key output port ����, Display output port �� ���� ���� ��� */
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
* @brief    Hardware Dirver Init �Լ�\n
*           CPU Init\n
*           Hardware variable Init\n
* @param    none
* @return   none
* @remark   �ʱ⿡ �ѹ� ���� �Ǵ� �Լ�\n
********************************************************************************
*/
void HwDrvInit(void)
{
#ifndef X2_MODE
	CKCON0 = 0x00;
#endif

	P0 = 0xff;
	P1 = 0xff;
	P2 = 0xef;		// P2.4  Backlight OFF
	P3 = 0xff;

	//AUXR 
	//7		 6		   5		     4		3	  2		1		0
	//DPU    Reserved  Pulse length  XRS2   XRS1  XRS0  EXTRAM  A0
	AUXR = 0x10;//00010000

	I2cPortUseFlag = OFF;
}

/**
********************************************************************************
* @brief    ������ �� �� Factory Boot�� ������ �����ϵ��� �ϴ� �Լ�\n
* @param    none
* @return   none
* @remark   ENBOOT = 1, BSB = 0xFF, SBV = 0xFC �� �����.\n
********************************************************************************
*/
void HwDrvSetFactoryBoot(void)
{
	EA = 0;
	AUXR1 |= 0x20;
	__api_wr_BSB(0xFF);
	__api_wr_SBV(0xFC);
	EA = 1;
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

#endif//#ifdef CPU_AT89C51ED2
