/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Filter API Header File\n
* @file    FilterApi.h
* @version 1.0
* @date    2006/12/05
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/12/05 created by HYP \n
********************************************************************************
*/

#ifndef _FILTER_API_H
#define _FILTER_API_H

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
//extern INT32S FIRFiltProc(INT32S inData);	//not use

//extern void MedianFiltInit(INT8U filtSize);		//not use
//extern INT32S MedianFiltProc(INT32S inData);	//not use
//extern void MAFInit(INT8U winMinSize, INT8U winMaxSize, INT32S winIncThres, INT32S winBreakThres);
//extern INT32S MAFProc(INT32S inData);

extern void MAFSInit(INT8U winMinSize, INT8U winMaxSize, INT32S winIncThres, INT32S winBreakThres, INT8U shockWinSize, INT32S shockThres, INT32S thresIncThres);
extern INT32S MAFSProc(INT32S inData);

#endif /* _FILTER_API_H */
