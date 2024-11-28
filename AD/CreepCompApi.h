/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Creep Compensation API Header File\n
* @file    CreepCompApi.h
* @version 1.0
* @date    2006/12/05
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/12/05 created by HYP \n
********************************************************************************
*/

#ifndef _CREEP_COMP_API_H
#define _CREEP_COMP_API_H

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
/** @brief Creep data를 사용하기 위한 Structure*/
typedef _PACKED_DEF struct 
{
	/** @brief 크립 보상 기준 시간 (0=disable) (단위 : 분)*/
	INT8U		creepTime;				
	/** @brief Creep 값*/
	INT16S		creep;						
} CREEP_PARAM_STRUCT;     ///3 byte <= PARAM_CREEP_AREA_SIZE

/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLE EXTERNALS
********************************************************************************
*/
/* Insert global variable externals here */
extern CREEP_PARAM_STRUCT CreepParam;

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */
 #ifdef USE_CREEP_COMP
 extern void CreepInitVariable(INT16U samplingRate);
 extern void CreepCalcMaxRaw(void);
 extern INT32S CreepCompProc(INT32S ad, INT8U stable);
 extern void CreepSetDefaultParam(void);
 extern void CreepWriteParam(void);
 extern void CreepReadParam(void);
 #endif	//USE_CREEP_COMP
#endif /* _CREEP_COMP_API_H */
