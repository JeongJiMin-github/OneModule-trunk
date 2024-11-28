/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Program Mode Applicaion Source File\n
* @file    PgmModeApp.c
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
#include "../Common/PluData.h"
#include "../Common/CommonLib.h"
#include "../Common/UiDefine.h"
#include "../DB/PluApi.h"
#include "../DB/StorageApi.h"
#include "../DB/ParamApi.h"
#include "../UI/DispApi.h"
#include "../UI/KeyApi.h"
#include "../UI/EditorApi.h"
#include "../UI/MenuApi.h"
#include "../Power/PowerApi.h"
#include "ScaleModeApp.h"
#include "PgmModeApp.h"

#include "../Common/PluData.h"
#include "../Communication/DebugApi.h"
#include "../Application/SaleModeApp.h"
#include "../Communication/PrtApi.h"
#include "../AD/AdApi.h"
#include "../SystemTimer/TimerApi.h"

#ifdef USE_PGM_AD
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
static INT8U pgmExitFunc(KEY_TYPE key);
static INT8U pgmUserSetup(KEY_TYPE key);

static INT8U pgmFilterSetup(KEY_TYPE key);

/** @brief Program Menu Table 개수*/
#define MENU_PGM_MAX_POS 3
/** @brief Program Menu Table*/
const MENU code pgmMenuTable[MENU_PGM_MAX_POS] = 
{
	{0x1000, "PGM",	MENU_MODE_RUN, 		pgmExitFunc},
	{0x1F10, "AP", 	MENU_MODE_NORMAL, 		pgmUserSetup},
	{0x1F20, "FILT", 	MENU_MODE_NORMAL, 		pgmFilterSetup}
};

/** @brief Program Direct Menu Table 개수*/
#define MENU_DIRECT_PGM_MAX_POS 2
/** @brief Program Direct Menu Table */
const DIRECT_MENU code pgmDirectMenuTable[MENU_DIRECT_PGM_MAX_POS] = 
{
	{KEY_ZERO, 0x1F10 },
	{KEY_TARE, 0x1F20 }
};

/** 
* @brief Suspend Mode 시간 Setting Table (단위 : sec) 
* @remark 0-off, 20,40,60 - delay sec.
*/
#define SUSPEND_TIME_TABLE_SIZE	4
const INT8U code SuspendTimeTable[SUSPEND_TIME_TABLE_SIZE] = 
{
	0, 20, 40, 60
};

/** 
* @brief Auto Power Off Mode 시간 Setting Table (단위 : min) 
* @remark 0-always on, 10,30,60 - delay min.
*/
#define POWER_AUTO_OFF_TIME_TABLE_SIZE	4
const INT8U code PowerAutoOffTimeTable[POWER_AUTO_OFF_TIME_TABLE_SIZE] = 
{
	0, 10, 30, 60
};

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
* @brief    Program Mode Application Init 함수\n
* @param    none
* @return   none
* @remark   초기에 한번 실행 되는 함수\n
********************************************************************************
*/
void PgmModeInit(void)
{
	DispSetDecPoint(WIN_WEIGHT, 0, 0, 0);
	DispSetDecPoint(WIN_UNIT_PRICE, 0, 0, 0);
	DispSetDecPoint(WIN_TOTAL_PRICE, 0, 0, 0);
	DispSetDecPoint(WIN_TARE, 0, 0, 0);
	DispClearAllDigits();
}

/**
********************************************************************************
* @brief    Program Menu Table 반환함\n
*           Menu Mode Application에서 사용
* @param    
* @return   Program Menu Table Pointer
* @remark   
********************************************************************************
*/
MENU code *PgmModeGetMenuTable(void)
{
	return pgmMenuTable;
}

/**
********************************************************************************
* @brief    Program Direct Menu Table 반환함\n
*           Menu Mode Application에서 사용
* @param    
* @return   Program Menu Table Pointer
* @remark   
********************************************************************************
*/
DIRECT_MENU code *PgmModeGetDirectMenuTable(void)
{
	return pgmDirectMenuTable;
}

/**
********************************************************************************
* @brief    Program Menu Table 개수 반환함\n
*           Menu Mode Application에서 사용
* @param    none
* @return   Program Menu Table 개수
* @remark   
********************************************************************************
*/
INT8U PgmModeGetMenuTableMaxPos(void)
{
	return MENU_PGM_MAX_POS;
}

/**
********************************************************************************
* @brief    Program Direct Menu Table 개수 반환함\n
*           Menu Mode Application에서 사용
* @param    none
* @return   Program Direct Menu Table 개수
* @remark   
********************************************************************************
*/
INT8U PgmModeGetDirectMenuTableMaxPos(void)
{
	return MENU_DIRECT_PGM_MAX_POS;
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
/**
********************************************************************************
* @brief    Program Menu Exit 함수\n
* @param    key : not use
* @return   Menu 함수 반환값
* @remark   MENU_RET_EXIT를 반환하여 Sale Mode로 전환하게 함
********************************************************************************
*/
INT8U pgmExitFunc(KEY_TYPE key)
{
	key = 1;
	return MENU_RET_EXIT;
}

/**
********************************************************************************
* @brief    Program User Setup \n
* @param    key : TARE KEY(시간 설정, 0분, 10분, 20분, 30분) 
* @return   Auto Power off 시간 설정
* @remark   NONE           
********************************************************************************
*/
INT8U pgmUserSetup(KEY_TYPE key)
{
	static INT8U 	tempInt8u;

	switch (MenuFuncState)
	{
		case 0x00:
#ifdef REVISION_NUM
			if(!TimerCheckDelay100ms(TIMER_DELAY_100MS_INITIAL))
			{
				DispSetMsgWithNum("r", REVISION_NUM);
				TimerSetDelay100ms(TIMER_DELAY_100MS_INITIAL, 10);//include disp. revision 10
				while (!TimerCheckDelay100ms(TIMER_DELAY_100MS_INITIAL));	//wait key
			}
#endif
			tempInt8u = ParamReadByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_AUTO_OFF_TIME);		
			switch (tempInt8u)
			{
				case 0:
				default:
					DispSetStr(MenuDataWinNum, "AP-00");
					break;
				case 10:
					DispSetStr(MenuDataWinNum, "AP-10");
					break;
				case 20:
					DispSetStr(MenuDataWinNum, "AP-20");
					break;
				case 30:
					DispSetStr(MenuDataWinNum, "AP-30");
					break;
			}
			if (key == KEY_TARE)
			{
				if (tempInt8u != 30)
				{
					tempInt8u += 10;
				}
				else 
				{
					tempInt8u = 0;
				}
				ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_AUTO_OFF_TIME,tempInt8u);		
			}			
			if (key == KEY_ZERO)
			{
				MenuFuncState++;
			}
			break;
			
		case 0x01:						
			tempInt8u = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST);
			tempInt8u = tempInt8u >> 6;

			switch (tempInt8u)
			{
				case PRT_NOT_USE:
				default:
					DispSetStr(MenuDataWinNum, "NO PRT");
					break;
				case PRT_USE_BY_KEY:
					DispSetStr(MenuDataWinNum, "KEY");
					break;
				case PRT_USE_BY_AUTO:
					DispSetStr(MenuDataWinNum, "AUTO");
					break;
				case PRT_USE_BY_STREAM:
					DispSetStr(MenuDataWinNum, "STRM");
					break;
			}
			if (key == KEY_TARE)
			{
				if (tempInt8u != 4)
				{
					tempInt8u += 1;
				}
				else 
				{
					tempInt8u = 0;
				}
				tempInt8u = tempInt8u << 6;
				ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST, tempInt8u);		
			}
		
#ifdef USE_SUSPEND_MODE
			if (key == KEY_ZERO)
			{
				MenuFuncState++;
			}
			break;
			
		case 0x01:			
			tempInt8u = ParamReadByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_STANDBY_TIME);		
			switch (tempInt8u)
			{
				case 0:
				default:
					DispSetStr(MenuDataWinNum, "SB-00");
					break;
				case 20:
					DispSetStr(MenuDataWinNum, "SB-20");
					break;
				case 40:
					DispSetStr(MenuDataWinNum, "SB-40");
					break;
				case 60:
					DispSetStr(MenuDataWinNum, "SB-60");
					break;
			}
			if (key == KEY_TARE)
			{
				if (tempInt8u != 60)
				{
					tempInt8u += 20;
				}
				else 
				{
					tempInt8u = 0;
				}
				ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_STANDBY_TIME,tempInt8u);		
			}
			if (key == KEY_ZERO)
			{
				PowerSaverInit(); 		
				return MENU_RET_EXIT;
			}
			break;
#else		
			if (key == KEY_ZERO)
			{
				PowerSaverInit(); 
				PrtInit();
				return MENU_RET_EXIT;
			}
			break;
#endif
	}
	return MENU_RET_INCOMPLETE;
}

/**
********************************************************************************
* @brief    Program Filter Setup \n
* @param  
* @return   
* @remark   두자리로 세팅 (앞자리 : Shock(0~7), 뒷자리 : 흔들림(0~9))
********************************************************************************
*/
INT8U pgmFilterSetup(KEY_TYPE key)
{
	static INT32U tempInt32u;

	switch (MenuFuncState)
	{
		case 0x00:
			DispFiltReadParam();
			EditorSetDataAndRun(MenuDataWinNum, DEC, (INT32U)DispFiltParam.filtIndex, 0, 99, 0, 0, 0);
			if (MenuWinIsOne) EditorSetConstStr("FLT");
			else DispSetStr(WIN_WEIGHT, " FLT");
			MenuFuncState++;
			break;
				
		case 0x01:
			EditorGetData(&tempInt32u);
			if(tempInt32u == 99)
			{
				MenuFuncState = 0x02;
				tempInt32u = 0;
				TimerSetDelay100ms(TIMER_DELAY_100MS_INITIAL, 5);
				return MENU_RET_INCOMPLETE;
			}
			else if((tempInt32u / 10) > 5 || (tempInt32u % 10) > 5)
			{				
				MenuFuncState = 0x00;
				//DispFiltParam.filtIndex = 11;
				return MENU_RET_INCOMPLETE;
			}
			else DispFiltParam.filtIndex = (INT8U)tempInt32u;
			DispFiltWriteParam();
			AdInit();
			return MENU_RET_EXIT;

		case 0x02:			
			if (key != KEY_TARE)
			{
				if(TimerCheckDelay100ms(TIMER_DELAY_100MS_INITIAL) == 1) 
				{
					DispTest(tempInt32u);
					tempInt32u++;
					tempInt32u %= 10;
					TimerSetDelay100ms(TIMER_DELAY_100MS_INITIAL, 5);
				}				
			}
			else
			{
				DispClearAll();
				MenuFuncState = 0x00;
				return MENU_RET_INCOMPLETE;
			}
			break;
	}
	return MENU_RET_INCOMPLETE;
}
#endif




