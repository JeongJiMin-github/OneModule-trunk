/**
********************************************************************************
* Copyright (c) 2012 CAS
* @brief   Program Mode Applicaion Source File\n
* @file    PgmModeApp_PR_PLUS.c
* @version 1.0
* @date    2012/09/24
* @author  Young Hoon Lee (LYH)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2012/09/24 created by LYH \n
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
#include "../Common/RingBuf.h"
#include "../DB/PluApi.h"
#include "../DB/StorageApi.h"
#include "../DB/ParamApi.h"
#include "../UI/DispApi.h"
#include "../UI/KeyApi.h"
#include "../UI/EditorApi.h"
#include "../UI/MenuApi.h"
#include "../Communication/DebugApi.h"
#include "../Communication/PrtApi.h"
#include "../Communication/CommApi.h"
#include "../AD/AdApi.h"
#include "../Time/TimeApi.h"
#include "../Power/PowerApi.h"
#include "ScaleModeApp.h"
#include "PgmModeApp.h"
#include "SaleModeApp.h"
#include "../SystemTimer/TimerApi.h"
#include "../Common/SaleData.h"
#include "../Common/AdData.h"

#ifdef USE_PGM_PR_PLUS
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
#define MENU_PGM_MAX_POS 4
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
	{0x1000, "PGM", 		MENU_MODE_RUN, 			pgmExitFunc},
	{0x1100, "PGM", 		MENU_MODE_NORMAL, 		pgmExitFunc},
	{0x1F10, "SETUP", 		MENU_MODE_NORMAL, 		pgmUserSetup},
	{0x1F30, "FILT", 		MENU_MODE_NORMAL, 		pgmFilterSetup}
};

/** @brief Program Direct Menu Table 개수*/
#define MENU_DIRECT_PGM_MAX_POS 2
/** @brief Program Direct Menu Table */
const DIRECT_MENU code pgmDirectMenuTable[MENU_DIRECT_PGM_MAX_POS] = 
{
	{KEY_ZERO, 0x1F10},
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
#define POWER_STAND_BY_TIME_TABLE_SIZE	4
const INT8U code PowerStandbyTimeTable[POWER_STAND_BY_TIME_TABLE_SIZE] = 										
{
	0, 20, 40, 60
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
	INT8U	backUpData;
	INT8U	i;
	
	static INT8U tempByte;

	retValue = MENU_RET_INCOMPLETE;

	switch (MenuFuncState)
	{
		case 0x00:	// initial
#ifdef REVISION_NUM
			if(!TimerCheckDelay100ms(TIMER_DELAY_100MS_INITIAL))
			{
				DispSetMsgWithNum("r", REVISION_NUM);
				TimerSetDelay100ms(TIMER_DELAY_100MS_INITIAL, 10);//include disp. revision 10
				while (!TimerCheckDelay100ms(TIMER_DELAY_100MS_INITIAL));	//wait key
			}
#endif
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

			if (tempByte == 0)
			{
				DispSetStr(WIN_UNIT_PRICE, "  NO  ");
			}
			else
			{
				DispSetNum(WIN_UNIT_PRICE, PowerAutoOffTimeTable[tempByte], 0);
			}

			if (key == MenuKeyMenuUp)				 
			{
				ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_AUTO_OFF_TIME, PowerAutoOffTimeTable[tempByte]);
				PowerSaverInit();//적용
				PowerInit();
#ifdef USE_USER_SETTING_FOR_DOUBLEZERO_BATTLED			
				MenuFuncState = 0x10;
#else
				MenuFuncState = 0x30;
#endif
			}
			else if (key == MenuKeyEnter)
			{
				tempByte++;
				tempByte %= 4;	//0,10,30,60min
			}
			break;
#ifdef USE_USER_SETTING_FOR_DOUBLEZERO_BATTLED			
		case 0x10:		// define [Decimal] key usage : [Decimal], [Num00]
			tempByte = ParamReadByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BLINK_COUNT);
			MenuFuncState = 0x11;

		case 0x11:		// define [Decimal] key usage : [Decimal], [Num00]		
			DispSetStr(WIN_WEIGHT, "  SET ");				
			DispSetStr(WIN_TOTAL_PRICE, " POINT");
			
			if (tempByte & 0x01) // Use Double Zero [00] key
			{
				DispSetStr(WIN_UNIT_PRICE, "NUM00 ");			
			}
			else	// Use [Decimal Point]key : Default
			{
				DispSetStr(WIN_UNIT_PRICE, "POINT ");
			}
			
			if (key == MenuKeyMenuUp)				 
			{
				ParamWriteByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BLINK_COUNT, tempByte);		//DISP_PARAM_OFFSET_BLINK_COUNT는 LED 제품에서 사용하지 않음
				PowerSaverInit();//적용
				PowerInit();
				//KeyInit();	//Menu 이동 시 문제 있음
				MenuFuncState = 0x20;
			}
			else if (key == MenuKeyEnter)
			{
				if (tempByte & 0x01) tempByte = tempByte & ~(0x01);
				else tempByte = tempByte | (0x01);
			}
			break;

		case 0x20:		// Batt LED indicator On/Off
			tempByte = ParamReadByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BLINK_COUNT);			//DISP_PARAM_OFFSET_BLINK_COUNT는 LED 제품에서 사용하지 않음
			MenuFuncState = 0x21;

		case 0x21:		// Batt LED indicator On/Off
			DispSetStr(WIN_WEIGHT, "  USE ");				
			DispSetStr(WIN_TOTAL_PRICE, "BATLED");
			
			if (tempByte & 0x02) // BattLED Off
			{
				DispSetStr(WIN_UNIT_PRICE, "  OFF ");			
			}
			else // BattLED On
			{
				DispSetStr(WIN_UNIT_PRICE, "   ON ");
			}
			if (key == MenuKeyMenuUp)
			{
				ParamWriteByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BLINK_COUNT, tempByte);		//DISP_PARAM_OFFSET_BLINK_COUNT는 LED 제품에서 사용하지 않음
				PowerSaverInit();//적용
				PowerInit();
				MenuFuncState = 0x30;
			}
			else if (key == MenuKeyEnter)
			{
				if (tempByte & 0x02) tempByte = tempByte & ~(0x02);
				else tempByte = tempByte | (0x02);
			}
			break;
#endif
		case 0x30:		// initial
#ifdef USE_SUSPEND_MODE
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

			MenuFuncState = 0x31;	
#else
			retValue = MENU_RET_EXIT;
			break;
#endif
		case 0x31:
			DispSetStr(WIN_WEIGHT, "STAND ");
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
//				MenuFuncState = 0x40;
				retValue = MENU_RET_EXIT;				
				PowerInit();
#ifdef USE_USER_SETTING_FOR_DOUBLEZERO_BATTLED	
				KeyInit();	//Point, Double Zero key change
#endif
			}
			else if (key == MenuKeyEnter)
			{
				tempByte++;
				tempByte %= POWER_STAND_BY_TIME_TABLE_SIZE;
			}
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
	INT8U	temp;
	static INT8U tempINT8U;	
	static INT8S charIndex;

	retValue = MENU_RET_INCOMPLETE;

	switch (MenuFuncState)
	{
		case 0x00:	// Index 초기화 			
			MenuSetKeyMode(KEY_MODE_HEX);
			charIndex = 0;	
			MenuFuncState = 0x01;
			break;
		case 0x01:	// OneTime E2ROM Data read 
			temp = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND); 	
			temp  = (temp & 0x10) >> 4;	//Use head message(0x02) or not(0x30)
			if(temp)
			{
				tempINT8U = ParamReadByte(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE1 + charIndex);
				MenuFuncState = 0x02;
			}
			else
			{
				MenuFuncState = 0x30;
			}
			break;
		case 0x02:	// Data input mode
			DispSetStr(WIN_TOTAL_PRICE, "HEAD 1");
			DispSetNum(WIN_UNIT_PRICE, charIndex + 1, 0);	//Initial value of charIndex is 0			
			DispSetCharWithHexNum(WIN_WEIGHT, tempINT8U);	//tempINT8U - Read value	(hex)

			if (key >= KEY_NUM_0 && key <= KEY_NUM_9)
			{
				tempINT8U = EditorInsHexDigit(tempINT8U, key-KEY_NUM_0, 2, 0xff, FALSE);
			}
			else if (key >= KEY_NUM_A && key <=KEY_NUM_F)
			{
				tempINT8U = EditorInsHexDigit(tempINT8U, key-KEY_NUM_A+10, 2, 0xff, FALSE);
			}
			else if (key == MenuKeyEnter)	//for ER Tare Key - skip inputting
			{
				ParamWriteByte(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE1 + charIndex, tempINT8U);
				MenuFuncState = 0x10;	  		// goto next line
			}				
			else if (key == MenuKeyC)		//for ER C Key - store 1 char
			{	
				ParamWriteByte(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE1 + charIndex, tempINT8U);//0				
				charIndex++;		////1		
				if (charIndex < HEAD_LINE1_SIZE)
				{
					MenuFuncState = 0x01;		// goto Next Index
				}
				else
				{
					MenuFuncState = 0x10; 		// goto Next Line
				}
			}
			else if (key == MenuKeyMenuUp)	// for ER ZERO key - back 1 char
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
			break;
		case 0x31:	// OneTime E2ROM Data read 
			tempINT8U = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PRICE_SYMBOL + charIndex);
			MenuFuncState = 0x32;
			break;
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

				MenuFuncState = 0x42;	  		// goto next line
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
					MenuFuncState = 0x42; 		// goto Next Line
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

		case 0x42:
			tempINT8U = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_MONTH);			
			MenuFuncState = 0x43; 
			break;
			
		case 0x43:	
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
				MenuFuncState = 0xA0;		
			}	
			else if (key == MenuKeyC)
			{	
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_MONTH, tempINT8U);		
				TimeData.month = tempINT8U;
				TimeWriteData();
				MenuFuncState = 0x50;						
			}
			else if (key == MenuKeyMenuUp)
			{
				TimeData.month = tempINT8U;
				TimeWriteData();
				MenuFuncState = 0x42; 		
			}
			break;
			

		case 0x50:	
			tempINT8U = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_DAY);
			MenuFuncState = 0x51; 
			break;
			
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
				MenuFuncState = 0xA0;	  	
			}	
			else if (key == MenuKeyC)
			{	
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_DAY, tempINT8U);
				MenuFuncState = 0x60; 						
			}				
			else if (key == MenuKeyMenuUp)
			{
				TimeData.date = tempINT8U;
				TimeWriteData();
				MenuFuncState = 0x50; 		
			}
			break;

		case 0x60:	 
			tempINT8U = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_YEAR);
			MenuFuncState = 0x61;
			break;
			
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
				MenuFuncState = 0xA0;	  	
			}	
			else if (key == MenuKeyC)
			{	
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_YEAR, tempINT8U);	
				MenuFuncState = 0xA0;						
			}				
			else if (key == MenuKeyMenuUp)
			{
				TimeData.year = tempINT8U;
				TimeWriteData();
				MenuFuncState = 0x60; 		
			}
			break;

		case 0xA0:
			tempINT8U = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_DATE_TYPE); //read date type
			//0 : YY/MM/DD, 1 : DD/MM/YY
			MenuFuncState = 0xA1;
			break;

		case 0xA1:
			DispSetStr(WIN_TOTAL_PRICE, " DATE ");
			DispSetStr(WIN_UNIT_PRICE, " TYPE ");
			switch(tempINT8U)
			{
				case 0:
				default:
					DispSetStr(WIN_WEIGHT, "YYMMDD");
					tempINT8U = 0;
					break;

				case 1:
					DispSetStr(WIN_WEIGHT, "DDMMYY");
					break;
			}
			if (key >= KEY_NUM_0 && key <= KEY_NUM_9)
			{				
				tempINT8U = EditorInsDecDigit(tempINT8U, key - KEY_NUM_0, 1, 2, FALSE);								
			}
			else if (key == MenuKeyEnter)
			{
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_DATE_TYPE, tempINT8U);
				MenuFuncState = 0xF0;		
			}	
			else if (key == MenuKeyC)
			{	
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_DATE_TYPE, tempINT8U);
				MenuFuncState = 0xF0;						
			}				
			else if (key == MenuKeyMenuUp)
			{
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_DATE_TYPE, tempINT8U);
				MenuFuncState = 0xA0;			
			}
			break;
			
		case 0xF0:
			MenuSetKeyMode(KEY_MODE_NORMAL);
			retValue = MENU_RET_EXIT;			
			break;
	}
	return retValue;
}

#endif		//#ifdef USE_PGM_PR_PLUS
