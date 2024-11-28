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
/** @brief KR : Backlight의 최대 밝기 값 \n
		  US : \n
		  CN :									*/
#define BL_DRV_MAX_BRIGHT		7
/** @brief KR : Backlight의 Default 밝기 값 \n
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
/** @brief KR : Backlight의 현재 On/Off 상태 변수 (0-off, 1-on)\n
		  US : \n
		  CN :									*/
static BOOLEAN blStatus;

/** @brief KR : Backlight의 사용 유무 변수 (0-no use, 1-use)\n
		  US : \n
		  CN :									*/
static BOOLEAN blUseFlag;

/** @brief KR : Backlight의 작동 유무 변수 (0-no use, 1-use)\n
		  US : \n
		  CN :									*/
static BOOLEAN blEnableFlag;

/** @brief KR : Backlight의 밝기 조절 Count 변수\n
		  US : \n
		  CN :									*/
static data INT8U blPwmCount;//data for high speed in interrupt

/** @brief KR : Backlight의 밝기 조절 Max Count 변수\n
		  US : \n
		  CN :									*/
static data INT8U blPwmMaxCount;//data for high speed in interrupt

/** @brief KR : Backlight의 밝기 조절 테이블\n
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
* @brief       KR : Back Light Driver Init 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : useFlag : BackLight 사용 유무, bright : 밝기 Level\n
*			     blOffTime : Auto BL off time (0 인지만 Check, 0 이면 Off)\n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : 초기에 실행 되는 함수\n
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
* @brief       KR : Back Light Driver Proc 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : 일정 시간(1ms)에 한번 실행 되는 함수\n
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
* @brief       KR : Back Light Bright 설정 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : level : 밝기 (1~BL_DRV_MAX_BRIGHT)\n
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
* @brief       KR : Back Light Maxinum Bright 설정값 반환 함수\n
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
* @brief       KR : Back Light On/Off 함수\n
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
* @brief       KR : Back Light 현재 상태 반환 함수\n
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
* @brief       KR : Back Light 사용 유무 설정 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : onOff (0-off, 1-on)\n
*                 US : onOff (0-off, 1-on)\n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : Off일 경우 무조건 BL는 무조건 Off된다.\n
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
* @brief       KR : Back Light 작동 유무 설정 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : enbale (0-disable, 1-enable)\n
*                 US : enbale (0-disable, 1-enable)\n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : disable일 경우 무조건 BL는 무조건 Off된다.\n
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
* @brief       KR : Back Light Driver Init 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : useFlag : BackLight 사용 유무, bright : 밝기 Level\n
*			     blOffTime : Auto BL off time (0 인지만 Check, 0 이면 Off)\n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : 초기에 실행 되는 함수\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BlDrvInit(INT8U bright)
{
}

/**
********************************************************************************
* @brief       KR : Back Light Driver Proc 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : 일정 시간(1ms)에 한번 실행 되는 함수\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BlDrvProc(void)
{
}

/**
********************************************************************************
* @brief       KR : Back Light Bright 설정 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : level : 밝기 (1~BL_DRV_MAX_BRIGHT)\n
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
* @brief       KR : Back Light Maxinum Bright 설정값 반환 함수\n
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
* @brief       KR : Back Light On/Off 함수\n
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
* @brief       KR : Back Light 현재 상태 반환 함수\n
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
* @brief       KR : Back Light 사용 유무 설정 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : onOff (0-off, 1-on)\n
*                 US : onOff (0-off, 1-on)\n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : Off일 경우 무조건 BL는 무조건 Off된다.\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BlDrvSetUseFlag(BOOLEAN onOff)
{
}

/**
********************************************************************************
* @brief       KR : Back Light 작동 유무 설정 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : enbale (0-disable, 1-enable)\n
*                 US : enbale (0-disable, 1-enable)\n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : disable일 경우 무조건 BL는 무조건 Off된다.\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BlDrvSetEnableFlag(BOOLEAN enable)
{
}

#endif//#ifdef USE_BACK_LIGHT
