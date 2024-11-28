/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Storage API Header File\n
* @file    StorageApi.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _STORAGE_API_H
#define _STORAGE_API_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief STORAGE_BASE_ADDR*/
#define STORAGE_PARAM_BASE_ADDR				0x0000
/** @brief STORAGE_AREA_SIZE*/
#define STORAGE_PARAM_AREA_SIZE				0x0200
/** @brief STORAGE_PLU_BASE_ADDR*/
#define STORAGE_PLU_BASE_ADDR					0x0200 //(STORAGE_PARAM_BASE_ADDR + STORAGE_PARAM_AREA_SIZE)
/** @brief STORAGE_DIRECT_PLU_BASE_ADDR*/
#define STORAGE_DIRECT_PLU_BASE_ADDR			(STORAGE_PLU_BASE_ADDR)
#ifdef USE_PLU_NAME_SIZE_16
/** @brief STORAGE_DIRECT_PLU_AREA_SIZE*/
#define STORAGE_DIRECT_PLU_AREA_SIZE			0x02B8
#else
/** @brief STORAGE_DIRECT_PLU_AREA_SIZE*/
#define STORAGE_DIRECT_PLU_AREA_SIZE			0x01D0
#endif
/** @brief STORAGE_INDIRECT_PLU_BASE_ADDR*/
#define STORAGE_INDIRECT_PLU_BASE_ADDR		(STORAGE_DIRECT_PLU_BASE_ADDR + STORAGE_DIRECT_PLU_AREA_SIZE)
/** @brief STORAGE_INDIRECT_PLU_AREA_SIZE*/
#define STORAGE_INDIRECT_PLU_AREA_SIZE		0x0320
/** @brief STORAGE_REPORT_BASE_ADDR*/
#define STORAGE_REPORT_BASE_ADDR				(STORAGE_INDIRECT_PLU_BASE_ADDR + STORAGE_INDIRECT_PLU_AREA_SIZE)
#ifdef USE_CTS_OMV2
	/** @brief STORAGE_REPORT_AREA_SIZE*/
	#define STORAGE_REPORT_AREA_SIZE				0xB4
#else
	/** @brief STORAGE_REPORT_AREA_SIZE*/
	#define STORAGE_REPORT_AREA_SIZE				0x0100
#endif
/** @brief STORAGE_EXT_PARAM_BASE_ADDR*/
#define STORAGE_EXT_PARAM_BASE_ADDR			(STORAGE_REPORT_BASE_ADDR + STORAGE_REPORT_AREA_SIZE)
/** @brief STORAGE_EXT_PARAM_AREA_SIZE*/
#define STORAGE_EXT_PARAM_AREA_SIZE				0x0001
#ifdef USE_CTS_MODE
/** @brief STORAGE_CTS_DATA_BASE_ADDR_*/
#define STORAGE_CTS_DATA_BASE_ADDR   (STORAGE_EXT_PARAM_BASE_ADDR + STORAGE_EXT_PARAM_AREA_SIZE)
/** @brief STORAGE_CTS_DATA_BASE_SIZE_*/
#ifdef USE_CTS_OMV2
	#define STORAGE_CTS_DATA_AREA_SIZE				0x50
#else
	#define STORAGE_CTS_DATA_AREA_SIZE				0x80
#endif
#endif
/** @brief External STORAGE_PLU_AREA_SIZE*/
#define EXT_STORAGE_PLU_BASE_ADDR			0x1000
/** @brief External STORAGE_PLU_AREA_SIZE*/
#define EXT_STORAGE_PLU_AREA_SIZE				(DIRECT_PLU_MAX * PLU_SIZE)	//200 PLU, Each 30 Byte
/** @brief External STORAGE_REPORT_BASE_ADDR*/
#define EXT_STORAGE_REPORT_BASE_ADDR		(EXT_STORAGE_PLU_BASE_ADDR + EXT_STORAGE_PLU_AREA_SIZE)
/** @brief External STORAGE_REPORT_BASE_ADDR*/
#define EXT_STORAGE_JIG_CHECK_ADDR			0x2FFF
/*
********************************************************************************
*                       GLOBAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert global typedef & struct here */


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
extern void StorageTest(void);
extern void StorageReadData(INT16U addr, INT8U *dst, INT16U size);
extern void StorageWriteData(INT16U addr, INT8U *src, INT16U size);
extern INT8U StorageReadByte(INT16U addr);
extern void StorageWriteByte(INT16U addr, INT8U byte);
extern INT16U StorageReadWord(INT16U addr);
extern void StorageWriteWord(INT16U addr, INT16U word);
extern INT32U StorageReadDword(INT16U addr);
extern void StorageWriteDword(INT16U addr, INT32U dword);
#endif /* _STORAGE_API_H */
