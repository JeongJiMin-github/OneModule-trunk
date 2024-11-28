/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Sale Mode Applicaion Source File\n
* @file    SaleModeApp.c
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

/*
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
/* Insert include files here */

#include "../Common/Globals.h"
#include "../Common/SaleData.h"
#include "../Common/CommonLib.h"
#include "../Common/AdData.h"
#include "../Common/RingBuf.h"
#include "../Common/PluData.h"
#include "../Common/UiDefine.h"
#include "../Communication/PrtApi.h"
#include "../Communication/CommApi.h"
#include "../UI/DispApi.h"
#include "../UI/KeyApi.h"
#include "../UI/EditorApi.h"
#include "../UI/BuzzerApi.h"
#include "../DB/StorageApi.h"
#include "../DB/ParamApi.h"
#include "../DB/PluApi.h"
#include "../Ad/AdApi.h"
#if defined (USE_M0516LDN_MCU) && defined (USE_AUTO_POWER_SETTING)
#include "../Hardware/HardwareConfig.h"
#endif
#include "../Power/PowerApi.h"
#include "../Communication/DebugApi.h"
#ifdef USE_ADD_KEY_FOR_CERTIFICATION
#include "../Communication/CommDrv.h"
#endif
#include "../SystemTimer/TimerApi.h"
#include "ScaleModeApp.h"
#include "SaleModeApp.h"
#include "../Time/TimeApi.h"
#include "../Common/TimeData.h"

#ifdef USE_SALE_PRICE
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief Sale 관련 Data struct */
SALE_DATA SaleData;
/** @brief 임시 PLU data*/
DIRECT_PLU_DATA tempPluData;
/** @brief 임시 Report data*/
PLU_REPORT_DATA tempPluReport;
/** @brief daily total report data*/
PLU_REPORT_DATA dailyTotalReport;
/** @brief Indirect PLU data*/
INDIRECT_PLU_DATA PluInDirectData;
/** @brief Direct PLU data*/
DIRECT_PLU_DATA PluDirectData;

/** @brief C4-2 setting data */
/** @brief bit 7, reserved*/
// bit 7 -> reserved
/** @brief bit 6, PLU tare사용여부, 0: Don't Use 1: Use*/
static BOOLEAN saleUsePluTareFlag;
/** @brief bit 5, PLU name 사용여부, 0: Don't Use 1: Use*/
static BOOLEAN saleUsePluNameFlag;
/** @brief bit 4, Daily total 사용여부, 0: Don't Use 1: Use*/
static BOOLEAN saleUseDailyTotalFlag;
/** @brief bit 3, 단가 삭제 사용여부, 0: Save 1: Clear*/
static BOOLEAN salePriceClearFlag;
/** @brief bit 2, Tare 삭제 사용여부, 0: Save 1: Clear*/
static BOOLEAN scaleTareClearFlag;	//TODO : 접두어 sale 필요??
/** @brief bit 1, Euro모드 사용여부, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseEuroFlag;	//TODO : 접두어 sale 필요??
/** @brief bit 0, 저울 On시에 Euro모드 사용여부, 0: Don't Use 1: Use*/
static BOOLEAN salePowerOnEuroFlag;
#ifdef	USE_DECIMAL_POINT
/** @brief bit 0, Decimal Point 눌림여부, 0; No Pressed  1: Pressed*/
BOOLEAN SaleDecimalPointKeyOnOff; // csh 20120731
/** @brief bit 0, 입력할 숫자위치  */
INT8S SaleNumberPositonFlag; // csh 20120731
#endif

/** @brief C4-3 setting data */
/** @brief bit 7, 가격 dot표시 종류, 0: "."Dot 1: "," Comma*/
static BOOLEAN priceDotTypeFlag;	//TODO : 접두어 sale 필요??
/** @brief bit 6, Preset tare사용여부, 0: Don't Use 1: Use*/
static BOOLEAN saleUsePresetTareFlag;
/** @brief bit 5, reserved*/
//backlight, already use another module
/** @brief bit 4, Head message사용여부, 0: Don't Use 1: Use*/
static BOOLEAN saleUseHeadMsgFlag;
/** @brief bit 3, Gram unit 사용여부, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseGramUnitFlag;	//TODO : 접두어 sale 필요??
/** @brief bit 2, Ounce unit 사용여부, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseOzUnitFlag;	//TODO : 접두어 sale 필요??
/** @brief bit 1, Pound unit 사용여부, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseLbUnitFlag;	//TODO : 접두어 sale 필요??
/** @brief bit 0, Kilo gram unit 사용여부, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseKgUnitFlag;	//TODO : 접두어 sale 필요??

/** @brief C4-4 setting data */
/** @brief bit 7, reserved*/
// bit 7 -> reserved
/** @brief bit 6, reserved*/
// bit 6 -> reserve
/** @brief bit 5, reserved*/
// bit 5 -> reserve
/** @brief bit 4, 00 : Normal 01: 0.5, 10: 00. 10. 20., 11: 25.50.75*/
static INT8U saleRoundOffUnitFlag;
/** @brief bit 3, reserved*/
// bit3 -> reserve
/** @brief bit 2, reserved*/
// bit2 -> reserve
/** @brief bit 1 & bit 0, 0:10, 1:100,  2:1000, 3:10000   E2_read_data +1 이다.*/
static INT8U salePriceForUnit;

/** @brief C4-5 setting data */
/** @brief bit 7, reserved*/
// bit 7 -> reserve
/** @brief bit 6 & bit 5 & bit 4, 단가 Decimal point position, 0: 0000,   1: 0.0, 2: 0.00, 3:0.001 4:0.0001*/
static INT8U priceDecPos;	//TODO : 접두어 sale 필요??
/** @brief bit 3, 0: Don't use 1: Use*/
static BOOLEAN scaleUseCanadaMsgFlag;	//TODO : 접두어 sale 필요??
#ifdef USE_MULTI_PRINT_FUNC			//#ifdef MultiPrint
/** @brief bit 2, 0: Don't use 1: Use*/
static BOOLEAN scaleUseMultiPrintFlag; //복수 인쇄 기능 사용 여부 1: 사용, 0: 미사용, default 미사용
#endif 			//#ifdef MultiPrint
/** @brief bit 1 & bit 0, 사용 프린터 종류, 00 : Don't Use, 01: Use DEP, 10: Use DLP, 11: reserve*/
static INT8U scaleUsePrt;	//TODO : 접두어 sale 필요??

/** @brief 정산 완료 플래그, 0: ready, 1: End*/
static BOOLEAN transactionEndFlag;	//TODO : 접두어 sale 필요??
/** @brief PrePack 호출여부 플래그*/
static BOOLEAN salePrePackFlag;
/** @brief Percent tare 호출여부 플래그, 0: no Call, 1: call Data*/
static BOOLEAN salePercentTareCallFlag;
/** @brief 프린트 가능 여부 플래그, 0: Don't use, 1: Use*/
static BOOLEAN salePrtEnableFlag;
/** @brief Tare clear 플래그, 0: save, 1: clear*/
static BOOLEAN saleTareClearFlag;
/** @brief Date 출력 플래그*/
static BOOLEAN salePrtDayFlag;
/** @brief Line 출력 플래그*/
static BOOLEAN salePrtLineFlag;	
/** @brief Unit price, tare 둘다 세이브 하는 플래그 0: clear, 1: save*/
static BOOLEAN saleBothSaveFlag;
/** @brief Euro 사용여부 플래그, 0: Don't Use 1: Use */
static BOOLEAN saleUseEuroFlag;
/** @brief 무게 Display에서 사용하는 Dot type, 0: Dot 1: Comma*/
static BOOLEAN weightDotTypeFlag;	//TODO : 접두어 sale 필요??
/** @brief Multiple sale 카운트*/
static INT16U saleMultiCount;			    
#ifdef USE_MULTI_PRINT_FUNC
/** @brief Multiple Label Print 카운트*/
static INT16U saleMultiPrintCount;			    
#endif
/** @brief Hold기능 count*/
static INT8U holdCount;	//Haning type INT16U / TODO : 접두어 sale 필요??
/** @brief */
static INT16U holdDisplayFlash;	//TODO : 접두어 sale 필요??
/** @brief Percent tare 값*/
static INT32S salePercentTare;
/** @brief Percent tare 기능에 사용하는 Price*/
static INT32U salePercentPrice;
/** @brief Percent tare 기능에 사용하는 비율(%)*/
static INT32U salePercentRate;
/** @brief 현재 Battery 전압값*/
static INT8U saleBatteryCapacity;
/** @brief Battery 체크 함수 진입을 위한 플래그*/
static BOOLEAN saleBatteryCheckFlag;
/** @brief PLU name 위치*/
static INT8S salePluNamePosition;
/** @brief 현재 사용중인 Price unit string data*/
static INT8U saleUsePriceUnit[PRICE_SYMBOL_SIZE];
/** @brief Euro rate(요율)*/
static INT32S saleEuroRate;
/** @brief Euro decimal position*/
static INT8U saleEuroDecPos;
/** @brief Euro Display decimal position*/
static INT8U saleEuroDispDecPos;
/** @brief Add currency */
static BOOLEAN saleAddCurrency;
/** @brief 판매 이후 받은 돈 액수*/
static INT32S saleReceiveMoney;
/** @brief 판매 이후 거스름 돈*/
static INT32S saleChangeMoney;
/** @brief 1,000원 단위에 comma출력 플래그*/
static BOOLEAN saleUseThousandPtFlag;
/** @brief 무게 단위*/
#ifdef IAR
INT8U code saleWeightUnit[4][3] = { "kg", "lb", "oz", "g "};
#else
const INT8U code saleWeightUnit[4][3] = { "kg", "lb", "oz", "g "};
#endif
/** @brief 가격 단위*/
#ifdef IAR
INT8U code salePriceUnit[2][3] = { "$", "\x80"}; //0-non euro. 1-Euro //see saleUseEuroFlag
#else
const INT8U code salePriceUnit[2][3] = { "$", "\x80"}; //0-non euro. 1-Euro //see saleUseEuroFlag
#endif
/** @brief 반올림 단위*/
const INT8U code saleRoundOffUnit[4] = {0, 5, 10, 25};
//const INT8U code saleRoundOffUnit[4] = {5, 25, 50, 125};

// for CASTON -->
/** @brief Auto Hold & Manual Hold Check flag 0: Auto Hold, 1: Manual Hold*/
//static BOOLEAN AMHoldSelectFlag;	//TODO : 접두어 sale 필요??
/** @brief Auto Hold & Manual Hold Check flag 1:Hold ok*/
//static BOOLEAN AMHoldOkFlag;	//TODO : 접두어 sale 필요??
/** @brief Auto Hold restart flag 1:restart*/
//static BOOLEAN ReHoldFlag;	//TODO : 접두어 sale 필요??
/** @brief 'HDOFF' Message display flag*/
//static BOOLEAN HoldClearMessageFlag;	//TODO : 접두어 sale 필요??
/** @brief 현재 사용하는 Hold type, 0:NORMAL, 1:PEAK, 2 : AVERAGE*/
//static INT8U saleHoldTypeFlag;
/** @brief Hold기능 count*/
//INT16U holdDispCount;	//TODO : 접두어 sale 필요??	//not use
/** @brief SUM기능 count*/
INT16U sumCount;	//TODO : 접두어 sale 필요??
/** @brief Sum기능 사용 무게, 평균값*/
INT32S sumWeight;	//TODO : 접두어 sale 필요??
/** @brief Sum기능 사용 무게, 평균값*/
INT32S sumSaveWeight;	//TODO : 접두어 sale 필요??
/** @brief */
//INT16U sumDisplayFlash;	//TODO : 접두어 sale 필요??
//<-- for CASTON

/** @brief 설정시간 동안 state delay하는 다음 state*/
static INT8U saleNextState;
/** @brief 일정시간 이후에 기존 data지우고 다시 시작 하는 flag*/
static BOOLEAN saleUnitPriceRenewalFlag = ON;
/** @brief sum값 prt하기 위한 flag*/
static BOOLEAN saleEnableSumPrt;
/** @brief 최초 add시에만 start정보 prt하기 위한 flag*/
static BOOLEAN saleFirstAdd;
/** @brief half키나 quarter키 사용가능 flag*/
static BOOLEAN saleHalfQuartEnable;
/** @brief 100g키 사용 가능 flag*/
static BOOLEAN saleHundredGramEnable;
/** @brief 100g기능 설정 Flag*/
static BOOLEAN saleHundredGramPrice;
/** @brief 100g키 사용 가능 flag*/
static INT8U saleDefaultPluName[PLU_NAME_SIZE];

#ifdef USE_ADD_KEY_FOR_CERTIFICATION
static INT8U saleWaitPrinterResponseCount = 0;
#endif

/** @brief Tare key is pressed flag*/
static BOOLEAN saleTareKeyPressed;
/** @brief Zero key is pressed flag*/
static BOOLEAN saleZeroKeyPressed;

/** @brief Last digit invalid function enable  flag*/
static BOOLEAN saleLastDigitInvalidEnable;

/** @brief Transaction Unitprice check flag*/
static BOOLEAN saleTransactionWeightCheck;

/** @brief Auto hold enable flag*/
static BOOLEAN saleAutoHoldEnable;

/** @brief Auto hold excute enable flag*/
static BOOLEAN saleAutoHoldExcute;
#ifdef USE_QUICK_STORE_FUNCTION
/** @brief PLU DIRECT SAVE enable flag*/
static BOOLEAN salePluDirectSaveFlag;
#endif

#ifdef USE_SEND_SAME_WEIGHT_ONCE
/** @brief Check same weight flag as previous */
INT8U checkSamePreviousWeight = UNKNOWN_WEIGHT;
INT32S previousWeight;
#endif

static INT8U saleTareType;
static INT8U saleDateType;
#ifdef USE_TURKEY_POS_PROTOCOL
static BOOLEAN keycommSetErrFlag;
#endif

/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
#define KEY_TARE_CALL 		0
#define PLU_TARE_CALL 		1
#define WEIGHT_TARE_CALL 	2

/*
********************************************************************************
*                       LOCAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert local typedef & structure here */

/** 
* @brief Key에 따른 Sale Function struct
* @remark minNum <= Key <= maxNum 이면 INT8U func(KEY_TYPE key) 실행
*/
typedef struct 
{ 
	/** @brief sale 함수 최소 번호*/
	KEY_TYPE minNum;	
	/** @brief sale 함수 최대 번호*/
	KEY_TYPE maxNum;
	/** @brief sale 함수*/
	INT8U (*func) (KEY_TYPE key); 
} SALE_KEY_FUNC;

/** 
* @brief Key에 따른 Sale Function struct (void 함수)
* @remark keyNum == Key 이면 void func(void) 실행
*/
typedef struct 
{ 
	/** @brief Void function key number*/
	KEY_TYPE keyNum;
	/** @brief void function*/
	void (*func) (void); 
} SALE_KEY_VOID_FUNC;

/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/
/* Insert file scope variable & tables here */


/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
static INT8U saleNumKey(KEY_TYPE key);
static INT8U saleDirectPluCall(KEY_TYPE key);

static void saleSetIndicator(void);
static void saleDisplayWeight(INT32S dispWeight);
static void saleDisplayFunc(void);
static void saleWeightFunc(void);
static void saleZeroKeyProc(void);
static void saleZeroKey(void);

static void saleWeightTareProc(void);
static void saleTareKey(void);
static void saleHoldKey(void);
static void saleClearKey(void);
static void saleGrossKey(void);

static void saleEuroKey(void);
static void salePluCallKey(void);
static void salePluSaveKey(void);
static void saleBattCheckKey(void);
static void salePayKey(void);
static void saleSumKey(void);
static void saleDoubleZeroKey(void);
#ifdef	USE_DECIMAL_POINT
static void saleDecimalPointKey(void); // csh 20120731
#endif
#ifdef USE_ADD_KEY_FOR_CERTIFICATION
static void saleSumCertiKey(void);
static void saleAddCertiKey(void);
#endif
static void saleAddKey(void);
static void saleModeKey(void);
static void saleCancelKey(void);
static void saleMultiKey(void);
static void salePrePackKey(void);
static void salePercentTareSetKey(void);
static void salePercentTareCallKey(void);
static void saleBackLightKey(void);
static void saleTareSaveKey(void);
static void saleBothSaveKey(void);
static void salePrintEnableKey(void);
static void saleUnitChangeKey(void);
static void saleHalfKey(void);
static void saleQuarterKey(void);
static void saleHundredGramKey(void);
static void saleDispTestKey(void);

static void 	saleDispNumWithDecPoint(INT8U winNum, INT8U position, INT8U type, INT32S num, BOOLEAN thousandPt);
static INT8U 	saleFuncPluDataReadInSale(INT16U num);
static void 	saleFuncPluDataWriteInSale(INT16U inputPluNum);
static void 	saleFuncDailyTotalSave(BOOLEAN sign, INT16U count, INT32U price);
#ifdef DIALOG6
static void receivedAllPluDataApply(INT8U *pluName, INT32U tempUnitPrice, INT32U tempTare);
static void receivedUnitPriceDataApply(INT32U tempUnitPrice);
static void receivedPluNameApply(INT8U *pluName);
static void receivedTareValueApply(INT32U tempTare);
#endif
static INT32U 	saleFuncEuro2Dollar(INT32U inputPrice);
static INT32U 	saleFuncDollar2Euro(INT32U inPutPrice);

static void   	saleFuncReadPercentTareData(void);
static void  	saleFuncClearUnitPrice(void);
static void 	saleFuncClearPluName(void);
static void 	saleFuncPrintCommand(INT8U	saleType);
#ifdef USE_PRINTER
static void 	salePrtWeightItem(void);
static void 	salePrtFixedItem(void);
static void 	salePrtMultiItem(void);
static void 	salePrtDiscountItem(void);
static void 	salePrtWeightItemCancel(void);
static void 	salePrtFixedItemCancel(void);
static void 	salePrtStartCondition(void);
static void     salePrtPluName(void);
static void 	salePrtSum(void);
static void     salePrtPayment(void);
static INT32U	saleCalculatorTotalPrice(INT32S	weightValue);
static void salePrtDlpStartCondition(INT8U printType);
static void salePrtDlpPluName(void);
static void salePrtDlp(INT8U winNum);
#if defined(USE_STREAM_CAS_22BYTE) && defined(USE_EB_STREAM)
static void salePrtCas22Byte(void);
#endif
#endif
#ifdef USE_QUICK_STORE_FUNCTION
static void salePluDirectSaveOn(void);
static void salePluDirectSaveOff(void);
#endif
#ifdef USE_SATO_PRINTER_PROTOCOL
BOOLEAN GetSaleTareClearFlag(void);
BOOLEAN GetSaleTransactionWeightCheck(void);
void SetScaleTareClearFlag(BOOLEAN flag);
void SetSaleTransactionWeightCheck(BOOLEAN flag);
#endif

void SetSaleZeroKey(void);
#ifdef USE_COMMAND_TARE_KEY
void RunSaleWeightTareProc(void);
#endif


/** @brief Key에 따른 Sale Function(key input type) 개수 */
#define SALE_KEY_MAX_FUNC_NUM	3
#define SALE_COUNT_ENABLE_CAPA	1200
#define SALE_COUNT_LOW_CAPA		1
#define SALE_PERCENT_LOW_CAPA		1800

#if defined(USE_STREAM_CAS_22BYTE) && defined(USE_EB_STREAM)
/** @brief hold average time */
#define HOLD_AVERAGE_TIME      10	// hold 잡는 시간 (10 = 1sec) duckspg hold test
INT8U flowPrtFlag;		//Stream Mode Enable flag (DP Model)
#endif

/** 
* @brief Key에 따른 Sale Function struct
* @remark minNum <= Key <= maxNum 이면 INT8U func(KEY_TYPE key) 실행\n
*         key 입력 받는 함수
*         함수 추가시 SALE_KEY_MAX_FUNC_NUM증가 시키시오.
*/
const SALE_KEY_FUNC code saleKeyFuncTable[SALE_KEY_MAX_FUNC_NUM] = 
{
	{KEY_NUM_0, 	KEY_NUM_9, 	saleNumKey},
	{KEY_NUM_A, KEY_NUM_F, 	saleNumKey},
//	{KEY_PLU_1, 	KEY_PLU_28, 	saleDirectPluCall},
	{KEY_PLU_1, 	KEY_PLU_29, 	saleDirectPluCall},
};

//
/** @brief Key에 따른 Sale Function(void type) 개수 */
#ifdef USE_DECIMAL_POINT
/** @brief Quick Store Key에 따른 Sale Function(void type) 개수 */
#define USE_DECIMAL_POINT_FUNC_NUM	1
#else
#define USE_DECIMAL_POINT_FUNC_NUM	0
#endif

#ifdef USE_QUICK_STORE_FUNCTION
/** @brief Quick Store Key에 따른 Sale Function(void type) 개수 */
#define PLU_QUICK_STORE_KEY_FUNC_NUM	1
#else
#define PLU_QUICK_STORE_KEY_FUNC_NUM	0
#endif

#define SALE_KEY_MAX_VOID_FUNC_NUM	(28+USE_DECIMAL_POINT_FUNC_NUM+PLU_QUICK_STORE_KEY_FUNC_NUM)

/**
* @brief Key에 따른 Sale Function struct (void 함수)
* @remark keyNum == Key 이면 void func(void) 실행\n
*         key 입력 받지 않는 함수
*         함수 추가시 SALE_KEY_MAX_VOID_FUNC_NUM증가 시키시오.
*/
const SALE_KEY_VOID_FUNC code saleKeyVoidFuncTable[SALE_KEY_MAX_VOID_FUNC_NUM] = 
{
	{KEY_ZERO, 				saleZeroKey},
	{KEY_TARE, 				saleTareKey},
	{KEY_HOLD, 				saleHoldKey},
	{KEY_C, 					saleClearKey},
	{KEY_EURO, 				saleEuroKey},	
	{KEY_PLU_CALL,  			salePluCallKey},
	{KEY_BAT_CHECK, 		saleBattCheckKey},
	{KEY_PLU_SAVE,			salePluSaveKey},
	{KEY_PAY,				salePayKey},
	{KEY_NUM_00,			saleDoubleZeroKey},
#ifdef	USE_DECIMAL_POINT
	{KEY_POINT,				saleDecimalPointKey}, // csh 20120731
#endif
#ifdef USE_QUICK_STORE_FUNCTION
	{KEY_PLU_DIRECT_SAVE,	salePluDirectSaveOn},
#endif
#ifdef USE_ADD_KEY_FOR_CERTIFICATION
	{KEY_ADD,				saleAddCertiKey},
	{KEY_SUM,				saleSumCertiKey},
#else
	{KEY_ADD,				saleAddKey},
	{KEY_SUM,				saleSumKey},
#endif
	{KEY_MODE,				saleModeKey},
	{KEY_CANCEL,			saleCancelKey},
	{KEY_X,					saleMultiKey},
	{KEY_PREPACK,			salePrePackKey},
	{KEY_PERCENT_TARE_SET, 	salePercentTareSetKey},
	{KEY_PERCENT_TARE, 		salePercentTareCallKey},
	{KEY_BACK_LIGHT, 		saleBackLightKey},
	{KEY_TARE_SAVE,	 		saleTareSaveKey},
	{KEY_BOTH_SAVE, 		saleBothSaveKey},
	{KEY_PRINT_ENABLE, 		salePrintEnableKey},
	{KEY_UNIT_CHANGE,		saleUnitChangeKey},
	{KEY_HALF,				saleHalfKey},
	{KEY_QUARTER,			saleQuarterKey},
	{KEY_HUNDREDGRAM,		saleHundredGramKey},
	{KEY_DISPLAY_TEST,		saleDispTestKey},
	{KEY_GROSS,				saleGrossKey}

};
/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/


/* Insert global functions here */
/**
********************************************************************************
* @brief    Sale Mode Application Parameter를 default 값으로 초기화\n
* @param    type : defualt type
* @return   none
* @remark   Parameter 영역에 초기값을 넣는다.\n
*           Cal Mode나 Program 모드에서 전체 초기화할 때 실행한다.
********************************************************************************
*/
void SaleModeSetDefaultParam(INT8U type)
{
	SaleModeClearAllData();		// Sale Data default 실행 

	switch (type)
	{
		case NTEP:
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST, 0xC8);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0x73);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_3RD, 0x23);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, 0x24);
			ParamWriteData(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PRICE_SYMBOL, "$", PRICE_SYMBOL_SIZE); 	// first unit				
			break;

		case KOREA:
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST, 0x00);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0xF1);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_3RD, 0x26);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, 0x70);				
			ParamWriteData(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PRICE_SYMBOL, "\\", PRICE_SYMBOL_SIZE); 	// first unit
			break;

#if (MODEL_NUM == MODEL_OMV4_ER2_LCD) && defined(COUNTRY_TURKEY)	//OMV4 ER-JR Turkey
		case TURKEY:
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, 0x20);	// Turkey: "TRSTR"로 통신 설정
			break;
#endif

		default: 		// OIML
#ifdef USE_TURKEY_POS_PROTOCOL
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST, 0x3C);
#else
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST, 0x38);
#endif
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0x31);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_3RD, 0x03);				
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, 0x25);
			ParamWriteData(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PRICE_SYMBOL, "$", PRICE_SYMBOL_SIZE); 	// first unit

			AdWriteParam(TARE_SUCCESSIVE, TARE_TYPE_SUCCESSIVE_PLUS);
			AdWriteParam(ZERO_MARK_INDICATOR, ZERO_MARK_TYPE_ALL);			
			break;
	}
	ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SALE_STATE_NORMAL);
}

/**
********************************************************************************
* @brief    sale에 관련된 모든 정보를 초기화 하는 함수       
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void SaleModeClearAllData(void)
{
	INT8U i;
	
	/** @brief PLU Data & Report Data Clear*/
	PluClearAllPlu();

	/** @brief Head message Data Clear*/
	for(i = 0; i < 24; i++)
	{
		ParamWriteByte(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE1 + i, 0x20);
		ParamWriteByte(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE2 + i, 0x20);		
	}
	for(i = 0; i < 16; i++)
	{
		ParamWriteByte(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE3 + i, 0x20);	
	}

	/** @brief PercentTare Data Clear*/
	ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PERCENT_TARE, 0);
	ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PERCENT_PRICE, 0);
	ParamWriteByte(PARAM_SALE_AREA_NUM,  SALE_PARAM_OFFSET_PERCENT_RATE, 0);

	/** @brief EuroData Clear*/
	ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_EURO_DEC, 2);			
	ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_EURO_RATE, 100);
}

/**
********************************************************************************
* @brief    Sale Mode Application Init 함수\n
*           Sale Mode State Init
* @param    none
* @return   none
* @remark   초기에 한번 실행 되는 함수\n
********************************************************************************
*/
void SaleModeInit(void)
{
	INT8U readSetDataInSale = 0;
	INT8U tempInt8u;
	INT8U readSetPrintMode;

#ifdef	USE_DECIMAL_POINT
	SaleDecimalPointKeyOnOff = OFF; // csh 20120731
	SaleNumberPositonFlag = 0; // csh 20120731
#endif	
	SaleData.weightUnit = AdData.weightUnit;
	SaleData.weightDecPos = AdData.weightDecPos;	

//  C4-2
	readSetDataInSale = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST); 
//	salePluTypeFlag = (readSetDataInSale & 0x80) >> 7;	--> reserve
	saleUsePluTareFlag = (readSetDataInSale & 0x40) >> 6;		
	saleUsePluNameFlag = (readSetDataInSale & 0x20) >> 5;
	saleUseDailyTotalFlag = (readSetDataInSale & 0x10) >> 4;
	salePriceClearFlag = (readSetDataInSale & 0x08) >> 3;
	scaleTareClearFlag = (readSetDataInSale & 0x04) >> 2;
	scaleUseEuroFlag = (readSetDataInSale & 0x02) >> 1;
	salePowerOnEuroFlag = (readSetDataInSale & 0x01) >> 0;
	
// 	C4-3
	readSetDataInSale = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND);

	priceDotTypeFlag = (readSetDataInSale & 0x80) >> 7;
	weightDotTypeFlag = priceDotTypeFlag;				
	saleUsePresetTareFlag = (readSetDataInSale & 0x40) >> 6;
//	ScaleParam.scaleUseBL = (readParamDataInScale & 0x20) >> 5;			<-- already initial
	saleUseHeadMsgFlag  = (readSetDataInSale & 0x10) >> 4;			// reserve
	scaleUseGramUnitFlag = (readSetDataInSale & 0x08) >> 3;
	scaleUseOzUnitFlag = (readSetDataInSale & 0x04) >> 2;
	scaleUseLbUnitFlag = (readSetDataInSale & 0x02) >> 1;
	scaleUseKgUnitFlag = (readSetDataInSale & 0x01) >> 0;
	
	tempInt8u = (INT8U)scaleUseGramUnitFlag + (INT8U)scaleUseOzUnitFlag + (INT8U)scaleUseLbUnitFlag + (INT8U)scaleUseKgUnitFlag;

	if (tempInt8u == 0)
	{
		scaleUseKgUnitFlag = 1;
		tempInt8u = 1;
	}
	
	if (tempInt8u == 1)
	{
		SaleData.saleState = SALE_STATE_NORMAL;//for setting using unit
		saleUnitChangeKey();
	}


//	C4-4
	readSetDataInSale = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_3RD);
//	saleUseHeadMsgFlag = (readSetDataInSale & 0x80) >> 7;		// reserve		
//	temp = (readSetDataInSale & 0x40) >> 6;		// reserve
	saleRoundOffUnitFlag = (readSetDataInSale & 0x30) >> 4;
	saleLastDigitInvalidEnable = (readSetDataInSale & 0x08) >> 3;		// reserve		
	saleHundredGramPrice = (readSetDataInSale & 0x04) >> 2;		//100g단가 사용 Flag세팅
	salePriceForUnit = (readSetDataInSale & 0x03);

// 	C4-5
   	readSetDataInSale = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH);
#if defined(USE_STREAM_CAS_22BYTE) && defined(USE_EB_STREAM)
	if(AdCheckInitZero())
	{
		readSetPrintMode = readSetDataInSale & 0x07;
  #if (MODEL_NUM == MODEL_OMV4_ER2_LCD) && defined(COUNTRY_TURKEY)
  		if(readSetPrintMode == 0x02)	//cas22byte
			flowPrtFlag = 1;
  #else
		if(readSetPrintMode == 0x04 || readSetPrintMode == 0x03)
			flowPrtFlag = (readSetPrintMode & 0x04)>>2;
  #endif
	}
	else
		flowPrtFlag = 0;
#endif
//	Use Standby Time Menu
	priceDecPos	= (readSetDataInSale & 0x70) >> 4;		// reserve		
	if (priceDecPos >= 7)
	{
		saleUseThousandPtFlag = 1;
		priceDotTypeFlag = 1;//comma
		priceDecPos = 0;
	}

	SaleData.priceDecPos = priceDecPos;	
	if (weightDotTypeFlag == DEC_TYPE_DOT)
	{
		SaleData.weightDotChar = '.';
		SaleData.priceDotChar = '.';
	} 
	else //weightDotTypeFlag == DEC_TYPE_COMMA
	{
		SaleData.weightDotChar = ',';
		SaleData.priceDotChar = ',';
	}


	scaleUseCanadaMsgFlag = (readSetDataInSale & 0x08) >> 3;		// reserve
#ifdef USE_MULTI_PRINT_FUNC			//#ifdef MultiPrint
	scaleUseMultiPrintFlag = (readSetDataInSale & 0x04)>>2; 	//read C45-3rd bit(bit2)//wcm20111103
#endif			//#ifdef MultiPrint

#if defined (USE_EB_STREAM) || defined (USE_PRII_SPAIN_ST_PROTOCOL)
	/* ER-JR 터키 펌웨어에서 프린트 세팅을 none으로 세팅할 시 
	   C4-5 설정에서 Multiple Print bit 영역을 침범하여 0x5로 세팅하므로 주의할 것 
	   관련 revision 2303 */
	scaleUsePrt = (readSetDataInSale & 0x07);		// reserve	
#else
	scaleUsePrt = (readSetDataInSale & 0x03);		// reserve	
#endif

#if (MODEL_NUM == MODEL_OMV4_ER2_LCD) && defined(COUNTRY_TURKEY)
  #ifdef USE_CONTI_SEND_WEIGHT_DATA
	if (scaleUsePrt == PRT_TRSRT)
		sendWeightFlag = 1;
	else 
		sendWeightFlag = 0;
  #endif
  #ifdef USE_ECR_TYPE_7
	if (scaleUsePrt == PRT_TYPE7)
		sendType7Flag  = 1;
	else
		sendType7Flag  = 0;
  #endif
#endif

	SaleData.saleState = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS);

	if (salePowerOnEuroFlag)		
	{
		MakeStrCopy(saleUsePriceUnit, "EUR", 3);

	}
	else
	{
		ParamReadData(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PRICE_SYMBOL, saleUsePriceUnit, PRICE_SYMBOL_SIZE);		// first unit
	}
	saleUsePriceUnit[3] = 0x00;	
#if defined (USE_REAL_STREAM) || defined (USE_PRII_SPAIN_CO_PROTOCOL)
	if (scaleUsePrt == PRT_NONE)
#else
	if (scaleUsePrt == PRT_NONE || scaleUsePrt == PRT_RESERVED) //PRT_RESERVED -> PRT_STREAM 으로 변경됨.
#endif
	{
		salePrtEnableFlag = 0;
	}
	else
	{
		salePrtEnableFlag = 1;
	}

	salePrtDayFlag = 1;
	salePrtLineFlag = 0;
	saleUseEuroFlag = 0;
			
	ReportReadData(REPORT_ALL_NUM, &dailyTotalReport);

	if (dailyTotalReport.count > DAILY_TOTAL_LIMIT)
	{
		dailyTotalReport.count = 0;
		dailyTotalReport.price = 0;
	}			
	saleEuroDispDecPos = 2;
	saleEuroDecPos = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_EURO_DEC);
	saleEuroRate = ParamReadDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_EURO_RATE);
	if (saleEuroDecPos > 6)
	{
		saleEuroDecPos = 1;
		saleEuroRate = 10;
	}	 		
	
	transactionEndFlag = FALSE;

	saleTareClearFlag = scaleTareClearFlag;
	saleBothSaveFlag = OFF;
	salePrePackFlag = OFF;

	salePercentTareCallFlag = OFF;
#if defined(DIALOG6) || defined(USE_SMART_BT_PROTOCOL)
	SaleData.keyTare = 0;
#endif
	SaleData.weight = 0;
	SaleData.unitPrice = 0;
	SaleData.pluNum = 0;
	SaleData.totalPrice = 0;

	SaleData.holdWeight = 0;
	saleMultiCount = 0;
#ifdef	USE_MULTI_PRINT_FUNC
	saleMultiPrintCount = 0;
#endif
	holdCount = 0;

	holdDisplayFlash = 0;
	
	sumWeight = 0;
	sumCount = 0;
	sumSaveWeight=0;

	saleReceiveMoney = 0L;
	saleChangeMoney = 0L;

	SaleData.addCount = 0;
	SaleData.addPrice = 0L;


	saleBatteryCapacity = 0;
	saleBatteryCheckFlag = 0;

	salePluNamePosition = 0;

	salePercentTare = 0;
    	salePercentPrice = 0;
	salePercentRate = 0;

	saleEnableSumPrt = 0;
	saleFirstAdd = 1;
	saleHalfQuartEnable = 1;
	saleHundredGramEnable = 0;
	if (salePowerOnEuroFlag)
	{	
		saleUseEuroFlag = salePowerOnEuroFlag;	
		MakeStrCopy(saleUsePriceUnit, "EUR", 3);


		DispSetConstStrMsg(WIN_WEIGHT, " EURO ");
		DispSetDecPoint(WIN_TOTAL_PRICE, saleEuroDecPos, 0, 0);
		DispSetNumMsg(WIN_TOTAL_PRICE, saleEuroRate, 0);
		DispSetRunMsgPage(10, 2);		
	}
	if (scaleUseCanadaMsgFlag)
	{
		if (SaleData.weightUnit == 0)
		{
			DispSetConstStrMsg(WIN_WEIGHT, "METRIC");			
			DispSetRunMsgPage(10, 2);			
		}
		else if (SaleData.weightUnit == 1)		
		{
			DispSetConstStrMsg(WIN_WEIGHT, " POUND");
			DispSetRunMsgPage(10, 2);			
		}
	}
	saleFuncClearPluName();
	tempPluData.tare = 0;
	tempPluData.unitPrice = 0;

	saleTareKeyPressed = 0;
	saleZeroKeyPressed = 0;
	saleAddCurrency = 0;

	saleTransactionWeightCheck = FALSE;
	saleAutoHoldEnable = FALSE;
	saleAutoHoldExcute = TRUE;

	saleDateType = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_DATE_TYPE);	//date type
#ifdef USE_ADD_KEY_FOR_CERTIFICATION
	SaleData.addState = 0;
	SaleData.printerState = 0;
#endif
#ifdef USE_AD_READ_COMPLETE_CHECK
	AdDataReadCompleteFlag = FALSE;
#endif
#ifdef USE_QUICK_STORE_FUNCTION
	salePluDirectSaveOff();
#endif
}

/**
********************************************************************************
* @brief    설정 시간동안 state delay 함 (key 입력 불가능)
* @param    onTime100ms : Delay 시간 (단위 : 100ms)\n
*           state : 설정 시간뒤에 실행할 state
* @return   none
* @remark   
********************************************************************************
*/
void SaleSetStateDelayTimer(INT8U onTime100ms, INT8U state)
{
	if (onTime100ms == 0)
	{
		return;
	}

	KeyEnable(OFF);//key 동작 방지

	TimerSetDelay100ms(TIMER_DELAY_100MS_SALE_STATE, onTime100ms);
	saleNextState = state;
}

/**
********************************************************************************
* @brief    unit price 입력 시 일정 시간이 지나면 기존 data는 지우고 다시 시작 
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void SaleSetUnitPriceRenewalTimer(void)
{
	TimerSetDelay100ms(TIMER_DELAY_100MS_UNIT_PRICE_RENEWAL, 70);
	saleUnitPriceRenewalFlag = OFF;
}

/**
********************************************************************************
* @brief    Sale Mode를 돌면서 timer를 체크하는 함수
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void SaleCheckTimer(void)
{
	if (TimerCheckDelay100ms(TIMER_DELAY_100MS_SALE_STATE) == 1)
	{
		TimerSetDelay100ms(TIMER_DELAY_100MS_SALE_STATE, 0);
	 	KeyEnable(ON);			//key 동작 허용
	 	SaleData.saleState = saleNextState;
	}
	if (TimerCheckDelay100ms(TIMER_DELAY_100MS_UNIT_PRICE_RENEWAL) == 1)
	{
 		saleUnitPriceRenewalFlag = ON;
	}
}

/**
********************************************************************************
* @brief    Sale Mode Application Procedure 함수\n
* @param    none
* @return   SCALE_MODE_SALE, SCALE_MODE_PGM, SCALE_MODE_CAL
* @remark   key값을 2가지 func table에서 찾아서 맞는 func을 실행\n
*           AD 변화 시 실행\n
*           State, Status 변화에 따라 Display 함수 실행\n
*           일정 시간에 한번 계속 실행되는 함수\n
********************************************************************************
*/
SCALE_MODE_TYPE SaleModeProc(void)
{
	SCALE_MODE_TYPE retValue;
	INT8U funcNum;
		
	KEY_TYPE Key;
	KEY_TYPE minKeyNum;
	KEY_TYPE maxKeyNum;

	SaleData.weight = AdData.weightData;
	SaleData.weightUnit = AdData.weightUnit;
	SaleData.weightDecPos = AdData.weightDecPos;	
	if(saleLastDigitInvalidEnable) SaleData.lastDigitIsInvalid = AdData.lastDigitIsInvalid;
	else SaleData.lastDigitIsInvalid = FALSE;

	retValue = SCALE_MODE_SALE;

#ifdef USE_SEND_SAME_WEIGHT_ONCE
	if (AdExtToIntValueExtern(AbsInt32s(previousWeight - AdData.weightData)) >= AdData.d1 * 5)	// 5눈금 이상인 경우
	{
		checkSamePreviousWeight = DIFFERENCE_WEIGHT;
		previousWeight = SaleData.weight;
	}
#endif

	if (SaleData.unitPrice == 0)
	{
		saleHalfQuartEnable = 1;
	}
#ifdef USE_ADD_KEY_FOR_CERTIFICATION
	switch (SaleData.addState)
	{
		case 1:
		case 3:
			PrtPutChar(ESC);
			PrtPutChar('v');
			SaleData.addState++;
			break;
			
		case 2:
		case 4:
			if (SaleData.printerState == 1)
			{
				saleWaitPrinterResponseCount++;
				if(saleWaitPrinterResponseCount == 20)
				{
					BuzzerSetOnTimes(2);
					saleWaitPrinterResponseCount = 0;
					SaleData.addState = 0;
					break;
				}
			}
			else if(SaleData.printerState == 2)
			{
				saleAddKey();
				SaleData.addState = 0;
				SaleData.printerState = 0;
				saleWaitPrinterResponseCount = 0;
			}
			else if(SaleData.printerState == 3)
			{
				BuzzerSetOnTimes(2);
				SaleData.addState = 0;
				SaleData.printerState = 0;
				saleWaitPrinterResponseCount = 0;
			}			
			else if(SaleData.printerState == 4)
			{
				saleSumKey();
				SaleData.addState = 0;
				SaleData.printerState = 0;
				saleWaitPrinterResponseCount = 0;
			}
			break;
			
		default:
			break;
	}
#endif

	if (KeyCheck())
	{
		if (SaleData.saleState == SALE_STATE_PLU_NAME)
		{
			Key = KeyGetFuncCode(KEY_MODE_HEX);
		}
		else
		{
			Key = KeyGetFuncCode(KEY_MODE_NORMAL);
#if defined (USE_M0516LDN_MCU) && defined (BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_VOLT)
            if (Key == KEY_BACK_LIGHT) LowDryBattBLoffFlag = 1;
#endif
			if(SaleData.saleState == SALE_STATE_HOLD) Key = KEY_NOT_PRESSED;
		}
	}
	else
	{
		Key = KEY_NOT_PRESSED;
	}

	SaleCheckTimer();

	for (funcNum = 0; funcNum < SALE_KEY_MAX_FUNC_NUM; funcNum++)
	{
		minKeyNum = saleKeyFuncTable[funcNum].minNum;
		maxKeyNum = saleKeyFuncTable[funcNum].maxNum;
		if ((Key >= minKeyNum) && (Key <= maxKeyNum))
		{
#ifdef USE_QUICK_STORE_FUNCTION
			if(salePluDirectSaveFlag && funcNum == 2)
			{
				saleFuncPluQuickStoreInSale(Key);
				salePluDirectSaveOff();
				break;	
			}
#endif
			retValue = saleKeyFuncTable[funcNum].func(Key);
			break;
		}
	}

	for (funcNum = 0; funcNum < SALE_KEY_MAX_VOID_FUNC_NUM; funcNum++)
	{
		minKeyNum = saleKeyVoidFuncTable[funcNum].keyNum;
		if (Key == minKeyNum)
		{
			saleKeyVoidFuncTable[funcNum].func();
			break;
		}
	}	

	saleBatteryCapacity = PowerGetUseablePercent();
	
	saleSetIndicator();

	saleWeightFunc();
	
	saleDisplayFunc();
#if defined (USE_REAL_STREAM) || defined (USE_PRII_SPAIN_CO_PROTOCOL)
 #if defined (USE_NOT_UNDERLOAD_ERROR)
 	if (scaleUsePrt == PRT_STREAM) 
 #elif defined (USE_PRII_SPAIN_CO_PROTOCOL)
 	if (scaleUsePrt == PRT_RS_CO) 
 #else
	if (scaleUsePrt == PRT_STREAM && !AdData.underloadFlag) 
 #endif
	{
		if (SaleData.saleState == SALE_STATE_NORMAL)
		{
			salePrtWeightItem();
		}
	}
#endif

#ifdef USE_PRII_SPAIN_ST_PROTOCOL
	if (scaleUsePrt == PRT_RS_ST) {
		salePrtWeightItem();
	}
#endif
	if (AdData.stableFlag)
	{
		if (saleTareKeyPressed)
		{
			saleWeightTareProc();
			saleTareKeyPressed = 0;
			TimerSetDelay100ms(TIMER_DELAY_100MS_WAIT_STABLE, 0);//stop timer
		}
		if (saleZeroKeyPressed)
		{
			saleZeroKeyProc();
			saleZeroKeyPressed = 0;
			TimerSetDelay100ms(TIMER_DELAY_100MS_WAIT_STABLE, 0);//stop timer
		}
	}	
	else
	{
#ifdef USE_CTS_MODE
		TimerSetDelay100ms(TIMER_DELAY_100MS_CTS_LAST_LOAD_AD,100);
#endif
		if (TimerCheckDelay100ms(TIMER_DELAY_100MS_WAIT_STABLE))
		{
			BuzzerSetOnTimes(3);
			saleTareKeyPressed = 0;
			saleZeroKeyPressed = 0;
			TimerSetDelay100ms(TIMER_DELAY_100MS_WAIT_STABLE, 0);//stop timer
		}
	}
#if defined(USE_STREAM_CAS_22BYTE) && defined(USE_EB_STREAM)
	if(flowPrtFlag == 1)				   //Stream Mode(CAS22BYTE) Enable flag
	{
		salePrtCas22Byte();
	}
#endif

	return retValue;
}
//kkh0225 code optimaization
#if defined(USE_BT_COMM_API) || defined(USE_TM_COMM_API) || defined(USE_COMMAND_TARE_KEY)
/**
********************************************************************************
* @brief    Sale Mode를 돌면서 timer를 체크하는 함수
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void SaleZeroExtKey(void)
{
	saleZeroKey();
}

/**
********************************************************************************
* @brief    Sale Mode를 돌면서 timer를 체크하는 함수
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void SaleTareExtKey(void)
{
	saleTareKey();
}

/**
********************************************************************************
* @brief    Sale Mode를 돌면서 timer를 체크하는 함수
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void SaleHoldExtKey(void)
{
	saleHoldKey();
}
#endif

  #ifdef USE_COMMAND_TARE_KEY
/**
********************************************************************************
* @brief   외부에서 SaleWeightTareProc()에 접근 하는 함수
* @param  none
* @return   none
* @remark   
********************************************************************************
*/
void RunSaleWeightTareProc(void)
{
	saleWeightTareProc();
}
  #endif

#ifdef USE_TURKEY_POS_PROTOCOL
/**
********************************************************************************
* @brief    COMMAND에 따라 키동작을 수행하는 함수.
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void saleKeyCommErrFlagSet(BOOLEAN flag)
{
	if(flag == ON)
		keycommSetErrFlag = 1;
	else
		keycommSetErrFlag = 0;
}

INT8U saleKeyCommErrFlagState(void)
{
	return keycommSetErrFlag;
}

void SaleTareExtKeyProc(void)
{
	saleWeightTareProc();
	BuzzerSetOnTimes(1);
}
#endif

/**
********************************************************************************
* @brief    ER PLUS ECR에서 PLU 이름을 받아오는 함수
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
/*
void SalePluNameCopyExt(INT8U *pluName)
{
}
*/

/**
********************************************************************************
* @brief	ER JR ECR에서 PLU data를	받아오는 함수
* @param	none
* @return	none
* @remark	
********************************************************************************
*/
#if defined(DIALOG6) || defined(USE_SATO_PRINTER_PROTOCOL)
void receivedPluDataApply(INT8U *pluName, INT32U tempUnitPrice, INT32U tempTare)
{
	INT8U i, tmpNum;

	if(pluName != 0) saleFuncClearPluName();											//PLU NAME

	if(SaleData.pluNum)
	{
		if(pluName != 0) MakeStrCopy(tempPluData.name, pluName, 6);
		if(tempUnitPrice != 0xFFFFFFFF) tempPluData.unitPrice = tempUnitPrice;
		else tempPluData.unitPrice = SaleData.unitPrice;
		tmpNum = SaleData.pluNum;
		saleFuncPluDataWriteInSale(SaleData.pluNum);
		SaleData.pluNum = tmpNum;
		saleFuncPluDataReadInSale(SaleData.pluNum);
	}
	else
	{
		SaleData.pluNum = 1;
		saleFuncPluDataReadInSale(SaleData.pluNum);
		if(pluName != 0) MakeStrCopy(tempPluData.name, pluName, 6);
		if(tempUnitPrice != 0xFFFFFFFF) tempPluData.unitPrice = tempUnitPrice;
		else tempPluData.unitPrice = SaleData.unitPrice;
#ifdef COUNTRY_FRANCE
		SaleData.pluNum = 0;
#endif
	}

	SaleData.unitPrice = tempPluData.unitPrice;

#ifdef COUNTRY_FRANCE
	// Tare 값이 0일 경우 Tare를 0으로 설정하지 않음(기존 값 유지)
	if(tempTare == 0) { tempTare = SaleData.keyTare; }
	if (tempTare != 0xFFFFFFFF) { AdSetPT(tempTare, 1); }
#else
	if (tempTare != 0xFFFFFFFF) AdSetPT(tempTare,0);
#endif
	ReportReadData(SaleData.pluNum, &tempPluReport);
}

/*	void receivedAllPluDataApply(INT8U *pluName, INT32U tempUnitPrice, INT32U tempTare)
	{
		INT8U i, tmpNum;
	
		saleFuncClearPluName(); 
		
		if(SaleData.pluNum)
		{
			MakeStrCopy(tempPluData.name, pluName, 6);
			tempPluData.unitPrice = tempUnitPrice;
			tmpNum = SaleData.pluNum;
			saleFuncPluDataWriteInSale(SaleData.pluNum);
			SaleData.pluNum = tmpNum;
			saleFuncPluDataReadInSale(SaleData.pluNum);
		}
		else
		{
			SaleData.pluNum = 1;
			saleFuncPluDataReadInSale(SaleData.pluNum);
			MakeStrCopy(tempPluData.name, pluName, 6);
			tempPluData.unitPrice = tempUnitPrice;
		}
	
		SaleData.unitPrice = tempPluData.unitPrice;
		AdSetPT(tempTare,0);
		ReportReadData(SaleData.pluNum, &tempPluReport);
	}*/
/**
********************************************************************************
* @brief	ER JR ECR에서 Unit Price data를	받아오는 함수
* @param	none
* @return	none
* @remark	
********************************************************************************
*/
/*	void receivedUnitPriceDataApply(INT32U tempUnitPrice)
	{
		INT8U i, tmpNum;
	
		
		if(SaleData.pluNum)
		{
//			MakeStrCopy(tempPluData.name, pluName, 6);
			tempPluData.unitPrice = tempUnitPrice;
			tmpNum = SaleData.pluNum;
			saleFuncPluDataWriteInSale(SaleData.pluNum);
			SaleData.pluNum = tmpNum;
			saleFuncPluDataReadInSale(SaleData.pluNum);
		}
		else
		{
			SaleData.pluNum = 1;
			saleFuncPluDataReadInSale(SaleData.pluNum);
//			MakeStrCopy(tempPluData.name, pluName, 6);
			tempPluData.unitPrice = tempUnitPrice;
		}
	
		SaleData.unitPrice = tempPluData.unitPrice; 
		ReportReadData(SaleData.pluNum, &tempPluReport);
	}*/
/**
********************************************************************************
* @brief	ER JR ECR에서 PLU Name data를	받아오는 함수
* @param	none
* @return	none
* @remark	
********************************************************************************
*/
/*	void receivedPluNameApply(INT8U *pluName)
	{
		INT8U i, tmpNum;
	
		saleFuncClearPluName(); 
		
		if(SaleData.pluNum)
		{
			MakeStrCopy(tempPluData.name, pluName, 6);
			tempPluData.unitPrice = SaleData.unitPrice;
			tmpNum = SaleData.pluNum;
			saleFuncPluDataWriteInSale(SaleData.pluNum);
			SaleData.pluNum = tmpNum;
			saleFuncPluDataReadInSale(SaleData.pluNum);
		}
		else
		{
			SaleData.pluNum = 1;
			saleFuncPluDataReadInSale(SaleData.pluNum);
			MakeStrCopy(tempPluData.name, pluName, 6);
			tempPluData.unitPrice = SaleData.unitPrice;
		}
	
		SaleData.unitPrice = tempPluData.unitPrice;
		ReportReadData(SaleData.pluNum, &tempPluReport);
	}*/
/**
********************************************************************************
* @brief	ER JR ECR에서 Tare data를	받아오는 함수
* @param	none
* @return	none
* @remark	
********************************************************************************
*/
/*	void receivedTareValueApply(INT32U tempTare)
	{
		INT8U i, tmpNum;
	
		
		if(SaleData.pluNum)
		{
//			MakeStrCopy(tempPluData.name, pluName, 6);
			tempPluData.unitPrice = SaleData.unitPrice;
			tmpNum = SaleData.pluNum;
			saleFuncPluDataWriteInSale(SaleData.pluNum);
			SaleData.pluNum = tmpNum;
			saleFuncPluDataReadInSale(SaleData.pluNum);
		}
		else
		{
			SaleData.pluNum = 1;
			saleFuncPluDataReadInSale(SaleData.pluNum);
//			MakeStrCopy(tempPluData.name, pluName, 6);
			tempPluData.unitPrice = SaleData.unitPrice;
		}
	
		SaleData.unitPrice = tempPluData.unitPrice;
		AdSetPT(tempTare,0);   
		ReportReadData(SaleData.pluNum, &tempPluReport);
	}*/
#endif

/**
********************************************************************************
* @brief    Sale Mode Indicator 함수 
* @param    none
* @return   none
* @remark   설정 Flag에 따라 현상태를 indicatation 한다.  \n
*           설정할 indicator 정보는 UiDefine.h를 참조한다. \n
********************************************************************************
*/
void saleSetIndicator(void)
{
	DispSetIndicator(INDICATOR_SUM, OFF);
	DispSetIndicator(INDICATOR_KG, OFF);
	DispSetIndicator(INDICATOR_UNITPRICE_KG, OFF);
	DispSetIndicator(INDICATOR_LB, OFF);
	DispSetIndicator(INDICATOR_UNITPRICE_LB, OFF);
	DispSetIndicator(INDICATOR_OZ, OFF);
	DispSetIndicator(INDICATOR_UNITPRICE_OZ, OFF);
	DispSetIndicator(INDICATOR_GRAM, OFF);
	
	if (scaleUseEuroFlag)
	{
		if (saleUseEuroFlag)
		{
			DispSetIndicator(INDICATOR_EURO, ON);
			DispSetIndicator(INDICATOR_EURO_KG, ON);
		}
		else
		{
			DispSetIndicator(INDICATOR_EURO, OFF);
			DispSetIndicator(INDICATOR_EURO_KG, OFF);
		}
	}
	else	
	{
		if (salePrePackFlag)
		{
			DispSetIndicator(INDICATOR_PREPACK, ON);
		}
		else
		{
			DispSetIndicator(INDICATOR_PREPACK, OFF);
		}
	 }

		if(scaleUseKgUnitFlag && scaleUseLbUnitFlag)
		{
			switch (SaleData.weightUnit)
			{
				case UNIT_KG:
					DispSetIndicator(INDICATOR_KG, ON);				
					break;

				case UNIT_LB:
					DispSetIndicator(INDICATOR_LB, ON);				
					break;
					
				case UNIT_OZ:
					DispSetIndicator(INDICATOR_OZ, ON); 			
					break;	
					
				default:
					break;
			}
		}

		if(scaleUseKgUnitFlag && scaleUseLbUnitFlag)
		{
			switch (SaleData.weightUnit)
			{
				case UNIT_KG:
					DispSetIndicator(INDICATOR_UNITPRICE_KG, ON);
					break;
				case UNIT_LB:
					DispSetIndicator(INDICATOR_UNITPRICE_LB, ON);
					break;
					
				case UNIT_OZ:
					DispSetIndicator(INDICATOR_UNITPRICE_OZ, ON);
					break;
					
				default:
					break;
			}	
		}

	if (saleHundredGramEnable)
	{
		DispSetIndicator(INDICATOR_UNITPRICE_100G, ON);
		DispSetIndicator(INDICATOR_UNITPRICE_KG, OFF);
	}
	else
	{
		DispSetIndicator(INDICATOR_UNITPRICE_100G, OFF);
	}

	if(scaleTareClearFlag)
	{
		if (!saleTareClearFlag)
		{
			DispSetIndicator(INDICATOR_TARE_SAVE, ON);
		}
		else
		{
			DispSetIndicator(INDICATOR_TARE_SAVE, OFF);
		}
	}
	if (saleBothSaveFlag)
	{
		DispSetIndicator(INDICATOR_BOTH_SAVE, ON);
	}
	else
	{
		DispSetIndicator(INDICATOR_BOTH_SAVE, OFF);
	}
	if (SaleData.addCount)
	{
		DispSetIndicator(INDICATOR_SUM, ON);
	}
	
#ifdef USE_CTS_TRIANGLE //인디게이터 미사용으로 결정
	if (CtsCheckExpired((INT8U *)(&TimeData)))
	{
		DispSetIndicator(INDICATOR_CTS, ON);
	}
	else
	{
		DispSetIndicator(INDICATOR_CTS, OFF);      
	}
#endif    

}


/**
********************************************************************************
* @brief    현재 Sale mode 에서 사용할 weight, unitprice에 따른 total price 를 결정한다.  
* @param    none
* @return   none
* @remark   AdData의 무게값을 잃어 현재 total price 계산. 	\n
*           Transaction의 Start와 End를 결정한다.         	\n
*			Hold 무게를 결정한다. 				\n
********************************************************************************
*/
void saleWeightFunc(void)
{
	INT8U returnData;
	INT32U	tempData;
	FP64	tempDoubledata;		

#ifdef USE_TURKEY_POS_PROTOCOL
	if (saleTareClearFlag && (AbsInt32s(AdData.rawSetZeroData) <= (INT32S)(AdData.d1 * 2)))	// Tare Clear 동작 조건: 2d 이내
#else
	if (salePercentTareCallFlag == OFF && saleTareClearFlag && transactionEndFlag  && AdData.grossZeroFlag && saleBothSaveFlag == OFF)
#endif
	{
		returnData = AdSetTare();
		if (returnData == ERR_NONE || returnData == ERR_NO_TARE)
		{
			transactionEndFlag = FALSE;		
		}
#ifdef USE_TURKEY_POS_PROTOCOL
		else
			saleKeyCommErrFlagSet(OFF);
#endif
	}

	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:
			if(saleAutoHoldExcute == FALSE && SaleData.weight <= 0) saleAutoHoldExcute = TRUE;
				
			if(saleAutoHoldEnable && SaleData.weight > 0 && saleAutoHoldExcute && (AdData.rawSetTareData > (INT32S)(AdData.d1 * 20)))
			{			
				holdCount = 0;
				SaleData.holdWeight = 0;
				holdDisplayFlash = 0;
				saleAutoHoldExcute = FALSE;
				saleTransactionWeightCheck = TRUE;
				SaleData.saleState = SALE_STATE_HOLD;		
				TimerSetDelay100ms(TIMER_DELAY_100MS_AUTO_HOLD, 10);
			}
		#ifdef USE_AD_READ_COMPLETE_CHECK
			if(SaleData.weight > 0 && AdData.stableFlag && AdDataReadCompleteFlag)
		#else
			if(SaleData.weight > 0 && AdData.stableFlag)
		#endif
			{
				saleTransactionWeightCheck = TRUE;
			}
			tempDoubledata = SaleData.weight;

			if (saleHundredGramEnable || saleHundredGramPrice)
			{
				tempData = SaleData.unitPrice * 10;
			}
			else
			{
				tempData = SaleData.unitPrice;
			}
			
			tempDoubledata = tempDoubledata * tempData;

			if (SaleData.weightDecPos == 0)
			{
				tempDoubledata = tempDoubledata / DecPowerConvTable[salePriceForUnit];
			}			
			else if (SaleData.weightDecPos < 4)
			{
				tempData = DecPowerConvTable[3 - SaleData.weightDecPos];
				tempDoubledata = tempDoubledata / DecPowerConvTable[salePriceForUnit];
				tempDoubledata = tempDoubledata * tempData;	
			}
			else
			{
				tempData = DecPowerConvTable[SaleData.weightDecPos - 3];

				tempDoubledata = tempDoubledata / DecPowerConvTable[salePriceForUnit];
				tempDoubledata = tempDoubledata / tempData;				
			}		    

			tempData = tempDoubledata;
			if ((tempDoubledata - tempData) >= 0.5) tempData++;			
					
			SaleData.totalPrice = PriceRoundOff(tempData, saleRoundOffUnit[saleRoundOffUnitFlag]);			

			if (SaleData.weight <= 0)
			{
				SaleData.totalPrice = 0;		
			}
			if (SaleData.weightUnit == 2)	  // Oz 일때는 가격계산 안한다 . 
			{
				SaleData.totalPrice = 0;
				break;
			}
			break;
			
		case SALE_STATE_MODE_SUM:
			if (sumCount < 10)
			{
				sumWeight = sumWeight + SaleData.weight;
				sumCount++;
			}
			else
			{
				if (sumCount == 10)
				{
						sumWeight = sumWeight / 10;
		
						sumSaveWeight = sumSaveWeight + sumWeight;
						
						if(sumSaveWeight > 99999) sumSaveWeight=0;
						
				}
				sumCount = 11;
			}		
			break;		
		
		case SALE_STATE_CANCEL:
			if (sumCount <1)
			{
				sumCount++;
				sumSaveWeight = 0;
			}
			else
			{
				sumCount = 2 ;
			}		
			break;		

		case SALE_STATE_ADD:	
			if (SaleData.weight <= 0)
			{
				saleFuncClearUnitPrice();						// Add 키에 의해 판매가 이루어진 경우
				SaleData.saleState = SALE_STATE_NORMAL;
			}
			break;

		case SALE_STATE_HOLD:
			if(AdData.overloadFlag)
			{
				SaleData.holdWeight = 0;
				SaleData.saleState = SALE_STATE_NORMAL;
			}
			if(saleAutoHoldExcute == FALSE && SaleData.weight <= 0) saleAutoHoldExcute = TRUE;

			if (SaleData.weight > 0 && holdCount == 0)
			{	
				DispSetConstStrMsg(WIN_WEIGHT, " HOLD");
				DispSetRunMsgPage(10, 2);
			}
			
			if(TimerCheckDelay100ms(TIMER_DELAY_100MS_AUTO_HOLD) == 1)
			{
				TimerSetDelay100ms(TIMER_DELAY_100MS_AUTO_HOLD, 1);
				if (SaleData.weight <= 0)
				{
					SaleData.saleState = SALE_STATE_NORMAL;
				}		
				if (holdCount < 10)
				{
					SaleData.holdWeight = SaleData.holdWeight + SaleData.weight;
					holdCount++;
				}
				else
				{
					if (holdCount == 10)
					{
		
						SaleData.holdWeight = AdGetRoundedWeight(SaleData.holdWeight / 10);//round off
						tempDoubledata = SaleData.holdWeight;
						tempDoubledata = tempDoubledata * SaleData.unitPrice;
		
						if (SaleData.weightDecPos == 0)
						{
							tempDoubledata = tempDoubledata / DecPowerConvTable[salePriceForUnit];
						}			
						else if (SaleData.weightDecPos < 4)
						{
							tempData = DecPowerConvTable[3 - SaleData.weightDecPos];
				
							tempDoubledata = tempDoubledata / DecPowerConvTable[salePriceForUnit];
							tempDoubledata = tempDoubledata * tempData; 			
						}
						else
						{
							tempData = DecPowerConvTable[SaleData.weightDecPos - 3];
		
							tempDoubledata = tempDoubledata / DecPowerConvTable[salePriceForUnit];
							tempDoubledata = tempDoubledata / tempData; 			
						}			
		
						tempData = tempDoubledata;		
						if ((tempDoubledata - tempData) >= 0.5) tempData++;
						
						SaleData.totalPrice = PriceRoundOff(tempData, saleRoundOffUnit[saleRoundOffUnitFlag]);
		
						if (SaleData.holdWeight <= 0)
						{
							SaleData.totalPrice = 0;
						}
					}
					holdCount = 11;
				}	
			}
			break;
		default:
			break;
	}
}

/**
********************************************************************************
* @brief    현재 Sale mode에서 Weight data를 Display 하는 함수   
* @param    weight data
* @return   none
* @remark   weight data를 Display 한다.
********************************************************************************
*/
void saleDisplayWeight(INT32S dispWeight)
{
	saleDispNumWithDecPoint(WIN_WEIGHT, SaleData.weightDecPos, weightDotTypeFlag, dispWeight, 0);
	if (SaleData.lastDigitIsInvalid)
	{
		if (SaleData.weightDecPos == 1) { ////2kg일 경우 0.5g에서 1g으로 올라가면 decimal point 삭제해야 하므로 다시 Display함수 불러줌
			saleDispNumWithDecPoint(WIN_WEIGHT,  0, weightDotTypeFlag, dispWeight, 0);
		}
		DispSetChar(WIN_WEIGHT, DispGetWinSize(WIN_WEIGHT) - 1, ' ');
	}
}

/**
********************************************************************************
* @brief    현재 Sale mode 정보를 Display 하는 함수   
* @param    none
* @return   none
* @remark   각 상태별 해당값을 Display 한다.
********************************************************************************
*/
#ifdef USE_GROSS_KEY
extern BOOLEAN AdGrossIndicatorOnFlag;
#endif
void saleDisplayFunc(void)
{
	INT8U	textBuf[8];
	INT16U	priceHi;
	INT32U	tempPrice;
	INT32S	tempInt32s;

	if (SaleData.saleState != SALE_STATE_NORMAL)
	{
		PowerSaverResetTimer(TIMER_DELAY_100MS_STAND_BY);
	}

	switch (SaleData.saleState)		/*	display	*/
	{
		case SALE_STATE_NORMAL:	
			saleReceiveMoney = 0L;   
			
			if (SaleData.unitPrice == 0)
			{
				saleTransactionWeightCheck = FALSE;
			}
			if(SaleData.weight <= 0 && saleTransactionWeightCheck && SaleData.unitPrice)
			{
				saleFuncClearUnitPrice();
				transactionEndFlag = TRUE;
				saleTransactionWeightCheck = FALSE;
#ifdef USE_AD_READ_COMPLETE_CHECK
				AdDataReadCompleteFlag = FALSE;
#endif
			}
#ifdef USE_GROSS_KEY
			if(AdGrossIndicatorOnFlag == 1)
			{
				saleDisplayWeight(SaleData.weight + AdData.tareWeight);
			}
			else
			{
				saleDisplayWeight(SaleData.weight);
			}
#else
			saleDisplayWeight(SaleData.weight);
#endif
			if(!saleUseEuroFlag) saleDispNumWithDecPoint(WIN_UNIT_PRICE,  priceDecPos, priceDotTypeFlag, SaleData.unitPrice, saleUseThousandPtFlag);
			else saleDispNumWithDecPoint(WIN_UNIT_PRICE,  saleEuroDispDecPos, priceDotTypeFlag, SaleData.unitPrice, saleUseThousandPtFlag);

			if (SaleData.totalPrice <= TOTALPRICE_MAX)
			{	
				if (saleUsePluNameFlag && SaleData.pluNum && SaleData.totalPrice <= 0)
				{
					if(saleDefaultPluName[0] != 0x00 && (SaleData.pluNum <= DIRECT_PLU_MAX)) DispSetStr(WIN_TOTAL_PRICE, saleDefaultPluName);
					else DispSetStr(WIN_TOTAL_PRICE, tempPluData.name);
				}
				else
				{
					if(!saleUseEuroFlag) saleDispNumWithDecPoint(WIN_TOTAL_PRICE, priceDecPos, priceDotTypeFlag, SaleData.totalPrice, saleUseThousandPtFlag);	
					else saleDispNumWithDecPoint(WIN_TOTAL_PRICE, saleEuroDispDecPos, priceDotTypeFlag, SaleData.totalPrice, saleUseThousandPtFlag);	
				}
			}
			else
			{
				DispSetStr(WIN_TOTAL_PRICE, " OVER ");
				BuzzerSetOnTimes(1); 
			}
			DispSetIndicator(INDICATOR_HOLD, OFF);
			break;
		case SALE_STATE_MULTI:
			MakeStrWithNum(textBuf, "M-", saleMultiCount, 6);		
			DispSetStr(WIN_WEIGHT, textBuf);		
			DispSetNum(WIN_UNIT_PRICE,  SaleData.unitPrice, 0);			
			if (SaleData.unitPrice * (INT32U)saleMultiCount > TOTALPRICE_MAX)
			{
				DispSetStr(WIN_TOTAL_PRICE, " OVER ");
				BuzzerSetOnTimes(1); 
			}
			else
			{
				SaleData.totalPrice = SaleData.unitPrice * saleMultiCount;
				DispSetNum(WIN_TOTAL_PRICE, SaleData.totalPrice, 0);
			}					
			break;
#ifdef USE_MULTI_PRINT_FUNC
		case SALE_STATE_MULTI_PRINT:
			MakeStrWithNum(textBuf, "CL-", saleMultiPrintCount, 6);		
			DispSetStr(WIN_WEIGHT, textBuf);		
			DispSetNum(WIN_UNIT_PRICE,  SaleData.unitPrice, 0);
			if (saleMultiPrintCount > MULTI_PRINT_MAX)
			{
				DispSetStr(WIN_TOTAL_PRICE, " OVER ");
				BuzzerSetOnTimes(1); 
			}
			else
			{
				DispSetNum(WIN_TOTAL_PRICE, SaleData.totalPrice, 0);
			}
			break;
#endif
		case SALE_STATE_PLU_NUM:
			DispSetStr(WIN_WEIGHT, "PLU NU");
			saleDispNumWithDecPoint(WIN_UNIT_PRICE, 0, priceDotTypeFlag, SaleData.pluNum, 0);
			DispSetNum(WIN_TOTAL_PRICE, tempPluData.unitPrice, 0);
			break;

		case SALE_STATE_PLU_TARE:
			DispSetStr(WIN_WEIGHT, "PLU TA");
			saleDispNumWithDecPoint(WIN_UNIT_PRICE, SaleData.weightDecPos, weightDotTypeFlag, tempPluData.tare, 0);
			DispSetNum(WIN_TOTAL_PRICE, tempPluData.unitPrice, 0);
			break;

		case SALE_STATE_PLU_NAME:
			DispSetCharWithHexNum(WIN_UNIT_PRICE, (INT8U)tempPluData.name[salePluNamePosition]);
			saleDispNumWithDecPoint(WIN_TOTAL_PRICE, 0, 0, salePluNamePosition + 1, 0);	  
			MakeStrWithNum(textBuf, "PLU-", SaleData.pluNum, 6);
			DispSetStr(WIN_WEIGHT, textBuf);
			break;

		case SALE_STATE_SUM_TTP:
			if (SaleData.addPrice > TOTALPRICE_MAX)
			{
				DispSetError(ERROR_NUM_TOTAL_ADDPRICE_OVER);
				SaleData.saleState = SALE_STATE_NORMAL;
			}
			else
			{
				DispSetStr(WIN_WEIGHT, " TTP  ");
				saleDispNumWithDecPoint(WIN_UNIT_PRICE, 0, priceDotTypeFlag, SaleData.addCount, 0);
				DispSetNum(WIN_TOTAL_PRICE, SaleData.addPrice, 0);
			}
			break;

		case SALE_STATE_CANCEL:
			DispSetStr(WIN_WEIGHT, " TTP-");
			saleDispNumWithDecPoint(WIN_UNIT_PRICE, priceDecPos, priceDotTypeFlag, saleReceiveMoney, saleUseThousandPtFlag);
			DispSetNum(WIN_TOTAL_PRICE, SaleData.addPrice, 0);
			break;

		case SALE_STATE_PAY:	
				DispSetStr(WIN_WEIGHT, " PAY ");				
			saleDispNumWithDecPoint(WIN_UNIT_PRICE,  priceDecPos, priceDotTypeFlag, saleReceiveMoney, saleUseThousandPtFlag);
			DispSetNum(WIN_TOTAL_PRICE, SaleData.totalPrice, 0);	
			break;

		case SALE_STATE_PAY_CHANGE:
			DispSetStr(WIN_WEIGHT, " CHG ");
			DispSetNum(WIN_UNIT_PRICE, saleReceiveMoney, 0);
			DispSetNum(WIN_TOTAL_PRICE, saleChangeMoney, 0);
			break;
		case SALE_STATE_EURO_INPUT:
			DispSetStr(WIN_WEIGHT, " EURO ");
			DispSetStr(WIN_TOTAL_PRICE, "RATE  ");
			saleDispNumWithDecPoint(WIN_UNIT_PRICE, saleEuroDecPos, 0, saleEuroRate, 0);	
			break;

		case SALE_STATE_EURO_DISPLAY:			
			DispSetStr(WIN_WEIGHT, " EURO ");
			DispSetStr(WIN_UNIT_PRICE, "      ");
			saleDispNumWithDecPoint(WIN_TOTAL_PRICE, saleEuroDecPos, 0, saleEuroRate, 0);
			break;

		case SALE_STATE_NOEURO_DISPLAY:
			DispSetStr(WIN_WEIGHT, "NOEURO");
			DispSetStr(WIN_UNIT_PRICE, "      ");
			saleDispNumWithDecPoint(WIN_TOTAL_PRICE, saleEuroDecPos, 0, saleEuroRate, 0);
			break;	
		case SALE_STATE_MODE:
			DispSetStr(WIN_WEIGHT,      " DTP  ");
			DispSetStr(WIN_UNIT_PRICE,  "      ");
			DispSetStr(WIN_TOTAL_PRICE, "      ");
			break;
		
		case SALE_STATE_MODE_SUM:
			if (scaleUseEuroFlag && saleUseEuroFlag)
			{
				tempPrice = saleFuncDollar2Euro(dailyTotalReport.price);			
			}
			else
			{
				tempPrice = dailyTotalReport.price;				
			}

			priceHi = tempPrice / (UNITPRICE_MAX + 1);
			tempPrice = tempPrice % (UNITPRICE_MAX + 1);	

			if (priceHi > 0)
			{
				DispSetDecPoint(WIN_WEIGHT, 0, 0, 0);  //display position				
				DispSetNum(WIN_WEIGHT, priceHi, 0);
			}
			else
			{
				DispSetStr(WIN_WEIGHT, "DAILY");
			}

			DispSetDecPoint(WIN_TOTAL_PRICE, 0, 0, 0);				
			DispSetNum(WIN_TOTAL_PRICE, dailyTotalReport.count, 0);
			
			if (scaleUseEuroFlag && saleUseEuroFlag) DispSetDecPoint(WIN_UNIT_PRICE, saleEuroDispDecPos, priceDotTypeFlag, 0);  //display position
			else DispSetDecPoint(WIN_UNIT_PRICE, priceDecPos, priceDotTypeFlag, 0);  //display position
			if (priceHi > 0) DispSetNum(WIN_UNIT_PRICE, tempPrice, '0');
			else DispSetNum(WIN_UNIT_PRICE, tempPrice, ' ');
				
			break;

		case SALE_STATE_MODE_PLU_NUM:			
			MakeStrWithNum(textBuf, "P-", SaleData.pluNum, 6);	
			DispSetStr(WIN_WEIGHT, textBuf);				
			DispSetStr(WIN_UNIT_PRICE,  "      ");
			DispSetStr(WIN_TOTAL_PRICE, "      ");
			break;

		case SALE_STATE_MODE_PLU:
			if (scaleUseEuroFlag && saleUseEuroFlag )				
			{
				tempPrice = saleFuncDollar2Euro(tempPluReport.price);
			}
			else
			{
				tempPrice = tempPluReport.price;
			}

			priceHi = tempPrice / (UNITPRICE_MAX + 1);
			tempPrice = tempPrice % (UNITPRICE_MAX + 1);	

			if (priceHi > 0)
			{
				DispSetDecPoint(WIN_WEIGHT, 0, 0, 0);  //display position				
				DispSetNum(WIN_WEIGHT, priceHi, 0);
			}
			else
			{
				MakeStrWithNum(textBuf, "P-", SaleData.pluNum, 6);
				DispSetStr(WIN_WEIGHT, textBuf);
			}

			DispSetDecPoint(WIN_TOTAL_PRICE, 0, 0, 0);				
			DispSetNum(WIN_TOTAL_PRICE, tempPluReport.count, 0);
			
			DispSetDecPoint(WIN_UNIT_PRICE, priceDecPos, priceDotTypeFlag, 0);  //display position
			DispSetNum(WIN_UNIT_PRICE, tempPrice, 0);
			break;
						
		case SALE_STATE_ADD:	
			DispSetStr(WIN_WEIGHT, " ADD  ");
			saleDispNumWithDecPoint(WIN_UNIT_PRICE, 0, priceDotTypeFlag, SaleData.addCount, 0); 
			DispSetNum(WIN_TOTAL_PRICE, SaleData.addPrice, 0);
			break;
			
		case SALE_STATE_FIXED_PRICE:			
			DispSetStr(WIN_WEIGHT, " FAD  ");
			saleDispNumWithDecPoint(WIN_UNIT_PRICE, 0, priceDotTypeFlag, SaleData.addCount, 0); 
			DispSetNum(WIN_TOTAL_PRICE, SaleData.addPrice, 0);
			break;
			
		case SALE_STATE_HOLD:
				DispSetIndicator(INDICATOR_HOLD, ON);
				holdDisplayFlash++;
				if (holdDisplayFlash == 300)
				{					
					if(saleAutoHoldExcute) 
					{
						SaleData.saleState = SALE_STATE_NORMAL;
						DispSetIndicator(INDICATOR_HOLD, OFF);
					}
		
				}
				if (holdDisplayFlash % 50 < 25)
				{
					saleDisplayWeight(SaleData.holdWeight);
					DispSetNum(WIN_UNIT_PRICE, SaleData.unitPrice, 0);
					DispSetNum(WIN_TOTAL_PRICE, SaleData.totalPrice, 0); 
				
				}
				else
				{
					DispSetStr(WIN_WEIGHT, "	  ");
					DispSetStr(WIN_UNIT_PRICE, "	  ");
					DispSetStr(WIN_TOTAL_PRICE, "	   ");
				}					
			break;				

		case SALE_STATE_AHOLD:			
			if(saleAutoHoldEnable) 
			{
				DispSetStr(WIN_WEIGHT, "AUTO");
				DispSetStr(WIN_UNIT_PRICE, " HOLD");
				DispSetStr(WIN_TOTAL_PRICE, "  ON");
			}
			else
			{			
				DispSetStr(WIN_WEIGHT, "AUTO");
				DispSetStr(WIN_UNIT_PRICE, " HOLD");
				DispSetStr(WIN_TOTAL_PRICE, "  OFF");
			}
			break;
			
		case SALE_STATE_PERCENT_TARE:
			DispSetStr(WIN_WEIGHT, "  PROG");
			DispSetStr(WIN_UNIT_PRICE, "  TARE");
			saleDispNumWithDecPoint(WIN_TOTAL_PRICE, SaleData.weightDecPos, weightDotTypeFlag, salePercentTare, 0);			
			break;	
				
		case SALE_STATE_PERCENT_PRICE:		
			DispSetStr(WIN_WEIGHT, "  PROG");
			DispSetStr(WIN_UNIT_PRICE, " PRICE");
			saleDispNumWithDecPoint(WIN_TOTAL_PRICE, priceDecPos, priceDotTypeFlag, salePercentPrice, saleUseThousandPtFlag);			
			break;

		case SALE_STATE_PERCENT_RATE:
			DispSetStr(WIN_WEIGHT, "  PROG");
			DispSetStr(WIN_UNIT_PRICE, " PRCENT");
			saleDispNumWithDecPoint(WIN_TOTAL_PRICE, 0, 0, salePercentRate, 0);
			break;

		case SALE_STATE_CHECK_BATTERY:  
			DispSetStr(WIN_WEIGHT, "  BAT ");
			saleDispNumWithDecPoint(WIN_UNIT_PRICE, 0, 0, saleBatteryCapacity, 0);
			DispSetStr(WIN_TOTAL_PRICE, "      ");
			break;

		default:
			DispSetStr(WIN_WEIGHT, "ERR-S");
			DispSetStr(WIN_UNIT_PRICE, "STATE ");
			DispSetNum(WIN_TOTAL_PRICE, SaleData.saleState, 0);
			BuzzerSetOnTimes(2);	
			
			SaleData.saleState = SALE_STATE_NORMAL;
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SALE_STATE_NORMAL);
			break;
	}
}

/**
********************************************************************************
* @brief    Zero Key 함수에서 Stable을 기다리고 Tare기능을 수행하는 함수 
* @param    none
* @return   none
* @remark  
********************************************************************************
*/
void saleZeroKeyProc(void)
{
	if (AdSetZero() != ERR_NONE)
	{
		BuzzerSetOnTimes(2);	
	}
}

/**
********************************************************************************
* @brief    Zero Key 동작을 수행한다.   
* @param    none
* @return   none
* @remark   Zero를 잡지 못하면 경고음
********************************************************************************
*/
void saleZeroKey(void)
{
	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:	
			saleTareKeyPressed = 0;
			saleZeroKeyPressed = 1;
			TimerSetDelay100ms(TIMER_DELAY_100MS_WAIT_STABLE, 50);
#ifdef DIALOG6
			SaleData.zeroErrFlag = 0;
#endif
			break;			
			
		case SALE_STATE_PLU_NAME:
			if (salePluNamePosition > 0)
			{
				salePluNamePosition--; 
				
				if (salePluNamePosition <0)
				{
					BuzzerSetOnTimes(2);
					salePluNamePosition = 0;
#ifdef DIALOG6
					SaleData.zeroErrFlag = 1;
#endif
				}
			}
			else
			{
				BuzzerSetOnTimes(2);
#ifdef DIALOG6
				SaleData.zeroErrFlag = 1;
#endif
			}
			break;

		default:
			BuzzerSetOnTimes(2);
#ifdef DIALOG6
			SaleData.zeroErrFlag = 1;
#endif
			break;
	}
}

/**
********************************************************************************
* @brief    Tare Key 함수에서 Stable을 기다리고 Tare기능을 수행하는 함수 
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void saleWeightTareProc(void)
{
	INT8U returnData;

	// Unit Price, total price 제거 하고 Tare설정 가능 하게 수정 
	if (salePercentTare == 0 )
	{
#ifndef USE_PLU_AFTER_TARE_SET
		if (SaleData.totalPrice)
		{
			//Total price검사 후 Unit price 및 Total Price 제거 
			SaleData.unitPrice = 0;
			SaleData.totalPrice = 0;
		}
		if (SaleData.pluNum)
		{
			//PLU 호출 상태 이면 PLU Name 제거
			SaleData.pluNum = 0;
			saleFuncClearPluName();
		}
#endif
		returnData = AdSetTare();

		if (returnData == ERR_NONE)
		{
			//SaleData.unitPrice = 0;
			saleTransactionWeightCheck = FALSE;
			saleTareType = WEIGHT_TARE_CALL;
#ifdef USE_TURKEY_POS_PROTOCOL
			saleKeyCommErrFlagSet(ON);
#endif
		}
		else
		{
#ifdef USE_TURKEY_POS_PROTOCOL
			saleKeyCommErrFlagSet(OFF);
#endif
			BuzzerSetOnTimes(2);						
		}
	}
	else
	{
		BuzzerSetOnTimes(2);
	}
}

/**
********************************************************************************
* @brief    Tare Key 동작을 수행한다.    
* @param    none
* @return   none
* @remark   각 상태별 zero key 기능  수행
********************************************************************************
*/
void saleTareKey(void)
{
	INT8U returnData;
	INT8U tempInt8u;	

	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:
#ifdef USE_GROSS_KEY
			AdGrossIndicatorOnFlag = 0;
#endif
			if (saleUsePresetTareFlag && (SaleData.unitPrice > 0) && AdData.grossZeroFlag && SaleData.pluNum == 0)  	// Key Tare
			{					
				if (AdData.tareWeight == 0)
				{
					returnData = AdSetPT(SaleData.unitPrice, 0);

					if (returnData == ERR_NONE)
					{
						SaleData.unitPrice = 0;
						saleTareType = KEY_TARE_CALL;
					}
					else
					{
						BuzzerSetOnTimes(3);
					}
				}
				else
				{
					BuzzerSetOnTimes(3);	
				}
			}
			else
			{
				saleZeroKeyPressed = 0;
				saleTareKeyPressed = 1;
				TimerSetDelay100ms(TIMER_DELAY_100MS_WAIT_STABLE, 50);
			}
			break;

		case SALE_STATE_PLU_NAME:
				saleFuncPluDataWriteInSale(SaleData.pluNum);
		
			break;

		case SALE_STATE_FIXED_PRICE:
			saleFuncClearUnitPrice();
			break;

		case SALE_STATE_AHOLD:
			if(saleAutoHoldEnable) saleAutoHoldEnable = FALSE;
			else saleAutoHoldEnable = TRUE;
			break;

		default:
			BuzzerSetOnTimes(2);
			break;
	}
}
/**
********************************************************************************
* @brief    Gross Key 동작을 수행한다.    
* @param    none
* @return   none
* @remark   Net Gross 상태의 display toggle key
********************************************************************************
*/
#ifdef USE_GROSS_KEY
void saleGrossKey(void)
{
	INT8U returnData;
	INT8U tempInt8u;	

	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:
			if(AdGrossIndicatorOnFlag == 1)
			{
				AdGrossIndicatorOnFlag = 0;
			}
			else
			{
				AdGrossIndicatorOnFlag = 1;
			}
			break;
		case SALE_STATE_PLU_NAME:
			break;

		case SALE_STATE_FIXED_PRICE:
			saleFuncClearUnitPrice();
			break;
		case SALE_STATE_AHOLD:
			if(saleAutoHoldEnable) saleAutoHoldEnable = FALSE;
			else saleAutoHoldEnable = TRUE;
			break;

		default:
			BuzzerSetOnTimes(2);
			break;
	}
}
#else
void saleGrossKey(void)
{
}
#endif
/* Insert global functions here */
/**
********************************************************************************
* @brief    Hold key 동작을 수행한다. 
* @param    type : defualt type
* @return   none
* @remark   무게가 "0"일때와 일정한 시간후에 "Hold"가 해제됨 \n
********************************************************************************
*/
void saleHoldKey(void)
{
	INT8U tempInt8u;

		switch (SaleData.saleState)
		{
			case SALE_STATE_NORMAL:
				if (SaleData.weight > 0)
				{
					holdCount = 0;
					SaleData.holdWeight = 0;
					holdDisplayFlash = 0;					
					saleTransactionWeightCheck = TRUE;
					SaleData.saleState = SALE_STATE_HOLD;
					TimerSetDelay100ms(TIMER_DELAY_100MS_AUTO_HOLD, 10);
				}
				else
				{
					SaleData.saleState = SALE_STATE_AHOLD;
				}
				break;

			case SALE_STATE_AHOLD:
				SaleData.saleState = SALE_STATE_NORMAL;
				break;
			
			default: 
				BuzzerSetOnTimes(2);
				break;
		}
}

/**
********************************************************************************
* @brief    Clear Key 동작을 수행한다.    
* @param    none
* @return   none
* @remark   각 상태별 Clear key 기능  수행
********************************************************************************
*/
void saleClearKey(void)
{
	INT16U i;

#ifdef	USE_DECIMAL_POINT	
	SaleDecimalPointKeyOnOff = OFF; // csh 20120731
#endif
		
	switch (SaleData.saleState)	 	/*clear*/
	{
		case SALE_STATE_NORMAL:			
			SaleData.unitPrice = 0L;
			if (SaleData.pluNum)
			{
				SaleData.pluNum = 0;
				if (tempPluData.tare) 
				{
					tempPluData.tare = 0;
					AdSetPT(tempPluData.tare, 1);				
				}
				saleFuncClearPluName();
			}
			break;

		case SALE_STATE_PLU_NUM:
			if (SaleData.pluNum == 0)
			{
				BuzzerSetOnTimes(2);	
				tempPluData.unitPrice = 0;			
				SaleData.unitPrice = 0L;
				SaleData.saleState = SALE_STATE_NORMAL;
			}			
			SaleData.pluNum = 0;		
			saleFuncClearPluName();
			break;

		case SALE_STATE_PLU_TARE:
			if (tempPluData.tare == 0)
			{
				BuzzerSetOnTimes(2);
				tempPluData.tare = 0;
				tempPluData.unitPrice = 0;
				SaleData.unitPrice = 0;
				SaleData.saleState = SALE_STATE_NORMAL;
			}
			tempPluData.tare = 0;
			saleFuncClearPluName();
			break;
		
		case SALE_STATE_PLU_NAME:
			salePluNamePosition++;
			if (salePluNamePosition == PLU_NAME_SIZE)
			{
					saleFuncPluDataWriteInSale(SaleData.pluNum);
			}
			break;

		case SALE_STATE_MODE:
		case SALE_STATE_MODE_SUM:
		case SALE_STATE_MODE_PLU:
			DispSetConstStrMsg(WIN_WEIGHT, "CLEAR ");
			DispSetRunMsgPage(30, 0);
			ReportClearAll();
			saleFuncClearUnitPrice();
			SaleData.addPrice = 0;
			SaleData.addCount = 0;
  			dailyTotalReport.count = 0;
			dailyTotalReport.price = 0;	
			saleFirstAdd = 1;		//add kkanno 071101	
			for (i = 0; i <= REPORT_ALL_NUM; i++)
			{
				ReportWriteData(i, &dailyTotalReport);		   
			}
 			SaleData.saleState = SALE_STATE_NORMAL;
			break;

		case SALE_STATE_PAY:
		case SALE_STATE_CANCEL:						
			saleReceiveMoney = 0L;			
			break;

		case SALE_STATE_PAY_CHANGE:		
			SaleData.unitPrice = 0L;			
			SaleData.addPrice = 0;
			SaleData.addCount = 0;
			saleChangeMoney = 0;
			SaleData.saleState = SALE_STATE_NORMAL;
			BuzzerSetOnTimes(2);
			break;

		case SALE_STATE_EURO_INPUT:
			saleEuroRate = 0;			 
		   	saleEuroDecPos = 0;
			break;

		case SALE_STATE_SUM_TTP:
			SaleData.addPrice = 0;
			SaleData.addCount = 0;
			saleFirstAdd = 1;		//add kkanno 071101	
			SaleData.saleState = SALE_STATE_NORMAL;
			BuzzerSetOnTimes(2);
			SaleData.unitPrice = 0;
#ifdef USE_PRINTER			
			PrtPutCRLF(1);
			PrtPutStr("Transaction was canceled");
#endif			
			break;
						
		case SALE_STATE_MULTI:
			if (saleMultiCount == 0)
			{
				SaleData.saleState = SALE_STATE_NORMAL;
			}
			else
			{
				saleMultiCount = 0;
			}
			saleFuncClearPluName();
			break;	

#ifdef USE_MULTI_PRINT_FUNC
		case SALE_STATE_MULTI_PRINT:
			if (saleMultiPrintCount == 0)
			{
				SaleData.saleState = SALE_STATE_NORMAL;
			}
			else
			{
				saleMultiPrintCount = 0;
			}
			saleFuncClearPluName();
			break;
#endif

		case SALE_STATE_PERCENT_TARE:
			if (salePercentTare == 0)
			{
				SaleData.saleState = SALE_STATE_NORMAL;
			}
			salePercentTare = 0;
			break;
		
		case SALE_STATE_PERCENT_PRICE:
			if (salePercentPrice == 0)
			{
				SaleData.saleState = SALE_STATE_NORMAL;
			}
			salePercentPrice = 0;	
			break;

		case SALE_STATE_PERCENT_RATE:
			if (salePercentRate == 0)
			{
				SaleData.saleState = SALE_STATE_NORMAL;
			}
			salePercentRate = 0;
			break;

		case SALE_STATE_FIXED_PRICE:
			saleFuncClearUnitPrice();
			SaleData.saleState = SALE_STATE_NORMAL;
			break;
		
		case SALE_STATE_CHECK_BATTERY :
			saleBatteryCheckFlag = 0;
			SaleData.saleState = SALE_STATE_NORMAL;
			break;
			
		default:			
			BuzzerSetOnTimes(2);
			break;
	}
}

/**
********************************************************************************
* @brief    현재 상태에서 변수 값을 입력 함수.     
* @param    none
* @return   none
* @remark   각 상태별 변수값 입력
********************************************************************************
*/
INT8U saleNumKey(KEY_TYPE key)	   
{
	switch (SaleData.saleState)		/* 	input func	*/ 
	{
		case SALE_STATE_PLU_NAME:
			if (key >= KEY_NUM_0 && key <= KEY_NUM_9)
			{
				tempPluData.name[salePluNamePosition] 
				= (INT8U)EditorInsHexDigit((INT32U)tempPluData.name[salePluNamePosition], key - KEY_NUM_0, 2, 0xff, FALSE);
			}
			else if (key >=KEY_NUM_A && key <=KEY_NUM_F)
			{
				tempPluData.name[salePluNamePosition] 
				= (INT8U)EditorInsHexDigit((INT32U)tempPluData.name[salePluNamePosition], key - KEY_NUM_A + 10, 2, 0xff, FALSE);
			}
			break;

		case SALE_STATE_PLU_NUM:
			SaleData.pluNum = EditorInsDecDigit(SaleData.pluNum, key - KEY_NUM_0, 3, INDIRECT_PLU_MAX, FALSE);		
			break;

		case SALE_STATE_PLU_TARE:
			tempPluData.tare = EditorInsDecDigit(tempPluData.tare, key - KEY_NUM_0, PRICE_SIZE, UNITPRICE_MAX, FALSE);
			break;

		case SALE_STATE_PAY:
		case SALE_STATE_CANCEL:
			saleReceiveMoney = EditorInsDecDigit(saleReceiveMoney, key - KEY_NUM_0, PRICE_SIZE, UNITPRICE_MAX, FALSE);			 
			break;

		case SALE_STATE_EURO_INPUT:
			saleEuroRate = EditorInsDecDigit(saleEuroRate, key - KEY_NUM_0, PRICE_SIZE, UNITPRICE_MAX, FALSE);			 
			break;
		case SALE_STATE_MULTI:
			saleMultiCount = EditorInsDecDigit(saleMultiCount, key - KEY_NUM_0, 3, 1000, FALSE);	//change kkanno 070615
			break;

#ifdef USE_MULTI_PRINT_FUNC
		case SALE_STATE_MULTI_PRINT:
			saleMultiPrintCount = EditorInsDecDigit(saleMultiPrintCount, key - KEY_NUM_0, 2, 100, FALSE);
			break;
#endif

		case SALE_STATE_PERCENT_TARE:
			salePercentTare = EditorInsDecDigit(salePercentTare, key - KEY_NUM_0, PRICE_SIZE, UNITPRICE_MAX, FALSE);
			break;
		
		case SALE_STATE_PERCENT_PRICE:
			salePercentPrice = EditorInsDecDigit(salePercentPrice, key - KEY_NUM_0, PRICE_SIZE, UNITPRICE_MAX, FALSE);		
			break;

		case SALE_STATE_PERCENT_RATE:
			salePercentRate = EditorInsDecDigit(salePercentRate, key - KEY_NUM_0, 3, 100, FALSE);
			break;

		case SALE_STATE_FIXED_PRICE:
			if(SaleData.pluNum) SaleData.pluNum = 0;	//add kkanno 071107
			SaleData.unitPrice = EditorInsDecDigit(SaleData.unitPrice, key - KEY_NUM_0, PRICE_SIZE, UNITPRICE_MAX, FALSE);	
			SaleData.saleState = SALE_STATE_NORMAL;
			break;
	
		case SALE_STATE_NORMAL:
			if (SaleData.weightUnit == 2)	  // g, Oz 일때는 가격계산 안한다 .
			{  
				SaleData.unitPrice = 0;				 
				break;			
			}

			if (SaleData.pluNum)
			{
				SaleData.unitPrice = 0;
				SaleData.pluNum = 0;	
				saleFuncClearPluName();
			}

			if (saleUnitPriceRenewalFlag == ON)
			{
				SaleData.unitPrice = 0;
#ifdef	USE_DECIMAL_POINT	
				SaleDecimalPointKeyOnOff = OFF; // csh 20120731
#endif	
			}

			SaleSetUnitPriceRenewalTimer();
			SaleData.unitPrice = EditorInsDecDigit(SaleData.unitPrice, key - KEY_NUM_0, PRICE_SIZE, UNITPRICE_MAX, TRUE);
			break;

		case SALE_STATE_MODE_PLU:
		case SALE_STATE_MODE_SUM:
			SaleData.pluNum = key - KEY_NUM_0;
			SaleData.saleState = SALE_STATE_MODE_PLU_NUM;
			break;
			
		case SALE_STATE_MODE_PLU_NUM:
			SaleData.pluNum = EditorInsDecDigit(SaleData.pluNum, key - KEY_NUM_0, 2, DIRECT_PLU_MAX, TRUE);
			break;
			
		default:
			break;
	}	

	return SCALE_MODE_SALE;
}	


/**
********************************************************************************
* @brief    Direct PLU 정보를 호출하는 함수    
* @param    none
* @return   none
* @remark   Direct PLU Data를  호출한다.
********************************************************************************
*/
INT8U saleDirectPluCall(KEY_TYPE key)
{
	INT8U  returnVal;
	INT16U tempPluNum;
	INT32U tempPluTare;

	tempPluNum = key - 0x80 + 1;

	if (tempPluNum == 0 || tempPluNum > DIRECT_PLU_MAX || SaleData.weightUnit == UNIT_OZ)
	{
		BuzzerSetOnTimes(2);		
		return SCALE_MODE_SALE;	
	}

	switch (SaleData.saleState)
	{
		case SALE_STATE_FIXED_PRICE:
			SaleData.saleState = SALE_STATE_NORMAL;
			
		case SALE_STATE_NORMAL:			
			SaleData.pluNum = tempPluNum;
			if(saleFuncPluDataReadInSale(SaleData.pluNum))
			{
				SaleData.unitPrice = tempPluData.unitPrice;			//delete kkanno 070605
				ReportReadData(SaleData.pluNum, &tempPluReport);	//delete kkanno 070605
			}
			break;

		case SALE_STATE_PLU_NUM:
			SaleData.pluNum = tempPluNum;
			if (saleUsePluNameFlag)
			{
				saleFuncPluDataReadInSale(SaleData.pluNum);	
				tempPluData.unitPrice = SaleData.unitPrice;
				salePluNamePosition = 0;
				if (saleUsePluTareFlag) SaleData.saleState = SALE_STATE_PLU_TARE;
				else
				{
					SaleData.saleState = SALE_STATE_PLU_NAME; 
				}
			}
			else
			{
				saleFuncPluDataWriteInSale(SaleData.pluNum);						
			}				
			break;

		case SALE_STATE_MODE:			// Report 
		case SALE_STATE_MODE_SUM:
		case SALE_STATE_MODE_PLU:
		case SALE_STATE_MODE_PLU_NUM:	
			SaleData.pluNum = tempPluNum;
			ReportReadData(SaleData.pluNum, &tempPluReport);			
			SaleData.saleState = SALE_STATE_MODE_PLU;
			break;
						
		default:
			BuzzerSetOnTimes(2);
			break;
	}

	return SCALE_MODE_SALE;	

}

/**
********************************************************************************
* @brief    PLU 정보를 호출하는 함수    
* @param    none
* @return   none
* @remark   Direct or Indirect PLU Data를  호출한다.
********************************************************************************
*/
void salePluCallKey(void)
{
	if (SaleData.saleState == SALE_STATE_MODE_PLU_NUM)
	{		
		if (SaleData.pluNum > DIRECT_PLU_MAX || SaleData.pluNum < 1 || SaleData.weightUnit == UNIT_OZ)
		{
			BuzzerSetOnTimes(2);
		}
		else
		{
			ReportReadData(SaleData.pluNum, &tempPluReport);
			SaleData.saleState = SALE_STATE_MODE_PLU;
		}
		return;
	}
	
	if (SaleData.unitPrice == 0 || SaleData.unitPrice > INDIRECT_PLU_MAX || SaleData.weightUnit == UNIT_OZ)
	{
		BuzzerSetOnTimes(2);
	}
	else
	{
		switch (SaleData.saleState)
		{
			case SALE_STATE_NORMAL:		
				SaleData.pluNum = SaleData.unitPrice;
				if (saleFuncPluDataReadInSale(SaleData.pluNum))
				{
					SaleData.unitPrice = tempPluData.unitPrice;
					ReportReadData(SaleData.pluNum, &tempPluReport);
				}
				break;

			default:
				BuzzerSetOnTimes(2);
				break;
		}
	}	
}

/**
********************************************************************************
* @brief    PLU Data / Euro rate Edit     
* @param    none
* @return   none
* @remark   PLU Data Edit\n
*           		Euro 사용시 EuroRate Edit
********************************************************************************
*/
void salePluSaveKey(void)
{
	INT8U returnVal;

	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:
			
			if (AdData.grossZeroFlag)
			{
				tempPluData.unitPrice = SaleData.unitPrice;
				SaleData.pluNum = 0;
				SaleData.saleState = SALE_STATE_PLU_NUM;					
			}
			else
			{
				BuzzerSetOnTimes(2);
			}			
			break;

		case SALE_STATE_PLU_TARE:
			salePluNamePosition = 0;
			returnVal = AdCheckPTInterval(tempPluData.tare);							

			if (returnVal != ERR_NONE)
			{
				BuzzerSetOnTimes(2);	
				tempPluData.tare = 0;
				break;
			}
			SaleData.saleState = SALE_STATE_PLU_NAME;
			break;

		case SALE_STATE_PLU_NUM:						
			if (saleUsePluNameFlag)			// PLU name 입력 상태
			{
				salePluNamePosition = 0;

				if (SaleData.pluNum != 0)
				{
					saleFuncPluDataReadInSale(SaleData.pluNum);	
					tempPluData.unitPrice = SaleData.unitPrice;

					if (SaleData.pluNum <= DIRECT_PLU_MAX)
					{
						if (saleUsePluTareFlag)
						{
							SaleData.saleState = SALE_STATE_PLU_TARE;	
						}
						else
						{
							SaleData.saleState = SALE_STATE_PLU_NAME;	
						}
						
					}
					else
					{ 
						saleFuncPluDataWriteInSale(SaleData.pluNum);						
					}
				}
				else
				{
					saleFuncPluDataWriteInSale(SaleData.pluNum);	
				}
			}
			else
			{
				saleFuncPluDataWriteInSale(SaleData.pluNum);			
			}
			break;
		
		case SALE_STATE_EURO_INPUT:
			if (saleEuroRate == 0)
			{
				BuzzerSetOnTimes(2);	
				return;
			}
			else
			{
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_EURO_DEC, saleEuroDecPos);			
				ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_EURO_RATE, saleEuroRate);
			} 	
			SaleData.unitPrice = 0L;
			SaleData.saleState = SALE_STATE_NORMAL;
			break;
		default:
			BuzzerSetOnTimes(2);
			break;
	}
	
}



/**
********************************************************************************
* @brief    Euro 기능 설정및 해제하는 함수     
* @param    none
* @return   none
* @remark   Euro 기능 수행및 해제
********************************************************************************
*/
void saleEuroKey(void)
{
	INT32S tempData;	
		
	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:
			if (scaleUseEuroFlag == 0)
			{
				BuzzerSetOnTimes(2);
				break;
			}
			if (saleUseEuroFlag == 0)	  // no euro -> euro로 변경 
			{
				tempData = saleFuncDollar2Euro(SaleData.unitPrice);
				if (tempData > UNITPRICE_MAX)				
				{
					DispSetError(ERROR_NUM_CHANGE_PRICE_OVER);
				}
				else
				{
					SaleData.unitPrice = tempData;
					SaleData.addPrice = saleFuncDollar2Euro(SaleData.addPrice);
					SaleData.saleState = SALE_STATE_EURO_DISPLAY;		// <-- Current Display_state
					SaleSetStateDelayTimer(10, SALE_STATE_NORMAL);
					saleUseEuroFlag = 1; 					
					MakeStrCopy(saleUsePriceUnit, "EUR", 3);

				}								
			}
			else	 					// euro	 -> no euro로 변경 
			{
				tempData = saleFuncEuro2Dollar(SaleData.unitPrice);
				if (tempData > UNITPRICE_MAX)				
				{
				 	DispSetError(ERROR_NUM_CHANGE_PRICE_OVER);
				}
				else
				{
					SaleData.unitPrice = tempData;
					SaleData.addPrice = saleFuncEuro2Dollar(SaleData.addPrice);
					SaleData.saleState = SALE_STATE_NOEURO_DISPLAY;
					SaleSetStateDelayTimer(10, SALE_STATE_NORMAL);
					saleUseEuroFlag = 0;
					ParamReadData(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PRICE_SYMBOL, saleUsePriceUnit, PRICE_SYMBOL_SIZE);		// first unit
				}
			}		
			break;

		case SALE_STATE_PLU_TARE:
		case SALE_STATE_PLU_NUM:
			SaleData.saleState = SALE_STATE_EURO_INPUT;
			break;

		case SALE_STATE_EURO_INPUT:
			saleEuroDecPos++;
			if (saleEuroDecPos >= PRICE_SIZE)
			{
				saleEuroDecPos = 0;			
			}
			break;

		default:
			BuzzerSetOnTimes(2);
			break;
	}
}

/**
********************************************************************************
* @brief    Battery 잔량 확인 하는 함수      
* @param    none
* @return   none
* @remark   Battery 잔량 확인
********************************************************************************
*/
void saleBattCheckKey(void)		
{
	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL :
		case SALE_STATE_CHECK_BATTERY :
			if (saleBatteryCheckFlag == 0)
			{
				saleBatteryCheckFlag = 1;
				SaleData.saleState = SALE_STATE_CHECK_BATTERY;
			}
			else
			{
				saleBatteryCheckFlag = 0;
				SaleData.saleState = SALE_STATE_NORMAL;
			}
			break;
		default:
			BuzzerSetOnTimes(2);
			break;
	}			
}


/**
********************************************************************************
* @brief    Back Light 상태변경 하는 함수      
* @param    none
* @return   none
* @remark   Back Light 상태변경
********************************************************************************
*/
void saleBackLightKey(void)
{
	if (ScaleParam.scaleUseBL == ON)
	{
		if (DispBlGetStatus() == OFF)	   // ON
		{
			DispBlSetOnOff(ON);	
		}
		else
		{
			DispBlSetOnOff(OFF);
		}	
#ifdef USE_BACKLIGHT_KEY_ONOFF_ALWAYS
		if(blPowerOffFlag)
		{
			blPowerOffFlag = FALSE;
		}
		else
		{
			blPowerOffFlag = TRUE;
		}
#endif

	}
	else
	{
		PowerOff();
	}
}


/**
********************************************************************************
* @brief    Payment 기능을 수행하는 함수       
* @param    none
* @return   none
* @remark   Payment 기능을 수행
********************************************************************************
*/
void salePayKey(void)
{
#ifdef USE_KEY_COMBINE_PAY_SUM
	if (SaleData.saleState == SALE_STATE_PAY) {
		saleSumKey();
		return;
	}
#endif
	if (SaleData.totalPrice != 0L && SaleData.saleState == SALE_STATE_NORMAL)
	{
		saleReceiveMoney = 0L;
		SaleData.saleState = SALE_STATE_PAY;		
	}
	else
	{				
		BuzzerSetOnTimes(2);								
	}
}
#ifdef USE_ADD_KEY_FOR_CERTIFICATION
/**
********************************************************************************
* @brief    ADD Key 함수    
* @param    none
* @return   none
* @remark   무게 판매와 정가판매시 Add 동작을 수행한다.			\n
*           Add는 총 1000회 까지 가능하다. 						\n
*			Add 시 Print사용 유무에 따라 현재정보가 Print 된다.
********************************************************************************
*/
void saleSumCertiKey(void)
{	
	switch (SaleData.saleState)
	{		
		case SALE_STATE_HOLD:
		case SALE_STATE_NORMAL:
		case SALE_STATE_MULTI:
		case SALE_STATE_PAY:
			SaleData.addState = 3;
			SaleData.printerState = 1;
			break;
			
		case SALE_STATE_MODE:
		case SALE_STATE_MODE_PLU:
		case SALE_STATE_SUM_TTP:
		case SALE_STATE_PAY_CHANGE:
		case SALE_STATE_MODE_SUM:
		case SALE_STATE_FIXED_PRICE:
		default:			
			saleSumKey();
			break;
	}
}
#endif
/**
********************************************************************************
* @brief    SUM 기능을 수행하는 함수       
* @param    none
* @return   none
* @remark   Add data 표시및 Payment 기능호출 	\n
*           Print 설정에 따라 Print 가능
********************************************************************************
*/
void saleSumKey(void)
{
#ifdef	USE_DECIMAL_POINT	
		SaleDecimalPointKeyOnOff = OFF; // csh 20120731
#endif		
	switch (SaleData.saleState)
	{	
#ifdef USE_MULTI_PRINT_FUNC
		case SALE_STATE_MULTI_PRINT:
#endif
		case SALE_STATE_NORMAL:
			if ((SaleData.addCount) && (SaleData.unitPrice == 0)) 		
			{
				if (saleAddCurrency != saleUseEuroFlag)
				{					
					BuzzerSetOnTimes(2);
					return;
				}
				SaleData.saleState = SALE_STATE_SUM_TTP;
			}
			else
			{
				if (AdData.stableFlag)
				{
					if (SaleData.totalPrice > TOTALPRICE_MAX)
					{
						DispSetStr(WIN_TOTAL_PRICE, " OVER ");
						BuzzerSetOnTimes(1); 
					}
					else if (scaleUsePrt == PRT_DLP50 && (AdData.rawSetTareData < (INT32S)(AdData.d1 * 20)))
					{
						BuzzerSetOnTimes(1); 
					}
					else if (SaleData.totalPrice == 0 && SaleData.unitPrice > 0 && (!SaleData.addCount) && (!SaleData.weight))
					{
						if (saleUseDailyTotalFlag)
						{
							saleFuncDailyTotalSave(0, 1, SaleData.unitPrice);
						}
#ifdef USE_PRINTER
						saleEnableSumPrt = 1;
						salePrtLineFlag = 1;
						salePrtFixedItem();
#endif
						transactionEndFlag = TRUE;
						saleFuncClearUnitPrice();				// Print키로 판매가 이루어진 경우 	
						saleHalfQuartEnable = 1;
					}						
					else if (SaleData.totalPrice > 0 && (!SaleData.addCount))
					{				
						if (saleUseDailyTotalFlag)
						{
							saleFuncDailyTotalSave(0, 1, SaleData.totalPrice);
						}
#ifdef USE_PRINTER
						saleEnableSumPrt = 1;
						salePrtLineFlag = 1;
						salePrtWeightItem();
#endif
#ifdef USE_SATO_PRINTER_PROTOCOL
                        sendDLP50withParity();
#endif
						transactionEndFlag = TRUE;
						saleFuncClearUnitPrice();		// Print키로 판매가 이루어진 경우 
						saleHalfQuartEnable = 1;
#ifdef USE_MULTI_PRINT_FUNC
						if(SaleData.saleState == SALE_STATE_MULTI_PRINT) SaleData.saleState = SALE_STATE_NORMAL;
#endif
					}
					else
					{
						BuzzerSetOnTimes(2);
					}
				}
				else
				{
					DispSetError(ERROR_NUM_UNSTABLE);
				}
			}			
			break;

		case SALE_STATE_MODE:
		case SALE_STATE_MODE_PLU:
			ReportReadData(REPORT_ALL_NUM, &dailyTotalReport);
			if (dailyTotalReport.count > DAILY_TOTAL_LIMIT)
			{
				dailyTotalReport.count = 0;
				dailyTotalReport.price = 0;
			}
			if (SaleData.pluNum)
			{
				SaleData.pluNum = 0;
				tempPluData.tare = 0;	//for test kkanno 070605
			}
			SaleData.saleState = SALE_STATE_MODE_SUM;
			break;

		case SALE_STATE_SUM_TTP:
			SaleData.totalPrice = SaleData.addPrice;
			saleReceiveMoney = 0L;
			SaleData.saleState = SALE_STATE_PAY;
			break;

		case SALE_STATE_PAY:
			if (saleReceiveMoney >= SaleData.totalPrice)
			{
				saleChangeMoney = saleReceiveMoney - SaleData.totalPrice;

#ifdef USE_PRINTER
				saleEnableSumPrt = 1;
				if(scaleUsePrt == PRT_DEP50)
				{
					salePrtSum();
					salePrtPayment();
				}
#endif
				if (saleChangeMoney > 0)
				{
					SaleData.saleState = SALE_STATE_PAY_CHANGE;
				}
				else
				{		
					SaleData.unitPrice = 0L;
					SaleData.addPrice = 0;
					SaleData.addCount = 0;
					SaleData.pluNum = 0;
					SaleData.saleState = SALE_STATE_NORMAL;	
				}			
			}
			else if (saleReceiveMoney == 0)
			{
#ifdef USE_PRINTER
				saleEnableSumPrt = 1;
				if(scaleUsePrt == PRT_DEP50) salePrtSum();
#endif
				saleReceiveMoney = SaleData.totalPrice;
#ifdef USE_PRINTER
				if(scaleUsePrt == PRT_DEP50) salePrtPayment();
#endif
				SaleData.unitPrice = 0L;
				SaleData.addPrice = 0;
				SaleData.addCount = 0;
				SaleData.pluNum = 0;
				SaleData.saleState = SALE_STATE_NORMAL;				
			}
			else
			{
				DispSetError(ERROR_NUM_LACK_PAYMENT);
				SaleData.saleState = SALE_STATE_NORMAL;
			}
			break;

		case SALE_STATE_PAY_CHANGE:
			SaleData.unitPrice = 0L;
			SaleData.addPrice = 0;
			SaleData.addCount = 0;
			SaleData.pluNum = 0;		
			saleChangeMoney = 0;
			SaleData.saleState = SALE_STATE_NORMAL;
			break;

		case SALE_STATE_MODE_SUM:
			SaleData.unitPrice = 0L;
			SaleData.saleState = SALE_STATE_NORMAL;
			break;

		case SALE_STATE_FIXED_PRICE:
			SaleData.unitPrice = 0L;
			SaleData.pluNum = 0;
			SaleData.saleState = SALE_STATE_NORMAL;
			break;

		case SALE_STATE_MULTI:
			if (!saleMultiCount || (SaleData.addCount))
			{
				BuzzerSetOnTimes(2);
				break;
			}			
			if (SaleData.unitPrice * saleMultiCount > TOTALPRICE_MAX)
			{
				saleMultiCount = 0;			
			}
			else
			{
				if (saleUseDailyTotalFlag)
				{
					saleFuncDailyTotalSave(0, 1, SaleData.totalPrice);
				}
#ifdef USE_PRINTER
				saleEnableSumPrt = 1;
				salePrtLineFlag = 1;
				if(scaleUsePrt == PRT_DEP50) salePrtMultiItem();	
				saleMultiCount = 0;			//add kkanno 080326				
#endif
				saleFuncClearUnitPrice();		// Print키로 판매가 이루어진 경우 
				saleHalfQuartEnable = 1;
				SaleData.saleState = SALE_STATE_NORMAL;
			}			
			break;

		default:			
			BuzzerSetOnTimes(2);
			break;
	}
}

/**
********************************************************************************
* @brief    Double Zero Key 함수    
* @param    none
* @return   none
* @remark   각상태에 따라 원하는 값에 "00"를 추가한다.
********************************************************************************
*/
void saleDoubleZeroKey(void)
{
	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:
			SaleData.unitPrice = EditorInsDecDigit(SaleData.unitPrice, KEY_NUM_0, PRICE_SIZE, UNITPRICE_MAX, FALSE);
			SaleData.unitPrice = EditorInsDecDigit(SaleData.unitPrice, KEY_NUM_0, PRICE_SIZE, UNITPRICE_MAX, FALSE);
			break;

		case SALE_STATE_PAY:
		case SALE_STATE_CANCEL:
			saleReceiveMoney = EditorInsDecDigit(saleReceiveMoney, KEY_NUM_0, PRICE_SIZE, UNITPRICE_MAX, FALSE); 
			saleReceiveMoney = EditorInsDecDigit(saleReceiveMoney, KEY_NUM_0, PRICE_SIZE, UNITPRICE_MAX, FALSE); 
			break;

		default : 
			BuzzerSetOnTimes(2);
			break;
	}
}

#ifdef	USE_DECIMAL_POINT
/**
********************************************************************************
* @brief    Decimal Point Key 함수    
* @param    none
* @return   none
* @remark   소수점 키를 누르면 동작하는 함수 
                  키 눌림을 알려주는 플래그를 ON시키고 소수점 자리 다음에 숫자가 입력되록
                  위치 지정해주는 숫자 위치 플래그 설정함.
********************************************************************************
*/
void saleDecimalPointKey(void) // csh 20120731
{
#ifdef	USE_USER_SETTING_FOR_DOUBLEZERO_BATTLED	
	INT8U usedoublezero;
	usedoublezero = ParamReadByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BLINK_COUNT);
	if(usedoublezero & 0x01)	saleDoubleZeroKey(); // Use double zero[00] key
	else // Use [Decimal Point] key : default
	{
		switch (SaleData.saleState)
		{
			case SALE_STATE_NORMAL:
			case SALE_STATE_PAY:
			case SALE_STATE_CANCEL:
				if (SaleDecimalPointKeyOnOff == ON) break;
				SaleDecimalPointKeyOnOff = ON;
				SaleNumberPositonFlag = SaleData.priceDecPos;
				SaleNumberPositonFlag--;
				break;
		
			default : 
				BuzzerSetOnTimes(2);
				break;
		}
	}
#else
	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:
		case SALE_STATE_PAY:
		case SALE_STATE_CANCEL:
			if (SaleDecimalPointKeyOnOff == ON) break;
			SaleDecimalPointKeyOnOff = ON;
			SaleNumberPositonFlag = SaleData.priceDecPos;
			SaleNumberPositonFlag--;
			break;
	
		default : 
			BuzzerSetOnTimes(2);
			break;
	}
#endif	
}
#endif

#ifdef USE_ADD_KEY_FOR_CERTIFICATION
/**
********************************************************************************
* @brief    ADD Key 함수    
* @param    none
* @return   none
* @remark   무게 판매와 정가판매시 Add 동작을 수행한다.			\n
*           Add는 총 1000회 까지 가능하다. 						\n
*			Add 시 Print사용 유무에 따라 현재정보가 Print 된다.
********************************************************************************
*/
void saleAddCertiKey(void)
{	
	switch (SaleData.saleState)
	{		
		case SALE_STATE_HOLD:
		case SALE_STATE_NORMAL:
		case SALE_STATE_MULTI:
			SaleData.addState = 1;
			SaleData.printerState = 1;
			break;
			
		case SALE_STATE_SUM_TTP:
		case SALE_STATE_FIXED_PRICE:
			saleAddKey();
			break;
	}
}
#endif
/**
********************************************************************************
* @brief    ADD Key 함수    
* @param    none
* @return   none
* @remark   무게 판매와 정가판매시 Add 동작을 수행한다.			\n
*           Add는 총 1000회 까지 가능하다. 						\n
*			Add 시 Print사용 유무에 따라 현재정보가 Print 된다.
********************************************************************************
*/
void saleAddKey(void)
{
	INT32U tempAddPrice;
	INT16U tempAddCnt;
		
	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:

			if (AdData.stableFlag == 0)
			{
				BuzzerSetOnTimes(2);
				break;
			}
			
			if (SaleData.addCount == 0)
			{
				saleAddCurrency = saleUseEuroFlag;
			}
			else
			{
				if (saleAddCurrency != saleUseEuroFlag)
				{					
					BuzzerSetOnTimes(2);
					return;
				}
			}

			if (SaleData.totalPrice > 0)			/* weight Item 으로 판매한 경우 */
			{		
				tempAddCnt = SaleData.addCount + 1;
				tempAddPrice = SaleData.addPrice + SaleData.totalPrice;
							
				if (tempAddPrice <= TOTALPRICE_MAX && tempAddCnt <= ADD_LIMIT_COUNT) // sele by weight	change kkanno 070615
				{	
					SaleData.addPrice = tempAddPrice;
					SaleData.addCount = SaleData.addCount + 1;					

					if (saleUseDailyTotalFlag) 			/* use daily total */
					{	 
						saleFuncDailyTotalSave(0, 1, SaleData.totalPrice);		// plu 정산도 포함
					}								

					SaleData.saleState = SALE_STATE_ADD;
#ifdef USE_PRINTER
					if (saleFirstAdd) salePrtLineFlag = 1;
					if (scaleUsePrt == PRT_DEP50)	salePrtWeightItem();
#endif
					saleHalfQuartEnable = 1;
					transactionEndFlag = TRUE;
					if (saleFirstAdd) saleFirstAdd = 0;
				}
				else
				{				
					DispSetError(ERROR_NUM_TOTAL_ADDPRICE_OVER);
					SaleData.saleState = SALE_STATE_NORMAL;
				}
			}
			else								/* Fixed  item 으로 판매한 경우 */
			{		
				if (SaleData.weight == 0 && SaleData.unitPrice > 0 )
				{				
					tempAddPrice = SaleData.addPrice + SaleData.unitPrice;
					tempAddCnt = SaleData.addCount + 1;	

					if (tempAddPrice <= TOTALPRICE_MAX && tempAddCnt <= ADD_LIMIT_COUNT)	//change kkanno 070615
					{	
						SaleData.addPrice = SaleData.addPrice + SaleData.unitPrice;
						SaleData.addCount = SaleData.addCount + 1;

						if (saleUseDailyTotalFlag) 			/* use daily total */
						{
							saleFuncDailyTotalSave(0, 1, SaleData.unitPrice);	  // plu 정산도 포함 
						}
						
						SaleData.saleState = SALE_STATE_FIXED_PRICE;	
#ifdef USE_PRINTER
						if (saleFirstAdd) salePrtLineFlag = 1;
						if (scaleUsePrt == PRT_DEP50)	salePrtFixedItem();
#endif
						if (saleFirstAdd) saleFirstAdd = 0;
						transactionEndFlag = TRUE;
						saleHalfQuartEnable = 1;
					}
					else
					{
						DispSetError(ERROR_NUM_TOTAL_ADDPRICE_OVER);
						SaleData.saleState = SALE_STATE_NORMAL;
					}					
					SaleData.unitPrice = 0; 
				}
				else
				{
					BuzzerSetOnTimes(2);		
				}
			}
			break;

		case SALE_STATE_SUM_TTP:
		case SALE_STATE_FIXED_PRICE:
			saleFuncClearUnitPrice();
			SaleData.saleState = SALE_STATE_NORMAL;
			break;

		case SALE_STATE_MULTI:
			if (!saleMultiCount)
			{
				BuzzerSetOnTimes(2);
				break;
			}
			tempAddCnt = SaleData.addCount + 1;
			tempAddPrice = SaleData.addPrice + SaleData.totalPrice;
			if ((tempAddPrice <= TOTALPRICE_MAX) && (tempAddCnt <= ADD_LIMIT_COUNT) && ((SaleData.unitPrice * saleMultiCount) <= TOTALPRICE_MAX))	//change kkanno 070615
			{					
				SaleData.addPrice = tempAddPrice;
				SaleData.addCount = SaleData.addCount + 1;

				if (saleUseDailyTotalFlag) 			/* use daily total */
				{	 
					saleFuncDailyTotalSave(0, 1, SaleData.totalPrice);		// plu 정산도 포함
				}
#ifdef USE_PRINTER
				if (saleFirstAdd) salePrtLineFlag = 1;
				salePrtMultiItem();
				saleMultiCount = 0;			//add kkanno 080326				
#endif
				if (saleFirstAdd) saleFirstAdd = 0;
				saleHalfQuartEnable = 1;
				SaleData.saleState = SALE_STATE_ADD;
			
				saleFuncClearUnitPrice();
							
				SaleData.saleState = SALE_STATE_FIXED_PRICE;
			}			
			else
			{
				DispSetError(ERROR_NUM_TOTAL_ADDPRICE_OVER);
				saleMultiCount = 0;
			}		
			break;
			
		default:
			BuzzerSetOnTimes(2);
			break;
	}
}

/**
********************************************************************************
* @brief    Mood Key 함수    
* @param    none
* @return   none
* @remark   Daily total 정산 정보와 PLU 정상정보를 확인할수 잇는 모드로 진입
*          정산 데이터는 Clear Key로 수행한다. (PLU별 정산Clear는 불가능함)
********************************************************************************
*/
void saleModeKey(void)
{
	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:
			if(saleUseDailyTotalFlag)		//add kkanno 070607
			{
				SaleData.saleState = SALE_STATE_MODE;
			}
			break;

		case SALE_STATE_MODE:
			SaleData.saleState = SALE_STATE_NORMAL;
			break;
		
		default:			
			SaleData.saleState = SALE_STATE_NORMAL;	
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SaleData.saleState);	
			break;
	}
}

/**
********************************************************************************
* @brief    Cancel Key 함수    
* @param    none
* @return   none
* @remark   기존 판매 행위를 취소하는 기능 						\n
*           Total price가 0이 될때 까지 취소 가능             	\n
*			Add 시 Print사용 유무에 따라 현재정보가 Print 된다.
********************************************************************************
*/
void saleCancelKey(void)
{
	INT16U	tempCount;
	INT32S	tempPrice;

	switch (SaleData.saleState)
	{	
		case SALE_STATE_NORMAL:			
			if (SaleData.addCount == 0)
			{
				saleAddCurrency = saleUseEuroFlag;
			}
			else
			{
				if (saleAddCurrency != saleUseEuroFlag)
				{					
					BuzzerSetOnTimes(2);
					return;
				}
			}
			if (SaleData.addCount > 0 && AdData.stableFlag)
			{								
				if (SaleData.totalPrice > 0 && SaleData.totalPrice <= TOTALPRICE_MAX)  // weight cancel _print
				{					
					tempCount = SaleData.addCount - 1;
					tempPrice = SaleData.addPrice - SaleData.totalPrice;
					
					if (((tempCount == 0) && (tempPrice == 0)) || (tempCount > 0 && tempPrice > 0))
					{						
						SaleData.addPrice = SaleData.addPrice - SaleData.totalPrice;
						SaleData.addCount = SaleData.addCount - 1;
						if(SaleData.addCount==0)	saleFirstAdd=1;
#ifdef USE_PRINTER													
						salePrtWeightItemCancel();
#endif
						transactionEndFlag = TRUE;

						if (saleUseDailyTotalFlag) 			/* use daily total */
						{ 
							saleFuncDailyTotalSave(1, 1, SaleData.totalPrice);
						}
						saleFuncClearUnitPrice();
						SaleData.unitPrice = 0L;			// Cancel은 무조건 clear
					}
					else
					{
						BuzzerSetOnTimes(2);					
					}
				}
				else  // total price = 0일때  -> Fixed price cancel _print								
				{
					if (SaleData.unitPrice > 0 && SaleData.unitPrice <= SaleData.addPrice)
					{
						tempCount = SaleData.addCount - 1;
						tempPrice = SaleData.addPrice - SaleData.unitPrice;
												
						if (((tempCount == 0) && (tempPrice == 0)) || (tempCount > 0 && tempPrice > 0))
						{
							SaleData.addPrice = SaleData.addPrice - SaleData.unitPrice;
							SaleData.addCount = SaleData.addCount - 1;
							if(SaleData.addCount==0)	saleFirstAdd=1;
#ifdef USE_PRINTER							
							salePrtFixedItemCancel();
#endif
							transactionEndFlag = TRUE;
							if (saleUseDailyTotalFlag) 			/* use daily total */
							{ 
								saleFuncDailyTotalSave(1, 1, SaleData.unitPrice);
							}						
							saleFuncClearUnitPrice();
							SaleData.unitPrice = 0L;		// Cancel은 무조건 clear
						}
						else
						{
							BuzzerSetOnTimes(2);
						}
					}
					else
					{
						BuzzerSetOnTimes(2);							
					}
				}
			}
			else
			{
				BuzzerSetOnTimes(2);
			}
			break;	
		case SALE_STATE_SUM_TTP:
			saleReceiveMoney = 0;
			SaleData.saleState = SALE_STATE_CANCEL;
			break;

		case SALE_STATE_CANCEL:
			if (saleReceiveMoney > 0)			// Discount 상태 
			{
				if (saleReceiveMoney < SaleData.addPrice)
				{
					SaleData.addPrice = SaleData.addPrice - saleReceiveMoney;
					SaleData.addCount = SaleData.addCount + 1;					
				   	SaleData.saleState = SALE_STATE_SUM_TTP;
					
					if (saleUseDailyTotalFlag) 			/* use daily total */
					{ 
						saleFuncDailyTotalSave(1, 0, saleReceiveMoney);
					}
#ifdef USE_PRINTER					
					salePrtDiscountItem();
#endif
				}
				else
				{
					BuzzerSetOnTimes(2);
				}
			}
			else
			{
				SaleData.saleState = SALE_STATE_NORMAL;
			}
			break;
		
		case SALE_STATE_MODE_PLU:
		case SALE_STATE_MODE_SUM:
		case SALE_STATE_MODE_PLU_NUM:
		case SALE_STATE_EURO_INPUT:
			BuzzerSetOnTimes(2);
			break;
			
		default:
			BuzzerSetOnTimes(2);
			break;
	}	
}

/**
********************************************************************************
* @brief    Multi Key 함수    
* @param    none
* @return   none
* @remark   정가 판매시 다수 판매 기능을 수행				\n
*			Add 시 Print사용 유무에 따라 현재정보가 Print 된다.
********************************************************************************
*/
void saleMultiKey(void)
{
	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:
			if (SaleData.totalPrice == 0 && SaleData.unitPrice > 0)
			{
				saleMultiCount = 0;
				SaleData.saleState = SALE_STATE_MULTI;
			}
#ifdef USE_MULTI_PRINT_FUNC
			else if(SaleData.totalPrice > 0 && scaleUseMultiPrintFlag)
			{
				saleMultiPrintCount = 0;
				SaleData.saleState = SALE_STATE_MULTI_PRINT;
			}
#endif
			else
			{
				BuzzerSetOnTimes(2);
			}
			break;
		case SALE_STATE_MULTI:
			SaleData.saleState = SALE_STATE_NORMAL;
			break;
		
		default:
			BuzzerSetOnTimes(2);
			break;
	}
}

/**
********************************************************************************
* @brief    Percent Tare 값을 설정하는 함수    
* @param    none
* @return   none
* @remark   Tare값 Unit price, Tare %값을 입력받을수 있음
********************************************************************************
*/
void salePercentTareSetKey(void)  // data write 
{	
	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:			
			saleFuncReadPercentTareData();
			SaleData.saleState = SALE_STATE_PERCENT_TARE;
			break;
		
		case SALE_STATE_PERCENT_TARE:
			if (AdCheckPTInterval(salePercentTare) == ERR_NONE)
			{	
				SaleData.saleState = SALE_STATE_PERCENT_PRICE;
			}
			else
			{
				BuzzerSetOnTimes(2);
				saleFuncReadPercentTareData();	
			}
			break;
		
		case SALE_STATE_PERCENT_PRICE:		
			SaleData.saleState = SALE_STATE_PERCENT_RATE;
			break;

		case SALE_STATE_PERCENT_RATE:	
			ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PERCENT_TARE, salePercentTare);
			ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PERCENT_PRICE, salePercentPrice);
			ParamWriteByte(PARAM_SALE_AREA_NUM,  SALE_PARAM_OFFSET_PERCENT_RATE, salePercentRate);
			SaleData.saleState = SALE_STATE_NORMAL;
			break;

		default:
			SaleData.saleState = SALE_STATE_NORMAL;
			break;		
	}	
}


/**
********************************************************************************
* @brief    Percent Tare 값을 호출 하는 함수    
* @param    none
* @return   none
* @remark   입력된 Tare값과 unit price, %Tare 값을 호출한후 사용및 사용유무 결정  	\n
*           기존 Tare값을 무시되고 현재 저장된 Tare값이 사용됨
********************************************************************************
*/
void salePercentTareCallKey(void) 
{	
	saleFuncReadPercentTareData();

	if (salePercentTareCallFlag == OFF  && AdData.tareWeight == 0)		 
	{								
		DispSetConstStrMsg(WIN_WEIGHT, " PRCENT");
		DispSetConstStrMsg(WIN_UNIT_PRICE, " TARE ");
		DispSetConstStrMsg(WIN_TOTAL_PRICE, "  ON  ");
		DispSetRunMsgPage(10, 2);				
		SaleData.unitPrice = salePercentPrice;

		AdSetPT(salePercentTare, 1);
		AdSetPercentTare(salePercentRate);			

		salePercentTareCallFlag = ON;				
	}
	else
	{			
		DispSetConstStrMsg(WIN_WEIGHT, " PRCENT");
		DispSetConstStrMsg(WIN_UNIT_PRICE, " TARE ");
		DispSetConstStrMsg(WIN_TOTAL_PRICE, " OFF  ");
				
		SaleData.unitPrice = 0;
		salePercentTare = 0;
		DispSetRunMsgPage(10, 2);

		AdSetPT(0, 1);
		AdSetPercentTare(0);			  
		
		salePercentTareCallFlag = OFF;
	}							
}

/**
********************************************************************************
* @brief    Prepack Key 함수    
* @param    none
* @return   none
* @remark   Prepack 기능을 수행하는 key							\n
*           Unit Price를 판매후에도 clear하지 않음
********************************************************************************
*/

void salePrePackKey(void)
{
	if (salePrePackFlag == 0)
	{
		salePriceClearFlag = OFF;	// unit price save
		salePrePackFlag = 1;	
	}
	else
	{
		salePrePackFlag = 0;

		salePriceClearFlag = ON;    // unit price clear
		SaleData.unitPrice = 0;
		SaleData.pluNum = 0;	
	}
}

/**
********************************************************************************
* @brief    Tare save Key 함수    
* @param    none
* @return   none
* @remark   Tare save 기능을 수행하는 key							\n
*           Tare 값을 를 판매후에도 clear하지 않음
********************************************************************************
*/
void saleTareSaveKey(void)
{
	if (scaleTareClearFlag == 0)			// Scale doesn't use tare clear	
	{
		saleTareClearFlag = 0;
	}
	else								// Scale possible setting TareClear
	{
		if (saleTareClearFlag)
		{
			saleTareClearFlag = FALSE;
		}
		else
		{
			saleTareClearFlag = TRUE;
		}
	}
}

/**
********************************************************************************
* @brief    Both(unit price, tare) save Key 함수    
* @param    none
* @return   none
* @remark   unit price값과 Tare save 기능을 수행하는 key					\n
*           Unit Price, tare 를 판매후에도 clear하지 않음
********************************************************************************
*/
void saleBothSaveKey(void)
{
	if (salePriceClearFlag)
	{
		salePriceClearFlag = FALSE;	// unit price save	
	}
	else
	{		
		salePriceClearFlag = TRUE;    // unit price clear
		SaleData.unitPrice = 0;
		SaleData.pluNum = 0;	
	}
	
	if (scaleTareClearFlag == 0)			// Scale doesn't use tare clear	
	{
		saleTareClearFlag = 0;
	}
	else								// Scale possible setting TareClear
	{
		if (saleBothSaveFlag)
		{
			saleBothSaveFlag = OFF;
		}
		else
		{
			saleBothSaveFlag = ON;
		}
	}
}


/**
********************************************************************************
* @brief    print 사용유무를 결정하는 Key
* @param    none
* @return   none
* @remark   print 사용유무를 결정하는 key로 사용됨 							
********************************************************************************
*/
void salePrintEnableKey(void)
{
/*   //PRT_RESERVED -> PRT_STREAM 으로 변경됨.
	if (scaleUsePrt == PRT_RESERVED)
	{
		salePrtEnableFlag = 0;
		BuzzerSetOnTimes(2);
	}
	else
	{
*/
		if (SaleData.saleState == SALE_STATE_NORMAL)
		{
			if (salePrtEnableFlag)
			{					
				salePrtEnableFlag = 0;
				DispSetConstStrMsg(WIN_WEIGHT, " P OFF ");
			}
			else
			{
				salePrtEnableFlag = 1;
				DispSetConstStrMsg(WIN_WEIGHT, " P ON");			
			}
			DispSetRunMsgPage(10, 1);
		}
		else
		{
			BuzzerSetOnTimes(2);	
		}
//	}
}

/**
********************************************************************************
* @brief    unit 변환 Key 함수    
* @param    none
* @return   none
* @remark   kg, lb, oz, g 순으로 단위가 변경됨 					\n
*           현재 저울에서 사용가능한 단위는 C-4 table 참조 		\n
********************************************************************************
*/
void saleUnitChangeKey(void)
{
	INT8U unitIndex[4];	 // { kg, lb, oz, g}
	INT8U step;
	INT8U tempByte;

	unitIndex[3] = scaleUseGramUnitFlag;
	unitIndex[2] = scaleUseOzUnitFlag; 
	unitIndex[1] = scaleUseLbUnitFlag;
	unitIndex[0] = scaleUseKgUnitFlag;

#ifdef USE_USER_SET_PLU_TARE
	tempByte = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST);			
	tempByte = (tempByte & 0x40)>>6;
		
	if ((SaleData.saleState == SALE_STATE_NORMAL || SaleData.saleState == SALE_STATE_UNITCHANGE) 
		&& AdData.tareWeight == 0 && saleHundredGramEnable == 0 && tempByte == 0)
#else
	if ((SaleData.saleState == SALE_STATE_NORMAL || SaleData.saleState == SALE_STATE_UNITCHANGE) 
		&& AdData.tareWeight == 0 && saleHundredGramEnable == 0)
#endif
	{	
		SaleData.unitPrice = 0;
		SaleData.totalPrice	= 0;
		SaleData.pluNum = 0;

		step = SaleData.weightUnit;	
		while (1)
		{
			step++;
			step %= 4;

			if (unitIndex[step] == 1)
			{
				if (AdSetWeightUnitFlag(step)) 
				{
					break;
				}
			}
		}
	}
	else
	{
		BuzzerSetOnTimes(2);
	}
}

/**
********************************************************************************
* @brief    unit 변환 Key 함수    
* @param    none
* @return   none
* @remark   kg, lb, oz, g 순으로 단위가 변경됨 					\n
*           현재 저울에서 사용가능한 단위는 C-4 table 참조 		\n
********************************************************************************
*/
void saleHalfKey(void)
{	
	if(SaleData.unitPrice && saleHalfQuartEnable)
	{
		if(SaleData.unitPrice * 2 <= UNITPRICE_MAX)
		{
			if (SaleData.saleState == SALE_STATE_NORMAL)
			{	
				SaleData.unitPrice *= 2;		
				DispSetConstStrMsg(WIN_WEIGHT, "HALF");
				DispSetRunMsgPage(10, 1);
				saleHalfQuartEnable = 0;
			}
		}
		else
		{
			DispSetError(ERROR_NUM_HALF_QUARTER_OVER);			
		}
	}
	else
	{
		BuzzerSetOnTimes(2);
	}
}

/**
********************************************************************************
* @brief    unit 변환 Key 함수    
* @param    none
* @return   none
* @remark   kg, lb, oz, g 순으로 단위가 변경됨 					\n
*           현재 저울에서 사용가능한 단위는 C-4 table 참조 		\n
********************************************************************************
*/
void saleQuarterKey(void)
{	
	if(SaleData.unitPrice && saleHalfQuartEnable)
	{
		if(SaleData.unitPrice * 4 <= UNITPRICE_MAX)
		{
			if (SaleData.saleState == SALE_STATE_NORMAL)
			{			
				SaleData.unitPrice *= 4;		
				DispSetConstStrMsg(WIN_WEIGHT, "QUART");
				DispSetRunMsgPage(10, 1);
				saleHalfQuartEnable = 0;
			}
		}
		else
		{
			DispSetError(ERROR_NUM_HALF_QUARTER_OVER);
		}
	}
	else
	{
		BuzzerSetOnTimes(2);
	}
}

/**
********************************************************************************
* @brief    unit 변환 Key 함수    
* @param    none
* @return   none
* @remark   kg, lb, oz, g 순으로 단위가 변경됨 				\n
*           현재 저울에서 사용가능한 단위는 C-4 table 참조 		\n
********************************************************************************
*/
void saleHundredGramKey(void)
{	
	if ((SaleData.saleState == SALE_STATE_NORMAL) && (SaleData.weightUnit == UNIT_KG))
	{			
		if (saleHundredGramEnable)
		{
			saleHundredGramEnable = 0;
		}
		else
		{
			saleHundredGramEnable = 1;
		}
	}
	else
	{
		BuzzerSetOnTimes(2);
	}
}

/**
********************************************************************************
* @brief    unit 변환 Key 함수    
* @param    none
* @return   none
* @remark   kg, lb, oz, g 순으로 단위가 변경됨 					\n
*           현재 저울에서 사용가능한 단위는 C-4 table 참조 		\n
********************************************************************************
*/
void saleDispTestKey(void)
{
	INT8U key;
	
	while (1)
	{
		DispTest(8);
		KeyEnable(ON);	
		if (KeyCheck())
		{
			key = KeyGetFuncCode(KEY_MODE_NORMAL);
			if (key)
			{
				return;
			}
		}
		PowerProc();
		DispProc();
	}
}

#ifdef USE_TURKEY_POS_PROTOCOL
/**
********************************************************************************
* @brief    Tare save Key 함수    
* @param    none
* @return   none
* @remark   Tare save 기능을 수행하는 key							\n
*           Tare 값을 를 판매후에도 clear하지 않음
********************************************************************************
*/
void SaleTareSaveProc(INT8U flag)
{
	if (flag == 0)			// Scale don't use tare clear	
	{
		saleTareClearFlag = FALSE;
		saleKeyCommErrFlagSet(ON);
		BuzzerSetOnTimes(1);
	}
	else if(flag == 1)								// Scale use tare clear
	{
		saleTareClearFlag = TRUE;
		saleKeyCommErrFlagSet(ON);
		BuzzerSetOnTimes(1);
	}
	else
	{
		saleKeyCommErrFlagSet(OFF);
	}
}
#endif


/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

/**
********************************************************************************
* @brief    Number입력시 Window 정보를 함께 변경할수  있는 함수
* @param    winNum:윈도우 위치,  position:소수점위치, type:소수점 형태 \n
*			num:display data, 	 thousandPt : 1000단위 comma사용 여부 
* @return   none
* @remark   DispSetDecPoint 와 DispSetNum을 합쳐놓은 함수
********************************************************************************
**/
void saleDispNumWithDecPoint(INT8U winNum, INT8U position, INT8U type, INT32S num, BOOLEAN thousandPt)	
{	
	if (thousandPt == 1)
	{
		type = 1;
	}
	DispSetDecPoint(winNum, position, type, thousandPt);
	DispSetNum(winNum, num, 0);
}

/**
********************************************************************************
* @brief    PLU 데이타 read및 오류 체크 
* @param    inputPluNum: Plu번호  
* @return   none
* @remark   Euro사용시 unitpirce는 Euro_rate에 따라 변경됨 
*			PluDirectData.unitPrice 가 PRICE_MAX보다 클경우  저장된 데이터를 초기화 \n
* 		 	unitPrice = 0;			\n
*			tareValue = 0;			\n
*			pluName = "IP-XXX"
********************************************************************************
*/	
INT8U saleFuncPluDataReadInSale(INT16U inputPluNum)
{
	INT8U i;
	INT8U j;

	if (inputPluNum <= DIRECT_PLU_MAX)
	{
		PluReadDirectPlu(inputPluNum, &PluDirectData);							
										
		if (PluDirectData.unitPrice > UNITPRICE_MAX)
		{
			PluDirectData.unitPrice = 0;
			PluDirectData.tare = 0;												
		}
		
		for (i = 0, j = 0; i < PLU_NAME_SIZE; i++)
		{
			if (PluDirectData.name[i] == 0x20) j++;
		}
		
		if (j > PLU_NAME_SIZE-1)
		{
			MakeStrWithNum(saleDefaultPluName, "IP-", inputPluNum, 6);
			saleDefaultPluName[6] = ' ';
			saleDefaultPluName[7] = ' ';
#ifdef USE_PLU_NAME_SIZE_16
			for(i = 8; i < PLU_NAME_SIZE; i++)
				saleDefaultPluName[i] = ' ';
#endif
		}
		else
		{
			saleDefaultPluName[0] = 0x00;
		}

		i = AdCheckPTInterval(PluDirectData.tare);

		if (i != ERR_OVER_RANGE && saleUsePluTareFlag)
		{				
			if (AdData.tareWeight && (saleTareType == KEY_TARE_CALL || saleTareType == WEIGHT_TARE_CALL))
			{
				DispSetError(ERROR_NUM_ALREADY_TARE);
				SaleData.pluNum = 0;
				saleFuncClearPluName();
				return FALSE;
			}	
			if((tempPluData.tare && !PluDirectData.tare) || (!AdData.tareWeight && PluDirectData.tare) || (tempPluData.tare && PluDirectData.tare))
			{
				i = AdSetPT(PluDirectData.tare, 1);
			}
			if (i != ERR_NONE)
			{ 
				DispSetError(ERROR_NUM_ALREADY_TARE);	//TODO : 다른 ERROR로 변경??
				PluDirectData.tare = 0;
			}
			else saleTareType = PLU_TARE_CALL;
		}
		else
		{
			PluDirectData.tare = 0;
		}

		if (j <= PLU_NAME_SIZE - 1)
		{
			tempPluData = PluDirectData;
		}
		else
		{
			tempPluData.tare = PluDirectData.tare;
			tempPluData.unitPrice = PluDirectData.unitPrice;
		}
	}
	else
	{
		PluReadIndirectPlu(inputPluNum, &PluInDirectData);		

		if (PluInDirectData.unitPrice > UNITPRICE_MAX)
		{
			PluInDirectData.unitPrice =0;							
		}

		tempPluData.unitPrice = PluInDirectData.unitPrice;

		MakeStrWithNum(tempPluData.name, "PLU", inputPluNum, 6);
		tempPluData.name[6] = ' ';
		tempPluData.name[7] = ' ';
#ifdef USE_PLU_NAME_SIZE_16
		for(i = 8; i < PLU_NAME_SIZE; i++)
			tempPluData.name[i] = ' ';
#endif
	}

	if (scaleUseEuroFlag && saleUseEuroFlag)
	{
		saleUseEuroFlag = 0;
		ParamReadData(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PRICE_SYMBOL, saleUsePriceUnit, PRICE_SYMBOL_SIZE);		// first unit
		DispSetConstStrMsg(WIN_WEIGHT, "NOEURO");
		DispSetDecPoint(WIN_TOTAL_PRICE, saleEuroDecPos, 0, 0);
		DispSetNumMsg(WIN_TOTAL_PRICE, saleEuroRate, 0);
		DispSetRunMsgPage(10, 3);
	}
	return TRUE;
}

/**
********************************************************************************
* @brief    DailyTotal을 저장하는 함수 
* @param    sign : 저장데이터가 Add 인지, minus인지를 설정    	\n
*			count: 변경되는 횟수의 정보							\n
*			price: 변경되는 값의 정보							\n
* @return   none
* @remark   Plu에 따른 정산일경우 PLU정산 데이터도 함께 처리함
********************************************************************************
*/	
void saleFuncDailyTotalSave(BOOLEAN sign, INT16U count, INT32U price)
{
	INT16U tempTotalCnt;
	
	if (scaleUseEuroFlag && saleUseEuroFlag)
	{
		price = saleFuncEuro2Dollar(price);
	}
	tempTotalCnt = dailyTotalReport.count + count;
						
	if (tempTotalCnt <= DAILY_TOTAL_LIMIT)// && (dailyTotalReport.price + price) <= PRICE_MAX)		//for test kkanno 070605
	{
		if (sign)
		{
			dailyTotalReport.count = dailyTotalReport.count - count;
			dailyTotalReport.price = dailyTotalReport.price - price;
		}
		else
		{
			dailyTotalReport.count = dailyTotalReport.count + count;
			dailyTotalReport.price = dailyTotalReport.price + price;			
		}
		ReportWriteData(REPORT_ALL_NUM, &dailyTotalReport);
	}						
	else
	{
		DispSetStr(WIN_TOTAL_PRICE, " OVER ");
		BuzzerSetOnTimes(1); 
	}
						
	if (SaleData.pluNum >= 1 && SaleData.pluNum <= DIRECT_PLU_MAX)
	{						
		tempTotalCnt = tempPluReport.count + count;

		if (tempTotalCnt <= DAILY_TOTAL_LIMIT)// && (dailyTotalReport.price + price) <= PRICE_MAX)		//for test kkanno 070605
		{						  
			if (sign)
			{
				tempPluReport.count = tempPluReport.count - count;
				tempPluReport.price = tempPluReport.price - price;
			}
			else
			{
				tempPluReport.count = tempPluReport.count + count;
				tempPluReport.price = tempPluReport.price + price;
			}
			ReportWriteData(SaleData.pluNum, &tempPluReport);
		}
		else
		{
			DispSetStr(WIN_TOTAL_PRICE, " OVER ");
			BuzzerSetOnTimes(1); 
		}
	}
}
/**
********************************************************************************
* @brief    Euro를 달러로 변경하는 함수  
* @param    inputPrice : 현재의 Euro     	\n
* @return   변경된 달러 값 
* @remark   
********************************************************************************
*/	
INT32U saleFuncEuro2Dollar(INT32U inputPrice)
{
	INT32U result;
	FP64 tempDouble1;
	FP64 tempDouble2;

	tempDouble1 = ((FP64)inputPrice/(FP64)DecPowerConvTable[saleEuroDispDecPos]) * ((FP64)saleEuroRate/(FP64)DecPowerConvTable[saleEuroDecPos]);

	result = (INT32U)(tempDouble1 * (FP64)DecPowerConvTable[priceDecPos]);
	tempDouble2 = (tempDouble1 * (FP64)DecPowerConvTable[priceDecPos]) - (FP64)result;
	if (tempDouble2 >= 0.5) result++;
	
	return result;
}

/**
********************************************************************************
* @brief    달러를 Euro_rate에 따라 변경하는 함수  
* @param    inputPrice : 현재의 달러 값     	\n
* @return   변경된 Euro값 
* @remark   
********************************************************************************
*/	
INT32U saleFuncDollar2Euro(INT32U inputPrice)
{
	INT32U result;
	FP64 tempDouble1;
	FP64 tempDouble2;

	if (saleEuroRate == 0)
	{
		saleEuroRate = 1;
	}

	tempDouble1 = ((FP64)inputPrice/(FP64)DecPowerConvTable[priceDecPos]) / ((FP64)saleEuroRate/(FP64)DecPowerConvTable[saleEuroDecPos]);

	result = (INT32U)(tempDouble1 * (FP64)DecPowerConvTable[saleEuroDispDecPos]);
	tempDouble2 = (tempDouble1 * (FP64)DecPowerConvTable[saleEuroDispDecPos]) - (FP64)result;
	if (tempDouble2 >= 0.5) result++;

	return result;
}

/**
********************************************************************************
* @brief    Euro를 달러로 변경하는 함수  
* @param    inputPrice : 현재의 Euro     	\n
* @return   변경된 달러 값 
* @remark   
********************************************************************************
*/	
void saleFuncReadPercentTareData(void) // percent tare data read
{
	salePercentTare  = ParamReadDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PERCENT_TARE);
	if (AdCheckPTInterval(salePercentTare) == ERR_OVER_RANGE)	
	{
		salePercentTare = 0;	
	}
	
	salePercentPrice = ParamReadDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PERCENT_PRICE);
	if (salePercentPrice > UNITPRICE_MAX)
	{
		salePercentPrice = 0;
	}
	
	salePercentRate  = ParamReadByte(PARAM_SALE_AREA_NUM,  SALE_PARAM_OFFSET_PERCENT_RATE);	
	if (salePercentRate > 100)
	{
		salePercentPrice = 0;
	}
}

/**
********************************************************************************
* @brief    모드및 sale완료후 unitprice와 Plu Number를 클리어하는 함수   
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void saleFuncClearUnitPrice(void)
{
	if (salePriceClearFlag && salePercentTareCallFlag == 0)
	{
#ifdef	USE_DECIMAL_POINT	
		SaleDecimalPointKeyOnOff = OFF; // csh 20120731
#endif
		SaleData.unitPrice = 0L;
		SaleData.pluNum = 0;					
		saleFuncClearPluName();
	}
}

/**
********************************************************************************
* @brief    모드및 sale완료후 PLU name을 클리어하는 함수   
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void saleFuncClearPluName(void)
{
	INT8U i;
	
	for (i = 0; i < PLU_NAME_SIZE; i++)	tempPluData.name[i] = 0x20;
}

/**
********************************************************************************
* @brief    PLU_SAVE Key에 따라 받아들인 Data를 저장하는 함수    
* @param    inputPluNum : 현재 변경된 plunum와 저장될 위치를 표시함 
* @return   none
* @remark   Plu name에 garbage값이 들어있으면 PLU name을 IP-xxx으로 처리함 
********************************************************************************
*/
void saleFuncPluDataWriteInSale(INT16U inputPluNum)
{
	if (inputPluNum > 0 && inputPluNum <= INDIRECT_PLU_MAX)
	{
		if (inputPluNum <= DIRECT_PLU_MAX)
		{			
			PluDirectData = tempPluData;
			tempPluData.tare = 0;
			AdSetPT(tempPluData.tare, 1);
			saleTareType = PLU_TARE_CALL;
			PluWriteDirectPlu(inputPluNum, &PluDirectData);						
		}
		else
		{
			PluInDirectData.unitPrice = tempPluData.unitPrice;
			PluWriteIndirectPlu(inputPluNum, &PluInDirectData);											
		}
	}
	else
	{
		BuzzerSetOnTimes(2);
	}	
	SaleData.pluNum = 0;
	SaleData.unitPrice = 0L;
	saleFuncClearPluName();	
	tempPluData.unitPrice = 0;
	SaleData.saleState = SALE_STATE_NORMAL;	
}

#ifdef USE_QUICK_STORE_FUNCTION
/**
********************************************************************************
* @brief    PLU_QUICK_SAVE Unit Price를 입력후 Direct PLU Key를 길게 누르면 해당 PLU Num.에 저장됨
* @param    inputkey : Direct Plu Key, PLU를 저장할 위치정보
* @return   none
* @remark   
********************************************************************************
*/
void saleFuncPluQuickStoreInSale(KEY_TYPE inputkey)
{
	INT16U tmpPluNum;
	INT32U tmpPrice;

	tmpPrice = SaleData.unitPrice;

	saleDirectPluCall(inputkey);
	tempPluData.unitPrice = tmpPrice;
	tmpPluNum = inputkey - 0x80 + 1;
	saleFuncPluDataWriteInSale(tmpPluNum);
	SaleData.unitPrice = tempPluData.unitPrice;
}

/**
********************************************************************************
* @brief    PLU_QUICK_SAVE를 위한 salePluDirectSaveFlag -> Enable
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePluDirectSaveOn(void)
{
	salePluDirectSaveFlag = TRUE;
}

/**
********************************************************************************
* @brief    PLU_QUICK_SAVE를 위한 salePluDirectSaveFlag -> Disable
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePluDirectSaveOff(void)
{
	salePluDirectSaveFlag = FALSE;
}
#endif

/**
********************************************************************************
* @brief    데이타 출력시 start line에 대한 정보     
* @param    none
* @return   none
* @remark   saleUseHeadMsgFlag = head 메세지 출력 여부를 결정함  	 		\n
*			salePrtDayFlag   = 날짜 정보 출력 여부를 결정함
********************************************************************************
*/
#ifdef USE_PRINTER
#ifdef USE_ADD_KEY_FOR_CERTIFICATION
#define WEIGHT_STR 			"WEIGHT"
#define UNIT_STR 			"UNIT PRICE"
#define TOTAL_STR 			"TOTAL"
#define FIXED_STR 			"FIXED ITEM"
#define DISCOUNT_STR		"DISCOUNT"
#define TOTAL_PRICE_STR		"TOTAL PRICE "
#define TOTAL_PRICE1_STR	"TOTAL PRICE "
#define TOTAL_PRICE2_STR	""
#define COUNT_STR			"COUNT"
#define TOTAL_PAYMENT_STR	"AMOUNT TENDERED "
#define CHANGE_STR	 		"CHANGE "
#elif defined USE_PESO_PRINT
#define WEIGHT_STR 			"PESO"
#define UNIT_STR 			" Pre\x87o/Uni"
#define TOTAL_STR 			"total"
#define FIXED_STR 			"fixed item"
#define DISCOUNT_STR		"Discount"
#define TOTAL_PRICE_STR		"total/"
#define TOTAL_PRICE1_STR	"total("
#define TOTAL_PRICE2_STR	")"
#define COUNT_STR			"Count"
#define TOTAL_PAYMENT_STR	"Payment/"
#define CHANGE_STR	 		"Remains/"
#else
#define WEIGHT_STR 			"weight"
#define UNIT_STR 			"unit price"
#define TOTAL_STR 			"total"
#define FIXED_STR 			"fixed item"
#define DISCOUNT_STR		"Discount"
#define TOTAL_PRICE_STR		"Sum/"
#define TOTAL_PRICE1_STR	"Sum("
#define TOTAL_PRICE2_STR	")"
#define COUNT_STR			"Count"
#define TOTAL_PAYMENT_STR	"Payment/"
#define CHANGE_STR	 		"Remains/"
#endif
void salePrtStartCondition(void)
{
	INT8U	tempINT8U;
	INT8U	tempStr[25];

	PrtSetTabPos(0,  0);
	PrtSetTabPos(1,  1);
	PrtSetTabPos(2, 11);
	PrtSetTabPos(3, 13);
	PrtSetTabPos(4, 14);
	PrtSetTabPos(5, 21);
	PrtSetTabPos(6, 24);	
	PrtSetTabPos(7, 25);

	if (salePrtLineFlag)
	{				
		if (saleUseHeadMsgFlag)
		{ 
			ParamReadData(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE1, tempStr, HEAD_LINE1_SIZE);
			tempStr[HEAD_LINE1_SIZE] = '\0';
			PrtPutStr(tempStr);	
			PrtPutCRLF(1);
			ParamReadData(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE2, tempStr, HEAD_LINE2_SIZE);
			tempStr[HEAD_LINE2_SIZE] = '\0';
			PrtPutStr(tempStr);
			PrtPutCRLF(1);
			ParamReadData(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE3, tempStr, HEAD_LINE3_SIZE);
			tempStr[HEAD_LINE3_SIZE] = '\0';
			PrtPutStr(tempStr);
			PrtPutCRLF(1);
		}
		if (salePrtDayFlag)
		{
			switch(saleDateType)
			{
				case 0:
				default:	//YYYY / MM / DD / DAY
					tempINT8U = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_YEAR);
					PrtPutDecNum(2000 + tempINT8U, 4, PRT_NUM_TYPE_FILL_ZERO);	PrtPutChar('/');
					tempINT8U = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_MONTH);
					PrtPutDecNum(tempINT8U, 2, PRT_NUM_TYPE_FILL_ZERO); 		PrtPutStr("/");
					tempINT8U = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_DAY);
					PrtPutDecNum(tempINT8U, 2, PRT_NUM_TYPE_FILL_ZERO);
#ifdef	USE_ADD_KEY_FOR_CERTIFICATION
					PrtPutTab(5);
					PrtPutConstStr("NO.");
					ReportReadData(REPORT_ALL_NUM, &dailyTotalReport);
					PrtPutNum(dailyTotalReport.count, 5, 0, 0, 0);
#endif
					PrtPutCRLF(1);
					break;
				case 1:	//DAY / DD / MM / YYYY
					tempINT8U = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_DAY);
					PrtPutDecNum(tempINT8U, 2, PRT_NUM_TYPE_FILL_ZERO);			PrtPutChar('/'); 		
					tempINT8U = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_MONTH);
					PrtPutDecNum(tempINT8U, 2, PRT_NUM_TYPE_FILL_ZERO); 		PrtPutStr("/");
					tempINT8U = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_YEAR);
					PrtPutDecNum(2000 + tempINT8U, 4, PRT_NUM_TYPE_FILL_ZERO);	
#ifdef	USE_ADD_KEY_FOR_CERTIFICATION
					PrtPutTab(5);
					PrtPutConstStr("NO.");
					ReportReadData(REPORT_ALL_NUM, &dailyTotalReport);
					PrtPutNum(dailyTotalReport.count, 5, 0, 0, 0);
#endif
					PrtPutCRLF(1);
					break;
			}
			
		}
	
//		PrtPutConstStrWithCRLF("1t3456789_t2tt56789_t23tt6789_");
//		PrtPutConstStrWithCRLF(" weight   unit price  total  ");		
		PrtPutLine();
		PrtPutTab(1);	// Teb 1	  
		PrtPutConstStr(WEIGHT_STR);
		PrtPutTab(1);	// Teb 2
		PrtPutConstStr(UNIT_STR);
		PrtPutTab(4);	// Teb 3
		PrtPutConstStr(TOTAL_STR);
		PrtPutCRLF(1);	

//		PrtPutConstStrWithCRLF("1t3456789_t2tt56789_t23tt6789_");
//		PrtPutConstStrWithCRLF(" (Kg)        \/Kg       \     ");			
		PrtPutTab(1);
		PrtPutConstStr("(");			
		if(saleMultiCount) PrtPutConstStr("pcs");
		else PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);		
		PrtPutConstStr(")");
		PrtPutTab(3);	
		PrtPutConstStr("(");	PrtPutStr(saleUsePriceUnit);	PrtPutConstStr("/");
		if (salePriceForUnit == 2 )
		{
			PrtPutNum(DecPowerConvTable[salePriceForUnit], 3, 0, 0, 0);
			PrtPutConstStr("g");		
		}
		else
		{
			if(saleMultiCount) PrtPutConstStr("pcs");
			else
			{
				if(saleHundredGramEnable || saleHundredGramPrice) PrtPutConstStr("100g");
				else PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);
			}
		}	
		PrtPutConstStr(")");  
		PrtPutTab(2);	// Teb 3
		PrtPutConstStr("(");	PrtPutStr(saleUsePriceUnit);	PrtPutConstStr(")");  
		PrtPutCRLF(1);
		PrtPutLine();
		salePrtLineFlag = 0;
	}
}

/**
********************************************************************************
* @brief    데이타 출력시 weight item에 대한 판매 정보 출력     
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtWeightItem(void)
{	
	char pBuf[1];
	char sendByte;

	if (salePrtEnableFlag)
	{		
		if(scaleUsePrt == PRT_DEP50)
		{
			salePrtStartCondition();			
			if (saleUsePluNameFlag)
			{
				salePrtPluName();
			}
			PrtPutTab(1);		// Teb 2
				PrtPutNum(SaleData.weight, 7, SaleData.weightDecPos, weightDotTypeFlag, 0);
			PrtPutTab(1);
			if (saleUseEuroFlag) PrtPutNum(SaleData.unitPrice, 9, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			else PrtPutNum(SaleData.unitPrice, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			PrtPutTab(3);		// Teb 3
			if (saleUseEuroFlag) PrtPutNum(SaleData.totalPrice, 9, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			else PrtPutNum(SaleData.totalPrice, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			PrtPutCRLF(1);
			if (saleEnableSumPrt)
			{
				PrtPutLine();
				PrtPutConstStr(TOTAL_PRICE1_STR);	PrtPutStr(saleUsePriceUnit);	PrtPutConstStr(")");
				PrtPutTab(5);		// Teb 3
				if (saleUseEuroFlag) PrtPutNum(SaleData.totalPrice, 9, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
				else PrtPutNum(SaleData.totalPrice, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
				PrtPutCRLF(9);
				saleEnableSumPrt = 0;
			}	
		}		
		else if (scaleUsePrt == PRT_DLP50)
		{
			pBuf[0] = LF;
			salePrtDlpStartCondition(TYPE_NONE);										//Start command
			salePrtDlp(WIN_WEIGHT);											//V00 : Weight
			PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);					//V01 : Weight Unit
			PrtPutData(pBuf, 1);	//LF			
			salePrtDlp(WIN_UNIT_PRICE);										//V02 : Unit Price
			salePrtDlp(WIN_TOTAL_PRICE);										//V03 : Total Price
			PrtPutDecNum(SaleData.weight, 5, PRT_NUM_TYPE_FILL_ZERO);		//V04 : Weight(Barcode)
			PrtPutData(pBuf, 1);	//LF			
			PrtPutDecNum(SaleData.unitPrice, 6, PRT_NUM_TYPE_FILL_ZERO);		//V05 : Unit Price(Barcode)
			PrtPutData(pBuf, 1);	//LF			
			PrtPutDecNum(SaleData.totalPrice, 6, PRT_NUM_TYPE_FILL_ZERO);		//V06 : Total Price(Barcode)
			PrtPutData(pBuf, 1);	//LF			
			salePrtDlpPluName();												//V07 : PLU name
																			//V08 : PLU number
			PrtPutData(salePriceUnit[saleUseEuroFlag], 1);						//V09 : unit unit
			pBuf[0] = '/';
			PrtPutData(pBuf, 1);
			pBuf[0] = LF;
			PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);
			PrtPutData(pBuf, 1);	//LF			
			PrtPutData(salePriceUnit[saleUseEuroFlag], 1);						//V10 : total price unit
			PrtPutData(pBuf, 1);	//LF			
			//Add location V11 or more...
			//Add location V11 or more...
#ifndef USE_MULTI_PRINT_FUNC
			PrtPutData("P1,1", 4);												//End command		
#else
			PrtPutData("P1,", 3);	//wcm1103											//End command		
			if(saleMultiPrintCount == 0) saleMultiPrintCount = 1;
			PrtPutDecNum(saleMultiPrintCount, 3, PRT_NUM_TYPE_FILL_ZERO);
			saleMultiPrintCount = 0;
#endif
			PrtPutData(pBuf, 1);	//LF			
		}
#if defined (USE_REAL_STREAM)
		else if(scaleUsePrt == PRT_STREAM) //PRT_RESERVED -> PRT_STREAM 으로 신규기능 추가.
		{
			PrtPutNum(SaleData.weight, 7, SaleData.weightDecPos, weightDotTypeFlag, 0);

			if(AdData.overloadFlag) sendByte = 0x4F;		//"O", Over weight
			else if(SaleData.weight < 0) sendByte = 0x2D;		//"-", Negative
			else {
				if(AdData.stableFlag) sendByte = 0x53;		//"S", Stable
				else sendByte = 0x55;		//"U", Unstable
			}
			
			PrtPutData(&sendByte, 1);
			
		}
#elif defined (USE_PRII_SPAIN_CO_PROTOCOL)
		else if(scaleUsePrt == PRT_RS_CO) {
			INT16U currTimer100ms = TimerGetSysTimer100ms();
			static INT16U prevTimer100ms;

			if ((INT16U)(currTimer100ms - prevTimer100ms) > 0) {
				char strBuf[12] = {0};
				INT32S current_weight = AdData.weightData;

				prevTimer100ms = currTimer100ms;

				if (AdData.stableFlag) {
					if (current_weight < 0) {
						Memset(strBuf, '-', 7);
					} else if (current_weight == 0) {
						Memset(strBuf, '0', 7);
					} else if (AdData.overloadFlag) {
						Memset(strBuf, 'F', 7);
					} else {
						Int32sToDecStr(strBuf, AbsInt32s(current_weight), 7, '0', AdData.weightDecPos, '.', 0);
					}
				} else {
					Memset(strBuf, '-', 7);
				}
				strBuf[7] = ASCII_CR;

				PrtPutStr(strBuf);
			}
		}
#endif
#ifdef USE_PRII_SPAIN_ST_PROTOCOL
		else if(scaleUsePrt == PRT_RS_ST) {
			static INT32S previous_weight;
			INT32S current_weight;

			if (!AdData.stableFlag)
				return;
			
			current_weight = AdData.weightData;

			if (AdExtToIntValueExtern(AbsInt32s(previous_weight - current_weight)) < AdData.d1 * 20)
				return;
			
			if (previous_weight == 0) {
				char strBuf[12] = {0};

				if (current_weight < 0) {
					Memset(strBuf, '-', 7);
				} else if (current_weight == 0) {
					return;
				} else if (AdData.overloadFlag) {
					Memset(strBuf, 'F', 7);
				} else {
					Int32sToDecStr(strBuf, AbsInt32s(current_weight), 7, '0', AdData.weightDecPos, '.', 0);
				}
				strBuf[7] = ASCII_CR;

				PrtPutStr(strBuf);
			}

			previous_weight = current_weight;
		}
#endif /* USE_PRII_SPAIN_ST_PROTOCOL */
	}
}

/**
********************************************************************************
* @brief    데이타 출력시 Fixed_value item에 대한 판매 정보 출력     
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtFixedItem(void)
{
	char	pBuf[1];
//		PrtPutConstStrWithCRLF("1t3456789_t2tt56789_t23tt6789_");
	if (salePrtEnableFlag)
	{
		if(scaleUsePrt == PRT_DEP50)
		{
			salePrtStartCondition();
			if (saleUsePluNameFlag)
			{
				salePrtPluName();
			}
			PrtPutTab(1);
			PrtPutConstStr(FIXED_STR);
			PrtPutTab(4);
			if (saleUseEuroFlag) PrtPutNum(SaleData.unitPrice, 9, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			else PrtPutNum(SaleData.unitPrice, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			PrtPutCRLF(1);
			
			if (saleEnableSumPrt)
			{
				PrtPutLine();
				PrtPutConstStr(TOTAL_PRICE1_STR);	PrtPutStr(saleUsePriceUnit);	PrtPutConstStr(TOTAL_PRICE2_STR);
				PrtPutTab(5);		// Teb 3
				if (saleUseEuroFlag) PrtPutNum(SaleData.unitPrice, 9, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
				else PrtPutNum(SaleData.unitPrice, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
				PrtPutCRLF(9);
				saleEnableSumPrt = 0;
			}
		}
		else if (scaleUsePrt == PRT_DLP50)
		{

			pBuf[0] = LF;
			salePrtDlpStartCondition(TYPE_NONE);										//Start command
			salePrtDlp(WIN_WEIGHT);											//V00 : Weight
			PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);					//V01 : Weight Unit
			PrtPutData(pBuf, 1);	//LF			
			salePrtDlp(WIN_UNIT_PRICE);										//V02 : Unit Price
			salePrtDlp(WIN_UNIT_PRICE);										//V03 : Total Price
			PrtPutDecNum(SaleData.weight, 5, PRT_NUM_TYPE_FILL_ZERO);		//V04 : Weight(Barcode)
			PrtPutData(pBuf, 1);	//LF			
			PrtPutDecNum(SaleData.unitPrice, 6, PRT_NUM_TYPE_FILL_ZERO);		//V05 : Unit Price(Barcode)
			PrtPutData(pBuf, 1);	//LF			
			PrtPutDecNum(SaleData.totalPrice, 6, PRT_NUM_TYPE_FILL_ZERO);		//V06 : Total Price(Barcode)
			PrtPutData(pBuf, 1);	//LF			
			salePrtDlpPluName();												//V07 : PLU name
																			//V08 : PLU number
			PrtPutData(salePriceUnit[saleUseEuroFlag], 1);						//V09 : unit unit
			pBuf[0] = '/';
			PrtPutData(pBuf, 1);
			pBuf[0] = LF;
			PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);
			PrtPutData(pBuf, 1);	//LF			
			PrtPutData(salePriceUnit[saleUseEuroFlag], 1);						//V10 : total price unit
			PrtPutData(pBuf, 1);	//LF			
			//Add location V11 or more...
			//Add location V11 or more...
			PrtPutData("P1,1", 4);												//End command		
			PrtPutData(pBuf, 1);	//LF			
		}
	}
}

/**
********************************************************************************
* @brief    데이타 출력시 Mul_ti item에 대한 판매 정보 출력     
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtMultiItem(void)
{
	if (salePrtEnableFlag)
	{
		salePrtStartCondition();
//		PrtPutConstStrWithCRLF("1t3456789_t2tt56789_t23tt6789_");
//		PrtPutConstStrWithCRLF(" Fixed Item         xxxxxxxxxx");
		PrtPutTab(1);
		PrtPutConstStr(FIXED_STR);
		PrtPutCRLF(1);

//		PrtPutConstStrWithCRLF("1t3456789_t2tt56789_t23tt6789_");
//		PrtPutConstStrWithCRLF(" xxxxxx   X         xxxxxxx,xx");
		PrtPutTab(1);
		PrtPutNum(saleMultiCount, 6, 0, 0, 0);
		PrtPutConstStr(" X");
		PrtPutTab(1);
		if (saleUseEuroFlag) PrtPutNum(SaleData.unitPrice, 9, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		else PrtPutNum(SaleData.unitPrice, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		PrtPutTab(3);
		if (saleUseEuroFlag) PrtPutNum(SaleData.totalPrice, 9, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		else PrtPutNum(SaleData.totalPrice, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		PrtPutCRLF(1);
		if (saleEnableSumPrt)
		{
			PrtPutLine();
			PrtPutConstStr(TOTAL_PRICE1_STR); PrtPutStr(saleUsePriceUnit);	PrtPutConstStr(TOTAL_PRICE2_STR);
			PrtPutTab(5);		// Teb 3
			if (saleUseEuroFlag) PrtPutNum(SaleData.totalPrice, 9, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			else PrtPutNum(SaleData.totalPrice, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			PrtPutCRLF(9);
			saleEnableSumPrt = 0;
		}
	}
}


/**
********************************************************************************
* @brief    데이타 출력시 Discount item에 대한 판매 정보 출력     
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtDiscountItem(void)
{
//		PrtPutConstStrWithCRLF("1t3456789_t2tt56789_t23tt6789_");
//		PrtPutConstStrWithCRLF(" Discount   -       xxxxxxx,xx");
	if (salePrtEnableFlag)
	{
		PrtPutTab(1);
		PrtPutConstStr(DISCOUNT_STR);
		PrtPutTab(4);
		PrtPutNum(-saleReceiveMoney, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);	
		PrtPutCRLF(1);
	}
}

/**
********************************************************************************
* @brief    데이타 출력시 weight item 에 대한 void 정보 출력     
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtWeightItemCancel(void)
{
//		PrtPutConstStrWithCRLF("1t3456789_t2tt56789_t23tt6789_");
//		PrtPutConstStrWithCRLF(" (CANCEL)  PLU-xx :           ");
	if (salePrtEnableFlag)
	{
		salePrtStartCondition();		

		PrtPutTab(1);
		PrtPutConstStr("(CANCEL)  ");
		if (saleUsePluNameFlag)
		{
			PrtPutCRLF(1);	
			salePrtPluName();
		}
		else
		{
			PrtPutCRLF(1);	
		}

//		PrtPutConstStrWithCRLF("1t3456789_t2tt56789_t23tt6789_");
//		PrtPutConstStrWithCRLF(" xxxxx.xx xxxxxx.xx  xxxxxx.xx");
		PrtPutTab(1);
			PrtPutNum(SaleData.weight, 7, SaleData.weightDecPos, weightDotTypeFlag, 0);
		PrtPutTab(1);
		if (saleUseEuroFlag) PrtPutNum(SaleData.unitPrice, 9, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		else PrtPutNum(SaleData.unitPrice, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		PrtPutTab(3);
		if (saleUseEuroFlag) PrtPutNum(-SaleData.totalPrice, 9, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		else PrtPutNum(-SaleData.totalPrice, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		PrtPutCRLF(1);	
	}
}


/**
********************************************************************************
* @brief    데이타 출력시 Fixed_price item 에 대한 void 정보 출력     
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtFixedItemCancel(void)
{

	if (salePrtEnableFlag == 1)
	{
		salePrtStartCondition();
//		PrtPutConstStrWithCRLF("1t3456789_t2tt56789_t23tt6789_");
//		PrtPutConstStrWithCRLF(" (CANCEL)  PLU-xx :           ");  
		
		PrtPutTab(1);
		PrtPutConstStr("(CANCEL)");
		if (saleUsePluNameFlag)
		{
			PrtPutCRLF(1);
			salePrtPluName();
		}
		else
		{
			PrtPutCRLF(1);
		}

//		PrtPutConstStrWithCRLF("1t3456789_t2tt56789_t23tt6789_");
//		PrtPutConstStrWithCRLF(" Fixed Item         xxxxxxx.xx");
		PrtPutTab(1);
		PrtPutConstStr(FIXED_STR);
		PrtPutTab(4);
		if (saleUseEuroFlag) PrtPutNum(-SaleData.unitPrice, 9, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		else PrtPutNum(-SaleData.unitPrice, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		PrtPutCRLF(1);
	}
}

/**
********************************************************************************
* @brief    데이타 출력시 Print number 출력     
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtPluName(void)
{
	if (SaleData.pluNum)
	{
//		PrtPutConstStrWithCRLF("1t3456789_t23456789_123t56789_");
//		PrtPutConstStrWithCRLF("PLU_XX : Name                 ");
		if (SaleData.pluNum <= DIRECT_PLU_MAX)
		{
			PrtPutConstStr("PLU_");
			PrtPutNum(SaleData.pluNum, 3, 0, 0, 0);
			PrtPutConstStr(" : ");	
			if(saleDefaultPluName[0] != 0x00) PrtPutData(saleDefaultPluName, PLU_NAME_SIZE);
			else PrtPutData(tempPluData.name, PLU_NAME_SIZE);
		}
		else
		{
			PrtPutConstStr("Indirect PLU No: ");
			PrtPutNum(SaleData.pluNum, 3, 0, 0, 0);	
		}
		
		PrtPutCRLF(1);				 
	}			
	else
	{
		PrtPutConstStr("NO_PLU");	
		PrtPutCRLF(1);
	}	
}

/**
********************************************************************************
* @brief    데이타 출력시 SUM_data 출력     
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtSum(void)
{	
	if(salePrtEnableFlag)
	{
		PrtPutLine();
	//	PrtPutConstStrWithCRLF("1t3456789_t23456789_t23t56789_");
	//	PrtPutConstStrWithCRLF(" Sum/               xxxxxxx.xx");
		PrtPutTab(1);
		PrtPutConstStr(TOTAL_PRICE_STR);  	
		PrtPutStr(saleUsePriceUnit);
		PrtPutTab(4);   
		if (saleUseEuroFlag) PrtPutNum(SaleData.addPrice, 9, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		else PrtPutNum(SaleData.addPrice, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		PrtPutCRLF(1);

	//	PrtPutConstStrWithCRLF("1t3456789_t23456789_t23t56789_");
	//	PrtPutConstStrWithCRLF(" Count              xxxxxxx.xx");
		PrtPutTab(1);
		PrtPutConstStr(COUNT_STR);
		PrtPutTab(6); 
		PrtPutNum(SaleData.addCount, 5, 0, 0, 0);
		PrtPutCRLF(1);
	}
}

/**
********************************************************************************
* @brief    데이타 출력시 Payment_Data 출력     
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtPayment(void)
{
	if(salePrtEnableFlag)
	{
	//	PrtPutConstStrWithCRLF("1t3456789_t23456789_t23t56789_");
	//	PrtPutConstStrWithCRLF(" Payment/\          xxxxxxx.xx");
		PrtPutTab(1);
		PrtPutConstStr(TOTAL_PAYMENT_STR);
		PrtPutStr(saleUsePriceUnit);
		PrtPutTab(4);	
		if (saleUseEuroFlag) PrtPutNum(saleReceiveMoney, 9, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		else PrtPutNum(saleReceiveMoney, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		PrtPutCRLF(1);

	//	PrtPutConstStrWithCRLF("1t3456789_t23456789_t23t56789_");
	//	PrtPutConstStrWithCRLF(" Remains/\			xxxxxxx.xx");
		PrtPutTab(1);
		PrtPutConstStr(CHANGE_STR);
		PrtPutStr(saleUsePriceUnit);
		PrtPutTab(4);	
		if (saleUseEuroFlag) PrtPutNum(saleChangeMoney, 9, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		else PrtPutNum(saleChangeMoney, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		PrtPutCRLF(1);	

		if (saleEnableSumPrt)
		{
			PrtPutCRLF(9);
			saleEnableSumPrt = 0;
		}
		saleFirstAdd = 1;
	}
}

/**
********************************************************************************
* @brief    DLP-50 사용시 처음에 보내는 Protocol 함수      
* @param  none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtDlpStartCondition(INT8U printType)
{
	char pBuf[1];
#ifdef USE_MULTI_LABEL
	char str[3];
#endif
	PrtPutConstStr("N\x0A");
	pBuf[0] = LF;
	PrtPutData(pBuf, 1);	//LF	
	PrtPutConstStr("FR");
	pBuf[0] = 0x22;
	PrtPutData(pBuf, 1);	//"
#ifdef USE_MULTI_LABEL
	Int32sToDecStr(str, SaleData.pluNum, 3, '0', 0, 0, 0);
	PrtPutStr(str);
#else
	PrtPutConstStr("ER");
#endif
	pBuf[0] = 0x22;
	PrtPutData(pBuf, 1);	//"
	pBuf[0] = LF;
	PrtPutData(pBuf, 1);	//LF
	pBuf[0] = 0x3F;
	PrtPutData(pBuf, 1);	//?
	pBuf[0] = LF;
	PrtPutData(pBuf, 1);	//LF
}

/**
********************************************************************************
* @brief    DLP-50 에서 PLU name을 보내는 Protocol 함수
* @param  none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtDlpPluName(void)
{
	char pBuf[1];

	if(SaleData.pluNum)
	{
		if (SaleData.pluNum <= DIRECT_PLU_MAX)
		{
			if(saleDefaultPluName[0] != 0x00) PrtPutData(saleDefaultPluName, PLU_NAME_SIZE);
			else PrtPutData(tempPluData.name, PLU_NAME_SIZE);
			pBuf[0] = LF;
			PrtPutData(pBuf, 1);	//LF
			PrtPutNum(SaleData.pluNum, 3, 0, 0, 0);
			PrtPutData(pBuf, 1);	//LF
		}
		else
		{
#ifdef USE_PLU_NAME_SIZE_16
			PrtPutData("NO_PLU          ", PLU_NAME_SIZE);
#else
			PrtPutData("NO_PLU  ", 8);
#endif
			pBuf[0] = LF;
			PrtPutData(pBuf, 1);	//LF
			PrtPutNum(SaleData.pluNum, 3, 0, 0, 0); 
			PrtPutData(pBuf, 1);	//LF
		}
	}
	else
	{
#ifdef USE_PLU_NAME_SIZE_16
		PrtPutData("NO_PLU          ", PLU_NAME_SIZE);
#else
		PrtPutData("NO_PLU  ", 8);
#endif
		pBuf[0] = LF;
		PrtPutData(pBuf, 1);	//LF
		PrtPutData("   ", 3);
		PrtPutData(pBuf, 1);	//LF
	}
}

/**
********************************************************************************
* @brief    DLP-50 사용시 Data 보내는 Protocol 함수      
* @param  winNum : 보내는 데이터의 종류
* @return   none
* @remark   
********************************************************************************
*/
void salePrtDlp(INT8U winNum)
{
	char pBuf[1];
	
	switch (winNum)
	{
		case WIN_WEIGHT:
			PrtPutNum(SaleData.weight, 6, SaleData.weightDecPos, weightDotTypeFlag, saleUseThousandPtFlag);
			break;
			
		case WIN_UNIT_PRICE:
			PrtPutNum(SaleData.unitPrice, 7, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			break;
			
		case WIN_TOTAL_PRICE:
			PrtPutNum(SaleData.totalPrice, 7, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			break;

		default:
			break;			
	}
	pBuf[0] = LF;
	PrtPutData(pBuf, 1);	//LF
}

#if defined(USE_STREAM_CAS_22BYTE) && defined(USE_EB_STREAM)
/**
********************************************************************************
* @brief    CAS 22 Byte  함수\n
* @param   none
* @return   none
* @remark   STB , NTB , DIB LSB , WDB   UNB CR LF 
*                 STB -> stable('ST'), unstable('US'), overload('OL')
*                 NTB -> gross('GS'), net('NT')
*                 DIB -> device id('0')
*                 LSB -> 8bit(7:1, 6:stable, 5:1, 4:hold, 3:1, 2:gross, 1:net, 0:zero) 
*                 WDB -> weight data(8byte)
*                 BLANK -> '  '
*                 UNB -> kilo gram('KG'), pound('LB'), once('OZ')
********************************************************************************
*/
void salePrtCas22Byte(void)
{
	char strBuf[22];
	char sendByte=0xa8;	// Bit3,5,7: 1(Fixed)
	INT8U i;
#ifdef USE_FSEND_PERIOD_100MS
	static INT16U sendPeriodTimer100ms = 0;
	INT16U currTimer100ms;

	currTimer100ms = TimerGetSysTimer100ms();

	if (!(currTimer100ms - sendPeriodTimer100ms))
	{
		return;
	}
	sendPeriodTimer100ms = currTimer100ms;
#endif
	if(holdCount > HOLD_AVERAGE_TIME && SaleData.saleState == SALE_STATE_HOLD) //Hold ON -> Hold weight
		Int32sToDecStr(strBuf, AbsInt32s(SaleData.holdWeight),6, ' ', AdData.weightDecPos, '.', 0);
	else 	//Hold OFF
		Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight),6, ' ', AdData.weightDecPos, '.', 0);

	for(i=0;i<6;i++)
	{
#ifdef TURKEY_REMOTE_DISP_DEMO //터키 리모트 디스플레이용 임시 디파인 2015.12.21 이동원
		strBuf[i+10] = strBuf[i];
#else
		strBuf[i+11] = strBuf[i];             
#endif
	}	
	if(AdData.stableFlag)
	{
		strBuf[0] = 'S';
		strBuf[1] = 'T';
		sendByte = sendByte | 0x40;	//Stable Status
	}
	else if(AdData.overloadFlag)
	{
		strBuf[0] = 'O';
		strBuf[1] = 'L';
		sendByte = sendByte | 0x00;
	}										
	else
	{
		strBuf[0] = 'U';
		strBuf[1] = 'S';
		sendByte = sendByte | 0x00;
	}

	if (AdData.tareWeight)
	{	
		strBuf[3] = 'N';	 
		strBuf[4] = 'T';
		sendByte = sendByte | 0x02;	//Tare Status
	}
	else							
	{
		strBuf[3] = 'G';	 
		strBuf[4] = 'S';
		sendByte = sendByte | 0x04;	//Gross Status
	}
	if(SaleData.saleState == SALE_STATE_HOLD)
		sendByte = sendByte | 0x10;	//Hold Status
	else
		sendByte = sendByte | 0x00;

	if (AdData.grossZeroFlag)
		sendByte = sendByte | 0x01;	//Zero Status
	else
		sendByte = sendByte | 0x00;

	if (SaleData.weight < 0)
		strBuf[9] = 0x2D;
	else
		strBuf[9] = 0x20;
	
	strBuf[2] = ',';	
	strBuf[5] = ',';
	strBuf[6] = 0x30;	// Device Number: 0 (Fixed)
	strBuf[7] = sendByte;
	strBuf[8] = ',';
#ifdef TURKEY_REMOTE_DISP_DEMO //터키 리모트 디스플레이용 임시 디파인 2015.12.21 이동원
	strBuf[16] = ' ';
#else
	strBuf[10] = ' ';
#endif
	strBuf[17] = ' ';
	strBuf[18] = 'k';	  
	strBuf[19] = 'g';
	strBuf[20] = 0x0d;
	strBuf[21] = 0x0a;

	PrtPutData(strBuf, 22);
}
#endif
#endif
/**
********************************************************************************
* @brief   외부에서 saleZeroKey()에 접근 하는 함수
* @param  none
* @return   none
* @remark   
********************************************************************************
*/
void SetSaleZeroKey(void)
{
	saleZeroKey();
}


#ifdef USE_SATO_PRINTER_PROTOCOL
/**
********************************************************************************
* @brief   외부에서 saleTareClearFlag에 접근 하는 함수
* @param  none
* @return   none
* @remark   
********************************************************************************
*/
BOOLEAN GetSaleTareClearFlag(void)
{
	return saleTareClearFlag;
}

/**
********************************************************************************
* @brief   외부에서 saleTareClearFlag 설정 하는 함수
* @param  none
* @return   none
* @remark   
********************************************************************************
*/
void SetScaleTareClearFlag(BOOLEAN flag)
{
	saleTareClearFlag = flag;
}

/**
********************************************************************************
* @brief   외부에서 saleTransactionWeightCheck 읽어오는 함수
* @param  none
* @return   none
* @remark   
********************************************************************************
*/
BOOLEAN GetSaleTransactionWeightCheck(void)
{
	return saleTransactionWeightCheck;
}

/**
********************************************************************************
* @brief   외부에서 saleTransactionWeightCheck 설정하는 함수
* @param  none
* @return   none
* @remark   
********************************************************************************
*/
void SetSaleTransactionWeightCheck(BOOLEAN flag)
{
	saleTransactionWeightCheck = flag;
}


/**
********************************************************************************
* @brief   외부에서 PLU Name 가져오는 함수
* @param	str, mode
        	mode 0 : saleDefaultPluName
			mode 1 : tempPluData
* @return   none
* @remark   
********************************************************************************
*/
void getSaleDefaultName(INT8U* str, INT8U mode)
{
	switch(mode)
	{
		default:
		case 0:
			MakeStrCopy(str, saleDefaultPluName, PLU_NAME_SIZE);
			break;
		case 1:
			MakeStrCopy(str, tempPluData.name, PLU_NAME_SIZE);
			break;
	}
}

/**
********************************************************************************
* @brief   외부에서 Prefix 가져오는 함수
* @param	none
* @return   none
* @remark   
********************************************************************************
*/
INT32S getSaleWeightPrefix(void)
{
}
#endif
#endif
