/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Sale Mode Applicaion Header File\n
* @file    SaleModeApp.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _SALE_MODE_APP_H
#define _SALE_MODE_APP_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
#define PLU_ITEM     				1
#define FIXED_ITEM				2
#define MULTI_ITEM				3
#define CANCEL_WEIGHT_ITEM	4
#define CANCEL_FIXED_ITEM		5

#ifdef USE_UNITPRICE_5DIGIT
#define UNITPRICE_MAX				99999
#else
#define UNITPRICE_MAX				999999
#endif

#ifdef USE_TOTAL_6DIGIT
#define TOTALPRICE_MAX				999999
#endif

#ifdef USE_TOTAL_7DIGIT
#define TOTALPRICE_MAX				9999999
#endif

#ifdef USE_TOTAL_8DIGIT
#define TOTALPRICE_MAX				99999999
#endif

#ifndef PRICE_SIZE
#define PRICE_SIZE				6
#endif

#define PERCENT_TARE_MAX		99
#define PERCENT_TARE_SIZE		2

#ifdef USE_MULTI_PRINT_FUNC
#define MULTI_PRINT_MAX			99
#endif

#define  SALE_STATE_NORMAL					0x00

#define  SALE_STATE_PLU_NUM				0x10
#define  SALE_STATE_PLU_TARE				0x11
#define  SALE_STATE_PLU_NAME				0x12
#define  SALE_STATE_PLU_VALID_DATE		0x13

#define  SALE_STATE_EURO_INPUT      			0x14
#define  SALE_STATE_PLU_ITEMCODE			0x15

#define  SALE_STATE_COMPARE_WEIGHT 		0x16 //for erjr compare ver.
#define  SALE_STATE_COMPARE_LIMIT   		0x17 //for erjr compare ver.


#define  SALE_STATE_SUM_TTP				0x20
#define  SALE_STATE_PAY						0x21
#define  SALE_STATE_PAY_CHANGE			0x22
//#define  SALE_STATE_PAY_2				0x23

#define  SALE_STATE_ADD					0x30
#define  SALE_STATE_FIXED_PRICE         		0x31
#define  SALE_STATE_CANCEL					0x32
#define  SALE_STATE_MULTI					0x33

#define  SALE_STATE_MODE					0x40
#define  SALE_STATE_MODE_SUM				0x41
#define  SALE_STATE_MODE_PLU				0x42
#define  SALE_STATE_MODE_PLU_NUM			0x43

#define	SALE_STATE_MULTI_PRINT				0x50 //add kmhnsb 090714
#define	SALE_STATE_MULTI_PRINT_FIXED		0x51 //add kmhnsb 090714
#define	SALE_STATE_MULTI_PRINT_WEIGH		0x52 //add kmhnsb 090714
#define	SALE_STATE_MULTI_PRINT_MULTI		0x53 //add kmhnsb 090714

#define  SALE_STATE_EURO_DISPLAY    		0xA0
#define  SALE_STATE_NOEURO_DISPLAY  		0xA1

#define  SALE_STATE_HOLD					0xE0
#define  SALE_STATE_PRECOUNT				0xE1	//add kkanno 070322
#define  SALE_STATE_COUNT					0xE2
#define  SALE_STATE_PRECOMPARE_1			0xE3
#define  SALE_STATE_PRECOMPARE_2			0xE4
#define  SALE_STATE_PRECOMPARE_3			0xE5	//add kkanno 070322
#define  SALE_STATE_PERCENT				0xE6
#define  SALE_STATE_COUNT_PRECOMPARE_1	0xE7
#define  SALE_STATE_COUNT_PRECOMPARE_2	0xE8
#define  SALE_STATE_COUNT_PRECOMPARE_3	0xE9
#define  SALE_STATE_UNITCHANGE			0xEA
#define  SALE_STATE_SAMPLE_WEIGHT			0xEB
#define  SALE_STATE_SUM_COUNT				0xEC
#define  SALE_STATE_SUM_WEIGHT			0xED
#define  SALE_STATE_AHOLD					0xEE

#define  SALE_STATE_PERCENT_TARE			0xF0
#define  SALE_STATE_PERCENT_PRICE			0xF1
#define  SALE_STATE_PERCENT_RATE			0xF2
#define  SALE_STATE_CHECK_BATTERY			0xF3
#define  SALE_STATE_OPENCLERK_CALL		0xF4
#ifdef USE_LONG_KEY_FOR_CHINAMARKET
#define  SALE_STATE_SEND_CHNDATA			0xF5
#define  SALE_STATE_SET_CHNDATA			0xF6
#endif


/* Address of Parameter */
#define  SALE_PARAM_OFFSET_1ST						0x00	// 1
#define  SALE_PARAM_OFFSET_2ND						0x01	// 1
#define  SALE_PARAM_OFFSET_3RD						0x02	// 1
#define  SALE_PARAM_OFFSET_4TH						0x03	// 1

#define  SALE_PARAM_OFFSET_COUNT_COMPARE_MIN_PCS 	0x04	///4
#define  SALE_PARAM_OFFSET_COUNT_COMPARE_MAX_PCS 	0x08	///4
#define  SALE_PARAM_OFFSET_COMPARE_RANGE			0x0C	///1
#define  SALE_PARAM_OFFSET_COUNT_COMPARE_RANGE		0x0D	///1
#define  SALE_PARAM_OFFSET_DATE_TYPE					0x0E	///1
#define	SALE_PARAM_OFFSET_MULTI_PRINT				0x0F		//1
#define  SALE_PARAM_OFFSET_EURO_DEC					0x10	// 1
#define  SALE_PARAM_OFFSET_EURO_RATE					0x11	// 4

#define  SALE_PARAM_OFFSET_PERCENT_RATE				0x15	// 1
#define  SALE_PARAM_OFFSET_PERCENT_TARE				0x16	// 4
#define  SALE_PARAM_OFFSET_PERCENT_PRICE			0x1A	// 4
#define  SALE_PARAM_OFFSET_KEY_LOCK 				0x1E	// 1
#define  SALE_PARAM_OFFSET_EXPANDED_WEIGHT_UNIT		0x1F	// 1

#define	SALE_PARAM_OFFSET_PRICE_SYMBOL				0x20	// 6
#define	PRICE_SYMBOL_SIZE								6
#define	SALE_PARAM_OFFSET_PCS_VALUE					0x26	///2
#define	SALE_PARAM_OFFSET_COUNT_WEIGHT			    0x28	///4
#define	SALE_PARAM_OFFSET_PERCENT_WEIGHT			0x2C	///4
#define	SALE_PARAM_OFFSET_MONTH						0x30	///1
#define	SALE_PARAM_OFFSET_DAY						0x31	///1
#define	SALE_PARAM_OFFSET_YEAR						0x32	///1
#define	SALE_PARAM_OFFSET_HOLD_TYPE					0x33 	///1

#define	SALE_PARAM_OFFSET_SALE_STATUS				0x34	///1

#define	SALE_PARAM_OFFSET_COMPARE_ON				0x35	///1
#define	SALE_PARAM_OFFSET_COMPARE_MIN_WEIGHT		0x36	///4
#define	SALE_PARAM_OFFSET_COMPARE_MAX_WEIGHT		0x3A	///4
#define 	SALE_PARAM_OFFSET_WEIGHT_PREFIX			0x3E	///1
#define 	SALE_PARAM_OFFSET_COUNT_PREFIX				0x3F	///1
//////////////////////////////////////////////////sale param size = 0x40 (FULL!!!!!)

//#define  DAILY_TOTAL_LIMIT				1000
#define  DAILY_TOTAL_LIMIT				50000
#define  ADD_LIMIT_COUNT				1000

#define	COMPARE_MODE_ON 			1
#define	COMPARE_MODE_OFF 		0

#define SALE_COMPARE_EDITOR_LOW_STATE	0
#define SALE_COMPARE_EDITOR_HIGH_STATE	1
#define SALE_COMPARE_EDITOR_COMPLETE_STATE	2

#define SALE_COMPARE_LOW		0
#define SALE_COMPARE_OK		1
#define SALE_COMPARE_HIGH		2
#define SALE_MIN_COUNT			1

#define SALE_HOLD_TYPE_NORMAL			1
#define SALE_HOLD_TYPE_PEAK			2
#define SALE_HOLD_TYPE_AVERAGE		3

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
extern void SaleModeSetDefaultParam(INT8U type);
extern void SaleModeInit(void);
//extern SCALE_MODE_TYPE SaleModeProc(void);
extern INT8U SaleModeProc(void);
#if defined(USE_WEIGHT_ONLY_ECR) || defined(USE_PDN_ECR)
/** @brief TARE KEY block flag*/
extern INT8U NoUseTarekey;
/** @brief PD-2 ECR Protocol Enable flag*/
extern INT8U Pd2ECRProtocol;
#endif
#ifdef USE_UNDERLOAD_10DIGIT
/** @brief Underload Limit 10digit Enable flag*/
extern INT8U UnderLoad10dEnable;
#endif
extern void SaleModeClearAllData(void);
//kkh0225 code optimaization
#if defined(USE_BT_COMM_API) || defined(USE_TM_COMM_API) || defined(USE_COMMAND_TARE_KEY) || defined(USE_SATO_PRINTER_PROTOCOL)
extern void SaleZeroExtKey(void);
extern void SaleTareExtKey(void);
extern void SaleHoldExtKey(void);
#endif

#ifdef USE_UNIT_TON_NEWTON_DAN_KN
extern void SaleUnitChangeExtKey(void);
#elif defined (USE_UNIT_KG_ONLY)
extern void SaleUnitChangeExtKey(void);
#endif

#ifdef USE_COMMAND_KEY
extern void SaleZeroExtKeyProc(void);
extern void SaleTareExtKeyProc(void);
extern void SaleHoldExtKeyProc(void);
#endif

#ifdef USE_COMMAND_TARE_KEY
void RunSaleWeightTareProc(void);
#endif

//extern void SalePluNameCopyExt(INT8U *pluName);
#if defined(DIALOG6) || defined(USE_SATO_PRINTER_PROTOCOL)
extern void receivedPluDataApply(INT8U *pluName, INT32U tempUnitPrice, INT32U tempTare);
//extern void SalePluDataCopyExt(INT8U *pluName, INT32U tempUnitPrice,INT32U tempTare);
//extern void receivedAllPluDataApply(INT8U *pluName, INT32U tempUnitPrice,INT32U tempTare);
//extern void receivedUnitPriceDataApply(INT32U tempUnitPrice);
//extern void receivedPluNameApply(INT8U *pluName);
//extern void receivedTareValueApply(INT32U tempTare);
#ifdef COUNTRY_CZECH
extern INT32S recentlySendWeight;
#endif
#endif

#ifdef USE_BACKLIGHT_KEY_ONOFF_ALWAYS
extern BOOLEAN blPowerOffFlag;
#endif
#ifdef	USE_DECIMAL_POINT
extern BOOLEAN SaleDecimalPointKeyOnOff; // csh 20120731
extern INT8S SaleNumberPositonFlag; // csh 20120731
#endif
#ifdef USE_TURKEY_POS_PROTOCOL
extern void SaleTareSaveProc(INT8U flag);
#endif
#ifdef USE_COMMAND_KEY
extern INT8U saleKeyCommErrFlagState(void);
#endif
extern INT8U GetSaleInitZeroErrFlagState(void);
#ifdef USE_QUICK_STORE_FUNCTION
extern void saleFuncPluQuickStoreInSale(KEY_TYPE inputkey);
#endif
extern void SetSaleZeroKey(void);
#ifdef USE_SATO_PRINTER_PROTOCOL
extern BOOLEAN GetSaleTareClearFlag(void);
extern void SetScaleTareClearFlag(BOOLEAN flag);
extern void getSaleDefaultName(INT8U* str, INT8U mode);
extern INT32S getSaleWeightPrefix(void);
#endif
/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */


#endif /* _SALE_MODE_APP_H */
