/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Display API Header File\n
* @file    DispApi.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _DISP_API_H
#define _DISP_API_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief Display Parameter offset*/
#define	DISP_PARAM_OFFSET_BL_BRIGHT		0	// 1~7 (1 byte)
#define	DISP_PARAM_OFFSET_TYPE			1	// (1 byte)
#define DISP_PARAM_OFFSET_BLINK_COUNT	2	//1~7 (1 byte)
#ifdef USE_CTS_MODE_OLD
#define DISP_PARAM_MODEL_NAME           3
#define DISP_PARAM_MODEL_NAME_SIZE      12
#endif

/** @brief Display Sum total Status*/
#ifdef USE_DISPLAY_SUM_TOTAL
#define DISPLAY_IDLE                    0
#define DISPLAY_TOTAL_WEIGHT            1	// 실제 무게 합산 기능 수행 시 보여주는 합산 누계 정보
#define DISPLAY_TOTAL_COUNT             2	// 실제 무게 합산 기능 수행 시 보여주는 합산 누계 정보
#define DISPLAY_G_TOTAL_WEIGHT          3	// 합산 정보 보여주는 기능에서의 합산 누계 정보
#define DISPLAY_G_TOTAL_COUNT           4	// 합산 정보 보여주는 기능에서의 합산 누계 정보
#define DISPLAY_INIT		            5	// 합산 정보 보여주는 시점에서 Hold 키 입력 시 합산기능 종료 (정보 초기화) 단계
#define DISPLAY_END                     6
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



/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */
extern void DispSetDefaultParam(void);
extern void DispInit(void);
extern void DispClearAll(void);
extern void DispClearAllDigits(void);
extern void DispSetChar(INT8U winNum, INT8U pos, char cdata);
extern void DispClearAllDigitsMsg(void);
extern INT8U DispGetWinSize(INT8U winNum);
extern INT8U DispGetMenuWinByNum(INT8U num);
extern void DispSetRunningPage(INT8U page);
extern void DispSetEditingPage(INT8U page);
extern INT8U DispGetDecPos(INT8U winNum);
extern INT8U DispGetDecType(INT8U winNum);
extern INT8U DispGetUseThousandPt(INT8U winNum);
extern void DispSetDecPoint(INT8U winNum, INT8U decPos, INT8U decType, INT8U useThousandPt);
extern void DispSetStr(INT8U winNum, char *str);
extern void DispSetConstStr(INT8U winNum, const char *str);
extern void DispSetNum(INT8U winNum, INT32S num, INT8U fillChar);
//extern void DispSetDecNum(INT8U winNum, INT32S num, INT8U digitSize);
//extern void DispSetHexNum(INT8U winNum, INT32U num, INT8U digitSize);
extern void DispSetConstStrWithDecNum(INT8U winNum, const char *src, INT32S num, INT8U digitSize);
extern void DispSetConstStrWithHexNum(INT8U winNum, const char *src, INT32U num, INT8U digitSize);
extern void DispSetCharWithHexNum(INT8U winNum, INT8U cData);
extern void DispSetIndicator(INT8U num, INT8U on);
extern void DispSetBattEnergy(INT8U percent);
extern void DispSetStrMsg(INT8U winNum, char *msg);
extern void DispSetConstStrMsg(INT8U winNum, const char *msg);
extern void DispSetNumMsg(INT8U winNum, INT32S num, INT8U fillChar);
extern void DispSetMsgWithNum(const char *cmsg, INT16U num);
#ifdef USE_VERSION_STR_DOT
extern void DispSetVersionWithDot(INT16U num);
#endif
extern void DispSetError(INT8U num);
extern void DispSetHelp(INT8U num);
extern void DispSetRunMsgPage(INT8U onTime100ms, INT8U blinkCount);
extern void DispImmedietly(void);
extern void DispProc(void);

extern void DispSetFraction(INT8U winNum, INT8U numer, INT8U denom);

extern void DispTest(INT8U dispTestState);

extern void DispBlSetOnOff(BOOLEAN onOff);
extern BOOLEAN DispBlGetStatus(void);
extern void DispBlSetUseFlag(BOOLEAN onOff);
extern void DispBlSetBright(INT8U level);
extern void DispBlSetEnableFlag(BOOLEAN enable);
extern INT8U DispBlGetMaxBright(void);
extern void DispTurnOnOff (BOOLEAN onOff);
extern void DispTurnOff (void); //for PowerReset of AP,AD

extern BOOLEAN DispGetOnOffStatus(void);
extern void DispSuspendOnOff(BOOLEAN onOff);
extern INT8U DispGetSuspendStatus(void);
extern void DispSetVersion(INT16U verNum);
extern void DispSetBlinkCount(INT8U cnt);
extern INT8U DispGetBlinkCount(void);
#ifdef USE_CTS_MODE 
extern void DispSetMsgStop(void);
#endif
#endif /* _DISP_API_H */
