/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Temperature Sensor Driver Header File\n
           Chip : TCN75A
* @file    TempDrv.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _TEMP_DRV_H
#define _TEMP_DRV_H

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
#if defined(USE_TEMP_COMP) || defined(USE_TEMP_COMP_MWIIH)
extern void TempDrvProc (INT16U currTimer100ms);
extern BOOLEAN TempDrvInit(void);
extern INT8S TempDrvReadData(void);
#endif
#ifdef DEBUG_TEMP
extern void TempDrvWriteDataForDebug(INT8S temp);
#endif

#endif /* _TEMP_DRV_H */
