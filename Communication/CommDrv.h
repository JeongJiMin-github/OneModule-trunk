/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Communication Driver Header File\n
* @file    CommDrv.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _COM_DRV_H
#define _COM_DRV_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief Serial Communication Baud Rate 9600 */
#define BAUD_RATE_9600     	0
/** @brief Serial Communication Baud Rate 19200 */
#define BAUD_RATE_19200    	1
/** @brief Serial Communication Baud Rate 38400 */
#define BAUD_RATE_38400    	2
/** @brief Serial Communication Baud Rate 57600 */
#define BAUD_RATE_57600    	3
/** @brief Serial Communication Baud Rate 115200 */
#define BAUD_RATE_115200    4
/** @brief Serial Communication Baud Rate 2400 For ECR_TYPE_9 */
#define BAUD_RATE_2400		5

/** @brief ECR Type 0 */
#define ECR_TYPE_0    		0
/** @brief ECR Type 1 */
#define ECR_TYPE_1    		1
/** @brief ECR Type 2 */
#define ECR_TYPE_2    		2
/** @brief ECR Type 3 */
#define ECR_TYPE_3    		3
/** @brief ECR Type 4 */
#define ECR_TYPE_4    		4
/** @brief ECR Type 5 */
#define ECR_TYPE_5    		5
/** @brief ECR Type 6 */
#define ECR_TYPE_6    		6
/** @brief ECR Type 7 */
#define ECR_TYPE_7    		7
/** @brief ECR Type 8 */
#define ECR_TYPE_8    		8
/** @brief ECR Type 9 */
#define ECR_TYPE_9    		9
/** @brief ECR Type 10 */
#define ECR_TYPE_10    		10
/** @brief ECR Type 11 */
//#define ECR_TYPE_11    		11
#ifdef DIALOG6 
/** @brief ECR Type 10 */
#define ECR_TYPE_12    		11
#define ECR_TYPE_13    		12
#define ECR_TYPE_14    		13
#define ECR_TYPE_15    		14
#endif
#ifdef USE_SATO_PRINTER_PROTOCOL
#define ECR_TYPE_12    		11
#endif
#ifdef USE_SLOVAKIA_PROTOCOL
#define ECR_TYPE_16    		15
#endif
#define ECR_TYPE_17			16
#define ECR_TYPE_20			19
#ifdef USE_PD2_ECR
#define ECR_TYPE_18			17
#define ECR_TYPE_19			18
#define ECR_TYPE_21			20
#define ECR_TYPE_22			21
#define ECR_TYPE_23			22
#define ECR_TYPE_24			23
#define ECR_TYPE_25			24
#define ECR_TYPE_26			25
#define ECR_TYPE_27			26
#define ECR_TYPE_28			27
#define ECR_TYPE_29			28
#define ECR_TYPE_30			29
#define ECR_TYPE_31			30
#define ECR_TYPE_32			31
#define ECR_TYPE_33			32
#define ECR_TYPE_34			33
#define ECR_TYPE_35			34
#endif
#define ECR_TYPE_36			35
#define ECR_TYPE_37			36

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
extern RING_BUF SerialRxRingBuf;
extern RING_BUF SerialTxRingBuf;
#ifdef USE_UART1 
extern RING_BUF SerialRx1RingBuf;
extern RING_BUF SerialTx1RingBuf;
#endif
/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */
#ifdef USE_UART1_BAUDRATE_SET
extern void CommDrvInit(INT8U uart0_baud, INT8U uart1_baud);
#else
extern void CommDrvInit(INT8U baud);
#endif
extern void CommDrvProc(void);
#ifdef USE_UART1 
extern void Comm1DrvProc(void);
#endif
#endif /* _COM_DRV_H */
