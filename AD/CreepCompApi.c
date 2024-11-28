/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Creep Compensation API Source File\n
* @file    CreepCompApi.c
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
#include "AdApi.h"
#include "CreepCompApi.h"


/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief Creep structure를 사용하기 위한 변수*/
CREEP_PARAM_STRUCT CreepParam;

/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */

/** @brief 크립보상 시간 Time (단위 : 분)*/
#define MAX_CREEP_TIME 60
/** @brief 최대 크립 보상값 (단위 : Normalized AD, 60000)*/
#define MAX_CREEP_VALUE 99
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
#ifdef USE_CREEP_COMP
/** @brief 크립 타이머 (단위 : AD 속도) */
INT16U creepTimer;
/** @brief 크립 에러값 (단위 : Normalized AD) */
INT32S creepErr;
/** @brief 크립 값 (-)방향 flag */
BOOLEAN flagNegCreep;
/** @brief 크립 타이머 최대값 (단위 : AD 속도)*/
INT16U creepMaxTimer;
/** @brief 크립 최대 값 (단위 : Normalized AD)*/
INT32S maxRawCreep;
#endif
/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
#ifdef USE_CREEP_COMP
static INT32S creepGetErrLimit(INT32S ad, INT32S zeroValue, INT32S spanValue);
static void creepGetShiftedTime(INT32S creep, INT32S maxCreep, INT16U *time);
static INT32S creepGetErr(INT32S prevCreep, INT32S maxCreep, INT16U time);
#endif

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */

#ifdef USE_CREEP_COMP
/**
********************************************************************************
* @brief    Creep compensation parameter들의 default 값을 세팅 해주는 함수\n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void CreepSetDefaultParam(void)
{
	CreepParam.creepTime = 0;
	CreepParam.creep = 0;
	CreepWriteParam();
}

/**
********************************************************************************
* @brief    Creep compensation parameter들을 EEPROM으로 Write하는 함수\n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void CreepWriteParam(void)
{
	ParamWriteData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_CREEP, (INT8U *)&CreepParam, sizeof(CreepParam));
}
/**
********************************************************************************
* @brief    EEPROM의 Creep compensation parameter들을 RAM으로 Read하는 함수\n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void CreepReadParam(void)
{
	ParamReadData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_CREEP, (INT8U *)&CreepParam, sizeof(CreepParam));
}

/**
********************************************************************************
* @brief    크립보상에서 사용하는 변수를 초기화 하는 함수\n
* @param    samplingRate : 단위 Hz (<35 Hz)
* @return   none
* @remark   
********************************************************************************
*/
void CreepInitVariable(INT16U samplingRate)
{	
	CreepReadParam();

	if (CalParam.calSpanValue == CalParam.calZeroValue) 
	{
		CreepParam.creepTime = 0;
	}
	if (CreepParam.creepTime > MAX_CREEP_TIME)
	{
	 	CreepParam.creepTime = 0;
	}
	if (CreepParam.creep > MAX_CREEP_VALUE)
	{
	 	CreepParam.creep = 0;
	}

	creepErr = 0;
	creepTimer = 0;	
	creepMaxTimer = CreepParam.creepTime * 60 * samplingRate;

	CreepCalcMaxRaw();
}

/**
********************************************************************************
* @brief    크립 최대값을 구하는 함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void CreepCalcMaxRaw(void)
{
	INT16S tempCreep;

	if (CreepParam.creep < 0)
	{
		tempCreep = (-CreepParam.creep);
		flagNegCreep = 1;
	}
	else
	{
		tempCreep = CreepParam.creep;
		flagNegCreep = 0;
	}
	maxRawCreep = (INT16U)AdNormalToRaw((INT32S)tempCreep) * 10; //최종 계산 시, 10으로 나눈다.
}
/**
********************************************************************************
* @brief    시간과 현재 무게에 대한 크립 오차를 계산하여 크립보상을 수행한다.\n
* @param    ad : raw filtered ad data
*           stable : raw stable flag
* @return   크립 보상된 ad data
* @remark   
********************************************************************************
*/
INT32S CreepCompProc(INT32S ad, INT8U stable)
{
	INT32S creepErrLimit;
	INT16U time;
	static BOOLEAN prevStable;

	if ((CreepParam.creepTime == 0) || (CreepParam.creep == 0)) 
	{
		return ad;
	}

	creepTimer++;
	if (creepTimer > creepMaxTimer) 
	{
		creepTimer = creepMaxTimer;
	}

	if (stable) 
	{
		time = creepTimer;
		creepErrLimit = creepGetErrLimit(ad, CalParam.calZeroValue, CalParam.calSpanValue);
		
		if (!prevStable) // Unstable=>Stable
		{
			creepGetShiftedTime(creepErr, creepErrLimit, &time);
			creepTimer = time;	// 시간축 Shift
		}

		creepErr = creepGetErr(creepErr, creepErrLimit, time);

		if (creepErr < 0) 
		{
			creepErr = 0;
		}
		else if (creepErr > maxRawCreep) 	
		{
			creepErr = maxRawCreep;		
		}
	}

	if (flagNegCreep) 	
	{
		ad = ad + (creepErr + 5) / 10;
	}
	else
	{
		ad = ad - (creepErr + 5) / 10;
	}
	
	prevStable = stable;

	return ad;	
}

/**
********************************************************************************
* @brief    현재 무게가 가질 수 있는 최대 크립 오차를 계산한다.(30분 크립량)\n
* @param    raw filtered ad data
* @return   최대 크립 오차
* @remark   
********************************************************************************
*/	
INT32S creepGetErrLimit(INT32S ad, INT32S zeroValue, INT32S spanValue)
{
	INT32S creep;
	
	if (ad > spanValue)			
	{
		creep = maxRawCreep;	
	}
	else if (ad < zeroValue)	
	{
		creep = 0;
	}
	else
	{
		creep = multiplyDivide(maxRawCreep, (ad - zeroValue), (spanValue - zeroValue)); 
	}
	
	return creep;
}

/**
********************************************************************************
* @brief    시간축을 Shift하는 함수\n
* @param    크립오차, 최대크립오차, 시간
* @return   none
* @remark   
********************************************************************************
*/
void creepGetShiftedTime(INT32S creep, INT32S maxCreep, INT16U *time)
{
	if (creep <= maxCreep) 
	{
		if (maxCreep == 0) 
		{
			return;
		}
		*time = (INT16U)multiplyDivide(creep, creepMaxTimer, maxCreep);
	} 
	else
	{
		if (maxRawCreep == maxCreep) 
		{
			return;
		}
		*time = (INT16U)multiplyDivide((maxRawCreep-creep), creepMaxTimer, (maxRawCreep-maxCreep));
	}

	if (*time > creepMaxTimer) 
	{
		*time = creepMaxTimer;
	}

	return;
}

/**
********************************************************************************
* @brief    크립 오차를 구하는 함수\n
* @param    이전 크립오차, 최대크립오차, 시간
* @return   현재 크립 오차
* @remark   
********************************************************************************
*/
INT32S creepGetErr(INT32S prevCreep, INT32S maxCreep, INT16U time)
{
	INT32S creep;
	
	creep = prevCreep;
	if (prevCreep < maxCreep) 
	{
		creep = multiplyDivide(maxCreep, (INT32S)time, creepMaxTimer);
	} 
	else if (prevCreep > maxCreep) 
	{
		creep = multiplyDivide((maxCreep-maxRawCreep), (INT32S)time, creepMaxTimer);		
		creep += maxRawCreep;	 
	}	
	return creep;
}
#endif//#ifdef USE_CREEP_COMP

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
