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
#include "../Common/Timedata.h"
#include "../DB/PluApi.h"
#include "../DB/StorageApi.h"
#include "../DB/ParamApi.h"
#include "../UI/DispApi.h"
#include "../UI/KeyApi.h"
#include "../UI/EditorApi.h"
#include "../UI/MenuApi.h"
#include "../Communication/DebugApi.h"
#include "../Communication/PrtApi.h"
#include "../AD/AdApi.h"
#include "../Time/TimeApi.h"
#include "../Power/PowerApi.h"
#include "ScaleModeApp.h"
#include "PgmModeApp.h"
#include "SaleModeApp.h"
#include "../SystemTimer/TimerApi.h"


#ifdef USE_PGM_ER_MLED
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
static INT8U pgmPrinterSetup(KEY_TYPE key);
static INT8U pgmFilterSetup(KEY_TYPE key);

/** @brief Program Menu Table 개수*/
#define MENU_PGM_MAX_POS 3
/** 
* @brief Program Menu Table
* @remark
* Menu 추가시 MENU_PGM_MAX_POS를 증가시키고 pgmMenuTable에 추가하시오
* menuNum은 같은 level에서는 연속으로 증가 시키시오.
* 예)   0x1110, 0x1120, 0x1130 다음 메뉴 이면 0x8140에 추가
* 예)   0x1100, 0x1200, 0x1300 다음 메뉴 이면 0x8400에 추가
* 예)   0x1500 하위 메뉴이면 0x1510에 추가(단, 0x1500의 Menu Mode는 MENU_MODE_MENU이여야함)
*/
const MENU code pgmMenuTable[MENU_PGM_MAX_POS] = 
{
	{0x1000, "PGM",	MENU_MODE_RUN, 		pgmExitFunc},
	{0x1F10, "AP", 	MENU_MODE_NORMAL, 		pgmUserSetup},
	{0x1F20, "FILT", 	MENU_MODE_NORMAL, 		pgmFilterSetup}
};

/** @brief Program Direct Menu Table 개수*/
#define MENU_DIRECT_PGM_MAX_POS 3
/** @brief Program Direct Menu Table */
const DIRECT_MENU code pgmDirectMenuTable[MENU_DIRECT_PGM_MAX_POS] = 
{
	{KEY_ZERO, 0x1F10},
	{KEY_C, 0x1F20},
	{KEY_TARE, 0x1F30}		
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
/** 
* @brief LCD BL Off로 가는 시간 Setting Table (단위 : sec) 
* @remark 0-initial off, 1-always on, 3,5,10 - delay sec.
*/
#define POWER_BL_OFF_TIME_TABLE_SIZE	5
const INT8U code PowerBLOffTimeTable[POWER_BL_OFF_TIME_TABLE_SIZE] = 
{
	0, 1, 3, 5, 10
};
/** 
* @brief LED Suspend Mode로 가는 시간 Setting Table (단위 : sec) 
* @remark 0-always on, 20, 40, 60 - delay sec
*/
#define POWER_STAND_BY_TIME_TABLE_SIZE	7
const INT8U code PowerStandbyTimeTable[POWER_STAND_BY_TIME_TABLE_SIZE] = 										
{
	0, 5, 10, 15, 20, 30, 60
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

/**
********************************************************************************
* @brief    Program User Setup \n
* @param    key : not use
* @return   Auto Power off 시간 설정, Backlight Time 설정, standby time 설정, 프린트 사용여부 설
* @remark   ER 이 본 Program 모드에서  Zero키를 Enter 키로 사용하고 있어서 \n
*           Zero키와 설정키가 바뀌어 있음 
********************************************************************************
*/
INT8U pgmUserSetup(KEY_TYPE key)
{
	INT8U 	retValue;
	INT8U	i;
	
	static INT8U tempByte;

	retValue = MENU_RET_INCOMPLETE;

	switch (MenuFuncState) {
		case 0x00:
#ifdef REVISION_NUM
			if(!TimerCheckDelay100ms(TIMER_DELAY_100MS_INITIAL))
			{
				DispSetMsgWithNum("r", REVISION_NUM);
				TimerSetDelay100ms(TIMER_DELAY_100MS_INITIAL, 10);//include disp. revision 10
				while (!TimerCheckDelay100ms(TIMER_DELAY_100MS_INITIAL));	//wait key
			}
#endif
			DispSetStr(WIN_WEIGHT, "SLTIME");
			DispSetStr(WIN_TOTAL_PRICE, " U-SET");
			tempByte = ParamReadByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_AUTO_OFF_TIME);

			for (i = 0; i < POWER_AUTO_OFF_TIME_TABLE_SIZE; i++)
			{
				if (tempByte == PowerAutoOffTimeTable[i])
				{
					tempByte = i;
					break;
				}
			}
			if (i == POWER_AUTO_OFF_TIME_TABLE_SIZE)
			{
				tempByte = 0;
			}			
			MenuFuncState = 0x01;
		case 0x01:
			DispSetStr(WIN_WEIGHT, " A-OFF");				
			DispSetStr(WIN_TOTAL_PRICE, " U-SET");

			if (tempByte == 0) {
				DispSetStr(WIN_UNIT_PRICE, "  NO  ");
			} else {
				DispSetNum(WIN_UNIT_PRICE, PowerAutoOffTimeTable[tempByte], 0);
			}

			if (key == MenuKeyMenuUp) {
				ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_AUTO_OFF_TIME, PowerAutoOffTimeTable[tempByte]);
				PowerSaverInit();//적용
				MenuFuncState = 0x10;
			} else if (key == MenuKeyEnter) {
				tempByte++;
				tempByte %= POWER_AUTO_OFF_TIME_TABLE_SIZE;
			} else if (key==MenuKeyC) {
				MenuFuncState = 0xF0;
			}
			break;
		case 0x10:		// initial
			tempByte = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH); 	
			tempByte = tempByte & 0x80;
			tempByte = ParamReadByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_STANDBY_TIME);

			for (i = 0; i < POWER_STAND_BY_TIME_TABLE_SIZE; i++)
			{
				if (tempByte == PowerStandbyTimeTable[i])
				{
					tempByte = i;
					break;
				}
			}
			if (i == POWER_STAND_BY_TIME_TABLE_SIZE)
			{
				tempByte = 0;
			}
			MenuFuncState = 0x11;
		case 0x11:
			DispSetStr(WIN_WEIGHT, "STAND");
			DispSetStr(WIN_TOTAL_PRICE, " U-SET");
			if (tempByte == 0)
			{
				DispSetStr(WIN_UNIT_PRICE, "  OFF ");
			}
			else
			{
				DispSetNum(WIN_UNIT_PRICE, PowerStandbyTimeTable[tempByte], 0);
			}
			if (key == MenuKeyMenuUp)		
			{
				ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_STANDBY_TIME, PowerStandbyTimeTable[tempByte]);
				MenuFuncState = 0x40;
				PowerInit();
			} else if (key == MenuKeyEnter) {
				tempByte++;
				tempByte %= POWER_STAND_BY_TIME_TABLE_SIZE;
			} else if (key==MenuKeyC) {
				MenuFuncState = 0xF0;
			}
			break;
		case 0x40:
			TimeReadData();
			tempByte = TimeData.year;
			MenuFuncState = 0x41;
		case 0x41:
			DispSetStr(WIN_TOTAL_PRICE, " DATE ");
			DispSetStr(WIN_UNIT_PRICE, " YEAR");
			DispSetNum(WIN_WEIGHT, tempByte, 0);
			if (key >= KEY_NUM_0 && key <= KEY_NUM_9) {
				tempByte = EditorInsDecDigit(tempByte, key-KEY_NUM_0, 2, 99, FALSE);
			} else  if (key==MenuKeyMenuDown) {
				if (tempByte==2) tempByte=100;
				tempByte--;
			} else if (key==MenuKeyEnter) {
				tempByte++;
				if (tempByte==100) tempByte=2;
			} else 	if (key==MenuKeyMenuUp) {
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_YEAR, tempByte);
				TimeData.year = tempByte;
				TimeWriteData();
				MenuFuncState = 0x42;
			} else if (key==MenuKeyC) {
				MenuFuncState = 0xF0;
			}
			break;
		case 0x42:
			TimeReadData();
			tempByte = TimeData.month;
			MenuFuncState = 0x43;
		case 0x43:
			DispSetStr(WIN_TOTAL_PRICE, " DATE ");
			DispSetStr(WIN_UNIT_PRICE, "  MON");
			DispSetNum(WIN_WEIGHT, tempByte, 0);
			if (key >= KEY_NUM_0 && key <= KEY_NUM_9) {
				tempByte = EditorInsDecDigit(tempByte, key - KEY_NUM_0, 2, 12, FALSE);
			} else  if (key==MenuKeyMenuDown) {
				if (tempByte==1) tempByte=13;
				tempByte--;
			} else if (key==MenuKeyEnter) {
				tempByte++;
				if (tempByte==13) tempByte=1;
			} else 	if (key==MenuKeyMenuUp) {
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_MONTH, tempByte);
				TimeData.month = tempByte;
				TimeWriteData();
				MenuFuncState = 0x44;
			} else if (key==MenuKeyC) {
				MenuFuncState = 0xF0;
			}
			break;
		case 0x44:
			TimeReadData();
			tempByte = TimeData.date;
			MenuFuncState = 0x45;
		case 0x45:
			DispSetStr(WIN_TOTAL_PRICE, " DATE ");
			DispSetStr(WIN_UNIT_PRICE, " DATE");
			DispSetNum(WIN_WEIGHT, tempByte, 0);
			if (key >= KEY_NUM_0 && key <= KEY_NUM_9) {
				tempByte = EditorInsDecDigit(tempByte, key - KEY_NUM_0, 2, 31, FALSE);
			} else  if (key==MenuKeyMenuDown) {
				if (tempByte==1) tempByte=32;
				tempByte--;
			} else if (key==MenuKeyEnter) {
				tempByte++;
				if (tempByte==32) tempByte=0;
			} else if (key == MenuKeyMenuUp) {
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_DAY, tempByte);
				TimeData.date = tempByte;
				TimeWriteData();
				MenuFuncState = 0x50;
			} else if (key==MenuKeyC) {
				MenuFuncState = 0xF0;
			}
			break;
		case 0x50:
			TimeReadData();
			tempByte = TimeData.hour;
			MenuFuncState = 0x51;
		case 0x51:
			DispSetStr(WIN_TOTAL_PRICE, " TIME ");
			DispSetStr(WIN_UNIT_PRICE, " HOUR");
			DispSetNum(WIN_WEIGHT, tempByte, 0);
			if (key >= KEY_NUM_0 && key <= KEY_NUM_9) {
				tempByte = EditorInsDecDigit(tempByte, key - KEY_NUM_0, 2, 23, FALSE);
			} else  if (key==MenuKeyMenuDown) {
				if (tempByte==0) tempByte=24;
				tempByte--;
			} else if (key==MenuKeyEnter) {
				tempByte++;
				if (tempByte==24) tempByte=0;
			} else if (key == MenuKeyMenuUp) {
				TimeData.hour = tempByte;
				TimeWriteData();
				MenuFuncState = 0x52;
			} 
			break;
		case 0x52:
			TimeReadData();
			tempByte = TimeData.min;
			MenuFuncState = 0x53;
		case 0x53:
			DispSetStr(WIN_TOTAL_PRICE, " TIME ");
			DispSetStr(WIN_UNIT_PRICE, "  MIN");
			DispSetNum(WIN_WEIGHT, tempByte, 0);
			if (key >= KEY_NUM_0 && key <= KEY_NUM_9) {
				tempByte = EditorInsDecDigit(tempByte, key - KEY_NUM_0, 2, 59, FALSE);
			} else  if (key==MenuKeyMenuDown) {
				if (tempByte==0) tempByte=60;
				tempByte--;
			} else if (key==MenuKeyEnter) {
				tempByte++;
				if (tempByte==60) tempByte=0;
			} else if (key == MenuKeyMenuUp) {
				TimeData.min = tempByte;
				TimeWriteData();
				MenuFuncState = 0x54;
			} 
			break;
		case 0x54:
			TimeReadData();
			tempByte = TimeData.sec;
			MenuFuncState = 0x55;
		case 0x55:
			DispSetStr(WIN_TOTAL_PRICE, " TIME ");
			DispSetStr(WIN_UNIT_PRICE, "  SEC");
			DispSetNum(WIN_WEIGHT, tempByte, 0);
			if (key >= KEY_NUM_0 && key <= KEY_NUM_9)
			{
				tempByte = EditorInsDecDigit(tempByte, key - KEY_NUM_0, 2, 60, FALSE);
			} else  if (key==MenuKeyMenuDown) {
				if (tempByte==0) tempByte=60;
				tempByte--;
			} else if (key==MenuKeyEnter) {
				tempByte++;
				if (tempByte==60) tempByte=0;
			} else if (key == MenuKeyMenuUp) {
				TimeData.sec = tempByte;
				TimeWriteData();
				MenuFuncState = 0xF0;
			} 
			break;
		case 0xF0:
			MenuSetKeyMode(KEY_MODE_NORMAL);
			retValue = MENU_RET_EXIT;			
			break;
	}
	return retValue;
}

/**
********************************************************************************
* @brief    Program Printer Setup \n
* @param    key : not use
* @return   
* @remark   
********************************************************************************
*/
INT8U pgmPrinterSetup(KEY_TYPE key)	     // getwing
{
	INT8U  	retValue;
	static INT8U tempINT8U;	
	static INT8S charIndex;

	retValue = MENU_RET_INCOMPLETE;

	switch (MenuFuncState)
	{
		case 0x00:	// Index 초기화 			
			MenuSetKeyMode(KEY_MODE_HEX);
			charIndex = 0;	
			MenuFuncState = 0x01;
		case 0x01:	// OneTime E2ROM Data read 
			tempINT8U = ParamReadByte(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE1 + charIndex);
			MenuFuncState = 0x02;
		case 0x02:	// Data input mode

			DispSetStr(WIN_TOTAL_PRICE, "HEAD 1");
			DispSetNum(WIN_UNIT_PRICE, charIndex + 1, 0);				
			DispSetCharWithHexNum(WIN_WEIGHT, tempINT8U);		

			if (key >= KEY_NUM_0 && key <= KEY_NUM_9)
			{
				tempINT8U = EditorInsHexDigit(tempINT8U, key-KEY_NUM_0, 2, 0xff, FALSE);
			}
			else if (key >= KEY_NUM_A && key <=KEY_NUM_F)
			{
				tempINT8U = EditorInsHexDigit(tempINT8U, key-KEY_NUM_A+10, 2, 0xff, FALSE);
			}
			else if (key == MenuKeyEnter)
			{
				ParamWriteByte(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE1 + charIndex, tempINT8U);
				
				if (charIndex < HEAD_LINE1_SIZE)
				{	
					tempINT8U = 0x00;
					charIndex++;
					for ( ; charIndex < HEAD_LINE1_SIZE; charIndex++)
					{
						ParamWriteByte(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE1 + charIndex, tempINT8U);		
					}
				}

				MenuFuncState = 0x10;	  		// goto next line
			}				
			else if (key == MenuKeyC)
			{	
				ParamWriteByte(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE1 + charIndex, tempINT8U);				
				charIndex++;				
				if (charIndex < HEAD_LINE1_SIZE)
				{
					MenuFuncState = 0x01;		// goto Next Index
				}
				else
				{
					MenuFuncState = 0x10; 		// goto Next Line
				}
			}
			else if (key == MenuKeyMenuUp)
			{
				ParamWriteByte(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE1 + charIndex, tempINT8U);				
				charIndex--;				
				if (charIndex < 0)
				{
					charIndex = 0;
				}
				MenuFuncState = 0x01;		// goto Next Index							
			}
			break;

		case 0x10:	// Index 초기화 			
			charIndex = 0;	
			MenuFuncState = 0x11;
		case 0x11:	// OneTime E2ROM Data read 
			tempINT8U = ParamReadByte(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE2 + charIndex);
			MenuFuncState = 0x12;
		case 0x12:	// Data input mode
			DispSetStr(WIN_TOTAL_PRICE, "HEAD 2");
			DispSetNum(WIN_UNIT_PRICE, charIndex + 1, 0);				
			DispSetCharWithHexNum(WIN_WEIGHT, tempINT8U);		

			if (key >= KEY_NUM_0 && key <= KEY_NUM_9)
			{
				tempINT8U = EditorInsHexDigit(tempINT8U, key - KEY_NUM_0, 2, 0xff, FALSE);
			}
			else if (key >= KEY_NUM_A && key <= KEY_NUM_F)
			{
				tempINT8U = EditorInsHexDigit(tempINT8U, key - KEY_NUM_A + 10, 2, 0xff, FALSE);
			}

			else if (key == MenuKeyEnter)
			{
				ParamWriteByte(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE2 + charIndex, tempINT8U);
				
				if (charIndex < HEAD_LINE2_SIZE)
				{	
					tempINT8U = 0x00;
					charIndex++;
					for ( ; charIndex < HEAD_LINE2_SIZE; charIndex++)
					{						
						ParamWriteByte(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE2 + charIndex, tempINT8U);		
					}
				}

				MenuFuncState = 0x20;	  		// goto next line
			}	
			else if (key == MenuKeyC)
			{	
				ParamWriteByte(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE2 + charIndex, tempINT8U);				
				charIndex++;				
				if (charIndex < HEAD_LINE2_SIZE)
				{
					MenuFuncState = 0x11;		// goto Next Index
				}
				else
				{
					MenuFuncState = 0x20; 		// goto Next Line
				}
			}
			else if (key == MenuKeyMenuUp)
			{
				ParamWriteByte(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE2 + charIndex, tempINT8U);				
				charIndex--;				
				if (charIndex < 0)
				{
					charIndex = 0;
				}
					MenuFuncState = 0x11;		// goto Next Index							
			}

			break;

		case 0x20:	// Index 초기화 			
			charIndex = 0;	
			MenuFuncState = 0x21;
		case 0x21:	// OneTime E2ROM Data read 
			tempINT8U = ParamReadByte(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE3 + charIndex);
			MenuFuncState = 0x22;
		case 0x22:	// Data input mode
			DispSetStr(WIN_TOTAL_PRICE, "HEAD 3");
			DispSetNum(WIN_UNIT_PRICE, charIndex + 1, 0);				
			DispSetCharWithHexNum(WIN_WEIGHT, tempINT8U);

			if (key >= KEY_NUM_0 && key <= KEY_NUM_9)
			{
				tempINT8U = EditorInsHexDigit(tempINT8U, key - KEY_NUM_0, 2, 0xff, FALSE);
			}
			else if (key >= KEY_NUM_A && key <= KEY_NUM_F)
			{
				tempINT8U = EditorInsHexDigit(tempINT8U, key - KEY_NUM_A + 10, 2, 0xff, FALSE);
			}
			else if (key == MenuKeyEnter)
			{
				ParamWriteByte(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE3 + charIndex, tempINT8U);
				
				if ( charIndex < HEAD_LINE3_SIZE)
				{	
					tempINT8U = 0x00;
					charIndex++;
					for ( ; charIndex < HEAD_LINE3_SIZE; charIndex++)
					{						
						ParamWriteByte(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE3 + charIndex, tempINT8U);		

					}
				}

				MenuFuncState = 0x30;	  		// goto next line
			}	
			else if (key == MenuKeyC)
			{	
				ParamWriteByte(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE3 + charIndex, tempINT8U);				
				charIndex++;				
				if (charIndex < HEAD_LINE3_SIZE)
				{
					MenuFuncState = 0x21;		// goto Next Index 
				}
				else
				{
					MenuFuncState = 0x30; 		// goto Next Line
				}
			}
			else if (key == MenuKeyMenuUp)
			{
				ParamWriteByte(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE3 + charIndex, tempINT8U);				
				charIndex--;				
				if (charIndex < 0)
				{
					charIndex = 0;
				}
					MenuFuncState = 0x21;		// goto Next Index							
			}		
			break;


		case 0x30:	// Index 초기화 			
			charIndex = 0;	
			MenuFuncState = 0x31;
		case 0x31:	// OneTime E2ROM Data read 
			tempINT8U = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PRICE_SYMBOL + charIndex);
			MenuFuncState = 0x32;
		case 0x32:	// Data input mode
			DispSetStr(WIN_TOTAL_PRICE, " UNIT ");
			DispSetNum(WIN_UNIT_PRICE, charIndex + 1, 0);				
			DispSetCharWithHexNum(WIN_WEIGHT, tempINT8U);

			if (key >= KEY_NUM_0 && key <= KEY_NUM_9)
			{
				tempINT8U = EditorInsHexDigit(tempINT8U, key - KEY_NUM_0, 2, 0xff, FALSE);
			}
			else if (key >= KEY_NUM_A && key <= KEY_NUM_F)
			{
				tempINT8U = EditorInsHexDigit(tempINT8U, key - KEY_NUM_A + 10, 2, 0xff, FALSE);
			}

			else if (key == MenuKeyEnter)
			{
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PRICE_SYMBOL + charIndex, tempINT8U);
				
				if (charIndex < PRICE_SYMBOL_SIZE)
				{	
					tempINT8U = 0x00;
					charIndex++;
					for ( ; charIndex < PRICE_SYMBOL_SIZE; charIndex++)
					{						
						ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PRICE_SYMBOL + charIndex, tempINT8U);		

					}
				}

				MenuFuncState = 0x40;	  		// goto next line
			}	
			else if (key == MenuKeyC)
			{	
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PRICE_SYMBOL + charIndex, tempINT8U);				
				charIndex++;				
				if (charIndex < PRICE_SYMBOL_SIZE)
				{
					MenuFuncState = 0x31;		// goto Next Index 
				}
				else
				{
					MenuFuncState = 0x40; 		// goto Next Line
				}
			}
			else if (key == MenuKeyMenuUp)
			{
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PRICE_SYMBOL + charIndex, tempINT8U);				
				charIndex--;				
				if (charIndex < 0)
				{
					charIndex = 0;
				}
					MenuFuncState = 0x31;		// goto Next Index							
			}		
			break;

		case 0x40:	 			
			tempINT8U = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_MONTH);
			MenuFuncState = 0x41; 
		case 0x41:	
			DispSetStr(WIN_TOTAL_PRICE, " DATE ");
			DispSetStr(WIN_UNIT_PRICE, " MONTH");			
			DispSetNum(WIN_WEIGHT, tempINT8U, 0);			
			if (key >= KEY_NUM_0 && key <= KEY_NUM_9)
			{
				tempINT8U = EditorInsDecDigit(tempINT8U, key - KEY_NUM_0, 2, 12, FALSE);								
			}
			else if (key == MenuKeyEnter)
			{
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_MONTH, tempINT8U);
				MenuFuncState = 0xF0;	  	
			}	
			else if (key == MenuKeyC)
			{	
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_MONTH, tempINT8U);				
				MenuFuncState = 0x50; 						
			}		
			break;

		case 0x50:	 			
			tempINT8U = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_DAY);
			MenuFuncState = 0x51; 
		case 0x51:	
			DispSetStr(WIN_TOTAL_PRICE, " DATE ");
			DispSetStr(WIN_UNIT_PRICE, "  DAY ");			
			DispSetNum(WIN_WEIGHT, tempINT8U, 0);

			if (key >= KEY_NUM_0 && key <= KEY_NUM_9)
			{
				tempINT8U = EditorInsDecDigit(tempINT8U, key - KEY_NUM_0, 2, 31, FALSE);
			}
			else if (key == MenuKeyEnter)
			{
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_DAY, tempINT8U);
				MenuFuncState = 0xF0;	  	
			}	
			else if (key == MenuKeyC)
			{	
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_DAY, tempINT8U);				
				MenuFuncState = 0x60; 						
			}		
			break;

		case 0x60:	 			
			tempINT8U = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_YEAR);
			MenuFuncState = 0x61; 
		case 0x61:	
			DispSetStr(WIN_TOTAL_PRICE, " DATE ");
			DispSetStr(WIN_UNIT_PRICE, " YEAR ");			
			DispSetNum(WIN_WEIGHT, 2000 + tempINT8U, 0);

			if (key >= KEY_NUM_0 && key <= KEY_NUM_9)
			{				
				tempINT8U = EditorInsDecDigit(tempINT8U, key - KEY_NUM_0, 2, 99, FALSE);								
			}
			else if (key == MenuKeyEnter)
			{
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_YEAR, tempINT8U);
				MenuFuncState = 0xF0;	  	
			}	
			else if (key == MenuKeyC)
			{	
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_YEAR, tempINT8U);				
				MenuFuncState = 0xF0;						
			}		
			break;

		case 0xF0:
			MenuSetKeyMode(KEY_MODE_NORMAL);
			retValue = MENU_RET_EXIT;			
			break;
	}
	return retValue;
}

#endif		//#if (MODEL_NUM == MODEL_EB_LCD)
