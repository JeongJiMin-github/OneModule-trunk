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
#include "../Communication/DebugApi.h"
#include "../Application/SaleModeApp.h"
#include "../Communication/PrtApi.h"
#include "../Communication/CommDrv.h"
#include "../Communication/CommApi.h"
#include "../Communication/DebugApi.h"
#include "../Time/TimeApi.h"
#include "../Systemtimer/TimerApi.h"
#include "../Common/TimeData.h"
#include "../AD/AdApi.h"
#include "../Hardware/HardwareConfig.h"

#ifdef USE_PGM_DB
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
#define PGM_BUZZER_SET_STATE 				1
#ifdef USE_WEIGHT_ONLY_ECR
 #define PGM_ECR_SET_STATE					2    //SW ECR Mode
#endif
#define PGM_PRT_SELECT_STATE				2
#define PGM_PRT_SET_STATE 					3
#define PGM_PRT_LF_SET_STATE				4
#define PGM_BAUDRATE_SET_STATE 				5
#define PGM_BACKLIGHT_SET_STATE 			6
#define PGM_BRIGHT_SET_STATE 				7
#define PGM_AUTOPOWER_SET_STATE 			8
#define PGM_COMPARE_SET_STATE 				9
#define PGM_COUNT_COMPARE_SET_STATE 		10
#define PGM_DATE_SET_STATE 					11
#define PGM_TIME_SET_STATE 					12
#define PGM_LABEL_SET_STATE 				13
#ifdef USE_SUSPEND_MODE
 #define PGM_SUSPEND_SET_STATE 				14
 #define PGM_DEFAULT_SET_STATE 				15
 #define PGM_DAY_SET_STATE 					16
 #define PGM_DATE_FORM_SET_STATE			17
#else
 #ifdef USE_TARE_AUTO_CLEAR_SET
  #define PGM_TARE_AUTO_CLEAR_SET_STATE		14
  #define PGM_DEFAULT_SET_STATE 			15
  #define PGM_DAY_SET_STATE 				16
  #define PGM_DATE_FORM_SET_STATE			17
 #else
  #define PGM_DEFAULT_SET_STATE 			14
  #define PGM_DAY_SET_STATE 				15
  #define PGM_DATE_FORM_SET_STATE			16
#endif
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
#ifdef USE_DB2_LED_5DIGIT_FUNC
static INT8U pgmDateSetup(KEY_TYPE key);
#endif

/** @brief Program mode menu */
static INT8U pgmUserSetMode;
/** @brief Buzzer on/off flag */
static INT8U pgmBuzzerOnOffFlag;	
/** @brief bit 1 & bit 0, printer type, 00 : Don't Use, 01: Use DEP, 10: Use DLP, 11: reserve*/
static INT8U pgmPrinterType;
/** @brief Printer 관련 Offset */
static INT8U pgmPrintOffet;
/** @brief Print Barcode on/off flag */
static INT8U pgmPrintBarcode;
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
/** @brief Printer method */
static INT8U pgmPrintLfSize;
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
#ifdef USE_WEIGHT_ONLY_ECR
/** @brief ECR Printer Type */
static INT8U pgmECRType;
#endif
/** @brief DATE edit flag */
static INT32U pgmDateStruct;

#ifdef USE_TARE_AUTO_CLEAR_SET
/** @brief Tare Auto Clear flag */
static INT8U pgmAutoTareClearFlag;
#endif


/*-----------------------------------------------------------*/
#ifdef USE_DB2_LED_5DIGIT_FUNC
/** @brief YEAR edit flag */
static INT32U pgmDateYearStruct;
/** @brief YEAR edit flag */
static INT32U pgmDateMonthStruct;
/** @brief YEAR edit flag */
static INT32U pgmDateDayStruct;
/** @brief YEAR edit flag */
static INT32U pgmDateHourStruct;
/** @brief YEAR edit flag */
static INT32U pgmDateMinuteStruct;
/** @brief YEAR edit flag */
static INT32U pgmDateSecondStruct;
/** @brief RTC EditMode Check flag */
static INT8U MenuMode =0;
#endif
/*-----------------------------------------------------------*/
/** @brief Label edit flag */
static INT32U pgmTimeStruct;

/** @brief Suspend Time */
static INT8U pgmSuspendTime;


/** @brief Date Format flag*/
static INT8U pgmDateFormFlag; 


/** @brief Program Menu Table 개수*/
#ifdef USE_DB2_LED_5DIGIT_FUNC
#define MENU_PGM_MAX_POS 4
/** @brief Program Menu Table*/
#ifdef IAR
MENU code pgmMenuTable[MENU_PGM_MAX_POS] = 
#else
const MENU code pgmMenuTable[MENU_PGM_MAX_POS] = 
#endif
{
	{0x1000, "PGM",	MENU_MODE_RUN, 		pgmExitFunc},
	{0x1F10, "U SET", 	MENU_MODE_NORMAL, 		pgmUserSetup},
	{0x1F20, "FILT", 	MENU_MODE_NORMAL, 		pgmFilterSetup},
	{0x1F30, "DATE", 	MENU_MODE_NORMAL, 		pgmDateSetup}
};
#else
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
	{0x1F20, "FILT", 	MENU_MODE_NORMAL, 		pgmFilterSetup},
};
#endif



#ifdef USE_DB2_LED_5DIGIT_FUNC
/** @brief Program Direct Menu Table round numbers*/
#define MENU_DIRECT_PGM_MAX_POS 3
/** @brief Program Direct Menu Table */
#ifdef IAR
DIRECT_MENU code pgmDirectMenuTable[MENU_DIRECT_PGM_MAX_POS] = 
#else
const DIRECT_MENU code pgmDirectMenuTable[MENU_DIRECT_PGM_MAX_POS] = 
#endif
{
	{KEY_PRINT, 0x1F10 },
	{KEY_TARE, 0x1F20 },
	{KEY_ZERO, 0x1F30 }
};
#else
/** @brief Program Direct Menu Table round numbers*/
#define MENU_DIRECT_PGM_MAX_POS 2
#ifdef IAR
DIRECT_MENU code pgmDirectMenuTable[MENU_DIRECT_PGM_MAX_POS] = 
#else
const DIRECT_MENU code pgmDirectMenuTable[MENU_DIRECT_PGM_MAX_POS] = 
#endif
{
	{KEY_PRINT, 0x1F10 },
	{KEY_TARE, 0x1F20 }
};
#endif

/** 
* @brief Auto Power Off Mode Time Setting Table (unit : min) 
* @remark 0-always on, 10,30,60 - delay min.
*/
/*
#define POWER_AUTO_OFF_TIME_TABLE_SIZE	4
const INT8U code PowerAutoOffTimeTable[POWER_AUTO_OFF_TIME_TABLE_SIZE] = 
{
	0, 10, 30, 60
};
*/
/** 
* @brief Auto BL Off Mode Time Setting Table (unit : sec) 
* @remark 0-initial off, 1-always on, 3,5,10 - delay sec.
*/
/*
#define POWER_BL_OFF_TIME_TABLE_SIZE	3
const INT8U code PowerBLOffTimeTable[POWER_BL_OFF_TIME_TABLE_SIZE] = 
{
	0, 10, 30
};
*/
/** 
* @brief LED Suspend Mode Time Setting Table (unit : sec) 
* @remark 0-always on, 20, 40, 60 - delay sec
*/
/*
#define POWER_STAND_BY_TIME_TABLE_SIZE	4
const INT8U code PowerStandbyTimeTable[POWER_STAND_BY_TIME_TABLE_SIZE] = 										
{
	0, 20, 40, 60
};
*/

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
	pgmDateEdit = 0;
	pgmTimeEdit = 0;
	pgmLabelEdit = 0;
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


/**
********************************************************************************
* @brief    Program User Setup \n
* @param    key : TARE KEY(시간 설정, 0분, 10분, 20분, 30분) 
* @return   Year Month Date 설정
* @remark   NONE           
********************************************************************************
*/
#ifdef USE_DB2_LED_5DIGIT_FUNC
INT8U pgmDateSetup(KEY_TYPE key)
{
	switch (MenuFuncState)
	{
//		default:
		case 0x00:
			
			TimeReadData();
			DispSetStr(WIN_WEIGHT, " TIME");

			pgmDateYearStruct = TimeData.year;
			pgmDateMonthStruct = TimeData.month;
			pgmDateDayStruct = TimeData.date;
			
			pgmDateHourStruct = TimeData.hour;
			pgmDateMinuteStruct = TimeData.min;
			pgmDateSecondStruct = TimeData.sec;			
						
			break;

		case 0x01:	
			if(MenuMode)
			{
				EditorSetDataAndRun(WIN_WEIGHT, DEC, pgmDateYearStruct, 0, 99, 0x00, DEC_TYPE_COMMA, 0);
				MenuMode = 0;
			}
			if (MenuWinIsOne) EditorSetConstStr("YAR");			
			break;
				
		case 0x02:
			if(MenuMode)
			{
				EditorSetDataAndRun(WIN_WEIGHT, DEC, pgmDateMonthStruct, 0, 12, 0x00, DEC_TYPE_COMMA, 0);
				MenuMode = 0;
			}
			if (MenuWinIsOne) EditorSetConstStr("MTH");

			break;
			
		case 0x03:			
			if(MenuMode)
			{			
				EditorSetDataAndRun(WIN_WEIGHT, DEC, pgmDateDayStruct, 0, 31, 0x00, DEC_TYPE_COMMA, 0);
				MenuMode = 0;
			}
			if (MenuWinIsOne) EditorSetConstStr("DAY");

			break;

		case 0x04:			
			if(MenuMode)
			{
				EditorSetDataAndRun(WIN_WEIGHT, DEC, pgmDateHourStruct, 0, 23, 0x00, DEC_TYPE_COMMA, 0);
				MenuMode = 0;
			}
			if (MenuWinIsOne) EditorSetConstStr("HOR");

			break;

		case 0x05:			
			if(MenuMode)
			{
				EditorSetDataAndRun(WIN_WEIGHT, DEC, pgmDateMinuteStruct, 0, 59, 0x00, DEC_TYPE_COMMA, 0);
				MenuMode = 0;
			}
			if (MenuWinIsOne) EditorSetConstStr("MIN");

			break;

		case 0x06:			
			if(MenuMode)
			{
				EditorSetDataAndRun(WIN_WEIGHT, DEC, pgmDateSecondStruct, 0, 59, 0x00, DEC_TYPE_COMMA, 0);
				MenuMode = 0;
			}
			if (MenuWinIsOne) EditorSetConstStr("SEC");			
			break; 
	}
	if (key == KEY_SET)
	{
		MenuMode = 1;
		switch(MenuFuncState)
		{
			case 0x01:	EditorGetData(&pgmDateYearStruct);
						TimeData.year = (INT8U)(pgmDateYearStruct);									
				break;
			case 0x02:	EditorGetData(&pgmDateMonthStruct);
						TimeData.month = (INT8U)(pgmDateMonthStruct);
				break;
			case 0x03:	EditorGetData(&pgmDateDayStruct);
						TimeData.date = (INT8U)(pgmDateDayStruct);
						
				break;
			case 0x04:	EditorGetData(&pgmDateHourStruct);
						TimeData.hour = (INT8U)(pgmDateHourStruct);
				break;
			case 0x05: 	EditorGetData(&pgmDateMinuteStruct);
						TimeData.min = (INT8U)(pgmDateMinuteStruct);
				break;	
			case 0x06:	EditorGetData(&pgmDateSecondStruct);
						TimeData.sec =  (INT8U)(pgmDateSecondStruct);
						MenuFuncState++;
				break;
		}
		if (MenuFuncState == 0x07)
		{									
			TimeWriteData();		
			PowerSaverInit();			
			AdInit();
			CommInit();
			return MENU_RET_EXIT;
		}
		else
		{
			MenuFuncState++;
		}
	}

	return MENU_RET_INCOMPLETE;
}
#endif
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
	static INT16U prevBlinkTimer100ms = 0;
	INT8U pgmHeadMessage[24];
	INT16U currTimer100ms;
#ifdef USE_TARE_AUTO_CLEAR_SET
	INT8U tempInt8u = 0;
#endif
#ifdef USE_PRINTER
	if(pgmUserSetMode == PGM_DATE_SET_STATE)
	{
		if(pgmDateEdit == ON)
		{
			TimeReadData();

			pgmDateStruct = (INT32U)((INT32U)TimeData.year * 10000);
			pgmDateStruct += (INT32U)((INT32U)TimeData.month * 100);
			pgmDateStruct += ((INT32U)TimeData.date);
			EditorSetDataAndRun(WIN_WEIGHT, DEC, pgmDateStruct, 0, 999999, 0xff, DEC_TYPE_COMMA, 0);
			pgmUserSetMode = 0xff;
		}
	}

	if(pgmUserSetMode == PGM_TIME_SET_STATE)
	{
		if(pgmTimeEdit == ON)
		{	
			TimeReadData();		

			pgmTimeStruct = (INT32U)((INT32U)TimeData.hour * 10000);
			pgmTimeStruct += (INT32U)((INT32U)TimeData.min * 100);
			pgmTimeStruct += ((INT32U)TimeData.sec);
			EditorSetDataAndRun(WIN_WEIGHT, DEC, pgmTimeStruct, 0, 999999, 0xff, DEC_TYPE_COMMA, 0);
			pgmUserSetMode = 0xff;
		}
	}

	if(pgmUserSetMode == PGM_LABEL_SET_STATE)
	{
		if(pgmLabelEdit == ON)
		{
			ParamReadData(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE1, pgmHeadMessage, 24);
			EditorSetDataAndRun(WIN_WEIGHT, STR, (INT32U)pgmHeadMessage, 0, 24, 0, DEC_TYPE_DOT, 0);
			pgmUserSetMode = 0xff;
		}
	}
#endif
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
			pgmBuzzerOnOffFlag = ParamReadByte(PARAM_BUZZER_AREA_NUM, BUZZER_PARAM_OFFSET_USE);	//ok

#ifdef USE_WEIGHT_ONLY_ECR
			pgmECRType= ParamReadByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_ECR_TYPE);	
#elif defined(USE_PRINTER)
			pgmPrintOffet = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST);
			pgmPrintMethod = pgmPrintOffet >> 6;
			pgmPrinterType = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH) & 0x03;
			pgmPrintBarcode = (pgmPrintOffet & 0x08) >> 3;
			pgmPrintLfSize = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_LF_SIZE);
			pgmBaudRate = ParamReadByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_BAUDRATE);
			pgmDateOnOffFlag = (pgmPrintOffet & 0x20) >> 5;
			pgmTimeOnOffFlag = (pgmPrintOffet & 0x10) >> 4;
			pgmLabelOnOffFlag = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND);
			pgmLabelOnOffFlag	= (pgmLabelOnOffFlag & 0x10) >> 4;
			pgmDateFormFlag = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_DATE_TYPE); // 0-0 - YYMMDD, 1 - DDMMYY, 2 - MMDDYY
#endif
			pgmBlOnOffFlag = ParamReadByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_BL_OFF_TIME);	//ok
			pgmBlBrightness = ParamReadByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BL_BRIGHT);
			pgmAutoOnOffFlag = ParamReadByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_AUTO_OFF_TIME);	//ok
			pgmCompareRange = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_RANGE);
			pgmCountCompareRange = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_COMPARE_RANGE);	
#ifdef USE_SUSPEND_MODE
			pgmSuspendTime = ParamReadByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_STANDBY_TIME);
#endif
			pgmDefaultOnOffFlag = 0;
#ifdef USE_TARE_AUTO_CLEAR_SET
			pgmAutoTareClearFlag = (ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST) & 0x04) >> 2;
#endif
			break;
#ifdef USE_DB2_LED_5DIGIT_FUNC
		case PGM_BUZZER_SET_STATE:
			if (pgmBuzzerOnOffFlag) DispSetStr(MenuDataWinNum, "B  ON");
			else DispSetStr(MenuDataWinNum, "B OFF");

			break;
  #ifdef USE_WEIGHT_ONLY_ECR
		case PGM_ECR_SET_STATE:
			switch(pgmECRType)
			{
				case 0:
					DispSetStr(MenuDataWinNum, "TYP 0");
					break;
				case 2:
					DispSetStr(MenuDataWinNum, "TYP 2");
					break;
				case 4:
					DispSetStr(MenuDataWinNum, "TYP 4");
					break;
				case 5:
					DispSetStr(MenuDataWinNum, "TYP 5");
					break;
				case 6:
					default:
					DispSetStr(MenuDataWinNum, "TYP 6");
					break;
				case 7:
					DispSetStr(MenuDataWinNum, "TYP 7");
					break;
				case 11:
					DispSetStr(MenuDataWinNum, "TYP12");
					break;
			}
			break;
  #else
    #ifdef USE_DLP_DEP_PRINTER
        case PGM_PRT_SELECT_STATE:
			switch(pgmPrinterType)
			{
				case PRT_NONE:
				default:
					DispSetStr(MenuDataWinNum, "P OFF");
					pgmPrinterType = 0;
					break;
				case PRT_DEP50:
					DispSetStr(MenuDataWinNum, "P DEP");	
					break;
				case PRT_DLP50:
					DispSetStr(MenuDataWinNum, "P DLP");	
					break;		
			}
			break;
    #endif
		case PGM_PRT_SET_STATE:
					
			switch(pgmPrintMethod)
			{
				case 0:
				default:
					DispSetStr(MenuDataWinNum, "P OFF");
					pgmPrintMethod = 0;
					break;
				case 1:
					DispSetStr(MenuDataWinNum, "P KEY");
					break;
				case 2:
					DispSetStr(MenuDataWinNum, "P AUT");
					break;					
				case 3:
#ifdef REAL_STREAM						
					DispSetStr(MenuDataWinNum, "R STR");
#elif defined(USE_STREAM_CAS_22BYTE)
					DispSetStr(MenuDataWinNum, "CASTR");
#else
					DispSetStr(MenuDataWinNum, "P STR");
#endif
					break;
			}

			break;

		case PGM_PRT_LF_SET_STATE:
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
			break;
			
		case PGM_BAUDRATE_SET_STATE:
			switch(pgmBaudRate)
			{
				case 0:
				default:
					DispSetStr(MenuDataWinNum, "BR 96");
					pgmBaudRate = 0;
					break;
				case 1:
					DispSetStr(MenuDataWinNum, "BR192");
					break;
				case 2:
					DispSetStr(MenuDataWinNum, "BR384");
					break;
			}
			break;
  #endif	//USE_WEIGHT_ONLY_ECR
		case PGM_AUTOPOWER_SET_STATE:
			switch(pgmAutoOnOffFlag)
			{
				case 0:
				default:
					DispSetStr(MenuDataWinNum, "APOFF");
					pgmAutoOnOffFlag = 0;
					break;
				case 10:
					DispSetStr(MenuDataWinNum, "AP 10");
					break;
				case 30:
					DispSetStr(MenuDataWinNum, "AP 30");
					break;
				case 60:
					DispSetStr(MenuDataWinNum, "AP 60");
					break;
			}
			break;

		case PGM_LABEL_SET_STATE:
			if (pgmLabelOnOffFlag) DispSetStr(MenuDataWinNum, "LA ON");
			else DispSetStr(MenuDataWinNum, "LAOFF");
			break;

		case PGM_DEFAULT_SET_STATE:
			if (pgmDefaultOnOffFlag) DispSetStr(MenuDataWinNum, "DF ON");
			else DispSetStr(MenuDataWinNum, "DFOFF");
			break;

		case PGM_DAY_SET_STATE:
			currTimer100ms = TimerGetSysTimer100ms();
			if ((INT16U)(currTimer100ms - prevBlinkTimer100ms) < 8)
			{
				switch (TimeData.day)
				{
					case 1:
						default:
						DispSetStr(MenuDataWinNum, " MON");
						break;
					case 2:					
						DispSetStr(MenuDataWinNum, " TUE");
						break;
					case 3:					
						DispSetStr(MenuDataWinNum, " WED");
						break;
					case 4:
						DispSetStr(MenuDataWinNum, " THU");
						break;
					case 5:
						DispSetStr(MenuDataWinNum, " FRI");
						break;
					case 6:
						DispSetStr(MenuDataWinNum, " SAT");
						break;
					case 7:
						DispSetStr(MenuDataWinNum, " SUN");
						break;				
				}
			}
			else if ((currTimer100ms - prevBlinkTimer100ms) < 10)
			{
				DispSetStr(MenuDataWinNum, " ");
				break;
			}
			else
			{
				prevBlinkTimer100ms = currTimer100ms;	
				break;
			}
 			break;

			case PGM_DATE_FORM_SET_STATE:
				switch (pgmDateFormFlag)
				{
					case 0x00:
					default:
						DispSetStr(MenuDataWinNum, "Y M D");
						break;
					case 0x01:
						DispSetStr(MenuDataWinNum, "D M Y");
						break;
					case 0x02:
						DispSetStr(MenuDataWinNum, "M D Y");
						break;
				}
					
#ifdef USE_SUSPEND_MODE			
		case PGM_SUSPEND_SET_STATE:
			switch (pgmSuspendTime)
			{
				case 0:
				default:
					DispSetStr(MenuDataWinNum, "SUS00");
					break;
				case 20:
					DispSetStr(MenuDataWinNum, "SUS20");
					break;
				case 40:
					DispSetStr(MenuDataWinNum, "SUS40");
					break;
				case 60:
					DispSetStr(MenuDataWinNum, "SUS60");
					break;
			}
			break;
#endif
		default:
			break;
	}



#else		 //****** IF 6 DIGIT USE******

		case PGM_BUZZER_SET_STATE:
			if (pgmBuzzerOnOffFlag) DispSetStr(MenuDataWinNum, "B  ON");
			else DispSetStr(MenuDataWinNum, "B  OFF");
			break;
  #ifdef USE_WEIGHT_ONLY_ECR
		case PGM_ECR_SET_STATE:
			switch(pgmECRType)
			{
				case 0:
					DispSetStr(MenuDataWinNum, "TYPE 0");
					break;
				case 2:
					DispSetStr(MenuDataWinNum, "TYPE 2");
					break;
				case 4:
					DispSetStr(MenuDataWinNum, "TYPE 4");
					break;
				case 5:
					DispSetStr(MenuDataWinNum, "TYPE 5");
					break;
				case 6:
					default:
					DispSetStr(MenuDataWinNum, "TYPE 6");
					break;
				case 7:
					DispSetStr(MenuDataWinNum, "TYPE 7");
					break;
				case 11:
					DispSetStr(MenuDataWinNum, "TYPE12");
					break;
			}
			break;
  #elif defined(USE_PRINTER)
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
    #ifdef REAL_STREAM						
						DispSetStr(MenuDataWinNum, "RE STR");
    #elif defined(USE_STREAM_CAS_22BYTE)
						DispSetStr(MenuDataWinNum, "CASSTR");
    #else
						DispSetStr(MenuDataWinNum, "PR STR");
    #endif	//REAL_STREAM
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
				} //
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
				//break;
			}
			else
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
    #else
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
			}
			break;
    #endif	//USE_6_BAUDRATE
  #endif	//USE_WEIGHT_ONLY_ECR
		case PGM_BACKLIGHT_SET_STATE:
			switch(pgmBlOnOffFlag)
			{
				case 0:
				default:
					DispSetStr(MenuDataWinNum, "BL OFF");
					pgmBlOnOffFlag = 0;
					break;
				case 1:
					DispSetStr(MenuDataWinNum, "BL ON");
					break;
				case 10:
					DispSetStr(MenuDataWinNum, "BL 10");
					break;
				case 30:
					DispSetStr(MenuDataWinNum, "BL 30");
					break;
			}
			break;

		case PGM_BRIGHT_SET_STATE:
			switch (pgmBlBrightness)
			{				
				case 1:
					DispSetStr(MenuDataWinNum, "BRIG 1");
					break;
				case 2:
					DispSetStr(MenuDataWinNum, "BRIG 2");
					break;
				case 3:
				default:
					DispSetStr(MenuDataWinNum, "BRIG 3");
					pgmBlBrightness = 3;
					break;
				case 4:
					DispSetStr(MenuDataWinNum, "BRIG 4");
					break;
				case 5:
					DispSetStr(MenuDataWinNum, "BRIG 5");
					break;
				case 6:
					DispSetStr(MenuDataWinNum, "BRIG 6");
					break;
				case 7:
					DispSetStr(MenuDataWinNum, "BRIG 7");
					break;
			}
			break;

		case PGM_AUTOPOWER_SET_STATE:
			switch(pgmAutoOnOffFlag)
			{
				case 0:
				default:
					DispSetStr(MenuDataWinNum, "AP OFF");
					pgmAutoOnOffFlag = 0;
					break;
				case 10:
					DispSetStr(MenuDataWinNum, "AP 10");
					break;
				case 30:
					DispSetStr(MenuDataWinNum, "AP 30");
					break;
				case 60:
					DispSetStr(MenuDataWinNum, "AP 60");
					break;
			}
			break;

		case PGM_COMPARE_SET_STATE:
			switch(pgmCompareRange)
			{
				case 0:
				default:
					DispSetStr(MenuDataWinNum, "WL M0");
					pgmCompareRange = 0;
					break;
				case 1:
					DispSetStr(MenuDataWinNum, "WL M1");
					break;
				case 2:
					DispSetStr(MenuDataWinNum, "WL M2");
					break;
				case 3:
					DispSetStr(MenuDataWinNum, "WL M3");
					break;
			}
			break;
			
		case PGM_COUNT_COMPARE_SET_STATE:
			switch(pgmCountCompareRange)
			{
				case 0:
					default:
					DispSetStr(MenuDataWinNum, "CL M0");
					pgmCountCompareRange = 0;
					break;
				case 1:
					DispSetStr(MenuDataWinNum, "CL M1");
					break;
				case 2:
					DispSetStr(MenuDataWinNum, "CL M2");
					break;
				case 3:
					DispSetStr(MenuDataWinNum, "CL M3");
					break;
			}
			break;
#ifdef USE_PRINTER
		case PGM_DATE_SET_STATE:
			if (pgmDateOnOffFlag) DispSetStr(MenuDataWinNum, "DY ON");
			else DispSetStr(MenuDataWinNum, "DY OFF");
			break;

		case PGM_DAY_SET_STATE:
			currTimer100ms = TimerGetSysTimer100ms();
			if ((INT16U)(currTimer100ms - prevBlinkTimer100ms) < 8)
			{
				switch (TimeData.day)
				{
					case 1:
						default:
						DispSetStr(MenuDataWinNum, " MON");
						break;
					case 2:					
						DispSetStr(MenuDataWinNum, " TUE");
						break;
					case 3:					
						DispSetStr(MenuDataWinNum, " WED");
						break;
					case 4:
						DispSetStr(MenuDataWinNum, " THU");
						break;
					case 5:
						DispSetStr(MenuDataWinNum, " FRI");
						break;
					case 6:
						DispSetStr(MenuDataWinNum, " SAT");
						break;
					case 7:
						DispSetStr(MenuDataWinNum, " SUN");
						break;				
				}
			}
			else if ((currTimer100ms - prevBlinkTimer100ms) < 10)
			{
				DispSetStr(MenuDataWinNum, " ");
				break;
			}
			else
			{
				prevBlinkTimer100ms = currTimer100ms;	
				break;
			}
 			break;
		
		case PGM_DATE_FORM_SET_STATE:			
			switch (pgmDateFormFlag)
			{
				case 0x00:
				default:
					DispSetStr(MenuDataWinNum, "YYMMDD");
					break;
				case 0x01:
					DispSetStr(MenuDataWinNum, "DDMMYY");
					break;
				case 0x02:
					DispSetStr(MenuDataWinNum, "MMDDYY");
					break;
			}
		break;

		case PGM_TIME_SET_STATE:
			if (pgmTimeOnOffFlag) DispSetStr(MenuDataWinNum, "TI ON");
			else DispSetStr(MenuDataWinNum, "TI OFF");
			break;

		case PGM_LABEL_SET_STATE:
			if (pgmLabelOnOffFlag) DispSetStr(MenuDataWinNum, "LA ON");
			else DispSetStr(MenuDataWinNum, "LA OFF");
			break;
#endif
		case PGM_DEFAULT_SET_STATE:
			if (pgmDefaultOnOffFlag) DispSetStr(MenuDataWinNum, "DF ON");
			else DispSetStr(MenuDataWinNum, "DF OFF");
			break;

#ifdef USE_SUSPEND_MODE			
		case PGM_SUSPEND_SET_STATE:
			switch (pgmSuspendTime)
			{
				case 0:
				default:
					DispSetStr(MenuDataWinNum, "SUS 00");
					break;
				case 20:
					DispSetStr(MenuDataWinNum, "SUS 20");
					break;
				case 40:
					DispSetStr(MenuDataWinNum, "SUS 40");
					break;
				case 60:
					DispSetStr(MenuDataWinNum, "SUS 60");
					break;
			}
			break;
#endif
#ifdef USE_TARE_AUTO_CLEAR_SET			
		case PGM_TARE_AUTO_CLEAR_SET_STATE:
			switch (pgmAutoTareClearFlag)
			{				
				case 0:
				default:
					DispSetStr(MenuDataWinNum, "TC OFF");
					break;
				case 1:
					DispSetStr(MenuDataWinNum, "TC ON ");
					break;
			}
			break;
#endif		

		default:
			break;
	}
#endif

#ifdef  USE_DISP_BRIGHT
#ifdef	USE_DB2_LED_5DIGIT_FUNC
	if (key == KEY_MODE)
	{
		if (pgmUserSetMode == 0xff) return MENU_RET_INCOMPLETE;
		
		if (pgmUserSetMode != PGM_DEFAULT_SET_STATE)
		{
  #ifdef USE_DLP_DEP_PRINTER
			if(pgmUserSetMode == PGM_BUZZER_SET_STATE)	pgmUserSetMode += 1;
  #else
			if(pgmUserSetMode == PGM_BUZZER_SET_STATE)	pgmUserSetMode += 2;
  #endif
  #ifdef USE_WEIGHT_ONLY_ECR
			if(pgmUserSetMode == PGM_ECR_SET_STATE) pgmUserSetMode += 6;
  #else
			else if(pgmUserSetMode == PGM_BAUDRATE_SET_STATE) 	pgmUserSetMode += 3;
  #endif
			else if(pgmUserSetMode == PGM_AUTOPOWER_SET_STATE) 	pgmUserSetMode += 5;
			else pgmUserSetMode += 1;
		}
		else 
		{
			pgmUserSetMode = PGM_BUZZER_SET_STATE;
		}
	}
#endif
#else
	if (key == KEY_MODE)
	{
		if (pgmUserSetMode == 0xff) return MENU_RET_INCOMPLETE;
		
		if (pgmUserSetMode != PGM_DEFAULT_SET_STATE)
		{
#ifdef USE_BL_MENU
  #ifdef USE_WEIGHT_ONLY_ECR
			if(pgmUserSetMode == PGM_ECR_SET_STATE) pgmUserSetMode += 4;
			else if(pgmUserSetMode == PGM_COUNT_COMPARE_SET_STATE) pgmUserSetMode += 4;
  #elif defined(USE_PRINTER)
			if(pgmUserSetMode == PGM_PRT_SELECT_STATE)
			{
				if(pgmPrinterType == PRT_NONE) pgmUserSetMode += 3;
				else pgmUserSetMode += 1;
			}
            else if(pgmUserSetMode == PGM_DATE_FORM_SET_STATE)
			{
				pgmUserSetMode = PGM_DATE_SET_STATE;
			}
  #endif
			else pgmUserSetMode += 1;
#else			
  #ifdef USE_WEIGHT_ONLY_ECR
			if(pgmUserSetMode == PGM_ECR_SET_STATE) pgmUserSetMode += 6;
			else if(pgmUserSetMode == PGM_COUNT_COMPARE_SET_STATE) pgmUserSetMode += 4;
  #elif defined(USE_PRINTER)
			if(pgmUserSetMode == PGM_BAUDRATE_SET_STATE) pgmUserSetMode += 3;
  #endif
			else pgmUserSetMode += 1;

#endif
		}
		else 
		{
			pgmUserSetMode = PGM_BUZZER_SET_STATE;
		}
	}
#endif
	
	if (key == KEY_PRINT)
	{
		switch(pgmUserSetMode)
		{
			case PGM_BUZZER_SET_STATE:				
				if (pgmBuzzerOnOffFlag)
				{
					pgmBuzzerOnOffFlag = OFF;					
				}
				else 
				{
					pgmBuzzerOnOffFlag = ON;
				}
				BuzzerSwitch(pgmBuzzerOnOffFlag);
				break;
#ifdef USE_PRINTER
			case PGM_DATE_SET_STATE:				
				if (pgmDateOnOffFlag)
				{
					pgmDateOnOffFlag = OFF;
				}
				else 
				{
					pgmDateOnOffFlag = ON;
				}
				break;
			case PGM_TIME_SET_STATE:				
				if (pgmTimeOnOffFlag)
				{
					pgmTimeOnOffFlag = OFF;
				}
				else 
				{
					pgmTimeOnOffFlag = ON;
				}
				break;
			case PGM_LABEL_SET_STATE:				
				if (pgmLabelOnOffFlag)
				{
					pgmLabelOnOffFlag = OFF;
				}
				else 
				{
					pgmLabelOnOffFlag = ON;
				}
				break;
#endif
			case PGM_DEFAULT_SET_STATE:
				if (pgmDefaultOnOffFlag)
				{
					pgmDefaultOnOffFlag = OFF;
				}
				else 
				{
					pgmDefaultOnOffFlag = ON;
				}
				break;	
#ifdef USE_PRINTER
			case PGM_PRT_SELECT_STATE:
				if(pgmPrinterType != 0)
				{
					pgmPrinterType -= 1;
				}
				else 
				{
					pgmPrinterType = 2;
				}
				break;	

			case PGM_PRT_SET_STATE:				
				if(pgmPrinterType != PRT_NONE)
				{
					if (pgmPrintMethod != 1)
					{
						pgmPrintMethod -= 1;
					}
					else 
					{
						if(pgmPrinterType == PRT_DEP50)
						{
							pgmPrintMethod = 3;
						}
						else if(pgmPrinterType == PRT_DLP50)
						{
							pgmPrintMethod = 2;
						}
					}
				}	
				break;
			case PGM_PRT_LF_SET_STATE: 
				if(pgmPrinterType != PRT_NONE)
				{
					if(pgmPrinterType == PRT_DEP50)
					{
						if (pgmPrintLfSize != 1)
						{
							pgmPrintLfSize -= 1;
						}
						else 
						{
							pgmPrintLfSize = 9;
						}
					}
					else
					{
						if(pgmPrintBarcode) pgmPrintBarcode = 0;
						else pgmPrintBarcode = 1;
					}
				}	
					break;	
#endif
			case PGM_COMPARE_SET_STATE:
				if (pgmCompareRange != 0)
				{
					pgmCompareRange -= 1;
				}
				else 
				{
					pgmCompareRange = 3;
				}
				break;
			case PGM_COUNT_COMPARE_SET_STATE:
				if (pgmCountCompareRange != 0)
				{
					pgmCountCompareRange -= 1;
				}
				else 
				{
					pgmCountCompareRange = 3;
				}
				break;
#ifdef USE_PRINTER
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
  #else
		case PGM_BAUDRATE_SET_STATE:
				if (pgmBaudRate != 0)
				{
					pgmBaudRate -= 1;
				}
				else
				{
					pgmBaudRate = 2;
				}
				CommDrvInit(pgmBaudRate);
				break;
  #endif
			case PGM_DAY_SET_STATE:
				if (TimeData.day != 1)
				{
					TimeData.day -= 1;
				}
				else
				{
					TimeData.day = 7;
				}
				break;				
#endif
			case PGM_BRIGHT_SET_STATE:				
				if (pgmBlBrightness != 1)
				{
					pgmBlBrightness -= 1;
				}
				else
				{
					pgmBlBrightness = 7;
				}								
				DispBlSetBright(pgmBlBrightness);
				break;		
			case PGM_BACKLIGHT_SET_STATE:
				if (pgmBlOnOffFlag == 0)
				{
					pgmBlOnOffFlag = 30;
				}
				else if (pgmBlOnOffFlag == 1)
				{
					pgmBlOnOffFlag = 0;
				}
				else if (pgmBlOnOffFlag == 10)
				{
					pgmBlOnOffFlag = 1;
				}
				else if (pgmBlOnOffFlag == 30)
				{
					pgmBlOnOffFlag = 10;
				}
				else pgmBlOnOffFlag = 0;
				PowerSaverSetBlOffTime1s(pgmBlOnOffFlag);
				break;
			case PGM_AUTOPOWER_SET_STATE:
				if (pgmAutoOnOffFlag == 0)
				{
					pgmAutoOnOffFlag = 60;
				}
				else if (pgmAutoOnOffFlag == 10)
				{
					pgmAutoOnOffFlag = 0;
				}
				else if (pgmAutoOnOffFlag == 30)
				{
					pgmAutoOnOffFlag = 10;
				}
				else if (pgmAutoOnOffFlag == 60)
				{
					pgmAutoOnOffFlag = 30;
				}
				else pgmAutoOnOffFlag = 0; 
				break;				
#ifdef USE_PRINTER
			case PGM_DATE_FORM_SET_STATE:
				if(pgmDateFormFlag == 0)
				{
					pgmDateFormFlag = 2;
				}
				else if(pgmDateFormFlag == 1)
				{
					pgmDateFormFlag = 0;
				}
				else if (pgmDateFormFlag == 2)
				{
					pgmDateFormFlag = 1;
				}
				else pgmDateFormFlag = 0;
				break;
#endif
#ifdef USE_WEIGHT_ONLY_ECR
			case PGM_ECR_SET_STATE:
				if (pgmECRType == 11)
				{
					pgmECRType = 7;
				}
				else if (pgmECRType == 7)
				{
					pgmECRType = 6;
				}
				else if (pgmECRType == 6)
				{
					pgmECRType = 5;
				}
				else if (pgmECRType == 5)
				{
					pgmECRType = 4;
				}
				else if (pgmECRType == 4) 
				{
					pgmECRType = 2;
				}
				else if (pgmECRType == 2) 
				{
					pgmECRType = 0;
				}
				else 
				{
					pgmECRType = 11;
				}
				break;
#endif
#ifdef USE_SUSPEND_MODE				
			case PGM_SUSPEND_SET_STATE:
				if (pgmSuspendTime != 0)
				{
					pgmSuspendTime -= 20;
				}
				else
				{
					pgmSuspendTime = 60;
				}
				break;
#endif
#ifdef USE_TARE_AUTO_CLEAR_SET
			case PGM_TARE_AUTO_CLEAR_SET_STATE: 
				if (pgmAutoTareClearFlag == 0)
				{
					pgmAutoTareClearFlag = 1;
				}
				else 
				{
					pgmAutoTareClearFlag = 0;
				}
				break;	
#endif
			default:
				break;
		}
	}
	
	if (key == KEY_HOLD)
	{
		switch(pgmUserSetMode)
		{
			case PGM_BUZZER_SET_STATE:
				if (pgmBuzzerOnOffFlag)
				{
					pgmBuzzerOnOffFlag = OFF;
				}
				else 
				{
					pgmBuzzerOnOffFlag = ON;
				}
				BuzzerSwitch(pgmBuzzerOnOffFlag);
				break;
#ifdef USE_PRINTER
			case PGM_DATE_SET_STATE:
				if (pgmDateOnOffFlag)
				{
					pgmDateOnOffFlag = OFF;
				}
				else 
				{
					pgmDateOnOffFlag = ON;
				}
				break;
			case PGM_TIME_SET_STATE:
				if (pgmTimeOnOffFlag)
				{
					pgmTimeOnOffFlag = OFF;
				}
				else 
				{
					pgmTimeOnOffFlag = ON;
				}
				break;
			case PGM_LABEL_SET_STATE:
				if (pgmLabelOnOffFlag)
				{
					pgmLabelOnOffFlag = OFF;
				}
				else 
				{
					pgmLabelOnOffFlag = ON;
				}
				break;
#endif
			case PGM_DEFAULT_SET_STATE:
				if (pgmDefaultOnOffFlag)
				{
					pgmDefaultOnOffFlag = OFF;
				}
				else 
				{
					pgmDefaultOnOffFlag = ON;
				}
				break;				
#ifdef USE_PRINTER
			case PGM_PRT_SELECT_STATE:
				if(pgmPrinterType != 2)
				{
					pgmPrinterType += 1;
				}
				else
				{
					pgmPrinterType = 0;
				}
				break;	
				
			case PGM_PRT_SET_STATE:				
				if(pgmPrinterType == PRT_DEP50)
				{
					if (pgmPrintMethod != 3)
					{
						pgmPrintMethod += 1;
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
						pgmPrintMethod += 1;
					}
					else 
					{
						pgmPrintMethod = 1;
					}					
				}
				break;			
			case PGM_PRT_LF_SET_STATE:
				if(pgmPrinterType != PRT_NONE)
				{
					if(pgmPrinterType == PRT_DEP50)
					{
						if (pgmPrintLfSize != 9)
						{
							pgmPrintLfSize += 1;
						}
						else 
						{
							pgmPrintLfSize = 1;
						}
					}
					else
					{
						if(pgmPrintBarcode) pgmPrintBarcode = 0;
						else pgmPrintBarcode =1;
					}
				}	
				break;			
#endif	//USE_PRINTER
			case PGM_COMPARE_SET_STATE:
				if (pgmCompareRange != 3)
				{
					pgmCompareRange += 1;
				}
				else 
				{
					pgmCompareRange = 0;
				}
				break;			
			case PGM_COUNT_COMPARE_SET_STATE:
				if (pgmCountCompareRange != 3)
				{
					pgmCountCompareRange += 1;
				}
				else 
				{
					pgmCountCompareRange = 0;
				}
				break;
#ifdef USE_PRINTER
  #ifdef	USE_6_BAUDRATE 
			case PGM_BAUDRATE_SET_STATE:
				if (pgmBaudRate != 5)
				{
					pgmBaudRate += 1;					
				}
				else
				{
					pgmBaudRate = 0;
				}
				CommDrvInit(pgmBaudRate);
				break;
  #else
			case PGM_BAUDRATE_SET_STATE:
				if (pgmBaudRate != 2)
				{
					pgmBaudRate += 1;					
				}
				else
				{
					pgmBaudRate = 0;
				}
				CommDrvInit(pgmBaudRate);
				break;
  #endif
			case PGM_DAY_SET_STATE:
				if (TimeData.day != 7)
				{
					TimeData.day += 1;
				}
				else
				{
					TimeData.day = 1;
				}
				break;
#endif	//USE_WEIGHT_ONLY_ECR
			case PGM_BRIGHT_SET_STATE:				
				if (pgmBlBrightness != 7)
				{
					pgmBlBrightness += 1;
				}
				else
				{
					pgmBlBrightness = 1;
				}
				DispBlSetBright(pgmBlBrightness);
				break;				
			case PGM_BACKLIGHT_SET_STATE:
				if (pgmBlOnOffFlag == 0)
				{
					pgmBlOnOffFlag = 1;
				}
				else if (pgmBlOnOffFlag == 1)
				{
					pgmBlOnOffFlag = 10;
				}
				else if (pgmBlOnOffFlag == 10)
				{
					pgmBlOnOffFlag = 30;
				}
				else if (pgmBlOnOffFlag == 30)
				{
					pgmBlOnOffFlag = 0;
				}
				else pgmBlOnOffFlag = 0;				
				PowerSaverSetBlOffTime1s(pgmBlOnOffFlag);
				break;
			case PGM_AUTOPOWER_SET_STATE:
				if (pgmAutoOnOffFlag == 0)
				{
					pgmAutoOnOffFlag = 10;
				}
				else if (pgmAutoOnOffFlag == 10)
				{
					pgmAutoOnOffFlag = 30;
				}
				else if (pgmAutoOnOffFlag == 30)
				{
					pgmAutoOnOffFlag = 60;
				}
				else if (pgmAutoOnOffFlag == 60)
				{
					pgmAutoOnOffFlag = 0;
				}
				else pgmAutoOnOffFlag = 0; 
				break;			
#ifdef USE_PRINTER
				case PGM_DATE_FORM_SET_STATE:
				if(pgmDateFormFlag == 0)
				{
					pgmDateFormFlag = 1;
				}
				else if(pgmDateFormFlag == 1)
				{
					pgmDateFormFlag = 2;
				}
				else if (pgmDateFormFlag == 2)
				{
					pgmDateFormFlag = 0;
				}
				else pgmDateFormFlag = 0;
				break;
#endif
#ifdef USE_WEIGHT_ONLY_ECR
			case PGM_ECR_SET_STATE:
				if (pgmECRType == 0)
				{
					pgmECRType = 2;
				}
				else if (pgmECRType == 2)
				{
					pgmECRType = 4;
				}
				else if (pgmECRType == 4)
				{
					pgmECRType = 5;
				}
				else if (pgmECRType == 5)
				{
					pgmECRType = 6;
				}
				else if (pgmECRType == 6) 
				{
					pgmECRType = 7;
				}
				else if (pgmECRType == 7) 
				{
					pgmECRType = 11;
				}
				else 
				{
					pgmECRType = 0;
				}
				break;
#endif
#ifdef USE_SUSPEND_MODE
			case PGM_SUSPEND_SET_STATE:
				if (pgmSuspendTime != 60)
				{
					pgmSuspendTime += 20;
				}
				else
				{
					pgmSuspendTime = 0;
				}
				break;
#endif
#ifdef USE_TARE_AUTO_CLEAR_SET
			case PGM_TARE_AUTO_CLEAR_SET_STATE: 
				if (pgmAutoTareClearFlag == 0)
				{
					pgmAutoTareClearFlag = 1;
				}
				else 
				{
					pgmAutoTareClearFlag = 0;
				}
				break;	
#endif
			default:
				break;
		}
	}
#ifdef USE_PRINTER
	if(key == KEY_SAMPLE)
	{
		switch(pgmUserSetMode)
		{
			case PGM_DATE_SET_STATE:
				if(pgmDateOnOffFlag)
				{
					pgmDateEdit = ON;
				}
				else break;
			case PGM_TIME_SET_STATE:
				if(pgmTimeOnOffFlag)
				{
					pgmTimeEdit = ON;
				}
				else break;
			case PGM_LABEL_SET_STATE:
				if(pgmLabelOnOffFlag)
				{
					pgmLabelEdit = ON;
				}
				else break;
			default:
				break;
		}		
	}
	if(key == KEY_ZERO)
	{
		if(pgmLabelEdit == ON && pgmUserSetMode == 0xff)
		{
			pgmTimeEdit = OFF;
			pgmDateEdit = OFF;
			pgmLabelEdit = OFF;			
			pgmUserSetMode = PGM_LABEL_SET_STATE;
			return MENU_RET_INCOMPLETE;
		}
		else
		{
			return MENU_RET_EXIT;
		}
	}
#endif
#ifdef USE_DB2_LED_5DIGIT_FUNC // SET KEY, To Save configuration
	if (key == KEY_SET)
	{	
		if(pgmDateEdit == ON)
		{		
			TimeReadData(); 
			EditorGetData(&pgmDateStruct);
			pgmDateEdit = OFF;
			pgmTimeEdit = OFF;
			pgmLabelEdit = OFF;
			pgmUserSetMode = PGM_DAY_SET_STATE;
			
			TimeData.year = (INT8U)(pgmDateStruct / 10000);
			pgmDateStruct %= 10000;
			TimeData.month = (INT8U)(pgmDateStruct / 100);
			pgmDateStruct %= 100;
			TimeData.date = (INT8U)pgmDateStruct;
			
			if (TimeData.year > 99 || TimeData.year < 0 
				|| TimeData.month > 12 || TimeData.month < 0
				|| TimeData.date > 31 || TimeData.date < 0)
			{				
				pgmDateEdit = ON;
				pgmUserSetMode = PGM_DATE_SET_STATE;
				DispSetConstStrMsg(WIN_WEIGHT, "DYERR");
				DispSetRunMsgPage(10, 1);
				return MENU_RET_INCOMPLETE;
			}

			TimeWriteData();
			DispSetConstStrMsg(WIN_WEIGHT, "DYSAV");
			DispSetRunMsgPage(10, 1);
			return MENU_RET_INCOMPLETE;
		}
		if(pgmTimeEdit == ON)
		{		
			TimeReadData(); 
			EditorGetData(&pgmTimeStruct);
			pgmDateEdit = OFF;
			pgmTimeEdit = OFF;
			pgmLabelEdit = OFF;
			pgmUserSetMode = PGM_TIME_SET_STATE;
			TimeData.hour = (INT8U)(pgmTimeStruct / 10000);
			pgmTimeStruct %= 10000;
			TimeData.min = (INT8U)(pgmTimeStruct / 100);
			pgmTimeStruct %= 100;
			TimeData.sec = (INT8U)pgmTimeStruct;
			if (TimeData.hour > 23 || TimeData.hour < 0 
				|| TimeData.min > 59 || TimeData.min < 0 
				|| TimeData.sec > 59 || TimeData.sec < 0)
			{
				pgmTimeEdit = ON;
				DispSetConstStrMsg(WIN_WEIGHT, "TIERR");
				DispSetRunMsgPage(10, 1);
				return MENU_RET_INCOMPLETE; 
			}
			TimeWriteData();
			DispSetConstStrMsg(WIN_WEIGHT, "TISAV");
			DispSetRunMsgPage(10, 1);
			return MENU_RET_INCOMPLETE;
		}		
		if(pgmLabelEdit == ON)
		{		
			EditorGetData((INT32U *)pgmHeadMessage);
			pgmDateEdit = OFF;
			pgmTimeEdit = OFF;
			pgmLabelEdit = OFF; 		
			pgmUserSetMode = PGM_LABEL_SET_STATE;
			ParamWriteData(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE1, pgmHeadMessage, 24);
			DispSetConstStrMsg(WIN_WEIGHT, "LASAV");
			DispSetRunMsgPage(10, 1);
			return MENU_RET_INCOMPLETE;
		}
		if(pgmUserSetMode == PGM_DAY_SET_STATE)
		{
			pgmUserSetMode = PGM_DATE_FORM_SET_STATE;
			TimeWriteData();			
			DispSetConstStrMsg(WIN_WEIGHT, "WKSAV");
			DispSetRunMsgPage(10, 1);
			return MENU_RET_INCOMPLETE;
		}
		DispSetConstStrMsg(WIN_WEIGHT, " SAVE");
		DispSetRunMsgPage(10, 1);
#else
	if (key == KEY_SET)
	{	
#ifdef USE_PRINTER
		if(pgmDateEdit == ON)
		{		
			TimeReadData();	
			EditorGetData(&pgmDateStruct);
			pgmDateEdit = OFF;
			pgmTimeEdit = OFF;
			pgmLabelEdit = OFF;
			pgmUserSetMode = PGM_DAY_SET_STATE;
			TimeData.year = (INT8U)(pgmDateStruct / 10000);
			pgmDateStruct %= 10000;
			TimeData.month = (INT8U)(pgmDateStruct / 100);
			pgmDateStruct %= 100;
			TimeData.date = (INT8U)pgmDateStruct;
			if (TimeData.year > 99 || TimeData.year < 0 
				|| TimeData.month > 12 || TimeData.month < 0
				|| TimeData.date > 31 || TimeData.date < 0)
			{				
				pgmDateEdit = ON;
				pgmUserSetMode = PGM_DATE_SET_STATE;
				DispSetConstStrMsg(WIN_WEIGHT, "DY ERR");
				DispSetRunMsgPage(10, 1);
				return MENU_RET_INCOMPLETE;
			}
			TimeWriteData();
			DispSetConstStrMsg(WIN_WEIGHT, "DYSAVE");
			DispSetRunMsgPage(10, 1);
			return MENU_RET_INCOMPLETE;
		}
		if(pgmTimeEdit == ON)
		{		
			TimeReadData();	
			EditorGetData(&pgmTimeStruct);
			pgmDateEdit = OFF;
			pgmTimeEdit = OFF;
			pgmLabelEdit = OFF;
			pgmUserSetMode = PGM_TIME_SET_STATE;
			TimeData.hour = (INT8U)(pgmTimeStruct / 10000);
			pgmTimeStruct %= 10000;
			TimeData.min = (INT8U)(pgmTimeStruct / 100);
			pgmTimeStruct %= 100;
			TimeData.sec = (INT8U)pgmTimeStruct;
			if (TimeData.hour > 23 || TimeData.hour < 0 
				|| TimeData.min > 59 || TimeData.min < 0 
				|| TimeData.sec > 59 || TimeData.sec < 0)
			{
				pgmTimeEdit = ON;
				DispSetConstStrMsg(WIN_WEIGHT, "TI ERR");
				DispSetRunMsgPage(10, 1);
				return MENU_RET_INCOMPLETE;	
			}
			TimeWriteData();
			DispSetConstStrMsg(WIN_WEIGHT, "TISAVE");
			DispSetRunMsgPage(10, 1);
			return MENU_RET_INCOMPLETE;
		}		
		if(pgmLabelEdit == ON)
		{		
			EditorGetData((INT32U *)pgmHeadMessage);
			pgmDateEdit = OFF;
			pgmTimeEdit = OFF;
			pgmLabelEdit = OFF;			
			pgmUserSetMode = PGM_LABEL_SET_STATE;
			ParamWriteData(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE1, pgmHeadMessage, 24);
			DispSetConstStrMsg(WIN_WEIGHT, "LASAVE");
			DispSetRunMsgPage(10, 1);
			return MENU_RET_INCOMPLETE;
		}
		if(pgmUserSetMode == PGM_DAY_SET_STATE)
		{
			pgmUserSetMode = PGM_DATE_FORM_SET_STATE;
			TimeWriteData();			
			DispSetConstStrMsg(WIN_WEIGHT, "WKSAVE");
			DispSetRunMsgPage(10, 1);
			return MENU_RET_INCOMPLETE;
		}
		
		if(pgmUserSetMode == PGM_DATE_FORM_SET_STATE)
		{
			pgmUserSetMode = PGM_DATE_SET_STATE;
			DispSetConstStrMsg(WIN_WEIGHT, "DTSAVE");
			DispSetRunMsgPage(10, 1);
			return MENU_RET_INCOMPLETE;
		}
#endif
		DispSetConstStrMsg(WIN_WEIGHT, " SAVE");
		DispSetRunMsgPage(10, 1);
#endif
#ifdef USE_PRINTER
		pgmPrintOffet = 0;
		pgmPrintOffet |= (pgmPrintMethod << 6);
		pgmPrintOffet |= (pgmDateOnOffFlag << 5);
		pgmPrintOffet |= (pgmTimeOnOffFlag << 4);
		pgmPrintOffet |= (pgmPrintBarcode << 3);
		pgmPrinterType |= (ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH) & 0xFC);
		pgmLabelOnOffFlag = pgmLabelOnOffFlag << 4;
		pgmLabelOnOffFlag  |= (ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND) & 0xEF);
#endif	//USE_WEIGHT_ONLY_ECR
#ifdef USE_TARE_AUTO_CLEAR_SET
	   tempInt8u = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST); 
	   if(pgmAutoTareClearFlag == 1)
	   {
		   tempInt8u |= 0x04;
		   ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST, tempInt8u);
	   }
	   else if(pgmAutoTareClearFlag == 0)
	   {
		   tempInt8u &= ~0x04;
		   ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST, tempInt8u);
	   }
#endif	//USE_TARE_AUTO_CLEAR_SET

		if(pgmDefaultOnOffFlag)	
		{			
			pgmBuzzerOnOffFlag = ON;
#ifdef USE_WEIGHT_ONLY_ECR
			pgmECRType = 6;
#elif defined(USE_PRINTER)
			pgmLabelOnOffFlag |= 0x10;
			pgmPrintOffet = 0x78;
			pgmPrintLfSize = 1;
			pgmBaudRate = 0x00;
#endif
			pgmBlOnOffFlag = OFF;
			pgmBlBrightness = 0;
			pgmAutoOnOffFlag = OFF;
			pgmCompareRange = 0x00;
			pgmCountCompareRange = 0x00;
#ifdef USE_SUSPEND_MODE
			pgmSuspendTime = 0;
#endif			
			ParamWriteByte(PARAM_BUZZER_AREA_NUM, BUZZER_PARAM_OFFSET_USE, pgmBuzzerOnOffFlag);
#ifdef USE_WEIGHT_ONLY_ECR
			ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_ECR_TYPE, pgmECRType);
#elif defined(USE_PRINTER)
			ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST, pgmPrintOffet);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, pgmPrinterType);
			ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_LF_SIZE, pgmPrintLfSize);
			ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_BAUDRATE, pgmBaudRate);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_DATE_TYPE, pgmDateFormFlag);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, pgmLabelOnOffFlag);
#endif
			ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_BL_OFF_TIME, pgmBlOnOffFlag);
			ParamWriteByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BL_BRIGHT, pgmBlBrightness);
			ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_AUTO_OFF_TIME, pgmAutoOnOffFlag);			
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_RANGE, pgmCompareRange);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_COMPARE_RANGE, pgmCountCompareRange);	
#ifdef USE_SUSPEND_MODE
			ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_STANDBY_TIME, pgmSuspendTime);
#endif
#ifdef USE_PRINTER
			PrtInit();
#elif defined(USE_WEIGHT_ONLY_ECR)
			CommInit();
#endif
			PowerInit();
			SaleModeInit();
		}
		else
		{			
			ParamWriteByte(PARAM_BUZZER_AREA_NUM, BUZZER_PARAM_OFFSET_USE, pgmBuzzerOnOffFlag);
#ifdef USE_WEIGHT_ONLY_ECR
			ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_ECR_TYPE, pgmECRType);
#elif defined(USE_PRINTER)
			ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST, pgmPrintOffet);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, pgmPrinterType);
			ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_LF_SIZE, pgmPrintLfSize);
			ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_BAUDRATE, pgmBaudRate);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, pgmLabelOnOffFlag);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_DATE_TYPE, pgmDateFormFlag);
#endif
			ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_BL_OFF_TIME, pgmBlOnOffFlag);
			ParamWriteByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BL_BRIGHT, pgmBlBrightness);
			ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_AUTO_OFF_TIME, pgmAutoOnOffFlag);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_RANGE, pgmCompareRange);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_COMPARE_RANGE, pgmCountCompareRange);		
#ifdef USE_SUSPEND_MODE
			ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_STANDBY_TIME, pgmSuspendTime);
#endif
#ifdef USE_PRINTER
			PrtInit();
#elif defined(USE_WEIGHT_ONLY_ECR)
			CommInit();
#endif
			PowerInit();
			SaleModeInit();
		}
		return MENU_RET_EXIT;
	}
	return MENU_RET_INCOMPLETE;
}
#endif

