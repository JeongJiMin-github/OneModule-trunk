/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Time(RTC) Driver Header File\n
* @file    TimeDrv.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _TIME_DRV_H
#define _TIME_DRV_H

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
extern void TimeDrvInit(void);
extern void TimeDrvWriteClkData(INT8U clk_addr, INT8U bData);
extern INT8U TimeDrvReadClkData(INT8U clk_addr);
extern void TimeDrvWriteAllClkData(void);
extern void TimeDrvReadAllClkData(void);

#endif /* _TIME_DRV_H */