/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Driver Header File\n
           Chip : ADS1100
* @file    AdDrv.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _AD_DRV_H
#define _AD_DRV_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
#ifdef USE_CHECK_AD_VENDOR
#define AD_VENDOR_OB1020     0
#define AD_VENDOR_XM24L      1
#endif
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
extern RING_BUF AdDrvRingBuf;
extern RING_BUF AdDrvRingBufForBatt;
#if defined(AD_DRV_XM24L) || defined(AD_DRV_XM24L_3_3V) 
extern RING_BUF XM24L_AdDrvRingBuf;
#endif
/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */
extern void AdDrvInit(void);
extern INT16U AdDrvGetSamplingRate(void);
extern void AdDrvProc(void);
#if defined(AD_DRV_XM24L) || defined(AD_DRV_XM24L_3_3V) 
extern void XM24L_AdDrvInit(void);
extern INT16U XM24L_AdDrvGetSamplingRate(void);
extern void XM24L_AdDrvProc(void);
#endif
#ifdef AD_M0516LDN
extern void AdDrvCpuAdcInit(void);
extern void AdDrvCpuAdcProc(void);
#endif
#endif /* _AD_DRV_H */