/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Buzzer API Source File\n
* @file    BuzzerApi.c
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
#include "../Db/StorageApi.h"
#include "../Db/ParamApi.h"
#include "BuzzerDrv.h"
#include "BuzzerApi.h"

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
/** @brief KR : Buzzer on flag \n
		  US : Buzzer on flag\n
		  CN :									*/
static BOOLEAN buzzerOnFlag;

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
* @brief       KR : Buzzer API Parameter�� default ������ �ʱ�ȭ\n
*                 US : \n
*                 CN : \n
* @param    KR : type : defualt type\n
*			     \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : Parameter ������ �ʱⰪ�� �ִ´�.\n
*           			Cal Mode�� Program ��忡�� ��ü �ʱ�ȭ�� �� �����Ѵ�.\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BuzzerSetDefaultParam(void)
{
	ParamWriteByte(PARAM_BUZZER_AREA_NUM, BUZZER_PARAM_OFFSET_USE, ON);
}

/**
********************************************************************************
* @brief       KR : Buzzer API Init �Լ�\n
*           			Buzzer Diver Init\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*			     \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : �ʱ⿡ ���� �Ǵ� �Լ�\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BuzzerInit(void)
{
	BuzzerDrvInit();
	BuzzerSetOnTimes(1);//initial buzzer 1 times
	buzzerOnFlag = (BOOLEAN)ParamReadByte(PARAM_BUZZER_AREA_NUM, BUZZER_PARAM_OFFSET_USE);
	
}

/**
********************************************************************************
* @brief       KR : Buzzer switch �Լ�\n
*                 US : \n
*                 CN : \n
* @param    KR : on/off flag\n
*                 US : on/off flag\n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BuzzerSwitch(INT8U switchFlag)
{
	buzzerOnFlag = (BOOLEAN)switchFlag;	
}


/**
********************************************************************************
* @brief       KR : Buzzer �︮�� Ƚ�� ����\n
*          			���� Buzzer Driver Procedure�� ȣ��Ǹ� Buzzer �︲\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*			     \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : Buzzer Driver Procedure�� ���� �ð�(50 ms) ���� ���� �ǹǷ�\n
*          			 ���� ��ü�� Buzzer�� �Ѵ� ������ ���Ƶ� ��\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
/**
********************************************************************************
* @brief    Buzzer �︮�� Ƚ�� ����\n
*           ���� Buzzer Driver Procedure�� ȣ��Ǹ� Buzzer �︲\n
* @param    none
* @return   none
* @remark   Buzzer Driver Procedure�� ���� �ð�(50 ms) ���� ���� �ǹǷ�\n
*           ���� ��ü�� Buzzer�� �Ѵ� ������ ���Ƶ� ��
********************************************************************************
*/
void BuzzerSetOnTimes(INT8U times)
{
	if (buzzerOnFlag) BuzzerDrvSetOnTimes(times);
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
