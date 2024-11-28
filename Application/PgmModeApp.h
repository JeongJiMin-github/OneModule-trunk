/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Program Mode Applicaion Header File\n
* @file    PgmModeApp.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _PGM_MODE_APP_H
#define _PGM_MODE_APP_H

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
extern void PgmModeInit(void);
extern MENU code *PgmModeGetMenuTable(void);
extern DIRECT_MENU code *PgmModeGetDirectMenuTable(void);
extern INT8U PgmModeGetMenuTableMaxPos(void);
extern INT8U PgmModeGetDirectMenuTableMaxPos(void);


#endif /* _PGM_MODE_APP_H */