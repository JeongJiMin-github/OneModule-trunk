/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Scale Mode Applicaion Header File\n
* @file    ScaleModeApp.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _SCALE_MODE_APP_H
#define _SCALE_MODE_APP_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief Scale Mode : Sale Mode (weighing) */
#define SCALE_MODE_SALE	0
/** @brief Scale Mode : Program Mode */
#define SCALE_MODE_PGM	1
/** @brief Scale Mode : Calibration Mode */
#define SCALE_MODE_CAL	2


/** @brief Scale Param Addr			*/
/*
********************************************************************************
*                       GLOBAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert global typedef & struct here */
/** @brief Scale Mode Typedef for recognition */
typedef INT8U SCALE_MODE_TYPE;

/** @brief Backlight 사용 유무*/
typedef _PACKED_DEF struct
{
	/** @brief Backlight 사용 유무*/
	INT8U		scaleUseBL;
} SCALE_PARAMETER;
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLE EXTERNALS
********************************************************************************
*/
/* Insert global variable externals here */
extern SCALE_MODE_TYPE ScaleMode;

extern SCALE_PARAMETER ScaleParam;

#ifdef USE_CTS_MODE
extern INT8U CtsStructBuf[80];
#endif

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */
extern void ScaleModeSetDefaultParam(INT8U type);
extern BOOLEAN ScaleModeCheckVersion(void);
extern void ScaleModeInit(void);
extern void ScaleModeProc(void);
#ifdef USE_CTS_MODE
extern void ScaleModeCtsProc(void);
#endif
#endif /* _SCALE_MODE_APP_H */
