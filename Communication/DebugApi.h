/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Debug Api Header File\n
* @file    DebugApi.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _DEBUG_API_H
#define _DEBUG_API_H

#ifdef USE_DEBUG_API
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
extern void DebugInit(void);
extern void DebugDataOut(char *debugData, INT16U size);
extern void DebugStrOut(char *debugStr);
#endif//#ifdef USE_DEBUG_API

#endif /* _DEBUG_API_H */
