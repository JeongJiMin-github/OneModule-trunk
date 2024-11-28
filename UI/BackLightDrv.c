/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Back Light Driver Source File\n
           for xx
* @file    BackLightDrv.c
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
#include "BackLightDrv.h"

#include "../Hardware/HardwareConfig.h"
#ifdef USE_BACK_LIGHT
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
/** @brief KR : Backlight�� �ִ� ��� �� \n
		  US : \n
		  CN :									*/
#define BL_DRV_MAX_BRIGHT		7
/** @brief KR : Backlight�� Default ��� �� \n
		  US : \n
		  CN :									*/
#define BL_DRV_DEFAULT_BRIGHT	3

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
/** @brief KR : Backlight�� ���� On/Off ���� ���� (0-off, 1-on)\n
		  US : \n
		  CN :									*/
static BOOLEAN blStatus;

/** @brief KR : Backlight�� ��� ���� ���� (0-no use, 1-use)\n
		  US : \n
		  CN :									*/
static BOOLEAN blUseFlag;

/** @brief KR : Backlight�� �۵� ���� ���� (0-no use, 1-use)\n
		  US : \n
		  CN :									*/
static BOOLEAN blEnableFlag;

/** @brief KR : Backlight�� ��� ���� Count ����\n
		  US : \n
		  CN :									*/
static data INT8U blPwmCount;//data for high speed in interrupt

/** @brief KR : Backlight�� ��� ���� Max Count ����\n
		  US : \n
		  CN :									*/
static data INT8U blPwmMaxCount;//data for high speed in interrupt

/** @brief KR : Backlight�� ��� ���� ���̺�\n
		  US : \n
		  CN :									*/
static const INT8U code blOnTable[BL_DRV_MAX_BRIGHT+1][BL_DRV_MAX_BRIGHT] =
{
	{ 0, 0, 0, 0, 0, 0, 0},///0
	{ 1, 0, 0, 0, 0, 0, 0},///1
	{ 1, 0, 0, 1, 0, 0, 0},///2
	{ 1, 0, 1, 0, 1, 0, 0},///3
	{ 1, 1, 0, 1, 1, 0, 0},///4
	{ 1, 1, 1, 0, 1, 1, 0},///5
	{ 1, 1, 1, 1, 1, 1, 0},///6
	{ 1, 1, 1, 1, 1, 1, 1} ///7
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
* @brief       KR : Back Light Driver Init �Լ�\n
*                 US : \n
*                 CN : \n
* @param    KR : useFlag : BackLight ��� ����, bright : ��� Level\n
*			     blOffTime : Auto BL off time (0 ������ Check, 0 �̸� Off)\n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : �ʱ⿡ ���� �Ǵ� �Լ�\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BlDrvInit(INT8U bright)
{
	blPwmCount = 0;
	BlDrvSetUseFlag(OFF);
	BlDrvSetBright(bright);
	BlDrvSetEnableFlag(ENABLE);
}

/**
********************************************************************************
* @brief       KR : Back Light Driver Proc �Լ�\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : ���� �ð�(1ms)�� �ѹ� ���� �Ǵ� �Լ�\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BlDrvProc(void)
{
	if ((blStatus == ON) && (blEnableFlag == ON) && blOnTable[blPwmMaxCount][blPwmCount])
	{
#ifdef BACK_LIGHT_ON_ACTIVE_HIGH
		BACK_LIGHT = 1;
#else
		BACK_LIGHT = 0;
#endif
	}
	else
	{
#ifdef BACK_LIGHT_ON_ACTIVE_HIGH
		BACK_LIGHT = 0;
#else
		BACK_LIGHT = 1;
#endif
	}
	blPwmCount++;
	blPwmCount = blPwmCount % BL_DRV_MAX_BRIGHT;
}

/**
********************************************************************************
* @brief       KR : Back Light Bright ���� �Լ�\n
*                 US : \n
*                 CN : \n
* @param    KR : level : ��� (1~BL_DRV_MAX_BRIGHT)\n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BlDrvSetBright(INT8U level)
{
	if (level >= 1 && level <= BL_DRV_MAX_BRIGHT)
	{
		blPwmMaxCount = level;
	}
	else
	{
		blPwmMaxCount = BL_DRV_DEFAULT_BRIGHT;
	}
}

/**
********************************************************************************
* @brief       KR : Back Light Maxinum Bright ������ ��ȯ �Լ�\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : BL_DRV_MAX_BRIGHT\n
*                 US : BL_DRV_MAX_BRIGHT\n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
INT8U BlDrvGetMaxBright(void)
{
	return BL_DRV_MAX_BRIGHT;
}


/**
********************************************************************************
* @brief       KR : Back Light On/Off �Լ�\n
*                 US : \n
*                 CN : \n
* @param    KR : onOff (0-off, 1-on)\n
*                 US : onOff (0-off, 1-on)\n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BlDrvSetOnOff(BOOLEAN onOff)
{
	if (onOff && blUseFlag)
	{
		blStatus = ON;
	}
	else
	{
		blStatus = OFF;
	}
}

/**
********************************************************************************
* @brief       KR : Back Light ���� ���� ��ȯ �Լ�\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : 0-off, 1-on\n
*                 US : \n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
BOOLEAN BlDrvGetStatus(void)
{
	return (blStatus && blEnableFlag);
}

/**
********************************************************************************
* @brief       KR : Back Light ��� ���� ���� �Լ�\n
*                 US : \n
*                 CN : \n
* @param    KR : onOff (0-off, 1-on)\n
*                 US : onOff (0-off, 1-on)\n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : Off�� ��� ������ BL�� ������ Off�ȴ�.\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BlDrvSetUseFlag(BOOLEAN onOff)
{
	blUseFlag = onOff;
	if (onOff == OFF)
	{
		BlDrvSetOnOff(OFF);
	}
}

/**
********************************************************************************
* @brief       KR : Back Light �۵� ���� ���� �Լ�\n
*                 US : \n
*                 CN : \n
* @param    KR : enbale (0-disable, 1-enable)\n
*                 US : enbale (0-disable, 1-enable)\n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : disable�� ��� ������ BL�� ������ Off�ȴ�.\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BlDrvSetEnableFlag(BOOLEAN enable)
{
	blEnableFlag = enable;
}


/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
#else	//#ifdef USE_BACK_LIGHT

/**
********************************************************************************
* @brief       KR : Back Light Driver Init �Լ�\n
*                 US : \n
*                 CN : \n
* @param    KR : useFlag : BackLight ��� ����, bright : ��� Level\n
*			     blOffTime : Auto BL off time (0 ������ Check, 0 �̸� Off)\n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : �ʱ⿡ ���� �Ǵ� �Լ�\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BlDrvInit(INT8U bright)
{
}

/**
********************************************************************************
* @brief       KR : Back Light Driver Proc �Լ�\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : ���� �ð�(1ms)�� �ѹ� ���� �Ǵ� �Լ�\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BlDrvProc(void)
{
}

/**
********************************************************************************
* @brief       KR : Back Light Bright ���� �Լ�\n
*                 US : \n
*                 CN : \n
* @param    KR : level : ��� (1~BL_DRV_MAX_BRIGHT)\n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BlDrvSetBright(INT8U level)
{
}

/**
********************************************************************************
* @brief       KR : Back Light Maxinum Bright ������ ��ȯ �Լ�\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : BL_DRV_MAX_BRIGHT\n
*                 US : BL_DRV_MAX_BRIGHT\n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
INT8U BlDrvGetMaxBright(void)
{
	return 0;
}

/**
********************************************************************************
* @brief       KR : Back Light On/Off �Լ�\n
*                 US : \n
*                 CN : \n
* @param    KR : onOff (0-off, 1-on)\n
*                 US : onOff (0-off, 1-on)\n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BlDrvSetOnOff(BOOLEAN onOff)
{
}

/**
********************************************************************************
* @brief       KR : Back Light ���� ���� ��ȯ �Լ�\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : 0-off, 1-on\n
*                 US : \n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
BOOLEAN BlDrvGetStatus(void)
{
	return OFF;
}

/**
********************************************************************************
* @brief       KR : Back Light ��� ���� ���� �Լ�\n
*                 US : \n
*                 CN : \n
* @param    KR : onOff (0-off, 1-on)\n
*                 US : onOff (0-off, 1-on)\n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : Off�� ��� ������ BL�� ������ Off�ȴ�.\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BlDrvSetUseFlag(BOOLEAN onOff)
{
}

/**
********************************************************************************
* @brief       KR : Back Light �۵� ���� ���� �Լ�\n
*                 US : \n
*                 CN : \n
* @param    KR : enbale (0-disable, 1-enable)\n
*                 US : enbale (0-disable, 1-enable)\n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : disable�� ��� ������ BL�� ������ Off�ȴ�.\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BlDrvSetEnableFlag(BOOLEAN enable)
{
}

#endif//#ifdef USE_BACK_LIGHT
