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
#include "../UI/MenuApi.h"
#include "../DB/StorageApi.h"
#include "../DB/ParamApi.h"
#include "../DB/PluApi.h"
#include "../Ad/AdApi.h"
#include "../Power/PowerApi.h"
#include "../Communication/DebugApi.h"
#include "../SystemTimer/TimerApi.h"
#include "ScaleModeApp.h"
#include "SaleModeApp.h"
#include "../Time/TimeApi.h"
#include "../Common/TimeData.h"
#include "../Hardware/HardwareConfig.h"

//#include "PgmModeApp.h"



#ifdef USE_SALE_SIMPLE_HEALTH
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
/** @brief Count Compare func. enable*/
static INT8U saleCountCompareEnable;


/** @brief C4-4 setting data */
/** @brief bit 1 & bit 0, 0:10, 1:100,  2:1000, 3:10000   E2_read_data +1 이다.*/
static INT8U salePriceForUnit;

/** @brief C4-5 setting data */
/** @brief bit 1 & bit 0, 사용 프린터 종류, 00 : Don't Use, 01: Use DEP, 10: Use DLP, 11: reserve*/
static INT8U scaleUsePrt;	//TODO : 접두어 sale 필요??

/** @brief 프린트 가능 여부 플래그, 0: Don't use, 1: Use*/
static BOOLEAN salePrtEnableFlag;
/** @brief Tare clear 플래그, 0: save, 1: clear*/
static BOOLEAN saleTareClearFlag;
/** @brief Line 출력 플래그*/
static BOOLEAN salePrtLineFlag;
/** @brief Count Compare Ok buzzer enable flag*/
static INT8U salePrtAutoCount;
/** @brief 무게 Display에서 사용하는 Dot type, 0: Dot 1: Comma*/
static BOOLEAN weightDotTypeFlag;	//TODO : 접두어 sale 필요??
/** @brief Hold기능 count*/
static INT8U holdCount;	//Haning type INT16U / TODO : 접두어 sale 필요??
/** @brief Hold기능 state*/
static INT8U holdState;	//Haning type INT16U / TODO : 접두어 sale 필요??
/** @brief */
static INT16U holdDisplayFlash;	//TODO : 접두어 sale 필요??
/** @brief 무게 단위*/
const INT8U code saleWeightUnit[4][3] = { "kg", "lb", "oz", "g "};

/** @brief Counting unit pieces*/
static INT16U salePcs;
/** @brief Compare func. enable*/
static INT8U saleCompareEnable;
/** @brief Compare minimum weight */
static INT32S saleCompareMinWeight;
/** @brief Compare maximum weight */
static INT32S saleCompareMaxWeight;

/** @brief Compare flag LOW or HIGH or OK */
static INT8U saleCompareFlag;
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
static BOOLEAN saleCountLackBit;

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
/** @brief Count Compare Ok buzzer enable flag*/
static BOOLEAN salePrtAutoEnable;

/** @brief Tare key is pressed flag*/
static BOOLEAN saleTareKeyPressed;
/** @brief Zero key is pressed flag*/
static BOOLEAN saleZeroKeyPressed;

/** @brief Last digit invalid function enable  flag*/
static BOOLEAN saleLastDigitInvalidEnable;

static INT8U saleDateType;

/** @brief Last digit invalid function enable  flag*/
static INT32U salePreWeight;

/** @brief Auto hold enable flag*/
static BOOLEAN saleAutoHoldEnable;
/** @brief Auto hold excute enable flag*/
static BOOLEAN saleAutoHoldExcute;
/** @brief bit 0, Hold key를 눌렀을 때 디스플레이 유지 사용여부, 0: Don't Use 1: Use*/
static BOOLEAN saleAutoHoldDisplayFlag;	//TODO : 접두어 sale 필요??

static INT32S totalValue;
/** @brief Auto Hold Summing Weight*/
/** @brief Auto Hold Current Weight*/
static INT32S curValue;
/** @brief Auto Hold Max Weight*/
static INT32S maxValue;
/** @brief Auto Hold Min Weight*/
static INT32S minValue;
/** @brief Auto Hold Average Weight*/
static INT32S averageValue;
/** @brief Auto Hold Average Weight*/
static INT32S oldAverageValue;
/** @brief Auto Hold Dispersion Value*/
static INT32S dataValue;
/** @brief Auto Hold temp */
static INT8U i;
/** @brief Auto Hold Blink Interval*/
static INT8U blinkInterval;

#ifdef USE_STREAM_CAS_22BYTE
INT8U flowPrtFlag;		//Stream Mode Enable flag (DP Model)
#endif


/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */

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

#define AUTO_SCALE_STABLE_RANGE				45 // ex) d = 5, 3d = 15
#define	AUTO_SCALE_DISPLAY_DELAY			50 // 100ms * 50 = 5 sec
#define AUTO_HOLD_DISPLAY_DELAY				40//80
#define AUTO_HOLD_DISPLAY_BLINK_INTERVAL	16//20
#define AUTO_HOLD_EXCEPTION_PERCENT			2	//%
#define AUTO_HOLD_AVERAGE_COUNT				20

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

static void saleModeKey(void);
static void saleBackLightKey(void);
static void saleUnitChangeKey(void);
static void salePrintKey(void);

static void 	saleDispWeight1_16OZ(INT32S weight);
static void 	saleDispWeight1_10OZ(INT32S weight, INT8U decPos);
static void 	saleDispNumWithDecPoint(INT8U winNum, INT8U position, INT8U type, INT32S num, BOOLEAN thousandPt);
static void 	salePrtWeightItem(void);
static void 	salePrtWeightTotal(void);
static void		salePrtCas22Byte(void);
static void 	salePrtWeightCompare(void);
static void 	salePrtFixedItem(void);
static void 	salePrtMultiItem(void);
static void 	salePrtPercentItem(void);
static void 	salePrtDiscountItem(void);
static void 	salePrtWeightItemCancel(void);
static void 	salePrtFixedItemCancel(void);
static void 	salePrtStartCondition(void);
static void     salePrtPluName(void);

/** @brief Key에 따른 Sale Function(key input type) 개수 */
#define SALE_KEY_MAX_FUNC_NUM	3
#define SALE_COUNT_ENABLE_CAPA	1200
#define SALE_COUNT_LOW_CAPA		1
#define SALE_PERCENT_LOW_CAPA	1800

/** @brief Key에 따른 Sale Function(void type) 개수 */
#ifdef USE_BACK_LIGHT
#define SALE_KEY_MAX_VOID_FUNC_NUM	7
#else
#define SALE_KEY_MAX_VOID_FUNC_NUM	6
#endif
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
	{KEY_MODE,				saleModeKey},
#ifdef USE_BACK_LIGHT
	{KEY_BACK_LIGHT, 		saleBackLightKey},
#endif
	{KEY_UNIT_CHANGE,		saleUnitChangeKey},
	{KEY_PRINT,				salePrintKey}
};
/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/

#define HOLD_BUFFER_MAX 10

INT32S HoldWeight[HOLD_BUFFER_MAX];

RING_BUF AutoHoldWeight;

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
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST, 0x38);
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

	tempInt8u = (INT8U)scaleUseGramUnitFlag + (INT8U)scaleUseOzUnitFlag + (INT8U)scaleUseLbUnitFlag + (INT8U)scaleUseKgUnitFlag;

	if (tempInt8u == 0)
	{
		scaleUseKgUnitFlag = 1;
		tempInt8u = 1;
	}
	
	if (tempInt8u == 1)
	{
		SaleData.saleState = SALE_STATE_NORMAL;//for setting using unit
#ifdef USE_PRINTER
		tempInt8u = PrtSetting.method;
		PrtSetting.method = PRT_NOT_USE;//prevent key print function run in saleUnitChangeKey()
		PrtSetting.method = tempInt8u;
#endif
		saleUnitChangeKey();
	}

//	C4-4
	readSetDataInSale = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_3RD);
	saleLastDigitInvalidEnable = (readSetDataInSale & 0x08) >> 3;		// reserve		
	salePriceForUnit = (readSetDataInSale & 0x03);

// 	C4-5
   	readSetDataInSale = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH);
	scaleUsePrt = (readSetDataInSale & 0x03);		// reserve	

#ifdef USE_COUNT_COMPARE
	SaleData.saleState = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS);
	saleCompareEnable = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_ON);
	saleCompareEnable &= 0x01;
	salePcs = ParamReadWord(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PCS_VALUE); 
	saleCountWeight = (INT32S)ParamReadDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_WEIGHT);
	//saleCountUnitWeight = (FP32)((FP32)ParamReadDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_WEIGHT) / (FP32)salePcs);
	saleCountUnitWeight = multiplyDivide((INT32S)ParamReadDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_WEIGHT),1,(INT32S)salePcs);
	saleCompareMinWeight = (INT32S)ParamReadDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MIN_WEIGHT); 
	saleCompareMaxWeight = (INT32S)ParamReadDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MAX_WEIGHT);	
	saleBuzzerCount = 0;
#endif
#ifdef NO_USE_COUNT
	SaleData.saleState = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS);
	saleCompareEnable = COMPARE_MODE_OFF;
	salePcs = 0; 
	saleCountWeight = 0;
	saleCountUnitWeight = 0;
	saleCompareMinWeight = 0; 
	saleCompareMaxWeight = (INT32U)AdData.maxCapa;
	saleCurrentPCS = 0;
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
	saleTareClearFlag = scaleTareClearFlag;
	SaleData.weight = 0;
	SaleData.holdWeight = 0;
	holdCount = 0;
	holdState = 0;
	holdDisplayFlash = 0;

	saleSumWeight = 0;
	saleSumTempWeight = 0;
	saleSumCount = 0;
	saleSumCancelEnable = 0;
	salePrtFirstLineFlag = 1;
	salePrtAutoEnable = 1;
	salePrtAutoCount = 0;
	averageValue = 0;
	oldAverageValue = 0;

	
	saleTareKeyPressed = 0;
	saleZeroKeyPressed = 0;

	RingBufInit(&AutoHoldWeight, (INT8U *)&HoldWeight[0], HOLD_BUFFER_MAX);
	saleAutoHoldEnable = TRUE;
	saleAutoHoldExcute = TRUE;
	saleAutoHoldDisplayFlag = FALSE;

	blinkInterval = AUTO_HOLD_DISPLAY_BLINK_INTERVAL * DispGetBlinkCount();
	
	DispSetIndicator(INDICATOR_HOLD, ON);	//DISP_DRV_ADDR_HOLD

#ifdef USE_PRINTER
	if(PrtSetting.method == PRT_NOT_USE) salePrtEnableFlag = 0;
	else salePrtEnableFlag =1;
#endif

#ifdef USE_STREAM_CAS_22BYTE
	if ((PrtSetting.method == PRT_USE_BY_FLOW) && AdCheckInitZero())	//if Initial Zero pass & Flow send mode 
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

	SaleData.weight = AdData.weightData;
	SaleData.weightUnit = AdData.weightUnit;
	SaleData.weightDecPos = AdData.weightDecPos;	

	if(saleLastDigitInvalidEnable) SaleData.lastDigitIsInvalid = AdData.lastDigitIsInvalid;
	else SaleData.lastDigitIsInvalid = FALSE;
   
	retValue = SCALE_MODE_SALE;
	
	saleCountWeight = (INT32S)ParamReadDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_WEIGHT);
	saleCurrentPCS = multiplyDivide(AdData.rawSetTareData, (INT32S)salePcs, saleCountWeight);

	if (KeyCheck())
	{
			Key = KeyGetFuncCode(KEY_MODE_NORMAL);
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

	saleSetIndicator();

	saleWeightFunc();
	
	saleDisplayFunc();

#ifdef USE_PRINTER
			if (PrtSetting.method == PRT_USE_BY_STREAM) 
			{
				if (AdData.stableFlag)
				{
					
					salePrtFirstLineFlag = 0;
					switch(SaleData.saleState)
					{
						case SALE_STATE_NORMAL:
							salePrtWeightItem();					
							break;
					}
				}
			}
			
			if (PrtSetting.method == PRT_USE_BY_AUTO && salePrtAutoEnable && SaleData.weight > 0)
			{
				if (AdData.stableFlag)
				{
					switch(SaleData.saleState)
					{
						case SALE_STATE_NORMAL:
		#if defined(USE_PGM_SW) || defined(USE_PGM_PB)
							salePrtFirstLineFlag = 0;
							salePrtWeightItem();
							break;			
		#else					
							if(scaleUsePrt == PRT_DEP50)
							{
								salePrtWeightItem();	
								if(saleCompareEnable) salePrtWeightCompare();
								else PrtPutCRLF(PrtSetting.lineFeedSize);	
							}
		#endif					
					}
					BuzzerSetOnTimes(1);
					salePrtAutoEnable = 0;
					salePrtFirstLineFlag = 0;
				}
			}
			if (AdData.stableFlag)
			{
				if (SaleData.weight <= 0)
				{
					salePrtAutoEnable = 1;
				}
			}
	if(PrtSetting.method == PRT_USE_BY_CHANGE_WEIGHT && salePreWeight != AdData.weightData)
	{
		salePrtFirstLineFlag = 0;
		salePrtWeightItem();
		salePreWeight = AdData.weightData;
	}
 #ifdef USE_STREAM_CAS_22BYTE
	if (PrtSetting.method == PRT_USE_BY_FLOW)	//F-Send (DP Model) 
	{
		if(flowPrtFlag == 1)					//Stream Mode(CAS22BYTE) Enable flag
		{
			salePrtCas22Byte();
		}
	}
 #endif
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

#ifdef USE_COMMAND_KEY
/**
********************************************************************************
* @brief    Sale Mode를 돌면서 timer를 체크하는 함수
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void SaleZeroExtKeyProc(void)
{
	saleZeroKey();
	saleZeroKeyProc();
	BuzzerSetOnTimes(1);
}

/**
********************************************************************************
* @brief    Sale Mode를 돌면서 timer를 체크하는 함수
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void SaleTareExtKeyProc(void)
{
	saleTareKey();
	saleWeightTareProc();
	BuzzerSetOnTimes(1);
}

/**
********************************************************************************
* @brief    Sale Mode를 돌면서 timer를 체크하는 함수
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void SaleHoldExtKeyProc(void)
{
	if(PrtSetting.method != PRT_USE_BY_KEY)
	{
		saleHoldKey();
		BuzzerSetOnTimes(1);
	}
	else 
	{
		CommSetKeyCmdErrFlag(ON);
	}
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
	DispSetIndicator(INDICATOR_LACK, OFF);

#ifdef USE_COUNT_COMPARE
	if(SaleData.saleState == SALE_STATE_NORMAL)
	{
		if (saleCompareEnable == COMPARE_MODE_ON)
		{
			if (saleCompareFlag == SALE_COMPARE_LOW)
			{
				DispSetIndicator(INDICATOR_LOW, ON);
			}
			else if (saleCompareFlag == SALE_COMPARE_HIGH)
			{
				DispSetIndicator(INDICATOR_HIGH, ON);
				BuzzerSetOnTimes(1);
			}
			else if (saleCompareFlag == SALE_COMPARE_OK)
			{
				DispSetIndicator(INDICATOR_OK, ON);
				if (saleBuzzerCount == 15)
				{
					BuzzerSetOnTimes(1);
					saleBuzzerCount = 0;
				}
				else
				{
					saleBuzzerCount++;
				}
			}
		}
	}
#endif

	if(SaleData.saleState == SALE_STATE_PRECOMPARE_2 || SaleData.saleState == SALE_STATE_COUNT_PRECOMPARE_2)
	{
		DispSetIndicator(INDICATOR_LOW, ON);
	}
	if(SaleData.saleState == SALE_STATE_PRECOMPARE_3 || SaleData.saleState == SALE_STATE_COUNT_PRECOMPARE_3)
	{
		DispSetIndicator(INDICATOR_HIGH, ON);
	}
	
	DispSetIndicator(INDICATOR_KG, OFF);
	DispSetIndicator(INDICATOR_LB, OFF);
	DispSetIndicator(INDICATOR_OZ, OFF);
	DispSetIndicator(INDICATOR_GRAM, OFF);
	
	if(SaleData.saleState == SALE_STATE_COUNT || SaleData.saleState == SALE_STATE_PRECOUNT 
		|| SaleData.saleState == SALE_STATE_COUNT_PRECOMPARE_2 || SaleData.saleState == SALE_STATE_COUNT_PRECOMPARE_3)
	{
		if (salePcs != 10000) DispSetIndicator(INDICATOR_PCS, ON);		
		if (saleCountLackBit) DispSetIndicator(INDICATOR_LACK, ON);
	}
	else if (SaleData.saleState == SALE_STATE_PRECOMPARE_1 || SaleData.saleState == SALE_STATE_COUNT_PRECOMPARE_1 
		|| SaleData.saleState == SALE_STATE_UNITCHANGE || SaleData.saleState == SALE_STATE_SUM_COUNT || SaleData.saleState == SALE_STATE_AHOLD)
		{
			
		}

	else
	{
		switch (SaleData.weightUnit)
		{
			case UNIT_KG:
				DispSetIndicator(INDICATOR_KG, ON);				
				break;

			case UNIT_LB:
				DispSetIndicator(INDICATOR_LB, ON);				
				break;
#ifdef USE_UNIT_1_16OZ
			case UNIT_1_16OZ:
#endif
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
	FP64	tempDoubledata;		

	if(saleTareClearFlag && AdData.grossZeroFlag)
	{
		returnData = AdSetTare();
	}

	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:

			if(saleAutoHoldExcute == FALSE && SaleData.weight <= 0) saleAutoHoldExcute = TRUE;

			//평균값 산정, 최대, 최소값 산정
				RingBufPutInt32sForAdDrv(&AutoHoldWeight, SaleData.weight);

				curValue =	SaleData.weight;

				maxValue = curValue;
				minValue = curValue;
				totalValue = 0;
				
				for (i = 0; i < 10; i++)
				{							
					curValue =  RingBufGetInt32s(&AutoHoldWeight);
					
					totalValue += curValue; 
					
					if (curValue > maxValue)
					{
						maxValue = curValue;
					}

					else if (minValue > curValue)
					{
						minValue = curValue;
					}
				}

				averageValue = totalValue / 10; 

				dataValue =  maxValue - minValue;
			
			if(saleAutoHoldEnable && SaleData.weight > 0 && saleAutoHoldExcute)
			{
				//평균값과 min,max의 차이가 정해진 값의 범위 안이면..
				if (dataValue <= (CalParam.divFactor*3) && averageValue > (AUTO_HOLD_EXCEPTION_PERCENT * AdIntToExtValueExtern(CalParam.resolution)/ 100L))//wcm0718
				{
					holdCount = 0;
					holdState++;//to STATE#1
					SaleData.holdWeight = SaleData.weight;
					holdDisplayFlash = blinkInterval + 1;
					SaleData.saleState = SALE_STATE_HOLD;
					TimerSetDelay100ms(TIMER_DELAY_100MS_AUTO_HOLD, 10);	//제품별 다른 값 적용 (HOLD 2번, 무게 3번 Blinking)
					totalValue = 0;
					saleAutoHoldExcute = FALSE;
					oldAverageValue = averageValue;
				}
			//아니면..
			
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
						if(saleAutoHoldExcute == FALSE && SaleData.weight <= 0) 
						{
							saleAutoHoldExcute = TRUE;
							if(saleAutoHoldEnable == TRUE)
							{
								TimerSetDelay100ms(TIMER_DELAY_100MS_AUTO_HOLD, AUTO_SCALE_DISPLAY_DELAY);
							}
						}
						if(AdData.overloadFlag)
						{
							SaleData.holdWeight = 0;
							SaleData.saleState = SALE_STATE_NORMAL;
							holdState = 0;
						}

						if(saleAutoHoldEnable == FALSE)
						{
							if (SaleData.weight > 0 && holdCount == 0)
							{	
								DispSetConstStrMsg(WIN_WEIGHT, " HOLD");
								DispSetRunMsgPage(10, 2);
							}
						}
						if(TimerCheckDelay100ms(TIMER_DELAY_100MS_AUTO_HOLD) == 1)
						{
							TimerSetDelay100ms(TIMER_DELAY_100MS_AUTO_HOLD, 1);

							if (SaleData.weight <= (AUTO_HOLD_EXCEPTION_PERCENT * AdIntToExtValueExtern(CalParam.resolution)/ 100L))
							{
								if(holdCount < AUTO_HOLD_AVERAGE_COUNT)
								{
									SaleData.saleState = SALE_STATE_NORMAL;
									SaleData.holdWeight = 0;
									holdDisplayFlash = blinkInterval + 1;
									holdState = 0;
								}
							}

							if(holdCount == 0)
							{
								totalValue = SaleData.weight;
							}
							else
							{
								totalValue = oldAverageValue * (holdCount) + SaleData.weight;//여기서 holdCount가 0부터 시작해서 (n - 1) -> holdCount가 되었다
							}

							averageValue = totalValue / (holdCount + 1);//holdCount가 0부터 시작하므로 totalValue / n 인데 n -> holdCount + 1 이 되었다.

							if (holdCount < AUTO_HOLD_AVERAGE_COUNT)//Auto Hold 무게 확인
							{
								if(SaleData.weight > (oldAverageValue + AUTO_SCALE_STABLE_RANGE) || SaleData.weight < (oldAverageValue - AUTO_SCALE_STABLE_RANGE))
								{
									holdState = 0;
									SaleData.holdWeight = 0;
									SaleData.saleState = SALE_STATE_NORMAL;
								}
								SaleData.holdWeight = averageValue;
								oldAverageValue = averageValue;
								holdCount++;
							}
							else if (holdCount == AUTO_HOLD_AVERAGE_COUNT)
							{
								holdDisplayFlash = 0;
								SaleData.holdWeight = averageValue;
								SaleData.holdWeight = AdGetRoundedWeight(SaleData.holdWeight);//round off			
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
												
								if (SaleData.holdWeight <= 0)
								{
									SaleData.totalPrice = 0;
								}
								holdCount++;
								holdState++;
							}
							else 
							{
								holdCount = AUTO_HOLD_AVERAGE_COUNT + 1;
							}	
						}
					break;
		case SALE_STATE_COUNT:	
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
#ifdef USE_UNIT_1_16OZ
		if (SaleData.weightUnit == UNIT_1_16OZ)
		{
			saleDispWeight1_16OZ(dispWeight);
		}
		else if (SaleData.weightUnit == UNIT_OZ)
		{
			if (SaleData.lastDigitIsInvalid)
			{
				saleDispWeight1_10OZ(dispWeight / 10, SaleData.weightDecPos-1);
			}
			else
			{
				saleDispWeight1_10OZ(dispWeight, SaleData.weightDecPos);
			}
		}
		else
		{
			DispSetFraction(WIN_WEIGHT, 0xff, 0xff);
			saleDispNumWithDecPoint(WIN_WEIGHT, SaleData.weightDecPos, weightDotTypeFlag, dispWeight, 0);
			if (SaleData.lastDigitIsInvalid)
			{
				if (SaleData.weightDecPos == 1) { ////2kg일 경우 0.5g에서 1g으로 올라가면 decimal point 삭제해야 하므로 다시 Display함수 불러줌
					saleDispNumWithDecPoint(WIN_WEIGHT,  0, weightDotTypeFlag, dispWeight, 0);
				}
				DispSetChar(WIN_WEIGHT, DispGetWinSize(WIN_WEIGHT) - 1, ' ');
			}
		}
#else
		saleDispNumWithDecPoint(WIN_WEIGHT, SaleData.weightDecPos, weightDotTypeFlag, dispWeight, 0);
		if (SaleData.lastDigitIsInvalid)
		{
			if (SaleData.weightDecPos == 1) { ////2kg일 경우 0.5g에서 1g으로 올라가면 decimal point 삭제해야 하므로 다시 Display함수 불러줌
				saleDispNumWithDecPoint(WIN_WEIGHT,  0, weightDotTypeFlag, dispWeight, 0);
			}
			DispSetChar(WIN_WEIGHT, DispGetWinSize(WIN_WEIGHT) - 1, ' ');
		}
#endif
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
	INT32S	tempInt32s;
	INT8U	tempInt8u;

	if (SaleData.saleState != SALE_STATE_NORMAL && SaleData.saleState != SALE_STATE_COUNT && SaleData.saleState != SALE_STATE_PERCENT)
	{
		PowerSaverResetTimer(TIMER_DELAY_100MS_STAND_BY);
	}

	switch (SaleData.saleState)		/*	display	*/
	{
		case SALE_STATE_NORMAL:	
			saleDisplayWeight(SaleData.weight);
			break;

		case SALE_STATE_HOLD:
			if(holdState == 1)
			{
				saleDisplayWeight(SaleData.holdWeight);
			}
			else if(holdState == 2)
			{
				holdDisplayFlash++;

				if (SaleData.weight <= (AUTO_HOLD_EXCEPTION_PERCENT * AdIntToExtValueExtern(CalParam.resolution)/ 100L))
				{
					holdState = 0;
					SaleData.saleState = SALE_STATE_NORMAL;
				}

				if(holdDisplayFlash == blinkInterval)
				{
						holdState++;
				}

				if (holdDisplayFlash % AUTO_HOLD_DISPLAY_BLINK_INTERVAL < (AUTO_HOLD_DISPLAY_BLINK_INTERVAL/2))
				{
					saleDisplayWeight(SaleData.holdWeight);
				}
				else
				{
					DispSetStr(WIN_WEIGHT, "	  ");
				}

			}
			else if(holdState == 3)
			{
				saleDisplayWeight(SaleData.holdWeight);
                if (SaleData.weight <= (AUTO_HOLD_EXCEPTION_PERCENT * AdIntToExtValueExtern(CalParam.resolution)/ 100L))
				{
					holdState++;
					holdDisplayFlash = 0;
				}
			}
			else if(holdState == 4)
			{
					holdState = 0;
					SaleData.saleState = SALE_STATE_NORMAL;
			}
			break;

		case SALE_STATE_AHOLD:			//Auto Hold 디스플레이

			if(saleAutoHoldDisplayFlag == FALSE)
			{
				saleAutoHoldDisplayFlag = TRUE;			
				if(saleAutoHoldEnable) 
				{
					DispSetStr(WIN_WEIGHT, "AH OFF");
					saleAutoHoldEnable = FALSE;
					DispSetIndicator(INDICATOR_HOLD, OFF);	//DISP_DRV_ADDR_HOLD
					TimerSetDelay100ms(TIMER_DELAY_100MS_UNIT_PRICE_RENEWAL, 10);//1초간 디스플레이 하는 용도
				}
				else
				{			
					DispSetStr(WIN_WEIGHT, "AH ON");
					saleAutoHoldEnable = TRUE;
					DispSetIndicator(INDICATOR_HOLD, ON);	//DISP_DRV_ADDR_HOLD

					for(tempInt8u = 0; tempInt8u < 10; tempInt8u++)
					{
						RingBufPutInt32sForAdDrv(&AutoHoldWeight, SaleData.weight);
					}
					TimerSetDelay100ms(TIMER_DELAY_100MS_UNIT_PRICE_RENEWAL, 10);
				}
			}

			if (TimerCheckDelay100ms(TIMER_DELAY_100MS_UNIT_PRICE_RENEWAL) == 1)
			{
				SaleData.saleState = SALE_STATE_NORMAL;
				saleAutoHoldDisplayFlag = FALSE;
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

		case SALE_STATE_COUNT:
			//saleDispNumWithDecPoint(WIN_WEIGHT, 0, weightDotTypeFlag, (INT32S)((FP32)AdData.rawSetTareData / (FP32)saleCountUnitWeight), 0);
			saleDispNumWithDecPoint(WIN_WEIGHT, 0, weightDotTypeFlag, saleCurrentPCS, 0);
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
#ifdef USE_COMMAND_KEY
		CommSetKeyCmdErrFlag(ON);
#endif
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
#ifdef USE_COMMAND_KEY
			CommSetKeyCmdErrFlag(OFF);
#endif
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
				EditorSetDataAndRun(WIN_WEIGHT, DEC, (INT32U)saleCompareMaxWeight, (INT32U)saleCompareMinWeight+1, AdData.maxCapa, SaleData.weightDecPos, weightDotTypeFlag, 0);
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
				ParamWriteWord(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PCS_VALUE, salePcs); 
			}
			else if (salePcs >= 100 && salePcs < 500)
			{
				salePcs += 100;				
				ParamWriteWord(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PCS_VALUE, salePcs); 
			}
			else if (salePcs == 500)
			{
				salePcs = 0;				
				ParamWriteWord(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PCS_VALUE, salePcs); 
			}
			else
			{
				salePcs = 0;				
				ParamWriteWord(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PCS_VALUE, salePcs); 
			}
			break;

		case SALE_STATE_COUNT:
			SaleData.saleState = SALE_STATE_PRECOUNT;
			salePcs = 0;
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SaleData.saleState);	
			break;
#endif			
		default:
			BuzzerSetOnTimes(2);
#ifdef USE_COMMAND_KEY
			CommSetKeyCmdErrFlag(ON);
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

	returnData = AdSetTare();
	
	if (returnData != ERR_NONE)
	{
		BuzzerSetOnTimes(2);						
#ifdef USE_COMMAND_KEY
		CommSetKeyCmdErrFlag(ON);
#endif
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
#ifndef USE_WEIGHT_ONLY_ECR
void saleTareKey(void)
{
	INT8U returnData;
	INT8U tempInt8u;	

	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:
			saleZeroKeyPressed = 0;
			saleTareKeyPressed = 1;
			TimerSetDelay100ms(TIMER_DELAY_100MS_WAIT_STABLE, 50);
#ifdef USE_COMMAND_KEY
			CommSetKeyCmdErrFlag(OFF);
#endif
			break;

		case SALE_STATE_PRECOUNT:
			if(AdData.stableFlag)
			{
				if (SaleData.weight <= 0 && AdData.tareWeight == 0) 
				{
					DispSetError(ERROR_NUM_NO_WEIGHT);
					break;
				}
				if (salePcs)
				{
					if(SaleData.weight > 0)
					{
						//saleCountUnitWeight = (FP32)((FP32)AdData.rawSetTareData /(FP32)salePcs);
						saleCountUnitWeight = multiplyDivide(saleCountWeight,1,(INT32S)salePcs);
						//if (saleCountUnitWeight <= (AdData.d1/2) || AdData.rawSetTareData <= SALE_COUNT_ENABLE_CAPA)
						if (saleCountUnitWeight <= (INT32S)(AdData.d1/2) || AdData.rawSetTareData <= SALE_COUNT_ENABLE_CAPA)
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
				else
				{					
					saleZeroKeyPressed = 0;
					saleTareKeyPressed = 1;
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
			tempInt8u = saleCompareEnable;
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_ON, tempInt8u);
			break;

		case SALE_STATE_AHOLD:
			if(saleAutoHoldEnable)
			{
				saleAutoHoldEnable = FALSE;
			}
			else
			{
				saleAutoHoldEnable = TRUE;

				for(tempInt8u = 0; tempInt8u < 10; tempInt8u++)
				{
					RingBufPutInt32sForAdDrv(&AutoHoldWeight, SaleData.weight);
				}
			}
			break;

		default:
			BuzzerSetOnTimes(2);
#ifdef USE_COMMAND_KEY
			CommSetKeyCmdErrFlag(ON);
#endif
	}
}
#else
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
	BuzzerSetOnTimes(2);
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
#ifdef USE_PRINTER
void saleHoldKey(void)
{
	if(PrtSetting.method != PRT_USE_BY_KEY && PrtSetting.method != PRT_USE_BY_ENTER)
	{
	
		switch (SaleData.saleState)
		{
			case SALE_STATE_NORMAL:
				
				if (SaleData.weight > 0)
				{
#ifdef USE_COMMAND_KEY
					CommSetKeyCmdErrFlag(OFF);
#endif
				}
				else
				{
					SaleData.saleState = SALE_STATE_AHOLD;//hold키가 눌러졌을때

#ifdef USE_COMMAND_KEY
					CommSetKeyCmdErrFlag(ON);
#endif
				}
				break;

			case SALE_STATE_AHOLD:						
				SaleData.saleState = SALE_STATE_NORMAL; 
				break;									
	
			default: 
				BuzzerSetOnTimes(2);
#ifdef USE_COMMAND_KEY
				CommSetKeyCmdErrFlag(ON);
#endif
				break;
		}
	}
	else if(PrtSetting.method == PRT_USE_BY_KEY)
	{
		if(SaleData.saleState == SALE_STATE_NORMAL)
		{
			if(AdData.stableFlag)
			{
				if(SaleData.weight > 0 && PrtSetting.method == PRT_USE_BY_KEY)
				{
					DispSetConstStrMsg(WIN_WEIGHT, "  SUM");
					DispSetRunMsgPage(10, 1);
					saleSumTempWeight = SaleData.weight;
					saleSumWeight += saleSumTempWeight;
					if(saleSumWeight > 99999)
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
						ParamWriteDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_COUNT_BACKUP, saleSumCount);
						ParamWriteDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_WEIGHT_BACKUP, (INT32U)saleSumWeight);
					}
					
				}
				else
				{
					BuzzerSetOnTimes(1);
				}
				
				if(SaleData.weight >= 0 && saleSumCount > 0)
				{
					salePrintKey();
				}	
			}
			else
			{
				BuzzerSetOnTimes(2);
			}
		}
		
	}
	else if (SaleData.saleState == SALE_STATE_NORMAL && SaleData.weight > 0 && AdData.stableFlag && PrtSetting.method == PRT_USE_BY_ENTER)
	{
		DispSetConstStrMsg(WIN_WEIGHT, " PRT ");
		DispSetRunMsgPage(10, 1);
		salePrtFirstLineFlag = 0;
		salePrtWeightItem();
	}
	else BuzzerSetOnTimes(1);
}
#else
void saleHoldKey(void)
{
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
				BuzzerSetOnTimes(2);
			}
			break;
		
		default: 
			BuzzerSetOnTimes(2);
			break;
	}
}
#endif

/**
********************************************************************************
* @brief    Back Light 상태변경 하는 함수      
* @param    none
* @return   none
* @remark   Back Light 상태변경
********************************************************************************
*/
#ifdef USE_BACK_LIGHT
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
	}
	else
	{
		BuzzerSetOnTimes(2);	
		DispSetConstStrMsg(WIN_WEIGHT, " NO BL");
		DispSetRunMsgPage(20, 0);
	}
}
#endif

/**
********************************************************************************
* @brief    Mood Key 함수    
* @param    none
* @return   none
* @remark  
********************************************************************************
*/
#ifdef USE_PRINTER
void saleModeKey(void)
{
 #ifdef USE_NOT_UNDERLOAD_ERROR
	if(PrtSetting.method != PRT_USE_BY_KEY && PrtSetting.method != PRT_USE_BY_ENTER)
 #else
 	if(PrtSetting.method != PRT_USE_BY_KEY && PrtSetting.method != PRT_USE_BY_ENTER && !AdData.underloadFlag)
 #endif
	{	
		switch (SaleData.saleState)
		{
			case SALE_STATE_NORMAL:
				SaleData.saleState = SALE_STATE_PRECOMPARE_1;			
				break;
				
			case SALE_STATE_PRECOMPARE_1:
			case SALE_STATE_PRECOMPARE_2:
				if(saleCountUnitWeight) SaleData.saleState = SALE_STATE_COUNT;
				else SaleData.saleState = SALE_STATE_PRECOUNT;
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SaleData.saleState);	
				break;
				
			case SALE_STATE_PRECOUNT:
			case SALE_STATE_COUNT:
				SaleData.saleState = SALE_STATE_NORMAL;
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SaleData.saleState);	
				break;
			
			default:			
				SaleData.saleState = SALE_STATE_NORMAL;	
				ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SaleData.saleState);	
				break;
		}
	}
 #ifdef USE_NOT_UNDERLOAD_ERROR
	else if(PrtSetting.method == PRT_USE_BY_KEY)
 #else
	else if(PrtSetting.method == PRT_USE_BY_KEY && !AdData.underloadFlag)
 #endif
	{
		if(SaleData.saleState == SALE_STATE_NORMAL)
		{
			if(AdData.stableFlag)
			{
				if(SaleData.weight > 0 && PrtSetting.method == PRT_USE_BY_KEY)
				{
					DispSetConstStrMsg(WIN_WEIGHT, "  SUM");
					DispSetRunMsgPage(10, 1);
					saleSumTempWeight = SaleData.weight;
					saleSumWeight += saleSumTempWeight;
					if(saleSumWeight > 99999)
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
						ParamWriteDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_COUNT_BACKUP, saleSumCount);
						ParamWriteDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_WEIGHT_BACKUP, (INT32U)saleSumWeight);
					}
					
				}
				else
				{
					BuzzerSetOnTimes(1);
				}
				
				if(SaleData.weight >= 0 && saleSumCount > 0)
				{
					salePrintKey();
				}	
			}
			else
			{
				BuzzerSetOnTimes(2);
			}
		}
		
	}
 #ifdef USE_NOT_UNDERLOAD_ERROR
	else if (SaleData.saleState == SALE_STATE_NORMAL && SaleData.weight > 0 && AdData.stableFlag && PrtSetting.method == PRT_USE_BY_ENTER)
 #else
	else if (SaleData.saleState == SALE_STATE_NORMAL && SaleData.weight > 0 && AdData.stableFlag && PrtSetting.method == PRT_USE_BY_ENTER && !AdData.underloadFlag)
 #endif
	{
		DispSetConstStrMsg(WIN_WEIGHT, " PRT ");
		DispSetRunMsgPage(10, 1);
		salePrtFirstLineFlag = 0;
		salePrtWeightItem();
	}
	else BuzzerSetOnTimes(1);
}
#else
void saleModeKey(void)
{
	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:
			SaleData.saleState = SALE_STATE_PRECOMPARE_1;			
			break;
				
		case SALE_STATE_PRECOMPARE_1:
		case SALE_STATE_PRECOMPARE_2:
			if(saleCountUnitWeight) SaleData.saleState = SALE_STATE_COUNT;
			else SaleData.saleState = SALE_STATE_PRECOUNT;
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SaleData.saleState);	
			break;
				
		case SALE_STATE_PRECOUNT:
		case SALE_STATE_COUNT:
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

/**
********************************************************************************
* @brief    unit 변환 Key 함수    
* @param    none
* @return   none
* @remark   kg, lb, oz, g 순으로 단위가 변경됨 					\n
*           현재 저울에서 사용가능한 단위는 C-4 table 참조 		\n
********************************************************************************
*/
#ifdef USE_UNIT_1_16OZ
void saleUnitChangeKey(void)
{
	INT8U unitIndex[6];	 // { kg, lb, oz, g, ton, 1/16 oz}
	INT8U step;
	
	unitIndex[5] = 1;//1/16 oz
	unitIndex[4] = 0;//ton
	unitIndex[3] = scaleUseGramUnitFlag;
	unitIndex[2] = scaleUseOzUnitFlag; 
	unitIndex[1] = scaleUseLbUnitFlag;
	unitIndex[0] = scaleUseKgUnitFlag;

	if(PrtSetting.method != PRT_USE_BY_KEY && PrtSetting.method != PRT_USE_BY_ENTER)                                 //Print Mode OFF
	{

		if ((SaleData.saleState == SALE_STATE_NORMAL || SaleData.saleState == SALE_STATE_UNITCHANGE) && AdData.tareWeight == 0)
		{	
			step = SaleData.weightUnit;	
			while (1)
			{
				step++;
				step %= 6;

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
	else                                                      //Print Mode ON    
	{
		if(SaleData.saleState == SALE_STATE_NORMAL)
		{
			if(AdData.stableFlag)
			{
				if(SaleData.weight > 0 && PrtSetting.method == PRT_USE_BY_KEY)
				{
					DispSetConstStrMsg(WIN_WEIGHT, "  SUM");
					DispSetRunMsgPage(10, 1);
					saleSumTempWeight = SaleData.weight;
					saleSumWeight += saleSumTempWeight;
					if(saleSumWeight > 99999)
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
						ParamWriteDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_COUNT_BACKUP, saleSumCount);
						ParamWriteDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_WEIGHT_BACKUP, (INT32U)saleSumWeight);
					}
					
				}
				else
				{
					BuzzerSetOnTimes(1);
				}
				
				if(SaleData.weight >= 0 && saleSumCount > 0)
				{
					salePrintKey();
				}	
			}
			else
			{
				BuzzerSetOnTimes(2);
			}
	 	}
	}
}  
#else
void saleUnitChangeKey(void)
{

		INT8U unitIndex[4];	 // { kg, lb, oz, g}
		INT8U step;
		
		unitIndex[3] = scaleUseGramUnitFlag;
		unitIndex[2] = scaleUseOzUnitFlag; 
		unitIndex[1] = scaleUseLbUnitFlag;
		unitIndex[0] = scaleUseKgUnitFlag;
#ifdef USE_PRINTER
	if(PrtSetting.method != PRT_USE_BY_KEY && PrtSetting.method != PRT_USE_BY_ENTER)                                 //Print Mode OFF
	{
		if ((SaleData.saleState == SALE_STATE_NORMAL || SaleData.saleState == SALE_STATE_UNITCHANGE) && AdData.tareWeight == 0)
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
	else if(PrtSetting.method == PRT_USE_BY_KEY)                                                     //Print Mode ON    
	{
		if(SaleData.saleState == SALE_STATE_NORMAL)
		{
			if(AdData.stableFlag)
			{
				if(SaleData.weight > 0 && PrtSetting.method == PRT_USE_BY_KEY)
				{
					DispSetConstStrMsg(WIN_WEIGHT, "  SUM");
					DispSetRunMsgPage(10, 1);
					saleSumTempWeight = SaleData.weight;
					saleSumWeight += saleSumTempWeight;
					if(saleSumWeight > 99999)
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
						ParamWriteDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_COUNT_BACKUP, saleSumCount);
						ParamWriteDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_WEIGHT_BACKUP, (INT32U)saleSumWeight);
					}
					
				}
				else
				{
					BuzzerSetOnTimes(1);
				}
				
				if(SaleData.weight >= 0 && saleSumCount > 0)
				{
					salePrintKey();
				}	
			}
			else
			{
				BuzzerSetOnTimes(2);
			}
	 	}
	}	
	else if (SaleData.saleState == SALE_STATE_NORMAL && SaleData.weight > 0 && AdData.stableFlag && PrtSetting.method == PRT_USE_BY_ENTER)
	{
		DispSetConstStrMsg(WIN_WEIGHT, " PRT ");
		DispSetRunMsgPage(10, 1);
		salePrtFirstLineFlag = 0;
		salePrtWeightItem();
	}
	else BuzzerSetOnTimes(1);
#else
	if ((SaleData.saleState == SALE_STATE_NORMAL || SaleData.saleState == SALE_STATE_UNITCHANGE) && AdData.tareWeight == 0)
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
#endif
}
#endif

#ifdef USE_PRINTER
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
				if(SaleData.weight == 0)
				{
					salePrtWeightTotal();
					salePrtFirstLineFlag = 1;
				}
				else
				{
					if(PrtSetting.method != PRT_NOT_USE)
					{
						salePrtWeightItem();
					}
				}
			}
			else
			{
				BuzzerSetOnTimes(1);
			}
			break;

		default:
			BuzzerSetOnTimes(1);
			break;
			
 	}
 }
#endif
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
#ifdef USE_UNIT_1_16OZ
/**
********************************************************************************
* @brief    1/16 oz 무게를 Display 하는 함수   
* @param    weight
* @return   none
* @remark   
********************************************************************************
*/
void saleDispWeight1_16OZ(INT32S weight)
{
	INT32S	weightLb;
	INT32S	weightOz;
	INT8U	weightOz1_16;
	INT8S 	weightSign;
	INT8U 	weightAuxSign;
	char	ozStr[8];


	if (weight < 0) 
	{
		weight = -weight;
		weightSign = -1;
	}
	else
	{
		weightSign = 1;
	}

	weightOz1_16 = weight % 16;//1/16 oz
	weightOz = weight / 16;//oz
	weightLb = weightOz / 16; //lb
	weightOz = weightOz % 16;
	
	//Display 1/16 oz				
	if (weightOz1_16 == 0)
	{
		DispSetFraction(WIN_WEIGHT, 0xff, 0xff);
	}
	else if ((weightOz1_16 & 0x07) == 0)
	{
		DispSetFraction(WIN_WEIGHT, weightOz1_16 / 8, 2);
	}
	else if ((weightOz1_16 & 0x03) == 0)
	{
		DispSetFraction(WIN_WEIGHT, weightOz1_16 / 4, 4);
	}
	else if ((weightOz1_16 & 0x01) == 0)
	{
		DispSetFraction(WIN_WEIGHT, weightOz1_16 / 2, 8);
	}
	else
	{
		DispSetFraction(WIN_WEIGHT, weightOz1_16, 16);
	}

	//Display lb, oz
	if (weightLb == 0)
	{
		if (weightOz == 0)
		{
			saleDispNumWithDecPoint(WIN_WEIGHT, 0, 0, 0, 0);
			if(weightOz1_16 != 0)
			{
				if (weightSign == -1)
				{
					DispSetChar(WIN_WEIGHT, DispGetWinSize(WIN_WEIGHT) - 1, '-');
				}
				else
				{
					DispSetChar(WIN_WEIGHT, DispGetWinSize(WIN_WEIGHT) - 1, ' ');
				}
			}
		}
		else
		{
			saleDispNumWithDecPoint(WIN_WEIGHT, 0, 0, (weightSign*weightOz), 0);
		}
	}
	else
	{
		weightAuxSign = Int32sToDecStr(&ozStr[0], weightSign*weightLb, 2, 0, 0, 0, 0);
		ozStr[2] = '@';//
		Int32sToDecStr(&ozStr[3], weightOz, 2, 0, 0, 0, 0);
		ozStr[5] = 0;
		DispSetStr(WIN_WEIGHT, ozStr);
		DispSetIndicator(INDICATOR_WEIGHT_MINUS, weightAuxSign);
	}
}
/**
********************************************************************************
* @brief    1/10 oz 무게를 Display 하는 함수   
* @param    weight
* @return   none
* @remark   
********************************************************************************
*/
void saleDispWeight1_10OZ(INT32S weight, INT8U decPos)
{
	INT32S	weightLb;
	INT32S	weightOz;
	INT8U	weightOz1_10;
	INT8S 	weightSign;
	INT8U 	weightAuxSign;
	char	ozStr[8];

	if (decPos < 3)
	{
		if (weight < 0) 
		{
			weight = -weight;
			weightSign = -1;
		}
		else
		{
			weightSign = 1;
		}

		weightOz1_10 = weight % 10;//1/10 oz
		weightOz = weight / 10;//oz
 		weightLb = (weightOz / DecPowerConvTable[decPos-1]) / 16; //lb
		weightOz -= (weightLb * 16) * DecPowerConvTable[decPos-1];
		
		//Display lb, oz
		if (weightLb == 0)
		{
			saleDispNumWithDecPoint(WIN_WEIGHT, decPos - 1, 0, weightSign*weightOz, 0);
			if (weightOz == 0)
			{
				if (weightSign == -1)
				{
					DispSetChar(WIN_WEIGHT, DispGetWinSize(WIN_WEIGHT) - decPos - 1, '-');
				}
			}
		}
		else
		{
			weightAuxSign = Int32sToDecStr(&ozStr[0], weightSign * weightLb, 3 - decPos, 0, 0, 0, 0);
			ozStr[3 - decPos] = '@';//
			Int32sToDecStr(&ozStr[4 - decPos], weightOz, decPos + 1, 0, decPos - 1, 0, 0);
			ozStr[5] = 0;
			DispSetStr(WIN_WEIGHT, ozStr);
			DispSetIndicator(INDICATOR_WEIGHT_MINUS, weightAuxSign);
		}

		//Display 1/10 oz and set dec pos				
		DispSetFraction(WIN_WEIGHT, 0xf0 + decPos - 1, weightOz1_10);
	}
	else
	{
		saleDispNumWithDecPoint(WIN_WEIGHT, decPos, weightDotTypeFlag, weight, 0);
		DispSetFraction(WIN_WEIGHT, 0xff, 0xff);
	}

}
#endif//#ifdef USE_UNIT_1_16OZ

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

#ifdef USE_PRINTER
void salePrtStartCondition(void)
{
		INT8U	tempStr[25];
#ifndef USE_NOT_UNDERLOAD_ERROR
		if(AdData.underloadFlag) return; 
#endif
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
					PrtPutConstStr("  Count");
					PrtPutTab(4);
					PrtPutConstStr("   Weight");	 //무게 단위 삭제 (실제 무게와 함께 단위 표시되도록 변경)					
					PrtPutCRLF(1);
					PrtPutConstStr(" ------------------------"); 
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
						PrtPutDecNum(TimeData.month, 2, PRT_NUM_TYPE_FILL_ZERO);		PrtPutStr("/");
						PrtPutDecNum(TimeData.date, 2, PRT_NUM_TYPE_FILL_ZERO); 		
						 
						PrtPutCRLF(1);
					}
					if (PrtSetting.timeUse)
					{
						TimeReadData();
						PrtPutStr(" TIME. ");
						PrtPutDecNum(TimeData.hour, 2, PRT_NUM_TYPE_FILL_ZERO); 		PrtPutStr(":");
						PrtPutDecNum(TimeData.min, 2, PRT_NUM_TYPE_FILL_ZERO);		PrtPutStr(":");
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
						PrtPutDecNum(TimeData.date, 2, PRT_NUM_TYPE_FILL_ZERO); 	PrtPutStr("/");
						PrtPutDecNum(TimeData.month, 2, PRT_NUM_TYPE_FILL_ZERO);		PrtPutStr("/");
						PrtPutDecNum(2000 + TimeData.year, 4, PRT_NUM_TYPE_FILL_ZERO); 
						PrtPutCRLF(1);
					}
					if (PrtSetting.timeUse)
					{
						TimeReadData();
						PrtPutStr(" TIME. ");
						PrtPutDecNum(TimeData.hour, 2, PRT_NUM_TYPE_FILL_ZERO); 		PrtPutStr(":");
						PrtPutDecNum(TimeData.min, 2, PRT_NUM_TYPE_FILL_ZERO);		PrtPutStr(":");
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
						PrtPutDecNum(TimeData.date, 2, PRT_NUM_TYPE_FILL_ZERO); 		PrtPutStr("/");
						PrtPutDecNum(2000 + TimeData.year, 4, PRT_NUM_TYPE_FILL_ZERO); 
						PrtPutCRLF(1);
					}
					if (PrtSetting.timeUse)
					{
						TimeReadData();
						PrtPutStr(" TIME. ");
						PrtPutDecNum(TimeData.hour, 2, PRT_NUM_TYPE_FILL_ZERO); 		PrtPutStr(":");
						PrtPutDecNum(TimeData.min, 2, PRT_NUM_TYPE_FILL_ZERO);		PrtPutStr(":");
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
#ifndef USE_NOT_UNDERLOAD_ERROR
	if(AdData.underloadFlag) return;
#endif
	if (salePrtFirstLineFlag) 
	{
		PrtPutCRLF(1);
		salePrtStartCondition();
		PrtPutCRLF(1);
		salePrtFirstLineFlag = 0;
	}
	if(saleSumCount)
	{
		PrtPutNum(saleSumCount, 4, 0, weightDotTypeFlag, 0);
		PrtPutTab(4);
	}

	PrtPutNum(SaleData.weight, 7, SaleData.weightDecPos, weightDotTypeFlag, 0);

#ifndef NO_USE_PRT_WEIGHT_UNIT
	PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);				  //프린터 출력시 무게 단위 추가 (OIML)
#endif

	PrtPutCRLF(1);
}

/**
********************************************************************************
* @brief    CAS 22 Byte  함수\n
* @param   none
* @return   none
* @remark   STB , NTB , DIB LSB , WDB   UNB CR LF 
*                 STB -> stable('ST'), unstable('US'), overload('OL')
*                 NTB -> gross('GS'), net('NT')
*                 DIB -> device id('0')
*                 LSB -> 8bit(7:1, 6:stable, 5:1, 4:hold, 3:print, 2:gross, 1:net, 0:zero) 
*                 WDB -> weight data(8byte)
*                 BLANK -> '  '
*                 UNB -> kilo gram('KG'), pound('LB'), once('OZ')
********************************************************************************
*/
void salePrtCas22Byte(void)
{
	char strBuf[22];
	char sendByte=0xa0;
	INT8U i;

	//if(pgmPrtType == PRT_USE_BY_FLOW)	//Stream Mode (DP Model)
	{
		Int32sToDecStr(strBuf, AbsInt32s(SaleData.weight),6, ' ', 0, 0, 0);

		for(i=0;i<6;i++)
		{
			strBuf[i+11] = strBuf[i]; 
		}	
		if(AdData.stableFlag)
		{
			strBuf[0] = 'S';
			strBuf[1] = 'T';
			sendByte = sendByte | 0x40;
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
			sendByte = sendByte | 0x04;
			sendByte = sendByte | 0x02;
		}
		else							
		{
			strBuf[3] = 'G';	 
			strBuf[4] = 'S';
			sendByte = sendByte | 0x00;
		}
		if(SaleData.saleState == SALE_STATE_HOLD) 	 	sendByte = sendByte | 0x10;
		else 							sendByte = sendByte | 0x00;
		
		if (AdData.grossZeroFlag)		sendByte = sendByte | 0x01;
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
#ifndef USE_NOT_UNDERLOAD_ERROR
	if(AdData.underloadFlag) return;
#endif
	//PrtPutConstStr(const char * src)(" ------------------------"); PrtPutCRLF(1);
	PrtPutConstStr(" ------------------------"); PrtPutCRLF(1);
	PrtPutConstStr("  Sum Total :"); PrtPutTab(4); PrtPutNum(saleSumWeight, 7, SaleData.weightDecPos, weightDotTypeFlag, 0);	
#ifndef NO_USE_PRT_WEIGHT_UNIT
	PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);	  //프린터 출력시 무게 단위 추가 (OIML)
#endif
	PrtPutCRLF(9);
	saleSumWeight = 0;
	saleSumCount = 0;
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
#ifndef USE_NOT_UNDERLOAD_ERROR
	if(AdData.underloadFlag) return;
#endif
	PrtPutCRLF(1);
	PrtPutConstStr(" -WEIGHT LIMIT SET VALUE-"); PrtPutCRLF(1);
	PrtPutConstStr(" Limit(H) :");PrtPutTab(1);
	PrtPutNum(saleCompareMaxWeight, 7, SaleData.weightDecPos, weightDotTypeFlag, 0);
	PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);	PrtPutCRLF(1);
	PrtPutConstStr(" Limit(L) :");PrtPutTab(1);
	PrtPutNum(saleCompareMinWeight, 7, SaleData.weightDecPos, weightDotTypeFlag, 0);
	PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);	PrtPutCRLF(1);
	PrtPutCRLF(9);
	salePrtFirstLineFlag = 1;
}
#else
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
}
#endif //USE_PRINTER
#endif
