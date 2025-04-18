/**
********************************************************************************
* Copyright (c) 2016 CAS
* @brief   Power Control Driver Source File\n
           Chip : M0516LDN
* @file    PowerDrv_M0516ldn.c
* @version 1.0
* @date    2016/12/06
* @author  Keum Young Kwang (KYK)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2016/12/06 created by KYK \n
********************************************************************************
*/

/*
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
/* Insert include files here */
#include "../Common/Globals.h"
#include "../Hardware/HardwareConfig.h"
#include "PowerDrv.h"

#ifdef POWER_DRV_M0516LDN
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
/* Insert local functions here */
/**
********************************************************************************
* @brief    Power Driver Init 함수\n
* @param    none
* @return   none
* @remark   Power Key Port를 입력으로 설정
********************************************************************************
*/
void PowerDrvInit(void)
{
	//POWER_CHK = 1;
}

/**
********************************************************************************
* @brief    Power On 함수\n
* @param    none
* @return   none
* @remark   전원을 켠다.
********************************************************************************
*/
void PowerDrvPowerOn(void)
{
#ifdef POWER_ON_ACTIVE_LOW
	POWER_CON_OFF;
#else
	POWER_CON_ON;
#endif
}

/**
********************************************************************************
* @brief    Power Off 함수\n
* @param    none
* @return   none
* @remark   전원을 끈다.
********************************************************************************
*/
void PowerDrvPowerOff(void)
{
#ifdef POWER_ON_ACTIVE_LOW
	POWER_CON_ON;
#else
	
	while(1)
	{
		POWER_CON_OFF;
		//PCON |= 0x02;
	}
#endif
}
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/

#endif//#ifdef POWER_DRV_M0516LDN	
