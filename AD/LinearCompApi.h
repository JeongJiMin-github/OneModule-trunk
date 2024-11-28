/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Linear Compensation API Header File\n
* @file    LinearCompApi.h
* @version 1.0
* @date    2006/12/05
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/12/05 created by HYP \n
********************************************************************************
*/

#ifndef _LINEAR_COMP_API_H
#define _LINEAR_COMP_API_H

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
/** @brief ���� ���󿡼� ���Ǵ� Struct*/
typedef _PACKED_DEF struct
{
	/** @brief linear calibration ���� ��*/
	INT32S		linearZeroValue;				
	/** @brief linear calibration �߰� ��*/
	INT32S		linearMidValue;					
	/** @brief linear calibration ���� ��*/
	INT32S		linearSpanValue;				
	/** @brief linear calculation ratio*/
	UNION_INT32U	linearRatio;					
} LINEAR_PARAM_STRUCT;//16 byte <= PARAM_LINEAR_AREA_SIZE

/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLE EXTERNALS
********************************************************************************
*/
/* Insert global variable externals here */
extern LINEAR_PARAM_STRUCT LinearParam;

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */
extern void LinearSetDefaultParam(void);
extern void LinearWriteParam(void);
extern void LinearReadParam(void);
extern void LinearInitVariable(void);
extern INT32S LinearCompProc(INT32S rawAd);
extern void LinearCalcVariable(void);
extern void LinearSetZero(INT32S lAd);

#endif /* _LINEAR_COMP_API_H */
