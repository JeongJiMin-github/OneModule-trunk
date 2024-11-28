/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Driver Header File\n
           Chip : EFM32G840F128
* @file    AdDrv_Efm32g840.h
* @version 1.0
* @date    2012/09/19
* @author  Deok Hyun Kim (KDH)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2012/09/19 created by KDH \n
********************************************************************************
*/

#ifndef _AD_DRV_EFM32_H
#define _AD_DRV_EFM32_H

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

extern void AdEfm32g840DrvInit(void);
extern void AdEfm32g840Proc(void);

#endif /* _AD_DRV_EFM32_H*/