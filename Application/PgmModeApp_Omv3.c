/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Driver Source File\n
           Chip : 
* @file    PgmModeApp_Omv3.c
* @version 1.0
* @date    2012/09/19
* @author  Deok Hyun Kim (KDH)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2012/09/19 created by KDH \n
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

#ifdef USE_PGM_OMV3
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

#ifdef USE_RS232
#define PGM_PRT_SET_STATE 					1
#endif

#ifdef USE_WEIGHT_ONLY_ECR
#define PGM_ECR_SET_STATE					2    //SW ECR Mode
#endif

#define PGM_AUTOPOWER_SET_STATE 			3

#ifdef USE_SUSPEND_MODE
#define PGM_SUSPEND_SET_STATE 				4
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
/** @brief bit 1 & bit 0, printer type, 00 : Don't Use, 01: Use DEP, 10: Use DLP, 11: reserve*/
static INT8U pgmPrinterType;
/** @brief Printer 관련 Offset */
static INT8U pgmPrintOffet;
/** @brief Auto on/off flag */
static INT8U pgmAutoOnOffFlag;
/** @brief Printer method */
static INT8U pgmPrintMethod;
/** @brief ECR Printer Type */
static INT8U pgmECRType;
/** @brief Default setting on/off flag */
static INT8U pgmDefaultOnOffFlag;
/** @brief Suspend Time */
static INT8U pgmSuspendTime;
/** @brief Default setting on/off flag */
static INT8U pgmDefaultOnOffFlag;



/** @brief Program Menu Table 개수*/
#define MENU_PGM_MAX_POS 3
/** @brief Program Menu Table*/
MENU code pgmMenuTable[MENU_PGM_MAX_POS] =
{
	{0x1000, "PGM",	MENU_MODE_RUN, 		pgmExitFunc},
	{0x1F10, "AP", 	MENU_MODE_NORMAL, 		pgmUserSetup},
	{0x1F20, "FILT", 	MENU_MODE_NORMAL, 		pgmFilterSetup}
};

/** @brief Program Direct Menu Table 개수*/
#define MENU_DIRECT_PGM_MAX_POS 2
/** @brief Program Direct Menu Table */
DIRECT_MENU code pgmDirectMenuTable[MENU_DIRECT_PGM_MAX_POS] =
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
			pgmPrintMethod = pgmPrintOffet;
			//pgmPrinterType -> 00 : Not use, 01 : DEP50, 10 : DLP50, 11 : RESERVED
			pgmPrinterType = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH) & 0x03;
			//pgmPrintBarcode = (pgmPrintOffet & 0x08) >> 3;
			//pgmPrintLfSize = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_LF_SIZE);
			//pgmUserSetMode++;
			pgmAutoOnOffFlag = ParamReadByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_AUTO_OFF_TIME);
			pgmSuspendTime = ParamReadByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_STANDBY_TIME);//ok
			pgmECRType= ParamReadByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_ECR_TYPE);			
			break;
			
#ifdef USE_RS232
		case PGM_PRT_SET_STATE:
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
 			
#endif
	#ifdef USE_WEIGHT_ONLY_ECR		//Only ECR Mode
			
		case PGM_ECR_SET_STATE:
			pgmPrinterType = PRT_NONE;
			pgmPrintMethod = 0;			
//			DispSetStr(MenuDataWinNum, "ECR");				
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
//				case 7:
//					DispSetStr(MenuDataWinNum, "TYP 7");
//					break;
//				case 8:
//					DispSetStr(MenuDataWinNum, "TYP 8");
//					break;
//				case 9:
//					DispSetStr(MenuDataWinNum, "TYP 9");
//					break;
//				case 10:
//					DispSetStr(MenuDataWinNum, "TYP10");
//					break;
//				case 11:
//					DispSetStr(MenuDataWinNum, "TYP11");
//					break;
			}
			break;
	#endif
/*			if (key == MenuKeyMenuUp)
			{
				ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_ECR_TYPE, pgmPrintMethod);
				MenuFuncState++;
			}
			else if (key == MenuKeyEnter)
			{
				pgmPrintMethod++;
				pgmPrintMethod %= 12;
			}
*/


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
#endif

		//	}
		/*	else if(pgmPrinterType == PRT_DLP50)
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
					DispSetStr(MenuDataWinNum, "BC	ON");
				}
				else
				{
					DispSetStr(MenuDataWinNum, "BC OFF");
				}
			}
			break;

		default:
			break;*/
	}
/*
#ifdef USE_SUSPEND_MODE	
 	if (key == KEY_ZERO)
	{
		//pgmUserSetMode = PGM_PRT_SET_STATE;
		if(pgmUserSetMode == PGM_INIT_STATE) 
		{
			pgmUserSetMode++;
			return MENU_RET_INCOMPLETE;
		}
		if(pgmUserSetMode !=  PGM_SUSPEND_SET_STATE) pgmUserSetMode++;
		else
		{
			DispSetConstStrMsg(WIN_WEIGHT, " SAVE");
			DispSetRunMsgPage(10, 1);
			//pgmPrintOffet = 0;
			pgmPrintOffet = pgmPrintMethod;
			pgmPrinterType |= (ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH) & 0xFC);
			ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST, pgmPrintOffet);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, pgmPrinterType);
			ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_STANDBY_TIME, pgmSuspendTime);
			ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_AUTO_OFF_TIME, pgmAutoOnOffFlag);
		}
 	}
 */
// #else

	if (key == KEY_ZERO)
	{
		if(pgmUserSetMode == PGM_INIT_STATE) 
		{
	#ifdef USE_RS232
			pgmUserSetMode++;
			return MENU_RET_INCOMPLETE;
	#elif defined(USE_WEIGHT_ONLY_ECR)
			pgmUserSetMode=pgmUserSetMode+2;
			return MENU_RET_INCOMPLETE;
	#else 
			pgmUserSetMode=pgmUserSetMode+3;	//RS-232, ECR Mode 모두 아닌 경우 Auto Power Set Mode로 진입
			return MENU_RET_INCOMPLETE;
	#endif			
		}
		if(pgmUserSetMode == 1)		//RS-232 Mode일 경우 Auto Power Set Mode로 진입
		{
		 	pgmUserSetMode++;
		}
#ifdef USE_SUSPEND_MODE	
		if(pgmUserSetMode != PGM_SUSPEND_SET_STATE) 
		{
			pgmUserSetMode++;
		}
#else
		if(pgmUserSetMode != PGM_AUTOPOWER_SET_STATE) 
		{
			pgmUserSetMode++;
		}
#endif
		else
		{
			DispSetConstStrMsg(WIN_WEIGHT, " SAVE");
			DispSetRunMsgPage(10, 1);
			//pgmPrintOffet = 0;
			pgmPrintOffet = pgmPrintMethod;
			pgmPrinterType |= (ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH) & 0xFC);
			ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST, pgmPrintOffet);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, pgmPrinterType);
			ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_STANDBY_TIME, pgmSuspendTime);
			ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_AUTO_OFF_TIME, pgmAutoOnOffFlag);
			ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_ECR_TYPE, pgmECRType);	
// #endif

#ifdef USE_PRINTER
			PrtInit();
#elif defined(USE_WEIGHT_ONLY_ECR)
			CommInit();
#endif
			PowerInit();
//			SaleModeInit();
			return MENU_RET_EXIT;
		
		}
 	}	
	
 	if (key == KEY_TARE)
	{
		switch(pgmUserSetMode)
		{
			/*case PGM_PRT_SELECT_STATE:
				if(pgmPrinterType != PRT_DLP50) pgmPrinterType++;
				else pgmPrinterType = PRT_NONE;
				break;*/
#ifdef USE_RS232				
			case PGM_PRT_SET_STATE: 
			//	if(pgmPrinterType == PRT_DEP50)
			//	{
				if (pgmPrintMethod != 5)
				{
					pgmPrintMethod++;
				}
				else 
				{
					pgmPrintMethod = 0;
				}
			//	}
				break;	
#elif defined(USE_WEIGHT_ONLY_ECR)
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
				else 
				{
					pgmECRType = 0;
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
/*					else if(pgmPrinterType == PRT_DLP50)
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
			default:
				break;*/
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



