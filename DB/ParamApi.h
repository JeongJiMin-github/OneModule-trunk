/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Parameter Data API Header File\n
           for One Module
* @file    ParamApi.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _PARAM_API_H
#define _PARAM_API_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief Parameter Area Number 개수 
*          Area 개수 변경 시 반드시 변경 할 것 
*/
#define PARAM_MAX_AREA_NUM		12	

// 주의 !!!!!!!
// Parameter Area Number 변경 시 ParamBaseAddr table 반드시 변경 할 것 
/** @brief AD Parameter Area Number */
#define PARAM_AD_AREA_NUM		0
/** @brief Head Area Number */
#define PARAM_HEAD_AREA_NUM		1
/** @brief Sale Parameter Area Number */
#define PARAM_SALE_AREA_NUM		2
/** @brief Key Parameter Area Number */
#define PARAM_KEY_AREA_NUM		3
/** @brief Display Parameter Area Number */
#define PARAM_DISP_AREA_NUM		4
/** @brief log Parameter Area Number */
#define PARAM_LOG_AREA_NUM		5
/** @brief Communication Parameter Area Number */
#define PARAM_COMM_AREA_NUM		6
/** @brief Printer Parameter Area Number */
#define PARAM_PRT_AREA_NUM		7
/** @brief Power Parameter Area Number */
#define PARAM_POWER_AREA_NUM		8
/** @brief Buzzer Parameter Area Number */
#define PARAM_BUZZER_AREA_NUM	9
/** @brief 추가 Parameter Area Number */
#define PARAM_AUX_AREA_NUM		10
/** @brief Parameter Version */
#define PARAM_VERSION_AREA_NUM	11

/** @brief AD_BASE_ADDR*/
#define PARAM_AD_BASE_ADDR		(STORAGE_PARAM_BASE_ADDR)
/** @brief AD_AREA_SIZE*/
#define PARAM_AD_AREA_SIZE		0x100

/** @brief HEAD_BASE_ADDR*/
#define PARAM_HEAD_BASE_ADDR	(PARAM_AD_BASE_ADDR + PARAM_AD_AREA_SIZE)
/** @brief HEAD_AREA_SIZE*/
#define PARAM_HEAD_AREA_SIZE	0x40

/** @brief SALE_BASE_ADDR*/
#define PARAM_SALE_BASE_ADDR	(PARAM_HEAD_BASE_ADDR + PARAM_HEAD_AREA_SIZE)
/** @brief SALE_AREA_SIZE*/
#define PARAM_SALE_AREA_SIZE	0x40
																						                 
/** @brief KEY_BASE_ADDR*/
#define PARAM_KEY_BASE_ADDR	(PARAM_SALE_BASE_ADDR + PARAM_SALE_AREA_SIZE)
/** @brief KEY_AREA_SIZE*/
#define PARAM_KEY_AREA_SIZE	0x40

/** @brief DISP_BASE_ADDR*/
#define PARAM_DISP_BASE_ADDR	(PARAM_KEY_BASE_ADDR + PARAM_KEY_AREA_SIZE)
/** @brief DISP_AREA_SIZE*/
#define PARAM_DISP_AREA_SIZE	0x10

/** @brief LOG_BASE_ADDR*/
#define PARAM_LOG_BASE_ADDR	(PARAM_DISP_BASE_ADDR + PARAM_DISP_AREA_SIZE)
/** @brief LOG_AREA_SIZE*/
#define PARAM_LOG_AREA_SIZE	0x10

/** @brief COMM_BASE_ADDR*/
#define PARAM_COMM_BASE_ADDR	(PARAM_LOG_BASE_ADDR + PARAM_LOG_AREA_SIZE)
/** @brief COMM_AREA_SIZE*/
#define PARAM_COMM_AREA_SIZE	0x4

/** @brief PRT_BASE_ADDR*/
#define PARAM_PRT_BASE_ADDR	(PARAM_COMM_BASE_ADDR + PARAM_COMM_AREA_SIZE)
/** @brief PRT_AREA_SIZE*/
#define PARAM_PRT_AREA_SIZE		0x4

/** @brief POWER_BASE_ADDR*/
#define PARAM_POWER_BASE_ADDR	(PARAM_PRT_BASE_ADDR + PARAM_PRT_AREA_SIZE)
/** @brief POWER_AREA_SIZE*/
#define PARAM_POWER_AREA_SIZE	0x8

/** @brief BUZZER_BASE_ADDR*/
#define PARAM_BUZZER_BASE_ADDR	(PARAM_POWER_BASE_ADDR + PARAM_POWER_AREA_SIZE)
/** @brief BUZZER_AREA_SIZE*/
#define PARAM_BUZZER_AREA_SIZE	0x4

/** @brief AUX_BASE_ADDR*/
#define PARAM_AUX_BASE_ADDR		(PARAM_BUZZER_BASE_ADDR + PARAM_BUZZER_AREA_SIZE)
/** @brief AUX_AREA_SIZE*/
#define PARAM_AUX_AREA_SIZE		0x4

// 주의 !!!!!
// 반드시 Parameter Area의 마지막에 위치해야 함
// Parameter Area를 모두 사용하도록 각 AREA SIZE 정할 것
/** @brief VERSION_BASE_ADDR*/
#define PARAM_VERSION_BASE_ADDR	(PARAM_AUX_BASE_ADDR + PARAM_AUX_AREA_SIZE)
/** @brief VERSION_AREA_SIZE*/
#define PARAM_VERSION_AREA_SIZE	0x8

#define PARAM_TOTAL_USING_SIZE	(PARAM_VERSION_BASE_ADDR + PARAM_VERSION_AREA_SIZE - STORAGE_PARAM_BASE_ADDR)

#if (PARAM_TOTAL_USING_SIZE > STORAGE_PARAM_AREA_SIZE)
/** @brief Parameter size over error*/
#error Parameter area size is over!!!!!!
#endif

/** @brief HEAD_PARAM_OFFSET_LINE1*/
#define 	HEAD_PARAM_OFFSET_LINE1	0									
/** @brief HEAD_LINE1_SIZE*/
#define 	HEAD_LINE1_SIZE		24
/** @brief HEAD_PARAM_OFFSET_LINE2*/
#define		HEAD_PARAM_OFFSET_LINE2	(HEAD_PARAM_OFFSET_LINE1 + HEAD_LINE1_SIZE)
/** @brief HEAD_LINE2_SIZE*/
#define 	HEAD_LINE2_SIZE		24
/** @brief HEAD_PARAM_OFFSET_LINE3*/
#define     HEAD_PARAM_OFFSET_LINE3	(HEAD_PARAM_OFFSET_LINE2 + HEAD_LINE2_SIZE)
/** @brief HEAD_LINE3_SIZE*/
#define	HEAD_LINE3_SIZE	16


/** @brief LOG_TEST_DATA_SIZE (5 Byte binery : YY|MM|DD|Result)*/
#define 	LOG_TEST_DATA_SIZE	4
/** @brief LOG_TEST_DATE_SIZE (3 Byte binery : YY|MM|DD)*/
#define 	LOG_TEST_DATE_SIZE	3
/** @brief CAL_PASSWORD_SIZE (2 Byte : INT16U)*/
#define 	CAL_PASSWORD_SIZE	2
/** @brief LCAL_PASSWORD_ENABLE_SIZE (1 Byte : INT8U)*/
#define 	CAL_PASSWORD_ENABLE_SIZE	1
/** @brief CAL_WARNNING_PERIOD_SIZE (1 Byte : INT8U)*/
#define 	CAL_WARNNING_PERIOD_SIZE		1
/** @brief CAL_WARNNING_WAIT_COUNT_SIZE (1 Byte : INT8U)*/
#define 	CAL_WARNNING_WAIT_COUNT_SIZE		1

/** @brief LOG_PARAM_OFFSET_MODULE_TEST_DATA */
#define 	LOG_PARAM_OFFSET_MODULE_TEST_DATA		0									
/** @brief LOG_PARAM_OFFSET_SCALE_TEST_DATA */
#define		LOG_PARAM_OFFSET_SCALE_TEST_DATA	(LOG_PARAM_OFFSET_MODULE_TEST_DATA + LOG_TEST_DATA_SIZE)
/** @brief LOG_PARAM_OFFSET_VALID_START_DATE */
#define 	LOG_PARAM_OFFSET_VALID_START_DATE			(LOG_PARAM_OFFSET_SCALE_TEST_DATA + LOG_TEST_DATA_SIZE)									
/** @brief LOG_PARAM_OFFSET_CAL_PASSWORD */		
#define		LOG_PARAM_OFFSET_CAL_PASSWORD		(LOG_PARAM_OFFSET_VALID_START_DATE + LOG_TEST_DATE_SIZE)
/** @brief LOG_PARAM_OFFSET_CAL_PASSWORD_ENABLE_KEY */
#define		LOG_PARAM_OFFSET_CAL_PASSWORD_ENABLE_KEY	(LOG_PARAM_OFFSET_CAL_PASSWORD + CAL_PASSWORD_SIZE)
/** @brief LOG_PARAM_OFFSET_CAL_WARNNING_PERIOD */
#define		LOG_PARAM_OFFSET_CAL_WARNNING_PERIOD	(LOG_PARAM_OFFSET_CAL_PASSWORD_ENABLE_KEY + CAL_PASSWORD_ENABLE_SIZE)
/** @brief LOG_PARAM_OFFSET_VALID_START_WAIT_COUNT */
#define		LOG_PARAM_OFFSET_VALID_START_WAIT_COUNT	(LOG_PARAM_OFFSET_CAL_WARNNING_PERIOD + CAL_WARNNING_PERIOD_SIZE)

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
extern void ParamReadData(INT16U ParamAreaNum, INT8U ParamOffset, INT8U *dst, INT16U size);
extern void ParamWriteData(INT16U ParamAreaNum, INT8U ParamOffset, INT8U *src, INT16U size);

extern INT8U ParamReadByte(INT16U ParamAreaNum, INT8U ParamOffset);
extern void ParamWriteByte(INT16U ParamAreaNum, INT8U ParamOffset, INT8U byte);

extern INT16U ParamReadWord(INT16U ParamAreaNum, INT8U ParamOffset);
extern void ParamWriteWord(INT16U ParamAreaNum, INT8U ParamOffset, INT16U word);

extern INT32U ParamReadDword(INT16U ParamAreaNum, INT8U ParamOffset);
extern void ParamWriteDword(INT16U ParamAreaNum, INT8U ParamOffset, INT32U dword);

extern void ParamTest(void);

#endif /* _PARAM_API_H */
