/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Printer API Header File\n
* @file    PrtApi.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _PRT_API_H
#define _PRT_API_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
#define PRT_PARAM_OFFSET_1ST 				0
#define PRT_PARAM_OFFSET_LF_SIZE			1
#define PRT_PARAM_OFFSET_BARCODE_TYPE	2
//#define PRT_PARAM_DEVICE_NUM			1
#define PRT_PARAM_DEVICE_NUM			3   //PRT_PARAM_OFFSET_LF_SIZE 겹침.

#define PRT_NOT_USE 					0
#define PRT_USE_BY_KEY 					1
#define PRT_USE_BY_AUTO					2
#define PRT_USE_BY_STREAM				3
#define PRT_USE_BY_CHANGE_WEIGHT		4
#define PRT_USE_BY_ENTER				5
#define PRT_USE_BY_FLOW					6

#define PRT_NUM_TYPE_WEIGHT			0
#define PRT_NUM_TYPE_PRICE				1
#define PRT_NUM_TYPE_FILL_SPACE		2
#define PRT_NUM_TYPE_FILL_ZERO		3

#if (MODEL_NUM == MODEL_OMV4_ER2_LCD) && defined(COUNTRY_TURKEY)
//USET>PRINT STREAM 순서
#define PRT_TRSRT						0
#define PRT_TYPE7						1
//#define PRT_CAS22BYTE					2	//사용하는 곳 없음
#define PRT_DEP50						3
#define PRT_DLP50						4
#define PRT_NONE						5
#define PRT_RESERVED					6 //USET>PRINT 6번 사용 시 수정 필요
#else
#define PRT_NONE						0
#ifdef USE_PRII_SPAIN_$_PROTOCOL
#define PRT_RS_PC						PRT_NONE
#endif /* USE_PRII_SPAIN_$_PROTOCOL */
#define PRT_DEP50						1
#define PRT_DLP50						2
#if defined (USE_REAL_STREAM)
	#define PRT_STREAM					3
#elif defined (USE_PRII_SPAIN_CO_PROTOCOL)
	#define PRT_RS_CO					3
#else
	#define PRT_RESERVED				3
#endif
#ifdef USE_PRII_SPAIN_ST_PROTOCOL
	#define PRT_RS_ST					4
#endif /* USE_PRII_SPAIN_ST_PROTOCOL */
#endif

/*
********************************************************************************
*                       GLOBAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert global typedef & struct here */
/** @brief Printer Setting Struct */
typedef struct 
{
	/** @brief 프린트 방법 */
	INT8U method;
	/** @brief date print enable */
	INT8U dateUse;
	/** @brief time print enable */
	INT8U timeUse;	
	/** @brief time print enable */
	INT8U lineFeedSize;
	/** @brief time print enable */
	INT8U barcodeType;
	/** @brief time print enable */
	INT8U barcodeOnOffFlag;
} PRT_SETTING;
//pgmPrintOffet = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST);
//pgmPrintMethod = pgmPrintOffet >> 6;	//00: off, 01: key, 10: auto, 11: stream (Use two bits)
//pgmDateOnOffFlag = (pgmPrintOffet & 0x20) >> 5;	//0: off, 1: on
//pgmTimeOnOffFlag = (pgmPrintOffet & 0x10) >> 4;	//0: off, 1: on 		
//pgmLabelOnOffFlag = (pgmPrintOffet & 0x08) >> 3;	//0: off, 1: on

/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLE EXTERNALS
********************************************************************************
*/
/* Insert global variable externals here */
extern PRT_SETTING	PrtSetting;

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */
extern void PrtSetDefaultParam(void);
extern void PrtInit(void);
extern void PrtProc(void);
extern void PrtSetTabPos(INT8U tabNum, INT8U tabPos);
extern void PrtPutTab(INT8U tabCount);
extern void PrtPutCmd(char cmd);
extern void PrtPutChar(char ch);
extern void PrtPutStr(char *src);
extern void PrtPutData(char *src, INT8U size);
extern void PrtPutConstStr(const char *src);
//extern void PrtPutStrWithCRLF(char *src);	//not use
//extern void PrtPutConstStrWithCRLF(char *src);	//not use
extern void PrtPutNum(INT32S num, INT8U size, INT8U decPos, INT8U decType, INT8U useThousandPt);
extern void PrtPutDecNum(INT32S num, INT8U size, INT8U type);
extern void PrtPutCRLF(INT8U lineNum);
extern void PrtPutLine(void);
//extern void PrtPutCmd(char *cmdType);	//not use

//extern void PrtTest(void);	//not use

#endif /* _PRT_API_H */
