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

#ifdef USE_PGM_PB
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
#ifdef USE_SIMPLE_PRT_SELECT
#define PGM_PRT_SELECT_STATE 				1
#define PGM_PRT_BARCODE_SET_STATE			2
#endif
#define PGM_PRT_SET_STATE 					3
#ifdef USE_WEIGHT_BACKUP
#define PGM_BACKUP_SET_STATE				4		//Weight Backup Setting State
#endif
#define PGM_AUTOPOWER_SET_STATE 			5
#ifdef USE_BL_MENU	
#define PGM_BACKLIGHT_SET_STATE				6
#define PGM_BRIGHT_SET_STATE 				7
#endif
#ifdef USE_SALE_SIMPLE_HEALTH 
#define PGM_BLINK_SET_STATE					8
#endif
#ifdef USE_TARE_AUTO_CLEAR_SET
#define PGM_TARE_AUTO_CLEAR_SET_STATE		9
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
/** @brief Printer method */
static INT8U pgmPrintMethod;
/** @brief Default setting on/off flag */
static INT8U pgmDefaultOnOffFlag;
/** @brief Suspend Time */
static INT8U pgmSuspendTime;
/** @brief Default setting on/off flag */
static INT8U pgmDefaultOnOffFlag;
/** @brief BackLight on/off flag */
static INT8U pgmBlOnOffFlag;
/** @brief BackLight brightness */
static INT8U pgmBlBrightness;
/** @brief Weight backup on/off flag */
static INT8U pgmBackupOnOffFlag;
/** @brief Auto Hold Blink Count */
static INT8U pgmBlinkCount;
/** @brief Tare Auto Clear flag */
static INT8U pgmAutoTareClearFlag;
/** @brief Print Barcode on/off flag */
static INT8U pgmPrintBarcode;



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
	
	//SGtest
	pgmBlOnOffFlag = OFF;
	pgmBackupOnOffFlag = 0;
	pgmBlBrightness = 0;
	pgmBlinkCount = 0;
	pgmPrintOffet = 0;
	pgmPrintBarcode = 0;
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
	INT8U tempInt8u;

	switch (pgmUserSetMode)
	{
		case PGM_INIT_STATE:
#ifdef REVISION_NUM
			if(!TimerCheckDelay100ms(TIMER_DELAY_100MS_INITIAL))
			{
				DispSetMsgWithNum("r", REVISION_NUM);
				TimerSetDelay100ms(TIMER_DELAY_100MS_INITIAL, 10);	//include disp. revision 10
				while (!TimerCheckDelay100ms(TIMER_DELAY_100MS_INITIAL));	//wait key
			}
#endif
			DispSetStr(MenuDataWinNum, "U SET");			
			pgmPrintOffet = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST);
			pgmPrintMethod = (pgmPrintOffet & 0x07);	//pgmPrintOffet의 LowBit 3Bit 사용
			pgmBackupOnOffFlag = DispFiltParam.zeroBufBackupEnable;
			//pgmPrinterType -> 00 : Not use, 01 : DEP50, 10 : DLP50, 11 : RESERVED
			pgmPrinterType = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH) & 0x03;
#ifdef USE_SIMPLE_PRT_SELECT
			pgmPrintBarcode = (pgmPrintOffet & 0x08) >> 3;	//pgmPrintOffet의 4th Bit 사용
#endif
			pgmBlOnOffFlag = ParamReadByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_BL_OFF_TIME);	//sgtest
			pgmBlBrightness = ParamReadByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BL_BRIGHT);
			pgmBlinkCount = ParamReadByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BLINK_COUNT);

			pgmAutoOnOffFlag = ParamReadByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_AUTO_OFF_TIME);
			pgmSuspendTime = ParamReadByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_STANDBY_TIME);	//ok

			tempInt8u = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST); 
			pgmAutoTareClearFlag = (tempInt8u & 0x04) >> 2;
			break;
			
#ifdef USE_SIMPLE_PRT_SELECT
		case PGM_PRT_SELECT_STATE:
			switch(pgmPrinterType)
			{
				case PRT_NONE:
				default:
					DispSetStr(MenuDataWinNum, " OFF ");
					pgmPrinterType = 0;
					pgmPrintMethod = PRT_NOT_USE;
					break;
				case PRT_DEP50:
					DispSetStr(MenuDataWinNum, " DEP ");
					break;
				case PRT_DLP50:
					DispSetStr(MenuDataWinNum, " DLP ");	//DLP Setting Mode 추가
					break;
			}
			break;
#endif			
		case PGM_PRT_SET_STATE:

			pgmPrintBarcode = 0;	//DEP 설정의 경우 DLP 바코드 설정  초기화

			switch(pgmPrintMethod)
			{	
				default:
				case PRT_NOT_USE:
					DispSetStr(MenuDataWinNum, "NOSND");
					pgmPrinterType = PRT_NONE;
					pgmPrintMethod = 0;						
					break;
				case PRT_USE_BY_KEY:
					DispSetStr(MenuDataWinNum, "K-SND");
					pgmPrinterType = PRT_DEP50;
					break;
				case PRT_USE_BY_AUTO:
					DispSetStr(MenuDataWinNum, "S-SND");
					pgmPrinterType = PRT_DEP50;
					break;		
#ifndef USE_NOT_UNSTABLE									  //OIML Certification product use not unstable mode
				case PRT_USE_BY_STREAM:
					DispSetStr(MenuDataWinNum, "U-SND");
					pgmPrinterType = PRT_DEP50;
					break;
				case PRT_USE_BY_CHANGE_WEIGHT:
					DispSetStr(MenuDataWinNum, "C-SND");
					pgmPrinterType = PRT_DEP50;
					break;
 #endif			
 #ifdef USE_STREAM_CAS_22BYTE
				case PRT_USE_BY_FLOW:
					DispSetStr(MenuDataWinNum, "F-SND");	//Flow send Mode (Only DP model)
					break;
 #endif
			}
			break;	 
#ifdef USE_SIMPLE_PRT_SELECT		
		case PGM_PRT_BARCODE_SET_STATE:	

			pgmPrintMethod = PRT_NOT_USE;	//DLP 바코드 설정의 경우 DEP프린트 모드 설정 초기화
							
			switch(pgmPrintBarcode)
			{				
				case 0:
				default:
					DispSetStr(MenuDataWinNum, "BC OFF");					
					pgmPrintBarcode = 0;
					break;
				case 1:
					DispSetStr(MenuDataWinNum, "BC ON");
					break;
			}
			break;
#endif			
#ifdef USE_WEIGHT_BACKUP
		case PGM_BACKUP_SET_STATE:
			switch(pgmBackupOnOffFlag)
			{
				pgmBackupOnOffFlag = DispFiltParam.zeroBufBackupEnable;		
				
				default:
				case 0:
					DispSetStr(MenuDataWinNum, "BA OFF");
					pgmBackupOnOffFlag = 0;
					break;
				case 1:
					DispSetStr(MenuDataWinNum, "BA ON");
					break;
			}
			break;
#endif			

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

#ifdef USE_BL_MENU 	//sg test	
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
#ifdef USE_SALE_SIMPLE_HEALTH
		case PGM_BLINK_SET_STATE:
			switch (pgmBlinkCount)
			{				
				case 1:
					DispSetStr(MenuDataWinNum, "BLINK1");
					break;
				case 2:
					DispSetStr(MenuDataWinNum, "BLINK2");
					break;
				case 3:
				default:
					DispSetStr(MenuDataWinNum, "BLINK3");
					pgmBlinkCount = 3;
					break;
				case 4:
					DispSetStr(MenuDataWinNum, "BLINK4");
					break;
				case 5:
					DispSetStr(MenuDataWinNum, "BLINK5");
					break;
				case 6:
					DispSetStr(MenuDataWinNum, "BLINK6");
					break;
				case 7:
					DispSetStr(MenuDataWinNum, "BLINK7");
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

	}

#ifdef USE_BL_MENU 	//sg test	
 	if (key == KEY_ZERO)
	{
		switch(pgmUserSetMode)
		{
			case PGM_INIT_STATE:	// 0
 #ifdef USE_SIMPLE_PRT_SELECT
				pgmUserSetMode = PGM_PRT_SELECT_STATE;	// 1
				break;
 #else
				pgmUserSetMode = PGM_PRT_SET_STATE;	// 3
				break;
 #endif
				
			case PGM_PRT_SELECT_STATE:	// 1
				if(pgmPrinterType == PRT_DEP50)
				{
					pgmUserSetMode = PGM_PRT_SET_STATE;	// 3
				}
				else if(pgmPrinterType == PRT_DLP50)
				{
					pgmUserSetMode = PGM_PRT_BARCODE_SET_STATE;	// 2
				}
				else // PRT_NONE
				{
 #ifndef USE_WEIGHT_BACKUP
					pgmUserSetMode = PGM_AUTOPOWER_SET_STATE;	// 5
 #else
					pgmUserSetMode = PGM_BACKUP_SET_STATE; // 4
 #endif
				}
				break;
				
			case PGM_PRT_BARCODE_SET_STATE:	// 2
 #ifndef USE_WEIGHT_BACKUP
				pgmUserSetMode = PGM_AUTOPOWER_SET_STATE;	// 5
 #else
				pgmUserSetMode = PGM_BACKUP_SET_STATE;	// 4
 #endif
				break;
				
			case PGM_PRT_SET_STATE:	// 3
 #ifndef USE_WEIGHT_BACKUP
				pgmUserSetMode = PGM_AUTOPOWER_SET_STATE;	// 5
 #else
				pgmUserSetMode = PGM_BACKUP_SET_STATE;	// 4
 #endif
				break;
 #ifdef USE_WEIGHT_BACKUP
			case PGM_BACKUP_SET_STATE:	// 4
				pgmUserSetMode = PGM_AUTOPOWER_SET_STATE;	// 5
				break;
 #endif						
			case PGM_AUTOPOWER_SET_STATE:	// 5
				pgmUserSetMode = PGM_BACKLIGHT_SET_STATE;	// 6
				break;
				
			case PGM_BACKLIGHT_SET_STATE:	// 6
				if(pgmBlOnOffFlag != 0)
				{
					pgmUserSetMode = PGM_BRIGHT_SET_STATE;	// 7
					break;
				}
				else
				{
 #ifdef USE_SALE_SIMPLE_HEALTH
					pgmUserSetMode = PGM_BLINK_SET_STATE;	// 8
 #else
  #ifdef USE_TARE_AUTO_CLEAR_SET
					pgmUserSetMode = PGM_TARE_AUTO_CLEAR_SET_STATE;	// 9
  #else
 					pgmUserSetMode = PGM_END_STATE;	// 10
  #endif
 #endif
 					break;
				}
				
			case PGM_BRIGHT_SET_STATE:	// 7
 #ifdef USE_SALE_SIMPLE_HEALTH
				pgmUserSetMode = PGM_BLINK_SET_STATE;	// 8
 #else
  #ifdef USE_TARE_AUTO_CLEAR_SET
				pgmUserSetMode = PGM_TARE_AUTO_CLEAR_SET_STATE;	// 9
  #else
  				pgmUserSetMode = PGM_END_STATE;	// 10
  #endif //USE_TARE_AUTO_CLEAR_SET
 #endif //USE_SALE_SIMPLE_HEALTH
 				break;
 #ifdef USE_SALE_SIMPLE_HEALTH
			case PGM_BLINK_SET_STATE:	// 8
  #ifdef USE_TARE_AUTO_CLEAR_SET
				pgmUserSetMode = PGM_TARE_AUTO_CLEAR_SET_STATE;	// 9
  #else
   				pgmUserSetMode = PGM_END_STATE;	// 10
  #endif	//USE_TARE_AUTO_CLEAR_SET
 #endif	//USE_SALE_SIMPLE_HEALTH
 				break;
 #ifdef USE_TARE_AUTO_CLEAR_SET
 			case PGM_TARE_AUTO_CLEAR_SET_STATE:	// 9
 				pgmUserSetMode = PGM_END_STATE;	// 10
 #endif	//USE_TARE_AUTO_CLEAR_SET
 			case PGM_END_STATE:	// 10
				DispSetConstStrMsg(WIN_WEIGHT, " SAVE");
				DispSetRunMsgPage(10, 1);
 #ifdef USE_WEIGHT_BACKUP
				DispFiltParam.zeroBufBackupEnable = pgmBackupOnOffFlag;
				DispFiltParam.zeroBufBackupValue = 0;
				DispFiltWriteParam();
 #endif
 				pgmPrintOffet = 0;
				pgmPrintOffet |= pgmPrintMethod;
 #ifdef USE_SIMPLE_PRT_SELECT
				pgmPrinterType |= (ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH) & 0xFC);
				pgmPrintOffet |= (pgmPrintBarcode << 3);
 #endif
				ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST, pgmPrintOffet);
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, pgmPrinterType);
				ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_STANDBY_TIME, pgmSuspendTime);
				ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_AUTO_OFF_TIME, pgmAutoOnOffFlag);
				ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_BL_OFF_TIME, pgmBlOnOffFlag);
				ParamWriteByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BL_BRIGHT, pgmBlBrightness);
 #ifdef USE_SALE_SIMPLE_HEALTH
				ParamWriteByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BLINK_COUNT, pgmBlinkCount);
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
				PowerInit();
				return MENU_RET_EXIT;
		}
	}
#endif  //USE_BL_MENU
	
 	if (key == KEY_TARE)
	{
		switch(pgmUserSetMode)
		{
#ifdef USE_SIMPLE_PRT_SELECT
			case PGM_PRT_SELECT_STATE:
				if (pgmPrinterType == PRT_NONE)
				{
					pgmPrinterType = PRT_DEP50;
				}
				else if(pgmPrinterType == PRT_DEP50)
				{
					pgmPrinterType = PRT_DLP50;
				}
				else
				{
					pgmPrinterType = PRT_NONE;
				}
				break;
				
			case PGM_PRT_BARCODE_SET_STATE:
				if(pgmPrintBarcode == 0)
				{
					pgmPrintBarcode = 1;
				}
				else
				{
					pgmPrintBarcode = 0;
				}
				break;	
#endif				
			case PGM_PRT_SET_STATE: 

#ifdef USE_NOT_UNSTABLE
				if (pgmPrintMethod < 2)
#else 
				if (pgmPrintMethod < 4)
#endif
				{
					pgmPrintMethod++;
				}
#ifdef USE_STREAM_CAS_22BYTE
				else if(pgmPrintMethod == 2 || pgmPrintMethod == 4)
				{
					pgmPrintMethod = 6;
				}
#endif
				else 
				{
					pgmPrintMethod = 0;
				}				
				break;	
			
#ifdef USE_WEIGHT_BACKUP
			case PGM_BACKUP_SET_STATE: 
				if (pgmBackupOnOffFlag == 0)
				{
					pgmBackupOnOffFlag = 1;
				}
				else 
				{
					pgmBackupOnOffFlag = 0;
				}
				break;	
#endif
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

#ifdef USE_BL_MENU 	//sg test	
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
 #ifdef USE_SALE_SIMPLE_HEALTH
			case PGM_BLINK_SET_STATE:		
				if (pgmBlinkCount != 1)
				{
					pgmBlinkCount -= 1;
				}
				else
				{
					pgmBlinkCount = 7;
				}					
				DispSetBlinkCount(pgmBlinkCount);
			break;	
#endif
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

		}
	}

 	if (key == KEY_HOLD)
	{
		switch(pgmUserSetMode)
		{

			default:
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




