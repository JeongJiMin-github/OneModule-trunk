/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Temperature Sensor Driver Source File\n
           Chip : TCN75A
* @file    TempDrv.c
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
#include "TempDrv.h"

#include "../Hardware/HardwareConfig.h"
#ifdef TEMP_DRV_NONE
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
#ifdef USE_TEMP_COMP
/*
********************************************************************************
* @brief    Temperature Sensor Driver Procedure\n
* @param    none
* @return   none
* @remark   ���� �ð��� �ѹ��� ��� ���� �Ǵ� �Լ�
*           tempValue�� ������Ʈ��
********************************************************************************
*/
void TempDrvProc(INT16U currTimer100ms)
{

}

/**
********************************************************************************
* @brief    Initialize Temperature sensor.\n
*           1. ONE shot mode : disable
*			2. ADC resolution : 12 bit
*			3. Fault queue : 1 bit
*			4. Alert polarity : Active low
*			5. mode : comparator
*			6. shut down : disable
* @param    none
* @return   Temperature Sensor Initial (1 : Succeed, 0 : Fail)
* @remark   
********************************************************************************
*/
BOOLEAN TempDrvInit(void)
{
	return 0;
}

/**
********************************************************************************
* @brief    Filtering �� �µ������� ���� �д� �Լ�\n
* @param    none
* @return   �µ��� (���� : 1 degree C)
* @remark   
********************************************************************************
*/
INT8S TempDrvReadData(void)
{
	return 20;
}
#endif
#ifdef DEBUG_TEMP
/**
********************************************************************************
* @brief    �µ������� �� �����ϴ� �Լ� (for Debug)\n
* @param    temp : �µ��� (���� : 1 degree C)
* @return   none
* @remark   
********************************************************************************
*/
void TempDrvWriteDataForDebug(INT8S temp)
{

}
#endif
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

#endif//#ifdef TEMP_DRV_NONE
