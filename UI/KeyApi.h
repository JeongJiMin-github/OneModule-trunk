/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Key API Header File\n
* @file    KeyApi.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _KEY_API_H
#define _KEY_API_H

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
extern void KeySetDefaultParam(void);
extern void KeyInit(void);
extern void KeyEnable(BOOLEAN on);
extern BOOLEAN KeyCheckCalPort(void);
//extern BOOLEAN KeyCheckPowerPort(void);
extern BOOLEAN KeyCheckPowerKey(void);
extern BOOLEAN KeyCheck(void);
extern BOOLEAN KeyCheckPressedFlag(void);
extern void KeyResetPressedFlag(void);
extern void KeySetFuncCode(INT8U keyMode, INT8U rawKey, KEY_TYPE funcKey);
extern KEY_TYPE KeyGetFuncCode(INT8U keyMode);
extern INT8U KeyGetPrevRawCode(INT8U keyMode, KEY_TYPE funcCode);
extern KEY_TYPE KeyGetPgmKeyByNum(INT8U num);
extern void KeyRewind(void);
extern void KeySaveChecksum(void);

#ifdef REVERSE_DISPLAY
extern BOOLEAN KeyDrvCheckReverseFlag(void);
#endif

#ifdef USE_LONG_KEY
extern void KeyLongKeyEnable(void);
extern void KeyLongKeyDisable(void);
#endif

#ifdef USE_UL_DISPLAY
INT8U KeyGetUlDisplayFlag(void);
#endif
extern void KeyTest(void);
#ifdef USE_AUTO_TARE_FUNCTION
extern BOOLEAN KeyCheckAutoTareFunction(void);
#endif

#endif /* _KEY_API_H */