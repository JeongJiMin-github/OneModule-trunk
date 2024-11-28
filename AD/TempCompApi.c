/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Temperature Compensation API Source File\n
* @file    TempCompApi.c
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
#include "AdApi.h"
#include "TempCompApi.h"

/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief 파일내에서 temperature structure를 사용하기 위한 변수*/
#if defined(USE_TEMP_COMP) || defined(USE_TEMP_COMP_MWIIH)
TEMP_PARAM_STRUCT TempParam;
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

#ifdef USE_TEMP_COMP
/** @brief 상온과 고온사이의 스판값 차이 변수*/
INT16S spanDiffMidToHigh;
/** @brief 상온과 고온사이의 영점값 차이 변수*/
INT16S zeroDiffMidToHigh;
/** @brief 상온과 저온사이의 스판값 차이 변수*/
INT16S spanDiffMidToLow;
/** @brief 상온과 저온사이의 영점값 차이 변수*/
INT16S zeroDiffMidToLow;

/** @brief 상온과 고온사이의 온도값 차이 변수*/
INT8S tempDiffMidToHigh;
/** @brief 상온과 저온사이의 온도값 차이 변수*/
INT8S tempDiffMidToLow;

/** @brief Recalibration 온도에서 상온과의 Span 차이 변수*/
INT16S tempCompSpanError;
/** @brief Recalibration 온도에서 상온과의 Zero 차이 변수*/
INT16S tempCompZeroError;
#elif defined (USE_TEMP_COMP_MWIIH)
FP32 Az;
FP32 Bz;
FP32 Cz;
FP32 As;
FP32 Bs;
FP32 Cs;

INT32S ad_zx;
INT32S ad_sx; 
#endif //USE_TEMP_COMP
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
#ifdef USE_TEMP_COMP
/**
********************************************************************************
* @brief    Temperature compensation parameter들의 default 값을 세팅 해주는 함수\n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void TempSetDefaultParam(void)
{
	TempParam.deltaZeroValueAtLowTemp = 0;			
	TempParam.deltaSpanValueAtLowTemp = 0;			
	TempParam.reserved1 = 0;			
	TempParam.reserved2 = 0;			
	TempParam.deltaZeroValueAtHighTemp = 0;			
	TempParam.deltaSpanValueAtHighTemp = 0;			

	TempParam.tempCompEnable = 0;
	TempParam.calAvailable = 0;
	TempParam.highTemp = -100;
	TempParam.lowTemp = -100;
	TempParam.midTemp = -100;
	TempParam.reCalTemp = -100;
	TempWriteParam();
}

/**
********************************************************************************
* @brief    Temperature compensation parameter들을 EEPROM으로 Write하는 함수\n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void TempWriteParam(void)
{
	ParamWriteData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_TEMP, (INT8U *)&TempParam, sizeof(TempParam));
	TempInitVariable();//apply temp variable
}

/**
********************************************************************************
* @brief    EEPROM의 Temperature compensation parameter들을 RAM으로 Read하는 함수\n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void TempReadParam(void)
{
	ParamReadData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_TEMP, (INT8U *)&TempParam, sizeof(TempParam));
}

/**
********************************************************************************
* @brief    온도 보상에 필요한 변수를 초기화하는 함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void TempInitVariable(void)
{
	//Normalized AD Error -> Raw AD Error
	zeroDiffMidToHigh = (INT16S)multiplyDivide(-TempParam.deltaZeroValueAtHighTemp, CalParam.calSpanValue - CalParam.calZeroValue, CalParam.resolution);
	spanDiffMidToHigh = (INT16S)multiplyDivide(-TempParam.deltaSpanValueAtHighTemp, CalParam.calSpanValue - CalParam.calZeroValue, CalParam.resolution);
	spanDiffMidToHigh += zeroDiffMidToHigh;
	zeroDiffMidToLow = (INT16S)multiplyDivide(-TempParam.deltaZeroValueAtLowTemp, CalParam.calSpanValue - CalParam.calZeroValue, CalParam.resolution);
	spanDiffMidToLow = (INT16S)multiplyDivide(-TempParam.deltaSpanValueAtLowTemp, CalParam.calSpanValue - CalParam.calZeroValue, CalParam.resolution);
	spanDiffMidToLow += zeroDiffMidToLow;

	tempDiffMidToHigh = TempParam.midTemp - TempParam.highTemp;
	tempDiffMidToLow = TempParam.midTemp - TempParam.lowTemp ;

	//recalibration(온도 보상 후 다시 상온 calibration 하는것)
	//recalibration 값의 온도 오차 계산
	//온도 보상 procedure에서 오차에 더해주기 위함(raw AD)
	if (TempParam.reCalTemp > TempParam.midTemp) //recalibration 온도가 온도보상 중간 온도보다 크면
	{   //상위쪽 오차 계산
		tempCompSpanError = (INT16S)multiplyDivide((INT32S)spanDiffMidToHigh, (INT32S)(TempParam.midTemp - TempParam.reCalTemp), (INT32S)tempDiffMidToHigh);
		tempCompZeroError = (INT16S)multiplyDivide((INT32S)zeroDiffMidToHigh, (INT32S)(TempParam.midTemp - TempParam.reCalTemp), (INT32S)tempDiffMidToHigh);
	}
	else if (TempParam.reCalTemp < TempParam.midTemp) //recalibration 온도가 온도보상 중간 온도보다 작으면
	{   //하위쪽 오차 계산
		tempCompSpanError = (INT16S)multiplyDivide((INT32S)spanDiffMidToLow, (INT32S)(TempParam.midTemp - TempParam.reCalTemp), (INT32S)tempDiffMidToLow);
		tempCompZeroError = (INT16S)multiplyDivide((INT32S)zeroDiffMidToLow, (INT32S)(TempParam.midTemp - TempParam.reCalTemp), (INT32S)tempDiffMidToLow);
	}
	else //같으면 오차 없음
	{
		tempCompSpanError = 0;
		tempCompZeroError = 0;
	}		

	if (TempParam.tempCompEnable == 0) //온도보상이 켜있지 않으면
	{
		TempParam.calAvailable = 0;	//온도보상 calibration 불가능
	}
	else 
	{
		if ((TempParam.highTemp != -100) && (TempParam.lowTemp != -100) && (TempParam.midTemp != -100))
		{	//온도 보상 calibration이 되었으면 온도 보상 calibration 불가능
			TempParam.calAvailable = 0;
		}
	}
}

/**
********************************************************************************
* @brief    온도 보상을 수행 하는 함수\n
* @param    현재 AD 값
* @return   보상된 AD값
* @remark   현재 AD값을 (re)calibration span, zero기준의 값으로 변경함 
********************************************************************************
*/
INT32S TempCompProc(INT32S lAd, INT8S currTemp)
{
	INT8U tempRange;
	INT32S adZeroValue;
	INT32S adSpanValue;
	
	if (TempParam.tempCompEnable == 0) //온도보상이 켜있지 않으면
	{
		return lAd;	//입력값 반환
	}

	tempRange = 0; 
	
	if (currTemp > TempParam.highTemp) //현재 온도가 온도 보상 상한보다 크면
	{
		currTemp = TempParam.highTemp; //온도 보상 상한까지 보상
	}
	else if (currTemp < TempParam.lowTemp) //현재 온도가 온도 보상 하한보다 작으면
	{
		currTemp = TempParam.lowTemp; //온도 보상 하한까지 보상
	}
	
	if (currTemp > TempParam.midTemp) //현재 온도가 온도 보상 중간 온도보다 크면
	{
		tempRange = CAL_HIGH_TEMP; //상위 온도 보상
	}
	else if (currTemp < TempParam.midTemp) //현재 온도가 온도 보상 중간 온도보다 작으면
	{
		tempRange = CAL_LOW_TEMP; //하위 온도 보상
	}
	else // 같으면 온도 보상 하지 않음
	{
		tempRange = CAL_NO_TEMP;
	}

	switch (tempRange)//현재 온도에서 calibration 한다고 가정할 때 zero, span값 계산
	{
		case CAL_HIGH_TEMP : //상위 온도 보상 + recalibration 오차
			adSpanValue = CalParam.calSpanValue - 
				multiplyDivide(spanDiffMidToHigh, (INT32S)(TempParam.midTemp - currTemp), (INT32S)tempDiffMidToHigh) + (INT32S)tempCompSpanError;
			adZeroValue = CalParam.calZeroValue - 
				multiplyDivide(zeroDiffMidToHigh, (INT32S)(TempParam.midTemp - currTemp), (INT32S)tempDiffMidToHigh) + (INT32S)tempCompZeroError;
			break;
			
		case CAL_LOW_TEMP :	//하위 온도 보상 + recalibration 오차		
			adSpanValue = CalParam.calSpanValue -
				multiplyDivide(spanDiffMidToLow, (INT32S)(TempParam.midTemp - currTemp), (INT32S)tempDiffMidToLow) + (INT32S)tempCompSpanError;
			adZeroValue = CalParam.calZeroValue - 
				multiplyDivide(zeroDiffMidToLow, (INT32S)(TempParam.midTemp - currTemp), (INT32S)tempDiffMidToLow) + (INT32S)tempCompZeroError;
			break;

		default :  //온도 보상 없음	+ recalibration 오차
			adSpanValue = CalParam.calSpanValue + (INT32S)tempCompSpanError;
			adZeroValue = CalParam.calZeroValue + (INT32S)tempCompZeroError;
			break;
	}
	//실제 normalize는 이 보상 다음 이므로 raw AD 반환값을 recalibration 기준으로 비율 맞추어 반환함
	lAd = (INT32S)((FP32)(lAd - adZeroValue) * (FP32)(CalParam.calSpanValue - CalParam.calZeroValue) / (FP32)(adSpanValue - adZeroValue)) + CalParam.calZeroValue;
	return lAd;
}

#elif defined (USE_TEMP_COMP_MWIIH)

/**
********************************************************************************
* @brief    Temperature compensation parameter들을 EEPROM으로 Write하는 함수\n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void TempWriteParam(void)
{
	ParamWriteData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_TEMP, (INT8U *)&TempParam, sizeof(TempParam));
	TempInitVariable();
}

/**
********************************************************************************
* @brief    Temperature compensation parameter들의 default 값을 세팅 해주는 함수\n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void TempSetDefaultParam(void)
{
    TempParam.tempCompEnable = 0;
	TempParam.calZeroValueAtLowTemp = 0;
	TempParam.calSpanValueAtLowTemp = 0;
	TempParam.calZeroValueAtMidTemp = 0;
	TempParam.calSpanValueAtMidTemp = 0;
	TempParam.calZeroValueAtHighTemp = 0;
	TempParam.calSpanValueAtHighTemp = 0;
	TempParam.highTemp = 0;
	TempParam.lowTemp = 0;
	TempParam.midTemp = 0;
	TempParam.reCalTemp = 0;
	
	TempWriteParam();
}

/**
********************************************************************************
* @brief    EEPROM의 Temperature compensation parameter들을 RAM으로 Read하는 함수\n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void TempReadParam(void)
{
	ParamReadData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_TEMP, (INT8U *)&TempParam, sizeof(TempParam));
}

/**
********************************************************************************
* @brief    온도 보상에 필요한 변수를 초기화하는 함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void TempInitVariable(void)
{    
    FP32 fTemp1, fTemp2;
    
	fTemp1 = (float)(TempParam.calZeroValueAtHighTemp - TempParam.calZeroValueAtMidTemp) * (float)(TempParam.midTemp - TempParam.lowTemp) - (float)(TempParam.calZeroValueAtMidTemp - TempParam.calZeroValueAtLowTemp) * (float)(TempParam.highTemp - TempParam.midTemp);
	fTemp2 = (float)(TempParam.highTemp - TempParam.lowTemp) * (float)(TempParam.midTemp - TempParam.lowTemp) * (float)(TempParam.highTemp - TempParam.midTemp);

	Az = fTemp1 / fTemp2;
	Bz = (float)(TempParam.calZeroValueAtHighTemp - TempParam.calZeroValueAtMidTemp) / (float)(TempParam.highTemp - TempParam.midTemp) - Az * (float)(TempParam.highTemp + TempParam.midTemp);
	Cz = (float)TempParam.calZeroValueAtMidTemp - (Az * (float)TempParam.midTemp + Bz) * (float)TempParam.midTemp;
	
	fTemp1 = (float)(TempParam.calSpanValueAtHighTemp - TempParam.calSpanValueAtMidTemp) * (float)(TempParam.midTemp - TempParam.lowTemp) - (float)(TempParam.calSpanValueAtMidTemp - TempParam.calSpanValueAtLowTemp) * (float)(TempParam.highTemp - TempParam.midTemp);
	fTemp2 = (float)(TempParam.highTemp - TempParam.lowTemp) * (float)(TempParam.midTemp - TempParam.lowTemp) * (float)(TempParam.highTemp - TempParam.midTemp);

	As = fTemp1 / fTemp2;
	Bs = (float)(TempParam.calSpanValueAtHighTemp - TempParam.calSpanValueAtMidTemp) / (float)(TempParam.highTemp - TempParam.midTemp) - As * (float)(TempParam.highTemp + TempParam.midTemp);
	Cs = (float)TempParam.calSpanValueAtMidTemp - (As * (float)TempParam.midTemp + Bs) * (float)TempParam.midTemp;
}

/**
********************************************************************************
* @brief    온도 보상을 수행 하는 함수\n
* @param    현재 AD 값
* @return   보상된 AD값
* @remark   현재 AD값을 (re)calibration span, zero기준의 값으로 변경함 
********************************************************************************
*/
INT32S TempCompProc(INT32S lAd, INT16S currTemp)
{
    if (TempParam.tempCompEnable != 1) //온도보상이 켜있지 않으면
	{
		return lAd;	//입력값 반환
	}
    
	ad_zx = ((Az * currTemp + Bz) * (float)currTemp + Cz)  + 0.5;
	ad_sx = ((As * currTemp + Bs) * (float)currTemp + Cs)  + 0.5;
	
	lAd = (long)((float)(TempParam.calSpanValueAtMidTemp - TempParam.calZeroValueAtMidTemp) * (float)(lAd - ad_zx) / (float)(ad_sx - ad_zx) + 0.5) + TempParam.calZeroValueAtMidTemp;

	return lAd;    
}

#endif//#ifdef USE_TEMP_COMP
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
