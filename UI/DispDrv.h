/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Display Driver Header File\n
           for ER-1 LED
* @file    DispDrv.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _DISP_DRV_H
#define _DISP_DRV_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */

/** @brief 일반 모드 */
#define DISP_DRV_NORMAL_MODE	0
/** @brief 대기 모드 */
#define DISP_DRV_SUSPEND_MODE	1

/** @brief Display Image Buffer page 크기 */
#define DISP_DRV_PAGE_SIZE	3
/** @brief Display Normal Image Buffer page 번호 
* @remark 평상시 사용되는 Page (일반 모드)*/
#define DISP_DRV_NORMAL_PAGE	0
/** @brief Display Message Image Buffer page 번호
* @remark Message(Error, Help...) Display 시 사용되는 Page (일반 모드)*/
#define DISP_DRV_MSG_PAGE		1
/** @brief Display Suspend Image Buffer page 번호 
* @remark Suspend Mode 동작시 사용되는 Page (대기 모드)*/
#define DISP_DRV_SUSPEND_PAGE	2

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
extern void DispDrvInit(INT8U brightness);
extern INT8U DispDrvGetWinSize(INT8U winNum);
extern BOOLEAN DispDrvGetMinusSignExist(INT8U winNum);
extern INT8U DispDrvGetMenuWinByNum(INT8U num);
extern void DispDrvSetMode(INT8U mode);
extern void DispDrvSetPage(INT8U page);
extern void DispDrvClearBuf(void);
extern void DispDrvSetDecPoint(INT8U winNum, INT8U decPos, INT8U decType, INT8U thousandPos);
extern void DispDrvSetStr(INT8U winNum, char *str);
extern void DispDrvSetConstStr(INT8U winNum, const char *str);
extern void DispDrvSetBattEnergy(INT8U percent);
extern void DispDrvSetIndicator(INT8U num, INT8U on);
extern void DispDrvSetEditingPage(INT8U page);
extern void DispDrvProc(void);
extern void DispDrvProcForInterrupt(void);

extern void DispDrvFillChar(char fillChar);
extern void DispDrvSetChar(INT8U winNum, INT8U pos, char cdata);
extern void DispDrvSetAllIndicator(INT8U on);

extern void DispDrvSetFraction(INT8U winNum, INT8U numer, INT8U denom);

extern INT8U DispDrvScanProc(void);
extern void DispDrvTurnOnOff(BOOLEAN onOff);
#ifdef USE_DISP_BRIGHT
extern void DispDrvSetBright(INT8U level);
#endif
#ifdef USE_HT1622_BUZZER_DRV
extern void ht1622TranCommand(INT8U command);
#endif
#ifdef USE_UNIT_OZ_LCD_CLEAR
extern void DispDrvOzLcdClear(void);
#endif

#endif /* _DISP_DRV_H */
