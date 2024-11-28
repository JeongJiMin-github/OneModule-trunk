/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Communication API Header File\n
* @file    CommApi.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _COM_API_H
#define _COM_API_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */

/** @brief COMM Parameter offset*/
#define	COMM_PARAM_OFFSET_BAUDRATE		0	// 1
#define	COMM_PARAM_OFFSET_ECR_TYPE		1	// 2
#define COMM_PARAM_OFFSET_1ST			2	// 3
#define COMM_PARAM_OFFSET_SELECT		3	// 4


#ifdef MODEL_OMV4_PDN_LED
    //RS232 Communication Type
    #define RS232_ECR	0
    #define RS232_DEP	1
    #define RS232_DLP	2
    #define RS232_BLE	3
    #define RS232_COMM_MENU_LAST	RS232_BLE
#endif
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
#ifdef USE_TWN_FUNCS
extern BOOLEAN BtCmdPowerOffFlag;
extern BOOLEAN BtCmdDispOffFlag;
#endif
#ifdef USE_BT_CONNECT
/** @brief 현재 BT flag*/
extern INT8U BtResetFlag;
extern INT8U BtSoftResetFlag;
extern INT8U BtOnFlag;
extern INT8U BtEnableFlag;
#endif

#ifdef USE_TWN_FUNCS
/** @brief 현재 BT flag*/
extern INT8U BtResetFlag;
extern INT8U BtOnFlag;
extern INT8U BtErrFlag;
#endif

#ifdef USE_CONTI_SEND_WEIGHT_DATA
extern INT8U sendWeightFlag;
#endif
#ifdef USE_ECR_TYPE_7
extern INT8U sendType7Flag;
#endif

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */
extern void CommSetDefaultParam(void);
extern void CommInit(void);
extern void CommProc(void);

extern void CommOmInit(void);
extern BOOLEAN CommOmInterpreter(RING_BUF *txRingBufPtr, char byte);
#ifdef USE_COMMAND_KEY
extern void CommSetKeyCmdErrFlag(BOOLEAN flag);
#endif

#ifdef USE_BT_COMM_API
extern void Btreset(void);
extern void Btmodeconversion(void);
#endif

#ifdef USE_BT_CONNECT
extern void Btreset(void);
extern void BtSoftreset(void);
extern void Btmodeconversion(void);
#endif

#ifdef USE_XBEE
extern void XbeeModuleInit(void);
extern void XbeeModuleSetchannel(void);
extern void XbeeModuleWriteCommand(void);
extern void XbeeModuleChannelReadCommand(void);
extern void XbeeModuleExitCommand(void);
extern void XbeeModeCmdOk(void);
#endif
#ifdef USE_SATO_PRINTER_PROTOCOL
extern void sendDLP50withParity(void);
#endif
#ifdef USE_PRII_SPAIN_$_PROTOCOL
extern void comm_$_protocol_send_weight(RING_BUF *txRingBufPtr);
#endif /* USE_PRII_SPAIN_$_PROTOCOL */
#endif /* _COM_API_H */
