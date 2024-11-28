/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Hysteresis Compensation API Header File\n
* @file    HystCompApi.h
* @version 1.0
* @date    2006/12/05
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/12/05 created by HYP \n
********************************************************************************
*/

#ifndef _HYST_COMP_API_H
#define _HYST_COMP_API_H

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
#ifdef USE_EXP_HYST_COMP
/** @brief 히스테리시스 보상에서 사용되는 struct*/
typedef _PACKED_DEF struct
{
	/** @brief 히스테리시스 보상 사용 여부*/
	INT8U		hystEnable;					
        INT8U           hystOrder;
	INT32S		hystAd[10];  
        UNION_INT32U    hystRatio[4];
} HYST_PARAM_STRUCT;// bytes <= PARAM_HEAD_AREA_SIZE(0x40 = 64)
        
#else

/** @brief 히스테리시스 보상에서 사용되는 struct*/
typedef _PACKED_DEF struct
{
	/** @brief 히스테리시스 보상 사용 여부*/
	INT8U		hystEnable;					////70
	/** @brief 히스테리시스 calibration 영점 값*/
	INT32S		hystZeroValue;				////18
	/** @brief 히스테리시스 calibration 중간상승 값*/
	INT32S		hystMidUpValue;				////22
	/** @brief 히스테리시스 calibration 중간하강 값*/
	INT32S		hystMidDownValue;			////26
	/** @brief 히스테리시스 calibration 스판 값*/
	INT32S		hystSpanValue;				////30
} HYST_PARAM_STRUCT;//17 byte <= PARAM_HYST_AREA_SIZE
#endif
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLE EXTERNALS
********************************************************************************
*/
/* Insert global variable externals here */
extern HYST_PARAM_STRUCT HystParam;

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */
extern void HystInitVariable(void);
extern void HystSetZero(INT32S lAd);
extern INT32S HystCompProc(INT32S ad);
extern void HystSetDefaultParam(void);
extern void HystWriteParam(void);
extern void HystReadParam(void);
#ifdef USE_EXP_HYST_COMP
extern INT8U HystCompSetRatio4Capa(INT16U capa);
#endif
#endif /* _HYST_COMP_API_H */
