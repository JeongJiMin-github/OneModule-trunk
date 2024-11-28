/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Hardware Driver Header File\n
           for ER-1 LED
* @file    HwDrv.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _HARDWARE_DRV_H
#define _HARDWARE_DRV_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/

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
/** @brief I2C Port 중복 사용을 제한하기 위한 Flag */
extern INT8U I2cPortUseFlag;

/** @brief Key output port 상태, Display output port 와 공용 사용시 사용 */
extern INT8U  KeyOutputPortStatus;

/** @brief 온도 보상에 따른 cpu 클럭을 변경할때 튜닝하는 값 */
extern unsigned int EfmTuning;

extern INT8S Add_Top;
extern INT8S Add_TIEMR1TOP;

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */
extern void HwDrvInit(void);
extern void HwDrvSetFactoryBoot(void);

#endif /* _HARDWARE_DRV_H */
