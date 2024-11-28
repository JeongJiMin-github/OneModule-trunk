/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Key Driver Header File\n
           for ER-1 LED
* @file    KeyDrv.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _KEY_DRV_H
#define _KEY_DRV_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief key model이 저장 되는 주소*/
#define KEY_PARAM_OFFSET_MODEL		0
/** @brief key check sum이 저장 되는 주소*/
#define KEY_PARAM_OFFSET_CHECKSUM	2
/** @brief key type 저장 되는 주소, use only SW */
#define KEY_PARAM_OFFSET_TYPE			3
/** @brief key table이 저장 되는 주소*/
#define KEY_PARAM_OFFSET_TABLE		4
//PARAM_KEY_AREA_SIZE 0x40

#define UPDATE_KEY_TABLE				0
#define	UPDATE_CHECKSUM				1

/** @brief Key Type 정의 (keyDrvType 상태 값)*/
#define KEY_TYPE_HOLD           0
#define KEY_TYPE_MODE           1
#define KEY_TYPE_UNIT_CHANGE    2
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
extern RING_BUF  KeyDrvRingBuf;
#ifdef	USE_DECIMAL_POINT	
extern BOOLEAN DecimalPointKeyExistFlag; // csh 20120731
#endif	
/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */
extern void KeyDrvInit(void);
extern void KeyDrvEnable(BOOLEAN on);
extern void KeyDrvLoadFuncTable(void);
extern void KeyDrvSaveFuncTable(INT8U mode);
extern void KeyDrvSetDefaultFuncTable(void);
extern BOOLEAN KeyDrvCheckCalPort(void);
//extern BOOLEAN KeyDrvCheckPowerPort(void);
extern BOOLEAN KeyDrvCheckPowerKey(void);
extern void KeyDrvProc(void);
#ifdef USE_ER_PLUS_M_TURKEY_KEY_DEFINE
extern void KeyDrvSetTurkeyFuncTable(void);
#endif

#ifdef REVERSE_DISPLAY
extern BOOLEAN KeyDrvCheckReversePort(void);
#endif

#ifdef USE_UL_DISPLAY
INT8U KeyDrvUlDisplayFlag(void);
#endif

#ifdef USE_LONG_KEY
extern BOOLEAN KeyDrvLongKeyEnable(void);
extern BOOLEAN KeyDrvLongKeyDisable(void);
#endif

extern KEY_TYPE KeyDrvGetFuncCode(INT8U keyMode, INT8U rawKey);
extern void KeyDrvSetFuncCode(INT8U keyMode, INT8U rawKey, KEY_TYPE funcKey);
extern KEY_TYPE KeyDrvGetPgmKeyByNum(INT8U num);
#ifdef USE_AUTO_TARE_FUNCTION
extern BOOLEAN KeyDrvCheckAutoTareFuction(void);
#endif
#endif /* _KEY_DRV_H */
