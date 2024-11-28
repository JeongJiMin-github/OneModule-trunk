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

#ifdef USE_PGM_SW
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

#if defined(USE_RS232) && defined(USE_WEIGHT_ONLY_ECR)
#define PGM_COM_SELECT_STATE 				1
#endif

#ifdef USE_RS232
#define PGM_PRT_SET_STATE 					2
#endif

#ifdef USE_WEIGHT_ONLY_ECR
#define PGM_ECR_SET_STATE					3    //SW ECR Mode
#endif

#define PGM_AUTOPOWER_SET_STATE 			4

#ifdef USE_SUSPEND_MODE
#define PGM_SUSPEND_SET_STATE 				5
#endif

#ifdef USE_TARE_AUTO_CLEAR_SET
#define PGM_TARE_AUTO_CLEAR_SET_STATE		7
#endif

#ifdef USE_BL_MENU
#define PGM_BACKLIGHT_SET_STATE				8
#define PGM_BRIGHT_SET_STATE 				9
#endif

#define PGM_END_STATE						10



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
/** @brief bit 1 & bit 0, printer type, 00 : Don't Use, 01: Use DEP, 10: Use DLP, 11: reserve*/
static INT8U pgmPrinterType;
/** @brief Printer 관련 Offset */
static INT8U pgmPrintOffet;
/** @brief Auto on/off flag */
static INT8U pgmAutoOnOffFlag;
/** @brief Communication Type */
static INT8U pgmCommType;
#ifdef USE_BL_MENU	
/** @brief BackLight on/off flag */
static INT8U pgmBlOnOffFlag;
/** @brief BackLight brightness */
static INT8U pgmBlBrightness;
#endif
/** @brief Printer method */
static INT8U pgmPrintMethod;
/** @brief ECR Printer Type */
static INT8U pgmECRType;
/** @brief Default setting on/off flag */
static INT8U pgmDefaultOnOffFlag;
/** @brief Suspend Time */
#ifdef USE_SUSPEND_MODE	
static INT8U pgmSuspendTime;
#endif
/** @brief Default setting on/off flag */
static INT8U pgmDefaultOnOffFlag;
#ifdef USE_TARE_AUTO_CLEAR_SET
/** @brief Tare Auto Clear flag */
static INT8U pgmAutoTareClearFlag;
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
	pgmUserSetMode = PGM_INIT_STATE;
#ifdef USE_BL_MENU
	pgmBlOnOffFlag = OFF;
	pgmBlBrightness = 0;
#endif
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
//csh 20130403
#ifdef USE_TARE_AUTO_CLEAR_SET
	INT8U tempInt8u;
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
#if defined(USE_RS232) && defined(USE_WEIGHT_ONLY_ECR)
			pgmCommType = ParamReadByte(PARAM_COMM_AREA_NUM,COMM_PARAM_OFFSET_SELECT);
#endif
			pgmPrintMethod = pgmPrintOffet;
			//pgmPrinterType -> 00 : Not use, 01 : DEP50, 10 : DLP50, 11 : RESERVED
			pgmPrinterType = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH) & 0x03;
			pgmAutoOnOffFlag = ParamReadByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_AUTO_OFF_TIME);
#ifdef USE_BL_MENU
			pgmBlOnOffFlag = ParamReadByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_BL_OFF_TIME);
			pgmBlBrightness = ParamReadByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BL_BRIGHT);
#endif
#ifdef USE_SUSPEND_MODE	
			pgmSuspendTime = ParamReadByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_STANDBY_TIME);//ok
#endif
#ifdef USE_WEIGHT_ONLY_ECR
			pgmECRType= ParamReadByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_ECR_TYPE);			
#endif
#ifdef USE_TARE_AUTO_CLEAR_SET
			tempInt8u = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST);
			pgmAutoTareClearFlag = (tempInt8u & 0x04) >> 2;
#endif
			break;
#if defined(USE_RS232) && defined(USE_WEIGHT_ONLY_ECR)
		case PGM_COM_SELECT_STATE:
			switch(pgmCommType)
			{
				default:
				case 0:
					DispSetStr(MenuDataWinNum, " PRT ");
					break;
				case 1:
					DispSetStr(MenuDataWinNum, " ECR ");
					break;
			}
			break;
#endif	//USE_RS232 && USE_WEIGHT_ONLY_ECR
#ifdef USE_RS232
		case PGM_PRT_SET_STATE:
			pgmECRType = 6;
			switch(pgmPrintMethod)
			{
				default:
				case 0:
					DispSetStr(MenuDataWinNum, "NOSND");
					pgmPrinterType = PRT_NONE;
					pgmPrintMethod = 0;						
					break;
				case 1:
					DispSetStr(MenuDataWinNum, "K-SND");
					pgmPrinterType = PRT_DEP50;
					break;
				case 2:
					DispSetStr(MenuDataWinNum, "S-SND");
					pgmPrinterType = PRT_DEP50;
					break;
				case 3:
					DispSetStr(MenuDataWinNum, "U-SND");
					pgmPrinterType = PRT_DEP50;
					break;
				case 4:
					DispSetStr(MenuDataWinNum, "C-SND");
					pgmPrinterType = PRT_DEP50;
					break;
				case 5:
					DispSetStr(MenuDataWinNum, "E-SND");	//Enter Send Mode 
					pgmPrinterType = PRT_DEP50;
					break;
			}
			break;	
#endif	//USE_RS232
#ifdef USE_WEIGHT_ONLY_ECR		//Only ECR Mode	
		case PGM_ECR_SET_STATE:
			pgmPrinterType = PRT_NONE;
			pgmPrintMethod = 0;			
			switch(pgmECRType)
			{
				case 0:
					DispSetStr(MenuDataWinNum, "TYP 0");
					break;
//				case 1:
//					DispSetStr(MenuDataWinNum, "TYP 1");
//					break;
				case 2:
					DispSetStr(MenuDataWinNum, "TYP 2");
					break;
//				case 3:
//					DispSetStr(MenuDataWinNum, "TYP 3");
//					break;
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
//				case 8:
//					DispSetStr(MenuDataWinNum, "TYP 8");
//					break;
//				case 9:
//					DispSetStr(MenuDataWinNum, "TYP 9");
//					break;
//				case 10:
//					DispSetStr(MenuDataWinNum, "TYP10");
//					break;
				case 11:
					DispSetStr(MenuDataWinNum, "TYP12");
					break;
			}
			break;
#endif	//USE_WEIGHT_ONLY_ECR
		case PGM_AUTOPOWER_SET_STATE:
			switch(pgmAutoOnOffFlag)
			{
				case 0:
				default:
					DispSetStr(MenuDataWinNum, "AP OFF");
					pgmAutoOnOffFlag = 0;
					break;
#ifdef USE_AUTO_POWER_SET_CHANGE
				case 2:
					DispSetStr(MenuDataWinNum, "AP  2");
					break;
				case 5:
					DispSetStr(MenuDataWinNum, "AP  5");
					break;
#endif	//USE_AUTO_POWER_SET_CHANGE
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
#ifdef USE_BL_MENU
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
#endif
#ifdef USE_SUSPEND_MODE			
		case PGM_SUSPEND_SET_STATE:
			switch (pgmSuspendTime)
			{
				case 0:
				default:
					DispSetStr(MenuDataWinNum, "SUSOFF");
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
#endif	//USE_SUSPEND_MODE

#ifdef USE_TARE_AUTO_CLEAR_SET
			case PGM_TARE_AUTO_CLEAR_SET_STATE:
				switch (pgmAutoTareClearFlag)
				{				
					case 0:
					default:
						DispSetStr(MenuDataWinNum, "TCOFF");
						break;
					case 1:
						DispSetStr(MenuDataWinNum, "TC ON");
						break;
			}
			break;
#endif	//USE_TARE_AUTO_CLEAR_SET
	}

	if (key == KEY_ZERO)
	{
		switch(pgmUserSetMode)
		{
			case PGM_INIT_STATE:	// 0
#if defined(USE_RS232) && defined(USE_WEIGHT_ONLY_ECR)
				pgmUserSetMode = PGM_COM_SELECT_STATE;		// 1
#elif defined(USE_RS232)
				pgmUserSetMode = PGM_PRT_SET_STATE;		// 2
#elif defined(USE_WEIGHT_ONLY_ECR)
 				pgmUserSetMode = PGM_ECR_SET_STATE;		// 3
#else
				pgmUserSetMode = PGM_AUTOPOWER_SET_STATE;		// 4
#endif	//USE_RS232 && USE_WEIGHT_ONLY_ECR
				break;
#if defined(USE_RS232) && defined(USE_WEIGHT_ONLY_ECR)
			case PGM_COM_SELECT_STATE:	// 1
				if(pgmCommType) pgmUserSetMode = PGM_ECR_SET_STATE;		// 2
				else pgmUserSetMode = PGM_PRT_SET_STATE;		// 3
				break;
#endif	//USE_RS232 && USE_WEIGHT_ONLY_ECR
#ifdef USE_RS232
			case PGM_PRT_SET_STATE:		// 2
				pgmUserSetMode = PGM_AUTOPOWER_SET_STATE;		// 4
				break;
#endif	//USE_RS232
#ifdef USE_WEIGHT_ONLY_ECR
			case PGM_ECR_SET_STATE:		// 3
				pgmUserSetMode = PGM_AUTOPOWER_SET_STATE;		// 4
				break;
#endif	//USE_WEIGHT_ONLY_ECR
			case PGM_AUTOPOWER_SET_STATE:	// 4
#ifdef USE_BL_MENU
				pgmUserSetMode = PGM_BACKLIGHT_SET_STATE;		// 8
				break;
            case PGM_BACKLIGHT_SET_STATE:   //8
                if (pgmBlOnOffFlag != 0) 
                {
                    pgmUserSetMode = PGM_BRIGHT_SET_STATE;		    // 9
                    break;
                }
				else
                {
                  pgmUserSetMode = PGM_END_STATE;	// 10
                }
            case PGM_BRIGHT_SET_STATE:      //9
    #ifdef defined(USE_TARE_AUTO_CLEAR_SET)
                pgmUserSetMode = PGM_TARE_AUTO_CLEAR_SET_STATE; // 7
                break;
    #else
                pgmUserSetMode = PGM_END_STATE; // 10
    #endif
#else   //USE_BL_MENU
    #ifdef USE_SUSPEND_MODE
				pgmUserSetMode = PGM_SUSPEND_SET_STATE;	// 5
                break;
    #elif defined(USE_TARE_AUTO_CLEAR_SET)
                pgmUserSetMode = PGM_TARE_AUTO_CLEAR_SET_STATE;	// 7
                break;
    #else
                pgmUserSetMode = PGM_END_STATE;	// 10
    #endif
    #ifdef USE_SUSPEND_MODE
            case PGM_SUSPEND_SET_STATE:	// 5
        #ifdef USE_TARE_AUTO_CLEAR_SET
                pgmUserSetMode = PGM_TARE_AUTO_CLEAR_SET_STATE;	// 7
                break;
        #else
                pgmUserSetMode = PGM_END_STATE;	// 10
        #endif
    #endif	//USE_SUSPEND_MODE
	#ifdef USE_TARE_AUTO_CLEAR_SET // 9
            case PGM_TARE_AUTO_CLEAR_SET_STATE:	// 7
                pgmUserSetMode = PGM_END_STATE;	// 10
	#endif	//USE_TARE_AUTO_CLEAR_SET
#endif	//USE_BL_MENU
			case PGM_END_STATE:	// 10
				DispSetConstStrMsg(WIN_WEIGHT, " SAVE");
				DispSetRunMsgPage(10, 1);
				pgmPrintOffet = pgmPrintMethod;
				pgmPrinterType |= (ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH) & 0xFC);
				ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST, pgmPrintOffet);
#if defined(USE_RS232) && defined(USE_WEIGHT_ONLY_ECR)
				ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_SELECT, pgmCommType);
#endif
#ifdef USE_RS232
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, pgmPrinterType);
#endif
#ifdef USE_BL_MENU
				ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_BL_OFF_TIME, pgmBlOnOffFlag);
				ParamWriteByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BL_BRIGHT, pgmBlBrightness);
#endif
#ifdef USE_SUSPEND_MODE	
				ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_STANDBY_TIME, pgmSuspendTime);
#endif
				ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_AUTO_OFF_TIME, pgmAutoOnOffFlag);
#ifdef USE_WEIGHT_ONLY_ECR
				ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_ECR_TYPE, pgmECRType);	
#endif

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
#endif
#ifdef USE_PRINTER
				PrtInit();
#endif
#ifdef USE_WEIGHT_ONLY_ECR
				CommInit();
#endif
				PowerInit();
				return MENU_RET_EXIT;  
		break;
		}
	}
	
 	if (key == KEY_TARE)
	{
		switch(pgmUserSetMode)
		{
#if defined(USE_RS232) && defined(USE_WEIGHT_ONLY_ECR)
			case PGM_COM_SELECT_STATE: 
				if (pgmCommType != 1)
				{
					pgmCommType++;
				}
				else
				{
					pgmCommType = 0;
				}
				break;
#endif	//USE_RS232 && USE_WEIGHT_ONLY_ECR
#ifdef USE_RS232
			case PGM_PRT_SET_STATE: 
				if (pgmPrintMethod != 5)
				{
					pgmPrintMethod++;
				}
				else 
				{
					pgmPrintMethod = 0;
				}
				break;	
#endif	//USE_RS232
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
#endif	//USE_WEIGHT_ONLY_ECR
#ifdef USE_BL_MENU
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
#endif	//USE_BL_MENU
			case PGM_AUTOPOWER_SET_STATE:
#ifdef USE_AUTO_POWER_SET_CHANGE
				if (pgmAutoOnOffFlag == 2)
				{
					pgmAutoOnOffFlag = 0;
				}
				else if (pgmAutoOnOffFlag == 0)
				{
					pgmAutoOnOffFlag = 60;
				}
				else if (pgmAutoOnOffFlag == 60)
				{
					pgmAutoOnOffFlag = 30;
				}
				else if (pgmAutoOnOffFlag == 30)
				{
					pgmAutoOnOffFlag = 10;
				}
				else if (pgmAutoOnOffFlag == 10)
				{
					pgmAutoOnOffFlag = 5;
				}
				else pgmAutoOnOffFlag = 2;
#else
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
#endif	//USE_AUTO_POWER_SET_CHANGE
				break;
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
#endif	//USE_SUSPEND_MODE

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
#endif	//USE_TARE_AUTO_CLEAR_SET
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



