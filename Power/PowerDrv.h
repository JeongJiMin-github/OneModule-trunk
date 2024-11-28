/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Power Control Driver Header File\n
* @file    PowerDrv.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _POWER_DRV_H
#define _POWER_DRV_H

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
extern void PowerDrvInit(void);
extern void PowerDrvPowerOn(void);
extern void PowerDrvPowerOff(void);
extern void PowerDrvSavingMode(INT8U modeLevel);
extern BOOLEAN PowerDrvACCheck(void);
#endif /* _POWER_DRV_H */