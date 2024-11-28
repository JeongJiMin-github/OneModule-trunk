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
#include "../Communication/CommApi.h"
#include "../Application/SaleModeApp.h"
#include "../Communication/PrtApi.h"
#include "../AD/AdApi.h"
#include "../SystemTimer/TimerApi.h"
#include "../Hardware/HardwareConfig.h"

#ifdef USE_PGM_HD
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
static INT8U pgmHoldSelect(KEY_TYPE key);

static INT8U pgmFilterSetup(KEY_TYPE key);

/** @brief Program Menu Table 개수*/
#define MENU_PGM_MAX_POS 4
/** @brief Program Menu Table*/
const MENU code pgmMenuTable[MENU_PGM_MAX_POS] = 
{
	{0x1000, "PGM",	MENU_MODE_RUN, 		pgmExitFunc},
	{0x1F10, "AP", 	MENU_MODE_NORMAL, 		pgmUserSetup},
	{0x1F20, "HL", 	MENU_MODE_NORMAL, 		pgmHoldSelect},
	{0x1F30, "FILT", 	MENU_MODE_NORMAL, 		pgmFilterSetup}
};

/** @brief Program Direct Menu Table 개수*/
#define MENU_DIRECT_PGM_MAX_POS 3
/** @brief Program Direct Menu Table */
const DIRECT_MENU code pgmDirectMenuTable[MENU_DIRECT_PGM_MAX_POS] = 
{
	{KEY_ZERO, 0x1F10 },
	{KEY_HOLD, 0x1F20 },
	{KEY_TARE, 0x1F30 }
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


/** 
* @brief Auto Power Off Mode 시간 Setting Table (단위 : min) 
* @remark 0-always on, 10,30,60 - delay min.
*/
#define HOLD_TYPE_TABLE_SIZE	3
const INT8U code HoldTypeTable[HOLD_TYPE_TABLE_SIZE] = 
{
	0, 1, 2
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
//	INT8U 	retValue;
	static INT8U tempInt8u;
	static INT16U zb16uEditData;	 
 	INT32U 	tempInt32u;
	INT8U	endMenuCnt = 2;

	switch (MenuFuncState)
	{
		case 0x00:					// Auto Power OFF SET
#ifdef REVISION_NUM
			if(!TimerCheckDelay100ms(TIMER_DELAY_100MS_INITIAL))
			{
				DispSetMsgWithNum("r", REVISION_NUM);
				TimerSetDelay100ms(TIMER_DELAY_100MS_INITIAL, 10);//include disp. revision 10
				while (!TimerCheckDelay100ms(TIMER_DELAY_100MS_INITIAL));	//wait key
			}
#endif
			tempInt8u = ParamReadByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_AUTO_OFF_TIME);		
			zb16uEditData = ParamReadWord(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PCS_VALUE);	//Pan ID Addr
			switch (tempInt8u)
			{
				default:
					tempInt8u = 0;
				case 0:
				case 10:
				case 20:
				case 30:
					break;
			}
			DispSetConstStrWithDecNum(MenuDataWinNum, "AP-", tempInt8u, 2);
			break;

		case 0x01:					//Sleep Mode SET    
			tempInt8u = ParamReadByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_STANDBY_TIME);		
			switch (tempInt8u)
			{
				default:
					tempInt8u = 0;
				case 0:
				case 20:
				case 40:
				case 60:
					break;
			}
			DispSetConstStrWithDecNum(MenuDataWinNum, "SB-", tempInt8u, 2);
			break;
			
		case 0x02:
			tempInt8u = ParamReadByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BL_BRIGHT);		 
			if (1 > tempInt8u && 5 < tempInt8u) tempInt8u = 3;
			DispSetConstStrWithDecNum(MenuDataWinNum, "BR- ", tempInt8u, 1);
			break;				
	
		case 0x03:
			tempInt8u = DispFiltParam.zeroBufBackupEnable;		
			switch (tempInt8u)
			{
				default:
					tempInt8u = 0;
				case 0:
					DispSetStr(MenuDataWinNum, "BA-OF");
					break;
				case 1:
					DispSetStr(MenuDataWinNum, "BA-ON");
					break;
			}
			break;
		
		case 0x04:
			tempInt8u = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST);		
			switch (tempInt8u)
			{
				
				default:
					tempInt8u = 1;
				case 3:
					DispSetStr(MenuDataWinNum, "C0-5T");
					break;
				case 1:
					BtErrFlag=0;
					DispSetStr(MenuDataWinNum, "C0-TU");

					if(BtResetFlag == 0)
					{					
						Btreset();
						BtResetFlag = 1;
					}

					if(BtResetFlag==1 && BtOnFlag==1)
					{ 						
						Btmodeconversion();
						BtResetFlag = 2;
					}
				   	if(BtResetFlag==2 && BtOnFlag==1) 
					{
						BtErrFlag = 1;		
					}
					else 
					{
						BtErrFlag = 2;
					}

					break;
				case 2:
					DispSetStr(MenuDataWinNum, "C0-CO");
					break;
			}
			break;
			
		case 0x05:
			tempInt8u = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_DEVICE_NUM);
			if (0 > tempInt8u && 9 < tempInt8u) tempInt8u = 1;
			DispSetConstStrWithDecNum(MenuDataWinNum, "ID- ", tempInt8u, 1);
			break;

		case 0x06:
			tempInt8u = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_RESERVED);		
			switch (tempInt8u)
			{
				default:
					tempInt8u = 0;
				case 2:
					DispSetStr(MenuDataWinNum, "OP-BT");
					break;
				case 1:
					DispSetStr(MenuDataWinNum, "OP-ZB");

					break;
				case 0:
					DispSetStr(MenuDataWinNum, "OP-NO");
					break;
			}
			break;

		case 0x07:
			tempInt8u = ParamReadByte(PARAM_SALE_AREA_NUM, PRT_NONE);		
			switch (tempInt8u)
			{
				case 1:
					DispSetStr(MenuDataWinNum, "CH-11");
					break;
				case 2:
					DispSetStr(MenuDataWinNum, "CH-12");
					break;
				case 3:
					DispSetStr(MenuDataWinNum, "CH-13");
					break; 
				case 4:
					DispSetStr(MenuDataWinNum, "CH-14");
					break; 
				case 5:
					DispSetStr(MenuDataWinNum, "CH-15");
					break;
				case 6:
					DispSetStr(MenuDataWinNum, "CH-16");
					break;
				case 7:
					DispSetStr(MenuDataWinNum, "CH-17");
					break; 
				case 8:
					DispSetStr(MenuDataWinNum, "CH-18");
					break; 
				case 9:
					DispSetStr(MenuDataWinNum, "CH-19");
					break;
				case 10:
					DispSetStr(MenuDataWinNum, "CH-20");
					break;
				case 11:
					DispSetStr(MenuDataWinNum, "CH-21");
					break;
				case 12:
					DispSetStr(MenuDataWinNum, "CH-22");
					break; 
				case 13:
					DispSetStr(MenuDataWinNum, "CH-23");
					break; 
				case 14:
					DispSetStr(MenuDataWinNum, "CH-24");
					break; 
				case 15:
					DispSetStr(MenuDataWinNum, "CH-25");
					break; 
				case 16:
					DispSetStr(MenuDataWinNum, "CH-26");
					break; 
				default:
					tempInt8u = 5;
			}
			break;
		
		case 0x08:
			if (key != KEY_HOLD)
			{
				DispSetConstStrMsg(WIN_WEIGHT, "PANID");
				DispSetRunMsgPage(20, 1);
				EditorSetDataAndRun(WIN_WEIGHT, DEC, zb16uEditData, 1, 9999, 0, DEC_TYPE_DOT, 0);
			}
			break;					
		 
		/*
		//overload check
		case 0x09:
			//tempInt8u = DispFiltParam.overLoadCount;
			tempInt8u = ParamReadByte(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_OVERLOAD_COUNT);
			if(tempInt8u == 0xFF) tempInt8u = 0;
			DispSetConstStrWithDecNum(MenuDataWinNum, "OL", tempInt8u, 2);		
			break;
		*/				

	}
	if (key == KEY_ZERO)
	{
		if (MenuFuncState == 0x00)		// Auto Power OFF SET
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
		else if(MenuFuncState == 0x01)	  //Sleep Mode SET
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
			DispFiltWriteParam();	
		}
		else if(MenuFuncState == 0x02)		//Bright Set
		{
			if (tempInt8u != 5)
			{
				tempInt8u += 1;
			}
			else 
			{
				tempInt8u = 1;
			}
			ParamWriteByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BL_BRIGHT, tempInt8u);
		}
		else if(MenuFuncState == 0x03)	  //Weight Backup Set
		{
			if (tempInt8u != 1)
			{
				tempInt8u += 1;
			}
			else 
			{
				tempInt8u = 0;
			}
			DispFiltParam.zeroBufBackupEnable = tempInt8u;
			DispFiltParam.zeroBufBackupValue = 0;
			DispFiltWriteParam();
			AdInit();
		}

		else if(MenuFuncState == 0x04)		//Communication Set(CO,TU,ST)
		{
			if (tempInt8u != 3)
			{
				tempInt8u += 1;
			}
			else 
			{
				tempInt8u = 1;
			}
			ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST, tempInt8u);
		}

		else if(MenuFuncState == 0x05)		//ID Set
		{
			if (tempInt8u != 9)
			{
				tempInt8u += 1;
			}
			else 
			{
				tempInt8u = 0;
			}
			ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_PARAM_DEVICE_NUM, tempInt8u);
		}

		else if(MenuFuncState == 0x06)		//무선통신방법 설정
		{
			if (tempInt8u != 2)
			{
				tempInt8u += 1;
			}
			else 
			{
				tempInt8u = 0;
			}
			ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_RESERVED, tempInt8u);
		}
		else if(MenuFuncState == 0x07)		//Zigbee Channel Set
		{
			
			if (tempInt8u != 16)
			{
				tempInt8u += 1;
			}
			else 
			{
				tempInt8u = 1;
			} 
			ParamWriteByte(PARAM_SALE_AREA_NUM, PRT_NONE, tempInt8u);
		}
	
		else if(MenuFuncState == 0x08)		//Zigbee PANID
		{
			zb16uEditData = 0;
			EditorStop();
		} 
	}
	if (key == KEY_HOLD)
	{
		MenuFuncState++;
		if(MenuFuncState > endMenuCnt)
			return MENU_RET_EXIT;	
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
INT8U pgmHoldSelect(KEY_TYPE key)
{
	INT8U tempInt8u;

	tempInt8u = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_HOLD_TYPE);		
	switch (tempInt8u)
	{
		case 0:
		default:
			DispSetStr(MenuDataWinNum, "HL-00");
			break;
		case 1:
			DispSetStr(MenuDataWinNum, "HL-01");
			break;
		case 2:
			DispSetStr(MenuDataWinNum, "HL-02");
			break;
        case 3:
			DispSetStr(MenuDataWinNum, "HL-03");
			break;

	}
	if (key == KEY_ZERO)
	{
		if (tempInt8u != 3)
		{
			tempInt8u += 1;
		}
		else 
		{
			tempInt8u = 0;
		}
		ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_HOLD_TYPE,tempInt8u);		
	}
	if (key == KEY_HOLD)
	{
		return MENU_RET_EXIT;	
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
#endif		//#ifdef USE_PGM_HD
