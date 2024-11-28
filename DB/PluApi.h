/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   PLU API Header File\n
* @file    PluApi.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _PLU_API_H
#define _PLU_API_H

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
extern void PluClearAllPlu(void);
extern void PluReadDirectPlu(INT16U pluNum, DIRECT_PLU_DATA *dst);
extern void PluWriteDirectPlu(INT16U pluNum, DIRECT_PLU_DATA *src);
#if defined(USE_SALE_PRICE) || defined(USE_SALE_PRICE_COMPARE) || defined(USE_SALE_PR) || defined(USE_SALE_ER_MLED)
extern void PluReadIndirectPlu(INT16U pluNum, INDIRECT_PLU_DATA *dst);
extern void PluWriteIndirectPlu(INT16U pluNum, INDIRECT_PLU_DATA *src);
#endif
extern void ReportClearAll(void);
extern void ReportReadData(INT16U pluNum, PLU_REPORT_DATA *dst);
extern void ReportWriteData(INT16U pluNum, PLU_REPORT_DATA *src);
//extern void PluTest(void);
#endif /* _PLU_API_H */
