/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Calibration Mode Applicaion Header File\n
* @file    CalModeApp.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _CAL_MODE_APP_H
#define _CAL_MODE_APP_H

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
extern void CalModeInit(void);
extern MENU code *CalModeGetMenuTable(void);
extern INT8U CalModeGetMenuTableMaxPos(void);

#endif /* _CAL_MODE_APP_H */
