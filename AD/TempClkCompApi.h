/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Temperature Clk Compensation API Source File\n
* @file    TempClkCompApi.c
* @version 1.0
* @date    2013/01/09
* @author  Deok Hyun Kim (KDH)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/12/05 created by HYP \n
********************************************************************************
*/


#ifndef _TEMP_CLK_COMP_API_H
#define _TEMP_CLK_COMP_API_H

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


typedef _PACKED_DEF struct
{
	/** @brief 상혼에서의 tuning 값*/
	INT8U	tempCompEnable;		
    /** @brief 고온에서의 클럭보정 진입값 */
	INT8S	tempHighEndterValue;		
    /** @brief 고온에서의 클럭보정 탈출값*/
	INT8S	tempHighEndValue;		
    /** @brief 저온에서의 클럭보정 진입값 */
	INT8S	tempLowEndterValue;		
    /** @brief 저온에서의 클럭보정 탈출값*/
	INT8S	tempLowEndValue;		
}TEMP_CLK_COMP_STRUCT;


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
extern void TempInitClkVariable (void);
extern INT32S TempClkCompProc(void);

#endif /* _TEMP_COMP_API_H */
