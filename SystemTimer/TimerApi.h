/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Timer(in CPU) API Header File\n
* @file    TimerApi.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _TIMER_API_H
#define _TIMER_API_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief 100ms Timer 전체 개수 */
#ifdef USE_CTS_MODE
#define TIMER_DELAY_100MS_MAX_NUM	12
#else
#define TIMER_DELAY_100MS_MAX_NUM	9
#endif
/** @brief 100ms Timer : Message display Timer Number (for blinking message)*/
#define TIMER_DELAY_100MS_DISP_MSG		0
/** @brief 100ms Timer : Initial Timer Number (for checking display and initial zero) */
#define TIMER_DELAY_100MS_INITIAL		1
/** 
* @brief 100ms Timer : Calibration Timer Number (for checking stable...) \n
*                      TIMER_DELAY_100MS_INITIAL와 동일, 동시 사용 가능성 없음
*/
#define TIMER_DELAY_100MS_CAL			1
/** 
* @brief 100ms Timer : for checking stable in sale mode \n
*                      TIMER_DELAY_100MS_INITIAL와 동일, 동시 사용 가능성 없음
*/
#define TIMER_DELAY_100MS_WAIT_STABLE			1
/** @brief 100ms Timer : Power Saver Auto power off Timer Number */
#define TIMER_DELAY_100MS_POWER_OFF				2
/** @brief 100ms Timer : Auto transaction Time number */
#define TIMER_DELAY_100MS_AUTO_TRANSACTION			3
/** @brief 100ms Timer : Power Saver Auto BL off Timer Number */
#define TIMER_DELAY_100MS_BL_OFF					3
/** @brief 100ms Timer : Power Saver Standby Mode Timer Number */
#define TIMER_DELAY_100MS_STAND_BY				4
/** @brief 100ms Timer : Editor Timer Number */
#define TIMER_DELAY_100MS_EDITOR					5
/** @brief 100ms Timer : Sale state delay Timer Number \n */
#define TIMER_DELAY_100MS_SALE_STATE				6
/** @brief 100ms Timer : Sale Unit Price renewal Timer number \n */
#define TIMER_DELAY_100MS_UNIT_PRICE_RENEWAL	7
/** @brief 100ms Timer : Auto hold Timer number \n */
#define TIMER_DELAY_100MS_AUTO_HOLD				8
#ifdef USE_CTS_MODE_OLD
/** @brief 100ms Timer : Auto hold Timer number \n */
#define TIMER_DELAY_100MS_CTS_CHECK				9
#elif defined(USE_CTS_MODE)
/** @brief 100ms Timer : Cts Check Last Use Key Timer number \n */
#define TIMER_DELAY_100MS_CTS_LAST_USE_KEY			10
/** @brief 100ms Timer : Cts Check Last Load AD Timer number \n */
#define TIMER_DELAY_100MS_CTS_LAST_LOAD_AD			11
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

extern void TimerInit(void);
//extern void TimerDelay10ms(INT16U delay);	//not use
extern INT16U TimerGetSysTimer100ms(void);
extern void TimerSetDelay100ms(INT8U num, INT16U delay);
extern INT8U TimerCheckDelay100ms(INT8U num);
extern void TimerSetDelay10ms(INT8U num, INT16U delay);
extern INT8U TimerCheckDelay10ms(INT8U num);

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */


#endif /* _TIMER_API_H */
