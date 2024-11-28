/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Window API Header File\n
* @file    WinApi.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _EDITOR_API_H
#define _EDITOR_API_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief Number Type : Decimal */
#define DEC 0
/** @brief Number Type : Hexa */
#define HEX 1
/** @brief Number Type : String */
#define STR 2

/** @brief Editor Status : Stop */
#define EDITOR_STOP	1
/** @brief Editor Status : Run */
#define EDITOR_RUN	0

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
extern void EditorInit(void);
extern void EditorProc(void);
extern void EditorRun(void);
extern void EditorStop(void);
extern INT8U EditorCheckStatus(void);
extern void EditorGetData(INT32U *dataPtr);
extern void EditorSetData(INT8U winNum, INT8U type, INT32U editData, INT32U min, INT32U max, INT8U decPos, INT8U decType, INT8U useThousandPt);
extern void EditorSetDataAndRun(INT8U winNum, INT8U type, INT32U editData, INT32U min, INT32U max, INT8U decPos, INT8U decType, INT8U useThousandPt);
extern void EditorSetConstStr(const char *cStr);
extern INT32U EditorInsDecDigit(INT32U inData, INT8U decNum, INT8U leng, INT32U max, INT8U denyOver);
extern INT32U EditorInsHexDigit(INT32U inData, INT8U hexNum, INT8U leng, INT32U max, INT8U denyOver);
extern INT32U EditorIncDecDigit(INT32U inData, INT8U pos);
extern INT32U EditorIncHexDigit(INT32U inData, INT8U pos);
extern INT32U EditorDecDecDigit(INT32U inData, INT8U pos);
extern INT32U EditorDecHexDigit(INT32U inData, INT8U pos);
extern INT32U EditorChangeDecDigit(INT32U inData, INT8U decNum, INT8U pos);
extern INT32U EditorChangeHexDigit(INT32U inData, INT8U hexNum, INT8U pos);
#endif /* _EDITOR_API_H */
