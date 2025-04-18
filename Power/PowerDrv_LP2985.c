/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Driver Source File\n
			chip : LP2985
* @file    PowerDrv_LP2985.c
* @version 1.0
* @date    2012/09/19
* @author  Deok Hyun Kim (KDH)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2012/09/19 created by KDH \n
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

#ifdef POWER_DRV_LP2985
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
    POWER_CON_ON;
	#ifdef CASTON_POWEROFF
	POWER_MODULE_ON;
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

    POWER_CON_OFF;
	#ifdef CASTON_POWEROFF
	POWER_MODULE_OFF;
	#endif

}

/**
********************************************************************************
* @brief    PowerSavingMode 함수\n
* @param    INT8U modeLevel
* @return   none
* @remark   각 power saveing mode level에 맞춰서 실행하는 함수 
********************************************************************************
*/
void PowerDrvSavingMode(INT8U modeLevel)
{
    EMU_EnterEM1();
}


/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/

#endif//#ifdef POWER_DRV_STD

		
