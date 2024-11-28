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
/** @brief Linear structure�� ����ϱ� ���� ����*/
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
/** @brief ������ ���� 2�� ������ Ax^2+Bx+C�� A��*/
FP32 linearConstantA;				
/** @brief ������ ���� 2�� ������ Ax^2+Bx+C�� B��*/
FP32 linearConstantB;				
/** @brief ������ ���� 2�� ������ Ax^2+Bx+C�� C��*/
FP32 linearConstantC;
/** @brief Linear error function�� ���� ����(����Ű�� ���� ������ ������Ad, Linear error�� 0�� ����)*/
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
* @brief    Linear compensation parameter���� default ���� ���� ���ִ� �Լ�\n
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
* @brief    Linear compensation parameter���� EEPROM���� Write�ϴ� �Լ�\n
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
* @brief    EEPROM�� Linear compensation parameter���� RAM���� Read�ϴ� �Լ�\n
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
* @brief    Linear compensation �������� �ʱ�ȭ�ϴ� �Լ�\n
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
* @brief    ������ ������ �����Ѵ�.\n
* @param    rawAd������
* @return   ����� rawAd������
* @remark   (Ax+B)x+C = Ax^2+Bx+C�� 2�� �������� ����Ͽ� ������ ������ �����Ѵ�.
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
			���� ���� �����͸� ����� ���� ��� A,B,C�� ���ϴ� �Լ�\n
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
* @brief    �������󿡼� ����ϴ� Zero ������ Setting�ϴ� �Լ�\n
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
