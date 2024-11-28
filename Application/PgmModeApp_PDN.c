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
#include "../Common/RingBuf.h"
#include "../Time/TimeApi.h"

#include "../Common/PluData.h"
#include "../Communication/DebugApi.h"
#include "../Application/SaleModeApp.h"
#include "../Communication/PrtApi.h"
#include "../Communication/CommApi.h"
#include "../AD/AdApi.h"
#include "../SystemTimer/TimerApi.h"
#ifdef USE_TIME_SET
#include "../Common/TimeData.h"
#endif

#ifdef USE_PGM_PDN
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
#ifdef USE_TIME_SET
#define PGM_USER_SELECT_STATE				1
#define PGM_TIME_SET_STATE					2

#define PGM_USB_SET_ECR_STATE 				3

#define PGM_RS232_SELECT_STATE 				4

#define PGM_RS232_SET_ECR_STATE 			5
#define PGM_DEP_SET_STATE					6
#define PGM_DLP_SET_STATE					7

#define PGM_END_STATE						8
#else
#define PGM_USB_SET_ECR_STATE 				1

#define PGM_RS232_SELECT_STATE 				2

#define PGM_RS232_SET_ECR_STATE 			3
#define PGM_DEP_SET_STATE					4
#define PGM_DLP_SET_STATE					5

#define PGM_END_STATE						6
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
#ifdef USE_TIME_SET
static INT8U pgmTimeSetup(KEY_TYPE key);
#endif

/** @brief Program mode menu */
static INT8U pgmUserSetMode;
/** @brief bit 1 & bit 0, printer type, 00 : Don't Use, 01: Use DEP, 10: Use DLP, 11: reserve*/
static INT8U pgmPrinterType;
/** @brief Printer 관련 Offset */
static INT8U pgmPrintOffet;
/** @brief Communication Type */
static INT8U pgmCommType;
/** @brief Printer method */
static INT8U pgmPrintMethod;
/** @brief USB ECR Type */
static INT8U pgmUsbEcrType;
/** @brief RS232 ECR Type */
static INT8U pgmRs232EcrType;

#ifdef USE_TIME_SET
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
	{0x1F10, "AP", 	MENU_MODE_NORMAL, 		pgmUserSetup},
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
INT8U pgmUserSetup(KEY_TYPE key)
{
	static INT8U keyDrvType = 0;
#ifdef USE_TIME_SET
	static INT8U pgmUserMenu = 0;
	static INT8U pgmTimeSetMode = 0;
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
			pgmPrintOffet = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST);
			//pgmCommType -> 0:RS232_ECR / 1:RS232_DEP / 2:RS232_DLP / 3:RS232_BLE (USB ECR always use)
			pgmCommType = ParamReadByte(PARAM_COMM_AREA_NUM,COMM_PARAM_OFFSET_SELECT);
			pgmPrintMethod = pgmPrintOffet;
			//pgmPrinterType -> 00 : Not use, 01 : DEP50, 10 : DLP50, 11 : RESERVED
			pgmPrinterType = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH) & 0x03;
			pgmUsbEcrType = ParamReadByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_ECR_TYPE);
			pgmRs232EcrType = StorageReadByte(STORAGE_EXT_PARAM_BASE_ADDR);
			keyDrvType = ParamReadByte(PARAM_KEY_AREA_NUM, 3); // key type(C-6)
			
			break;
#ifdef USE_TIME_SET
		case PGM_USER_SELECT_STATE :
			switch(pgmUserMenu)
			{
				default:
				case 0:
					DispSetStr(MenuDataWinNum, "U.ECR ");
					break;
				case 1:
					DispSetStr(MenuDataWinNum, " RTC ");
					break;
			}
			break;
		case PGM_TIME_SET_STATE :
			pgmTimeSetup(key);
			break;
#endif
		case PGM_USB_SET_ECR_STATE:
			switch(pgmUsbEcrType)
			{
				case 0:
					DispSetStr(MenuDataWinNum, "TYP 0");
					break;
				case 1:
					DispSetStr(MenuDataWinNum, "TYP 1");
					break;
				case 2:
					DispSetStr(MenuDataWinNum, "TYP 2");
					break;
				case 3:
					DispSetStr(MenuDataWinNum, "TYP 3");
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
				case 8:
					DispSetStr(MenuDataWinNum, "TYP 8");
					break;
				case 9:
					DispSetStr(MenuDataWinNum, "TYP 9");
					break;
				case 10:
					DispSetStr(MenuDataWinNum, "TYP10");
					break;
				case 11:
					DispSetStr(MenuDataWinNum, "TYP12");
					break;
				case 12:
					DispSetStr(MenuDataWinNum, "TYP13");
					break;
				case 13:
					DispSetStr(MenuDataWinNum, "TYP14");
					break;
				case 14:
					DispSetStr(MenuDataWinNum, "TYP15");
					break;
#ifdef USE_SLOVAKIA_PROTOCOL
				case 15:
					DispSetStr(MenuDataWinNum, "TYP16");
					break;
#endif
				case 19:
					DispSetStr(MenuDataWinNum, "TYP20");
					break;
#ifdef USE_PD2_ECR
				case 17:
					DispSetStr(MenuDataWinNum, "TYP18");
					break;
				case 18:
					DispSetStr(MenuDataWinNum, "TYP19");
					break;
				case 20:
					DispSetStr(MenuDataWinNum, "TYP21");
					break;
				case 21:
					DispSetStr(MenuDataWinNum, "TYP22");
					break;
				case 22:
					DispSetStr(MenuDataWinNum, "TYP23");
					break;
				case 23:
					DispSetStr(MenuDataWinNum, "TYP24");
					break;
				case 24:
					DispSetStr(MenuDataWinNum, "TYP25");
					break;
				case 25:
					DispSetStr(MenuDataWinNum, "TYP26");
					break;
				case 26:
					DispSetStr(MenuDataWinNum, "TYP27");
					break;
				case 27:
					DispSetStr(MenuDataWinNum, "TYP28");
					break;
				case 28:
					DispSetStr(MenuDataWinNum, "TYP29");
					break;
				case 29:
					DispSetStr(MenuDataWinNum, "TYP30");
					break;
				case 30:
					DispSetStr(MenuDataWinNum, "TYP31");
					break;
				case 31:
					DispSetStr(MenuDataWinNum, "TYP32");
					break;
				case 32:
					DispSetStr(MenuDataWinNum, "TYP33");
					break;
				case 33:
					DispSetStr(MenuDataWinNum, "TYP34");
					break;
				case 34:
					DispSetStr(MenuDataWinNum, "TYP35");
					break;
#endif	//USE_PD2_ECR
				case 35:
					DispSetStr(MenuDataWinNum, "TYP36");
					break;
				case 36:
					DispSetStr(MenuDataWinNum, "TYP37");
					break;
			}
			break;
		case PGM_RS232_SELECT_STATE:
			switch(pgmCommType)
			{
				default:
				case RS232_ECR:
					DispSetStr(MenuDataWinNum, "R.ECR ");
					break;
				case RS232_DEP:
					DispSetStr(MenuDataWinNum, " DEP ");
					break;
				case RS232_DLP:
					DispSetStr(MenuDataWinNum, " DLP ");
					break;
				case RS232_BLE:
					DispSetStr(MenuDataWinNum, " BLE ");
					break;
			}
			break;
		case PGM_RS232_SET_ECR_STATE:
			pgmPrinterType = PRT_NONE;
			pgmPrintMethod = 0;
			switch(pgmRs232EcrType)
			{
				case 0:
					DispSetStr(MenuDataWinNum, "TYP 0");
					break;
				case 1:
					DispSetStr(MenuDataWinNum, "TYP 1");
					break;
				case 2:
					DispSetStr(MenuDataWinNum, "TYP 2");
					break;
				case 3:
					DispSetStr(MenuDataWinNum, "TYP 3");
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
				case 8:
					DispSetStr(MenuDataWinNum, "TYP 8");
					break;
				case 9:
					DispSetStr(MenuDataWinNum, "TYP 9");
					break;
				case 10:
					DispSetStr(MenuDataWinNum, "TYP10");
					break;
				case 11:
					DispSetStr(MenuDataWinNum, "TYP12");
					break;
				case 12:
					DispSetStr(MenuDataWinNum, "TYP13");
					break;
				case 13:
					DispSetStr(MenuDataWinNum, "TYP14");
					break;
				case 14:
					DispSetStr(MenuDataWinNum, "TYP15");
					break;
#ifdef USE_SLOVAKIA_PROTOCOL
				case 15:
					DispSetStr(MenuDataWinNum, "TYP16");
					break;
#endif
				case 19:
					DispSetStr(MenuDataWinNum, "TYP20");
					break;
#ifdef USE_PD2_ECR
				case 17:
					DispSetStr(MenuDataWinNum, "TYP18");
					break;
				case 18:
					DispSetStr(MenuDataWinNum, "TYP19");
					break;
				case 20:
					DispSetStr(MenuDataWinNum, "TYP21");
					break;
				case 21:
					DispSetStr(MenuDataWinNum, "TYP22");
					break;
				case 22:
					DispSetStr(MenuDataWinNum, "TYP23");
					break;
				case 23:
					DispSetStr(MenuDataWinNum, "TYP24");
					break;
				case 24:
					DispSetStr(MenuDataWinNum, "TYP25");
					break;
				case 25:
					DispSetStr(MenuDataWinNum, "TYP26");
					break;
				case 26:
					DispSetStr(MenuDataWinNum, "TYP27");
					break;
				case 27:
					DispSetStr(MenuDataWinNum, "TYP28");
					break;
				case 28:
					DispSetStr(MenuDataWinNum, "TYP29");
					break;
				case 29:
					DispSetStr(MenuDataWinNum, "TYP30");
					break;
				case 30:
					DispSetStr(MenuDataWinNum, "TYP31");
					break;
				case 31:
					DispSetStr(MenuDataWinNum, "TYP32");
					break;
				case 32:
					DispSetStr(MenuDataWinNum, "TYP33");
					break;
				case 33:
					DispSetStr(MenuDataWinNum, "TYP34");
					break;
				case 34:
					DispSetStr(MenuDataWinNum, "TYP35");
					break;
#endif	//USE_PD2_ECR
				case 35:
					DispSetStr(MenuDataWinNum, "TYP36");
					break;
				case 36:
					DispSetStr(MenuDataWinNum, "TYP37");
					break;
			}
			break;
		case PGM_DEP_SET_STATE:
			pgmPrinterType = PRT_DEP50;
			switch(pgmPrintMethod)
			{
				default:
				case 0:
					DispSetStr(MenuDataWinNum, "NOSND");
					pgmPrintMethod = 0;						
					break;
				case 1:
					if (keyDrvType != 1) { // Unit Change Key
						DispSetStr(MenuDataWinNum, "K-SND");
						break;
					}
					else {
						pgmPrintMethod++;
					}
				case 2:
					DispSetStr(MenuDataWinNum, "S-SND");
					break;
				case 6:
					DispSetStr(MenuDataWinNum, "F-SND");	//CAS 22byte, CD-300
					break;
			}
			break;	
		case PGM_DLP_SET_STATE:
			pgmPrinterType = PRT_DLP50;
			if (pgmPrintMethod == 6) // F-Snd
				pgmPrintMethod = 0;
			switch(pgmPrintMethod)
			{
				default:
				case 0:
					DispSetStr(MenuDataWinNum, "NOSND");
					pgmPrintMethod = 0;
					break;
				case 1:
					if (keyDrvType != 1) { // Unit Change Key
						DispSetStr(MenuDataWinNum, "K-SND");
						break;
					}
					else {
						pgmPrintMethod++;
					}
				case 2:
					DispSetStr(MenuDataWinNum, "S-SND");
					break;
			}
			break;
			
		case PGM_END_STATE:
			DispSetStr(MenuDataWinNum, " SAVE ");
			if (pgmCommType == RS232_BLE) {
				pgmPrinterType = PRT_NONE;
				pgmPrintMethod = 0;
			}
			break;
	}

	if (key == KEY_ZERO)
	{
		switch(pgmUserSetMode)
		{
			case PGM_INIT_STATE:
#ifdef USE_TIME_SET
				pgmUserSetMode = PGM_USER_SELECT_STATE;
#else
				pgmUserSetMode = PGM_USB_SET_ECR_STATE;
#endif
				break;
#ifdef USE_TIME_SET
			case PGM_USER_SELECT_STATE:
				if (pgmUserMenu == 0)
					pgmUserSetMode = PGM_USB_SET_ECR_STATE; // 0
				else
					pgmUserSetMode = PGM_TIME_SET_STATE; // 1
				break;
			case PGM_TIME_SET_STATE:
				if (pgmTimeSetMode < 6)
					pgmTimeSetMode++;
				else if (pgmTimeSetMode == 6)
					pgmUserSetMode = PGM_END_STATE;
				break;
#endif
			case PGM_USB_SET_ECR_STATE:
				pgmUserSetMode = PGM_RS232_SELECT_STATE;
				break;
			case PGM_RS232_SELECT_STATE:
				if (pgmCommType == RS232_ECR)
					pgmUserSetMode = PGM_RS232_SET_ECR_STATE;
				else if (pgmCommType == RS232_DEP)
					pgmUserSetMode = PGM_DEP_SET_STATE;
				else if (pgmCommType == RS232_DLP)
					pgmUserSetMode = PGM_DLP_SET_STATE;
				else
					pgmUserSetMode = PGM_END_STATE;
				break;
			case PGM_RS232_SET_ECR_STATE:
				pgmUserSetMode = PGM_END_STATE;
				break;
			case PGM_DEP_SET_STATE:
				pgmUserSetMode = PGM_END_STATE;
				break;
			case PGM_DLP_SET_STATE:
				pgmUserSetMode = PGM_END_STATE;
				break;

			case PGM_END_STATE:
				if (pgmPrintMethod == 0)
				{
					pgmPrinterType = PRT_NONE;
				}
				pgmPrintOffet = pgmPrintMethod;
				pgmPrinterType |= (ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH) & 0xFC);
				ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST, pgmPrintOffet);
				ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_SELECT, pgmCommType);
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, pgmPrinterType);
				ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_ECR_TYPE, pgmUsbEcrType);
				StorageWriteByte(STORAGE_EXT_PARAM_BASE_ADDR, pgmRs232EcrType);
#ifdef USE_PRINTER
				PrtInit();
#endif
				CommInit();
				PowerInit();
				return MENU_RET_EXIT;  
		break;
		}
	}
	
 	if (key == KEY_TARE)
	{
		switch(pgmUserSetMode)
		{
#ifdef USE_TIME_SET
			case PGM_USER_SELECT_STATE:
				pgmUserMenu++;
				if (pgmUserMenu > 1)
					pgmUserMenu = 0;
				break;
#endif
#ifdef USE_PD2_ECR
			case PGM_USB_SET_ECR_STATE:
				pgmUsbEcrType++;
				if (pgmUsbEcrType > 35)
					pgmUsbEcrType = 0;
				else if (pgmUsbEcrType == 11)
					pgmUsbEcrType = 12;
	#ifndef USE_SLOVAKIA_PROTOCOL
				else if (pgmUsbEcrType == 15)
					pgmUsbEcrType = 17;
	#endif
				else if (pgmUsbEcrType == 16)
					pgmUsbEcrType = 18;
				break;
#else	//USE_PD2_ECR
			case PGM_USB_SET_ECR_STATE:
				pgmUsbEcrType++;
	#ifdef USE_SLOVAKIA_PROTOCOL
				if (pgmUsbEcrType > 15)
	#else
				if (pgmUsbEcrType == 15)	// ECR Type0~10, 12~15, 20 사용
					pgmUsbEcrType = 19;
				else if (pgmUsbEcrType == 20)
					pgmUsbEcrType = 35; // ECR 36 사용 
				else if (pgmUsbEcrType > 36) // ECR 37 사용
	#endif
					pgmUsbEcrType = 0;
				break;
#endif	//USE_PD2_ECR
			case PGM_RS232_SELECT_STATE: 
				if (pgmCommType != RS232_COMM_MENU_LAST)
				{
					pgmCommType++;
				}
				else
				{
					pgmCommType = 0;
				}
#ifdef USE_PRINTER
#else
				if ((pgmCommType == RS232_DEP) || (pgmCommType == RS232_DLP))
					pgmCommType = RS232_BLE;
#endif
				break;
#ifdef USE_PD2_ECR
			case PGM_RS232_SET_ECR_STATE:
				pgmRs232EcrType++;
				if (pgmRs232EcrType > 35)
					pgmRs232EcrType = 0;
				else if (pgmRs232EcrType == 11)
					pgmRs232EcrType = 12;
	#ifndef USE_SLOVAKIA_PROTOCOL
				else if (pgmRs232EcrType == 15)
					pgmRs232EcrType = 17;
	#endif
				else if (pgmRs232EcrType == 16)
					pgmRs232EcrType = 18;
				break;
#else	//USE_PD2_ECR
			case PGM_RS232_SET_ECR_STATE:
				pgmRs232EcrType++;
	#ifdef USE_SLOVAKIA_PROTOCOL
				if (pgmRs232EcrType > 15)
	#else
				if (pgmRs232EcrType == 15)	// ECR Type0~10, 12~15, 20 사용
					pgmRs232EcrType = 19;
				else if (pgmRs232EcrType == 20)
					pgmRs232EcrType = 35; // ECR 36 사용 
				else if (pgmRs232EcrType > 36) // ECR 37 사용 
	#endif
					pgmRs232EcrType = 0;
				break;
#endif	//USE_PD2_ECR
			case PGM_DEP_SET_STATE: 
				if (pgmPrintMethod == 2)
				{
					pgmPrintMethod = 6;
				}
				else if (pgmPrintMethod == 0)
				{
					if (keyDrvType == 1) // Unit Change Key
						pgmPrintMethod = 2;
					else
						pgmPrintMethod++;
				}
				else if (pgmPrintMethod != 6)
				{
					pgmPrintMethod++;
				}
				else 
				{
					pgmPrintMethod = 0;
				}
				break;	
			case PGM_DLP_SET_STATE: 
				if (pgmPrintMethod == 0)
				{
					if (keyDrvType == 1)
						pgmPrintMethod = 2;
					else
						pgmPrintMethod++;
				}
				else if (pgmPrintMethod != 2)
				{
					pgmPrintMethod++;
				}
				else 
				{
					pgmPrintMethod = 0;
				}
				break;
		}
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

#ifdef USE_TIME_SET
/**
********************************************************************************
* @brief    Program Time Setup \n
* @param    key : TARE KEY(자리수 변경), FUNCTION KEY(숫자 변경)
* @return   
* @remark   연,월,일,시,분,초 세팅
********************************************************************************
*/
INT8U pgmTimeSetup(KEY_TYPE key)
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
	if (key == KEY_ZERO)
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
