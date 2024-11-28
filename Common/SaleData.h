/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Sale Data Header File\n
* @file    SaleData.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _SALE_DATA_H
#define _SALE_DATA_H

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
/** @brief Sale 에서 사용되는 Struct*/
typedef struct
{
	/** @brief saleState*/
	INT8U	saleState;
	/** @brief add기능 Count개수*/
	INT16U	addCount;
	/** @brief add기능 Price*/
	INT32U	addPrice;

	/** @brief 현재 무게 값*/
	INT32S	weight;
	/** @brief 현재 사용하는 무게 unit, 0:kg, 1:lb, 2:oz, 3:g*/
	INT8U 	weightUnit;
	/** @brief 무게값의 Decimal Position */
	INT8S 	weightDecPos;
	/** @brief 마지막 자릿수가 유효한지를 나타내는 플래그*/
	INT8U	lastDigitIsInvalid;

#if defined(DIALOG6) || defined (USE_SMART_BT_PROTOCOL) || defined (USE_SATO_PRINTER_PROTOCOL)
	/** @brief Key tare 값*/
	INT32S	keyTare;
	/** @brief Key tare 값*/
	INT8U	zeroErrFlag;	//not use
#endif
	/** @brief Unit price 값*/
	INT32U	unitPrice;
	/** @brief Total price 값*/
	INT32U	totalPrice;	
	
	/** @brief weight dot character*/
	INT8U 	weightDotChar;	
	/** @brief price dot character*/
	INT8U	priceDotChar;
	/** @brief price decimal position*/
	INT8U	priceDecPos;
	
	/** @brief Plu 최대 개수 */
	//INT8U 	pluMaxNum;	//not use
	/** @brief 호출된 Plu number, pluNum = 0 은 Plu가 호출되지 않은 Normal 상태임*/
	INT16U  pluNum;
	/** @brief Hold기능 사용 무게, 평균값*/
	INT32S holdWeight;
    
    #if defined(USE_PGM_CASTON) || defined(USE_PGM_TM) || defined(USE_PGM_HD) || defined(USE_PGM_DB)
    /** @brief Sum능 사용 무게, 평균값*/
	INT32S sumSaveWeight;  
    #endif
	
	#ifdef USE_ADD_KEY_FOR_CERTIFICATION
	INT8U	addState;
	INT8U	printerState;
	#endif
} SALE_DATA;

/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLE EXTERNALS
********************************************************************************
*/
/* Insert global variable externals here */
extern SALE_DATA SaleData;
extern INT8U checkSamePreviousWeight;

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */


#endif /* _SALE_DATA_H */
