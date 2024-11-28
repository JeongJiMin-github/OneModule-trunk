/**
********************************************************************************
* Copyright (c) 2016 CAS
* @brief   AD Driver Source File\n
           Chip : M0516LDN
* @file    AdDrv_M0516ldn.h
* @version 1.0
* @date    2016/12/06
* @author  Keum Young Kwang (KYK)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2016/12/06 created by KYK \n
********************************************************************************
*/

#ifndef _AD_DRV_M0516LDN_H
#define _AD_DRV_M0516LDN_H

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
extern RING_BUF AdDrvRingBufForPowerCheck;

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */

extern void AdM0516ldnDrvInit(void);
extern void AdM0516ldnProc(void);
#ifdef TEMP_DRV_NTC10K
extern INT16S TempDrvReadData(void);
#endif
#endif /* _AD_DRV_M0516LDN_H*/