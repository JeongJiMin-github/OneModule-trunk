/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Back Light Driver Header File\n
           for xx
* @file    BackLightDrv.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _BACKLIGHT_DRV_H
#define _BACKLIGHT_DRV_H

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
extern void BlDrvInit(INT8U bright);
extern void BlDrvProc(void);
extern void BlDrvSetBright(INT8U level);
extern INT8U BlDrvGetMaxBright(void);
extern void BlDrvSetOnOff(BOOLEAN onOff);
extern BOOLEAN BlDrvGetStatus(void);
extern void BlDrvSetUseFlag(BOOLEAN onOff);
extern void BlDrvSetEnableFlag(BOOLEAN enable);

#endif /* _BACKLIGHT_DRV_H */