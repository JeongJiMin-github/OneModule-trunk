/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Linear Compensation API Source File\n
* @file    LinearCompApi.c
* @version 1.0
* @date    2006/12/05
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/12/05 created by HYP \n
********************************************************************************
*/

/*
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
/* Insert include files here */
#include "../Common/Globals.h"
#include "../Common/CommonLib.h"
#include "../DB/StorageApi.h"
#include "../DB/ParamApi.h"
#include "../Communication/DebugApi.h"
#include "LinearCompApi.h"
#include "AdApi.h"

/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief Linear structure를 사용하기 위한 변수*/
LINEAR_PARAM_STRUCT LinearParam;

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
/** @brief 직선성 보정 2차 방정식 Ax^2+Bx+C중 A값*/
FP32 linearConstantA;				
/** @brief 직선성 보정 2차 방정식 Ax^2+Bx+C중 B값*/
FP32 linearConstantB;				
/** @brief 직선성 보정 2차 방정식 Ax^2+Bx+C중 C값*/
FP32 linearConstantC;
/** @brief Linear error function의 시작 지점(영점키를 누른 상태의 보상전Ad, Linear error가 0인 지점)*/
INT32S LinearZeroRawAd;


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
* @brief    Linear compensation parameter들의 default 값을 세팅 해주는 함수\n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void LinearSetDefaultParam(void)
{
	LinearParam.linearZeroValue = 0;
	LinearParam.linearMidValue = 0;
	LinearParam.linearSpanValue = 0;

#ifdef USE_M0516LDN_MCU
    LinearParam.linearRatio.dword = 0x000f0005;	// little endian
#else
	LinearParam.linearRatio.dword = 0x0005000f;	// 5kg/15kg
#endif
	LinearWriteParam();
}

/**
********************************************************************************
* @brief    Linear compensation parameter들을 EEPROM으로 Write하는 함수\n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void LinearWriteParam(void)
{
	ParamWriteData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_LINEAR, (INT8U *)&LinearParam, sizeof(LinearParam));
}

/**
********************************************************************************
* @brief    EEPROM의 Linear compensation parameter들을 RAM으로 Read하는 함수\n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void LinearReadParam(void)
{
	ParamReadData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_LINEAR, (INT8U *)&LinearParam, sizeof(LinearParam));
}

/**
********************************************************************************
* @brief    Linear compensation 변수들을 초기화하는 함수\n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void LinearInitVariable(void)
{
	LinearReadParam();
	
	LinearZeroRawAd = LinearParam.linearZeroValue;
	linearConstantA = 0;
	linearConstantB = 1;
	linearConstantC = 0;
}

/**
********************************************************************************
* @brief    직선성 보상을 수행한다.\n
* @param    rawAd데이터
* @return   보상된 rawAd데이터
* @remark   (Ax+B)x+C = Ax^2+Bx+C의 2차 방정식을 사용하여 직선성 보상을 수행한다.
********************************************************************************
*/
INT32S LinearCompProc(INT32S rawAd)
{
#ifdef USE_EXP_HYST_COMP  
        return rawAd;        
#else
	FP32 linearValue;
	INT32S LinearZeroOffset;
	
	LinearZeroOffset = LinearZeroRawAd - LinearParam.linearZeroValue;
	linearValue = (linearConstantA * (FP32)rawAd + linearConstantB) * (FP32)rawAd + linearConstantC - (FP32)LinearZeroOffset;

	return (INT32S)(linearValue + 0.5) + LinearZeroOffset;
#endif
}

/**
********************************************************************************
* @brief    compensated value f(x) = linearA*x*x + linearB*x + linearC\n
			samples(raw,weight) : (zero,zero), (mid,zero+(full-zero)*ratio), (full, full)\n
			gdSlope compensate error at the calibration\n
			linearA = (1 - r) / (full - mid) - r / (mid - zero)\n
			linearB = 1 - linearA * (full + zero)\n
			linearC = linearA * full * zero\n
			선형 보상 데이터를 만들기 위한 상수 A,B,C를 구하는 함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void LinearCalcVariable(void)
{
	if (LinearParam.linearZeroValue == LinearParam.linearMidValue ||LinearParam.linearMidValue == LinearParam.linearSpanValue)
	{
		return;
	}
	if (LinearParam.linearRatio.word.low == 0) 
	{
		return;
	}
	if (LinearParam.linearMidValue == 0) 
	{
		return;
	}

	linearConstantA = (FP32)(LinearParam.linearRatio.word.low - LinearParam.linearRatio.word.high) / (FP32)(LinearParam.linearSpanValue - LinearParam.linearMidValue) 
						- (FP32)LinearParam.linearRatio.word.high / (FP32)(LinearParam.linearMidValue - LinearParam.linearZeroValue);
	linearConstantA /= (FP32)LinearParam.linearRatio.word.low;
	linearConstantB = 1 - linearConstantA* (FP32)(LinearParam.linearSpanValue + LinearParam.linearZeroValue);
	linearConstantC = linearConstantA * (FP32)LinearParam.linearSpanValue * (FP32)LinearParam.linearZeroValue;
}

/**
********************************************************************************
* @brief    선형보상에서 사용하는 Zero 변수를 Setting하는 함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void LinearSetZero(INT32S lAd)
{
	if (AbsInt32s(lAd - LinearParam.linearZeroValue) < (LinearParam.linearSpanValue - LinearParam.linearZeroValue) / 10)
	{
		LinearZeroRawAd = lAd;
	}
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
