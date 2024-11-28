/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Timer(in CPU) API Source File\n
* @file    TimerApi.c
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
#include "TimerDrv.h"
#include "TimerApi.h"

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
/** @brief 100ms Timer 시작 시간 */
static INT16U timerStart100ms[TIMER_DELAY_100MS_MAX_NUM];
/** @brief 100ms Timer Delay 시간 */
static INT16U timerDelay100ms[TIMER_DELAY_100MS_MAX_NUM];

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
* @brief    Timer API Init 함수\n
*           Timer Driver 초기화\n
*           각종 Timer 초기화\n
* @param    smode : 다음에 실행할 모드
* @return   none
* @remark   초기에 한번 실행 되는 함수\n
********************************************************************************
*/
void TimerInit(void)
{
	INT8U i;

	TimerDrvInit();

	for (i = 0; i < TIMER_DELAY_100MS_MAX_NUM; i++)
	{
		timerDelay100ms[i] = 0;
	}
}

/**
********************************************************************************
* @brief    10ms Delay 함수\n
* @param    delay : delay time (단위 : 10ms)
* @return   none
* @remark   100ms 이상 delay 하지 말 것\n
********************************************************************************
*/
/*
void TimerDelay10ms(INT16U delay)
{
	INT16U	start;

	start = TimerDrvGetSysTimer10ms();
	while (delay > TimerDrvGetSysTimer10ms() - start);
}
*/

/**
********************************************************************************
* @brief    100ms 단위로 돌아가는 System Timer 값을 읽는 함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
INT16U TimerGetSysTimer100ms(void)
{
	return TimerDrvGetSysTimer100ms();
}

/**
********************************************************************************
* @brief    100ms 단위의 delay Time을 Setting하는 함수\n
* @param    num : 100ms timer 번호(TIMER_DELAY_100MS_DISP_MSG ~ )
*           delay : delay 시간 (단위 : 100ms)
* @return   none
* @remark   TimerCheckDelay100ms()를 이용하여 Timer가 끝났는지 Check할 수 있음
********************************************************************************
*/
void TimerSetDelay100ms(INT8U num, INT16U delay)
{
	timerStart100ms[num] = TimerDrvGetSysTimer100ms();
	timerDelay100ms[num] = delay;
}

/**
********************************************************************************
* @brief    TimerSetDelay100ms()에서 Setting Timer를 Check하는 함수\n
* @param    num : 100ms timer 번호(TIMER_DELAY_100MS_DISP_MSG ~ )
* @return   0 -> not complete, 1 -> complete (timer is over)
* @remark   see TimerSetDelay100ms()
********************************************************************************
*/
INT8U TimerCheckDelay100ms(INT8U num)
{
	
	if (timerDelay100ms[num] == 0)
	{
		return 0;
	}

	if (timerDelay100ms[num] > (INT16U)(TimerDrvGetSysTimer100ms() - timerStart100ms[num])) 
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
