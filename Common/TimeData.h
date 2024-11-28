/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Time Data Header File\n
* @file    TimeData.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _TIME_DATA_H
#define _TIME_DATA_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
// refer to HT1361 datasheet (range:1~7)
/** @brief Time data MONDAY*/
#define MONDAY		1
/** @brief Time data TUESDAY*/
#define TUESDAY		2
/** @brief Time data WEDNESDAY*/
#define WEDNESDAY	3
/** @brief Time data THURSDAY*/
#define THURSDAY	4
/** @brief Time data FRIDAY*/
#define FRIDAY		5
/** @brief Time data SATURDAY*/
#define SATURDAY	6
/** @brief Time data SUNDAY*/
#define SUNDAY		7

/*
********************************************************************************
*                       GLOBAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert global typedef & struct here */
/** @brief Time data structure*/
typedef struct {
	/** @brief Second*/
	INT8U sec;
	/** @brief Minute*/
	INT8U min;
	/** @brief Hour*/
	INT8U hour;
	/** @brief Date*/
	INT8U date;
	/** @brief Month*/
	INT8U month;
	/** @brief Day, 0~6*/
	INT8U day;
	/** @brief year*/
	INT8U year;	
} TIME_DATA;

/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLE EXTERNALS
********************************************************************************
*/
/* Insert global variable externals here */
extern TIME_DATA TimeData;

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */


#endif /* _TIME_DATA_H */
