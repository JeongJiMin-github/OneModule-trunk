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
/** @brief I2C Port �ߺ� ����� �����ϱ� ���� Flag */
extern INT8U I2cPortUseFlag;

/** @brief Key output port ����, Display output port �� ���� ���� ��� */
extern INT8U  KeyOutputPortStatus;

/** @brief �µ� ���� ���� cpu Ŭ���� �����Ҷ� Ʃ���ϴ� �� */
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
