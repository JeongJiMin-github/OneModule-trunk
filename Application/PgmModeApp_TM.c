/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Program Mode Applicaion Source File\n
* @file    PgmModeApp_TM.c
* @version 1.0
* @date    2012/10/10
* @author  TAE WOO KOO (KTW)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2012/10/10 created by KTW \n
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
#include "../Common/AdData.h"
#include "../SystemTimer/TimerApi.h"
#include "../Hardware/HardwareConfig.h"

#ifdef USE_PGM_TM
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
static INT8U pgmUnitSelect(KEY_TYPE key);
static INT8U pgmUserSetup(KEY_TYPE key);
static INT8U pgmHoldSelect(KEY_TYPE key);
static INT8U pgmFilterSetup(KEY_TYPE key);

#define MENU_PGM_MAX_POS 5
MENU code pgmMenuTable[MENU_PGM_MAX_POS] =
{
	{0x1000,	"PGM",	MENU_MODE_RUN,			pgmExitFunc},
#ifdef USE_UNIT_TON_NEWTON_DAN_KN
	{0x1F00, 	"UNIT", MENU_MODE_NORMAL, 		pgmUnitSelect},
#endif
	{0x1F10, 	"AP", 	MENU_MODE_NORMAL, 		pgmUserSetup},
	{0x1F20, 	"HL", 	MENU_MODE_NORMAL, 		pgmHoldSelect},
	{0x1F30, 	"FILT", MENU_MODE_NORMAL, 		pgmFilterSetup}
};

#define MENU_DIRECT_PGM_MAX_POS 4
DIRECT_MENU code pgmDirectMenuTable[MENU_DIRECT_PGM_MAX_POS] =
{
	{KEY_UNIT_CHANGE,	0x1F00},
    {KEY_ZERO,	0x1F10},
	{KEY_HOLD,	0x1F20},
	{KEY_TARE,	0x1F30}
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
	0, 10, 20, 30
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
	static INT8U tempInt8u;
	static INT16U pgmHighWeightThres;
	static INT16U pgmLowWeightThres;
    static INT8U commModuleChannel;
 	INT32U 	tempInt32u; 
    INT8U  i;

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
        case 0x02:                  //Weightbackup SET
            #ifdef USE_WEIGHT_BACKUP
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
            #endif
            break;
		case 0x03:                  //KeyLock SET     
            tempInt8u = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_KEY_LOCK);		
			switch (tempInt8u)
			{
				default:
					tempInt8u = 0;
				case 0:
					DispSetStr(MenuDataWinNum, "ULOCK");
					break;
				case 1:
					DispSetStr(MenuDataWinNum, " LOCK");
					break;
			}
			break;	
		
		case 0x04:
			tempInt8u = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST);		
			switch (tempInt8u)
			{
				default:
                case 0:
                    DispSetStr(MenuDataWinNum, "C1-NO");
                    break;
                case 1:
                    DispSetStr(MenuDataWinNum, "C1-CO");
                    break;
				case 2:
					DispSetStr(MenuDataWinNum, "C1-5T");
					break;
			}
			break;
        case 0x05:
            tempInt8u = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_EURO_DEC);	
            switch (tempInt8u)
            {
                default:
                case 0:
                DispSetStr(MenuDataWinNum, "C2-NO");
                break;
                case 1:
                DispSetStr(MenuDataWinNum, "C2-CO");
                break;
                case 2:
                DispSetStr(MenuDataWinNum, "C2-5T");
                break;
            }
        break;
		case 0x06:
            tempInt8u = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_LF_SIZE);
            switch (tempInt8u)
			{
				default:
                case 0:
                     DispSetStr(MenuDataWinNum, "OP-NO");
                    break;
                case 1:
                    DispSetStr(MenuDataWinNum, "OP-TU");
                    break;
				case 2:
					DispSetStr(MenuDataWinNum, "OP-ZB");
					break;
			}
            ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_LF_SIZE, tempInt8u);
            break;
        case 0x07:
            tempInt8u = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_LF_SIZE);
            if(tempInt8u == 1)//Bluetooth reset
            {
                DispSetStr(MenuDataWinNum, "RESET");
                if(BtResetFlag == 0)
                {					
                    Btreset();
                    BtResetFlag = 1;
                }
                if((BtResetFlag==1) && (BtOnFlag==1))
                { 						
                    Btmodeconversion();
                    BtResetFlag = 2;
                }
            }
            else if(tempInt8u == 2)    //X-bee channel setting
            {
                commModuleChannel = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_BARCODE_TYPE);
                if ((0 > commModuleChannel) || (11 < commModuleChannel)) commModuleChannel = 0;
                DispSetConstStrWithDecNum(MenuDataWinNum, "CH-", commModuleChannel, 2);	  
            }
            else    MenuFuncState++;    
            break;
		case 0x08:
			tempInt8u = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_DEVICE_NUM);
			if ((0 > tempInt8u) || (9 < tempInt8u)) tempInt8u = 1;
			DispSetConstStrWithDecNum(MenuDataWinNum, "ID- ", tempInt8u, 1);
			break;		
		case 0x09:
			if ((key != KEY_HOLD) && (key != KEY_UNIT_CHANGE))
			{
			  	pgmHighWeightThres = ParamReadWord(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MAX_WEIGHT);//High 값을 읽어와서 보여주기위함
			  	//if(CalParam.weightDecPos == 1)   pgmHighWeightThres = pgmHighWeightThres/10;
                DispSetConstStrMsg(WIN_WEIGHT, " HI  ");
				DispSetRunMsgPage(20, 1);
				EditorSetDataAndRun(WIN_WEIGHT, DEC, pgmHighWeightThres, 0, 99999, 0, DEC_TYPE_DOT, 0);
			}
			break;
			
		case 0x0A:
			if((key != KEY_HOLD) && (key != KEY_UNIT_CHANGE))
			{
				pgmLowWeightThres = ParamReadWord(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MIN_WEIGHT);//Low 값을 읽어와서 보여주기위함
				//if(CalParam.weightDecPos == 1)   pgmLowWeightThres = pgmLowWeightThres/10;
                DispSetConstStrMsg(WIN_WEIGHT, " LOW ");
				DispSetRunMsgPage(20, 1);
				EditorSetDataAndRun(WIN_WEIGHT, DEC, pgmLowWeightThres, 0, 99999, 0, DEC_TYPE_DOT, 0);	
			}
			break;
	}
	if (key == KEY_UNIT_CHANGE)
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

		else if(MenuFuncState == 0x02)	  //Weight Backup Set
		{   
            if (tempInt8u != 1)
            {
                tempInt8u += 1;
            }
            else 
            {
                tempInt8u = 0;
            }
            #ifdef USE_WEIGHT_BACKUP
                DispFiltParam.zeroBufBackupEnable = tempInt8u;
                DispFiltParam.zeroBufBackupValue = 0;
                DispFiltWriteParam();
                AdInit();
            #endif
		}
        
        else if(MenuFuncState == 0x03)	  //Key Lock Set
		{   
            if (tempInt8u != 1)
            {
                tempInt8u += 1;
            }
            else 
            {
                tempInt8u = 0;
            }
            ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_KEY_LOCK,tempInt8u);
            SaleModeInit();
		}

		else if(MenuFuncState == 0x04)		//UART 0 SET(CO,TU,ST)
		{
			if (tempInt8u < 2)
			{
				tempInt8u++;
			}
			else 
			{
				tempInt8u = 0;
			}
			ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST, tempInt8u);
            CommInit();
		}
        else if(MenuFuncState == 0x05)		//UART 1 SET(CO,TU,ST)
		{
			if (tempInt8u < 2)
			{
				tempInt8u++;
			}
			else 
			{
				tempInt8u = 0;
			}
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_EURO_DEC, tempInt8u);
            CommInit();
		}
        else if(MenuFuncState == 0x06)		//Optinon Set(Bluetooth/X-bee)
		{
			if (tempInt8u  < 2)
			{
				tempInt8u++;
			}
			else 
			{
				tempInt8u = 0;
			}
            ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_LF_SIZE, tempInt8u);
		}
        else if(MenuFuncState == 0x07)		//Optinon Set(Bluetooth reset / X-bee channel set)
		{
			if (commModuleChannel < 11)
			{
				commModuleChannel += 1;
			}
			else 
			{
				commModuleChannel = 0;
			}
			ParamWriteByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_BARCODE_TYPE, commModuleChannel);
		}
		else if(MenuFuncState == 0x08)		//ID Set
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
            CommInit();
		}
		else if(MenuFuncState == 0x09)		//pgmHighWeightThres Set
		{
            ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MAX_WEIGHT, pgmHighWeightThres);
		}
		else if(MenuFuncState == 0x0A)		//pgmLowWeightThres Set
		{
            ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MIN_WEIGHT, pgmLowWeightThres);
		}
	}
	if (key == KEY_ZERO)
	{
        if (MenuFuncState <= 0x0A)
		{
			MenuFuncState++;
		}
        else
        {
            MenuFuncState = 0;
        }
        
	}
	if (key == KEY_HOLD)
	{		
		if(MenuFuncState == 0x07)
        {                                                                                                           
            tempInt8u = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_LF_SIZE);
            if(tempInt8u == 1)
            {
                if(BtResetFlag==2)
                {
                    DispSetStr(MenuDataWinNum, " OK  ");
                    DispSetRunMsgPage(20, 1);
                    MenuFuncState++; 
                }
                else 
                {
                    DispSetConstStrMsg(WIN_WEIGHT, "RETRY");
                    DispSetRunMsgPage(20, 1);
                }		
            }
            else if(tempInt8u == 2)
            { 
                XbeeModuleInit();
                XbeeModeCmdOk();
                XbeeModuleSetchannel();
                XbeeModeCmdOk();
                XbeeModuleWriteCommand();
                XbeeModeCmdOk();
                //XbeeModuleChannelReadCommand();
                XbeeModuleExitCommand();
                XbeeModeCmdOk();
                if(BtErrFlag)   //채널 설정이 실패한 경우
                {
                    DispSetConstStrMsg(WIN_WEIGHT, " err ");
                    DispSetRunMsgPage(20, 1);
                    BuzzerSetOnTimes(10);
                    DispProc(); 
                    BtErrFlag = 0;
                    return MENU_RET_INCOMPLETE;
                }
                else    //채널 설정이 완료 된 경우
                {
                    BtErrFlag = 0;
                    MenuFuncState++;
                }
            }
            else    MenuFuncState++;
        }
        else if(MenuFuncState == 0x09)
		{
            EditorGetData(&tempInt32u); 
            if(CalParam.weightDecPos == 0)   
            {
                if (tempInt32u > CalParam.capa)
                {
                    tempInt32u = 0;
                    for(i=0; i<10; i++)
                    {
                        DispSetConstStrMsg(WIN_WEIGHT, " err ");
                        DispSetRunMsgPage(20, 1);
                        BuzzerSetOnTimes(10);
                        DispProc();
                    }
                    EditorStop();	
                    return MENU_RET_INCOMPLETE;
                }
                pgmHighWeightThres = (INT32U)tempInt32u;
            }
            else                                        // 소수점이 있는 경우 ...
            {
                if (tempInt32u > CalParam.capa)
                {
                    tempInt32u = 0;
                    for(i=0; i<10; i++)
                    {
                        DispSetConstStrMsg(WIN_WEIGHT, " err ");
                        DispSetRunMsgPage(20, 1);
                        BuzzerSetOnTimes(10);
                        DispProc();
                    }
                    EditorStop();	
                    return MENU_RET_INCOMPLETE;
                }
                pgmHighWeightThres = (INT32U)tempInt32u;
            }
            MenuFuncState++;
            //pgmHighWeightThres = (INT32U)tempInt32u;
            ParamWriteWord(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MAX_WEIGHT, pgmHighWeightThres);//상한값 저장
		}
		else
		{
            pgmHighWeightThres = ParamReadWord(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MAX_WEIGHT);
			EditorGetData(&tempInt32u);
            if (((tempInt32u <= CalParam.divFactor)||(tempInt32u >= pgmHighWeightThres))&&(tempInt32u != 0))
			{
				tempInt32u = 0;
                for(i=0; i<10; i++)
                {
                    DispSetConstStrMsg(WIN_WEIGHT, " err ");
                    DispSetRunMsgPage(20, 1);
                    BuzzerSetOnTimes(10);
                    DispProc();
                }
                EditorStop();	
                return MENU_RET_INCOMPLETE;
			}
            EditorStop();	
			pgmLowWeightThres = (INT32U)tempInt32u;	
			ParamWriteWord(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MIN_WEIGHT, pgmLowWeightThres);	
		    return MENU_RET_EXIT;
        }
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
	if (key == KEY_UNIT_CHANGE)
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
			PowerSaverInit();			
			AdInit();
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
INT8U pgmUnitSelect(KEY_TYPE key)
{
  	VERSION version;
	INT8U tempUnitOption;
	INT8U tempInt8u;
  
	ParamReadData(PARAM_VERSION_AREA_NUM, 0, (INT8U *)&version, sizeof(version));
	if(version.countryNum == KOREA)
	{
	  	AdData.weightUnit = UNIT_KG;	//TM 국내 kg 단위만 사용 가능
		return MENU_RET_EXIT;  
	}
	
//#ifdef USE_UNIT_TON_NEWTON_DAN_KN    

    tempUnitOption = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_EXPANDED_WEIGHT_UNIT);		

    switch (MenuFuncState)
	{
		case 0x00:	//matric ton
			if (tempUnitOption & EXPAND_UNIT_MASK_TON) tempInt8u = 1;
            else tempInt8u = 0;
			DispSetConstStrWithDecNum(MenuDataWinNum, "MT- ", tempInt8u, 1);            
            break;
		case 0x01:	//short ton
            if (tempUnitOption & EXPAND_UNIT_MASK_LONG_TON)                     //short ton과 longton을 동시에 사용 하지 않기 떄문.
            {
                tempInt8u = 0;
                //DispSetConstStrWithDecNum(MenuDataWinNum, "    ", tempInt8u, 1);
                tempUnitOption = tempUnitOption & ~(EXPAND_UNIT_MASK_SHORT_TON);
                ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_EXPANDED_WEIGHT_UNIT, tempUnitOption);
                MenuFuncState++;
            }
            else
            {
                if (tempUnitOption & EXPAND_UNIT_MASK_SHORT_TON) tempInt8u = 1;
                else tempInt8u = 0;
                DispSetConstStrWithDecNum(MenuDataWinNum, "ST- ", tempInt8u, 1);            
            }
            break;
		case 0x02:	//long ton                                                  //short ton과 longton을 동시에 사용 하지 않기 떄문.
            if (tempUnitOption & EXPAND_UNIT_MASK_SHORT_TON)
            {
                tempInt8u = 0;
                //DispSetConstStrWithDecNum(MenuDataWinNum, "    ", tempInt8u, 1);
                tempUnitOption = tempUnitOption & ~(EXPAND_UNIT_MASK_LONG_TON);
                ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_EXPANDED_WEIGHT_UNIT, tempUnitOption);
                MenuFuncState++;  
            }
            else
            {
                if (tempUnitOption & EXPAND_UNIT_MASK_LONG_TON) tempInt8u = 1;
                else tempInt8u = 0;
                DispSetConstStrWithDecNum(MenuDataWinNum, "LT- ", tempInt8u, 1);            
            }
            break;
		case 0x03:	//newton
			if (tempUnitOption & EXPAND_UNIT_MASK_NEWTON) tempInt8u = 1;
            else tempInt8u = 0;
			DispSetConstStrWithDecNum(MenuDataWinNum, " N- ", tempInt8u, 1);            
            break;
		case 0x04:	//kN
			if (tempUnitOption & EXPAND_UNIT_MASK_KN) tempInt8u = 1;
            else tempInt8u = 0;
			DispSetConstStrWithDecNum(MenuDataWinNum, "KN- ", tempInt8u, 1);            
            break;
		case 0x05:	//daN
			if (tempUnitOption & EXPAND_UNIT_MASK_DAN) tempInt8u = 1;
            else tempInt8u = 0;
			DispSetConstStrWithDecNum(MenuDataWinNum, "DA- ", tempInt8u, 1);            
            break;            
    }
    
	if (key == KEY_UNIT_CHANGE)
	{
		if (MenuFuncState != 0x05)
		{
            MenuFuncState++;
		}
		else
		{
            MenuFuncState = 0x00; 
		}
	}
    
	if (key == KEY_ZERO)
	{		
        if (tempInt8u == 0) tempInt8u = 1;
        else tempInt8u = 0;

        switch (MenuFuncState)
        {
            case 0x00:	//matric ton 
              if (tempInt8u == 0) tempUnitOption = tempUnitOption & ~(EXPAND_UNIT_MASK_TON);
              else tempUnitOption = tempUnitOption | EXPAND_UNIT_MASK_TON;
              break;
            case 0x01:	//short ton 
              if (tempInt8u == 0) tempUnitOption = tempUnitOption & ~(EXPAND_UNIT_MASK_SHORT_TON);
              else tempUnitOption = tempUnitOption | EXPAND_UNIT_MASK_SHORT_TON;
              break; 
            case 0x02:	//long ton 
              if (tempInt8u == 0) tempUnitOption = tempUnitOption & ~(EXPAND_UNIT_MASK_LONG_TON);
              else tempUnitOption = tempUnitOption | EXPAND_UNIT_MASK_LONG_TON;
              break;
            case 0x03:	//newton 
              if (tempInt8u == 0) tempUnitOption = tempUnitOption & ~(EXPAND_UNIT_MASK_NEWTON);
              else tempUnitOption = tempUnitOption | EXPAND_UNIT_MASK_NEWTON;
              break;
            case 0x04:	//kN
              if (tempInt8u == 0) tempUnitOption = tempUnitOption & ~(EXPAND_UNIT_MASK_KN);
              else tempUnitOption = tempUnitOption | EXPAND_UNIT_MASK_KN;
              break;
            case 0x05:	//daN
              if (tempInt8u == 0) tempUnitOption = tempUnitOption & ~(EXPAND_UNIT_MASK_DAN);
              else tempUnitOption = tempUnitOption | EXPAND_UNIT_MASK_DAN;
              break;              
        }
        ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_EXPANDED_WEIGHT_UNIT, tempUnitOption);
    }
    
	if(key == KEY_HOLD)
	{		
		AdData.weightUnit = CalParam.calUnit;//사용 단위설정을 한 후에는 calibration 설정 단위에서 시작함.
        return MENU_RET_EXIT;	
	}
	return MENU_RET_INCOMPLETE; 
//#endif
}

#endif		//#ifdef USE_PGM_CASTON
