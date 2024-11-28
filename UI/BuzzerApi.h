/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Buzzer API Header File\n
* @file    BuzzerApi.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _BUZZER_API_H
#define _BUZZER_API_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */

/** @brief KR : Buzzer Parameter offset\n
		  US : \n
		  CN :									*/
#define	BUZZER_PARAM_OFFSET_USE		0	// 1

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
extern void BuzzerSetDefaultParam(void);
extern void BuzzerInit(void);
extern void BuzzerSwitch(INT8U switchFlag);
extern void BuzzerSetOnTimes(INT8U times);

#endif /* _BUZZER_API_H */
