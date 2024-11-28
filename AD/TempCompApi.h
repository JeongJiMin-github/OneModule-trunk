/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Temperature Compensation API Header File\n
* @file    TempCompApi.h
* @version 1.0
* @date    2006/12/05
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/12/05 created by HYP \n
********************************************************************************
*/

#ifndef _TEMP_COMP_API_H
#define _TEMP_COMP_API_H

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
/** @brief Calibration시 온도보상 관련 데이터*/
typedef _PACKED_DEF struct
{
#ifdef USE_TEMP_COMP_MWIIH
	/** @brief 온도보상 사용여부*/
	INT8U	tempCompEnable; 	
//절대 보상
	/** @brief 저온에서 Calibration 영점값*/
	INT32S	calZeroValueAtLowTemp;
	/** @brief 저온에서 Calibration 스판값*/
	INT32S	calSpanValueAtLowTemp;
	/** @brief 상온에서 Calibration 영점값*/
	INT32S	calZeroValueAtMidTemp;
	/** @brief 상온에서 Calibration 스판값*/
	INT32S	calSpanValueAtMidTemp;
	/** @brief 고온에서 Calibration 영점값*/
	INT32S	calZeroValueAtHighTemp;
	/** @brief 고온에서 Calibration 스판값*/
	INT32S	calSpanValueAtHighTemp;
	/** @brief 저온값*/
	INT16S	lowTemp;
	/** @brief 상온값*/
	INT16S	midTemp;
	/** @brief 고온값*/
	INT16S	highTemp;
	/** @brief ReCalibration 온도*/
	INT8S reCalTemp;
#else //32/32 byte <= PARAM_TEMP_AREA_SIZE
	/** @brief 온도보상 사용여부*/
	INT8U	tempCompEnable;		
//절대 보상
	/** @brief 저온에서 Calibration 영점값*/
//	INT32S	calZeroValueAtLowTemp;			
	/** @brief 저온에서 Calibration 스판값*/
//	INT32S	calSpanValueAtLowTemp;			
	/** @brief 상온에서 Calibration 영점값*/
//	INT32S	calZeroValueAtMidTemp;			
	/** @brief 상온에서 Calibration 스판값*/
//	INT32S	calSpanValueAtMidTemp;			
	/** @brief 고온에서 Calibration 영점값*/
//	INT32S	calZeroValueAtHighTemp;			
	/** @brief 고온에서 Calibration 스판값*/
//	INT32S	calSpanValueAtHighTemp;			
//상대 보상
	/** @brief 저온에서의 상온대비 영점 변화값(normalized) */
	INT32S	deltaZeroValueAtLowTemp;			
	/** @brief 저온에서의 상온대비 스판 변화값(normalized) */
	INT32S	deltaSpanValueAtLowTemp;			
	/** @brief reserved */
	INT32S	reserved1;			
	/** @brief reserved */
	INT32S	reserved2;			
	/** @brief 고온에서의 상온대비 영점 변화값(normalized) */
	INT32S	deltaZeroValueAtHighTemp;			
	/** @brief 고온에서의 상온대비 스판 변화값(normalized) */
	INT32S	deltaSpanValueAtHighTemp;			
	/** @brief 저온값*/
	INT8S	lowTemp;						
	/** @brief 상온값*/
	INT8S	midTemp;						
	/** @brief 고온값*/
	INT8S	highTemp;					
	/** @brief 온도보상 Calibration 가능여부*/
	INT8U	calAvailable;	
	/** @brief ReCalibration 온도*/
	INT8S	reCalTemp;
#endif
}TEMP_PARAM_STRUCT;//30/32 byte <= PARAM_TEMP_AREA_SIZE

/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLE EXTERNALS
********************************************************************************
*/
/* Insert global variable externals here */
extern TEMP_PARAM_STRUCT TempParam;

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */
 #if defined(USE_TEMP_COMP) || defined(USE_TEMP_COMP_MWIIH)
 extern void TempInitVariable (void);
 #ifdef USE_TEMP_COMP
 extern INT32S TempCompProc(INT32S lAd, INT8S currTemp);
 #elif defined(USE_TEMP_COMP_MWIIH)
 extern INT32S TempCompProc(INT32S lAd, INT16S currTemp);
 #endif
 extern void TempSetDefaultParam(void);
 extern void TempWriteParam(void);
 extern void TempReadParam(void);
 #endif	//USE_TEMP_COMP
#endif /* _TEMP_COMP_API_H */
