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
/** @brief Calibration�� �µ����� ���� ������*/
typedef _PACKED_DEF struct
{
#ifdef USE_TEMP_COMP_MWIIH
	/** @brief �µ����� ��뿩��*/
	INT8U	tempCompEnable; 	
//���� ����
	/** @brief ���¿��� Calibration ������*/
	INT32S	calZeroValueAtLowTemp;
	/** @brief ���¿��� Calibration ���ǰ�*/
	INT32S	calSpanValueAtLowTemp;
	/** @brief ��¿��� Calibration ������*/
	INT32S	calZeroValueAtMidTemp;
	/** @brief ��¿��� Calibration ���ǰ�*/
	INT32S	calSpanValueAtMidTemp;
	/** @brief ��¿��� Calibration ������*/
	INT32S	calZeroValueAtHighTemp;
	/** @brief ��¿��� Calibration ���ǰ�*/
	INT32S	calSpanValueAtHighTemp;
	/** @brief ���°�*/
	INT16S	lowTemp;
	/** @brief ��°�*/
	INT16S	midTemp;
	/** @brief ��°�*/
	INT16S	highTemp;
	/** @brief ReCalibration �µ�*/
	INT8S reCalTemp;
#else //32/32 byte <= PARAM_TEMP_AREA_SIZE
	/** @brief �µ����� ��뿩��*/
	INT8U	tempCompEnable;		
//���� ����
	/** @brief ���¿��� Calibration ������*/
//	INT32S	calZeroValueAtLowTemp;			
	/** @brief ���¿��� Calibration ���ǰ�*/
//	INT32S	calSpanValueAtLowTemp;			
	/** @brief ��¿��� Calibration ������*/
//	INT32S	calZeroValueAtMidTemp;			
	/** @brief ��¿��� Calibration ���ǰ�*/
//	INT32S	calSpanValueAtMidTemp;			
	/** @brief ��¿��� Calibration ������*/
//	INT32S	calZeroValueAtHighTemp;			
	/** @brief ��¿��� Calibration ���ǰ�*/
//	INT32S	calSpanValueAtHighTemp;			
//��� ����
	/** @brief ���¿����� ��´�� ���� ��ȭ��(normalized) */
	INT32S	deltaZeroValueAtLowTemp;			
	/** @brief ���¿����� ��´�� ���� ��ȭ��(normalized) */
	INT32S	deltaSpanValueAtLowTemp;			
	/** @brief reserved */
	INT32S	reserved1;			
	/** @brief reserved */
	INT32S	reserved2;			
	/** @brief ��¿����� ��´�� ���� ��ȭ��(normalized) */
	INT32S	deltaZeroValueAtHighTemp;			
	/** @brief ��¿����� ��´�� ���� ��ȭ��(normalized) */
	INT32S	deltaSpanValueAtHighTemp;			
	/** @brief ���°�*/
	INT8S	lowTemp;						
	/** @brief ��°�*/
	INT8S	midTemp;						
	/** @brief ��°�*/
	INT8S	highTemp;					
	/** @brief �µ����� Calibration ���ɿ���*/
	INT8U	calAvailable;	
	/** @brief ReCalibration �µ�*/
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
