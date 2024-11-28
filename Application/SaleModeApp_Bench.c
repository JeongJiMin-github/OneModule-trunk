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
#include "../UI/DispApi.h"
#include "../UI/KeyApi.h"
#include "../UI/EditorApi.h"
#include "../UI/BuzzerApi.h"
#include "../DB/StorageApi.h"
#include "../DB/ParamApi.h"
#include "../DB/PluApi.h"
#include "../Ad/AdApi.h"
#include "../Power/PowerApi.h"
#include "../Communication/DebugApi.h"
#include "../Communication/CommApi.h"
#include "../SystemTimer/TimerApi.h"
#include "ScaleModeApp.h"
#include "SaleModeApp.h"
#include "../Time/TimeApi.h"
#include "../Common/TimeData.h"

#ifdef USE_SALE_BENCH
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief Sale 관련 Data struct */
SALE_DATA SaleData;

/** @brief C4-2 setting data */
/** @brief bit 2, Tare 삭제 사용여부, 0: Save 1: Clear*/
static BOOLEAN scaleTareClearFlag;	//TODO : 접두어 sale 필요??

/** @brief C4-3 setting data */
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
/** @brief bit 4, 00 : Normal 01: 0.5, 10: 00. 10. 20., 11: 25.50.75*/
static INT8U saleRoundOffUnitFlag;
/** @brief bit 1 & bit 0, 0:10, 1:100,  2:1000, 3:10000   E2_read_data +1 이다.*/
static INT8U salePriceForUnit;

/** @brief C4-5 setting data */
/** @brief bit 6 & bit 5 & bit 4, 단가 Decimal point position, 0: 0000,   1: 0.0, 2: 0.00, 3:0.001 4:0.0001*/
static INT8U priceDecPos;	//TODO : 접두어 sale 필요??
/** @brief bit 3, 0: Don't use 1: Use*/
static BOOLEAN scaleUseCanadaMsgFlag;	//TODO : 접두어 sale 필요??
/** @brief bit 1 & bit 0, 사용 프린터 종류, 00 : Don't Use, 01: Use DEP, 10: Use DLP, 11: reserve*/
static INT8U scaleUsePrt;	//TODO : 접두어 sale 필요??

/** @brief 정산 완료 플래그, 0: ready, 1: End*/
static BOOLEAN transactionEndFlag;	//TODO : 접두어 sale 필요??
/** @brief 프린트 가능 여부 플래그, 0: Don't use, 1: Use*/
static BOOLEAN salePrtEnableFlag;
/** @brief Tare clear 플래그, 0: save, 1: clear*/
static BOOLEAN saleTareClearFlag;
/** @brief Line 출력 플래그*/
static BOOLEAN salePrtLineFlag;	
/** @brief 무게 Display에서 사용하는 Dot type, 0: Dot 1: Comma*/
static BOOLEAN weightDotTypeFlag;	//TODO : 접두어 sale 필요??
/** @brief Hold기능 count*/
static INT8U holdCount;	//Haning type INT16U / TODO : 접두어 sale 필요??
/** @brief */
static INT16U holdDisplayFlash;	//TODO : 접두어 sale 필요??
/** @brief 현재 Battery 전압값*/
static INT8U saleBatteryCapacity;
/** @brief Battery 체크 함수 진입을 위한 플래그*/
static BOOLEAN saleBatteryCheckFlag;
/** @brief 1,000원 단위에 comma출력 플래그*/
static BOOLEAN saleUseThousandPtFlag;
/** @brief 무게 단위*/
#ifdef IAR
INT8U code saleWeightUnit[4][3] = { "kg", "lb", "oz", "g "};
#else
const INT8U code saleWeightUnit[4][3] = { "kg", "lb", "oz", "g "};
#endif
/** @brief Counting unit pieces*/
static INT16U salePcs;
/** @brief Compare func. enable*/
static INT8U saleCompareEnable;
/** @brief Count Compare func. enable*/
static INT8U saleCountCompareEnable;
/** @brief Compare minimum weight */
static INT32S saleCompareMinWeight;
/** @brief Compare maximum weight */
static INT32S saleCompareMaxWeight;
/** @brief Count Compare minimum pcs */
static INT32S saleCountCompareMinPcs;
/** @brief Count Compare maximum pcs */
static INT32S saleCountCompareMaxPcs;

/** @brief Compare flag LOW or HIGH or OK */
static INT8U saleCompareFlag;
/** @brief Count Compare flag LOW or HIGH or OK */
static INT8U saleCountCompareFlag;
/** @brief Compare flag LOW or HIGH or OK */
static INT8U saleBuzzerCount;
/** @brief Compare flag LOW or HIGH or OK */
static INT32S saleCountWeight;
/** @brief Count Current PCS */
static INT32S saleCurrentPCS;
/** @brief Compare flag LOW or HIGH or OK */
//static FP32 saleCountUnitWeight;
static INT32S saleCountUnitWeight;
/** @brief Compare flag LOW or HIGH or OK */
static FP32 saleCountCompareUnitWeight;
/** @brief Compare flag LOW or HIGH or OK */
static BOOLEAN saleCountLackBit;


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
INT16U sumDisplayFlash;	//TODO : 접두어 sale 필요??
//<-- for CASTON

/** @brief Percent 기준 무게*/
static INT32S salePercentWeight;
/** @brief Summing Weight*/
static INT32S saleSumWeight;
/** @brief Summing Weight*/
static INT32S saleSumTempWeight;
/** @brief Summing Count*/
static INT32U saleSumCount;
/** @brief Summing Cancel enable*/
static BOOLEAN saleSumCancelEnable;
/** @brief Summing Print First Line flag*/
static BOOLEAN salePrtFirstLineFlag;
/** @brief Compare Low buzzer enable flag*/
static BOOLEAN saleLowBuzzerEnable;
/** @brief Compare High buzzer enable flag*/
static BOOLEAN saleHighBuzzerEnable;
/** @brief Compare Ok buzzer enable flag*/
static BOOLEAN saleOkBuzzerEnable;
/** @brief Count Compare Low buzzer enable flag*/
static BOOLEAN saleLowCountBuzzerEnable;
/** @brief Count Compare High buzzer enable flag*/
static BOOLEAN saleHighCountBuzzerEnable;
/** @brief Count Compare Ok buzzer enable flag*/
static BOOLEAN saleOkCountBuzzerEnable;
/** @brief Count Compare Ok buzzer enable flag*/
static BOOLEAN salePrtAutoEnable;
/** @brief Count Compare Ok buzzer enable flag*/
static INT8U salePrtAutoCount;
/** @brief Display Count Unit Weight enable*/
#ifdef USE_DISP_COUNT_UNIT_WEIGHT
static BOOLEAN saleDispCountUnitWeightEnable;
#endif

/** @brief Tare key is pressed flag*/
static BOOLEAN saleTareKeyPressed;
/** @brief Zero key is pressed flag*/
static BOOLEAN saleZeroKeyPressed;

/** @brief Last digit invalid function enable  flag*/
static BOOLEAN saleLastDigitInvalidEnable;

/** @brief Auto hold enable flag*/
static BOOLEAN saleAutoHoldEnable;

/** @brief Auto hold excute enable flag*/
static BOOLEAN saleAutoHoldExcute;

static INT8U saleUseOnlyOneUnit;

static INT8U saleDateType;

/** @brief Last digit invalid function enable  flag*/
static INT32U salePreWeight;

#ifdef USE_TURKEY_POS_PROTOCOL
static BOOLEAN keycommSetErrFlag;
#endif
#ifdef USE_STREAM_CAS_22BYTE
INT8U flowPrtFlag;		//Stream Mode Enable flag (DP Model)
#endif

/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */

/** @brief hold average time */
#define HOLD_AVERAGE_TIME           10	// hold 잡는 시간 (10 = 1sec)
/** @brief hold display time */
#define HOLD_DISPLAY_TIME           60
/** @brief hold extend display time */
#define HOLD_EXTEND_DISPLAY_TIME    60

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

static void saleModeKey(void);
static void saleUnitChangeKey(void);
static void saleSetKey(void);
static void saleSampleKey(void);
static void salePrintKey(void);

static void 	saleDispNumWithDecPoint(INT8U winNum, INT8U position, INT8U type, INT32S num, BOOLEAN thousandPt);
static void 	salePrtWeightItem(void);
static void 	salePrtWeightTotal(void);
static void 	salePrtWeightCompare(void);
static void 	salePrtCountItem(void);
static void 	salePrtCountCompare(void);
static void 	salePrtPercentItem(void);
static void salePrtDlpStartCondition(INT8U printType);
static void 	salePrtStartCondition(void);
#ifdef USE_STREAM_CAS_22BYTE
static void		salePrtCas22Byte(void);
#endif
/** @brief Key에 따른 Sale Function(key input type) 개수 */
#define SALE_KEY_MAX_FUNC_NUM	1

#define SALE_COUNT_ENABLE_CAPA	1200
#define SALE_COUNT_LOW_CAPA		1
#define SALE_PERCENT_LOW_CAPA		1800

/** @brief Display의 Weight Digit에 따른 Sum Weight 최대치 */
#ifndef USE_WEIGHT_5DIGIT
#define SUM_WEIGHT_MAX				999999
#else
#define SUM_WEIGHT_MAX				999999
#endif

#ifndef NO_USE_HOLD_KEY
#define HOLD_KEY_FUNC_NUM	1
#else
#define HOLD_KEY_FUNC_NUM	0
#endif

/** @brief Key에 따른 Sale Function(void type) 개수 */
#define SALE_KEY_MAX_VOID_FUNC_NUM	(8+HOLD_KEY_FUNC_NUM)

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
#ifndef NO_USE_HOLD_KEY
	{KEY_HOLD, 				saleHoldKey},
#endif
	{KEY_C, 					saleClearKey},
	{KEY_MODE,				saleModeKey},
	{KEY_UNIT_CHANGE,		saleUnitChangeKey},
	{KEY_SET,				saleSetKey},
	{KEY_SAMPLE,			saleSampleKey},
	{KEY_PRINT,				salePrintKey}
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
	char tempMessage[25] = "WELCOME TO CAS          ";
	
	switch (type)
	{
		case NTEP:
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST, 0xC8);
#ifdef USE_ALL_WEIGHT_UNIT
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0x7F);		
#else
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0x73);
#endif
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_3RD, 0x23);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, 0x24);
			ParamWriteData(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PRICE_SYMBOL, "$", PRICE_SYMBOL_SIZE); 	// first unit				
			break;

		case KOREA:
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST, 0x00);
#ifdef USE_ALL_WEIGHT_UNIT			
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0xFF);
#else
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0xF1);
#endif
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_3RD, 0x26);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, 0x70);				
			ParamWriteData(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PRICE_SYMBOL, "\\", PRICE_SYMBOL_SIZE); 	// first unit
			break;

		default: 		// OIML
#ifdef USE_TURKEY_POS_PROTOCOL
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST, 0x3C);
#else
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST, 0x38);
#endif
#ifdef USE_ALL_WEIGHT_UNIT
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0x2F);		
#else
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0x31);
#endif
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_3RD, 0x03);				
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, 0x25);
			ParamWriteData(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PRICE_SYMBOL, "$", PRICE_SYMBOL_SIZE); 	// first unit

			AdWriteParam(TARE_SUCCESSIVE, TARE_TYPE_SUCCESSIVE_PLUS);
			AdWriteParam(ZERO_MARK_INDICATOR, ZERO_MARK_TYPE_ALL);
			break;
	}
	ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SALE_STATE_NORMAL);
	ParamWriteWord(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PCS_VALUE, 10000);
	ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_ON, COMPARE_MODE_OFF);
	ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_WEIGHT, 0); 
	ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MIN_WEIGHT, 0); 
	ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MAX_WEIGHT, 0);
	ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PERCENT_WEIGHT, 0);
	ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_COMPARE_MIN_PCS, 0);
	ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_COMPARE_MAX_PCS, 0);
       ParamWriteDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_WEIGHT_BACKUP, 0);
	ParamWriteDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_COUNT_BACKUP, 0);
	ParamWriteData(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE1, tempMessage, 24);
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
	
	SaleData.weightUnit = AdData.weightUnit;
	SaleData.weightDecPos = AdData.weightDecPos;	

//  C4-2
	readSetDataInSale = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST); 
	scaleTareClearFlag = (readSetDataInSale & 0x04) >> 2;
	
// 	C4-3
	readSetDataInSale = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND);

	weightDotTypeFlag = (readSetDataInSale & 0x80) >> 7;

	if (weightDotTypeFlag == DEC_TYPE_DOT)
	{
		SaleData.weightDotChar = '.';
	} 
	else //weightDotTypeFlag == DEC_TYPE_COMMA
	{
		SaleData.weightDotChar = ',';
	}

	saleUseHeadMsgFlag  = (readSetDataInSale & 0x10) >> 4;			// reserve
	scaleUseGramUnitFlag = (readSetDataInSale & 0x08) >> 3;
	scaleUseOzUnitFlag = (readSetDataInSale & 0x04) >> 2;
	scaleUseLbUnitFlag = (readSetDataInSale & 0x02) >> 1;
	scaleUseKgUnitFlag = (readSetDataInSale & 0x01) >> 0;

	saleSumWeight = (INT32S)ParamReadDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_WEIGHT_BACKUP);
	saleSumCount = ParamReadDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_COUNT_BACKUP);
	saleSumTempWeight = 0;
	
	saleUseOnlyOneUnit = (INT8U)scaleUseGramUnitFlag + (INT8U)scaleUseOzUnitFlag + (INT8U)scaleUseLbUnitFlag + (INT8U)scaleUseKgUnitFlag;

	if (saleUseOnlyOneUnit == 0)
	{
		scaleUseKgUnitFlag = 1;
		saleUseOnlyOneUnit = 1;
	}
	
	if (saleUseOnlyOneUnit == 1)
	{
		SaleData.saleState = SALE_STATE_NORMAL;//for setting using unit
		saleUnitChangeKey();
	}


//	C4-4
	readSetDataInSale = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_3RD);
	saleRoundOffUnitFlag = (readSetDataInSale & 0x30) >> 4;
	saleLastDigitInvalidEnable = (readSetDataInSale & 0x08) >> 3;		// reserve		
	salePriceForUnit = (readSetDataInSale & 0x03);

// 	C4-5
   	readSetDataInSale = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH);
//	Use Standby Time Menu
	priceDecPos	= (readSetDataInSale & 0x70) >> 4;		// reserve		

	if (priceDecPos >= 7)
	{
		saleUseThousandPtFlag = 1;
		priceDecPos = 0;
	}

	scaleUseCanadaMsgFlag = (readSetDataInSale & 0x08) >> 3;		// reserve
	scaleUsePrt = (readSetDataInSale & 0x03);		// reserve	

#ifdef NO_USE_COUNT
	SaleData.saleState = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS);
	saleCompareEnable = COMPARE_MODE_OFF;
	salePcs = 0; 
	saleCountWeight = 0;
	saleCountUnitWeight = 0;
	saleCompareMinWeight = 0; 
	saleCompareMaxWeight = (INT32U)AdData.maxCapa;
	salePercentWeight = 0;
	saleCountCompareMinPcs = 0;
	saleCountCompareMaxPcs = 0;	
	saleLowBuzzerEnable = 0;
	saleHighBuzzerEnable = 0;
	saleOkBuzzerEnable = 0;	
	saleLowCountBuzzerEnable = 0;
	saleHighCountBuzzerEnable = 0;
	saleOkCountBuzzerEnable = 0;
	saleCurrentPCS = 0;
#endif
#ifdef USE_COUNT_COMPARE_PERCENT
	SaleData.saleState = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS);
	tempInt8u = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_ON);	
	saleCompareEnable = tempInt8u & 0x01;
	saleCountCompareEnable = (tempInt8u & 0x02) >> 1;
	salePcs = ParamReadWord(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PCS_VALUE); 
	saleCountWeight = (INT32S)ParamReadDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_WEIGHT);
	//saleCountUnitWeight = (FP32)((FP32)ParamReadDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_WEIGHT) / (FP32)salePcs); 
	saleCountUnitWeight = multiplyDivide((INT32S)ParamReadDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_WEIGHT),1,(INT32S)salePcs);
	saleCompareMinWeight = (INT32S)ParamReadDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MIN_WEIGHT); 
	saleCompareMaxWeight = (INT32S)ParamReadDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MAX_WEIGHT);	
	salePercentWeight = (INT32S)ParamReadDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PERCENT_WEIGHT);
	saleCountCompareMinPcs = (INT32S)ParamReadDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_COMPARE_MIN_PCS);
	saleCountCompareMaxPcs = (INT32S)ParamReadDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_COMPARE_MAX_PCS);

	tempInt8u = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_RANGE);
	switch(tempInt8u)
	{
		case 0:
			saleLowBuzzerEnable = OFF;
			saleHighBuzzerEnable = OFF;
			saleOkBuzzerEnable = ON; 
			break;
		case 1:
			saleLowBuzzerEnable = ON;
			saleHighBuzzerEnable = ON;
			saleOkBuzzerEnable = OFF; 
			break;
		case 2:
			saleLowBuzzerEnable = ON;
			saleHighBuzzerEnable = OFF;
			saleOkBuzzerEnable = OFF; 
			break;
		case 3:
			saleLowBuzzerEnable = OFF;
			saleHighBuzzerEnable = ON;
			saleOkBuzzerEnable = OFF; 
			break;			
	}	

	tempInt8u = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_COMPARE_RANGE);
	switch(tempInt8u)
	{
		case 0:
			saleLowCountBuzzerEnable = OFF;
			saleHighCountBuzzerEnable = OFF;
			saleOkCountBuzzerEnable = ON;
			break;
		case 1:
			saleLowCountBuzzerEnable = ON;
			saleHighCountBuzzerEnable = ON;
			saleOkCountBuzzerEnable = OFF;
			break;
		case 2:
			saleLowCountBuzzerEnable = ON;
			saleHighCountBuzzerEnable = OFF;
			saleOkCountBuzzerEnable = OFF;
			break;
		case 3:
			saleLowCountBuzzerEnable = OFF;
			saleHighCountBuzzerEnable = ON;
			saleOkCountBuzzerEnable = OFF;
			break;
			
	}
#endif
	
	if (scaleUsePrt == PRT_NONE || scaleUsePrt == PRT_RESERVED)
	{
		salePrtEnableFlag = 0;
	}
	else
	{
		salePrtEnableFlag = 1;
	}

	salePrtLineFlag = 0;
			
	transactionEndFlag = FALSE;

	saleTareClearFlag = scaleTareClearFlag;

	SaleData.weight = 0;
	SaleData.unitPrice = 0;
	SaleData.pluNum = 0;
	SaleData.totalPrice = 0;

	SaleData.holdWeight = 0;
	holdCount = 0;
	holdDisplayFlash = 0;
	
	sumWeight = 0;
	sumCount = 0;
	sumSaveWeight=0;
	sumDisplayFlash=0;

	SaleData.addCount = 0;
	SaleData.addPrice = 0L;


	saleBatteryCapacity = 0;
	saleBatteryCheckFlag = 0;

	saleBuzzerCount = 0;

	saleCountLackBit = 0;

	if (scaleUseCanadaMsgFlag)
	{
		if (SaleData.weightUnit == 0)
		{
			DispSetConstStrMsg(WIN_WEIGHT, "METRIC");			
			DispSetRunMsgPage(10, 2);			
		}
		else if (SaleData.weightUnit == 1)		
		{
			DispSetConstStrMsg(WIN_WEIGHT, "POUND");
			DispSetRunMsgPage(10, 2);			
		}
	}
	saleSumCancelEnable = 0;
	salePrtFirstLineFlag = 1;
	salePrtAutoEnable = 1;
	salePrtAutoCount = 0;
#ifdef USE_DISP_COUNT_UNIT_WEIGHT
	saleDispCountUnitWeightEnable = FALSE;
#endif
	saleTareKeyPressed = 0;
	saleZeroKeyPressed = 0;
	
	saleAutoHoldEnable = FALSE;
	saleAutoHoldExcute = TRUE;
	saleDateType = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_DATE_TYPE);	//read date type from E2PROM

#ifdef USE_STREAM_CAS_22BYTE
	if ((PrtSetting.method == PRT_USE_BY_STREAM) && AdCheckInitZero())	//if Initial Zero pass & Flow send mode 
	{
		flowPrtFlag = 1;
	}
	else
	{
		flowPrtFlag = 0;
	}
#endif

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
	//KEY_TYPE maxKeyNum;
#ifdef USE_ED_CONTI_SEND_WEIGHT_DATA 
	static INT16U prevSendWeightTimer100ms = 0;
	INT16U currTimer100ms;
#endif


	SaleData.weight = AdData.weightData;
	SaleData.weightUnit = AdData.weightUnit;	
	SaleData.weightDecPos = AdData.weightDecPos;	
	if(saleLastDigitInvalidEnable) SaleData.lastDigitIsInvalid = AdData.lastDigitIsInvalid;
	else SaleData.lastDigitIsInvalid = FALSE;

	retValue = SCALE_MODE_SALE;
	
	//saleCountWeight = (INT32S)ParamReadDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_WEIGHT);
	saleCurrentPCS = multiplyDivide(AdData.rawSetTareData, (INT32S)salePcs, saleCountWeight);

	if (KeyCheck())
	{
			Key = KeyGetFuncCode(KEY_MODE_NORMAL);
			if(SaleData.saleState == SALE_STATE_HOLD) Key = KEY_NOT_PRESSED;
	}
	else
	{
		Key = KEY_NOT_PRESSED;
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

 #ifndef REAL_STREAM
 #if !(defined(USE_NETHERLAND_STREAM) || defined(USE_ED_CONTI_SEND_WEIGHT_DATA))
 #ifndef USE_ADD_STATUS_FLAG
	if (AdData.stableFlag)
	{
 #endif
 #endif
 #endif
 #ifndef USE_STREAM_CAS_22BYTE
 #if defined(USE_NOT_UNDERLOAD_ERROR) || defined(USE_ED_CONTI_SEND_WEIGHT_DATA)
		if (PrtSetting.method == PRT_USE_BY_STREAM) 
 #else
		if (PrtSetting.method == PRT_USE_BY_STREAM && !AdData.underloadFlag) 
 #endif
		{
			salePrtFirstLineFlag = 0;
			switch(SaleData.saleState)
			{
 #ifndef REAL_STREAM
				case SALE_STATE_NORMAL:
	#ifdef USE_ED_CONTI_SEND_WEIGHT_DATA
					currTimer100ms = TimerGetSysTimer100ms();

					if ((INT16U)(currTimer100ms - prevSendWeightTimer100ms) > 2)
					{
						prevSendWeightTimer100ms = currTimer100ms;
						
						salePrtWeightItem();
					}
					break;
	#else
					if(AdData.stableFlag)
					{
						salePrtWeightItem();
					}
					else if(salePreWeight != AdData.weightData)
					{
						salePrtWeightItem();
						salePreWeight = AdData.weightData;
					}
					break;
	#endif
					
				case SALE_STATE_COUNT:
					if(AdData.stableFlag)
					{
						salePrtCountItem();
					}
					else if(salePreWeight != AdData.weightData)
					{
						salePrtWeightItem();
						salePreWeight = AdData.weightData;
					}
					break;
					
				case SALE_STATE_PERCENT:
					if(AdData.stableFlag)
					{
						salePrtPercentItem();
					}
					else if(salePreWeight != AdData.weightData)
					{
						salePrtWeightItem();
						salePreWeight = AdData.weightData;
					}
					break;
 #else
				case SALE_STATE_NORMAL: 

					salePrtWeightItem();
					break;
				 
				case SALE_STATE_COUNT:

					salePrtWeightItem();
					break;
				 
				case SALE_STATE_PERCENT:

					salePrtWeightItem();
				 	break;
 #endif	 //REAL_STREAM
			}
		}
 #endif	//USE_STREAM_CAS_22BYTE
 #ifdef USE_BT_CONNECT
		if (PrtSetting.method == PRT_USE_BY_AUTO && salePrtAutoEnable && SaleData.weight > 0 && BtEnableFlag && AdData.stableFlag)
 #else
		if (PrtSetting.method == PRT_USE_BY_AUTO && salePrtAutoEnable && SaleData.weight > 0 && AdData.stableFlag)
 #endif
		{
			switch(SaleData.saleState)
			{
				case SALE_STATE_NORMAL:
#ifdef USE_PGM_DB1H
					salePrtWeightItem();
					if(scaleUsePrt == PRT_DEP50)
					{
						PrtPutCRLF(PrtSetting.lineFeedSize);	
					}
					break;			
#else				
					if(scaleUsePrt == PRT_DEP50)
					{
						salePrtWeightItem();	
						if(saleCompareEnable) salePrtWeightCompare();
						else PrtPutCRLF(PrtSetting.lineFeedSize);	
					}
					else if(scaleUsePrt == PRT_DLP50)
					{
						if(saleCompareEnable) salePrtWeightCompare();
						else salePrtWeightItem();
					}
					break;
 #endif
				case SALE_STATE_COUNT:
					if(scaleUsePrt == PRT_DEP50)
					{
						salePrtCountItem();
						if(saleCountCompareEnable) salePrtCountCompare();
						else PrtPutCRLF(PrtSetting.lineFeedSize);
					}
					else if(scaleUsePrt == PRT_DLP50)
					{
						if(saleCountCompareEnable) salePrtCountCompare();
					}
					break;
					
				case SALE_STATE_PERCENT:
					if(scaleUsePrt == PRT_DEP50)
					{
						salePrtPercentItem();
					}
					break;
			}
			BuzzerSetOnTimes(1);
			salePrtAutoEnable = 0;
			salePrtFirstLineFlag = 1;
		}
		if (SaleData.weight <= 0)
		{
			salePrtAutoEnable = 1;
		}
#ifndef REAL_STREAM
#if !(defined(USE_NETHERLAND_STREAM) || defined(USE_ED_CONTI_SEND_WEIGHT_DATA))
#ifndef USE_ADD_STATUS_FLAG
	}
#endif
#endif
#endif

#ifdef USE_STREAM_CAS_22BYTE
 #ifdef USE_BT_CONNECT
	if(PrtSetting.method == PRT_USE_BY_STREAM && BtEnableFlag)   //DBI(LED) Stream Mode (Bluetooth)
 #else
	if(PrtSetting.method == PRT_USE_BY_STREAM)   //F-Send (DP Model) 
 #endif
	{
   		if(flowPrtFlag == 1) 				   //Stream Mode(CAS22BYTE) Enable flag
   		{
	   		salePrtCas22Byte();
   		}
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
#ifdef USE_COMPARE_AUTOTARE
		if(SaleData.weight > 0)
		{
			if(AdData.tareWeight == 0)
			{
				saleWeightTareProc();
				TimerSetDelay100ms(TIMER_DELAY_100MS_WAIT_STABLE, 0);//stop timer
			}
			else if((AdData.tareWeight > 0)&&(saleCompareFlag == SALE_COMPARE_OK))
			{
				saleWeightTareProc();
				TimerSetDelay100ms(TIMER_DELAY_100MS_WAIT_STABLE, 0);//stop timer
			}
		}
		if ((AdData.tareWeight > 0) && AdData.grossZeroFlag)
		{
			AdSetTare();
		}
#endif
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
	return retValue;
}
//kkh0225 code optimaization
#if defined(USE_BT_COMM_API) || defined(USE_TM_COMM_API)
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
* @brief    Sale Mode Indicator 함수 
* @param    none
* @return   none
* @remark   설정 Flag에 따라 현상태를 indicatation 한다.  \n
*           설정할 indicator 정보는 UiDefine.h를 참조한다. \n
********************************************************************************
*/
void saleSetIndicator(void)
{
	DispSetIndicator(INDICATOR_OK, OFF);
	DispSetIndicator(INDICATOR_LOW, OFF);
	DispSetIndicator(INDICATOR_HIGH, OFF);
	DispSetIndicator(INDICATOR_PCS, OFF);
	DispSetIndicator(INDICATOR_PERCENT, OFF);
	DispSetIndicator(INDICATOR_SUM, OFF);
	DispSetIndicator(INDICATOR_LACK, OFF);

#ifdef USE_COUNT_COMPARE_PERCENT
	if(SaleData.saleState == SALE_STATE_NORMAL)
	{
		if (saleCompareEnable == COMPARE_MODE_ON)
		{
			if (saleCompareFlag == SALE_COMPARE_LOW)
			{
				DispSetIndicator(INDICATOR_LOW, ON);
				if(saleLowBuzzerEnable) BuzzerSetOnTimes(1);
			}
			else if (saleCompareFlag == SALE_COMPARE_HIGH)
			{
				DispSetIndicator(INDICATOR_HIGH, ON);
				if(saleHighBuzzerEnable) BuzzerSetOnTimes(1);
			}
			else if(saleCompareFlag == SALE_COMPARE_OK)
			{
				DispSetIndicator(INDICATOR_OK, ON);
				if(saleOkBuzzerEnable) BuzzerSetOnTimes(1);
			}
		}
	}
#endif

	if(SaleData.saleState == SALE_STATE_SUM_COUNT || SaleData.saleState == SALE_STATE_SUM_WEIGHT)
	{
		DispSetIndicator(INDICATOR_SUM, ON);
	}
	if(SaleData.saleState == SALE_STATE_PRECOMPARE_2 || SaleData.saleState == SALE_STATE_COUNT_PRECOMPARE_2)
	{
		DispSetIndicator(INDICATOR_LOW, ON);
	}
	if(SaleData.saleState == SALE_STATE_PRECOMPARE_3 || SaleData.saleState == SALE_STATE_COUNT_PRECOMPARE_3)
	{
		DispSetIndicator(INDICATOR_HIGH, ON);
	}
	
	DispSetIndicator(INDICATOR_KG, OFF);
	DispSetIndicator(INDICATOR_UNITPRICE_KG, OFF);
	DispSetIndicator(INDICATOR_LB, OFF);
	DispSetIndicator(INDICATOR_UNITPRICE_LB, OFF);
	DispSetIndicator(INDICATOR_OZ, OFF);
	DispSetIndicator(INDICATOR_GRAM, OFF);
	
	if(SaleData.saleState == SALE_STATE_COUNT || SaleData.saleState == SALE_STATE_PRECOUNT 
		|| SaleData.saleState == SALE_STATE_COUNT_PRECOMPARE_2 || SaleData.saleState == SALE_STATE_COUNT_PRECOMPARE_3)
	{
		if(salePcs != 10000) DispSetIndicator(INDICATOR_PCS, ON);		
		if (saleCountLackBit) DispSetIndicator(INDICATOR_LACK, ON);
		if(SaleData.saleState == SALE_STATE_COUNT)
		{
			if (saleCountCompareEnable == COMPARE_MODE_ON)
			{			
				if (saleCountCompareFlag == SALE_COMPARE_LOW)
				{
					DispSetIndicator(INDICATOR_LOW, ON);
					if(saleLowCountBuzzerEnable) BuzzerSetOnTimes(1);
				}
				else if (saleCountCompareFlag == SALE_COMPARE_HIGH)
				{
					DispSetIndicator(INDICATOR_HIGH, ON);
					if(saleHighCountBuzzerEnable) BuzzerSetOnTimes(1);
				}
				else if (saleCountCompareFlag == SALE_COMPARE_OK)
				{
					DispSetIndicator(INDICATOR_OK, ON);
					if(saleOkCountBuzzerEnable) BuzzerSetOnTimes(1);
				}
			}
#ifdef USE_DISP_COUNT_UNIT_WEIGHT
			if (saleDispCountUnitWeightEnable) {
 				switch (SaleData.weightUnit) {
					case UNIT_KG:
						DispSetIndicator(INDICATOR_KG, ON);				
						break;

					case UNIT_LB:
						DispSetIndicator(INDICATOR_LB, ON);				
						break;

					case UNIT_OZ:
						DispSetIndicator(INDICATOR_OZ, ON);			
						break;

					case UNIT_GRAM:
						DispSetIndicator(INDICATOR_GRAM, ON);
						break;
					default:
						break;
				}
			}
#endif
		}
	}
	else if (SaleData.saleState == SALE_STATE_PERCENT)
	{
		if(salePercentWeight) DispSetIndicator(INDICATOR_PERCENT, ON);
	}
	else if (SaleData.saleState == SALE_STATE_PRECOMPARE_1 || SaleData.saleState == SALE_STATE_COUNT_PRECOMPARE_1 
		|| SaleData.saleState == SALE_STATE_UNITCHANGE || SaleData.saleState == SALE_STATE_SUM_COUNT || SaleData.saleState == SALE_STATE_AHOLD)
	{
		
	}
	else
	{
#ifdef USE_ALL_WEIGHT_UNIT
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

			case UNIT_GRAM:
				DispSetIndicator(INDICATOR_GRAM, ON);
				break;
			default:
				break;
		}
#else
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

				default:
					break;
			}
		}
#endif
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
				default:
					break;
			}	
		}
	}
}


/**
********************************************************************************
* @brief    현재 Sale mode 에서 사용할 weight, unitprice에 따른 total price 를 결정한다.  
* @param    none
* @return   none
* @remark   AdData의 무게값을 잃어 현재 total price 계산. 	\n
*           Transaction의 Start와 End를 결정한다.         	\n
*			Hold 무게를 결정한다. 							\n
********************************************************************************
*/
void saleWeightFunc(void)
{
	INT8U returnData;
	INT32U	tempData;

#ifdef USE_TURKEY_POS_PROTOCOL
	if (saleTareClearFlag && (AbsInt32s(AdData.rawSetZeroData) <= (INT32S)(AdData.d1 * 2)))	// Tare Clear 동작 조건: 2d 이내
#else
	if (saleTareClearFlag && transactionEndFlag  && AdData.grossZeroFlag)
#endif
	{
		returnData = AdSetTare();
		if (returnData == ERR_NONE || returnData == ERR_NO_TARE)
		{
			transactionEndFlag = FALSE;		
		}
		else
		{
#ifdef USE_TURKEY_POS_PROTOCOL
			saleKeyCommErrFlagSet(OFF);
#endif
		}
	}

	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:
			if(saleAutoHoldExcute == FALSE && SaleData.weight <= 0) saleAutoHoldExcute = TRUE;
				
			if(saleAutoHoldEnable && SaleData.weight > 0 && saleAutoHoldExcute)
			{			
				holdCount = 0;
				SaleData.holdWeight = 0;
				holdDisplayFlash = 0;
				saleAutoHoldExcute = FALSE;
				SaleData.saleState = SALE_STATE_HOLD;		
				TimerSetDelay100ms(TIMER_DELAY_100MS_AUTO_HOLD, 10);	//제품별 다른 값 적용 (HOLD 2번, 무게 3번 Blinking)
			}

			if (saleCompareEnable == COMPARE_MODE_ON)
			{
				if (SaleData.weight < saleCompareMinWeight)
				{
					saleCompareFlag = SALE_COMPARE_LOW;
				}
				else if (SaleData.weight >= saleCompareMinWeight && SaleData.weight < (INT32S)saleCompareMaxWeight)
				{
					saleCompareFlag = SALE_COMPARE_OK;
				}
				else
				{
					saleCompareFlag = SALE_COMPARE_HIGH;
				}
			}	
			break;

		case SALE_STATE_HOLD:
			if(saleAutoHoldExcute == FALSE && SaleData.weight <= 0) saleAutoHoldExcute = TRUE;
				if (SaleData.weight > 0 && holdCount == 0)
				{	
					DispSetConstStrMsg(WIN_WEIGHT, " HOLD");
					DispSetRunMsgPage(10, 2);	//제품별 다른 값 적용 (HOLD 2번, 무게 3번 Blinking)
				}
				if(TimerCheckDelay100ms(TIMER_DELAY_100MS_AUTO_HOLD) == 1)
				{
					TimerSetDelay100ms(TIMER_DELAY_100MS_AUTO_HOLD, 1);
					if (SaleData.weight <= 0)
					{
						SaleData.saleState = SALE_STATE_NORMAL;
					}		
					if (holdCount < HOLD_AVERAGE_TIME)
					{
						SaleData.holdWeight = SaleData.holdWeight + SaleData.weight;
						holdCount++;
					}
					else
					{
						if (holdCount == HOLD_AVERAGE_TIME)
						{			
							SaleData.holdWeight = AdGetRoundedWeight(SaleData.holdWeight / HOLD_AVERAGE_TIME);//round off
						}
                        if (holdCount != 0xFFFF) holdCount++;
					}
				}
			break;

		case SALE_STATE_COUNT:			
			if (saleCountCompareEnable == COMPARE_MODE_ON)
			{
				if (saleCurrentPCS < saleCountCompareMinPcs)
				{
					saleCountCompareFlag = SALE_COMPARE_LOW;
				}
				else if ((saleCurrentPCS >= saleCountCompareMinPcs) && (saleCurrentPCS < (INT32S)saleCountCompareMaxPcs))
				{
					saleCountCompareFlag = SALE_COMPARE_OK;
				}
				else
				{
					saleCountCompareFlag = SALE_COMPARE_HIGH;
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
	INT8U weightDecPosTemp;
	
#ifdef USE_DB2_LED_5DIGIT_FUNC
	if(dispWeight > 99999)	//최대 자리수 넘는 경우 소수점 변경
	{
		weightDecPosTemp = SaleData.weightDecPos - 1;
	}
	else
	{
		weightDecPosTemp = SaleData.weightDecPos;
	}
	saleDispNumWithDecPoint(WIN_WEIGHT, weightDecPosTemp, weightDotTypeFlag, dispWeight, 0);
#else
	saleDispNumWithDecPoint(WIN_WEIGHT, SaleData.weightDecPos, weightDotTypeFlag, dispWeight, 0);
#endif
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
void saleDisplayFunc(void)
{
	INT32S	tempInt32s, salePcsTemp;
	INT8S	weightDecPosTemp;

	if (SaleData.saleState != SALE_STATE_NORMAL && SaleData.saleState != SALE_STATE_COUNT && SaleData.saleState != SALE_STATE_PERCENT)
	{
		PowerSaverResetTimer(TIMER_DELAY_100MS_STAND_BY);
	}

	switch (SaleData.saleState)		/*	display	*/
	{
		case SALE_STATE_NORMAL:	
			saleDisplayWeight(SaleData.weight);
			DispSetIndicator(INDICATOR_HOLD, OFF);
			break;

		case SALE_STATE_SUM_COUNT:
			saleDispNumWithDecPoint(WIN_WEIGHT, 0, weightDotTypeFlag, saleSumCount, 0);	
			break;
		
		case SALE_STATE_SUM_WEIGHT:
#ifdef USE_DB2_LED_5DIGIT_FUNC
			if(saleSumWeight > 99999)
			{
				weightDecPosTemp = SaleData.weightDecPos - 1;
			}
			else
			{
				weightDecPosTemp = SaleData.weightDecPos;
			}
			saleDispNumWithDecPoint(WIN_WEIGHT, weightDecPosTemp, weightDotTypeFlag, saleSumWeight, 0);	
#else
			saleDispNumWithDecPoint(WIN_WEIGHT, SaleData.weightDecPos, weightDotTypeFlag, saleSumWeight, 0);	
#endif
			break;
			
		case SALE_STATE_SAMPLE_WEIGHT:
			//tempInt32s = AdIntToExtValueExtern((INT32S)saleCountUnitWeight);		
			//saleDispNumWithDecPoint(WIN_WEIGHT, SaleData.weightDecPos, weightDotTypeFlag, tempInt32s, 0);
			//salePcsTemp = (AdData.rawSetTareData * (INT32S)salePcs) / saleCountWeight;
			saleDispNumWithDecPoint(WIN_WEIGHT, SaleData.weightDecPos, weightDotTypeFlag, saleCurrentPCS, 0);
			break;
		case SALE_STATE_HOLD:
				DispSetIndicator(INDICATOR_HOLD, ON);
#ifdef USE_EXTEND_HOLD_TIME
                if (holdCount == HOLD_EXTEND_DISPLAY_TIME + HOLD_AVERAGE_TIME)
#else
                if (holdCount == HOLD_DISPLAY_TIME + HOLD_AVERAGE_TIME)
#endif
                {
                    if(saleAutoHoldExcute)
                    {
                        SaleData.saleState = SALE_STATE_NORMAL;
                        DispSetIndicator(INDICATOR_HOLD, OFF);
                    }
                }
                
                if ((holdCount-HOLD_AVERAGE_TIME) % 10 < 5)
                {
                    saleDisplayWeight(SaleData.holdWeight);
                }
                else
                {
                    DispSetStr(WIN_WEIGHT, "	  ");
                }
                break;

		case SALE_STATE_AHOLD:			
			if(saleAutoHoldEnable) 
			{
				DispSetStr(WIN_WEIGHT, "AH ON");
			}
			else
			{			
				DispSetStr(WIN_WEIGHT, "AH OFF");
			}
			break;

		case SALE_STATE_PRECOMPARE_1:
			if (saleCompareEnable == COMPARE_MODE_ON)
			{
				DispSetStr(WIN_WEIGHT, "WL ON");
			}
			else
			{
				DispSetStr(WIN_WEIGHT, "WLOFF");
			}	
			break;

		case SALE_STATE_PRECOMPARE_2:
		case SALE_STATE_PRECOMPARE_3:
			break;

#ifdef USE_COUNT_COMPARE
		case SALE_STATE_PRECOUNT:			
			switch (salePcs)
			{
				case 0:
					DispSetStr(WIN_WEIGHT, "P_  0");
					break;
				case 10:
					DispSetStr(WIN_WEIGHT, "P_ 10");
					break;
				case 20:
					DispSetStr(WIN_WEIGHT, "P_ 20");
					break;
				case 30:
					DispSetStr(WIN_WEIGHT, "P_ 30");
					break;
				case 40:
					DispSetStr(WIN_WEIGHT, "P_ 40");
					break;
				case 50:
					DispSetStr(WIN_WEIGHT, "P_ 50");
					break;
				case 60:
					DispSetStr(WIN_WEIGHT, "P_ 60");
					break;
				case 70:
					DispSetStr(WIN_WEIGHT, "P_ 70");
					break;
				case 80:
					DispSetStr(WIN_WEIGHT, "P_ 80");
					break;
				case 90:
					DispSetStr(WIN_WEIGHT, "P_ 90");
					break;
				case 100:
					DispSetStr(WIN_WEIGHT, "P_100");
					break;
				case 200:
					DispSetStr(WIN_WEIGHT, "P_200");
					break;
				case 300:
					DispSetStr(WIN_WEIGHT, "P_300");
					break;
				case 400:
					DispSetStr(WIN_WEIGHT, "P_400");
					break;
				case 500:
					DispSetStr(WIN_WEIGHT, "P_500");
					break;
				default:
					DispSetStr(WIN_WEIGHT, "PCS");
					break;
			}
			break;	
#endif
#ifdef USE_COUNT_COMPARE_PERCENT		
		case SALE_STATE_PRECOUNT:			
			switch (salePcs)
			{
				case 0:
					DispSetStr(WIN_WEIGHT, "    0");
					break;
				case 10:
					DispSetStr(WIN_WEIGHT, "   10");
					break;
				case 20:
					DispSetStr(WIN_WEIGHT, "   20");
					break;
				case 30:
					DispSetStr(WIN_WEIGHT, "   30");
					break;
				case 40:
					DispSetStr(WIN_WEIGHT, "   40");
					break;
				case 50:
					DispSetStr(WIN_WEIGHT, "   50");
					break;
				case 100:
					DispSetStr(WIN_WEIGHT, "  100");
					break;
				case 150:
					DispSetStr(WIN_WEIGHT, "  150");
					break;
				case 200:
					DispSetStr(WIN_WEIGHT, "  200");
					break;
				default:
					DispSetStr(WIN_WEIGHT, "PCSSET");
					break;
			}
			break;
		
		case SALE_STATE_COUNT_PRECOMPARE_1:
			if (saleCountCompareEnable == COMPARE_MODE_ON)
			{
				DispSetStr(WIN_WEIGHT, "CL ON");
			}
			else
			{
				DispSetStr(WIN_WEIGHT, "CLOFF");
			}	
			break;

		case SALE_STATE_COUNT_PRECOMPARE_2:
		case SALE_STATE_COUNT_PRECOMPARE_3:
			break;

		case SALE_STATE_UNITCHANGE:			
			switch (SaleData.weightUnit)
			{
				case UNIT_KG:
					DispSetStr(WIN_WEIGHT, "U  KG");
					break;
				case UNIT_LB:
					DispSetStr(WIN_WEIGHT, "U  LB");
					break;
				case UNIT_OZ:
					DispSetStr(WIN_WEIGHT, "U  OZ");
					break;
				case UNIT_GRAM:
					DispSetStr(WIN_WEIGHT, "U GRAM");
					break;
			}		
			break;
#endif

		case SALE_STATE_COUNT:
			//saleDispNumWithDecPoint(WIN_WEIGHT, 0, weightDotTypeFlag, (INT32S)((FP32)AdData.rawSetTareData / (FP32)saleCountUnitWeight), 0);
			//salePcsTemp = (AdData.rawSetTareData * (INT32S)salePcs) / saleCountWeight;
#ifdef USE_DISP_COUNT_UNIT_WEIGHT
			if(saleDispCountUnitWeightEnable) {		
				saleSetIndicator();
				tempInt32s = AdIntToExtValueExtern((INT32S)saleCountUnitWeight);
				saleDispNumWithDecPoint(WIN_WEIGHT, SaleData.weightDecPos, weightDotTypeFlag, tempInt32s, 0);
			}
			else
#endif
			saleDispNumWithDecPoint(WIN_WEIGHT, 0, weightDotTypeFlag, saleCurrentPCS, 0);
			break;

		case SALE_STATE_PERCENT:
			if(salePercentWeight) saleDispNumWithDecPoint(WIN_WEIGHT, 1, weightDotTypeFlag, (INT32S)((FP32)((FP32)AdData.rawSetTareData / (FP32)salePercentWeight)*1000), 0);
			else DispSetStr(WIN_WEIGHT, "PERSET");
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
			break;			
			
		case SALE_STATE_SUM_COUNT:
		case SALE_STATE_SUM_WEIGHT:
			saleSumCount = 0;
			saleSumWeight = 0;
			saleSumTempWeight = 0;
			ParamWriteDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_COUNT_BACKUP, saleSumCount);
			ParamWriteDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_WEIGHT_BACKUP, (INT32U)saleSumWeight);
			break;
#ifdef USE_COUNT_COMPARE			
		case SALE_STATE_PRECOMPARE_1:
			if(saleCompareEnable == COMPARE_MODE_ON)
			{
				EditorSetDataAndRun(WIN_WEIGHT, DEC, (INT32U)saleCompareMinWeight, 1, AdData.maxCapa, SaleData.weightDecPos, weightDotTypeFlag, 0);
				SaleData.saleState = SALE_STATE_PRECOMPARE_2;
			}
			break;
			
		case SALE_STATE_PRECOMPARE_2:
			EditorGetData(&saleCompareMinWeight);
			ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MIN_WEIGHT, (INT32U)saleCompareMinWeight); 
			if(saleCompareEnable == COMPARE_MODE_ON)
			{
				EditorSetDataAndRun(WIN_WEIGHT, DEC, (INT32U)saleCompareMaxWeight, (INT32U)saleCompareMinWeight + 1, AdData.maxCapa, SaleData.weightDecPos, weightDotTypeFlag, 0);
				SaleData.saleState = SALE_STATE_PRECOMPARE_3;
			}
			break;
			
		case SALE_STATE_PRECOMPARE_3:
			EditorGetData(&saleCompareMaxWeight);
			ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MAX_WEIGHT, (INT32U)saleCompareMaxWeight);
			SaleData.saleState = SALE_STATE_PRECOMPARE_1;		
			break;

		case SALE_STATE_PRECOUNT:			
			if (salePcs < 100)
			{
				salePcs += 10;
			}
			else if (salePcs >= 100 && salePcs < 500)
			{
				salePcs += 100;				
			}
			else if (salePcs == 500)
			{
				salePcs = 0;				
			}
			else
			{
				salePcs = 0;	
			}
			ParamWriteWord(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PCS_VALUE, salePcs); 
			break;

		case SALE_STATE_COUNT:
			SaleData.saleState = SALE_STATE_PRECOUNT;
			salePcs = 0;
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SaleData.saleState);	
			break;
#endif			
		default:
			BuzzerSetOnTimes(2);
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
	if (AdSetTare() != ERR_NONE)
	{
#ifdef USE_TURKEY_POS_PROTOCOL
		saleKeyCommErrFlagSet(OFF);
#endif
		BuzzerSetOnTimes(2);
	}
	else
	{
#ifdef USE_TURKEY_POS_PROTOCOL
		saleKeyCommErrFlagSet(ON);
#endif
	}
	TimerSetDelay100ms(TIMER_DELAY_100MS_WAIT_STABLE, 50);
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
		case SALE_STATE_PERCENT:
		case SALE_STATE_COUNT:
			saleZeroKeyPressed = 0;
			saleTareKeyPressed = 1;
			TimerSetDelay100ms(TIMER_DELAY_100MS_WAIT_STABLE, 50);
			break;

		case SALE_STATE_PRECOUNT:
			if(AdData.stableFlag)
			{
				if (SaleData.weight <= 0 && AdData.tareWeight == 0) 
				{
					DispSetError(ERROR_NUM_NO_WEIGHT);
					break;
				}
				if (salePcs > 0 && salePcs != 10000)
				{
					if(SaleData.weight > 0)
					{
						saleCountWeight = AdData.rawSetTareData;
						//saleCountUnitWeight = (FP32)((FP32)AdData.rawSetTareData /(FP32)salePcs);
						saleCountUnitWeight = multiplyDivide(saleCountWeight,1,(INT32S)salePcs);
						//if (saleCountUnitWeight <= (FP32)(AdData.d1/2))// || AdData.rawSetTareData <= SALE_COUNT_ENABLE_CAPA)
						if (saleCountUnitWeight <= (INT32S)(AdData.d1/2))// || AdData.rawSetTareData <= SALE_COUNT_ENABLE_CAPA)
						{
							DispSetConstStrMsg(WIN_WEIGHT, "LACK");
							saleCountLackBit = TRUE;
							DispSetRunMsgPage(10, 1);
							BuzzerSetOnTimes(2);
						}
						else
						{
							saleCountLackBit = FALSE;
						}
						ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_WEIGHT, (INT32U)AdData.rawSetTareData); 
						
						SaleData.saleState = SALE_STATE_COUNT;
						ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SaleData.saleState);						
					}
					else
					{
						BuzzerSetOnTimes(1);
						break;
					}
				}
				else if (salePcs == 10000)
				{
					BuzzerSetOnTimes(1);
					break;
				}
				else
				{
						saleZeroKeyPressed = 0;
						saleTareKeyPressed = 1;
						TimerSetDelay100ms(TIMER_DELAY_100MS_WAIT_STABLE, 50);
				}
			}
			else
			{
				DispSetError(ERROR_NUM_UNSTABLE);
			}
			break;

		case SALE_STATE_PRECOMPARE_1:
			if (saleCompareEnable == COMPARE_MODE_ON)
			{
				saleCompareEnable = COMPARE_MODE_OFF;
			}
			else
			{
				saleCompareEnable = COMPARE_MODE_ON;
			}
			tempInt8u = (saleCountCompareEnable << 1) | saleCompareEnable;
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_ON, tempInt8u);
			break;

		case SALE_STATE_AHOLD:
			if(saleAutoHoldEnable) saleAutoHoldEnable = FALSE;
			else saleAutoHoldEnable = TRUE;
			break;
		
		default:
			BuzzerSetOnTimes(2);
	}
}

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

#ifdef USE_COUNT_COMPARE_PERCENT
	switch(SaleData.saleState)
	{
		case SALE_STATE_PRECOMPARE_1:
			if(saleCompareEnable == COMPARE_MODE_ON) saleCompareEnable = COMPARE_MODE_OFF;
			else	saleCompareEnable = COMPARE_MODE_ON;
			tempInt8u = (saleCountCompareEnable << 1) | saleCompareEnable;
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_ON, tempInt8u);
			return;
			
		case SALE_STATE_COUNT_PRECOMPARE_1:
			if(saleCountCompareEnable == COMPARE_MODE_ON) saleCountCompareEnable = COMPARE_MODE_OFF;
			else	saleCountCompareEnable = COMPARE_MODE_ON;
			tempInt8u = saleCompareEnable | (saleCountCompareEnable << 1);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_ON, tempInt8u);
			return;

		case SALE_STATE_UNITCHANGE:
			if(!AdData.tareWeight) saleUnitChangeKey();
			else BuzzerSetOnTimes(1);
			return;
	}
#endif 
		switch (SaleData.saleState)
		{
			case SALE_STATE_NORMAL:
				if (SaleData.weight > 0)
				{
					holdCount = 0;
					SaleData.holdWeight = 0;
					holdDisplayFlash = 0;
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
		
	switch (SaleData.saleState)	 	/*clear*/
	{
		case SALE_STATE_NORMAL:			
			break;
		case SALE_STATE_MODE:
			break;
		default:			
			BuzzerSetOnTimes(2);
			break;
	}
}

#ifdef USE_COUNT_COMPARE_PERCENT
/**
********************************************************************************
* @brief    Mood Key 함수    
* @param    none
* @return   none
* @remark  
********************************************************************************
*/
void saleModeKey(void)
{
	switch (SaleData.saleState)
	{			
		case SALE_STATE_NORMAL:
			if(saleCountUnitWeight) SaleData.saleState = SALE_STATE_COUNT;
			else SaleData.saleState = SALE_STATE_PRECOUNT;
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SaleData.saleState);
			break;
			
		case SALE_STATE_PRECOUNT:
		case SALE_STATE_COUNT:
			SaleData.saleState = SALE_STATE_PERCENT;
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SaleData.saleState);	
			break;
			
		case SALE_STATE_PERCENT:			
			SaleData.saleState = SALE_STATE_PRECOMPARE_1;
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SaleData.saleState);	
			break;
			
		case SALE_STATE_PRECOMPARE_1:
		case SALE_STATE_PRECOMPARE_2:
			SaleData.saleState = SALE_STATE_COUNT_PRECOMPARE_1;
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SaleData.saleState);	
			break;

		case SALE_STATE_COUNT_PRECOMPARE_1:
		case SALE_STATE_COUNT_PRECOMPARE_2:			
			if (saleUseOnlyOneUnit == 1) SaleData.saleState = SALE_STATE_NORMAL;
			else SaleData.saleState = SALE_STATE_UNITCHANGE;	
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SaleData.saleState);	
			break;

		case SALE_STATE_UNITCHANGE:
			SaleData.saleState = SALE_STATE_NORMAL;	
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SaleData.saleState);	
			break;
			
		default:			
			SaleData.saleState = SALE_STATE_NORMAL;	
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SaleData.saleState);	
			break;
	}
}
#endif
#ifdef NO_USE_COUNT
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
}
#endif

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
	
	unitIndex[3] = scaleUseGramUnitFlag;
	unitIndex[2] = scaleUseOzUnitFlag; 
	unitIndex[1] = scaleUseLbUnitFlag;
	unitIndex[0] = scaleUseKgUnitFlag;

	if ((SaleData.saleState == SALE_STATE_NORMAL || SaleData.saleState == SALE_STATE_UNITCHANGE) && AdData.tareWeight == 0 && !saleSumWeight)
	{	
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
* @brief    Set Key 함수    
* @param    none
* @return   none
* @remark  
********************************************************************************
*/
void saleSetKey(void)
{
#ifdef USE_COUNT_COMPARE_PERCENT
	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:
			SaleData.saleState = SALE_STATE_SUM_COUNT;
			break;

		case SALE_STATE_SUM_COUNT:
			SaleData.saleState = SALE_STATE_SUM_WEIGHT;
			break;

		case SALE_STATE_SUM_WEIGHT:
			SaleData.saleState = SALE_STATE_NORMAL;
			break;
			
		case SALE_STATE_PRECOUNT:
			if(AdData.stableFlag)
			{
				if (SaleData.weight <= 0 && AdData.tareWeight == 0) 
				{
					DispSetError(ERROR_NUM_NO_WEIGHT);
					break;
				}
				if (salePcs > 0 && salePcs != 10000)
				{
					if(SaleData.weight > 0)
					{
                        saleCountWeight = AdData.rawSetTareData;    //PCS 최초 계산시 버그 수정
						//saleCountUnitWeight = (FP32)((FP32)AdData.rawSetTareData /(FP32)salePcs);
						saleCountUnitWeight = multiplyDivide(saleCountWeight,1,(INT32S)salePcs);
						if (AdData.rawSetTareData <= SALE_COUNT_LOW_CAPA)
						{
							DispSetConstStrMsg(WIN_WEIGHT, " LOW");
							DispSetRunMsgPage(10, 1);
							BuzzerSetOnTimes(1);
							break;
						}
						//if (saleCountUnitWeight <= (FP32)(AdData.d1/2))// || AdData.rawSetTareData <= SALE_COUNT_ENABLE_CAPA)
						if (saleCountUnitWeight <= (INT32S)(AdData.d1/2))// || AdData.rawSetTareData <= SALE_COUNT_ENABLE_CAPA)
						{
							DispSetConstStrMsg(WIN_WEIGHT, "LACK");
							saleCountLackBit = TRUE;
							DispSetRunMsgPage(10, 1);
							BuzzerSetOnTimes(2);
						}
						else
						{
							saleCountLackBit = FALSE;
						}
						ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_WEIGHT, (INT32U)AdData.rawSetTareData); 						
						
						SaleData.saleState = SALE_STATE_COUNT;
						ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SaleData.saleState); 					
					}
					else
					{
						BuzzerSetOnTimes(1);
						break;
					}
				}
				else if (salePcs == 10000)
				{
					BuzzerSetOnTimes(1);
					break;
				}
			}			
			else
			{
				DispSetError(ERROR_NUM_UNSTABLE);
			}
			break;

		case SALE_STATE_COUNT:
#ifdef USE_DISP_COUNT_UNIT_WEIGHT
			if(saleDispCountUnitWeightEnable)
				saleDispCountUnitWeightEnable = FALSE;
			else
				saleDispCountUnitWeightEnable = TRUE;
#endif
			break;

		case SALE_STATE_SAMPLE_WEIGHT:
			SaleData.saleState = SALE_STATE_COUNT;
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SaleData.saleState); 					
			break;

		case SALE_STATE_PRECOMPARE_1:
			if(saleCompareEnable == COMPARE_MODE_ON)
			{
				EditorSetDataAndRun(WIN_WEIGHT, DEC, (INT32U)saleCompareMinWeight, 0, 99999, SaleData.weightDecPos, DEC_TYPE_DOT, 0);
				EditorSetConstStr("L");
				SaleData.saleState = SALE_STATE_PRECOMPARE_2;
			}
			break;
			
		case SALE_STATE_PRECOMPARE_2:
			EditorGetData(&saleCompareMinWeight);
			ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MIN_WEIGHT, (INT32U)saleCompareMinWeight); 
			if(saleCompareEnable == COMPARE_MODE_ON)
			{
				EditorSetDataAndRun(WIN_WEIGHT, DEC, (INT32U)saleCompareMaxWeight, 0, 99999, SaleData.weightDecPos, DEC_TYPE_DOT, 0);				
				EditorSetConstStr("H");
				SaleData.saleState = SALE_STATE_PRECOMPARE_3;
			}
			break;
			
		case SALE_STATE_PRECOMPARE_3:
			EditorGetData(&saleCompareMaxWeight);
			if (saleCompareMaxWeight <= saleCompareMinWeight)
			{
				DispSetConstStrMsg(WIN_WEIGHT, "WL ERR");
				DispSetRunMsgPage(10, 1);
				BuzzerSetOnTimes(2);
				EditorSetDataAndRun(WIN_WEIGHT, DEC, (INT32U)saleCompareMinWeight, 0, 99999, SaleData.weightDecPos, DEC_TYPE_DOT, 0);
				EditorSetConstStr("L");
				SaleData.saleState = SALE_STATE_PRECOMPARE_2;
				break;
			}			
			ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MAX_WEIGHT, (INT32U)saleCompareMaxWeight);
			SaleData.saleState = SALE_STATE_PRECOMPARE_1;		
			break;
		
		case SALE_STATE_COUNT_PRECOMPARE_1:
			if(saleCountCompareEnable == COMPARE_MODE_ON)
			{
				EditorSetDataAndRun(WIN_WEIGHT, DEC, (INT32U)saleCountCompareMinPcs, 0, 99999, 0, DEC_TYPE_DOT, 0);
				EditorSetConstStr("L");
				SaleData.saleState = SALE_STATE_COUNT_PRECOMPARE_2;
			}
			break;
			
		case SALE_STATE_COUNT_PRECOMPARE_2:
			EditorGetData(&saleCountCompareMinPcs);
			ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_COMPARE_MIN_PCS, (INT32U)saleCountCompareMinPcs); 
			if(saleCountCompareEnable == COMPARE_MODE_ON)
			{
				EditorSetDataAndRun(WIN_WEIGHT, DEC, (INT32U)saleCountCompareMaxPcs, 0, 99999, 0, DEC_TYPE_DOT, 0);				
				EditorSetConstStr("H");
				SaleData.saleState = SALE_STATE_COUNT_PRECOMPARE_3;
			}
			break;
			
		case SALE_STATE_COUNT_PRECOMPARE_3:
			EditorGetData(&saleCountCompareMaxPcs);
			if (saleCountCompareMaxPcs <= saleCountCompareMinPcs)
			{
				DispSetConstStrMsg(WIN_WEIGHT, "CL ERR");
				DispSetRunMsgPage(10, 1);
				BuzzerSetOnTimes(2);
				EditorSetDataAndRun(WIN_WEIGHT, DEC, (INT32U)saleCountCompareMinPcs, 0, 99999, 0, DEC_TYPE_DOT, 0);
				EditorSetConstStr("L");
				SaleData.saleState = SALE_STATE_COUNT_PRECOMPARE_2;
				break;
			}			
			ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_COMPARE_MAX_PCS, (INT32U)saleCountCompareMaxPcs);
			SaleData.saleState = SALE_STATE_COUNT_PRECOMPARE_1;		
			break;

		case SALE_STATE_UNITCHANGE:
			salePcs = 10000;
			salePercentWeight = 0;
			saleCountUnitWeight = 0;
			saleCountLackBit = OFF;
			saleCountCompareEnable = OFF;			
			saleCountCompareMinPcs = 0;
			saleCountCompareMaxPcs = 0;
			saleCompareEnable = OFF;
			saleCompareMinWeight = 0;
			saleCompareMaxWeight = 0;			
			ParamWriteWord(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PCS_VALUE, salePcs);
			ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PERCENT_WEIGHT, (INT32U)salePercentWeight);
			ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_WEIGHT, 0); 
			ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_COMPARE_MIN_PCS, (INT32U)saleCountCompareMinPcs); 
			ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_COMPARE_MAX_PCS, (INT32U)saleCountCompareMaxPcs);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_ON, 0);
			ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MIN_WEIGHT, (INT32U)saleCompareMinWeight); 
			ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MAX_WEIGHT, (INT32U)saleCompareMaxWeight);
			DispSetConstStrMsg(WIN_WEIGHT, "CHANGE");
			DispSetRunMsgPage(10, 1);
			break;
	}
#endif
#ifdef USE_DB2_LED_5DIGIT_FUNC
	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:
			SaleData.saleState = SALE_STATE_SUM_COUNT;
			break;

		case SALE_STATE_SUM_COUNT:
			SaleData.saleState = SALE_STATE_SUM_WEIGHT;
			break;

		case SALE_STATE_SUM_WEIGHT:
			SaleData.saleState = SALE_STATE_NORMAL;
			break;
	}
#endif
}

/**
********************************************************************************
* @brief    Sample Key 함수    
* @param    none
* @return   none
* @remark  
********************************************************************************
*/
void saleSampleKey(void)
{			
#ifdef USE_COUNT_COMPARE_PERCENT
	switch (SaleData.saleState)
	{	
		case SALE_STATE_NORMAL:	
			if(AdData.stableFlag)
			{
				if(SaleData.weight > 0 && PrtSetting.method == PRT_USE_BY_KEY)
				{
					DispSetConstStrMsg(WIN_WEIGHT, "  SUM");
					DispSetRunMsgPage(10, 1);
					saleSumTempWeight = SaleData.weight;
					saleSumWeight += saleSumTempWeight;
					if(saleSumWeight > SUM_WEIGHT_MAX)
					{
						DispSetConstStrMsg(WIN_WEIGHT, "OVER");
						DispSetRunMsgPage(10, 1);
						BuzzerSetOnTimes(1);
						saleSumWeight -= saleSumTempWeight;
					}
					else
					{
						saleSumCount += 1;
						saleSumCancelEnable = 1;
						salePrtWeightItem();
						ParamWriteDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_COUNT_BACKUP, saleSumCount);
						ParamWriteDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_WEIGHT_BACKUP, (INT32U)saleSumWeight);
					}										
				}
				else
				{
					BuzzerSetOnTimes(1);
				}
			}
			else
			{
				BuzzerSetOnTimes(1);
			}
			break;			

		case SALE_STATE_SUM_COUNT:
		case SALE_STATE_SUM_WEIGHT:
			if(PrtSetting.method == PRT_USE_BY_KEY)
			{
				if (saleSumCancelEnable)
				{
					DispSetConstStrMsg(WIN_WEIGHT, "S SUB");
					DispSetRunMsgPage(10, 1);
					saleSumWeight -= saleSumTempWeight;
					saleSumCount -= 1;
					saleSumCancelEnable = 0;
					if(PrtSetting.method != PRT_NOT_USE) salePrtWeightItem();
					ParamWriteDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_COUNT_BACKUP, saleSumCount);
					ParamWriteDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_WEIGHT_BACKUP, (INT32U)saleSumWeight);
				}
				else
				{
					BuzzerSetOnTimes(1);
				}
			}
			else BuzzerSetOnTimes(1);
			break;
			
			
		case SALE_STATE_PRECOUNT:			
			if (salePcs < 50)
			{
				salePcs += 10;
			}
			else if (salePcs >= 50 && salePcs < 200)
			{
				salePcs += 50; 			
			}
			else if (salePcs == 200)
			{
				salePcs = 10;				
			}
			else
			{
				salePcs = 10;				
			}			
			ParamWriteWord(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PCS_VALUE, salePcs); 
			break;

		case SALE_STATE_COUNT:
			SaleData.saleState = SALE_STATE_PRECOUNT;
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SaleData.saleState);
			break;

		case SALE_STATE_PERCENT:		
			if(AdData.stableFlag)
			{
				if (SaleData.weight <= 0 && AdData.tareWeight == 0) 
				{
					DispSetError(ERROR_NUM_NO_WEIGHT);
					break;
				}
				if(SaleData.weight > 0)
				{
					if (AdData.rawSetTareData <= SALE_PERCENT_LOW_CAPA)
					{
						DispSetConstStrMsg(WIN_WEIGHT, " LOW");
						DispSetRunMsgPage(10, 1);
						BuzzerSetOnTimes(1);
						break;
					}
					else
					{
						salePercentWeight = AdData.rawSetTareData;
					}
					ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PERCENT_WEIGHT, (INT32U)salePercentWeight); 						
				}
			}			
			else
			{
				DispSetError(ERROR_NUM_UNSTABLE);
			}
			break;
	}

#endif
#ifdef USE_DB2_LED_5DIGIT_FUNC
	switch (SaleData.saleState)
	{	
		case SALE_STATE_NORMAL:	
			if(AdData.stableFlag)
			{
				if(SaleData.weight > 0)
				{
					DispSetConstStrMsg(WIN_WEIGHT, " SUM ");
					DispSetRunMsgPage(10, 1);
					saleSumTempWeight = SaleData.weight;
					saleSumWeight += saleSumTempWeight;
					if(saleSumWeight > SUM_WEIGHT_MAX)
					{
						DispSetConstStrMsg(WIN_WEIGHT, "OVER");
						DispSetRunMsgPage(10, 1);
						BuzzerSetOnTimes(1);
						saleSumWeight -= saleSumTempWeight;
					}
					else
					{
						saleSumCount += 1;
						saleSumCancelEnable = 1;
						if(PrtSetting.method != PRT_NOT_USE) salePrtWeightItem();
						ParamWriteDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_COUNT_BACKUP, saleSumCount);
						ParamWriteDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_WEIGHT_BACKUP, (INT32U)saleSumWeight);
					}										
				}
				else
				{
					BuzzerSetOnTimes(1);
				}
			}
			else
			{
				BuzzerSetOnTimes(1);
			}
			break;
		case SALE_STATE_SUM_COUNT:
		case SALE_STATE_SUM_WEIGHT:
			if (saleSumCancelEnable)
			{
				DispSetConstStrMsg(WIN_WEIGHT, "S SUB");
				DispSetRunMsgPage(10, 1);
				saleSumWeight -= saleSumTempWeight;
				saleSumCount -= 1;
				saleSumCancelEnable = 0;
				if(PrtSetting.method != PRT_NOT_USE) salePrtWeightItem();
				ParamWriteDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_COUNT_BACKUP, saleSumCount);
				ParamWriteDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_WEIGHT_BACKUP, (INT32U)saleSumWeight);
			}
			else
			{
				BuzzerSetOnTimes(1);
			}
			break;			
	}
#endif
	
}

/**
********************************************************************************
* @brief    Print Key 함수    
* @param    none
* @return   none
* @remark  
********************************************************************************
*/
void salePrintKey(void)
{	
	INT8U tempInt8u;

#ifndef USE_NOT_UNDERLOAD_ERROR
	if(AdData.underloadFlag)	return;
#endif
	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:
			if(AdData.stableFlag)
			{
				if(PrtSetting.method != PRT_NOT_USE)
				{
#ifdef USE_PGM_DB1H
					salePrtWeightItem();	
					if(scaleUsePrt == PRT_DEP50 && PrtSetting.method == PRT_USE_BY_KEY)
					{
						PrtPutCRLF(PrtSetting.lineFeedSize);				
						salePrtFirstLineFlag = 1;
					}
					else
					{		
						BuzzerSetOnTimes(1);
					}
#else
					if(PrtSetting.method == PRT_USE_BY_KEY)
					{							
						if(scaleUsePrt == PRT_DEP50)
						{
							salePrtWeightItem();
							if(saleCompareEnable) salePrtWeightCompare();
							else PrtPutCRLF(PrtSetting.lineFeedSize);				
							salePrtFirstLineFlag = 1;
						}
						else if(scaleUsePrt == PRT_DLP50)
						{
							if(saleCompareEnable) salePrtWeightCompare();
							else salePrtWeightItem();
						}						
						else
						{		
							BuzzerSetOnTimes(1);
						}					
					}
					else
					{		
						BuzzerSetOnTimes(1);
					}					
#endif
				}
			}
			else
			{
				BuzzerSetOnTimes(1);
			}
			break;
			
		case SALE_STATE_COUNT:
			if(AdData.stableFlag)
			{
				if(PrtSetting.method == PRT_USE_BY_KEY)
				{
					if(scaleUsePrt == PRT_DEP50)
					{
						salePrtCountItem();
						if(saleCountCompareEnable) salePrtCountCompare();
						else PrtPutCRLF(PrtSetting.lineFeedSize);
						salePrtFirstLineFlag = 1;
					}					
					else if(scaleUsePrt == PRT_DLP50)
					{
						if(saleCountCompareEnable) salePrtCountCompare();
					}
					else
					{
						BuzzerSetOnTimes(1);
					}					
				}
				else
				{
					BuzzerSetOnTimes(1);
				}				
			}
			else
			{
				BuzzerSetOnTimes(1);
			}
			break;

		case SALE_STATE_PERCENT:
			if(AdData.stableFlag)
			{
				if(PrtSetting.method == PRT_USE_BY_KEY) 
				{
					if(scaleUsePrt == PRT_DEP50) salePrtPercentItem();
					else BuzzerSetOnTimes(1);
				}
				else BuzzerSetOnTimes(1);
			}
			else
			{
				BuzzerSetOnTimes(1);
			}
			break;
		
		case SALE_STATE_SUM_COUNT:
		case SALE_STATE_SUM_WEIGHT:
			if(scaleUsePrt == PRT_DEP50 && PrtSetting.method == PRT_USE_BY_KEY) salePrtWeightTotal();
			else BuzzerSetOnTimes(1);
			break;

		case SALE_STATE_PRECOMPARE_1:
			if(saleCompareEnable == COMPARE_MODE_ON) saleCompareEnable = COMPARE_MODE_OFF;
			else	saleCompareEnable = COMPARE_MODE_ON;
			tempInt8u = (saleCountCompareEnable << 1) | saleCompareEnable;
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_ON, tempInt8u);
			break;
		
		case SALE_STATE_COUNT_PRECOMPARE_1:
			if(saleCountCompareEnable == COMPARE_MODE_ON) saleCountCompareEnable = COMPARE_MODE_OFF;
			else	saleCountCompareEnable = COMPARE_MODE_ON;			
			tempInt8u = saleCompareEnable | (saleCountCompareEnable << 1);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_ON, tempInt8u);
			return;
		
		case SALE_STATE_UNITCHANGE:
			if(!AdData.tareWeight) saleUnitChangeKey();
			else BuzzerSetOnTimes(1);
			return;
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
* @brief	DLP-50 사용시 처음에 보내는 Protocol 함수	   
* @param  none
* @return	none
* @remark	
********************************************************************************
*/
void salePrtDlpStartCondition(INT8U printType)
{
	PrtPutCRLF(1); 
	PrtPutConstStr("FR");
	PrtPutChar(0x22);	//"
	switch(printType)
	{
		case TYPE_WEIGHT:
			PrtPutConstStr("FORM1");
			break;
		case TYPE_WEIGHT_WITH_BARCODE:
			PrtPutConstStr("FORM4");
			break;
		case TYPE_WEIGHT_COMPARE:
			PrtPutConstStr("FORM2");
			break;
		case TYPE_COUNT_COMPARE:
			PrtPutConstStr("FORM3");
			break;
	}
	PrtPutChar(0x22);	//"
	PrtPutCRLF(1);
	PrtPutChar(0x3F);	//?
	PrtPutCRLF(1);
}

/**
********************************************************************************
* @brief    데이타 출력시 start line에 대한 정보     
* @param    none
* @return   none
* @remark   saleUseHeadMsgFlag = head 메세지 출력 여부를 결정함  	 		\n
*			salePrtDayFlag   = 날짜 정보 출력 여부를 결정함
********************************************************************************
*/
void salePrtStartCondition(void)
{
	INT8U	tempStr[25];

	PrtSetTabPos(0,  0);
	PrtSetTabPos(1,  1);
	PrtSetTabPos(2, 11);
	PrtSetTabPos(3, 13);
	PrtSetTabPos(4, 14);
	PrtSetTabPos(5, 21);
	PrtSetTabPos(6, 24);	
	PrtSetTabPos(7, 25);
	
	salePrtLineFlag = 1;
	
	if (salePrtLineFlag)
	{				
		switch (SaleData.saleState)
		{
			case SALE_STATE_NORMAL:				
				PrtPutConstStr(" ======== WEIGHT ========");
				PrtPutCRLF(1);	
				break;
			case SALE_STATE_COUNT:
				PrtPutConstStr(" ======== COUNT =========");
				PrtPutCRLF(1);	
				break;
			case SALE_STATE_PERCENT:
				PrtPutConstStr(" ======== PERCENT ========");
				PrtPutCRLF(1);	
				break;
			default:
				break;
		}
		if (saleUseHeadMsgFlag)
		{
			PrtPutTab(1);
			ParamReadData(PARAM_HEAD_AREA_NUM, HEAD_PARAM_OFFSET_LINE1, tempStr, 24);
			tempStr[24] = '\0';
			PrtPutStr(tempStr);	
			PrtPutCRLF(1);
		}
		switch (saleDateType)
		{
			case 0: //YYMMDD
			default:
			{
				if (PrtSetting.dateUse)
				{
					TimeReadData();
					PrtPutStr(" DATE. ");
					PrtPutDecNum(2000 + TimeData.year, 4, PRT_NUM_TYPE_FILL_ZERO);	PrtPutStr("/");
					PrtPutDecNum(TimeData.month, 2, PRT_NUM_TYPE_FILL_ZERO); 		PrtPutStr("/");
					PrtPutDecNum(TimeData.date, 2, PRT_NUM_TYPE_FILL_ZERO); 		
					 
					PrtPutCRLF(1);
				}
				if (PrtSetting.timeUse)
				{
					TimeReadData();
					PrtPutStr(" TIME. ");
					PrtPutDecNum(TimeData.hour, 2, PRT_NUM_TYPE_FILL_ZERO); 		PrtPutStr(":");
					PrtPutDecNum(TimeData.min, 2, PRT_NUM_TYPE_FILL_ZERO); 		PrtPutStr(":");
					PrtPutDecNum(TimeData.sec, 2, PRT_NUM_TYPE_FILL_ZERO); 
					PrtPutCRLF(1);
				}
			}
			break;
			case 1: //DDMMYY
			{
				if (PrtSetting.dateUse)
				{
					TimeReadData();
					PrtPutStr(" DATE. ");
					PrtPutDecNum(TimeData.date, 2, PRT_NUM_TYPE_FILL_ZERO);		PrtPutStr("/");
					PrtPutDecNum(TimeData.month, 2, PRT_NUM_TYPE_FILL_ZERO); 		PrtPutStr("/");
					PrtPutDecNum(2000 + TimeData.year, 4, PRT_NUM_TYPE_FILL_ZERO); 
					PrtPutCRLF(1);
				}
				if (PrtSetting.timeUse)
				{
					TimeReadData();
					PrtPutStr(" TIME. ");
					PrtPutDecNum(TimeData.hour, 2, PRT_NUM_TYPE_FILL_ZERO); 		PrtPutStr(":");
					PrtPutDecNum(TimeData.min, 2, PRT_NUM_TYPE_FILL_ZERO); 		PrtPutStr(":");
					PrtPutDecNum(TimeData.sec, 2, PRT_NUM_TYPE_FILL_ZERO); 
					PrtPutCRLF(1);
				}
			}
			break;
			case 2: //MMDDYY
			{
				if (PrtSetting.dateUse)
				{
					TimeReadData();
					PrtPutStr(" DATE. ");
					PrtPutDecNum(TimeData.month, 2, PRT_NUM_TYPE_FILL_ZERO);		PrtPutStr("/");
					PrtPutDecNum(TimeData.date, 2, PRT_NUM_TYPE_FILL_ZERO);			PrtPutStr("/");
					PrtPutDecNum(2000 + TimeData.year, 4, PRT_NUM_TYPE_FILL_ZERO); 
					PrtPutCRLF(1);
				}
				if (PrtSetting.timeUse)
				{
					TimeReadData();
					PrtPutStr(" TIME. ");
					PrtPutDecNum(TimeData.hour, 2, PRT_NUM_TYPE_FILL_ZERO); 		PrtPutStr(":");
					PrtPutDecNum(TimeData.min, 2, PRT_NUM_TYPE_FILL_ZERO); 		PrtPutStr(":");
					PrtPutDecNum(TimeData.sec, 2, PRT_NUM_TYPE_FILL_ZERO); 
					PrtPutCRLF(1);
				}
			}
			break;
		}
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
#ifdef USE_ED_CONTI_SEND_WEIGHT_DATA
	char strBuf[7];
#endif

#ifndef	USE_NETHERLAND_STREAM
	if (salePrtEnableFlag)
	{		
		if(scaleUsePrt == PRT_DEP50)
		{
#ifdef USE_ED_CONTI_SEND_WEIGHT_DATA
			if(PrtSetting.method == PRT_USE_BY_STREAM)
			{
				if(AdData.stableFlag)
					PrtPutChar('S');	//Stable 경우 'S' Flag 추가
				else
					PrtPutChar('U');	//Unstable 경우 'U' Flag 추가
				
				if (AdData.overloadFlag)
				{
					PrtPutConstStr("FFFFFFF");
				}
				else
				{
					if (SaleData.weight < 0) PrtPutChar('-');
					else PrtPutChar(' ');
					Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight), 6, ' ', AdData.weightDecPos, '.', 0);
					PrtPutStr(strBuf);
				}
			}
			else
			{
#endif
			if (salePrtFirstLineFlag) 
			{
				salePrtStartCondition();
				PrtPutCRLF(1);
				salePrtFirstLineFlag = 0;
			}
		
			if (SaleData.saleState == SALE_STATE_SUM_COUNT || SaleData.saleState == SALE_STATE_SUM_WEIGHT) 
			{
				PrtPutConstStr("  Weight : -");
				PrtPutTab(4);
			}
			else
			{
				PrtPutConstStr("  Weight :");
				PrtPutTab(4);
			}
			if (SaleData.saleState == SALE_STATE_SUM_COUNT || SaleData.saleState == SALE_STATE_SUM_WEIGHT)
			{
				PrtPutNum(saleSumTempWeight, 7, SaleData.weightDecPos, weightDotTypeFlag, 0);
			}
			else
			{
#ifdef USE_ADD_STATUS_FLAG
				if(AdData.stableFlag) PrtPutChar('S');	//Stable 경우 'S' Flag 추가
				else PrtPutChar('U');	//Unstable 경우 'U' Flag 추가
#endif
				PrtPutNum(SaleData.weight, 7, SaleData.weightDecPos, weightDotTypeFlag, 0);
			}
#ifdef USE_ED_CONTI_SEND_WEIGHT_DATA
			}
#endif
			PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);	PrtPutCRLF(1);
		}

#else
if (salePrtEnableFlag)
	{		
		if(scaleUsePrt == PRT_DEP50)		 // LRE
		{
			if(AdData.stableFlag)					   // stable?
			{
				PrtPutConstStr("ST");
			}
			else if(AdData.overloadFlag)
			{
				PrtPutConstStr("OL");
			}
			else if(AdData.stableFlag == 0)
			{
				PrtPutConstStr("US");
			}

			if(AdData.tareWeight)					   // gross weight? 
			{
				PrtPutConstStr("GS");
			}
			else
			{
				PrtPutConstStr("NT");	
			}

			PrtPutConstStr("0");					// scale number. not use
			PrtPutConstStr("0");					// lamp number. not use
			PrtPutConstStr("0");					// code number. not use

			PrtPutNum(SaleData.weight, 8, SaleData.weightDecPos, weightDotTypeFlag, 0);  // weight			

			PrtPutConstStr(" ");					// space

			if(AdData.grossZeroFlag)				// zero status
			{
				PrtPutConstStr("1");				//on
			}
			else
			{
				PrtPutConstStr("0");				//off
			}

			if (saleCompareFlag == SALE_COMPARE_LOW)
			{
				PrtPutConstStr("1");
				PrtPutConstStr("0");
				PrtPutConstStr("0");		
			}
			else if (saleCompareFlag == SALE_COMPARE_HIGH)
			{
				PrtPutConstStr("0");
				PrtPutConstStr("1");
				PrtPutConstStr("0");
			}
			else if(saleCompareFlag == SALE_COMPARE_OK)
			{
				PrtPutConstStr("0");
				PrtPutConstStr("0");
				PrtPutConstStr("1");
			}
				
			PrtPutCRLF(1);
		}
#endif
		else if(scaleUsePrt == PRT_DLP50)
		{
			if(PrtSetting.barcodeOnOffFlag)
			{
				salePrtDlpStartCondition(TYPE_WEIGHT_WITH_BARCODE);	   //Start command weight with barcode type
			}
			else
			{
				salePrtDlpStartCondition(TYPE_WEIGHT);	   //Start command weight type			
			}			
			PrtPutNum(SaleData.weight, 7, SaleData.weightDecPos, weightDotTypeFlag, saleUseThousandPtFlag);
			PrtPutCRLF(1); 			
			PrtPutNum(AdData.tareWeight, 7, SaleData.weightDecPos, weightDotTypeFlag, saleUseThousandPtFlag);
			PrtPutCRLF(1);
			PrtPutNum(SaleData.weight + AdData.tareWeight, 7, SaleData.weightDecPos, weightDotTypeFlag, saleUseThousandPtFlag);
			PrtPutCRLF(1);
			PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);
			PrtPutCRLF(1);
			if(PrtSetting.barcodeOnOffFlag)
			{
				PrtPutDecNum(SaleData.weight, 5, PRT_NUM_TYPE_FILL_ZERO);			//V04 : Weight(Barcode)
				PrtPutCRLF(1);
			}
			PrtPutData("P1,1", 4);													//End command		
			PrtPutCRLF(1);
		}
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
void salePrtWeightTotal(void)
{	
	PrtPutConstStr(" ------------------------"); PrtPutCRLF(1);
	PrtPutConstStr("  Total :"); PrtPutTab(4); PrtPutNum(saleSumWeight, 7, SaleData.weightDecPos, weightDotTypeFlag, 0);
	PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);	PrtPutCRLF(1);
	PrtPutConstStr("  Count :"); PrtPutTab(4); PrtPutNum(saleSumCount, 7, 0, weightDotTypeFlag, 0);PrtPutConstStr("times");
	PrtPutCRLF(PrtSetting.lineFeedSize);
	salePrtFirstLineFlag = 1;
}

/**
********************************************************************************
* @brief    데이타 출력시 weight item에 대한 판매 정보 출력     
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtWeightCompare(void)
{
	if(scaleUsePrt == PRT_DEP50)
	{
		PrtPutCRLF(1);
		PrtPutConstStr(" -WEIGHT LIMIT SET VALUE-"); PrtPutCRLF(1);
		PrtPutConstStr(" Limit(H) :");PrtPutTab(1);
		PrtPutNum(saleCompareMaxWeight, 7, SaleData.weightDecPos, weightDotTypeFlag, 0);
		PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);	PrtPutCRLF(1);
		PrtPutConstStr(" Limit(L) :");PrtPutTab(1);
		PrtPutNum(saleCompareMinWeight, 7, SaleData.weightDecPos, weightDotTypeFlag, 0);
		PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);	PrtPutCRLF(1);
		PrtPutCRLF(PrtSetting.lineFeedSize);
		salePrtFirstLineFlag = 1;
	}
	else if(scaleUsePrt == PRT_DLP50)
	{
		salePrtDlpStartCondition(TYPE_WEIGHT_COMPARE);	   //Start command weight compare type
		
		PrtPutNum(SaleData.weight, 7, SaleData.weightDecPos, weightDotTypeFlag, saleUseThousandPtFlag);
		PrtPutCRLF(1); 			
		PrtPutNum(saleCompareMaxWeight, 7, SaleData.weightDecPos, weightDotTypeFlag, saleUseThousandPtFlag);
		PrtPutCRLF(1);
		PrtPutNum(saleCompareMinWeight, 7, SaleData.weightDecPos, weightDotTypeFlag, saleUseThousandPtFlag);
		PrtPutCRLF(1);
		PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);
		PrtPutCRLF(1);
		PrtPutData("P1,1", 4);													//End command		
		PrtPutCRLF(1);
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
void salePrtCountItem(void)
{
	INT32S tempInt32s;
	
	if (salePrtEnableFlag)
	{	
		if (salePrtFirstLineFlag) 
		{
			salePrtStartCondition();
			PrtPutCRLF(1);
			salePrtFirstLineFlag = 0;
		}
		if (PrtSetting.method != PRT_USE_BY_STREAM) 
		{
			PrtPutConstStr("  Weight :");
			PrtPutTab(4);		
				PrtPutNum(SaleData.weight, 7, SaleData.weightDecPos, weightDotTypeFlag, 0);
			PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);	PrtPutCRLF(1);
		}
		PrtPutConstStr("  Quantity :");
		PrtPutTab(4);		
		PrtPutNum(saleCurrentPCS, 7, 0, weightDotTypeFlag, 0);
		PrtPutConstStr("pcs");PrtPutCRLF(1);
	}
}

/**
********************************************************************************
* @brief    데이타 출력시 weight item에 대한 판매 정보 출력     
* @param   none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtCountCompare(void)
{	
	if(scaleUsePrt == PRT_DEP50)
	{
		PrtPutCRLF(1);
		PrtPutConstStr(" -COUNT LIMIT SET VALUE-"); PrtPutCRLF(1);
		PrtPutConstStr(" Limit(H) :");PrtPutTab(1);
		PrtPutNum(saleCountCompareMaxPcs, 7, 0, weightDotTypeFlag, 0);
		PrtPutConstStr("pcs");	PrtPutCRLF(1);
		PrtPutConstStr(" Limit(L) :");PrtPutTab(1);
		PrtPutNum(saleCountCompareMinPcs, 7, 0, weightDotTypeFlag, 0);
		PrtPutConstStr("pcs");	PrtPutCRLF(1);
		PrtPutCRLF(PrtSetting.lineFeedSize);
		salePrtFirstLineFlag = 1;
	}
	else if(scaleUsePrt == PRT_DLP50)
	{		
		salePrtDlpStartCondition(TYPE_COUNT_COMPARE);	   //Start command weight compare type
		
		PrtPutNum(SaleData.weight, 7, SaleData.weightDecPos, weightDotTypeFlag, saleUseThousandPtFlag);
		PrtPutCRLF(1); 			
		PrtPutNum(saleCountCompareMaxPcs, 7, 0, weightDotTypeFlag, saleUseThousandPtFlag);
		PrtPutCRLF(1); 			
		PrtPutNum(saleCountCompareMinPcs, 7, 0, weightDotTypeFlag, saleUseThousandPtFlag);
		PrtPutCRLF(1); 			
		PrtPutNum(saleCurrentPCS, 7, 0, weightDotTypeFlag, 0);
		PrtPutCRLF(1); 			
		PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);
		PrtPutCRLF(1); 			
		PrtPutData("P1,1", 4);													//End command		
		PrtPutCRLF(1); 			
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
void salePrtPercentItem(void)
{
	INT32S tempInt32s;
	
	if (salePrtEnableFlag)
	{
		if (PrtSetting.method != PRT_USE_BY_STREAM) 
		{
			salePrtStartCondition();
			PrtPutCRLF(1);
			PrtPutConstStr("  Weight :");
			PrtPutTab(4);
				PrtPutNum(SaleData.weight, 7, SaleData.weightDecPos, weightDotTypeFlag, 0);
			PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);
			PrtPutCRLF(1);
			
			PrtPutConstStr("    100% :");
			PrtPutTab(4);
			tempInt32s = AdIntToExtValueExtern(salePercentWeight);
			PrtPutNum(tempInt32s, 7, SaleData.weightDecPos, weightDotTypeFlag, 0);
			PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);	PrtPutCRLF(1);
		}
		PrtPutConstStr("  Percent:");
		PrtPutTab(4);		
		PrtPutNum((INT32S)((FP32)((FP32)AdData.rawSetTareData / (FP32)salePercentWeight)*1000), 7, 1, weightDotTypeFlag, 0);
		PrtPutConstStr("%");
		PrtPutCRLF(1);
		PrtPutCRLF(PrtSetting.lineFeedSize);
	}
}

#ifdef USE_STREAM_CAS_22BYTE
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

	Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight),6, ' ', AdData.weightDecPos, '.', 0);

	for(i=0;i<6;i++)
	{
		strBuf[i+11] = strBuf[i]; 
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

	if (AdData.tareWeight)// && !sendStatus )
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
	if(SaleData.holdWeight) 	 	sendByte = sendByte | 0x10;	//Hold Status
	else 							sendByte = sendByte | 0x00;
	
	if (AdData.grossZeroFlag)		sendByte = sendByte | 0x01;	//Zero Status
	else							sendByte = sendByte | 0x00;

	if (SaleData.weight < 0)		strBuf[9] = 0x2D;
	else							strBuf[9] = 0x20;
	
	strBuf[2] = ',';
	strBuf[5] = ',';
	strBuf[6] = 0x30;
	strBuf[7] = sendByte;
	strBuf[8] = ',';
	strBuf[10] = ' ';
	strBuf[17] = ' ';
	strBuf[18] = 'k';
	strBuf[19] = 'g';
	strBuf[20] = 0x0d;
	strBuf[21] = 0x0a;

	PrtPutData(strBuf, 22);
}

#endif	//USE_STREAM_CAS_22BYTE
#endif
