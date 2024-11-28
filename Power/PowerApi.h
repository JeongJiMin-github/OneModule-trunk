/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Power API Header File\n
* @file    PowerApi.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _POWER_API_H
#define _POWER_API_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief Auto Power Off time (min)*/
#define	POWER_PARAM_OFFSET_AUTO_OFF_TIME		0
/** @brief Auto BL off time (sec)*/
#define	POWER_PARAM_OFFSET_BL_OFF_TIME			1
/** @brief Stanby time (sec)*/
#define	POWER_PARAM_OFFSET_STANDBY_TIME		2
/** @brief 예약, 이름 변경하여 사용 가능*/
#define	POWER_PARAM_OFFSET_RESERVED			3
/** @brief POWER_PARAM_OFFSET_BATT_AD_OFFSET, 2 byte*/
#define POWER_PARAM_OFFSET_BATT_AD_OFFSET		4
/** @brief Low batter 경고 일 경우 Backlight off Enable flag */
#define POWER_PARAM_OFFSET_LOWBAT_BL_OFF		6
#ifdef USE_AUTO_POWER_SETTING
/** @brief 사용 전원에 따른 절전 기능을 위한 Auto Power Setting flag*/
#define POWER_PARAM_OFFSET_AUTO_POWER_SETTING		7

/** @brief 사용 전원에 따른 절전 기능을 위한 Auto Power Setting Value*/
#define AUTOPOWER_PARAM_OFFSET_ADAPTER 		1
#define AUTOPOWER_PARAM_OFFSET_PB_BT		2
#define AUTOPOWER_PARAM_OFFSET_DRY_BT		3
#endif

//check PARAM_POWER_AREA_SIZE = 0x08

/*
********************************************************************************
*                       GLOBAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert global typedef & struct here */


/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLE EXTERNALS
********************************************************************************
*/
/* Insert global variable externals here */
extern BOOLEAN  PowerRestartFlag;
extern BOOLEAN PowerOffFlag;
#ifdef USE_AUTO_POWER_SETTING
extern INT8U AutoPowerSetting;
#endif
#if defined (USE_M0516LDN_MCU) && defined (BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_VOLT)
extern INT8U LowDryBattBLoffFlag;
#endif
/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */
extern void PowerSetDefaultParam(void);
extern void PowerInit(void);
extern void PowerProc(void);
extern void PowerOff(void);
extern void PowerSaverInit(void);
extern void PowerSaverSetBlOffTime1s(INT8U time1s);
extern void PowerSaverResetTimer(INT8U timerNum);
extern void PowerSaverCheckPowerOffTimer(void);
extern void PowerSaverCheckBLOffTimer(void);
extern void PowerSaverCheckStandByTimer(void);
extern INT16U PowerReadBattVolt(void);
extern INT8U PowerGetUseablePercent(void);
extern void PowerCalBattOffset(void);
extern void PowerResetBattCutOffTimer(void);
#ifdef USE_POWER_SAVING_MODE
extern void PowerSavingMode(INT8U modeLevel);
#endif
#ifdef USE_AUTO_POWER_SETTING
extern void AutoPowerInit(void);
extern void AutoPowerSettingProc(void);
#endif

#endif /* _POWER_API_H */
