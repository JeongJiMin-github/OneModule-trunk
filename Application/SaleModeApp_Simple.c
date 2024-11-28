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
#include "../Communication/CommDrv.h"
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
#include "../UI/KeyDrv.h"
#include "../UI/DispDrv.h"
#ifdef USE_SALE_SIMPLE
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
/** @brief bit 1, Viss unit 사용여부, 0: Don't Use 1: Use*/
#ifdef USE_VISS_UNIT_CHANGE
static BOOLEAN scaleUseVissUnitFlag;	//TODO : 접두어 sale 필요??
#endif

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
static INT16U holdCount;	//Haning type INT16U / TODO : 접두어 sale 필요??      duckspg hold test
/** @brief */
static INT16U holdDisplayFlash;	//TODO : 접두어 sale 필요??
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
#if defined(USE_WEIGHT_ONLY_ECR) || defined(USE_PDN_ECR)
/** @brief TARE KEY block flag*/
INT8U NoUseTarekey;
#endif
static INT8U saleDateType;

/** @brief Last digit invalid function enable  flag*/
static INT32U salePreWeight;

/** @brief Scale Initial zero error state flag*/
#ifdef USE_STREAM_CAS_22BYTE
INT8U flowPrtFlag;		//Stream Mode Enable flag (DP Model)
#endif
#ifdef USE_COMMAND_KEY
static BOOLEAN keycommSetErrFlag;
#endif
#ifdef USE_AUTO_TARE_FUNCTION
/** @brief Auto Tare Set flag */
static BOOLEAN SaleAutoTareFlag;
#endif
#ifdef USE_DISPLAY_SUM_TOTAL
extern INT8U sumDisplayStatus;
#endif

/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
#ifdef USE_M0516LDN_MCU
    #define COMPARE_OK_MAX_BUZZER_CNT   80
#else
    #define COMPARE_OK_MAX_BUZZER_CNT   15
#endif
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
#ifdef USE_COMMAND_KEY
static void 	saleKeyCommErrFlagSet(BOOLEAN flag);
#endif
void SetSaleZeroKey(void);
#ifdef USE_COMMAND_TARE_KEY
void RunSaleWeightTareProc(void);
#endif

/** @brief Key에 따른 Sale Function(key input type) 개수 */
#define SALE_KEY_MAX_FUNC_NUM	3
#define SALE_COUNT_ENABLE_CAPA	1200
#define SALE_COUNT_LOW_CAPA		1
#define SALE_PERCENT_LOW_CAPA	1800


/** @brief hold average time */
#define HOLD_AVERAGE_TIME      10	// hold 잡는 시간 (10 = 1sec) duckspg hold test
/** @brief Key에 따른 Sale Function(key input type) 개수 */
#define HOLD_DISPLAY_TIME      50	// hold 잡는 시간 (10 = 1sec) duckspg hold test

/** @brief Key에 따른 Sale Function(void type) 개수 */

#ifndef NO_USE_HOLD_KEY
#define HOLD_KEY_FUNC_NUM	1
#else
#define HOLD_KEY_FUNC_NUM	0
#endif

#ifdef USE_BACK_LIGHT
#define BACKLIGHT_KEY_FUNC_NUM	1
#else
#define BACKLIGHT_KEY_FUNC_NUM	0
#endif

/** @brief Key에 따른 Sale Function(void type) 개수 */
#define SALE_KEY_MAX_VOID_FUNC_NUM	(6+HOLD_KEY_FUNC_NUM + BACKLIGHT_KEY_FUNC_NUM)

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
	{KEY_HOLD,				saleHoldKey},
#endif
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
#ifdef USE_UNITEDSTATES_DEFAULT	// Only SW
			/*ECR TYPE4 기본 설정*/
			ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_BAUDRATE, BAUD_RATE_9600);
			ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_ECR_TYPE, ECR_TYPE_4);
			ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_1ST, 0);		// PRINT METHOD
			ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_SELECT, 1);	// 0:PRT, 1:ECR
	
			ParamWriteByte(PARAM_KEY_AREA_NUM,	KEY_PARAM_OFFSET_TYPE, 0x02);	// 2: UNIT CHANGE를 기본 값으로 설정
#else
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST, 0xC8);
  #ifdef USE_ALL_WEIGHT_UNIT
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0x7F);		
  #else
    #ifdef USE_PDN_USA_SETTING
			ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_BAUDRATE, BAUD_RATE_9600);
			ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_ECR_TYPE, ECR_TYPE_4);	// USB ECR TYPE
			StorageWriteByte(STORAGE_EXT_PARAM_BASE_ADDR, ECR_TYPE_4);						// RS232 ECR TYPE
			ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_SELECT, RS232_ECR); 		// USE USB/RS232 ECR
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0x07);

			//CAL11에서 설정되는 부분
			ParamWriteByte(PARAM_KEY_AREA_NUM,	KEY_PARAM_OFFSET_TYPE, 0x00);	// NTEP(01)설정 시 UNIT CHANGE(0)키를 기본 값으로 설정
			ParamWriteByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_TYPE, 0x00);	// UNIT Indicator 표시사용
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SALE_STATE_NORMAL);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_ON, OFF);
    #else
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0x73);
    #endif
  #endif
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_3RD, 0x23);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, 0x24);
#endif
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

		case TURKEY:
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST, 0x38);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0x31);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_3RD, 0x03);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, 0x25);
			ParamWriteData(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PRICE_SYMBOL, "$", PRICE_SYMBOL_SIZE); 	// first unit

			AdWriteParam(TARE_SUCCESSIVE, TARE_TYPE_SUCCESSIVE_PLUS);
			AdWriteParam(ZERO_MARK_INDICATOR, ZERO_MARK_TYPE_ALL);

			ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_BAUDRATE, BAUD_RATE_9600);
			ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_ECR_TYPE, ECR_TYPE_15);	// USB ECR TYPE
			StorageWriteByte(STORAGE_EXT_PARAM_BASE_ADDR, ECR_TYPE_15);						// RS232 ECR TYPE
			ParamWriteByte(PARAM_COMM_AREA_NUM, COMM_PARAM_OFFSET_SELECT, RS232_ECR); 		// USE USB/RS232 ECR
			break;

		default: 		// OIML
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST, 0x38);
#ifdef USE_ALL_WEIGHT_UNIT
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0x2F);		
#elif defined(USE_PDN_USA_SETTING)
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0x07);

			//CAL11에서 설정되는 부분
			ParamWriteByte(PARAM_KEY_AREA_NUM,	KEY_PARAM_OFFSET_TYPE, 0x00);	// NTEP(00)설정 시 UNIT CHANGE(0)키를 기본 값으로 설정
			ParamWriteByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_TYPE, 0x00);	// UNIT Indicator 표시사용
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SALE_STATE_NORMAL);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_ON, OFF);
#elif defined USE_PDN_CANADA_SETTING
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0x0B);	// kg, lb, g 사용
			ParamWriteByte(PARAM_KEY_AREA_NUM,	KEY_PARAM_OFFSET_TYPE, 0x00);	// UNIT CHANGE(0)키를 기본 값으로 설정
			ParamWriteByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_TYPE, 0x00);	// UNIT Indicator 표시사용
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
	/** @brief PLU Data & Report Data Clear*/
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
	/** @brief bit 1 & bit 0, printer type, 00 : Don't Use, 01: Use DEP, 10: Use DLP, 11: reserve*/
#ifdef USE_STABLE_ZERO_WEIGHT_PRT	
	INT8U pgmPrinterType;
#endif
	
	SaleData.weightUnit = AdData.weightUnit;
	SaleData.weightDecPos = AdData.weightDecPos;	
#ifdef USE_STABLE_ZERO_WEIGHT_PRT
	// Printer Type(ONLY DEP)
	tempInt8u = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH);
	pgmPrinterType = tempInt8u & 0x03;
	if(pgmPrinterType != PRT_DEP50)
	{
		pgmPrinterType = PRT_DEP50;
		tempInt8u &= ~(0x03);
		tempInt8u |= pgmPrinterType;
		ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, tempInt8u);	
	}
#endif

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
#ifdef USE_VISS_UNIT_CHANGE
	scaleUseVissUnitFlag = (readSetDataInSale & 0x04) >> 2;
#else
	scaleUseOzUnitFlag = (readSetDataInSale & 0x04) >> 2;
#endif
	scaleUseLbUnitFlag = (readSetDataInSale & 0x02) >> 1;
	scaleUseKgUnitFlag = (readSetDataInSale & 0x01) >> 0;

#ifdef USE_VISS_UNIT_CHANGE
	tempInt8u = (INT8U)scaleUseGramUnitFlag + (INT8U)scaleUseVissUnitFlag + (INT8U)scaleUseLbUnitFlag + (INT8U)scaleUseKgUnitFlag;
#else
	tempInt8u = (INT8U)scaleUseGramUnitFlag + (INT8U)scaleUseOzUnitFlag + (INT8U)scaleUseLbUnitFlag + (INT8U)scaleUseKgUnitFlag;
#endif

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

	SaleData.saleState = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS);

#ifdef USE_COUNT_COMPARE
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
	saleCompareEnable = COMPARE_MODE_OFF;
	salePcs = 0; 
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
	holdDisplayFlash = 0;
#if defined(DIALOG6) || defined(USE_SMART_BT_PROTOCOL)
	SaleData.keyTare = 0;
#endif
	saleSumWeight = 0;
	saleSumTempWeight = 0;
	saleSumCount = 0;
	saleSumCancelEnable = 0;
	salePrtFirstLineFlag = 1;
	salePrtAutoEnable = 1;
	salePrtAutoCount = 0;

	
	saleTareKeyPressed = 0;
	saleZeroKeyPressed = 0;

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

#ifdef USE_AUTO_TARE_FUNCTION
	SaleAutoTareFlag = 0;
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
#ifdef USE_STABLE_ZERO_WEIGHT_PRT
	static INT16U prevSendWeightTimer100ms = 0;
	INT16U currTimer100ms;
#endif
#ifdef USE_PRT_STABLE_WEIGHT
	static INT32S prtEnable = 0;
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
#ifdef USE_STABLE_ZERO_WEIGHT_PRT
		if (SaleData.weight >= 0)
		{
			currTimer100ms = TimerGetSysTimer100ms();
			if ((INT16U)(currTimer100ms - prevSendWeightTimer100ms) > 1)
			{
				prevSendWeightTimer100ms = currTimer100ms;
				switch(SaleData.saleState)
				{
					case SALE_STATE_NORMAL:
						salePrtFirstLineFlag = 0;
						salePrtWeightItem();
						break;			
				}
			}
			salePrtAutoEnable = 0;
			salePrtFirstLineFlag = 0;
		}
#else
		salePrtFirstLineFlag = 0;
		
		switch(SaleData.saleState)
		{
			case SALE_STATE_NORMAL:
				salePrtWeightItem();					
				break;
		}
#endif		
	}			
#ifdef USE_STABLE_ZERO_WEIGHT_PRT
//	if (PrtSetting.method == PRT_USE_BY_AUTO && salePreWeight != AdData.weightData)
	if (PrtSetting.method == PRT_USE_BY_AUTO && SaleData.weight >= 0)		
#elif defined(USE_PRT_STABLE_WEIGHT)
	if (PrtSetting.method == PRT_USE_BY_AUTO && prtEnable && SaleData.weight > 0)
#else
	if (PrtSetting.method == PRT_USE_BY_AUTO && salePrtAutoEnable && SaleData.weight > 0)
#endif
	{
		if (AdData.stableFlag)
		{
#ifdef USE_STABLE_ZERO_WEIGHT_PRT
			currTimer100ms = TimerGetSysTimer100ms();
			
			if ((INT16U)(currTimer100ms - prevSendWeightTimer100ms) > 1)
			{
				prevSendWeightTimer100ms = currTimer100ms;
#endif		
				switch(SaleData.saleState)
				{
					case SALE_STATE_NORMAL:
#if defined(USE_PGM_SW) || defined(USE_PGM_PB) || defined(USE_PGM_FW)|| defined(USE_PGM_PDN)
						salePrtFirstLineFlag = 0;
						salePrtWeightItem();
#ifdef USE_PRT_STABLE_WEIGHT
						salePreWeight = AdData.weightData;
#endif
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
#ifdef USE_STABLE_ZERO_WEIGHT_PRT		
			}
#else
			BuzzerSetOnTimes(1);
#endif					
			salePrtAutoEnable = 0;
			salePrtFirstLineFlag = 0;
#ifdef USE_PRT_STABLE_WEIGHT
			prtEnable = 0;
#endif
		}
	}
	if (AdData.stableFlag)
	{
		if (SaleData.weight <= 0)
		{
			salePrtAutoEnable = 1;
#ifdef USE_PRT_STABLE_WEIGHT
			salePreWeight = AdData.weightData;
#endif
		}
#ifdef USE_PRT_STABLE_WEIGHT
		if (AdExtToIntValueExtern(AbsInt32s(salePreWeight - AdData.weightData)) >= AdData.d1 * 3)
		{
			prtEnable = 1;
		}
#endif

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
#ifdef AUTO_ZERO
    AutoSetZero();
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
	return retValue;
}
//kkh0225 code optimaization
#if defined(USE_BT_COMM_API) || defined(USE_TM_COMM_API) || defined(USE_SMART_BT_PROTOCOL)
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
	saleZeroKeyProc();
	BuzzerSetOnTimes(1);
}
/**
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
	}
	else 
	{
		BuzzerSetOnTimes(1);
		CommSetKeyCmdErrFlag(ON);
	}
}
#endif	//USE_COMMAND_KEY
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
				if (saleBuzzerCount == COMPARE_OK_MAX_BUZZER_CNT)
				{
					BuzzerSetOnTimes(1);
					saleBuzzerCount = 0;

//#ifdef USE_AUTO_TARE_FUNCTION
//					if(KeyCheckAutoTareFunction() && !SaleAutoTareFlag) saleTareKey();
//#endif
				}
				else
				{
					saleBuzzerCount++;
				}
			}
		}
	}
#endif

#ifdef USE_AUTO_TARE_FUNCTION
	if(AdData.stableFlag && (SaleData.weight > 0))
	{
		if(KeyCheckAutoTareFunction() && (SaleAutoTareFlag == 0))
		{
			saleTareKey();
			SaleAutoTareFlag = 1;
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
		|| SaleData.saleState == SALE_STATE_UNITCHANGE || SaleData.saleState == SALE_STATE_SUM_COUNT)				 
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
#ifdef USE_VISS_UNIT_CHANGE
			case UNIT_VISS:
				DispSetIndicator(INDICATOR_VISS, ON);
				break;
#else
			case UNIT_OZ:
				DispSetIndicator(INDICATOR_OZ, ON);			
				break;
#endif
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

	if(saleTareClearFlag && AdData.grossZeroFlag)
	{
		returnData = AdSetTare();
#ifdef USE_AUTO_TARE_FUNCTION
		SaleAutoTareFlag = 0;
#endif
	}

	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:
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
				if(AdData.overloadFlag)
				{
					SaleData.holdWeight = 0;
					SaleData.saleState = SALE_STATE_NORMAL;
				}
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
                        if (holdCount != 0xFFFF)
    						holdCount++;
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
#ifdef USE_UL_DISPLAY
		else if (SaleData.weightUnit == UNIT_OZ && (!KeyGetUlDisplayFlag()))
#else
                else if (SaleData.weightUnit == UNIT_OZ)
#endif
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
	if (SaleData.saleState != SALE_STATE_NORMAL && SaleData.saleState != SALE_STATE_COUNT && SaleData.saleState != SALE_STATE_PERCENT)
	{
		PowerSaverResetTimer(TIMER_DELAY_100MS_STAND_BY);
	}

	switch (SaleData.saleState)		/*	display	*/
	{
		case SALE_STATE_NORMAL:
#ifdef USE_DISPLAY_WEIGHT_ONLY_WHEN_STABLE
				if (!AdData.stableFlag) {
					break;
				}
#endif
				saleDisplayWeight(SaleData.weight);
				DispSetIndicator(INDICATOR_HOLD, OFF);
			break;
		case SALE_STATE_HOLD:
				DispSetIndicator(INDICATOR_HOLD, ON);

                if (holdCount == HOLD_DISPLAY_TIME + HOLD_AVERAGE_TIME)
                {
                    SaleData.saleState = SALE_STATE_NORMAL;
					DispSetIndicator(INDICATOR_HOLD, OFF);
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
#ifdef USE_COMMAND_KEY
		saleKeyCommErrFlagSet(OFF);
#endif
		BuzzerSetOnTimes(2);
	}
	else
	{
#ifdef USE_COMMAND_KEY
		saleKeyCommErrFlagSet(ON);
#endif	
	}
	TimerSetDelay100ms(TIMER_DELAY_100MS_WAIT_STABLE, 50);
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

#ifdef USE_COUNT_COMPARE			
		case SALE_STATE_PRECOMPARE_1:
			if(saleCompareEnable == COMPARE_MODE_ON)
			{
				EditorSetDataAndRun(WIN_WEIGHT, DEC, (INT32U)saleCompareMinWeight, 1, AdData.maxCapa, SaleData.weightDecPos, weightDotTypeFlag, 0);
				SaleData.saleState = SALE_STATE_PRECOMPARE_2;
			}
 			break; 
 			
		case SALE_STATE_PRECOMPARE_2:
			EditorGetData((INT32U*)&saleCompareMinWeight);
			ParamWriteDword(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MIN_WEIGHT, (INT32U)saleCompareMinWeight); 
			if(saleCompareEnable == COMPARE_MODE_ON)
			{
				EditorSetDataAndRun(WIN_WEIGHT, DEC, (INT32U)saleCompareMaxWeight, (INT32U)saleCompareMinWeight+1, AdData.maxCapa, SaleData.weightDecPos, weightDotTypeFlag, 0);
				SaleData.saleState = SALE_STATE_PRECOMPARE_3;
			}
			break;
			
		case SALE_STATE_PRECOMPARE_3:
			EditorGetData((INT32U*)&saleCompareMaxWeight);
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
#ifdef USE_COMMAND_KEY
		saleKeyCommErrFlagSet(OFF);
#endif
		BuzzerSetOnTimes(2);
	}
	else
	{
#ifdef USE_COMMAND_KEY
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
	INT8U tempInt8u;	

#if defined(USE_WEIGHT_ONLY_ECR) || defined(USE_PDN_ECR)
	if(NoUseTarekey == 1)
	{
		BuzzerSetOnTimes(2);
		return;
	}
#endif
	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:
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
				if (salePcs)
				{
					if(SaleData.weight > 0)
					{
						saleCountWeight = AdData.rawSetTareData;
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
			//tempInt8u = (saleCountCompareEnable << 1) | saleCompareEnable;
			tempInt8u = saleCompareEnable;
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_ON, tempInt8u);
			break;

		default:
			BuzzerSetOnTimes(2);
			break;
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
					holdCount = 0;
					SaleData.holdWeight = 0;
					holdDisplayFlash = 0;
					SaleData.saleState = SALE_STATE_HOLD;
 #ifdef USE_COMMAND_KEY
					saleKeyCommErrFlagSet(ON);
 #endif
					TimerSetDelay100ms(TIMER_DELAY_100MS_AUTO_HOLD, 1);
				}
				else
				{
 #ifdef USE_COMMAND_KEY
					saleKeyCommErrFlagSet(OFF);
 #endif
					BuzzerSetOnTimes(2);
				}
				break;
			
			default: 
 #ifdef USE_COMMAND_KEY
				saleKeyCommErrFlagSet(OFF);
 #endif
				BuzzerSetOnTimes(2);
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
#ifdef USE_DISPLAY_SUM_TOTAL
					DispSetRunMsgPage(10, 0);					// 짐판위에 무게가 있는 상태에서
					sumDisplayStatus = DISPLAY_TOTAL_WEIGHT;	// Hold키 입력 시 합산 무게 표시 모드로 변경
#else
					DispSetRunMsgPage(10, 1);
#endif
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
#ifdef USE_DISPLAY_SUM_TOTAL
					if( sumDisplayStatus == DISPLAY_IDLE )					// 짐판 위 무게가 없는 상태에서 Hold 키 입력 시
					{
						DispSetConstStrMsg(WIN_WEIGHT, "TOTAL");			// 합산 된 무게 정보를 표시하는 모드로 전환
						DispSetRunMsgPage(10, 0);
						KeyEnable(ON);
						sumDisplayStatus = DISPLAY_G_TOTAL_WEIGHT;
					}
					else if( (sumDisplayStatus == DISPLAY_G_TOTAL_WEIGHT) || (sumDisplayStatus == DISPLAY_G_TOTAL_COUNT) || (sumDisplayStatus == DISPLAY_END) )
					{
						sumDisplayStatus = DISPLAY_INIT;					// 합산 된 무게 정보가 표시되고 있는 상태에서 Hold 키 입력 시 초기화 모드로 전환
					}
#endif
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
//#define CTS_RESET
void saleHoldKey(void)
{
#ifdef CTS_RESET
        static INT8U yearOldchangeflag = 0;
        INT8U counterTemp;
#endif
  
	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:
#ifdef CTS_RESET
                if (yearOldchangeflag == 0)
                {
                    TimeReadData();
                    yearOldchangeflag = 1;
                    counterTemp = ParamReadByte(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_HISTORY_COUNT);
                    counterTemp = counterTemp % CAL_HISTORY_MAX_NUM;
                    ParamWriteByte(PARAM_AD_AREA_NUM, (AD_PARAM_OFFSET_HISTORY + (counterTemp*21))+2,TimeData.year-4);
                    ParamWriteByte(PARAM_AD_AREA_NUM, (AD_PARAM_OFFSET_HISTORY + (counterTemp*21))+3,TimeData.month);
                    ParamWriteByte(PARAM_AD_AREA_NUM, (AD_PARAM_OFFSET_HISTORY + (counterTemp*21))+4,TimeData.date);
                    ParamWriteByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_VALID_START_WAIT_COUNT,0);
                    ParamWriteByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_CAL_PASSWORD_ENABLE_KEY,1);
                }
              
  
#endif

			if (SaleData.weight > 0)
			{
				holdCount = 0;
				SaleData.holdWeight = 0;
				holdDisplayFlash = 0;
				SaleData.saleState = SALE_STATE_HOLD;
#ifdef USE_HOLD_FAST_DISPLAY
				TimerSetDelay100ms(TIMER_DELAY_100MS_AUTO_HOLD, 1);
#else
                TimerSetDelay100ms(TIMER_DELAY_100MS_AUTO_HOLD, 10);
#endif
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
#ifdef USE_UL_DISPLAY
			while (1)
			{
				step++;
				step %= 6;
                if(KeyGetUlDisplayFlag())
                    if (step == 5)
                        step = 0;
				if (unitIndex[step] == 1)
				{
					if (AdSetWeightUnitFlag(step)) 
					{
						break;
					}
				}
			}
#else
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
#endif
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
#ifdef USE_VISS_UNIT_CHANGE
		INT8U unitIndex[12];	 // { kg, lb, oz, g}
#else
		INT8U unitIndex[4];	 // { kg, lb, oz, g}
#endif
		INT8U step;

#ifdef USE_VISS_UNIT_CHANGE
		unitIndex[11] = scaleUseVissUnitFlag; 
#endif
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
#ifdef USE_VISS_UNIT_CHANGE
				step %= 12;
#else
				step %= 4;
#endif				
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
#ifdef USE_DISPLAY_SUM_TOTAL
					DispSetRunMsgPage(10, 0);
					sumDisplayStatus = DISPLAY_TOTAL_WEIGHT;
#else
					DispSetRunMsgPage(10, 1);
#endif
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
#ifdef USE_DISPLAY_SUM_TOTAL
					if( sumDisplayStatus == DISPLAY_IDLE )					// 짐판 위 무게가 없는 상태에서 Hold 키 입력 시
					{
						DispSetConstStrMsg(WIN_WEIGHT, "TOTAL");			// 합산 된 무게 정보를 표시하는 모드로 전환
						DispSetRunMsgPage(10, 0);
						KeyEnable(ON);
						sumDisplayStatus = DISPLAY_G_TOTAL_WEIGHT;
					}
					else if( (sumDisplayStatus == DISPLAY_G_TOTAL_WEIGHT) || (sumDisplayStatus == DISPLAY_G_TOTAL_COUNT) || (sumDisplayStatus == DISPLAY_END) )
					{
						sumDisplayStatus = DISPLAY_INIT;					// 합산 된 무게 정보가 표시되고 있는 상태에서 Hold 키 입력 시 초기화 모드로 전환
					}
#endif
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
#ifdef COUNTRY_MYANMAR
  #if defined(MODEL_OMV4_SW2W_LED) & defined(USE_VISS_UNIT_CHANGE)
			// Gram(3) -> KG(0) -> LB(1) -> Viss(11)
			switch(step)
			{
				case UNIT_GRAM:	step = 0;  break;
				case UNIT_KG:	step = 1;  break;
				case UNIT_LB:	step = 11; break;
				case UNIT_VISS:	step = 3;  break;
			}
  #endif
#else
			step++;
			step %= 4;
#endif
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
#ifndef USE_NOT_UNDERLOAD_ERROR
	if(AdData.underloadFlag)	return;
#endif
	if(scaleUsePrt == PRT_DLP50)
	{
  #ifdef USE_PRINT_CERTIFICATION
		if(AdData.overloadFlag == TRUE || AdData.underloadFlag == TRUE ||
		   AdData.stableFlag == FALSE  || (AdData.weightData < AdIntToExtValueExtern((INT32S)(AdData.d1)) * 20))
		{
				BuzzerSetOnTimes(3);
				return;
		}
  #endif
		DispSetConstStrMsg(WIN_WEIGHT, " PRINT");
		DispSetRunMsgPage(10, 1);
		salePrtWeightItem();
	}
	else
	{	
		switch (SaleData.saleState)
		{
			case SALE_STATE_NORMAL:
				if(AdData.stableFlag)
				{
					if(SaleData.weight == 0)
					{
#ifdef USE_DISPLAY_SUM_TOTAL		// 해당 디파인이 켜져있는 조건에서는 
						if( sumDisplayStatus == DISPLAY_INIT ) // 합산정보 초기화 모드에서만 토탈정보 인쇄 명령과 초기화를 실행
						{
#endif
#ifndef NO_USE_PRT_WEIGHT_TOTAL
						salePrtWeightTotal();
#endif
						salePrtFirstLineFlag = 1;
#ifdef USE_DISPLAY_SUM_TOTAL
						}
#endif
					}
					else
					{
						if(PrtSetting.method != PRT_NOT_USE)
						{
							//salePrtFirstLineFlag = 1;
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
/**
********************************************************************************
* @brief	DLP-50 사용시 처음에 보내는 Protocol 함수	   
* @param  none
* @return	none
* @remark	
********************************************************************************
*/
#ifdef USE_SIMPLE_PRT_SELECT
void salePrtDlpStartCondition(INT8U printType)
{
	PrtPutCRLF(1); 
	PrtPutConstStr("FR");
	PrtPutChar(0x22);	//"
	switch(printType)
	{
		case TYPE_WEIGHT:
			PrtPutConstStr("PBW");
			break;
		case TYPE_WEIGHT_WITH_BARCODE:
			PrtPutConstStr("PBWB");
			break;
	}
	PrtPutChar(0x22);	//"
	PrtPutCRLF(1);
	PrtPutChar(0x3F);	//?
	PrtPutCRLF(1);
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
#ifndef NO_USE_PRT_WEIGHT_TOTAL
					PrtPutConstStr("  Count");
					PrtPutTab(4);
#endif
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
#ifdef USE_STABLE_ZERO_WEIGHT_PRT
	char weightstr[10];
	char weightstrpos;
#endif
#ifndef USE_NOT_UNDERLOAD_ERROR
	if(AdData.underloadFlag) return;
#endif
	if(scaleUsePrt == PRT_DEP50)
	{	
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
		if(PrtSetting.method == PRT_USE_BY_STREAM || PrtSetting.method == PRT_USE_BY_CHANGE_WEIGHT)
		{
#ifndef USE_STABLE_ZERO_WEIGHT_PRT
			if(AdData.stableFlag) PrtPutChar('S');	//Stable 경우 'S' Flag 추가
			else PrtPutChar('U');	//Unstable 경우 'U' Flag 추가		
#endif
		}
#ifdef USE_STABLE_ZERO_WEIGHT_PRT
		weightstrpos = 0;
		weightstr[weightstrpos++] = STX;

		if(weightDotTypeFlag == DEC_TYPE_DOT)
		{
			Int32sToDecStr(&weightstr[weightstrpos], SaleData.weight, 6, PRT_PUT_NUM_FILL_CHAR, SaleData.weightDecPos, '.', 0);
		}
		else
		{
			Int32sToDecStr(&weightstr[weightstrpos], SaleData.weight, 6, PRT_PUT_NUM_FILL_CHAR, SaleData.weightDecPos, ',', 0);		
		}
		weightstrpos += 6;
//		PrtPutChar(STX);
//		PrtPutNum(SaleData.weight, 6, SaleData.weightDecPos, weightDotTypeFlag, 0);
#else
		PrtPutNum(SaleData.weight, 7, SaleData.weightDecPos, weightDotTypeFlag, 0);
#endif
#if defined NO_USE_PRT_WEIGHT_UNIT
		/* NOT SEND */
#elif defined NO_USE_PRT_WEIGHT_UNIT_EXCEPT_FOR_K_SEND
		if (ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST) == 0x01) { /* 0x01 : K-Send */
			PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);				  //프린터 출력시 무게 단위 추가 (OIML)
		}
#else
		PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);				  //프린터 출력시 무게 단위 추가 (OIML)
#endif
#ifdef	USE_STABLE_ZERO_WEIGHT_PRT
		weightstr[weightstrpos++] = 0x0D;
		PrtPutData(weightstr, weightstrpos);
//		PrtPutChar(0x0D);
#else
		PrtPutCRLF(1);
#endif
	}
#ifdef USE_SIMPLE_PRT_SELECT
	else if(scaleUsePrt == PRT_DLP50)
	{
		if(PrtSetting.barcodeOnOffFlag)
		{
			salePrtDlpStartCondition(TYPE_WEIGHT_WITH_BARCODE);    //Start command weight with barcode type
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
#endif
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
	PrtPutConstStr(" ------------------------"); PrtPutCRLF(1);
	PrtPutConstStr("  Sum Total :"); PrtPutTab(4); PrtPutNum(saleSumWeight, 7, SaleData.weightDecPos, weightDotTypeFlag, 0);	
#if defined NO_USE_PRT_WEIGHT_UNIT
		/* NOT SEND */
#elif defined NO_USE_PRT_WEIGHT_UNIT_EXCEPT_FOR_K_SEND
	if (ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST) == 0x01) { /* 0x01 : K-Send */
		PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);				  //프린터 출력시 무게 단위 추가 (OIML)
	}
#else
	PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);				  //프린터 출력시 무게 단위 추가 (OIML)
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

#endif

#ifdef USE_DISPLAY_SUM_TOTAL
// Unit Indicator ON
void changeWeightUnitIndicator(void)
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
#ifdef USE_VISS_UNIT_CHANGE
		case UNIT_VISS:
			DispSetIndicator(INDICATOR_VISS, ON);
			break;
#else
		case UNIT_OZ:
			DispSetIndicator(INDICATOR_OZ, ON);			
			break;
#endif
		case UNIT_GRAM:
			DispSetIndicator(INDICATOR_GRAM, ON);
			break;
		default:
			break;
	}
}

// Unit Indicator Off
void clearWeightUnitIndicator(void)
{
	switch (SaleData.weightUnit)
	{
		case UNIT_KG:
			DispSetIndicator(INDICATOR_KG, OFF);				
			break;

		case UNIT_LB:
			DispSetIndicator(INDICATOR_LB, OFF);				
			break;
#ifdef USE_UNIT_1_16OZ
		case UNIT_1_16OZ:
#endif
#ifdef USE_VISS_UNIT_CHANGE
		case UNIT_VISS:
			DispSetIndicator(INDICATOR_VISS, OFF);
			break;
#else
		case UNIT_OZ:
			DispSetIndicator(INDICATOR_OZ, OFF);			
			break;
#endif
		case UNIT_GRAM:
			DispSetIndicator(INDICATOR_GRAM, OFF);
			break;
		default:
			break;
	}
}

// 합산된 Total Weight 표시 
void displayTotalWeight(void)
{
	INT32U totalWeight = 0;

	DispDrvSetEditingPage(DISP_DRV_MSG_PAGE);
	totalWeight = ParamReadDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_WEIGHT_BACKUP);
	DispSetNum(WIN_WEIGHT, totalWeight, 0);
	DispSetRunMsgPage(10, 0);
	changeWeightUnitIndicator();
	DispDrvSetEditingPage(DISP_DRV_NORMAL_PAGE);
}

// Total Count 표시
void displayTotalCount(void)
{
	INT32U totalCount = 0;

	DispDrvSetEditingPage(DISP_DRV_MSG_PAGE);
	totalCount = ParamReadDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SUM_COUNT_BACKUP);
	DispSetDecPoint(WIN_WEIGHT, 0, 0, 0);
	DispSetNum(WIN_WEIGHT, totalCount, 0);
	DispSetRunMsgPage(10, 0);
	clearWeightUnitIndicator();
	DispDrvSetEditingPage(DISP_DRV_NORMAL_PAGE);
}
#endif