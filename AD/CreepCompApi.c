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
/** @brief Creep structure�� ����ϱ� ���� ����*/
CREEP_PARAM_STRUCT CreepParam;

/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */

/** @brief ũ������ �ð� Time (���� : ��)*/
#define MAX_CREEP_TIME 60
/** @brief �ִ� ũ�� ���� (���� : Normalized AD, 60000)*/
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
/** @brief ũ�� Ÿ�̸� (���� : AD �ӵ�) */
INT16U creepTimer;
/** @brief ũ�� ������ (���� : Normalized AD) */
INT32S creepErr;
/** @brief ũ�� �� (-)���� flag */
BOOLEAN flagNegCreep;
/** @brief ũ�� Ÿ�̸� �ִ밪 (���� : AD �ӵ�)*/
INT16U creepMaxTimer;
/** @brief ũ�� �ִ� �� (���� : Normalized AD)*/
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
* @brief    Creep compensation parameter���� default ���� ���� ���ִ� �Լ�\n
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
* @brief    Creep compensation parameter���� EEPROM���� Write�ϴ� �Լ�\n
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
* @brief    EEPROM�� Creep compensation parameter���� RAM���� Read�ϴ� �Լ�\n
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
* @brief    ũ�����󿡼� ����ϴ� ������ �ʱ�ȭ �ϴ� �Լ�\n
* @param    samplingRate : ���� Hz (<35 Hz)
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
* @brief    ũ�� �ִ밪�� ���ϴ� �Լ�\n
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
	maxRawCreep = (INT16U)AdNormalToRaw((INT32S)tempCreep) * 10; //���� ��� ��, 10���� ������.
}
/**
********************************************************************************
* @brief    �ð��� ���� ���Կ� ���� ũ�� ������ ����Ͽ� ũ�������� �����Ѵ�.\n
* @param    ad : raw filtered ad data
*           stable : raw stable flag
* @return   ũ�� ����� ad data
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
			creepTimer = time;	// �ð��� Shift
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
* @brief    ���� ���԰� ���� �� �ִ� �ִ� ũ�� ������ ����Ѵ�.(30�� ũ����)\n
* @param    raw filtered ad data
* @return   �ִ� ũ�� ����
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
* @brief    �ð����� Shift�ϴ� �Լ�\n
* @param    ũ������, �ִ�ũ������, �ð�
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
* @brief    ũ�� ������ ���ϴ� �Լ�\n
* @param    ���� ũ������, �ִ�ũ������, �ð�
* @return   ���� ũ�� ����
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
