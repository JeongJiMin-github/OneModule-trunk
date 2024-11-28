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
* @brief       KR : Buzzer API Parameter를 default 값으로 초기화\n
*                 US : \n
*                 CN : \n
* @param    KR : type : defualt type\n
*			     \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : Parameter 영역에 초기값을 넣는다.\n
*           			Cal Mode나 Program 모드에서 전체 초기화할 때 실행한다.\n
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
* @brief       KR : Buzzer API Init 함수\n
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
* @remark   KR : 초기에 실행 되는 함수\n
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
* @brief       KR : Buzzer switch 함수\n
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
* @brief       KR : Buzzer 울리는 횟수 셋팅\n
*          			그후 Buzzer Driver Procedure가 호출되면 Buzzer 울림\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*			     \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : Buzzer Driver Procedure는 일정 시간(50 ms) 마다 실행 되므로\n
*          			 셋팅 자체가 Buzzer를 켜는 것으로 보아도 됨\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
/**
********************************************************************************
* @brief    Buzzer 울리는 횟수 셋팅\n
*           그후 Buzzer Driver Procedure가 호출되면 Buzzer 울림\n
* @param    none
* @return   none
* @remark   Buzzer Driver Procedure는 일정 시간(50 ms) 마다 실행 되므로\n
*           셋팅 자체가 Buzzer를 켜는 것으로 보아도 됨
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
