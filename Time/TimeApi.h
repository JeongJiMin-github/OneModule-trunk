/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Time(RTC) API Header File\n
* @file    TimeApi.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _TIME_API_H
#define _TIME_API_H

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
extern void TimeSetDefaultParam(void);
extern void TimeInit(void);
extern void TimeWriteData(void);
extern void TimeReadData(void);
//extern void TimeTest(void);
//extern void TimeWriteTest(void);

//extern INT8U  TimeGetWeek(INT16U year, INT16U month, INT16U day );
extern INT16U TimeCheckLeap(INT16U year);
//extern INT16U TimeGetDays(INT16U year, INT16U month, INT16U day);	// 1..365
extern void  TimeGetNextDay(INT16U *year, INT16U *month, INT16U *day, INT16U ds);
//extern void  TimeGetBeforeDay(INT16U *year, INT16U *month, INT16U *day, INT16U ds);
//extern INT16U TimeGetYearDays(INT16U year);
extern INT16U TimeGetMonthDays(INT16U year, INT16U month);
//extern INT8U  TimeCheckDay(INT16U year, INT16U month, INT16U day);
#ifdef USE_CTS_MODE 
extern void TimeVerify();
#endif
#endif /* _TIME_API_H */
