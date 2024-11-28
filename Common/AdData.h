/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Data Header File\n
* @file    AdData.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _AD_DATA_H
#define _AD_DATA_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief KR : kg을 나타내는 번호\n
		  US : \n
		  CN :									*/
#define UNIT_KG		0

/** @brief KR : lb를 나타내는 번호\n
		  US : \n
		  CN :									*/
#define UNIT_LB		1

/** @brief KR : oz를 나타내는 번호\n
		  US : \n
		  CN :									*/
#define UNIT_OZ		2

/** @brief KR : gram을 나타내는 번호\n
		  US : \n
		  CN :									*/
#define UNIT_GRAM	3

/** @brief KR : ton을 나타내는 번호\n
		  US : \n
		  CN :									*/
#define UNIT_TON	4

/** @brief KR : 1/16형태로 oz을 나타내는 번호\n
		  US : \n
		  CN :									*/
#define UNIT_1_16OZ	5

/** @brief KR : short_ton 나타내는 번호\n
		  US : \n
		  CN :									*/
#define UNIT_SHORT_TON	6

/** @brief KR : long_ton 나타내는 번호\n
		  US : \n
		  CN :									*/
#define UNIT_LONG_TON	7

/** @brief KR : Nuton을 나타내는 번호\n
		  US : \n
		  CN :									*/
#define UNIT_NEWTON	8

/** @brief KR : X1000N을 나타내는 번호\n
		  US : \n
		  CN :									*/
#define UNIT_KN     9

/** @brief KR : daN을 나타내는 번호\n
		  US : \n
		  CN :									*/
#define UNIT_DAN	10

/** @brief KR : viss을 나타내는 번호\n
		  US : \n
		  CN :									*/
#define UNIT_VISS	11

/** @brief KR : Unit이 없는 경우를 나타내는 번호\n
		  US : \n
		  CN :									*/
#define UNIT_NONE	100

/*
********************************************************************************
*                       GLOBAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert global typedef & struct here */
/** @brief KR : AD Data Struct\n
		  US : AD Data Struct\n
		  CN :									*/
typedef _PACKED_DEF struct 
{
	/** @brief KR : raw filtered data stable 값\n
			  US : \n
			  CN :									*/
	INT8U rawStableFlag;

	/** @brief KR : raw data 값\n
			  US : \n
			  CN :									*/
	INT32S rawData;		//not use

	/** @brief KR : raw filtered data 값\n
			  US : \n
			  CN :									*/
	INT32S rawFilteredData;

	/** @brief KR : raw Temp Compensated Data 값\n
			  US : \n
			  CN :									*/
	INT32S rawTempCompData;

	/** @brief KR : raw Hysteresys Compensated Data 값\n
			  US : \n
			  CN :									*/
	INT32S rawHystCompData;

	/** @brief KR : raw Creep Compensated Data 값\n
			  US : \n
			  CN :									*/
	INT32S rawCreepCompData;

	/** @brief KR : raw Linear Compensated Data 값\n
			  US : \n
			  CN :									*/
	INT32S rawLinearCompData;

	/** @brief KR : raw Normalized Data 값\n
			  US : \n
			  CN :									*/
	INT32S rawNormalizedData;

	/** @brief KR : raw Internal Data 값\n
			  US : \n
			  CN :									*/
	INT32S rawInternalData;

	/** @brief KR : raw Display filtered Data 값\n
			  US : \n
			  CN :									*/
	INT32S rawDispFilteredData;

	/** @brief KR : raw Initial zero Data 값\n
			  US : \n
			  CN :									*/
	INT32S rawInitZeroData;

	/** @brief KR : raw Set zero Data 값\n
			  US : \n
			  CN :									*/
	INT32S rawSetZeroData;

	/** @brief KR : raw Set tare Data 값\n
			  US : \n
			  CN :									*/
	INT32S rawSetTareData;

	/** @brief KR : 온도\n
			  US : \n
			  CN :									*/
#ifdef USE_TEMP_COMP_MWIIH
	INT16S temp;
#else
	INT8S temp;
#endif
	/** @brief KR : 무게값\n
			  US : \n
			  CN :									*/
	INT32S weightData;
	
	/** @brief KR : 무게값의 Decimal Position\n
			  US : \n
			  CN :									*/
	INT8S weightDecPos;

	/** @brief KR : 무게단위\n
			  US : \n
			  CN :									*/
	INT8U weightUnit;

	/** @brief KR : 용기 무게값\n
			  US : \n
			  CN :									*/
	INT32S tareWeight;

	/** @brief KR : Gross zero 확인 플래그\n
			  US : \n
			  CN :									*/
	INT8U grossZeroFlag;

	/** @brief KR : Net zero 확인 플래그\n
			  US : \n
			  CN :									*/
	INT8U netZeroFlag;

	/** @brief KR : Stable 확인 플래그\n
			  US : \n
			  CN :									*/
	INT8U stableFlag;

	/** @brief KR : Overload 확인 플래그\n
			  US : \n
			  CN :									*/
	INT8U overloadFlag;

	/** @brief KR : Underload 확인 플래그\n
			  US : \n
			  CN :									*/
	INT8U underloadFlag;
	
	/** @brief KR : 마지막 Digit이 유효하지 않음을 확인하는 플래그\n
			  US : \n
			  CN :									*/
	INT8U lastDigitIsInvalid;

	/** @brief KR : weight max capa \n
			  US : \n
			  CN :									*/
	INT32S maxCapa;

	/** @brief KR : initial zero range error \n
			  US : \n
			  CN :									*/
	INT8U initZeroError;

	/** @brief KR : 1d value (internal value) \n
			  US : \n
			  CN :									*/
	INT16U d1;
} AD_DATA;

/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLE EXTERNALS
********************************************************************************
*/
/* Insert global variable externals here */
extern AD_DATA	AdData;

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */


#endif /* _AD_DATA_H */
