/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   PLU Data Header File\n
* @file    PluData.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _PLU_DATA_H
#define _PLU_DATA_H

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
//#ifdef USE_SALE_PRICE_EXTERNAL_PLU
#if defined (USE_SALE_PRICE_EXTERNAL_PLU) || defined (USE_SALE_PRICE_EXTERNAL_PLU_AP)
/* Insert global typedef & struct here */
/** @brief Direct PLU 개수 */
#define DIRECT_PLU_MAX				200
/** @brief Indirect PLU 개수 */
#define INDIRECT_PLU_MAX			0
/** @brief PLU Report 개수 */
#define PLU_REPORT_MAX_NUM		(DIRECT_PLU_MAX + 1)//PLU number 0xffff -> All plu report
/** @brief All PLU Report 번호 (PLU Report의 마지막 번호) */
#define REPORT_ALL_NUM				PLU_REPORT_MAX_NUM

/** @brief Direct PLU Name 길이 */
#ifdef USE_ADD_KEY_FOR_CERTIFICATION
/** @brief 호주 인증용 Direct PLU Name 길이 */
#define PLU_NAME_SIZE 				10	//
/** @brief Direct PLU Name 길이 */
#else 
/** @brief PLU Structure 길이 */
#define	PLU_NAME_SIZE 				16
#endif
//#define PLU_SIZE 					20
//#define PLU_SIZE 					26
#define PLU_SIZE 					30

/** @brief PLU Report Structure 길이 */
#define REPORT_SIZE 					6

/** @brief External PLU struct (20 byte) */
typedef _PACKED_DEF struct
{
	/** @brief Unit Price */
	INT32U unitPrice;
	/** @brief Tare */
	INT32U tare;
	/** @brief Valid date */
	INT16U validDate;	
	/** @brief name */
	char name[PLU_NAME_SIZE];
	/** @brief item code */
	INT32U itemCode;
} DIRECT_PLU_DATA;

#elif defined(USE_SALE_PRICE_COMPARE_PLU) //#if defined (USE_SALE_PRICE_EXTERNAL_PLU) || defined (USE_SALE_PRICE_EXTERNAL_PLU_AP)	

/* Insert global typedef & struct here */
/** @brief Direct PLU 개수 */
#define DIRECT_PLU_MAX				28
/** @brief Indirect PLU 개수 */
#define INDIRECT_PLU_MAX			200
/** @brief PLU Report 개수 */
#define PLU_REPORT_MAX_NUM		(DIRECT_PLU_MAX + 1)//PLU number 0xffff -> All plu report
/** @brief All PLU Report 번호 (PLU Report의 마지막 번호) */
#define REPORT_ALL_NUM				PLU_REPORT_MAX_NUM

/** @brief Direct PLU Name 길이 */
#define PLU_NAME_SIZE 				8

/** @brief Direct PLU struct (16 byte) */
typedef _PACKED_DEF struct
{
	/** @brief Unit Price */
	INT32U compareWeight;
	/** @brief Unit Price */
	INT32U limitWeight;
	/** @brief Unit Price */
	INT32U unitPrice;
	/** @brief Tare */
	INT32U tare;
	/** @brief name */
	char name[PLU_NAME_SIZE];
} DIRECT_PLU_DATA;

#else//#if defined (USE_SALE_PRICE_EXTERNAL_PLU) || defined (USE_SALE_PRICE_EXTERNAL_PLU_AP)

/* Insert global typedef & struct here */
/** @brief Direct PLU 개수 */
//#define DIRECT_PLU_MAX				28
#define DIRECT_PLU_MAX				29

/** @brief Indirect PLU 개수 */
#define INDIRECT_PLU_MAX			200
/** @brief PLU Report 개수 */
#define PLU_REPORT_MAX_NUM		(DIRECT_PLU_MAX + 1)//PLU number 0xffff -> All plu report
/** @brief All PLU Report 번호 (PLU Report의 마지막 번호) */
#define REPORT_ALL_NUM				PLU_REPORT_MAX_NUM

#ifdef USE_PLU_NAME_SIZE_16
/** @brief Direct PLU Name 길이 */
#define PLU_NAME_SIZE 				16
#else
/** @brief Direct PLU Name 길이 */
#define PLU_NAME_SIZE 				8
#endif

/** @brief Direct PLU struct (16 byte) */
typedef _PACKED_DEF struct
{
	/** @brief Unit Price */
	INT32U unitPrice;
	/** @brief Tare */
	INT32U tare;
	/** @brief name */
	char name[PLU_NAME_SIZE];
} DIRECT_PLU_DATA;
#endif//#if defined (USE_SALE_PRICE_EXTERNAL_PLU) || defined (USE_SALE_PRICE_EXTERNAL_PLU_AP)

/** @brief Indirect PLU struct (4 byte) */
typedef _PACKED_DEF struct
{
	/** @brief Unit Price */
	INT32U unitPrice;
} INDIRECT_PLU_DATA;

/** @brief PLU Report struct (6 byte)*/
typedef _PACKED_DEF struct
{
	/** @brief Price sum */
	INT32U price;
	/** @brief sale count */
	INT16U count;
} PLU_REPORT_DATA;

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


#endif /* _PLU_DATA_H */
