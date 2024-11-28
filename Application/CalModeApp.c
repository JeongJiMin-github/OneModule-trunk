/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Calibration Mode Applicaion Source File\n
* @file    CalModeApp.c
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
#include "../Common/RingBuf.h"
#include "../Common/CommonLib.h"
#include "../Common/UiDefine.h"
#include "../Common/TimeData.h"
#include "../DB/StorageApi.h"
#include "../DB/ParamApi.h"	
#include "../UI/BuzzerApi.h"
#include "../UI/DispApi.h"
#include "../UI/KeyDrv.h"
#include "../UI/KeyApi.h"
#include "../UI/EditorApi.h"
#include "../UI/MenuApi.h"
#include "../AD/AdApi.h"
#if defined(USE_TEMP_COMP) || defined(USE_TEMP_COMP_MWIIH)
#include "../AD/TempCompApi.h"
#endif
#include "../SystemTimer/TimerApi.h"
#include "../Communication/PrtApi.h"
#include "../Communication/CommApi.h"
#include "../Communication/DebugApi.h"
#include "../Time/TimeApi.h"
#include "../Power/PowerApi.h"

#include "ScaleModeApp.h"
#include "saleModeApp.h"
#include "CalModeApp.h"
#ifdef USE_CTS_MODE
#include "CtsModeApp.h"
#endif
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
/** @brief KR : 무게 Calibration 시 Stable 기다리는 최대 횟수\n
*		  US : \n
*		  CN :									*/
#define CAL_CHECK_COUNT	5

/** @brief Hysteresis 보상 Calibration Mode*/
#define CAL_PROC_HYST		0
/** @brief 선형 보상 Calibration Mode*/
#define CAL_PROC_LINEAR		1
/** @brief SPAN Calibration Mode*/
#define CAL_PROC_SPAN 		2
/** @brief Percent Calibration Mode*/
#define CAL_PROC_PERCENT	3
/** @brief Weight Calibration Mode for CASTON*/
#define CAL_PROC_WEIGHT		4
/** @brief Temp Compesation Calibration Mode*/
#define CAL_PROC_TEMP		5


/** @brief 100%*/
#define CAL_PERCENT_100 	100

/** @brief CAL6 menu tree Last index*/
#ifdef USE_AUTO_TARE_FUNCTION
#define CAL6_KEY_END_INDEX		3
#else
#define CAL6_KEY_END_INDEX		2
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

static INT8U calExitFunc(KEY_TYPE key);
static INT8U cal1Func(KEY_TYPE key);
static INT8U cal2Func(KEY_TYPE key);
static INT8U cal3Func(KEY_TYPE key);

static INT8U cal41Func(KEY_TYPE key);
static INT8U cal42Func(KEY_TYPE key);
static INT8U cal43Func(KEY_TYPE key);
static INT8U cal44Func(KEY_TYPE key);
static INT8U cal45Func(KEY_TYPE key);
#ifdef USE_TEMP_COMP_MWIIH
static INT8U cal46Func(KEY_TYPE key);
#endif
static INT8U cal5Func(KEY_TYPE key);
static INT8U cal6Func(KEY_TYPE key);
static INT8U cal71Func(KEY_TYPE key);
static INT8U cal72Func(KEY_TYPE key);
static INT8U cal8Func(KEY_TYPE key);
static INT8U cal91Func(KEY_TYPE key);
static INT8U cal92Func(KEY_TYPE key);
static INT8U cal101Func(KEY_TYPE key);
static INT8U cal102Func(KEY_TYPE key);
static INT8U cal103Func(KEY_TYPE key);
static INT8U cal104Func(KEY_TYPE key);
static INT8U cal105Func(KEY_TYPE key);
static INT8U cal106Func(KEY_TYPE key);
static INT8U cal107Func(KEY_TYPE key);

static INT8U cal11Func(KEY_TYPE key);
#ifdef USE_CREEP_MENU
 #ifdef USE_CREEP_TIME
static INT8U cal121Func(KEY_TYPE key);
static INT8U cal122Func(KEY_TYPE key);
 #else
static INT8U cal12Func(KEY_TYPE key);
 #endif
#endif
#ifdef USE_CHINA_LIMIT_CAL
static INT8U cal13Func(KEY_TYPE key);
#endif
#ifdef TURKEY_BLOCK_FUNCTION_KEY
static INT8U cal14Func(KEY_TYPE key);
#endif
#ifdef USE_TEMP_COMP_MWIIH
static INT8U cal15Func(KEY_TYPE key);
#endif
#ifdef USE_ZERO_TRACKING_RANGE_SETTING
static INT8U cal15Func(KEY_TYPE key);
#endif
static INT8U calCheckPwdFunc(KEY_TYPE key);

/** @brief Calibration Menu Table 개수 */
#if defined (USE_TEMP_COMP_MWIIH) && defined (USE_CREEP_MENU) && defined (USE_CREEP_TIME)
#define MENU_CAL_MAX_POS 34
#elif defined (USE_CHINA_LIMIT_CAL) || defined (USE_CREEP_MENU) || defined (TURKEY_BLOCK_FUNCTION_KEY) || defined (USE_ZERO_TRACKING_RANGE_SETTING)
#define MENU_CAL_MAX_POS 30
#else
#define MENU_CAL_MAX_POS 29
#endif

/** 
* @brief Calibration Menu Table
* @remark
* Menu 추가시 MENU_CAL_MAX_POS를 증가시키고 CalMenuTable에 추가하시오\n
* menuNum은 같은 level에서는 연속으로 증가 시키시오.\n
* 예)   0x8110, 0x8120, 0x8130 다음 메뉴 이면 0x8140에 추가\n
* 예)   0x8100, 0x8200, 0x8300 다음 메뉴 이면 0x8400에 추가\n
* 예)   0x8500 하위 메뉴이면 0x8510에 추가(단, 0x8500의 Menu Mode는 MENU_MODE_MENU이여야함)
*/
#ifdef IAR
    MENU code calMenuTable[MENU_CAL_MAX_POS] =
#else
    const MENU code calMenuTable[MENU_CAL_MAX_POS] =
#endif
{
	{0x8000, "CAL", MENU_MODE_RUN, calExitFunc},
	{0x8100, "CAL1", MENU_MODE_NORMAL, cal1Func},
	{0x8200, "CAL2", MENU_MODE_NORMAL, cal2Func},
	{0x8300, "CAL3", MENU_MODE_NORMAL, cal3Func},
		
	{0x8400, "CAL4", MENU_MODE_MENU, 0},
	{0x8410, "C41", MENU_MODE_CONTI_EDIT_NO_BLINK, cal41Func},
	{0x8420, "C42", MENU_MODE_CONTI_EDIT_NO_BLINK, cal42Func},
	{0x8430, "C43", MENU_MODE_CONTI_EDIT_NO_BLINK, cal43Func},
	{0x8440, "C44", MENU_MODE_CONTI_EDIT_NO_BLINK, cal44Func},
	{0x8450, "C45", MENU_MODE_CONTI_EDIT_NO_BLINK, cal45Func},
#ifdef USE_TEMP_COMP_MWIIH
			{0x8460, "C46", MENU_MODE_CONTI_EDIT_NO_BLINK, cal46Func},
#endif
	
	{0x8500, "CAL5", MENU_MODE_NORMAL, cal5Func},
	{0x8600, "CAL6", MENU_MODE_NORMAL, cal6Func},

	{0x8700, "CAL7", MENU_MODE_MENU, 0},
#ifdef USE_CAL7_WEIGHT_CAL
	{0x8710, "WSPAN", MENU_MODE_CONTI_EDIT_BLINK, cal71Func},
#else
	{0x8710, "PER", MENU_MODE_CONTI_EDIT_NO_BLINK, cal71Func},
#endif
	{0x8720, "C72", MENU_MODE_RUN, cal72Func},

	{0x8800, "CAL8", MENU_MODE_NORMAL, cal8Func},

	{0x8900, "CAL9", MENU_MODE_MENU, 0},
	{0x8910, " G-1", MENU_MODE_CONTI_EDIT_BLINK, cal91Func},
	{0x8920, " G-2", MENU_MODE_CONTI_EDIT_BLINK, cal92Func},

	{0x8A00, "CAL10", MENU_MODE_MENU, 0},	
	{0x8A10, "UNIT", MENU_MODE_CONTI_EDIT_BLINK, cal101Func},		
	{0x8A20, "CAPA", MENU_MODE_CONTI_EDIT_BLINK, cal102Func},
#ifdef USE_EXP_HYST_COMP           
	{0x8A30, "ORDER", MENU_MODE_CONTI_EDIT_BLINK, cal103Func},
#else        
	{0x8A30, "MID", MENU_MODE_CONTI_EDIT_BLINK, cal103Func},
#endif
	{0x8A40, "W-DP", MENU_MODE_CONTI_EDIT_BLINK, cal104Func},	
	{0x8A50, "  1d", MENU_MODE_CONTI_EDIT_BLINK, cal105Func},	
	{0x8A60, "DUAL", MENU_MODE_CONTI_EDIT_BLINK, cal106Func},	
	{0x8A70, "TARE", MENU_MODE_CONTI_EDIT_BLINK, cal107Func},	
		
	{0x8B00, "CAL11", MENU_MODE_NORMAL, cal11Func},
#ifdef USE_CHINA_LIMIT_CAL
	{0x8C00, "CAL13", MENU_MODE_NORMAL, cal13Func},
#elif defined (TURKEY_BLOCK_FUNCTION_KEY)
	{0x8C00, "CAL14", MENU_MODE_NORMAL, cal14Func},
#elif defined (USE_CREEP_MENU)// || defined (USE_TEMP_COMP)
#ifdef USE_CREEP_TIME
	{0x8C00, "CAL12", MENU_MODE_MENU, 0},
	{0x8C10, "CREEP", MENU_MODE_CONTI_EDIT_BLINK, cal121Func},
	{0x8C20, "TIME", MENU_MODE_CONTI_EDIT_BLINK, cal122Func},
#else
	{0x8C00, "CAL12", MENU_MODE_NORMAL, cal12Func},
#endif
#elif defined (USE_ZERO_TRACKING_RANGE_SETTING)
	{0x8C00, "CAL12", MENU_MODE_NORMAL, cal15Func},
#endif
#ifdef USE_TEMP_COMP_MWIIH
 #ifdef USE_CREEP_MENU
		{0x8D00, "CAL15", MENU_MODE_NORMAL, cal15Func},
 #else
		{0x8C00, "CAL15", MENU_MODE_NORMAL, cal15Func},
 #endif
#endif
	{0xF100, "PWD", MENU_MODE_NORMAL, calCheckPwdFunc},
};

/** @brief Calibration중에 Stable을 기다리는 Count*/
static INT8U calWaitCount;
/** @brief 영점값*/
static INT32S zeroValue;
/** @brief 중간값(상승)*/
static INT32S midUpValue;
/** @brief 중간값(하강)*/
static INT32S midDownValue;
/** @brief 스판값*/
static INT32S spanValue;
/** @brief Percent calibration(%)*/
static INT8U calPercent;
/** @brief Weight calibration*/
static INT16U calWeight;
#ifdef USE_UNIT_0_1KG_CAPA_TABLE
/** @brief Weight calibration*/
static INT8U calUnit;
#endif
#ifdef USE_EXP_HYST_COMP
static INT32S hystAdValue[10];
#endif
#ifdef USE_TEMP_COMP_MWIIH
/** @brief 보상시 온도상태 high,mid,low */
static INT8S tempState;
static INT8U TempCompState;
#endif
/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
static void calSetDispFilt(INT8U filtValue);
static INT8U calProc(KEY_TYPE key, INT8U cMode);
#ifdef USE_EXP_HYST_COMP
static INT8U calProcExpHyst(KEY_TYPE key);
#endif
static INT8U cal41SetParam(INT8U writeFlag, INT8U regData);

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/**
********************************************************************************
* @brief    Calibration Mode Application Init 함수\n
*           AD Data Pointer 받아옴\n
* @param    none
* @return   none
* @remark   초기에 실행 되는 함수\n
*           AD Data 접근 방법 결정 필요 (TO DO:Pointer or extern)
********************************************************************************
*/
void CalModeInit(void)
{
	DispSetDecPoint(WIN_WEIGHT, 0, 0, 0);
	DispSetDecPoint(WIN_UNIT_PRICE, 0, 0, 0);
	DispSetDecPoint(WIN_TOTAL_PRICE, 0, 0, 0);
	DispSetDecPoint(WIN_TARE, 0, 0, 0);
	DispClearAllDigits();

	calPercent = 5; /*Percent initial (0 = No percent calibration)*/
	calWeight = 0; /*Percent initial (0 = No percent calibration)*/
#ifdef USE_TEMP_COMP_MWIIH
			TempCompState = 0;
#endif
}

/**
********************************************************************************
* @brief    Calibration Menu Table 반환함\n
*           Menu Mode Application에서 사용
* @param    none
* @return   Calibration Menu Table Pointer
* @remark   
********************************************************************************
*/
MENU code *CalModeGetMenuTable(void)
{
	return calMenuTable;
}

/**
********************************************************************************
* @brief    Calibration Menu Table 개수 반환함\n
*           Menu Mode Application에서 사용
* @param    none
* @return   Calibration Menu Table 개수
* @remark   
********************************************************************************
*/
INT8U CalModeGetMenuTableMaxPos(void)
{
	return MENU_CAL_MAX_POS;
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
/**
********************************************************************************
* @brief    Calibration Menu Exit 함수\n
* @param    key : not use
* @return   Menu 함수 반환값
* @remark   MENU_RET_EXIT를 반환하여 Sale Mode로 전환하게 함
********************************************************************************
*/
INT8U calExitFunc(KEY_TYPE key)
{
	return MENU_RET_EXIT;
}

/**
********************************************************************************
* @brief    Calibration Check Password 함수\n
* @param    key : not use
* @return   Menu 함수 반환값
* @remark   MENU_RET_CAL를 반환하여 Cal Mode로 전환하게 함
********************************************************************************
*/
/** @brief Calibration password when using RTC \n
           password = (calPwd / year) + (calPwd % 10) \n
		   ex) 2007, calPwd = 6789;  6789 / 7 + 6789 % 7 = 0975 \n
 			   2008, calPwd = 6789;  6789 / 8 + 6789 % 8 = 0853 \n
 			   2009, calPwd = 6789;  6789 / 9 + 6789 % 9 = 0757 \n
**/
#ifdef USE_CTS_MODE
INT8U calCheckPwdFunc(KEY_TYPE key)
{
	INT8U rawKey;
	INT8U calStateBuf;

	
	if (CtsCheckEnabled())
	{
		calStateBuf = 2;
		CtsUpdateScaleData(12, &calStateBuf);	// 12 CAL state - 2 CAL 대기모드
#ifdef CTS_ONE_DP
        DispSetConstStrMsg(WIN_WEIGHT, "ACCESS");
#elif  defined(CTS_ONE_DP_UNDER6)
        DispSetConstStrMsg(WIN_WEIGHT, " ACC ");
#else
		DispSetConstStrMsg(WIN_WEIGHT, "ACCESS");
     	DispSetConstStrMsg(WIN_UNIT_PRICE, " NEED ");
#endif
     	DispSetRunMsgPage(10,1);
     	BuzzerSetOnTimes(3);
     	KeyEnable(ON);
        while(1)
		{
        	CommProc(); 
        	if (CtsCheckCalEnterEnabled()) 
			{
				calStateBuf = 1;
				CtsUpdateScaleData(12, &calStateBuf);	// 12 CAL state - 1 CAL 모드
                break;
			}
			if(KeyCheck())
			{
				rawKey = RingBufGetChar(&KeyDrvRingBuf);
				KeyDrvGetFuncCode(KEY_MODE_NORMAL, rawKey);
			}
#ifdef CTS_ONE_DP
            DispSetConstStrMsg(WIN_WEIGHT, "ACCESS");
#elif  defined(CTS_ONE_DP_UNDER6)
            DispSetConstStrMsg(WIN_WEIGHT, " ACC ");
#else
            DispSetConstStrMsg(WIN_WEIGHT, "ACCESS");
            DispSetConstStrMsg(WIN_UNIT_PRICE, " NEED ");
#endif
            DispSetRunMsgPage(10,0);    
			PowerProc();
			AdProc(0, 0);
		}
	}
	
	return MENU_RET_CAL_MODE;
}
#else //USE_CTS_MODE
INT8U calCheckPwdFunc(KEY_TYPE key)
{
	INT32U tempInt32u;
	INT16U tempInt16u;
	INT8U pwdEnable;

	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			pwdEnable = ParamReadByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_CAL_PASSWORD_ENABLE_KEY);
			tempInt16u = ParamReadWord(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_CAL_PASSWORD);
#ifdef USE_CTS_MODE_OLD
            CtsCalEnter();
#endif
			if ((pwdEnable != 0x55) || (tempInt16u > 9999))
			{
				return MENU_RET_CAL_MODE;
			}

			tempInt16u = 0;		
			EditorSetDataAndRun(MenuDataWinNum, DEC, tempInt16u, 0, 9999, 0, 0, 0);
			MenuFuncState = MENU_EDITOR_COMPLETE_STATE;
			break;
			
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData(&tempInt32u);
#ifdef USE_RTC
			TimeReadData();
			tempInt16u = ParamReadWord(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_CAL_PASSWORD);
			tempInt16u = (tempInt16u / TimeData.year) + (tempInt16u % TimeData.year);
#else
			tempInt16u = ParamReadWord(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_CAL_PASSWORD);
#endif
			if (tempInt16u == (INT16U)tempInt32u)
			{
				return MENU_RET_CAL_MODE;
			}
			MenuFuncState = MENU_EDITOR_INIT_STATE;
			break; 
	}
	return MENU_RET_INCOMPLETE;
}
#endif //USE_CTS_MODE
/**
********************************************************************************
* @brief    Calibration 1 함수\n
* @param    key : Zero key, C key
* @return   Menu 함수 반환값
* @remark   MENU_RET_COMPLETE를 반환하여 Cal Mode를 연속적으로 수행함\n
			Nomalized AD(Raw)를 보는 메뉴, Zero key : 영점을 잡는다.
			Enter key : Zero가 가능한 Nomalized AD(Raw)와 Zero가 잡히지 않는
			Nomalized AD(Raw)를 toggle시켜준다.
********************************************************************************
*/
INT8U cal1Func(KEY_TYPE key)
{
	static INT32S zeroValue = 0;
	static BOOLEAN toggle = 0;	//static으로 하지 않으면 값을 기억하지 못함

	if (MenuFuncState == 0)
	{
		zeroValue = AdData.rawNormalizedData;
		MenuFuncState++;
	}
	
	if (key == MenuKeyMenuUp)	//Menu Up키로 Zero잡는 모드와 Zero잡지 않는 모드를 토글 함, 온도 테스트 목적
	{
		if (toggle)
		{
			toggle = 0;
		}
		else
		{
			toggle = 1;
		}
	}
	
	if (toggle) 
	{
		DispSetNum(MenuDataWinNum, AdData.rawNormalizedData - zeroValue, 0);
	}
	else
	{
		DispSetNum(MenuDataWinNum, AdData.rawNormalizedData, 0);
	}

	if (key == MenuKeyC || key == MenuKeyEnter)	//SW모델은 C키가 없으므로 Enter키로 대체
	{
		return MENU_RET_COMPLETE;
	}

	if (key == KEY_ZERO)	//SW모델은 Zero key와 Enter Key가 겹치므로 Zero기능을 사용할 수 없음
	{
		zeroValue = AdData.rawNormalizedData;
	}

	return MENU_RET_INCOMPLETE;
}

/**
********************************************************************************
* @brief    Calibration 2 함수\n
* @param    key : Test key
* @return   Menu 함수 반환값
* @remark   입력받는 Key 값을 보여준다.
********************************************************************************
*/
INT8U cal2Func(KEY_TYPE key)
{
	INT8U rawKey;

	switch (MenuFuncState)
	{
		case 0:
			DispSetConstStr(MenuStrWinNum, "KEY");
			MenuFuncState++;
			break;
			
		case 1:
			if (key != KEY_NOT_PRESSED)
			{				
				if (key == MenuKeyC || key == KEY_ZERO)	//TODO : SW모델은 C키가 없으므로 Unit change로 대체(Tare + Hold) 추후 제거
				{
					return MENU_RET_COMPLETE;
				}
				
				rawKey = KeyGetPrevRawCode(KEY_MODE_NORMAL, key);
				DispSetNum(MenuAuxWinNum, rawKey, 0);
				DispSetNum(MenuDataWinNum, key, 0);
			}
			break;
	}

	return MENU_RET_INCOMPLETE; 
}

/**
********************************************************************************
* @brief    Calibration 3 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   Hysteresis, Linear, Span(or %) calibration을 수행한다.
********************************************************************************
*/
INT8U cal3Func(KEY_TYPE key)
{
#ifdef USE_EXP_HYST_COMP        
	if (AdReadParam(HYST_COMP_ORDER) == 0) return calProc(key, CAL_PROC_SPAN);
	else return calProcExpHyst(key); 
#else
	if (AdReadParam(LINEAR_RATIO) == 0) return calProc(key, CAL_PROC_SPAN);
	else return calProc(key, CAL_PROC_HYST); 
#endif        
}

/**
********************************************************************************
* @brief    Calibration 4-1 Parameter edit 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   
********************************************************************************
*/
INT8U cal41Func(KEY_TYPE key)
{
	INT32U tempInt32u;
	INT8U tempInt8u;

	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			tempInt8u = cal41SetParam(0, 0);
			EditorSetDataAndRun(MenuDataWinNum, HEX, tempInt8u, 0, 0xFF, 0, 0, 0);
			if (MenuWinIsOne) EditorSetConstStr("C41");
			MenuFuncState = MENU_EDITOR_COMPLETE_STATE;
			break;
			
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData(&tempInt32u);
			cal41SetParam(1, (INT8U)tempInt32u);
			return MENU_RET_COMPLETE;
	}
	return MENU_RET_INCOMPLETE;
}

/**
********************************************************************************
* @brief    Calibration 4-2 Parameter edit 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   
********************************************************************************
*/
INT8U cal42Func(KEY_TYPE key)
{
#if defined (USE_SALE_PRICE) || defined (USE_SALE_PRICE_EXTERNAL_PLU) || defined (USE_SALE_PRICE_EXTERNAL_PLU_AP)|| defined (USE_SALE_ER_MLED) || defined (USE_SALE_PRICE_COMPARE_PLU) || defined (USE_SALE_PR)
	INT32U tempInt32u;
	INT8U tempInt8u;

	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:		
			tempInt8u = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST);
			EditorSetDataAndRun(MenuDataWinNum, HEX, tempInt8u, 0, 0xFF, 0, 0, 0);
			MenuFuncState = MENU_EDITOR_COMPLETE_STATE;
			break;
			
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData(&tempInt32u);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST, (INT8U)tempInt32u);
			return MENU_RET_COMPLETE;
	}
	return MENU_RET_INCOMPLETE;
#else
	return MENU_RET_COMPLETE;
#endif
}

/**
********************************************************************************
* @brief    Calibration 4-3 Parameter edit 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   
********************************************************************************
*/
INT8U cal43Func(KEY_TYPE key)
{
#ifdef NO_USE_CAL43
	return MENU_RET_COMPLETE;
#else		
	INT32U tempInt32u;
	INT8U tempInt8u;

	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			tempInt8u = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND);
			EditorSetDataAndRun(MenuDataWinNum, HEX, tempInt8u, 0, 0xFF, 0, 0, 0);
			if (MenuWinIsOne) EditorSetConstStr("C43");
			MenuFuncState = MENU_EDITOR_COMPLETE_STATE;
			break;
			
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData(&tempInt32u);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, (INT8U)tempInt32u);
			return MENU_RET_COMPLETE;
	}
	return MENU_RET_INCOMPLETE;
#endif
}

/**
********************************************************************************
* @brief    Calibration 4-4 Parameter edit 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   
********************************************************************************
*/
INT8U cal44Func(KEY_TYPE key)
{
#if defined (USE_SALE_PRICE) ||defined (USE_SALE_PRICE_EXTERNAL_PLU) || defined (USE_SALE_PRICE_EXTERNAL_PLU_AP) || defined (USE_SALE_ER_MLED) || defined (USE_SALE_PRICE_COMPARE_PLU) || defined (USE_SALE_PR)
	INT32U tempInt32u;
	INT8U tempInt8u;

	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			tempInt8u = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_3RD);
			EditorSetDataAndRun(MenuDataWinNum, HEX, tempInt8u, 0, 0xFF, 0, 0, 0);
			MenuFuncState = MENU_EDITOR_COMPLETE_STATE;
			break;
			
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData(&tempInt32u);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_3RD, (INT8U)tempInt32u);
#ifdef USE_OVER_LOAD_LIMIT_BIT
			tempInt8u = (INT8U)tempInt32u;
	  		if(tempInt8u & 0x40)//bit 6 
			{
				AdWriteParam(OVER_LOAD_LIMIT, 12);
			}
			else
			{
				AdWriteParam(OVER_LOAD_LIMIT, 9);
			}
#endif
			return MENU_RET_COMPLETE;
	}
	return MENU_RET_INCOMPLETE;	
#else
	return MENU_RET_COMPLETE;
#endif
}

/**
********************************************************************************
* @brief    Calibration 4-5 Parameter edit 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   
********************************************************************************
*/
INT8U cal45Func(KEY_TYPE key)
{
#if defined (USE_SALE_PRICE) ||defined (USE_SALE_PRICE_EXTERNAL_PLU) || defined (USE_SALE_PRICE_EXTERNAL_PLU_AP)|| defined (USE_SALE_ER_MLED) || defined (USE_SALE_PRICE_COMPARE_PLU) || defined (USE_SALE_PR)
	INT32U tempInt32u;
	INT8U tempInt8u;
	
	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			tempInt8u = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH);
			EditorSetDataAndRun(MenuDataWinNum, HEX, tempInt8u, 0, 0xFF, 0, 0, 0);
			MenuFuncState = MENU_EDITOR_COMPLETE_STATE;
			break;
			
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData(&tempInt32u);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, (INT8U)tempInt32u);
			return MENU_RET_COMPLETE;
	}
	return MENU_RET_INCOMPLETE;
#else
	return MENU_RET_COMPLETE;
#endif
}

#ifdef USE_TEMP_COMP_MWIIH
/**
********************************************************************************
* @brief    Calibration 4-6 Parameter edit 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   온도 보상 On/Off
********************************************************************************
*/
INT8U cal46Func(KEY_TYPE key)
{
	INT32S tempInt32u;
	INT8U tempInt8u;
	
	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			tempInt8u = (INT8U)AdReadParam(TEMP_COMP_ENABLE);
//			EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)tempInt8u), 0, 1, 0, DEC_TYPE_DOT, 0);
//			MenuFuncState++;
			EditorSetDataAndRun(MenuDataWinNum, HEX, tempInt8u, 0, 0xFF, 0, 0, 0);
			if (MenuWinIsOne) EditorSetConstStr("C46");
			MenuFuncState = MENU_EDITOR_COMPLETE_STATE;
			break;
			
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData(&tempInt32u); 		
			tempInt8u = (INT8U)(tempInt32u);
			AdWriteParam(TEMP_COMP_ENABLE, (INT32S)tempInt8u);
			return MENU_RET_COMPLETE;
//			MenuFuncState++;
			break;
	}
	return MENU_RET_INCOMPLETE;
}
#endif


/**
********************************************************************************
* @brief    Calibration 5 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   AD (Raw filtered) data를 Display 한다.
********************************************************************************
*/
INT8U cal5Func(KEY_TYPE key)
{
	DispSetNum(MenuDataWinNum, AdData.rawFilteredData, 0);
	if (key == MenuKeyC || key == MenuKeyEnter)
	{
		return MENU_RET_COMPLETE;
	}
	return MENU_RET_INCOMPLETE;
}

/**
********************************************************************************
* @brief    Calibration 6 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   이전 ER과 같이 수정, 변경 Code입력 한 뒤 원하는 Soft key를 결정하면 설정 완료
*			PB, SW는 TARE + HOLD 키 동시에 눌러야 C 키로 동작(KEY_DUMMY가 C키로 동작, KeyDrvProc 참고)
********************************************************************************
*/
#ifdef USE_CAL6_KEY_TYPE
    #if MODEL_NUM == MODEL_OMV4_PDN_LED
	#if defined COUNTRY_UNITEDSTATES || defined COUNTRY_CANADA
	const char code cal6KeyTypeDispStr[3][3] = {"U", "P", "S"};
	#else
	const char code cal6KeyTypeDispStr[3][3] = {"P", "U", "S"};
	#endif
	#endif
    #if MODEL_NUM == MODEL_OMV4_SW1SCU_LCD
	const char code cal6KeyTypeDispStr[3][3] = {"S", "C", "U"};
	#endif
    #if MODEL_NUM == MODEL_OMV4_PB_LCD //pys cal6 기능 추가 
	#ifdef USE_AUTO_TARE_FUNCTION
	const char code cal6KeyTypeDispStr[4][3] = {"S", "C", "U", "T"};
	#else
	const char code cal6KeyTypeDispStr[3][3] = {"S", "C", "U",};
	#endif
	#endif
    #if MODEL_NUM == MODEL_OMV3_SW1SCU_LCD
	const char code cal6KeyTypeDispStr[3][3] = {"S", "C", "U"};
	#endif	
    #if MODEL_NUM == MODEL_OMV3_SWLRWR_LED
	const char code cal6KeyTypeDispStr[3][5] = {"WR", "LR", "WRUC"};
	#endif
	#if MODEL_NUM == MODEL_SW1SCU_LCD
	const char code cal6KeyTypeDispStr[3][3] = {"S", "C", "U"};
	#endif
	#if MODEL_NUM == MODEL_SM59R16A5_SW1SCU_LCD
	const char code cal6KeyTypeDispStr[3][3] = {"S", "C", "U"};
	#endif
	#if MODEL_NUM == MODEL_SW1SU_LCD_ECR
	const char code cal6KeyTypeDispStr[3][3] = {"S", "C", "U"};
	#endif
	#if MODEL_NUM == MODEL_SWLRWR_LED
	const char code cal6KeyTypeDispStr[3][5] = {"WR", "LR", "WRUC"};
	#endif
	#if MODEL_NUM == MODEL_SWLRWR_LED_ECR
	const char code cal6KeyTypeDispStr[3][5] = {"WR", "LR", "WRUC"};
	#endif
	#if MODEL_NUM == MODEL_SM59R16A5_SWLRWR_LED
	const char code cal6KeyTypeDispStr[3][5] = {"WR", "LR", "WRUC"};
	#endif
	#if MODEL_NUM == MODEL_SW1S_LCD_LB
	const char code cal6KeyTypeDispStr[3][3] = {"S", "C", "U"};
	#endif
	#if MODEL_NUM == MODEL_PW2_LCD_LB
	const char code cal6KeyTypeDispStr[3][3] = {"S", "C", "U"};
	#endif
	#if MODEL_NUM == MODEL_PW2_LCD
	const char code cal6KeyTypeDispStr[3][3] = {"S","C", "U"};
	#endif
	#if MODEL_NUM == MODEL_AD_VFD 
	const char code cal6KeyTypeDispStr[3][6] = {"HOLD", "PRINT", "UNIT"};
	#endif
	#if MODEL_NUM == MODEL_DB1H_VFD	 
	const char code cal6KeyTypeDispStr[2][6] = {"HOLD", "PRINT"};
	#endif
	#if MODEL_NUM == MODEL_PB_LCD
	#ifdef USE_AUTO_TARE_FUNCTION
	const char code cal6KeyTypeDispStr[4][3] = {"S", "C", "U", "T"};
	#else
	const char code cal6KeyTypeDispStr[3][3] = {"S", "C", "U",};
	#endif
	#endif
	#if MODEL_NUM == MODEL_PB_LCD_HEALTH
	const char code cal6KeyTypeDispStr[3][3] = {"S", "C", "U"};
	#endif
	#if MODEL_NUM == MODEL_PB_LCD_BLUETOOTH
	const char code cal6KeyTypeDispStr[3][3] = {"S", "C", "U"};
	#endif
	#if MODEL_NUM == MODEL_DP_LCD
	const char code cal6KeyTypeDispStr[3][3] = {"S", "C", "U"};
	#endif
	#if MODEL_NUM == MODEL_OMV3_FW500S_LCD
	const char code cal6KeyTypeDispStr[3][3] = {"S", "C", "U"};
	#endif
	#if MODEL_NUM == MODEL_OMV3_FW500S_LED
	const char code cal6KeyTypeDispStr[3][5] = {"WR", "LR", "WRUC"};
	#endif
    #if MODEL_NUM == MODEL_OMV3_FW500S_LCD_LB
	const char code cal6KeyTypeDispStr[4][3] = {"S", "C", "U", "UL"};
	#endif
	#if MODEL_NUM == MODEL_OMV3_FW500S_LCD_RS232
	const char code cal6KeyTypeDispStr[3][3] = {"S", "C", "U"};
	#endif
	#if MODEL_NUM == MODEL_OMV4_DB1H_VFD
	const char code cal6KeyTypeDispStr[2][6] = {"HOLD", "PRINT"};
	#endif
    #if MODEL_NUM == MODEL_OMV4_SWLRWR_LED
	const char code cal6KeyTypeDispStr[3][5] = {"WR", "LR", "WRUC"};
	#endif
	#if MODEL_NUM == MODEL_OMV4_PW2_LCD
	const char code cal6KeyTypeDispStr[3][3] = {"S","C", "U"};
	#endif
	#if MODEL_NUM == MODEL_OMV4_SW1SU_LCD_ECR
	const char code cal6KeyTypeDispStr[3][3] = {"S", "C", "U"};
	#endif
	#if MODEL_NUM == MODEL_OMV4_SW2W_LCD
	const char code cal6KeyTypeDispStr[3][3] = {"S", "C", "U"};
	#endif
	#if MODEL_NUM == MODEL_OMV4_SW2W_LED
	const char code cal6KeyTypeDispStr[3][5] = {"WR", "LR", "WRUC"};
	#endif

INT8U cal6Func(KEY_TYPE key)
{
	static INT8U keyType;

	switch (MenuFuncState)
	{
		case 0:
			MenuFuncState++;
			keyType = KeyGetFuncCode(KEY_MODE_KEY_TYPE);
			DispSetConstStr(MenuDataWinNum, &(cal6KeyTypeDispStr[keyType][0]));
			break;
		case 1:
			if (key != KEY_NOT_PRESSED)
			{
				if (key == MenuKeyEnter)	
				{
					KeySetFuncCode(KEY_MODE_KEY_TYPE, 0, keyType);
					ParamWriteByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_TYPE, keyType);
					ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SALE_STATE_NORMAL);
					ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_ON, OFF);
					return MENU_RET_COMPLETE;
				}
				else if (key == MenuKeyC)
				{
					keyType++;				
				#if MODEL_NUM == MODEL_DB1H_VFD
					if (keyType > 1)
                                #elif MODEL_NUM == MODEL_OMV3_FW500S_LCD_LB
                                        if (keyType > 3) 
				#elif MODEL_NUM == MODEL_PW2_LCD
					if (keyType == 1)
						keyType++;
					else if(keyType > CAL6_KEY_END_INDEX)
				#elif MODEL_NUM == MODEL_SW1SU_LCD_ECR
					if (keyType == 1)
						keyType++;
					else if(keyType > CAL6_KEY_END_INDEX)
				//#elif MODEL_NUM == MODEL_OMV4_SW1SU_LCD_ECR
				//	if (keyType == 1)
				//		keyType++;
				//	else if(keyType > CAL6_KEY_END_INDEX)
				#elif MODEL_NUM == MODEL_OMV4_DB1H_VFD
					if (keyType > 1)
				#else
					if (keyType > CAL6_KEY_END_INDEX) 
				#endif
					{
						keyType = 0;
					}
				}
				#if MODEL_NUM == MODEL_OMV4_PDN_LED
				else if (key == MenuKeyMenuDown)
				{
					keyType++;
					if (keyType > 2)
					{
						keyType = 0;
					}
				}
				#endif
				DispSetConstStr(MenuDataWinNum, &(cal6KeyTypeDispStr[keyType][0]));
			}
			break;
	}
	return MENU_RET_INCOMPLETE; 
}
#else
INT8U cal6Func(KEY_TYPE key)
{
	static INT32U funcCode;
	static INT8U rawKey;

	switch (MenuFuncState)
	{
		case 0:
			DispSetConstStr(MenuStrWinNum, "E-SET");
			funcCode = 0;
			rawKey = 0xff;
			MenuFuncState++;
			break;
		case 1:
			if (key != KEY_NOT_PRESSED)
			{
				if (key == MenuKeyC)	
				{
					KeyInit();
					return MENU_RET_COMPLETE;
				}
				else if (key >= KEY_NUM_0 && key <= KEY_NUM_9)
				{
					funcCode = EditorInsDecDigit(funcCode, key-KEY_NUM_0, 3, 255, FALSE);
					DispSetNum(MenuDataWinNum, funcCode, 0);
				}
				else
				{
					rawKey = KeyGetPrevRawCode(KEY_MODE_NORMAL, key);
					DispSetNum(MenuAuxWinNum, rawKey, 0);
					if (rawKey != 0xff)
					{
						KeySetFuncCode(KEY_MODE_NORMAL, rawKey, funcCode);
					}
				}
			}
			break;
	}

	return MENU_RET_INCOMPLETE; 
}
#endif

#ifdef USE_CAL7_WEIGHT_CAL
/**
********************************************************************************
* @brief    Calibration 7 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   
********************************************************************************
*/
INT8U cal71Func(KEY_TYPE key)
{
	INT32U tempInt32u;
	INT8U tempInt8u;

	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			EditorSetDataAndRun(MenuDataWinNum, DEC, (INT32U)calWeight, 0, CalParam.capa, 0, 0, 0);
			MenuFuncState++;
			break;
			
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData(&tempInt32u);
			calWeight = (INT16U)tempInt32u;
			return MENU_RET_COMPLETE;
	}
	return MENU_RET_INCOMPLETE;
}

/**
********************************************************************************
* @brief    Calibration 7 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   
********************************************************************************
*/
INT8U cal72Func(KEY_TYPE key)
{
	return calProc(key, CAL_PROC_WEIGHT); 		
}
#else
/**
********************************************************************************
* @brief    Calibration 7 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   
********************************************************************************
*/
INT8U cal71Func(KEY_TYPE key)
{
	INT32U tempInt32u;

	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			EditorSetDataAndRun(MenuDataWinNum, DEC, (INT32U)calPercent, 5, 90, 0, 0, 0);
			if (MenuWinIsOne) EditorSetConstStr("PER");
			MenuFuncState++;
			break;
			
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData(&tempInt32u);
			if(tempInt32u != 5)
			{
				if((tempInt32u % 10) != 0)
				{
					MenuFuncState = MENU_EDITOR_INIT_STATE;
					calPercent = 5;
					break;
				}
			}
			calPercent = (INT8U)tempInt32u;
			return MENU_RET_COMPLETE;
	}
	return MENU_RET_INCOMPLETE;
}

/**
********************************************************************************
* @brief    Calibration 7 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   
********************************************************************************
*/
INT8U cal72Func(KEY_TYPE key)
{
	return calProc(key, CAL_PROC_PERCENT);
}
#endif
/**
********************************************************************************
* @brief    Calibration 8 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   
********************************************************************************
*/
INT8U cal8Func(KEY_TYPE key)
{
#ifdef USE_TEMP_COMP_MWIIH
		INT16S tempInt16s;
	
		tempInt16s = TempDrvReadData(); 
		DispSetNum(MenuDataWinNum, (INT32S)tempInt16s, 0);
		if (key == MenuKeyC || key == MenuKeyEnter) 
		{
			return MENU_RET_COMPLETE;
		}
#else
	INT16U tempInt16u;

	tempInt16u = PowerReadBattVolt();	

	DispSetNum(MenuDataWinNum, (INT32S)tempInt16u, 0);
	if (key == MenuKeyC || key == MenuKeyEnter)	
	{
		return MENU_RET_COMPLETE;
	}

	PowerResetBattCutOffTimer();

	switch (MenuFuncState)
	{
		case 0: 
			if (key == MenuKeyMenuUp)
			{
				MenuFuncState++;
			}
			break;
		case 1: 
			if (key == MenuKeyMenuUp)
			{
				MenuFuncState++;
			}
			else if (key == MenuKeyMenuDown || key == KEY_NUM_1)
			{
				MenuFuncState = 0;
			}
			break;
		case 2: 
			if (key == MenuKeyMenuDown || key == KEY_NUM_1) 
			{
				MenuFuncState++;
			}
			else if (key == MenuKeyMenuUp) 
			{
				MenuFuncState = 0;
			}
			break;
		case 3: 
			if (key == MenuKeyMenuDown || key == KEY_NUM_1)
			{
				MenuFuncState++;
			}
			else if (key == MenuKeyMenuUp)
			{
				MenuFuncState = 0;
			}
			break;
		case 4: 
			PowerCalBattOffset();
			MenuFuncState = 0;
			break;
	}

	return MENU_RET_INCOMPLETE;
#endif
}

/**
********************************************************************************
* @brief    Calibration 9 - 1 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   Gravity값을 설정한다. (Calibration 위치)
********************************************************************************
*/
INT8U cal91Func(KEY_TYPE key)
{
	INT32U tempInt32u;
	INT16S tempInt16s;

	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			tempInt16s = (INT16S)AdReadParam(CAL_G_NUM);
			EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)tempInt16s+90000L), 90000, 99999, 4, DEC_TYPE_DOT, 0);
			MenuFuncState = MENU_EDITOR_COMPLETE_STATE;
			break;
			
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData(&tempInt32u);
			tempInt16s = (INT16S)(tempInt32u-90000L);		
			AdWriteParam(CAL_G_NUM, (INT32S)tempInt16s);
			return MENU_RET_COMPLETE;
	}
	return MENU_RET_INCOMPLETE;
}

/**
********************************************************************************
* @brief    Calibration 9 - 2 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   Gravity값을 설정한다. (Using 위치)
********************************************************************************
*/
INT8U cal92Func(KEY_TYPE key)
{
	INT32U tempInt32u;
	INT16S tempInt16s;

	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			tempInt16s = (INT16S)AdReadParam(USING_G_NUM);
#ifndef USE_MOON_GRAVITY
			EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)tempInt16s+90000L), 90000, 99999, 4, DEC_TYPE_DOT, 0);
#else
			EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)tempInt16s+10000L), 10000, 99999, 4, DEC_TYPE_DOT, 0); //달 중력보정값 입력을 위함
#endif
			MenuFuncState = MENU_EDITOR_COMPLETE_STATE;
			break;
			
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData(&tempInt32u);
#ifndef USE_MOON_GRAVITY			
			tempInt16s = (INT16S)(tempInt32u-90000L);
#else
			tempInt16s = (INT16S)(tempInt32u-10000L); //달 중력보정값 입력을 위함
#endif
			AdWriteParam(USING_G_NUM, (INT32S)tempInt16s);
			return MENU_RET_COMPLETE;		
	}
	return MENU_RET_INCOMPLETE;
}

/**
********************************************************************************
* @brief    Calibration 10-1 Calibration Unit 설정 함수\n
* @param    key : Edit keys
* @return   Menu 함수 반환값
* @remark   Calibration Unit을 설정한다. (0 : kg, 1 : lb, 3 : gram, 4 : ton), 2 : oz인 경우는 return
********************************************************************************
*/
INT8U cal101Func (KEY_TYPE key)
{
	INT32U tempInt32u;
	INT8U tempInt8u;

	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			tempInt8u = (INT8U)AdReadParam(CAL_UNIT);
			EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)tempInt8u), 0, 4, 0, DEC_TYPE_DOT, 0);
			MenuFuncState++;
			break;

		case MENU_EDITOR_COMPLETE_STATE:			
			EditorGetData(&tempInt32u);
			tempInt8u = (INT8U)(tempInt32u);
			if (tempInt8u == UNIT_OZ)
			{
				MenuFuncState--;
			}
			else
			{
				AdWriteParam(CAL_UNIT, (INT32S)tempInt8u);
#ifdef USE_UNIT_0_1KG_CAPA_TABLE				
				calUnit = (INT32S)tempInt8u;
#endif				
				MenuFuncState++;
			}
			return MENU_RET_COMPLETE;
	}
	return MENU_RET_INCOMPLETE;
}

/**
********************************************************************************
* @brief    Calibration 10-2 CAPA 설정 함수\n
* @param    key : Edit keys
* @return   Menu 함수 반환값
* @remark   Calibration CAPA를 설정한다. (테이블 참조)
********************************************************************************
*/
INT8U cal102Func (KEY_TYPE key)
{
	INT32U tempInt32u;
	INT16U tempInt16u;
#ifdef USE_32KG_CAPA
	INT32S tempInt32s;
#endif
	
	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			tempInt16u = (INT16U)AdReadParam(CAPA);
#ifdef USE_32KG_CAPA
			tempInt32s = AdReadParam(OVER_LOAD_LIMIT);
			if ((tempInt32s == 209) && (tempInt16u == 30))
				tempInt16u = 32;
#endif
#ifdef USE_UNIT_0_1KG_CAPA_TABLE
			if (calUnit == UNIT_KG)
			{
				EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)tempInt16u), 1, 60000, 1, DEC_TYPE_DOT, 0);
			}
			else
			{
				EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)tempInt16u), 1, 60000, 0, DEC_TYPE_DOT, 0);
			}
#else
			EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)tempInt16u), 1, 60000, 0, DEC_TYPE_DOT, 0);
#endif
			MenuFuncState++;
			break;
			
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData(&tempInt32u); 		
			tempInt16u = (INT16U)(tempInt32u);
		
			if (AdFindCapaIndex(CalParam.calUnit, tempInt16u) != 0xff)
			{
#ifdef USE_32KG_CAPA
				if (tempInt16u == 32)
				{
					AdWriteParam(OVER_LOAD_LIMIT, 209);
					tempInt16u = 30;
				}
				else
				{
					AdWriteParam(OVER_LOAD_LIMIT, 9);
				}
#endif
				AdWriteParam(CAPA, (INT32S)tempInt16u);
				return MENU_RET_COMPLETE;
			}
			MenuFuncState = MENU_EDITOR_INIT_STATE;
			break;
	}
	return MENU_RET_INCOMPLETE;
}

/**
********************************************************************************
* @brief    Calibration 10-3 Calibration Middle CAPA를 설정 하는 함수\n
* @param    key : Editor keys
* @return   Menu 함수 반환값
* @remark   Calibration Middle CAPA를 설정한다. (Hysteresis, Linear보상을 위한 중간 무게값)
********************************************************************************
*/
INT8U cal103Func (KEY_TYPE key)
{	
	INT32U tempInt32u;
	INT16U tempInt16u;
	
	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
#ifdef USE_EXP_HYST_COMP                  
			tempInt16u = (INT16U)AdReadParam(HYST_COMP_ORDER);
#else
			tempInt16u = (INT16U)AdReadParam(LINEAR_RATIO);
#endif
#ifdef USE_UNIT_0_1KG_CAPA_TABLE
			if (calUnit == UNIT_KG)
			{
				EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)tempInt16u), 0, 60000, 1, DEC_TYPE_DOT, 0);
			}
			else
			{
				EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)tempInt16u), 0, 60000, 0, DEC_TYPE_DOT, 0);
			}
#else
			EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)tempInt16u), 0, 60000, 0, DEC_TYPE_DOT, 0);
#endif
			MenuFuncState++;
			break;
			
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData(&tempInt32u); 		
			tempInt16u = (INT16U)AdReadParam(CAPA);
			
			if (tempInt32u < (INT32U)tempInt16u)
			{
#ifdef USE_EXP_HYST_COMP                          
				AdWriteParam(HYST_COMP_ORDER, (INT32S)tempInt32u);
#else
				AdWriteParam(LINEAR_RATIO, (INT32S)tempInt32u);
#endif                                
				return MENU_RET_COMPLETE;
			}
			MenuFuncState = MENU_EDITOR_INIT_STATE;
			break;
	}
	return MENU_RET_INCOMPLETE;
}

/**
********************************************************************************
* @brief    Calibration 10-4 Weight decimal point 설정 함수\n
* @param    key : Editor keys
* @return   Menu 함수 반환값
* @remark   Weight decimal point를 설정한다. (3,2,1,0)
********************************************************************************
*/
INT8U cal104Func (KEY_TYPE key)
{	
	INT32U tempInt32u;
	INT8U tempInt8u;
	
	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			tempInt8u = (INT8U)AdReadParam(WEIGHT_DEC_POS);
#ifdef USE_UNIT_0_1KG_CAPA_TABLE
			EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)tempInt8u), 0, 4, 0, DEC_TYPE_DOT, 0);
#else
			EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)tempInt8u), 0, 3, 0, DEC_TYPE_DOT, 0);
#endif
			MenuFuncState++;
			break;
			
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData(&tempInt32u);
#ifdef USE_UNIT_0_1KG_CAPA_TABLE
			if (tempInt32u <= 4)
#else				
			if (tempInt32u <= 3)
#endif
			{
				AdWriteParam(WEIGHT_DEC_POS, tempInt32u);
				return MENU_RET_COMPLETE;
			}
			MenuFuncState = MENU_EDITOR_INIT_STATE;
			break;
	}
	return MENU_RET_INCOMPLETE;
}

/**
********************************************************************************
* @brief    Calibration 10-5 1d설정 함수\n
* @param    key : Editor keys
* @return   Menu 함수 반환값
* @remark   1d의 값을 설정한다. (1, 2, 5, 10, 20, 50, weight dp와 연동된다.)
********************************************************************************
*/
INT8U cal105Func (KEY_TYPE key)
{
	INT32U tempInt32u;
	INT16U tempInt16u;
	INT8U tempInt8u;
	INT8U i;
	
	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			tempInt8u = (INT8U)AdReadParam(DIV_FACTOR);
			i = (INT8U)AdReadParam(WEIGHT_DEC_POS);
			EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)tempInt8u), 1, 50, i, DEC_TYPE_DOT, 0);
			MenuFuncState++;
			break;
			
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData(&tempInt32u);
			if (AdFindDivTableIndex((INT8U)tempInt32u) != 0xff)
			{
				AdWriteParam(DIV_FACTOR, tempInt32u);
				tempInt16u = 60000;
				AdWriteParam(RESOLUTION, (INT32S)tempInt16u);
				return MENU_RET_COMPLETE;
			}
			MenuFuncState = MENU_EDITOR_INIT_STATE;
			break;
	}
	return MENU_RET_INCOMPLETE;
}

/**
********************************************************************************
* @brief    Calibration 10-6 Dual 설정 함수\n
* @param    key : Editor keys
* @return   Menu 함수 반환값
* @remark   Dual여부를 결정 한다. 
********************************************************************************
*/
INT8U cal106Func (KEY_TYPE key)
{
	INT32U tempInt32u;
	INT8U tempInt8u;
	
	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			tempInt8u = (INT8U)AdReadParam(DUAL_INTERVAL);
			EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)tempInt8u), 0, 1, 0, DEC_TYPE_DOT, 0);
			MenuFuncState++;
			break;
			
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData(&tempInt32u);
			tempInt8u = (INT8U)(tempInt32u);
			AdWriteParam(DUAL_INTERVAL, (INT32S)tempInt8u);
			return MENU_RET_COMPLETE;
		}
	return MENU_RET_INCOMPLETE;
}

/**
********************************************************************************
* @brief    Calibration 10-7 Tare 설정 함수\n
* @param    key : Editor keys
* @return   Menu 함수 반환값
* @remark   Dual여부를 결정 한다. 
********************************************************************************
*/
INT8U cal107Func (KEY_TYPE key)
{
	INT32U tempInt32u;
//	INT8U tempInt8u;
	
	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			if(RangeParam.tareLimitType != TARE_LIMIT_TYPE_USER) RangeParam.tareLimitType = TARE_LIMIT_TYPE_DEFAULT;
			EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)RangeParam.tareLimitType), 0, 1, 0, DEC_TYPE_DOT, 0);
			MenuFuncState++;
			break;
			
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData(&tempInt32u);
			RangeParam.tareLimitType = (INT8U)(tempInt32u);
			if(RangeParam.tareLimitType == TARE_LIMIT_TYPE_USER) 
			{
				EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)RangeParam.tareLimit), 0, 60000, 0, DEC_TYPE_DOT, 0);
				MenuFuncState++;
			}
			else
			{
				RangeWriteParam();
				return MENU_RET_COMPLETE;
			}
			break;

		case MENU_EDITOR_COMPLETE_STATE + 1:			
			EditorGetData(&tempInt32u);
			RangeParam.tareLimit = (INT32S)(tempInt32u);
			RangeParam.tareLimitLb = (INT32S)(tempInt32u);
			RangeWriteParam();
			return MENU_RET_COMPLETE;
		}
	return MENU_RET_INCOMPLETE;
}

/**
********************************************************************************
* @brief    Calibration 11 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   국가세팅
********************************************************************************
*/
INT8U cal11Func (KEY_TYPE key)
{	
	INT32U tempInt32u;
	INT8U countryNum;
	VERSION version;
	
	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			DispSetConstStr(MenuStrWinNum, "CNTRY");
			ParamReadData(PARAM_VERSION_AREA_NUM, 0, (INT8U *)&version, sizeof(version));
			EditorSetDataAndRun(MenuDataWinNum, DEC, version.countryNum, 0, 99, 0, DEC_TYPE_DOT, 0);
			MenuFuncState++;
			break;
		case MENU_EDITOR_COMPLETE_STATE:
			if (key == MenuKeyEnter)
			{
				DispSetConstStrMsg(MenuDataWinNum, "WAIT");
				DispSetRunMsgPage(5,1);
				EditorGetData(&tempInt32u);
				countryNum = (INT8U)tempInt32u;
				ScaleModeSetDefaultParam(countryNum);
			}
			return MENU_RET_COMPLETE;
	}
	return MENU_RET_INCOMPLETE;
}

/**
********************************************************************************
* @brief    Calibration 12 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   국가세팅 (100 보다 크면 100을 -값으로 인식 예, 109 = -9, 116 = -16)
********************************************************************************
*/
#ifdef USE_CREEP_MENU
#ifdef USE_CREEP_TIME
INT8U cal121Func (KEY_TYPE key)
{	
	INT32S tempInt32s;
	INT16S tempInt16s;
	
	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			tempInt16s = (INT16S)AdReadParam(CREEP_COMP_VALUE);
			if (tempInt16s < 0) tempInt16s = -tempInt16s + 100;
			EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)tempInt16s), 0, 199, 0, DEC_TYPE_DOT, 0);
			MenuFuncState++;
			break;
			
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData((INT32U *)&tempInt32s); 		
			tempInt16s = (INT16S)(tempInt32s);
		
			if (tempInt16s > 100 && tempInt16s < 200)
			{
				tempInt16s = -(tempInt16s - 100);
				AdWriteParam(CREEP_COMP_VALUE, (INT32S)tempInt16s);
				AdWriteParam(CREEP_COMP_TIME, 30);
			}
			else if (tempInt16s > 0 && tempInt16s < 100)
			{
				AdWriteParam(CREEP_COMP_VALUE, (INT32S)tempInt16s);
				AdWriteParam(CREEP_COMP_TIME, 30);
			}
			else
			{
				AdWriteParam(CREEP_COMP_VALUE, 0);
				AdWriteParam(CREEP_COMP_TIME, 0);
			}
			return MENU_RET_COMPLETE;
			break;
	}
	return MENU_RET_INCOMPLETE;
}

/**
********************************************************************************
* @brief    Calibration 12 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   국가세팅 (100 보다 크면 100을 -값으로 인식 예, 109 = -9, 116 = -16)
********************************************************************************
*/
INT8U cal122Func (KEY_TYPE key)
{
	INT32S tempInt32s;
	INT8U tempInt8u;
	
	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			tempInt8u = (INT8U)AdReadParam(CREEP_COMP_TIME);
			EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)tempInt8u), 0, 60, 0, DEC_TYPE_DOT, 0);
			MenuFuncState++;
			break;
			
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData((INT32U *)&tempInt32s);
			tempInt8u = (INT8U)(tempInt32s);
			AdWriteParam(CREEP_COMP_TIME, (INT32S)tempInt8u);
			return MENU_RET_COMPLETE;
			break;
	}
	return MENU_RET_INCOMPLETE;
}

#else

INT8U cal12Func (KEY_TYPE key)
{	
	INT32S tempInt32s;
	INT16S tempInt16s;
	
	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			tempInt16s = (INT16S)AdReadParam(CREEP_COMP_VALUE);
			if (tempInt16s < 0) tempInt16s = -tempInt16s + 100;
			EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)tempInt16s), 0, 199, 0, DEC_TYPE_DOT, 0);
			MenuFuncState++;
			break;
			
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData((INT32U *)&tempInt32s); 		
			tempInt16s = (INT16S)(tempInt32s);
		
			if (tempInt16s > 100 && tempInt16s < 200)
			{
				tempInt16s = -(tempInt16s - 100);
				AdWriteParam(CREEP_COMP_VALUE, (INT32S)tempInt16s);
				AdWriteParam(CREEP_COMP_TIME, 30);
			}
			else if (tempInt16s > 0 && tempInt16s < 100)
			{
				AdWriteParam(CREEP_COMP_VALUE, (INT32S)tempInt16s);
				AdWriteParam(CREEP_COMP_TIME, 30);
			}
			else
			{
				AdWriteParam(CREEP_COMP_VALUE, 0);
				AdWriteParam(CREEP_COMP_TIME, 0);
			}
			return MENU_RET_COMPLETE;
			break;
	}
	return MENU_RET_INCOMPLETE;
}
#endif
#endif

/**
********************************************************************************
* @brief    Calibration 12 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   국가세팅 (100 보다 크면 100을 -값으로 인식 예, 109 = -9, 116 = -16)
********************************************************************************
*/
#ifdef USE_TEMP_COMP
/*
INT8U cal12Func (KEY_TYPE key)
{	
	INT32S tempInt32s;
	INT8S tempInt8s;
	INT8U tempInt8u;
	
	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			tempInt8u = (INT8U)AdReadParam(TEMP_COMP_ENABLE);
			EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)tempInt8u), 0, 1, 0, DEC_TYPE_DOT, 0);
			MenuFuncState++;
			break;
			
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData(&tempInt32s); 		
			tempInt8u = (INT8U)(tempInt32s);
			AdWriteParam(TEMP_COMP_ENABLE, (INT32S)tempInt8u);
			//return MENU_RET_COMPLETE;
			MenuFuncState++;
			break;

		case 2:
			tempInt8s = TempDrvReadData();
			DispSetNum(MenuDataWinNum, (INT32S)tempInt8s, 0);
			if(key == MenuKeyEnter) return MENU_RET_COMPLETE;
			break;
	}
	return MENU_RET_INCOMPLETE;
}
*/
#endif

#ifdef USE_TEMP_COMP_MWIIH
/**
********************************************************************************
* @brief    Calibration 15 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   온도보상
********************************************************************************
*/

INT8U cal15Func(KEY_TYPE key)
{
	INT16S temp;
    INT32S tempInt32s;
	INT8S tempInt8s;
	INT8U tempInt8u;
	static INT8U dispFiltBackup;
    
    temp = TempDrvReadData();

	if (key == MenuKeyC)
	{
		calSetDispFilt(dispFiltBackup);
		return MENU_RET_COMPLETE; 
	}
						
    if(TempCompState == 0)
    {
        switch (tempState)
        {
            case 0:
                DispSetConstStr(MenuStrWinNum,"M-Temp");
                break;
                
            case 1:
                DispSetConstStr(MenuStrWinNum,"H-Temp");
                break;
            
            case 2:
                DispSetConstStr(MenuStrWinNum,"L-Temp");
                break;
        }
    }
	if ((key == KEY_TARE) || (key == KEY_ZERO))
    {
        switch (key)
        {
            case KEY_TARE:
                tempState++;
                if(tempState > 2) tempState = 0;
                break;
            case KEY_ZERO:
				if (TempCompState != 99) {
	                TempCompState = 99;
                	return calProc(KEY_NOT_PRESSED, CAL_PROC_TEMP);
				}
                break;
			default:
				break;
        }
    }
    if (TempCompState == 99)
		calProc(key, CAL_PROC_TEMP);
}
#endif


#define CAL_FILT_VALUE	5//05 : no shock=0, AD_MAF_SET_TABLE_INDEX_MAX-1=5
void calSetDispFilt(INT8U filtValue)
{
	DispFiltParam.filtIndex = filtValue;
	DispFiltWriteParam();
	AdInit();
}

#ifdef USE_EXP_HYST_COMP
/**
********************************************************************************
* @brief    Calibration 3 proc 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   
********************************************************************************
*/
INT8U calProcExpHyst(KEY_TYPE key)
{
	INT32S rawData;
	INT8U retValue;
	INT8U runCountDispFlag;
	static INT8U dispFiltBackup;
        INT8U order;

	if (key == MenuKeyC)
	{
		calSetDispFilt(dispFiltBackup);
		return MENU_RET_COMPLETE; 
	}

        order = (INT8U)AdReadParam(HYST_COMP_ORDER);
	rawData = AdData.rawFilteredData;
	retValue = MENU_RET_INCOMPLETE;
	runCountDispFlag = 0;

	if (MenuFuncState%2 == 0)
	{
              if (MenuFuncState == 0)
              {
                      dispFiltBackup = DispFiltParam.filtIndex;//backup current filter value
              }

              DispSetConstStrWithDecNum(MenuStrWinNum, "PT- ", MenuFuncState/2, 1); 
              if (key == MenuKeyEnter) 
              {
                      TimerSetDelay100ms(TIMER_DELAY_100MS_CAL, 10);
                      MenuFuncState++;
                      calWaitCount = 0;
                      calSetDispFilt(CAL_FILT_VALUE);
              }
	}
        else
	{
              if (TimerCheckDelay100ms(TIMER_DELAY_100MS_CAL))
              {
                      TimerSetDelay100ms(TIMER_DELAY_100MS_CAL, 10);
                      calWaitCount++;
                      if (!AdData.rawStableFlag)
                      {
                              calWaitCount = 0;
                      }
                      if (calWaitCount == CAL_CHECK_COUNT) 
                      {					
                              hystAdValue[MenuFuncState/2] = rawData;
                              if (rawData == 0)
                              {
                                      DispSetError(ERROR_NUM_CAL_RANGE);
                                      retValue = MENU_RET_COMPLETE;
                              }
                              if ((MenuFuncState / 2) == (order * 2 - 1))
                              {
                                      if (AdWriteHystCalParam(&hystAdValue[0]))
                                      {
                                              DispSetError(ERROR_NUM_CAL_RANGE);
                                      }
                                      retValue = MENU_RET_COMPLETE;                              
                              }
                              else
                              {
                                       MenuFuncState++;		
                              }
                      }
              }

              if (calWaitCount != 0)
              {
                      runCountDispFlag = 1;
              }
        }

	if (runCountDispFlag)
	{
		DispSetNum(MenuAuxWinNum, calWaitCount + 1, 0);
		DispSetNum(MenuDataWinNum, rawData, 0);
	}

	if (retValue == MENU_RET_COMPLETE)
	{
		calSetDispFilt(dispFiltBackup);
	}
	return retValue;	
}
#endif
/**
********************************************************************************
* @brief    Calibration 3 proc 함수\n
* @param    key : Enter key, C key
* @return   Menu 함수 반환값
* @remark   
********************************************************************************
*/
INT8U calProc(KEY_TYPE key, INT8U cMode)
{
	INT32S rawData;
	INT8U retValue;
	INT8U runCountDispFlag;
	static INT8U dispFiltBackup;

	if (key == MenuKeyC)
	{
		calSetDispFilt(dispFiltBackup);
		return MENU_RET_COMPLETE; 
	}

	if (cMode == CAL_PROC_HYST) 
	{
		rawData = AdData.rawFilteredData;
	}
	else if (cMode == CAL_PROC_LINEAR)
	{
		rawData = AdData.rawHystCompData;
	}
	else if (cMode == CAL_PROC_SPAN)
	{
		rawData = AdData.rawLinearCompData;
	}
	else if (cMode == CAL_PROC_PERCENT)
	{
		rawData = AdData.rawLinearCompData;
	}
	else if (cMode == CAL_PROC_WEIGHT)
	{
		rawData = AdData.rawLinearCompData;
	}
#ifdef USE_TEMP_COMP_MWIIH
	else if(cMode == CAL_PROC_TEMP)
	{
		rawData = AdData.rawLinearCompData;
	}
#endif
	else
	{
		calSetDispFilt(dispFiltBackup);
		return MENU_RET_COMPLETE;
	}

	retValue = MENU_RET_INCOMPLETE;
	runCountDispFlag = 0;

	switch (MenuFuncState)
	{
		case 0:
			dispFiltBackup = DispFiltParam.filtIndex;//backup current filter value

			DispSetConstStr(MenuStrWinNum, "ZERO");
			if (key == MenuKeyEnter) 
			{
				TimerSetDelay100ms(TIMER_DELAY_100MS_CAL, 10);
				MenuFuncState++;
				calWaitCount = 0;
				calSetDispFilt(CAL_FILT_VALUE);
			}
			break;
		case 1:
			if (TimerCheckDelay100ms(TIMER_DELAY_100MS_CAL))
			{
				TimerSetDelay100ms(TIMER_DELAY_100MS_CAL, 10);
				calWaitCount++;
				if (!AdData.rawStableFlag)
				{
					calWaitCount = 0;
				}
				if (calWaitCount == CAL_CHECK_COUNT) 
				{					
					zeroValue = rawData;
					if (zeroValue == 0)
					{
						DispSetError(ERROR_NUM_CAL_RANGE);
						retValue = MENU_RET_COMPLETE;
					}
#ifdef USE_TEMP_COMP_MWIIH
					if (cMode == CAL_PROC_SPAN || cMode == CAL_PROC_PERCENT || cMode == CAL_PROC_WEIGHT || cMode == CAL_PROC_TEMP)
#else
					if (cMode == CAL_PROC_SPAN || cMode == CAL_PROC_PERCENT || cMode == CAL_PROC_WEIGHT)
#endif
					{
						MenuFuncState = 4;
					}
					else
					{
						MenuFuncState++;					
					}
				}
			}

			if (calWaitCount != 0)
			{
				runCountDispFlag = 1;
			}
			break;
		case 2:
			DispSetConstStr(MenuStrWinNum, "MIDUP");
			if (key == MenuKeyEnter) 
			{
				TimerSetDelay100ms(TIMER_DELAY_100MS_CAL, 10);
				calWaitCount = 0;
				MenuFuncState++;
			}
			break;
		case 3:
			if (TimerCheckDelay100ms(TIMER_DELAY_100MS_CAL))
			{
				TimerSetDelay100ms(TIMER_DELAY_100MS_CAL, 10);
				calWaitCount++;
				if (!AdData.rawStableFlag)
				{
					calWaitCount = 0;
				}
				if (calWaitCount == CAL_CHECK_COUNT + 1) 
				{
					midUpValue = rawData;
					MenuFuncState++;
				}
			}
			else
			{
				runCountDispFlag = 1;
			}
			break;
		case 4:
			if(cMode == CAL_PROC_PERCENT) DispSetConstStr(MenuStrWinNum, "PSPAN");
			else if(cMode == CAL_PROC_WEIGHT) DispSetConstStr(MenuStrWinNum, "WSPAN");
			else DispSetConstStr(MenuStrWinNum, "FULL");
			if (key == MenuKeyEnter)
			{
				calWaitCount = 0;
				MenuFuncState++;
				TimerSetDelay100ms(TIMER_DELAY_100MS_CAL, 10);
			}
			break;
		case 5:
			if (TimerCheckDelay100ms(TIMER_DELAY_100MS_CAL))
			{
				TimerSetDelay100ms(TIMER_DELAY_100MS_CAL, 10);
				calWaitCount++;
				if (!AdData.rawStableFlag) 
				{
					calWaitCount = 0;
				}
				if (calWaitCount == CAL_CHECK_COUNT + 2) 
				{
					spanValue = rawData;
					if (cMode == CAL_PROC_LINEAR)
					{
						if (AdWriteLinearCalParam(zeroValue, midUpValue, spanValue))
						{
							DispSetError(ERROR_NUM_CAL_RANGE);
						}
						retValue = MENU_RET_COMPLETE;
					}
					else if (cMode == CAL_PROC_SPAN)
					{
						if (AdWriteSpanCalParam(cMode, 0, zeroValue, spanValue))
						{
							DispSetError(ERROR_NUM_CAL_RANGE);
						}
						retValue = MENU_RET_COMPLETE;
					}
					else if (cMode == CAL_PROC_PERCENT)
					{
						spanValue = zeroValue + ((spanValue - zeroValue) * 100L) / (INT32S)calPercent;
						if (AdWriteSpanCalParam(cMode, (INT16U)calPercent, zeroValue, spanValue))
						{
							DispSetError(ERROR_NUM_CAL_RANGE);
						}
						retValue = MENU_RET_COMPLETE;
					}
					else if (cMode == CAL_PROC_WEIGHT)
					{
						spanValue = zeroValue + (INT32S)((FP64)(spanValue - zeroValue) * (FP64)((FP64)CalParam.capa / (FP64)calWeight));
						if (AdWriteSpanCalParam(cMode, calWeight, zeroValue, spanValue))
						{
							DispSetError(ERROR_NUM_CAL_RANGE);
						}
						retValue = MENU_RET_COMPLETE;
					}		
#ifdef USE_TEMP_COMP_MWIIH
					else if (cMode == CAL_PROC_TEMP)
					{
						if ((CalParam.calSpanLimit < spanValue) || (CalParam.calZeroLimit > zeroValue)
							|| (zeroValue > spanValue))
						{
							DispSetError (ERROR_NUM_CAL_RANGE);
						}
						else
						{
							if (tempState == 0)
							{
								TempParam.calZeroValueAtMidTemp =  zeroValue;
								TempParam.calSpanValueAtMidTemp =  spanValue;
								TempParam.midTemp = TempDrvReadData();
							}
							else if (tempState == 1)
							{
								TempParam.calZeroValueAtHighTemp = zeroValue;
								TempParam.calSpanValueAtHighTemp = spanValue;
								TempParam.highTemp = TempDrvReadData();
							}
							else if (tempState == 2)
							{
								TempParam.calZeroValueAtLowTemp = zeroValue;
								TempParam.calSpanValueAtLowTemp = spanValue;
								TempParam.lowTemp = TempDrvReadData();
							}
							TempWriteParam();
						}
						TempCompState = 0;
						retValue = MENU_RET_COMPLETE;
					}
#endif

					MenuFuncState++;
				}
			}
			else
			{
				runCountDispFlag = 1;
			}
			break;
		case 6:
			DispSetConstStr(MenuStrWinNum, "MIDDN");
			if (key == MenuKeyEnter)
			{
				calWaitCount = 0;
				MenuFuncState++;
				TimerSetDelay100ms(TIMER_DELAY_100MS_CAL, 10);
			}
			break;
		case 7:
			if (TimerCheckDelay100ms(TIMER_DELAY_100MS_CAL))
			{
				TimerSetDelay100ms(TIMER_DELAY_100MS_CAL, 10);
				calWaitCount++;
				if (!AdData.rawStableFlag) calWaitCount = 0;
				if (calWaitCount == CAL_CHECK_COUNT + 1)
				{
					midDownValue = rawData;
#ifdef USE_EXP_HYST_COMP                                      
#else                                        
					if (AdWriteHystCalParam(zeroValue, midUpValue, midDownValue, spanValue))
#endif                                          
					{
						DispSetError(ERROR_NUM_CAL_RANGE);
					}
					retValue = MENU_RET_COMPLETE;
				}
			}
			else
			{
				runCountDispFlag = 1;
			}
			break;
	}

	if (runCountDispFlag)
	{
		DispSetNum(MenuAuxWinNum, calWaitCount + 1, 0);
		DispSetNum(MenuDataWinNum, rawData, 0);
	}

	if (retValue == MENU_RET_COMPLETE)
	{
		calSetDispFilt(dispFiltBackup);
	}
	return retValue;	
}

/**
********************************************************************************
* @brief    Calibration 4-1 Parameter set 함수\n
* @param    Write(1) or read(0), Write data(read시에는 0) 
* @return   Read시에 읽은 데이더를 반환한다.
* @remark   bit 7,6 : Initial zero range\n
*           bit 5 : Full tare\n
*           bit 3 : custom tare under user's demand\n
*			bit 2 : successive or one time tare
********************************************************************************
*/
INT8U cal41SetParam(INT8U writeFlag, INT8U regData)
{
	INT8U tempInt8u;
	INT8U tempBitData;	
	INT32S tempInt32s;
	
	//bit 7,6 : Initial zero range
	if (writeFlag == 1)
	{
		tempBitData = (regData & 0xc0) >> 6;
		switch (tempBitData)
		{
			case 0:
				tempInt8u = 2;//+-2%
				break;
			case 1:	
				tempInt8u = 3;//+-3%
				break;
			case 2:
			default:				
				tempInt8u = 10;//+-10%
				break;
			case 3:
#ifdef	USE_CHANGE_ZERO_RANGE_SETTING
				tempInt8u = 15;//+15%,-5%
#else
				tempInt8u = 5;//+-5%
#endif
				break;
		}
		AdWriteParam(INIT_ZERO_NUM, (INT32S)tempInt8u);
	}
	else
	{
		tempInt8u = (INT8U)AdReadParam(INIT_ZERO_NUM);
		switch (tempInt8u)
		{
			case 2:	
				tempBitData = 0;//+-2%
				break;
			case 3:	
				tempBitData = 1;//+-3%
				break;
			case 10:	
			default:
				tempBitData = 2;//+-10%
				break;
#ifdef	USE_CHANGE_ZERO_RANGE_SETTING
			case 15:	
				tempBitData = 3;//+15%, -5%
#else
			case 5:
				tempBitData = 3;//+-5%
#endif
				break;
		}
		tempBitData = tempBitData << 6;
		regData = (regData & ~(0xc0)) | tempBitData;
	}
	
	//bit 5 : Last digit invalid enable
	if (writeFlag == 1)
	{		
		tempBitData = (regData & 0x20) >> 2;
		tempInt8u = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_3RD);
		tempBitData = (tempInt8u & ~(0x08)) |tempBitData;		
		ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_3RD, tempBitData);
	}
	else
	{
		tempInt8u = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_3RD);
		tempBitData = (tempInt8u & 0x08) << 2;
		regData = (regData & ~(0x20)) | tempBitData;
	}

	//bit 4 : Key zero percent
	if (writeFlag == 1)
	{		
		tempBitData = (regData & 0x10) >> 4;
		if(tempBitData) tempBitData = 2;		//+-2% default (1)
		else tempBitData = 3;				//+-3% 스리랑카 인증  (0)
		AdWriteParam(ZERO,(INT32S)(tempBitData));
	}
	else
	{
		tempInt32s = AdReadParam(ZERO);
		if (tempInt32s == 2) tempBitData = 1;	//+-2% default (1)
		else tempBitData = 0;					//+-3% 스리랑카 인증  (0)
		tempBitData = tempBitData << 4;
		regData = (regData & ~(0x10)) | tempBitData;
	}
	
	//bit 3 : custom tare under user's demand
	//bit 2 : successive or one time tare
	if (writeFlag == 1)
	{		
		tempBitData = (regData & 0x0c) >> 2;
		AdWriteParam(TARE_SUCCESSIVE,(INT32S)(tempBitData));
	}
	else
	{
		tempBitData = (INT8U)AdReadParam(TARE_SUCCESSIVE);
		tempBitData = tempBitData << 2;
		regData = (regData & ~(0x0c)) | tempBitData;
	}
	
	//bit 1 ,bit 0 : zero mark indicating (0 : gross zero, 1 : net zero, 2 : all zero)
	if (writeFlag == 1)
	{		
		tempBitData = (regData & 0x03);
		AdWriteParam(ZERO_MARK_INDICATOR,(INT32S)(tempBitData));
	}
	else
	{
		tempBitData = (INT8U)AdReadParam(ZERO_MARK_INDICATOR);
		regData = (regData & ~(0x03)) | tempBitData;
	}
	return regData;
}

#ifdef USE_CHINA_LIMIT_CAL
#ifdef USE_RTC
INT8U cal13Func (KEY_TYPE key)
{	
	INT32U tempInt32u;
	INT8U  tempInt8u;
	INT16U tempInt16u;
	
	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			tempInt16u = (INT8U)ParamReadByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_VALID_START_DATE);
			DispSetConstStr(MenuStrWinNum, " YEAR");
			EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)tempInt16u), 0, 99, 0, DEC_TYPE_DOT, 0);
			MenuFuncState++;
			break;
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData(&tempInt32u);
			tempInt8u = (INT8U)tempInt32u;
			ParamWriteByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_VALID_START_DATE, tempInt8u); //write YEAR
			tempInt16u = (INT8U)ParamReadByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_VALID_START_DATE+1);
			DispSetConstStr(MenuStrWinNum, "MONTH");

			EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)tempInt16u), 0, 12, 0, DEC_TYPE_DOT, 0);
			MenuFuncState++;
			break;
		case MENU_EDITOR_COMPLETE_STATE+1:
			EditorGetData(&tempInt32u);
			tempInt8u = (INT8U)tempInt32u;
			ParamWriteByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_VALID_START_DATE+1, tempInt8u); //write YEAR
			tempInt16u = (INT8U)ParamReadByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_VALID_START_DATE+2);
			DispSetConstStr(MenuStrWinNum, " DATE");

			EditorSetDataAndRun(MenuDataWinNum, DEC, ((INT32U)tempInt16u), 0, 31, 0, DEC_TYPE_DOT, 0);
			MenuFuncState++;
			break;
		case MENU_EDITOR_COMPLETE_STATE+2:
			EditorGetData(&tempInt32u);
			tempInt8u = (INT8U)tempInt32u;
			ParamWriteByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_VALID_START_DATE+2, tempInt8u); //write YEAR
			return MENU_RET_COMPLETE;
	}
	return MENU_RET_INCOMPLETE;
}
#endif
#endif

#ifdef TURKEY_BLOCK_FUNCTION_KEY
INT8U cal14Func (KEY_TYPE key)
{	
	INT32U  tempInt32u;

	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			tempInt32u = (INT32U)ParamReadByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BLINK_COUNT);			
			EditorSetDataAndRun(MenuDataWinNum, HEX, (INT8U)tempInt32u, 0, 0xFF, 0, 0, 0);
			if (MenuWinIsOne) EditorSetConstStr("C14");
			MenuFuncState = MENU_EDITOR_COMPLETE_STATE;
			break;
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData(&tempInt32u);
			ParamWriteByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BLINK_COUNT, (INT8U)tempInt32u);
			return MENU_RET_COMPLETE;
	}
	return MENU_RET_INCOMPLETE;	
}
#endif

#ifdef USE_ZERO_TRACKING_RANGE_SETTING
INT8U cal15Func (KEY_TYPE key)
{	
	INT32U  tempInt32u;

	switch (MenuFuncState)
	{
		case MENU_EDITOR_INIT_STATE:
			tempInt32u = RangeParam.zeroTrackingRangeSetting;
			EditorSetDataAndRun(MenuDataWinNum, DEC, tempInt32u, 0, 10, 0, DEC_TYPE_DOT, 0);
			if (MenuWinIsOne) EditorSetConstStr("ZTR-");
			MenuFuncState = MENU_EDITOR_COMPLETE_STATE;
			break;
		case MENU_EDITOR_COMPLETE_STATE:
			EditorGetData(&tempInt32u);
			RangeParam.zeroTrackingRangeSetting = tempInt32u;
			RangeWriteParam();
			return MENU_RET_COMPLETE;
	}
	return MENU_RET_INCOMPLETE;	
}
#endif

