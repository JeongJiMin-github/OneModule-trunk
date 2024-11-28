/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Timer(in CPU) Driver Header File\n
           Chip : C52
* @file    TimerDrv.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _TIMER_DRV_H
#define _TIMER_DRV_H

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
extern void TimerDrvInit(void);
extern INT16U TimerDrvGetSysTimer10ms(void);
extern INT16U TimerDrvGetSysTimer100ms(void);

#endif /* _TIMER_DRV_H */