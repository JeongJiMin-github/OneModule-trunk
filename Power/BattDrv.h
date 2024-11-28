/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Battery Driver Header File\n
           for xx
* @file    BattDrv.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _BATT_DRV_H
#define _BATT_DRV_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */


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

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */
extern void BattDrvInit(INT16U adOffset);
extern INT16U BattDrvGetDefaultAdOffset(void);
extern void BattDrvProc(INT16U currTimer100ms);
extern void BattDrvResetChargingRefVolt(void);
extern INT8U BattDrvCheckAdOffsetRange(INT32S adOffset);
extern void BattDrvSetAdOffset(INT32S adOffset);
extern INT32S BattDrvReadAd(void);
extern INT16U BattDrvReadVolt(void);
extern BOOLEAN BattDrvCheckLow(void);
extern BOOLEAN BattDrvCheckFull(void);
extern BOOLEAN BattDrvCheckCutOff(void);
extern INT8U BattDrvGetUseablePercent(void);
extern BOOLEAN BattChargeCheck(void);
#ifdef USE_AUTO_POWER_SETTING 
/** @brief Power Type Check function */
extern INT8U PowerTypeCheck(void);
#endif

#endif /* _BATT_DRV_H */