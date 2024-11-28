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
/** @brief ���ϳ����� temperature structure�� ����ϱ� ���� ����*/
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
/** @brief ��°� ��»����� ���ǰ� ���� ����*/
INT16S spanDiffMidToHigh;
/** @brief ��°� ��»����� ������ ���� ����*/
INT16S zeroDiffMidToHigh;
/** @brief ��°� ���»����� ���ǰ� ���� ����*/
INT16S spanDiffMidToLow;
/** @brief ��°� ���»����� ������ ���� ����*/
INT16S zeroDiffMidToLow;

/** @brief ��°� ��»����� �µ��� ���� ����*/
INT8S tempDiffMidToHigh;
/** @brief ��°� ���»����� �µ��� ���� ����*/
INT8S tempDiffMidToLow;

/** @brief Recalibration �µ����� ��°��� Span ���� ����*/
INT16S tempCompSpanError;
/** @brief Recalibration �µ����� ��°��� Zero ���� ����*/
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
* @brief    Temperature compensation parameter���� default ���� ���� ���ִ� �Լ�\n
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
* @brief    Temperature compensation parameter���� EEPROM���� Write�ϴ� �Լ�\n
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
* @brief    EEPROM�� Temperature compensation parameter���� RAM���� Read�ϴ� �Լ�\n
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
* @brief    �µ� ���� �ʿ��� ������ �ʱ�ȭ�ϴ� �Լ�\n
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

	//recalibration(�µ� ���� �� �ٽ� ��� calibration �ϴ°�)
	//recalibration ���� �µ� ���� ���
	//�µ� ���� procedure���� ������ �����ֱ� ����(raw AD)
	if (TempParam.reCalTemp > TempParam.midTemp) //recalibration �µ��� �µ����� �߰� �µ����� ũ��
	{   //������ ���� ���
		tempCompSpanError = (INT16S)multiplyDivide((INT32S)spanDiffMidToHigh, (INT32S)(TempParam.midTemp - TempParam.reCalTemp), (INT32S)tempDiffMidToHigh);
		tempCompZeroError = (INT16S)multiplyDivide((INT32S)zeroDiffMidToHigh, (INT32S)(TempParam.midTemp - TempParam.reCalTemp), (INT32S)tempDiffMidToHigh);
	}
	else if (TempParam.reCalTemp < TempParam.midTemp) //recalibration �µ��� �µ����� �߰� �µ����� ������
	{   //������ ���� ���
		tempCompSpanError = (INT16S)multiplyDivide((INT32S)spanDiffMidToLow, (INT32S)(TempParam.midTemp - TempParam.reCalTemp), (INT32S)tempDiffMidToLow);
		tempCompZeroError = (INT16S)multiplyDivide((INT32S)zeroDiffMidToLow, (INT32S)(TempParam.midTemp - TempParam.reCalTemp), (INT32S)tempDiffMidToLow);
	}
	else //������ ���� ����
	{
		tempCompSpanError = 0;
		tempCompZeroError = 0;
	}		

	if (TempParam.tempCompEnable == 0) //�µ������� ������ ������
	{
		TempParam.calAvailable = 0;	//�µ����� calibration �Ұ���
	}
	else 
	{
		if ((TempParam.highTemp != -100) && (TempParam.lowTemp != -100) && (TempParam.midTemp != -100))
		{	//�µ� ���� calibration�� �Ǿ����� �µ� ���� calibration �Ұ���
			TempParam.calAvailable = 0;
		}
	}
}

/**
********************************************************************************
* @brief    �µ� ������ ���� �ϴ� �Լ�\n
* @param    ���� AD ��
* @return   ����� AD��
* @remark   ���� AD���� (re)calibration span, zero������ ������ ������ 
********************************************************************************
*/
INT32S TempCompProc(INT32S lAd, INT8S currTemp)
{
	INT8U tempRange;
	INT32S adZeroValue;
	INT32S adSpanValue;
	
	if (TempParam.tempCompEnable == 0) //�µ������� ������ ������
	{
		return lAd;	//�Է°� ��ȯ
	}

	tempRange = 0; 
	
	if (currTemp > TempParam.highTemp) //���� �µ��� �µ� ���� ���Ѻ��� ũ��
	{
		currTemp = TempParam.highTemp; //�µ� ���� ���ѱ��� ����
	}
	else if (currTemp < TempParam.lowTemp) //���� �µ��� �µ� ���� ���Ѻ��� ������
	{
		currTemp = TempParam.lowTemp; //�µ� ���� ���ѱ��� ����
	}
	
	if (currTemp > TempParam.midTemp) //���� �µ��� �µ� ���� �߰� �µ����� ũ��
	{
		tempRange = CAL_HIGH_TEMP; //���� �µ� ����
	}
	else if (currTemp < TempParam.midTemp) //���� �µ��� �µ� ���� �߰� �µ����� ������
	{
		tempRange = CAL_LOW_TEMP; //���� �µ� ����
	}
	else // ������ �µ� ���� ���� ����
	{
		tempRange = CAL_NO_TEMP;
	}

	switch (tempRange)//���� �µ����� calibration �Ѵٰ� ������ �� zero, span�� ���
	{
		case CAL_HIGH_TEMP : //���� �µ� ���� + recalibration ����
			adSpanValue = CalParam.calSpanValue - 
				multiplyDivide(spanDiffMidToHigh, (INT32S)(TempParam.midTemp - currTemp), (INT32S)tempDiffMidToHigh) + (INT32S)tempCompSpanError;
			adZeroValue = CalParam.calZeroValue - 
				multiplyDivide(zeroDiffMidToHigh, (INT32S)(TempParam.midTemp - currTemp), (INT32S)tempDiffMidToHigh) + (INT32S)tempCompZeroError;
			break;
			
		case CAL_LOW_TEMP :	//���� �µ� ���� + recalibration ����		
			adSpanValue = CalParam.calSpanValue -
				multiplyDivide(spanDiffMidToLow, (INT32S)(TempParam.midTemp - currTemp), (INT32S)tempDiffMidToLow) + (INT32S)tempCompSpanError;
			adZeroValue = CalParam.calZeroValue - 
				multiplyDivide(zeroDiffMidToLow, (INT32S)(TempParam.midTemp - currTemp), (INT32S)tempDiffMidToLow) + (INT32S)tempCompZeroError;
			break;

		default :  //�µ� ���� ����	+ recalibration ����
			adSpanValue = CalParam.calSpanValue + (INT32S)tempCompSpanError;
			adZeroValue = CalParam.calZeroValue + (INT32S)tempCompZeroError;
			break;
	}
	//���� normalize�� �� ���� ���� �̹Ƿ� raw AD ��ȯ���� recalibration �������� ���� ���߾� ��ȯ��
	lAd = (INT32S)((FP32)(lAd - adZeroValue) * (FP32)(CalParam.calSpanValue - CalParam.calZeroValue) / (FP32)(adSpanValue - adZeroValue)) + CalParam.calZeroValue;
	return lAd;
}

#elif defined (USE_TEMP_COMP_MWIIH)

/**
********************************************************************************
* @brief    Temperature compensation parameter���� EEPROM���� Write�ϴ� �Լ�\n
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
* @brief    Temperature compensation parameter���� default ���� ���� ���ִ� �Լ�\n
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
* @brief    EEPROM�� Temperature compensation parameter���� RAM���� Read�ϴ� �Լ�\n
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
* @brief    �µ� ���� �ʿ��� ������ �ʱ�ȭ�ϴ� �Լ�\n
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
* @brief    �µ� ������ ���� �ϴ� �Լ�\n
* @param    ���� AD ��
* @return   ����� AD��
* @remark   ���� AD���� (re)calibration span, zero������ ������ ������ 
********************************************************************************
*/
INT32S TempCompProc(INT32S lAd, INT16S currTemp)
{
    if (TempParam.tempCompEnable != 1) //�µ������� ������ ������
	{
		return lAd;	//�Է°� ��ȯ
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
