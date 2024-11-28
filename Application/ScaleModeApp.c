/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Scale Mode Applicaion Source File\n
* @file    ScaleModeApp.c
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
#include "../Common/AdData.h"
#include "../Common/UiDefine.h"
#include "../Common/RingBuf.h"
#include "../Common/TimeData.h"
#include "../Common/CommonLib.h"
#include "../DB/StorageApi.h"
#include "../DB/ParamApi.h"
#include "../UI/DispApi.h"
#include "../UI/BuzzerApi.h"
#include "../UI/KeyApi.h"
#include "../UI/EditorApi.h"
#include "../UI/MenuApi.h"
#include "../AD/AdApi.h"
#include "../Power/PowerApi.h"
#include "../SystemTimer/TimerApi.h"
#include "../Communication/CommApi.h"
#include "../Communication/PrtApi.h"
#include "../Communication/DebugApi.h"
#include "../Time/TimeApi.h"
#include "ScaleModeApp.h"
#include "SaleModeApp.h"
#include "CalModeApp.h"
#include "PgmModeApp.h"
#ifdef USE_CTS_MODE
#include "CtsModeApp.h"
#endif
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief 현재 Scale Mode  (SCALE_MODE_SALE, SCALE_MODE_PGM, SCALE_MODE_CAL)*/
SCALE_MODE_TYPE ScaleMode;

/** @brief 현재 Scale parameter*/
SCALE_PARAMETER ScaleParam;
#ifdef USE_CTS_MODE
/** @brief EEPROM에 저장된 CTS 데이터의 버퍼. CtsModeApp에서 해당 데이터 참조하여 사용*/
INT8U CtsStructBuf[80];
#endif
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
#if defined(USE_WEIGHT_ONLY_ECR) || defined(USE_PDN_ECR)
INT8U Pd2ECRProtocol;
#endif
#ifdef USE_CAL_ENTER_FLAG
static INT8U calEnterFlag;
#endif
/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
static void scaleModeInitParam(void);
static void scaleModeSetMenu(SCALE_MODE_TYPE smode);
static INT8U scaleModeSetDirectMenu(SCALE_MODE_TYPE smode, KEY_TYPE key);
static void scaleModeCtsInit(void);
static void scaleModeCtsAlarm(BOOLEAN usekey);

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
/**
********************************************************************************
* @brief    Scale Mode Application Parameter를 default 값으로 초기화\n
* @param    type : defualt type
* @return   none
* @remark   Parameter 영역에 초기값을 넣는다.\n
*           Cal Mode나 Program 모드에서 전체 초기화할 때 실행한다.
********************************************************************************
*/
void ScaleModeSetDefaultParam(INT8U type)
{
	VERSION version;
        
	if(type == DEFAULT_AD_SET)
	{
		AdSetDefaultParam(type);
		return;
	}
	else if(type == DEFAULT_KEY_SET)
	{
		KeySetDefaultParam();
		return;
	}
#ifdef USE_ER_PLUS_M_TURKEY_KEY_DEFINE
		if(type == TURKEY)
		{
			KeyDrvSetTurkeyFuncTable();
		}
#endif

	BuzzerSetDefaultParam();
	CommSetDefaultParam();
	DispSetDefaultParam();
	PowerSetDefaultParam();
#ifdef USE_PRINTER
	PrtSetDefaultParam();
#endif
	SaleModeSetDefaultParam(type);
	TimeSetDefaultParam();

	version.modelNum = MODEL_NUM;
	version.countryNum = type;
	switch (version.countryNum)
	{
		case RUSSIA:
			version.verNum = VERSION_NUM_RUSSIA;
			break;
		case SOUTH_AFRICA:
			version.verNum = VERSION_NUM_SOUTH_AFRICA;
			break;
		default:
			version.verNum = VERSION_NUM;
	}
	version.fwErasePwd = 0;
	ParamWriteData(PARAM_VERSION_AREA_NUM, 0, (INT8U *)&version, sizeof(version));
	
#ifdef USE_CAL_ENTER_FLAG
	if(version.countryNum == JAPAN)
		calEnterFlag = 1;
	else
		calEnterFlag = 0;		
#endif

#ifdef USE_UNITEDSTATES_DEFAULT
	if (version.countryNum == NTEP)
		CommInit();	//ECR TYPE4 설정 후 CommInit
#endif

#ifdef USE_CTS_MODE_OLD
    CtsSetDefaultParam(type);
#endif
}

/**
********************************************************************************
* @brief    version확인 하여 초기화 필요하면 초기화\n
* @param    none
* @return   restart flag
* @remark   
********************************************************************************
*/
BOOLEAN ScaleModeCheckVersion(void)
{
	VERSION version;
	INT16U buf;

	ParamReadData(PARAM_VERSION_AREA_NUM, 0, (INT8U *)&version, sizeof(version));

	switch(version.countryNum)
	{
		case RUSSIA:
			buf = VERSION_NUM_RUSSIA;
			break;
		case SOUTH_AFRICA:
			buf = VERSION_NUM_SOUTH_AFRICA;
			break;
		default:
			buf = VERSION_NUM;
	}

	if (version.modelNum != MODEL_NUM) 
	{
		DispSetConstStrMsg(WIN_WEIGHT, "INIT");
		DispSetRunMsgPage(5,1);
		ScaleModeSetDefaultParam(OIML);
		return 1;
	}
	else if (version.verNum != buf) //correct version after f/w download
	{
		version.verNum = VERSION_NUM;
		version.fwErasePwd = 0;
		ParamWriteData(PARAM_VERSION_AREA_NUM, 0, (INT8U *)&version, sizeof(version));
	}
	return 0;
}

#ifdef USE_CHINA_LIMIT_CAL
INT32U china_calMonth;

INT8U ScaleModeLimitChina()
{
	INT8U  tempInt8u;
	INT32U curMonth;
	INT8U  callimit;

	callimit = 0;
	tempInt8u = (INT8U)ParamReadByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_VALID_START_DATE);
	if (tempInt8u != 0xff && tempInt8u != 0)
	{
		china_calMonth = ((INT32U)tempInt8u) * 10000l;
		tempInt8u = (INT8U)ParamReadByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_VALID_START_DATE + 1);
		china_calMonth += ((INT32U)tempInt8u) * 100l;
		tempInt8u = (INT8U)ParamReadByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_VALID_START_DATE + 2);
		china_calMonth += ((INT32U)tempInt8u);
		TimeReadData();
		curMonth = ((INT32U)TimeData.year) * 10000l;
		curMonth += ((INT32U)TimeData.month) * 100l;
		curMonth += ((INT32U)TimeData.date);
		if (curMonth > china_calMonth) callimit = 1;
	}
	return callimit;
}
#endif

/**
********************************************************************************
* @brief    Scale Mode Application Init 함수\n
*           Calibration Key Check하여 Calibratio Mode 진입\n
*           Sale and Menu Application 초기화\n
*           Display Test and Check AD initial Zero\n
* @param    smode : 다음에 실행할 모드
* @return   none
* @remark   초기에 한번 실행 되는 함수\n
********************************************************************************
*/
const INT8U code calEnterTable[8] = 
{
	KEY_TARE,
	KEY_ZERO,
	KEY_TARE,
	KEY_ZERO,
	KEY_TARE,
	KEY_TARE,
	KEY_ZERO,
	KEY_ZERO
};

const INT8U code initZeroResetTable[6] = 
{
	KEY_ZERO,
	KEY_TARE,
	KEY_ZERO,
	KEY_TARE,
	KEY_ZERO,
	KEY_TARE
};

void ScaleModeInit(void)
{
	INT8U initialState;
	INT8U skipDispTest;
	KEY_TYPE key;
	INT8U initZeroResetState;

#ifdef USE_RTC
	INT8U tempInt8u;
	INT16U calMonth;
	INT16U curMonth;
#endif
	INT8U calEnterState;
#ifdef USE_CTS_MODE_OLD
    CAL_HISTORY_STRUCT calHistory;
    INT8U counter = 0;
#endif
	VERSION version;
	INT8U countryStr[3];

	ParamReadData(PARAM_VERSION_AREA_NUM, 0, (INT8U *)&version, sizeof(version));

	calEnterState = 0;

	initZeroResetState = 0;

	scaleModeInitParam();
	MenuInit();

	initialState = 0;
	skipDispTest = 0;//TODO : Change eeprom param & need test
	
	TimerSetDelay100ms(TIMER_DELAY_100MS_INITIAL, 3);
	while (!TimerCheckDelay100ms(TIMER_DELAY_100MS_INITIAL));	//wait key
	TimerSetDelay100ms(TIMER_DELAY_100MS_INITIAL, 0);	//Timer Stop

	DispClearAll();
#ifdef USE_CTS_MODE
	scaleModeCtsInit();
#endif 
	if (KeyCheckCalPort())
	{
		scaleModeSetDirectMenu(SCALE_MODE_CAL, KEY_CAL);
		DispSetConstStrMsg(WIN_WEIGHT, " ONE");
		DispSetRunMsgPage(10,1);
		BuzzerSetOnTimes(1);
	}
	else
	{
		scaleModeSetMenu(SCALE_MODE_PGM);
		ScaleMode = SCALE_MODE_SALE;
		if (KeyCheck())
		{
#ifdef USE_LONG_KEY
			KeyLongKeyDisable();
			key = KeyGetFuncCode(KEY_MODE_NORMAL);
			KeyLongKeyEnable();
			scaleModeSetDirectMenu(SCALE_MODE_PGM, key);
#else
			key = KeyGetFuncCode(KEY_MODE_NORMAL);
			scaleModeSetDirectMenu(SCALE_MODE_PGM, key);
#endif
		}
		else
		{
			BuzzerSetOnTimes(1);
		}
	}

	PowerSaverResetTimer(0xff);

	while (1)
	{
#if defined (USE_AC_POWER) || defined (USE_AC_BATT_POWER)
		if(PowerRestartFlag) 
		{
			initialState = 0;
			DispTurnOff(); //for PowerReset of AD,AP
			return;
		}
#endif						
		if (ScaleMode == SCALE_MODE_SALE)
		{
#ifdef USE_CHINA_LIMIT_CAL
			if (ScaleModeLimitChina()) {
				DispSetConstStr(WIN_WEIGHT, "-CAL-");
				DispSetConstStr(WIN_UNIT_PRICE, "--REQ");
				DispSetNum(WIN_TOTAL_PRICE,china_calMonth,'0');
				BuzzerSetOnTimes(2);
				goto SALEHALT;
			}
#endif

			if (TimerCheckDelay100ms(TIMER_DELAY_100MS_INITIAL) || (initialState == 0))
			{
				if (initialState == 13)
				{
					TimerSetDelay100ms(TIMER_DELAY_100MS_INITIAL, 5);
					DispClearAll();
#ifdef USE_AUTO_POWER_SETTING
					if(AutoPowerSetting)
					{
						AutoPowerSettingProc();
					}
#endif
					if (AdData.stableFlag) 
					{
						if (AdCheckInitZero())
						{
							TimerSetDelay100ms(TIMER_DELAY_100MS_INITIAL, 0);//Timer Stop
							DispSetRunMsgPage(1, 0);
							SaleModeInit(); //move to initialState == 0
							AdData.initZeroError = FALSE;

#ifdef USE_RTC
#ifndef USE_CHINA_LIMIT_CAL
#ifdef USE_CTS_MODE_OLD
                            if (ParamReadByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_CAL_PASSWORD_ENABLE_KEY) == 0)       // cts enable check
                                break;
#elif defined(USE_CTS_MODE)
                            if (CtsCheckExpired((INT8U *)(&TimeData)))
                            {
                                scaleModeCtsAlarm(OFF);
                            }
#endif
							tempInt8u = ParamReadByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_VALID_START_WAIT_COUNT);
							if (tempInt8u == 0)
							{
								tempInt8u = ParamReadByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_CAL_WARNNING_PERIOD);
								if (tempInt8u != 0xff && tempInt8u != 0)
								{
#ifdef USE_CTS_MODE_OLD
                                    counter = ParamReadByte(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_HISTORY_COUNT);
                                    counter = counter % CAL_HISTORY_MAX_NUM;
                                    ParamReadData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_HISTORY + counter * sizeof(calHistory), (INT8U *)&calHistory, sizeof(calHistory));

									TimeReadData();
									calMonth = 12 * calHistory.year + calHistory.month;  //month
									curMonth = 12 * TimeData.year + TimeData.month;
#else
									TimeReadData();
									calMonth = 12 * ParamReadByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_VALID_START_DATE) //year
									              + ParamReadByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_VALID_START_DATE + 1);  //month
									curMonth = 12 * TimeData.year + TimeData.month;
#endif
									if ((curMonth < calMonth) || ((curMonth - calMonth) > (INT16U)tempInt8u))
									{
#ifdef USE_CTS_ON_INDICATOR_OLD
                                        CalWarringOn();
#else
										DispSetConstStrMsg(WIN_WEIGHT, "-CAL-");
										DispSetRunMsgPage(10, 3);
										BuzzerSetOnTimes(3);
#endif
									}
								}
							}
							else if (tempInt8u < 0xff)
							{
								tempInt8u--;
								ParamWriteByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_VALID_START_WAIT_COUNT, tempInt8u);
								if (tempInt8u == 0)
								{
									TimeReadData();
									ParamWriteByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_VALID_START_DATE, TimeData.year);
									ParamWriteByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_VALID_START_DATE + 1, TimeData.month);
									ParamWriteByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_VALID_START_DATE + 2, TimeData.date);
								}	
							}
#endif
#endif
							break;
						}
						else
						{
							DispSetError(ERROR_NUM_INIT_ZERO);
							AdData.initZeroError = TRUE;

							KeyEnable(ON);	
							if (KeyCheck())
							{
								key = KeyGetFuncCode(KEY_MODE_NORMAL);
								if (initZeroResetTable[initZeroResetState] == key)
								{
									initZeroResetState++;
								}
								else
								{
									initZeroResetState = 0;								
								}
								if (initZeroResetState == 6) 
								{	
									AdInitZeroReset();	
									initZeroResetState = 0;
								}
							}
						}
					}
					else
					{
						DispSetError(ERROR_NUM_UNSTABLE);
						KeyEnable(ON);
						if (KeyCheck())
						{
							key = KeyGetFuncCode(KEY_MODE_NORMAL);
						}
					}
				}
				else
				{
					if (skipDispTest == 1)
					{
						DispTest(8);
						TimerSetDelay100ms(TIMER_DELAY_100MS_INITIAL, 20);
						initialState = 10;
					}
					else
					{
						if (initialState == 0)
						{
							switch (version.countryNum)
							{
								case SOUTH_AFRICA:
									countryStr[0]='Z';
									countryStr[1]='A';
									countryStr[2]=0;
									break;
								//case RUSSIA:
									//countryStr[0]='V';
									//countryStr[1]=' ';
									//countryStr[2]=0;
									//break;
								case GUATEMALA:
									countryStr[0]='G';
									countryStr[1]='T';
									countryStr[2]=0;
									break;
								default:
									countryStr[0]='v';
									countryStr[1]=' ';
									countryStr[2]=0;
									break;
							}
#ifdef USE_VERSION_STR_DOT
							DispSetVersionWithDot(VERSION_NUM);	//인증서 버전표시에 국가 버전 없음
#else
							if(*VERSION_STR != *countryStr) DispSetMsgWithNum(VERSION_STR, VERSION_NUM);	//국가설정 외 국가버전 있는 경우 해당 국가버전 표시
							else DispSetMsgWithNum(countryStr, version.verNum);
#endif
#ifdef USE_SLOVAKIA_PROTOCOL
							DispSetConstStrMsg(WIN_TOTAL_PRICE, "SK");
#endif
#ifdef USE_CAL_SPAN_EXT_LIMIT_PR_PLUS
							DispSetConstStrMsg(WIN_TOTAL_PRICE, "ESPAN");
#endif
							TimerSetDelay100ms(TIMER_DELAY_100MS_INITIAL, 10);//include disp. version 10
						}
						else if (initialState == 11)
						{
							DispTest(8);
							TimerSetDelay100ms(TIMER_DELAY_100MS_INITIAL, 20);
#ifndef EDITION_STR
							initialState++;// goto 13
#endif
						}
#ifdef EDITION_STR
						else if (initialState == 12)
						{
							DispSetConstStrMsg(WIN_WEIGHT, EDITION_STR);
							DispSetRunMsgPage(20,1);
							TimerSetDelay100ms(TIMER_DELAY_100MS_INITIAL, 20);
						}
#endif						
						else
						{
#ifdef USE_AUTO_POWER_SETTING
							if(AutoPowerSetting)
							{
								if ( initialState == 1)
								{
									PowerSaverSetBlOffTime1s(1);
								}
							}
#endif
							DispTest(initialState-1);
							TimerSetDelay100ms(TIMER_DELAY_100MS_INITIAL, 5);
						}

						initialState++;
						KeyEnable(ON);	
						
						if (KeyCheck())
						{
							key = KeyGetFuncCode(KEY_MODE_NORMAL);
							if (calEnterTable[calEnterState] == key)
							{
#ifdef USE_CAL_ENTER
								calEnterState++;
#elif defined(USE_CAL_ENTER_FLAG)
								if(calEnterFlag)
									calEnterState++;
								else
									calEnterState = 0;
#else
								calEnterState=0;
#endif
							}
							else
							{
								calEnterState = 0;								
								if (key == KEY_ZERO)
								{
									DispTest(8);
									TimerSetDelay100ms(TIMER_DELAY_100MS_INITIAL, 20);
									initialState = 12;
#ifndef EDITION_STR
									initialState++;// goto 13
#endif
								}
								else if (key == KEY_TARE)
								{
									KeyResetPressedFlag();	//why???
								}
							}
							if(calEnterState == 8) 
							{		
								DispTest(9);
								DispSetConstStrMsg(WIN_WEIGHT, " ONE");
								DispSetRunMsgPage(10,1);
								BuzzerSetOnTimes(1);
								scaleModeSetDirectMenu(SCALE_MODE_CAL, KEY_CAL);
								calEnterState = 0;
								initialState = 0;
							}
						}
					}//if (skipDispTest == 1)
				}
			}

			if (initialState >= 13)
			{
				AdProc(1, 0);
			}
			else
			{
				AdProc(0, 0);
			}
		}
		else
		{
			AdProc(0, 1);
#ifdef USE_CTS_MODE_OLD
            CtsModeCalProc();
#endif
			ScaleModeProc();
			if (ScaleMode == SCALE_MODE_SALE)
			{
				if (KeyCheckCalPort())
				{
					scaleModeSetMenu(SCALE_MODE_CAL);
				}
			}
		}
#ifdef USE_CHINA_LIMIT_CAL
SALEHALT:
		AdProc(1, 0);
#endif
		PowerProc();
		CommProc();
#ifdef USE_PRINTER
		PrtProc();
#endif
		DispProc();
		EditorProc();
	}
}

/**
********************************************************************************
* @brief    Scale Mode Application Procedure 함수\n
* @param    none
* @return   none
* @remark   Editor 동작중이 아니면 Scale Mode에 따라 관련 Procedure 실행\n
*           Scale Mode 변경 시 관련 Init 실행
*           일정 시간에 한번 계속 실행되는 함수\n
********************************************************************************
*/
void ScaleModeProc(void)
{	
	SCALE_MODE_TYPE retScaleMode;
	INT8U retMenuProc;

	if (EditorCheckStatus() == EDITOR_RUN)
	{
		return;		
	}

	retScaleMode = ScaleMode;

	switch (ScaleMode)
	{
		case SCALE_MODE_SALE:
#ifdef USE_CHINA_LIMIT_CAL
			if (ScaleModeLimitChina()) {
				DispSetConstStr(WIN_WEIGHT, "-CAL-");
				DispSetConstStr(WIN_UNIT_PRICE, "--REQ");
				DispSetNum(WIN_TOTAL_PRICE,china_calMonth,'0');
				BuzzerSetOnTimes(2);
				return;
			}
#endif
			retScaleMode = SaleModeProc();
			break;
		case SCALE_MODE_PGM:
		case SCALE_MODE_CAL:
#ifdef USE_CTS_MODE
          	if(PowerRestartFlag)
          	{
              	ScaleMode = SCALE_MODE_SALE;
              	return;
          	}
#endif
			PowerSaverResetTimer(TIMER_DELAY_100MS_STAND_BY);
			retMenuProc = MenuProc();
			if (retMenuProc == MENU_RET_EXIT)
			{
				TimerSetDelay100ms(TIMER_DELAY_100MS_CAL, 0);//stop timer
				retScaleMode = SCALE_MODE_SALE;
			}
			else if (retMenuProc == MENU_RET_CAL_MODE)
			{
				retScaleMode = SCALE_MODE_CAL;
				scaleModeSetMenu(retScaleMode);
			}
			break;
	}

	if (retScaleMode != ScaleMode)
	{
		scaleModeSetMenu(retScaleMode);
	}
}

#ifdef USE_CTS_MODE
/**
********************************************************************************
* @brief   CtsAlarmProc함수\n
*           
* @param    none
* @return   none
* @remark   CTS 교정기간 만료시 오후 3시에 알람 메세지가 울릴수 있도록 확인하는 함수
*           
********************************************************************************
*/  
void ScaleModeCtsProc(void)
{
	static INT8U data ctsAlarmRun = OFF;
	static INT8U data lastAlarmDate = 0xFF;
	
	TimeReadData();
	switch (CtsCheckExpired((INT8U *)(&TimeData)))
	{
		case CTS_NEED_UPDATE:
			StorageWriteData(STORAGE_CTS_DATA_BASE_ADDR, CtsStructBuf, CTS_STRUCT_SIZE);
		case CTS_EXPIRED:
			if(TimeData.hour == 0x0F && TimeData.min == 0x00 && TimeData.sec < 0x05 && 
			   lastAlarmDate != TimeData.date )
			{
				ctsAlarmRun = ON;
			}
			if(ctsAlarmRun && TimerCheckDelay100ms(TIMER_DELAY_100MS_CTS_LAST_USE_KEY) == 1
				&& TimerCheckDelay100ms(TIMER_DELAY_100MS_CTS_LAST_LOAD_AD) == 1) 
			{
				scaleModeCtsAlarm(ON);
				lastAlarmDate = TimeData.date;
				ctsAlarmRun = OFF;
			}
			break;
		default:
			break;
	}
}
#endif

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
/**
********************************************************************************
* @brief    Scale Parameter Initial 함수  <= E2 Rom에서 읽어 오는 값임 
* @param    none
* @return   none
* @remark   초기에 한번 실행 되는 함수\n
********************************************************************************
*/

void scaleModeInitParam(void)
{
	INT8U	readParamDataInScale;
#if defined (USE_WEIGHT_ONLY_ECR) || defined (USE_UNDERLOAD_10DIGIT)
	VERSION version;
#endif
	readParamDataInScale = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND); 
	
	ScaleParam.scaleUseBL = (readParamDataInScale & 0x20) >> 5;

	DispBlSetUseFlag(ScaleParam.scaleUseBL);

#ifdef USE_UNDERLOAD_10DIGIT
	ParamReadData(PARAM_VERSION_AREA_NUM, 0, (INT8U *)&version, sizeof(version));

	if(version.countryNum == JAPAN) 
	{
		UnderLoad10dEnable = 1;
	}
	else
	{
		UnderLoad10dEnable = 0;
	}
#endif

#ifdef USE_WEIGHT_ONLY_ECR
	ParamReadData(PARAM_VERSION_AREA_NUM, 0, (INT8U *)&version, sizeof(version));
	if(version.countryNum == AUSTRALIA)
	{
		NoUseTarekey = 0;
		Pd2ECRProtocol = 1;
	}
	else
	{
  #ifdef NOT_USE_TARE_KEY
  		NoUseTarekey = 1;
  #else
		NoUseTarekey = 0;
  #endif
  #ifdef USE_ECR_WEIGHT_UNIT_CAPITAL
  		Pd2ECRProtocol = 1;
  #else
		Pd2ECRProtocol = 0;
  #endif
	}
#elif defined(USE_PDN_ECR)
 #ifdef NOT_USE_TARE_KEY
  #ifdef COUNTRY_UNITEDSTATES
	// PDN - 국가셋팅 미국으로 설정 시 Tare Key 사용안함
	if(version.countryNum == NTEP)
		NoUseTarekey = 1;
	else
		NoUseTarekey = 0;
  #else
 		NoUseTarekey = 1;
  #endif
 #else
	NoUseTarekey = 0;
 #endif
#endif
}

/**
********************************************************************************
* @brief    Menu Mode Setting 함수\n
* @param    smode : 실행할 모드
* @return   none
* @remark   Menu Procedure에서 설정된 Menu 실행
********************************************************************************
*/
void scaleModeSetMenu(SCALE_MODE_TYPE smode)
{
	ScaleMode = smode;
	switch (smode)
	{
		case SCALE_MODE_SALE:
			SaleModeInit();
			break;
		case SCALE_MODE_PGM:
			MenuSetMode(MENU_PGM_ROOT_NUM, MENU_STATE_MENU, PgmModeGetMenuTable(), PgmModeGetMenuTableMaxPos());
			PgmModeInit();
			break;
		case SCALE_MODE_CAL:	
			MenuSetMode(MENU_CAL_ROOT_NUM, MENU_STATE_MENU, CalModeGetMenuTable(), CalModeGetMenuTableMaxPos());
			CalModeInit();
			break;
	}
}

/**
********************************************************************************
* @brief    Power On menu Mode Setting 함수\n
* @param    smode : 실행할 모드
*			key : 초기 Key 값
* @return   모드와 키에 따른 Menu Setting됨 = 1, 안됨 = 0
* @remark   Menu Procedure에서 설정된 Menu 실행
********************************************************************************
*/
INT8U scaleModeSetDirectMenu(SCALE_MODE_TYPE smode, KEY_TYPE key)
{
	INT8U i;
	INT8U max;
	DIRECT_MENU code *dMenu;

	if (smode == SCALE_MODE_PGM)
	{	
		dMenu = PgmModeGetDirectMenuTable();
		max = PgmModeGetDirectMenuTableMaxPos();
		for (i = 0; i < max; i++)
		{
			if (key == dMenu[i].menuKey)
			{
				MenuSetMode(dMenu[i].menuNum, MENU_STATE_RUN, PgmModeGetMenuTable(), PgmModeGetMenuTableMaxPos());
				PgmModeInit();
				ScaleMode = SCALE_MODE_PGM;
				return 1;
			}
		}
	} 
	else if (smode == SCALE_MODE_CAL)
	{
		if (key == KEY_CAL)
		{
			MenuSetMode(MENU_CAL_CHECK_PWD_NUM, MENU_STATE_RUN, CalModeGetMenuTable(), CalModeGetMenuTableMaxPos());
			CalModeInit();
			ScaleMode = SCALE_MODE_CAL;
			return 1;
		}
	}
	return 0;
}

#ifdef USE_CTS_MODE
/**
********************************************************************************
* @brief   Scale mode CTS init \n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/  
void scaleModeCtsInit(void)
{
	INT8U ctsUpdateBuf[16];
	CAL_HISTORY_STRUCT calHistory;
	INT8U counter = 0;
	VERSION version;
	
	counter = ParamReadByte(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_HISTORY_COUNT);
	counter = counter % CAL_HISTORY_MAX_NUM;
	ParamReadData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_HISTORY + counter * sizeof(calHistory), (INT8U *)&calHistory, sizeof(calHistory));
	MakeStrCopy(ctsUpdateBuf, MODEL_NAME_STR, sizeof(MODEL_NAME_STR));
	#ifdef USE_CTS_OMV2
	reverse_array((INT8U *)&ctsUpdateBuf,sizeof(INT32U));
	reverse_array((INT8U *)&ctsUpdateBuf+sizeof(INT32U),sizeof(INT32U));
	reverse_array((INT8U *)&ctsUpdateBuf+(sizeof(INT32U)*2),sizeof(INT32U));
	#endif
	CtsUpdateScaleData(0, ctsUpdateBuf);	// 0 Model name
	ParamReadData(PARAM_VERSION_AREA_NUM, 0, (INT8U *)&version, sizeof(version));
	*((INT32U*)ctsUpdateBuf) = version.verNum;
	CtsUpdateScaleData(1, ctsUpdateBuf);	// 1 Scale version
	*((INT32U*)ctsUpdateBuf) = calHistory.calZeroValue;
	CtsUpdateScaleData(2, ctsUpdateBuf);	// 2 Zero
	*((INT32U*)ctsUpdateBuf) = calHistory.calMidUpValue;
	CtsUpdateScaleData(3, ctsUpdateBuf);	// 3 Mid up
	*((INT32U*)ctsUpdateBuf) = calHistory.calMidDownValue;
	CtsUpdateScaleData(4, ctsUpdateBuf);	// 4 Mid dn
	*((INT32U*)ctsUpdateBuf) = calHistory.calSpanValue;
	CtsUpdateScaleData(5, ctsUpdateBuf);	// 5 Span
	*((INT16U*)ctsUpdateBuf) = CalParam.calG;
	CtsUpdateScaleData(6, ctsUpdateBuf);	// 6 Factory gravity factor
	*((INT16U*)ctsUpdateBuf) = CalParam.usingG;
	CtsUpdateScaleData(7, ctsUpdateBuf);	// 7 Local gravity factor
	*(INT32U*)ctsUpdateBuf = CalParam.capa;
	CtsUpdateScaleData(8, ctsUpdateBuf);	// 8 CAPA
	CtsUpdateScaleData(9, &CalParam.divFactor);	// 9 e
	CtsUpdateScaleData(10, &CalParam.divFactor);	// 10 d
	CtsUpdateScaleData(11, &CalParam.weightDecPos);	// 11 Weight decimal point
	*ctsUpdateBuf = 0;
	CtsUpdateScaleData(12, ctsUpdateBuf);	// 12 CAL state - 0.판매모드
	CLMemSet(ctsUpdateBuf, 0, sizeof(ctsUpdateBuf));
	switch (CalParam.calUnit)
	{
#ifdef USE_CTS_OMV2
		case 0:
			MakeStrCopy(ctsUpdateBuf, "gk", 2);
			break;
		case 1:
			MakeStrCopy(ctsUpdateBuf, "bl", 2);
			break;
		case 2:
			MakeStrCopy(ctsUpdateBuf, "zo", 2);
			break;
		case 3:
			MakeStrCopy(ctsUpdateBuf, " g", 2);
			break;
		case 4:
			MakeStrCopy(ctsUpdateBuf, " t", 2);
			break;			
#else
		case 0:
			MakeStrCopy(ctsUpdateBuf, "kg", 2);
			break;
		case 1:
			MakeStrCopy(ctsUpdateBuf, "lb", 2);
			break;
		case 2:
			MakeStrCopy(ctsUpdateBuf, "oz", 2);
			break;
		case 3:
			MakeStrCopy(ctsUpdateBuf, "g", 1);
			break;
		case 4:
			MakeStrCopy(ctsUpdateBuf, "t", 1);
			break;
#endif			
	}
	CtsUpdateScaleData(13, ctsUpdateBuf);	// 13 Unit

	StorageReadData(STORAGE_CTS_DATA_BASE_ADDR, CtsStructBuf, CTS_STRUCT_SIZE);
	if (CtsModeInit(CtsStructBuf))
	{
		StorageWriteData(STORAGE_CTS_DATA_BASE_ADDR, CtsStructBuf, CTS_STRUCT_SIZE);
	}
	
	TimeReadData();
	if (CtsCheckExpired((INT8U *)(&TimeData)) == CTS_NEED_UPDATE)
	{
		StorageWriteByte(STORAGE_CTS_DATA_BASE_ADDR + CTS_STORAGE_OFFSET_EXPIRED, CtsStructBuf[CTS_STORAGE_OFFSET_EXPIRED]);
		StorageWriteByte(STORAGE_CTS_DATA_BASE_ADDR + CTS_STORAGE_OFFSET_BCC, CtsStructBuf[CTS_STORAGE_OFFSET_BCC]);
	}
	
	TimerSetDelay100ms(TIMER_DELAY_100MS_CTS_LAST_USE_KEY,100);
	TimerSetDelay100ms(TIMER_DELAY_100MS_CTS_LAST_LOAD_AD,100);	
}

/**
********************************************************************************
* @brief   CtsAlarm함수\n
*           
* @param    none
* @return   none
* @remark   CTS 교정기간 만료 알람을 울리도록 한다.
*           
********************************************************************************
*/  
void scaleModeCtsAlarm(BOOLEAN usekey)
{
#ifdef CTS_ONE_DP 
	DispSetConstStrMsg(WIN_WEIGHT, "CTSCAL");
#elif  defined(CTS_ONE_DP_UNDER6)
    DispSetConstStrMsg(WIN_WEIGHT, " CTS ");    
#else
	DispSetConstStrMsg(WIN_WEIGHT, "PLEASE");
	DispSetConstStrMsg(WIN_UNIT_PRICE, "CTSCAL");
#endif
	DispSetRunMsgPage(10, 3);
	BuzzerSetOnTimes(3);
	if (usekey == ON)
	{
		KeyEnable(ON);
		DispSetMsgStop();
	}
}
#endif