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
/** @brief Sale ���� ���Ǵ� Struct*/
typedef struct
{
	/** @brief saleState*/
	INT8U	saleState;
	/** @brief add��� Count����*/
	INT16U	addCount;
	/** @brief add��� Price*/
	INT32U	addPrice;

	/** @brief ���� ���� ��*/
	INT32S	weight;
	/** @brief ���� ����ϴ� ���� unit, 0:kg, 1:lb, 2:oz, 3:g*/
	INT8U 	weightUnit;
	/** @brief ���԰��� Decimal Position */
	INT8S 	weightDecPos;
	/** @brief ������ �ڸ����� ��ȿ������ ��Ÿ���� �÷���*/
	INT8U	lastDigitIsInvalid;

#if defined(DIALOG6) || defined (USE_SMART_BT_PROTOCOL) || defined (USE_SATO_PRINTER_PROTOCOL)
	/** @brief Key tare ��*/
	INT32S	keyTare;
	/** @brief Key tare ��*/
	INT8U	zeroErrFlag;	//not use
#endif
	/** @brief Unit price ��*/
	INT32U	unitPrice;
	/** @brief Total price ��*/
	INT32U	totalPrice;	
	
	/** @brief weight dot character*/
	INT8U 	weightDotChar;	
	/** @brief price dot character*/
	INT8U	priceDotChar;
	/** @brief price decimal position*/
	INT8U	priceDecPos;
	
	/** @brief Plu �ִ� ���� */
	//INT8U 	pluMaxNum;	//not use
	/** @brief ȣ��� Plu number, pluNum = 0 �� Plu�� ȣ����� ���� Normal ������*/
	INT16U  pluNum;
	/** @brief Hold��� ��� ����, ��հ�*/
	INT32S holdWeight;
    
    #if defined(USE_PGM_CASTON) || defined(USE_PGM_TM) || defined(USE_PGM_HD) || defined(USE_PGM_DB)
    /** @brief Sum�� ��� ����, ��հ�*/
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
