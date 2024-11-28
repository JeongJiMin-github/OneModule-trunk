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
#include "../Common/RingBuf.h"

#include "../DB/PluApi.h"
#include "../DB/StorageApi.h"
#include "../DB/ParamApi.h"
#include "../UI/DispApi.h"
#include "../UI/KeyApi.h"
#include "../UI/EditorApi.h"
#include "../UI/MenuApi.h"
#include "../UI/BuzzerApi.h"

#include "../Power/PowerApi.h"
#include "ScaleModeApp.h"
#include "PgmModeApp.h"

#include "../Common/PluData.h"
#include "../db/PluApi.h"
#include "../Communication/CommDrv.h"
#include "../Communication/DebugApi.h"
#include "../Application/SaleModeApp.h"
#include "../Communication/PrtApi.h"
#include "../Communication/CommApi.h"
#include "../Time/TimeApi.h"
#include "../Systemtimer/TimerApi.h"
#include "../Common/TimeData.h"
#include "../AD/AdApi.h"

#ifdef USE_PGM_DB1H
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
#define PGM_INIT_STATE 						0
#define PGM_PRT_SELECT_STATE				1
#define PGM_PRT_SET_STATE 					2
#define PGM_PRT_LF_SET_STATE				3
#ifdef USE_6_BAUDRATE
#define PGM_BAUDRATE_SET_STATE 				4
#endif

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

/** @brief Program mode menu */
static INT8U pgmUserSetMode;
/** @brief Buzzer on/off flag */
static INT8U pgmBuzzerOnOffFlag;	
/** @brief Printer 관련 Offset */
static INT8U pgmPrintOffet;
/** @brief bit 1 & bit 0, 사용 프린터 종류, 00 : Don't Use, 01: Use DEP, 10: Use DLP, 11: reserve*/
static INT8U pgmPrinterType;
/** @brief Print Barcode on/off flag */
static INT8U pgmPrintBarcode;
/** @brief Printer method */
static INT8U pgmPrintLfSize;
/** @brief Communication Baudrate */
static INT8U pgmBaudRate;
/** @brief BackLight on/off flag */
static INT8U pgmBlOnOffFlag;
/** @brief BackLight brightness */
static INT8U pgmBlBrightness;
/** @brief Auto on/off flag */
static INT8U pgmAutoOnOffFlag;
/** @brief Compare mode range */
static INT8U pgmCompareRange;
/** @brief Count compare mode range */
static INT8U pgmCountCompareRange;
/** @brief Printer method */
static INT8U pgmPrintMethod;
/** @brief Date print on/off flag */
static INT8U pgmDateOnOffFlag;
/** @brief Time print on/off flag */
static INT8U pgmTimeOnOffFlag;
/** @brief Label print on/off flag */
static INT8U pgmLabelOnOffFlag;
/** @brief Default setting on/off flag */
static INT8U pgmDefaultOnOffFlag;
/** @brief Date edit flag */
static BOOLEAN pgmDateEdit;
/** @brief Time edit flag */
static BOOLEAN pgmTimeEdit;
/** @brief Label edit flag */
static BOOLEAN pgmLabelEdit;

/** @brief Label edit flag */
static INT32U pgmDateStruct;

/** @brief Label edit flag */
static INT32U pgmTimeStruct;

/** @brief Program Menu Table 개수*/
#define MENU_PGM_MAX_POS 3
/** @brief Program Menu Table*/
#ifdef IAR
MENU code pgmMenuTable[MENU_PGM_MAX_POS] = 
#else
const MENU code pgmMenuTable[MENU_PGM_MAX_POS] = 
#endif
{
	{0x1000, "PGM",	MENU_MODE_RUN, 		pgmExitFunc},
	{0x1F10, "U SET", 	MENU_MODE_NORMAL, 		pgmUserSetup},
	{0x1F20, "FILT", 	MENU_MODE_NORMAL, 		pgmFilterSetup}	
};
/** @brief Program Direct Menu Table 개수*/
#define MENU_DIRECT_PGM_MAX_POS 2
/** @brief Program Direct Menu Table */
#ifdef IAR
DIRECT_MENU code pgmDirectMenuTable[MENU_DIRECT_PGM_MAX_POS] = 
#else
const DIRECT_MENU code pgmDirectMenuTable[MENU_DIRECT_PGM_MAX_POS] = 
#endif
{
	{KEY_ZERO, 0x1F10 },
	{KEY_TARE, 0x1F20 }
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
#define POWER_BL_OFF_TIME_TABLE_SIZE	3
const INT8U code PowerBLOffTimeTable[POWER_BL_OFF_TIME_TABLE_SIZE] = 
{
	0, 10, 30
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
	pgmUserSetMode = PGM_INIT_STATE;
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
* @param    key : TARE KEY(시간 설정, 0분, 10분, 20분, 30분) 
* @return   Auto Power off 시간 설정
* @remark   NONE           
********************************************************************************
*/
INT8U pgmUserSetup(KEY_TYPE key)
{	
	switch (pgmUserSetMode)
	{
		case PGM_INIT_STATE:
#ifdef REVISION_NUM
			if(!TimerCheckDelay100ms(TIMER_DELAY_100MS_INITIAL))
			{
				DispSetMsgWithNum("r", REVISION_NUM);
				TimerSetDelay100ms(TIMER_DELAY_100MS_INITIAL, 10);//include disp. revision 10
				while (!TimerCheckDelay100ms(TIMER_DELAY_100MS_INITIAL));	//wait key
			}
#endif
			DispSetStr(MenuDataWinNum, "U SET");			
			pgmPrintOffet = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST);
			pgmPrintMethod = pgmPrintOffet >> 6;
			//pgmPrinterType -> 00 : Not use, 01 : DEP50, 10 : DLP50, 11 : RESERVED
			pgmBaudRate = ParamReadByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_BAUDRATE);
			pgmPrinterType = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH) & 0x03;
			pgmPrintBarcode = (pgmPrintOffet & 0x08) >> 3;
			pgmPrintLfSize = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_LF_SIZE);
			break;

		case PGM_PRT_SELECT_STATE:
			switch(pgmPrinterType)
			{
				case PRT_NONE:
				default:
					DispSetStr(MenuDataWinNum, "PR OFF");
					pgmPrinterType = 0;
					break;
				case PRT_DEP50:
					DispSetStr(MenuDataWinNum, "PR DEP");	
					break;
				case PRT_DLP50:
					DispSetStr(MenuDataWinNum, "PR DLP");	
					break;		
			}
			break;
			
		case PGM_PRT_SET_STATE:
			if(pgmPrinterType == PRT_DEP50)
			{
				switch(pgmPrintMethod)
				{
					default:
					case 1:
						DispSetStr(MenuDataWinNum, "PR KEY");
						pgmPrintMethod = 1;
						break;
					case 2:
						DispSetStr(MenuDataWinNum, "PR AUT");
						break;
					case 3:
						DispSetStr(MenuDataWinNum, "PR STR");
						break;
				}
			}
			else if(pgmPrinterType == PRT_DLP50)
			{
				switch(pgmPrintMethod)
				{
					default:
					case 1:
						DispSetStr(MenuDataWinNum, "PR KEY");
						break;
					case 2:
						DispSetStr(MenuDataWinNum, "PR AUT");
						break;
				}
			}
			break;

		case PGM_PRT_LF_SET_STATE:
			if(pgmPrinterType == PRT_DEP50)
			{
				switch(pgmPrintLfSize)
				{
					case 1:
					default:
						DispSetStr(MenuDataWinNum, "LF 1");
						pgmPrintLfSize = 1;
						break;
					case 2:
						DispSetStr(MenuDataWinNum, "LF 2");
						break;
					case 3:
						DispSetStr(MenuDataWinNum, "LF 3");
						break;
					case 4:
						DispSetStr(MenuDataWinNum, "LF 4");
						break;
					case 5:
						DispSetStr(MenuDataWinNum, "LF 5");
						break;
					case 6:
						DispSetStr(MenuDataWinNum, "LF 6");
						break;
					case 7:
						DispSetStr(MenuDataWinNum, "LF 7");
						break;
					case 8:
						DispSetStr(MenuDataWinNum, "LF 8");
						break;
					case 9:
						DispSetStr(MenuDataWinNum, "LF 9");
						break;
				}
			}
			else if(pgmPrinterType == PRT_DLP50)
			{
				if(pgmPrintBarcode == ON)
				{					
					DispSetStr(MenuDataWinNum, "BC  ON");
				}
				else
				{
					DispSetStr(MenuDataWinNum, "BC OFF");
				}
			}
			break;
			
#ifdef USE_6_BAUDRATE
		case PGM_BAUDRATE_SET_STATE:
			switch(pgmBaudRate)
			{
				case 0:
				default:
					DispSetStr(MenuDataWinNum, "BR  96");
					pgmBaudRate = 0;
					break;
				case 1:
					DispSetStr(MenuDataWinNum, "BR 192");
					break;
				case 2:
					DispSetStr(MenuDataWinNum, "BR 384");
					break;
				case 3:
					DispSetStr(MenuDataWinNum, "BR 576");
					break;
				case 4:
					DispSetStr(MenuDataWinNum, "BR1152");
					break;
				case 5:
					DispSetStr(MenuDataWinNum, "BR  24");
					break;
			}
			break;
#endif
		default:
			break;
	}
	
	if (key == KEY_TARE)
	{
		if(pgmUserSetMode == PGM_INIT_STATE) 
		{
			pgmUserSetMode++;
			return MENU_RET_INCOMPLETE;
		}
#ifdef USE_6_BAUDRATE
		if(pgmUserSetMode != PGM_BAUDRATE_SET_STATE) 
#else
		if(pgmUserSetMode != PGM_PRT_LF_SET_STATE) 
#endif
		{
			if(pgmPrinterType != PRT_NONE) pgmUserSetMode++;
			else 
			{
				DispSetConstStrMsg(WIN_WEIGHT, " SAVE");
				DispSetRunMsgPage(10, 1);
				pgmPrinterType |= (ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH) & 0xFC);
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, pgmPrinterType);
#ifdef USE_PRINTER
				PrtInit();
#endif
				return MENU_RET_EXIT;
			}
		}
		else 
		{
			DispSetConstStrMsg(WIN_WEIGHT, " SAVE");
			DispSetRunMsgPage(10, 1);
			pgmPrintOffet = 0;
			pgmPrintOffet |= (pgmPrintMethod << 6);
			pgmPrintOffet |= (pgmPrintBarcode << 3);
			pgmPrinterType |= (ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH) & 0xFC);
			ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST, pgmPrintOffet);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, pgmPrinterType);
			ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_LF_SIZE, pgmPrintLfSize);
			ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_BAUDRATE, pgmBaudRate);
#ifdef USE_PRINTER
			PrtInit();
#endif
			return MENU_RET_EXIT;
		}
	}
	
	if (key == KEY_ZERO)
	{
		switch(pgmUserSetMode)
		{
			case PGM_PRT_SELECT_STATE:
				if(pgmPrinterType != PRT_DLP50) pgmPrinterType++;
				else pgmPrinterType = PRT_NONE;
				break;
			case PGM_PRT_SET_STATE:	
				if(pgmPrinterType == PRT_DEP50)
				{
					if (pgmPrintMethod != 3)
					{
						pgmPrintMethod++;
					}
					else 
					{
						pgmPrintMethod = 1;
					}
				}
				else if(pgmPrinterType == PRT_DLP50)
				{
					if (pgmPrintMethod != 2)
					{
						pgmPrintMethod++;
					}
					else 
					{
						pgmPrintMethod = 1;
					}
				}
				break;
			case PGM_PRT_LF_SET_STATE:
				if(pgmPrinterType == PRT_DEP50)
				{
					if (pgmPrintLfSize != 9)
					{
						pgmPrintLfSize++;
					}
					else 
					{
						pgmPrintLfSize = 1;
					}
				}
				else if(pgmPrinterType == PRT_DLP50)
				{
					if (pgmPrintBarcode != 1)
					{
						pgmPrintBarcode++;
					}
					else 
					{
						pgmPrintBarcode = 0;
					}
				}
				break;
#ifdef	USE_6_BAUDRATE
			case PGM_BAUDRATE_SET_STATE:
				if (pgmBaudRate != 0)
				{
					pgmBaudRate -= 1;
				}
				else
				{
					pgmBaudRate = 5;
				}
				CommDrvInit(pgmBaudRate);
				break;
#endif
				default:
				break;
		}
	}
	return MENU_RET_INCOMPLETE;
}
#endif


