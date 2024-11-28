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
#include "../SystemTimer/TimerApi.h"
#include "ScaleModeApp.h"
#include "SaleModeApp.h"
#include "../Time/TimeApi.h"
#include "../Common/TimeData.h"

#ifdef USE_SALE_PRICE_EXTERNAL_PLU_AP
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief Sale ���� Data struct */
SALE_DATA SaleData;
/** @brief �ӽ� PLU data*/
DIRECT_PLU_DATA tempPluData;
/** @brief �ӽ� Report data*/
PLU_REPORT_DATA tempPluReport;
/** @brief daily total report data*/
PLU_REPORT_DATA dailyTotalReport;
/** @brief Direct PLU data*/
DIRECT_PLU_DATA PluDirectData;

/** @brief C4-2 setting data */
/** @brief bit 7, PLU ��ȿ�� ��� ���� 0: Don't Use 1: Use*/
static BOOLEAN saleUsePluValidDateFlag;
/** @brief bit 6, PLU tare��뿩��, 0: Don't Use 1: Use*/
static BOOLEAN saleUsePluTareFlag;
/** @brief bit 5, PLU name ��뿩��, 0: Don't Use 1: Use*/
static BOOLEAN saleUsePluNameFlag;
/** @brief bit 4, Daily total ��뿩��, 0: Don't Use 1: Use*/
static BOOLEAN saleUseDailyTotalFlag;
/** @brief bit 3, �ܰ� ���� ��뿩��, 0: Save 1: Clear*/
static BOOLEAN salePriceClearFlag;
/** @brief bit 2, Tare ���� ��뿩��, 0: Save 1: Clear*/
static BOOLEAN scaleTareClearFlag;	//TODO : ���ξ� sale �ʿ�??
/** @brief bit 1, Euro��� ��뿩��, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseEuroFlag;	//TODO : ���ξ� sale �ʿ�??
/** @brief bit 0, ���� On�ÿ� Euro��� ��뿩��, 0: Don't Use 1: Use*/
static BOOLEAN salePowerOnEuroFlag;

/** @brief C4-3 setting data */
/** @brief bit 7, ���� dotǥ�� ����, 0: "."Dot 1: "," Comma*/
static BOOLEAN priceDotTypeFlag;	//TODO : ���ξ� sale �ʿ�??
/** @brief bit 6, Preset tare��뿩��, 0: Don't Use 1: Use*/
static BOOLEAN saleUsePresetTareFlag;
/** @brief bit 5, reserved*/
//backlight, already use another module
/** @brief bit 4, Head message��뿩��, 0: Don't Use 1: Use*/
static BOOLEAN saleUseHeadMsgFlag;
/** @brief bit 3, Gram unit ��뿩��, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseGramUnitFlag;	//TODO : ���ξ� sale �ʿ�??
/** @brief bit 2, Ounce unit ��뿩��, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseOzUnitFlag;	//TODO : ���ξ� sale �ʿ�??
/** @brief bit 1, Pound unit ��뿩��, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseLbUnitFlag;	//TODO : ���ξ� sale �ʿ�??
/** @brief bit 0, Kilo gram unit ��뿩��, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseKgUnitFlag;	//TODO : ���ξ� sale �ʿ�??

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
/** @brief bit 1 & bit 0, 0:10, 1:100,  2:1000, 3:10000   E2_read_data +1 �̴�.*/
static INT8U salePriceForUnit;

/** @brief C4-5 setting data */
/** @brief bit 7, reserved*/
// bit 7 -> reserve
/** @brief bit 6 & bit 5 & bit 4, �ܰ� Decimal point position, 0: 0000,   1: 0.0, 2: 0.00, 3:0.001 4:0.0001*/
static INT8U priceDecPos;	//TODO : ���ξ� sale �ʿ�??
/** @brief bit 3, 0: Don't use 1: Use*/
static BOOLEAN scaleUseCanadaMsgFlag;	//TODO : ���ξ� sale �ʿ�??
/** @brief bit 2, reserved*/
// bit 2 -> reserve
/** @brief bit 1 & bit 0, ��� ������ ����, 00 : Don't Use, 01: Use DEP, 10: Use DLP, 11: reserve*/
static INT8U scaleUsePrt;	//TODO : ���ξ� sale �ʿ�??

/** @brief ���� �Ϸ� �÷���, 0: ready, 1: End*/
static BOOLEAN transactionEndFlag;	//TODO : ���ξ� sale �ʿ�??
/** @brief PrePack ȣ�⿩�� �÷���*/
static BOOLEAN salePrePackFlag;
/** @brief Percent tare ȣ�⿩�� �÷���, 0: no Call, 1: call Data*/
static BOOLEAN salePercentTareCallFlag;
/** @brief ����Ʈ ���� ���� �÷���, 0: Don't use, 1: Use*/
static BOOLEAN salePrtEnableFlag;
/** @brief Tare clear �÷���, 0: save, 1: clear*/
static BOOLEAN saleTareClearFlag;
/** @brief Date ��� �÷���*/
static BOOLEAN salePrtDayFlag;
/** @brief Line ��� �÷���*/
static BOOLEAN salePrtLineFlag;	
/** @brief Unit price, tare �Ѵ� ���̺� �ϴ� �÷��� 0: clear, 1: save*/
static BOOLEAN saleBothSaveFlag;
/** @brief Euro ��뿩�� �÷���, 0: Don't Use 1: Use */
static BOOLEAN saleUseEuroFlag;
/** @brief Euro ���� ��뿩�� �÷���, 0: Don't Use 1: Use */
static BOOLEAN scaleEuroOffFlag; //for AP USD ǥ�� off
/** @brief ���� Display���� ����ϴ� Dot type, 0: Dot 1: Comma*/
static BOOLEAN weightDotTypeFlag;	//TODO : ���ξ� sale �ʿ�??
/** @brief Multiple sale ī��Ʈ*/
static INT16U saleMultiCount;
/** @brief Hold��� count*/
static INT8U holdCount;	//Haning type INT16U / TODO : ���ξ� sale �ʿ�??
/** @brief */
static INT16U holdDisplayFlash;	//TODO : ���ξ� sale �ʿ�??
/** @brief Percent tare ��*/
static INT32S salePercentTare;
/** @brief Percent tare ��ɿ� ����ϴ� Price*/
static INT32U salePercentPrice;
/** @brief Percent tare ��ɿ� ����ϴ� ����(%)*/
static INT32U salePercentRate;
/** @brief ���� Battery ���а�*/
static INT8U saleBatteryCapacity;
/** @brief Battery üũ �Լ� ������ ���� �÷���*/
static BOOLEAN saleBatteryCheckFlag;
/** @brief PLU name ��ġ*/
static INT8S salePluNamePosition;
/** @brief ���� ������� Price unit string data*/
static INT8U idata saleUsePriceUnit[PRICE_SYMBOL_SIZE];
/** @brief Euro rate(����)*/
static INT32S saleEuroRate;
/** @brief Euro decimal position*/
static INT8U saleEuroDecPos;
/** @brief Euro Display decimal position*/
static INT8U saleEuroDispDecPos;
/** @brief Add currency */
static BOOLEAN saleAddCurrency;
/** @brief �Ǹ� ���� ���� �� �׼�*/
static INT32S saleReceiveMoney;
/** @brief �Ǹ� ���� �Ž��� ��*/
static INT32S saleChangeMoney;
/** @brief 1,000�� ������ comma��� �÷���*/
static BOOLEAN saleUseThousandPtFlag;
/** @brief ���� ����*/
const INT8U code saleWeightUnit[5][5] = { "kg", "lb", "oz", "g ", "100g"};
/** @brief ���� ����*/
const INT8U code salePriceUnit[2][3] = { "$", "\x80"}; //0-non euro. 1-Euro //see saleUseEuroFlag
/** @brief �ݿø� ����*/
const INT8U code saleRoundOffUnit[4] = {0, 5, 10, 25};
//const INT8U code saleRoundOffUnit[4] = {5, 25, 50, 125}

// for CASTON -->
/** @brief Auto Hold & Manual Hold Check flag 0: Auto Hold, 1: Manual Hold*/
//static BOOLEAN AMHoldSelectFlag;	//TODO : ���ξ� sale �ʿ�??
/** @brief Auto Hold & Manual Hold Check flag 1:Hold ok*/
//static BOOLEAN AMHoldOkFlag;	//TODO : ���ξ� sale �ʿ�??
/** @brief Auto Hold restart flag 1:restart*/
//static BOOLEAN ReHoldFlag;	//TODO : ���ξ� sale �ʿ�??
/** @brief 'HDOFF' Message display flag*/
//static BOOLEAN HoldClearMessageFlag;	//TODO : ���ξ� sale �ʿ�??
/** @brief ���� ����ϴ� Hold type, 0:NORMAL, 1:PEAK, 2 : AVERAGE*/
//static INT8U saleHoldTypeFlag;
/** @brief Hold��� count*/
//INT16U holdDispCount;	//TODO : ���ξ� sale �ʿ�??	//not use
/** @brief SUM��� count*/
INT16U sumCount;	//TODO : ���ξ� sale �ʿ�??
/** @brief Sum��� ��� ����, ��հ�*/
INT32S sumWeight;	//TODO : ���ξ� sale �ʿ�??
/** @brief Sum��� ��� ����, ��հ�*/
INT32S sumSaveWeight;	//TODO : ���ξ� sale �ʿ�??
/** @brief */
//INT16U sumDisplayFlash;	//TODO : ���ξ� sale �ʿ�??
//<-- for CASTON

/** @brief �����ð� ���� state delay�ϴ� ���� state*/
static INT8U saleNextState;
/** @brief �����ð� ���Ŀ� ���� data����� �ٽ� ���� �ϴ� flag*/
static BOOLEAN saleUnitPriceRenewalFlag = ON;
/** @brief sum�� prt�ϱ� ���� flag*/
static BOOLEAN saleEnableSumPrt;
/** @brief ���� add�ÿ��� start���� prt�ϱ� ���� flag*/
static BOOLEAN saleFirstAdd;
/** @brief halfŰ�� quarterŰ ��밡�� flag*/
static BOOLEAN saleHalfQuartEnable;
/** @brief 100gŰ ��� ���� flag*/
static BOOLEAN saleHundredGramEnable;
/** @brief 100g��� ���� Flag*/
static BOOLEAN saleHundredGramPrice;
/** @brief 100gŰ ��� ���� flag*/
static idata INT8U saleDefaultPluName[PLU_NAME_SIZE];
/** @brief Tare key is pressed flag*/
static BOOLEAN saleTareKeyPressed;
/** @brief Zero key is pressed flag*/
static BOOLEAN saleZeroKeyPressed;
/** @brief�����ð� ���Ŀ� �ڵ� �Ǹ�  flag*/
static BOOLEAN saleAutoTransactionStartFlag;
/** @brief�����ð� ���Ŀ� �ڵ� �Ǹ� ���� üũ flag*/
static BOOLEAN saleAutoTransactionWeightCheck; 


/** @brief Last digit invalid function enable  flag*/
static BOOLEAN saleLastDigitInvalidEnable;

/** @brief Transaction Unitprice check flag*/
static BOOLEAN saleTransactionWeightCheck;
/** @brief Barcode Weight Prefix value*/
static INT8U saleWeightPrefix;
/** @brief Barcode Count Prefix value*/
static INT8U saleCountPrefix;

/** @brief Barcode Count Prefix value*/
static INT8U saleClerkNum = 0;

static INT8U saleTareType;

static INT8U saleDateType;

static INT32U saleLastAddedPrice;
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
* @brief Key�� ���� Sale Function struct
* @remark minNum <= Key <= maxNum �̸� INT8U func(KEY_TYPE key) ����
*/
typedef struct 
{ 
	/** @brief sale �Լ� �ּ� ��ȣ*/
	KEY_TYPE minNum;	
	/** @brief sale �Լ� �ִ� ��ȣ*/
	KEY_TYPE maxNum;
	/** @brief sale �Լ�*/
	INT8U (*func) (KEY_TYPE key); 
} SALE_KEY_FUNC;

/** 
* @brief Key�� ���� Sale Function struct (void �Լ�)
* @remark keyNum == Key �̸� void func(void) ����
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

static void saleEuroKey(void);
static void salePluCallKey(void);
static void salePluSaveKey(void);
static void saleBattCheckKey(void);
static void salePayKey(void);
static void saleSumKey(void);
static void saleDoubleZeroKey(void);
static void saleAddKey(void);
static void saleFAddKey(void);     //fixed Add key
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
static void salePrintKey(void);

static void 	saleDispNumWithDecPoint(INT8U winNum, INT8U position, INT8U type, INT32S num, BOOLEAN thousandPt);
static INT8U 	saleFuncPluDataReadInSale(INT16U num);
static void 	saleFuncPluDataWriteInSale(INT16U inputPluNum);
static void 	saleFuncDailyTotalSave(BOOLEAN sign, INT16U count, INT32U price);
static INT32U 	saleFuncEuro2Dollar(INT32U inputPrice);
static INT32U 	saleFuncDollar2Euro(INT32U inPutPrice);
static void   	saleFuncReadPercentTareData(void);
static void  	saleFuncClearUnitPrice(void);
static void 	saleFuncClearPluName(void);
static void 	saleFuncPrintCommand(INT8U	saleType);
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

#ifdef USE_CLERK_KEY
static void saleOpenClerkKey(void);
static void saleSendClerkKey(void);
static void saleClerkKey(INT8U clerkNum);
static void saleClerkKey1st(void);
static void saleClerkKey2nd(void);
static void saleClerkKey3rd(void);
static void saleClerkKey4th(void);
static void saleClerkKey5th(void);
#endif

#ifdef USE_BARCODE_PRINT
static void salePrtBarcode(void);
#endif

/** @brief Key�� ���� Sale Function(key input type) ���� */
#define SALE_KEY_MAX_FUNC_NUM	3
#define SALE_COUNT_ENABLE_CAPA	1200
#define SALE_COUNT_LOW_CAPA		1
#define SALE_PERCENT_LOW_CAPA		1800

/** 
* @brief Key�� ���� Sale Function struct
* @remark minNum <= Key <= maxNum �̸� INT8U func(KEY_TYPE key) ����\n
*         key �Է� �޴� �Լ�
*         �Լ� �߰��� SALE_KEY_MAX_FUNC_NUM���� ��Ű�ÿ�.
*/
const SALE_KEY_FUNC code saleKeyFuncTable[SALE_KEY_MAX_FUNC_NUM] = 
{
	{KEY_NUM_0, 	KEY_NUM_9, 	saleNumKey},
	{KEY_NUM_A, KEY_NUM_F, 	saleNumKey},
	{KEY_PLU_1, 	KEY_PLU_41, 	saleDirectPluCall},
};

#ifdef USE_CLERK_KEY
/** @brief Clerk Key�� ���� Sale Function(void type) ���� */
#define CLERK_KEY_FUNC_NUM	7
#else
#define CLERK_KEY_FUNC_NUM	0
#endif

#ifdef USE_BARCODE_PRINT
/** @brief Key�� ���� Sale Function(void type) ���� */
#define BARCODE_PRINT_KEY_FUNC_NUM	1
#else
#define BARCODE_PRINT_KEY_FUNC_NUM	0
#endif

/** @brief Key�� ���� Sale Function(void type) ���� */
#define SALE_KEY_MAX_VOID_FUNC_NUM (29+CLERK_KEY_FUNC_NUM+BARCODE_PRINT_KEY_FUNC_NUM)

/**
* @brief Key�� ���� Sale Function struct (void �Լ�)
* @remark keyNum == Key �̸� void func(void) ����\n
*         key �Է� ���� �ʴ� �Լ�
*         �Լ� �߰��� SALE_KEY_MAX_VOID_FUNC_NUM���� ��Ű�ÿ�.
*/
const SALE_KEY_VOID_FUNC code saleKeyVoidFuncTable[SALE_KEY_MAX_VOID_FUNC_NUM] = 
{
	{KEY_ZERO, 				saleZeroKey},
	{KEY_TARE, 				saleTareKey},
	{KEY_HOLD, 				saleHoldKey},
	{KEY_C, 				saleClearKey},
	{KEY_EURO, 				saleEuroKey},	
	{KEY_PLU_CALL,  		salePluCallKey},
	{KEY_BAT_CHECK, 		saleBattCheckKey},
	{KEY_PLU_SAVE,			salePluSaveKey},
	{KEY_PAY,				salePayKey},
	{KEY_SUM,				saleSumKey},
	{KEY_NUM_00,			saleDoubleZeroKey},
	{KEY_ADD,				saleAddKey},
	{KEY_FIXED_ADD,			saleFAddKey},	
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
		
#ifdef USE_CLERK_KEY
	{KEY_CLERK_1ST, 		saleClerkKey1st},
	{KEY_CLERK_2ND,			saleClerkKey2nd},
	{KEY_CLERK_3RD,			saleClerkKey3rd},
	{KEY_CLERK_4TH, 		saleClerkKey4th},
	{KEY_CLERK_5TH, 		saleClerkKey5th},
	{KEY_OPEN_CLERK, 		saleOpenClerkKey},
	{KEY_SEND_CLERK,		saleSendClerkKey},
#endif
#ifdef USE_BARCODE_PRINT
	{KEY_SEND_BARCODE,		salePrtBarcode}
#endif
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
* @brief    Sale Mode Application Parameter�� default ������ �ʱ�ȭ\n
* @param    type : defualt type
* @return   none
* @remark   Parameter ������ �ʱⰪ�� �ִ´�.\n
*           Cal Mode�� Program ��忡�� ��ü �ʱ�ȭ�� �� �����Ѵ�.
********************************************************************************
*/
void SaleModeSetDefaultParam(INT8U type)
{
	SaleModeClearAllData();		// Sale Data default ���� 

	switch (type)
	{
		case NTEP:
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST, 0xC8);
#ifdef USE_SUSPEND_MODE
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0x53);
#else
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0x73);
#endif
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_3RD, 0x23);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, 0x24);
			ParamWriteData(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PRICE_SYMBOL, "$", PRICE_SYMBOL_SIZE); 	// first unit				
			break;

		case KOREA:
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST, 0x00);
#ifdef USE_SUSPEND_MODE
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0xD1);
#else
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0xF1);
#endif
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_3RD, 0x26);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, 0x70);				
			ParamWriteData(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PRICE_SYMBOL, "\\", PRICE_SYMBOL_SIZE); 	// first unit
			break;

		default: 		// OIML
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST, 0xB8);
#ifdef USE_SUSPEND_MODE
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0x11);
#else
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0x31);
#endif
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_3RD, 0x0B);				
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH, 0x21);
			ParamWriteData(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_PRICE_SYMBOL, "$", PRICE_SYMBOL_SIZE); 	// first unit

			AdWriteParam(TARE_SUCCESSIVE, TARE_TYPE_SUCCESSIVE_PLUS);
			AdWriteParam(ZERO_MARK_INDICATOR, ZERO_MARK_TYPE_ALL);			
			break;
	}
	ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS, SALE_STATE_NORMAL);
	ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_WEIGHT_PREFIX, 11);
	ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_PREFIX, 22);
}

/**
********************************************************************************
* @brief    sale�� ���õ� ��� ������ �ʱ�ȭ �ϴ� �Լ�       
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
* @brief    Sale Mode Application Init �Լ�\n
*           Sale Mode State Init
* @param    none
* @return   none
* @remark   �ʱ⿡ �ѹ� ���� �Ǵ� �Լ�\n
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
//	salePluTypeFlag = (readSetDataInSale & 0x80) >> 7;	--> reserve
	saleUsePluValidDateFlag = (readSetDataInSale & 0x80) >> 7;
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
//	saleUseHeadMsgFlag = (readSetDataInSale & 0x80) >> 7;			// reserve		
//	temp = (readSetDataInSale & 0x40) >> 6;							// reserve
	saleRoundOffUnitFlag = (readSetDataInSale & 0x30) >> 4;
	saleLastDigitInvalidEnable = (readSetDataInSale & 0x08) >> 3;		// reserve		
	saleHundredGramPrice = (readSetDataInSale & 0x04) >> 2;		//100g�ܰ� ��� Flag����
	salePriceForUnit = (readSetDataInSale & 0x03);

// 	C4-5
   	readSetDataInSale = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH);
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
	scaleUsePrt = (readSetDataInSale & 0x03);		// reserve	

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
	
	if (scaleUsePrt == PRT_NONE || scaleUsePrt == PRT_RESERVED)			// kiho
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
	if(scaleUseEuroFlag)
	{
		scaleEuroOffFlag=0;
	}
	else
	{
		scaleEuroOffFlag=1;
	}
			
	ReportReadData(REPORT_ALL_NUM, &dailyTotalReport);

	if (dailyTotalReport.count > DAILY_TOTAL_LIMIT)
	{
		dailyTotalReport.count = 0;
		dailyTotalReport.price = 0;
	}			

	saleEuroDecPos = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_EURO_DEC);
	saleEuroDispDecPos = 2;
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

	SaleData.weight = 0;
	SaleData.unitPrice = 0;
	SaleData.pluNum = 0;
	SaleData.totalPrice = 0;

	SaleData.holdWeight = 0;
	saleMultiCount = 0;
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
			DispSetConstStrMsg(WIN_WEIGHT, "POUND");
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
	saleWeightPrefix = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_WEIGHT_PREFIX);
	saleCountPrefix= ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COUNT_PREFIX);
	saleDateType = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_DATE_TYPE);	//date type

	saleAutoTransactionStartFlag = OFF;
	saleAutoTransactionWeightCheck = OFF;
	saleLastAddedPrice = 0;


}

/**
********************************************************************************
* @brief    ���� �ð����� state delay �� (key �Է� �Ұ���)
* @param    onTime100ms : Delay �ð� (���� : 100ms)\n
*           state : ���� �ð��ڿ� ������ state
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

	KeyEnable(OFF);//key ���� ����

	TimerSetDelay100ms(TIMER_DELAY_100MS_SALE_STATE, onTime100ms);
	saleNextState = state;
}

/**
********************************************************************************
* @brief    unit price �Է� �� ���� �ð��� ������ ���� data�� ����� �ٽ� ���� 
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
* @brief    Sale Mode�� ���鼭 timer�� üũ�ϴ� �Լ�
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
	 	KeyEnable(ON);			//key ���� ���
	 	SaleData.saleState = saleNextState;
	}
	if (TimerCheckDelay100ms(TIMER_DELAY_100MS_UNIT_PRICE_RENEWAL) == 1)
	{
 		saleUnitPriceRenewalFlag = ON;
	}
}

/**
********************************************************************************
* @brief    Sale Mode Application Procedure �Լ�\n
* @param    none
* @return   SCALE_MODE_SALE, SCALE_MODE_PGM, SCALE_MODE_CAL
* @remark   key���� 2���� func table���� ã�Ƽ� �´� func�� ����\n
*           AD ��ȭ �� ����\n
*           State, Status ��ȭ�� ���� Display �Լ� ����\n
*           ���� �ð��� �ѹ� ��� ����Ǵ� �Լ�\n
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

	if (SaleData.unitPrice == 0)
	{
		saleHalfQuartEnable = 1;
	}
	if (KeyCheck())
	{
		if (SaleData.saleState == SALE_STATE_PLU_NAME)
		{
			Key = KeyGetFuncCode(KEY_MODE_HEX);
		}
		else
		{
			Key = KeyGetFuncCode(KEY_MODE_NORMAL);			
			if(SaleData.saleState == SALE_STATE_HOLD) Key = KEY_NOT_PRESSED;
		}

		saleAutoTransactionStartFlag = OFF;
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

	if (SaleData.totalPrice > 0 && AdData.stableFlag && saleAutoTransactionWeightCheck)
	{
		if (saleAutoTransactionStartFlag == OFF)
		{
			saleAutoTransactionStartFlag = ON;
			TimerSetDelay100ms(TIMER_DELAY_100MS_AUTO_TRANSACTION, 30);
		}
	}
	else
	{
		saleAutoTransactionStartFlag = OFF;
		TimerSetDelay100ms(TIMER_DELAY_100MS_AUTO_TRANSACTION, 0);//stop timer
	}
	if (TimerCheckDelay100ms(TIMER_DELAY_100MS_AUTO_TRANSACTION))
	{
		saleFuncDailyTotalSave(0, 1, SaleData.totalPrice);	   
		saleLastAddedPrice = SaleData.totalPrice;
		BuzzerSetOnTimes(1);
		TimerSetDelay100ms(TIMER_DELAY_100MS_AUTO_TRANSACTION, 0);
		saleAutoTransactionWeightCheck = OFF;
	}


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
* @brief    Sale Mode�� ���鼭 timer�� üũ�ϴ� �Լ�
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
* @brief    Sale Mode�� ���鼭 timer�� üũ�ϴ� �Լ�
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
* @brief    Sale Mode�� ���鼭 timer�� üũ�ϴ� �Լ�
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

/**
********************************************************************************
* @brief    ER PLUS ECR���� PLU �̸��� �޾ƿ��� �Լ�
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
/*
void SalePluNameCopyExt(INT8U *pluName)
{
	INT8U i;

	for(i = 0; i < 13; i++)
	{
		tempPluData.name[i] = pluName[i];
	}
}
*/
/**
********************************************************************************
* @brief    Sale Mode Indicator �Լ� 
* @param    none
* @return   none
* @remark   ���� Flag�� ���� �����¸� indicatation �Ѵ�.  \n
*           ������ indicator ������ UiDefine.h�� �����Ѵ�. \n
********************************************************************************
*/
void saleSetIndicator(void)
{
	DispSetIndicator(INDICATOR_SUM, OFF);
	
	if (salePrePackFlag)
	{
		DispSetIndicator(INDICATOR_PREPACK, ON);
	}
	else
	{
		DispSetIndicator(INDICATOR_PREPACK, OFF);
	}

	if (scaleUseEuroFlag && saleUseEuroFlag)
	{
		DispSetIndicator(INDICATOR_EURO, ON);
		DispSetIndicator(INDICATOR_EURO_KG, ON);
		
		DispSetIndicator(INDICATOR_USD, OFF);
		DispSetIndicator(INDICATOR_USD_KG, OFF);
	}
	else if(scaleEuroOffFlag)
	{
		DispSetIndicator(INDICATOR_EURO, OFF);
		DispSetIndicator(INDICATOR_EURO_KG, OFF);

		DispSetIndicator(INDICATOR_USD, OFF);
		DispSetIndicator(INDICATOR_USD_KG, OFF);
	}
	else
	{
		DispSetIndicator(INDICATOR_EURO, OFF);
		DispSetIndicator(INDICATOR_EURO_KG, OFF);

		DispSetIndicator(INDICATOR_USD, ON);
		DispSetIndicator(INDICATOR_USD_KG, ON);
	}

	DispSetIndicator(INDICATOR_KG, OFF);
	DispSetIndicator(INDICATOR_UNITPRICE_KG, OFF);
	DispSetIndicator(INDICATOR_LB, OFF);
	DispSetIndicator(INDICATOR_UNITPRICE_LB, OFF);
	DispSetIndicator(INDICATOR_OZ, OFF);
	DispSetIndicator(INDICATOR_GRAM, OFF);

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
	else 
	{
		DispSetIndicator(INDICATOR_TARE_SAVE, OFF);
	}
	if (saleBothSaveFlag)
	{
		DispSetIndicator(INDICATOR_BOTH_SAVE, ON);
	}
	else
	{
		DispSetIndicator(INDICATOR_BOTH_SAVE, OFF);
	}
}


/**
********************************************************************************
* @brief    ���� Sale mode ���� ����� weight, unitprice�� ���� total price �� �����Ѵ�.  
* @param    none
* @return   none
* @remark   AdData�� ���԰��� �Ҿ� ���� total price ���. 	\n
*           Transaction�� Start�� End�� �����Ѵ�.         	\n
*			Hold ���Ը� �����Ѵ�. 							\n
********************************************************************************
*/
void saleWeightFunc(void)
{
	INT8U returnData;
	INT32U	tempData;
	FP64	tempDoubledata;		

	if (salePercentTareCallFlag == OFF && saleTareClearFlag && transactionEndFlag  && AdData.grossZeroFlag && saleBothSaveFlag == OFF)
	{
		returnData = AdSetTare();
		if (returnData == ERR_NONE || returnData == ERR_NO_TARE)
		{
			transactionEndFlag = FALSE;		
		}
	}

	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:
			if(SaleData.weight <= 0 && AdData.stableFlag) saleAutoTransactionWeightCheck = ON;
			if(SaleData.weight > 0 && AdData.stableFlag)
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
			
			if (SaleData.weightUnit == 2)	  // Oz �϶��� ���ݰ�� ���Ѵ� . 
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
				saleFuncClearUnitPrice();						// Add Ű�� ���� �ǸŰ� �̷���� ���
				SaleData.saleState = SALE_STATE_NORMAL;
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
			break;

		default:
			break;
	}
}

/**
********************************************************************************
* @brief    ���� Sale mode���� Weight data�� Display �ϴ� �Լ�   
* @param    weight data
* @return   none
* @remark   weight data�� Display �Ѵ�.
********************************************************************************
*/
void saleDisplayWeight(INT32S dispWeight)
{
	INT8U	weightWinSize;
	weightWinSize = DispGetWinSize(WIN_WEIGHT);

	saleDispNumWithDecPoint(WIN_WEIGHT, SaleData.weightDecPos, weightDotTypeFlag, dispWeight, 0);
	if (SaleData.lastDigitIsInvalid)
	{
		if (SaleData.weightDecPos == 1) 
		{ 	////2kg�� ��� 0.5g���� 1g���� �ö󰡸� decimal point �����ؾ� �ϹǷ� �ٽ� Display�Լ� �ҷ���
			saleDispNumWithDecPoint(WIN_WEIGHT,  0, weightDotTypeFlag, dispWeight, 0);
		}
		DispSetChar(WIN_WEIGHT, weightWinSize - 1, ' ');
	}
}
/**
********************************************************************************
* @brief    ���� Sale mode ������ Display �ϴ� �Լ�   
* @param    none
* @return   none
* @remark   �� ���º� �ش簪�� Display �Ѵ�.
********************************************************************************
*/
void saleDisplayFunc(void)
{
	INT8U	textBuf[8];
	INT8U	weightWinSize;
	INT16U	priceHi;
	INT32U	tempPrice;

	weightWinSize = DispGetWinSize(WIN_WEIGHT);
	
	if (SaleData.saleState != SALE_STATE_NORMAL)
	{
		PowerSaverResetTimer(TIMER_DELAY_100MS_STAND_BY);
	}

	switch (SaleData.saleState)		/*	display	*/
	{
		case SALE_STATE_NORMAL:	
			saleReceiveMoney = 0L;   
/////			
			if (SaleData.unitPrice == 0)
			{
				saleTransactionWeightCheck = FALSE;
				saleLastAddedPrice = 0;
			}
			if(SaleData.weight <= 0 && saleTransactionWeightCheck && SaleData.unitPrice)
			{
				saleFuncClearUnitPrice();
				transactionEndFlag = TRUE;
				saleTransactionWeightCheck = FALSE;
				saleLastAddedPrice = 0;
			}
/////
			saleDisplayWeight(SaleData.weight);

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
			MakeStrWithNum(textBuf, "M-", saleMultiCount, weightWinSize);		
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

		case SALE_STATE_PLU_NUM:
#ifdef USE_WEIGHT_5DIGIT
			DispSetStr(WIN_WEIGHT, "PLUNU");
#else
			DispSetStr(WIN_WEIGHT, "PLU NU");
#endif
			saleDispNumWithDecPoint(WIN_UNIT_PRICE, 0, priceDotTypeFlag, SaleData.pluNum, 0);
			DispSetNum(WIN_TOTAL_PRICE, tempPluData.unitPrice, 0);
			break;

		case SALE_STATE_PLU_TARE:			
#ifdef USE_WEIGHT_5DIGIT
			DispSetStr(WIN_WEIGHT, "PLUTA");
#else
			DispSetStr(WIN_WEIGHT, "PLU TA");
#endif
			saleDispNumWithDecPoint(WIN_UNIT_PRICE, SaleData.weightDecPos, weightDotTypeFlag, tempPluData.tare, 0);
			DispSetNum(WIN_TOTAL_PRICE, tempPluData.unitPrice, 0);
			break;
			
		case SALE_STATE_PLU_VALID_DATE:
#ifdef USE_WEIGHT_5DIGIT
			DispSetStr(WIN_WEIGHT, "PLUVD");
#else
			DispSetStr(WIN_WEIGHT, "PLU VD");
#endif
			saleDispNumWithDecPoint(WIN_UNIT_PRICE, 0, 0, tempPluData.validDate, 0);
			DispSetNum(WIN_TOTAL_PRICE, tempPluData.unitPrice, 0);
			break;
			
		case SALE_STATE_PLU_NAME:
			DispSetCharWithHexNum(WIN_UNIT_PRICE, (INT8U)tempPluData.name[salePluNamePosition]);
			saleDispNumWithDecPoint(WIN_TOTAL_PRICE, 0, 0, salePluNamePosition + 1, 0);	  
			MakeStrWithNum(textBuf, "P-", SaleData.pluNum, weightWinSize);
			DispSetStr(WIN_WEIGHT, textBuf);
			break;

		case SALE_STATE_PLU_ITEMCODE:
#ifdef USE_WEIGHT_5DIGIT
			DispSetStr(WIN_WEIGHT, "PLUIT");
#else
			DispSetStr(WIN_WEIGHT, "PLU IT");
#endif
			saleDispNumWithDecPoint(WIN_UNIT_PRICE, 0, priceDotTypeFlag, tempPluData.itemCode, 0);
			DispSetNum(WIN_TOTAL_PRICE, tempPluData.unitPrice, 0);
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
			DispSetStr(WIN_UNIT_PRICE, " RATE ");
			saleDispNumWithDecPoint(WIN_TOTAL_PRICE, saleEuroDecPos, 0, saleEuroRate, 0);	
			break;

		case SALE_STATE_EURO_DISPLAY:			
			DispSetStr(WIN_WEIGHT, " EURO ");
			DispSetStr(WIN_UNIT_PRICE, "      ");
			saleDispNumWithDecPoint(WIN_TOTAL_PRICE, saleEuroDecPos, 0, saleEuroRate, 0);
			break;

		case SALE_STATE_NOEURO_DISPLAY:
			DispSetStr(WIN_WEIGHT, "DOLLAR");
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
				DispSetDecPoint(WIN_TOTAL_PRICE, 0, 0, 0);  //display position				
				DispSetNum(WIN_TOTAL_PRICE, priceHi, 0);
			}
			else
			{
				DispSetStr(WIN_TOTAL_PRICE, " DAILY ");
			}

			DispSetDecPoint(WIN_UNIT_PRICE, 0, 0, 0);				
			DispSetNum(WIN_UNIT_PRICE, dailyTotalReport.count, 0);
			
			if (scaleUseEuroFlag && saleUseEuroFlag) DispSetDecPoint(WIN_WEIGHT, saleEuroDispDecPos, priceDotTypeFlag, 0);  //display position
			else DispSetDecPoint(WIN_WEIGHT, priceDecPos, priceDotTypeFlag, 0);  //display position
			if (priceHi > 0) DispSetNum(WIN_WEIGHT, tempPrice, '0');
			else DispSetNum(WIN_WEIGHT, tempPrice, ' ');
				
			break;

		case SALE_STATE_MODE_PLU_NUM:			
			MakeStrWithNum(textBuf, "P-", SaleData.pluNum, weightWinSize);	
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
				DispSetDecPoint(WIN_TOTAL_PRICE, 0, 0, 0);  //display position				
				DispSetNum(WIN_TOTAL_PRICE, priceHi, 0);
			}
			else
			{
				MakeStrWithNum(textBuf, " P-", SaleData.pluNum, weightWinSize);
				DispSetStr(WIN_TOTAL_PRICE, textBuf);
			}

			DispSetDecPoint(WIN_UNIT_PRICE, 0, 0, 0);				
			DispSetNum(WIN_UNIT_PRICE, tempPluReport.count, 0);
			
			DispSetDecPoint(WIN_WEIGHT, priceDecPos, priceDotTypeFlag, 0);  //display position
			DispSetNum(WIN_WEIGHT, tempPrice, 0);
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
				if (holdDisplayFlash == 200)
				{
					SaleData.saleState = SALE_STATE_NORMAL;
					DispSetIndicator(INDICATOR_HOLD, OFF);
		
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

		case SALE_STATE_OPENCLERK_CALL:			
			DispSetStr(WIN_WEIGHT, "CLERK");
			DispSetStr(WIN_UNIT_PRICE, "NUMBER");
			saleDispNumWithDecPoint(WIN_TOTAL_PRICE, 0, 0, saleClerkNum, 0);			
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
* @brief    Zero Key �Լ����� Stable�� ��ٸ��� Tare����� �����ϴ� �Լ� 
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
* @brief    Zero Key ������ �����Ѵ�.   
* @param    none
* @return   none
* @remark   Zero�� ���� ���ϸ� �����
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
			
		case SALE_STATE_PLU_NAME:
			if (salePluNamePosition > 0)
			{
				salePluNamePosition--; 
				
				if (salePluNamePosition <0)
				{
					BuzzerSetOnTimes(2);
					salePluNamePosition = 0;
				}
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

/**
********************************************************************************
* @brief    Tare Key �Լ����� Stable�� ��ٸ��� Tare����� �����ϴ� �Լ� 
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void saleWeightTareProc(void)
{
	INT8U returnData;

	// Unit Price, total price ���� �ϰ� Tare���� ���� �ϰ� ���� 
	if (salePercentTare == 0 )
	{
		if (SaleData.totalPrice)
		{
			//Total price�˻� �� Unit price �� Total Price ���� 
			SaleData.unitPrice = 0;
			SaleData.totalPrice = 0;
		}
		if (SaleData.pluNum)
		{
			//PLU ȣ�� ���� �̸� PLU Name ����
			SaleData.pluNum = 0;
			saleFuncClearPluName();
		}
		returnData = AdSetTare();

		if (returnData == ERR_NONE)
		{
			saleTransactionWeightCheck = FALSE;
			saleLastAddedPrice = 0;
			saleTareType = WEIGHT_TARE_CALL;
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

/**
********************************************************************************
* @brief    Tare Key ������ �����Ѵ�.    
* @param    none
* @return   none
* @remark   �� ���º� zero key ���  ����
********************************************************************************
*/
void saleTareKey(void)
{
	INT8U returnData;

	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:
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

		default:
			BuzzerSetOnTimes(2);
			break;
	}
}

/* Insert global functions here */
/**
********************************************************************************
* @brief    Hold key ������ �����Ѵ�. 
* @param    type : defualt type
* @return   none
* @remark   ���԰� "0"�϶��� ������ �ð��Ŀ� "Hold"�� ������ \n
********************************************************************************
*/
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
					saleTransactionWeightCheck = TRUE;
					SaleData.saleState = SALE_STATE_HOLD;
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

/**
********************************************************************************
* @brief    Clear Key ������ �����Ѵ�.    
* @param    none
* @return   none
* @remark   �� ���º� Clear key ���  ����
********************************************************************************
*/
void saleClearKey(void)
{
		
	switch (SaleData.saleState)	 	/*clear*/
	{
		case SALE_STATE_NORMAL:			
			SaleData.unitPrice = 0L;			
			saleMultiCount = 0; 		//add kkanno 080326
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
				SaleData.pluNum = 0;
				if (tempPluData.tare) 
				{
					tempPluData.tare = 0;
					AdSetPT(tempPluData.tare, 1);				
				}
				tempPluData.itemCode = 0;
				tempPluData.validDate = 0;				
				tempPluData.unitPrice = 0;
				SaleData.unitPrice = 0;
				saleFuncClearPluName();
				SaleData.saleState = SALE_STATE_NORMAL;
			}
			tempPluData.itemCode = 0;
			tempPluData.tare = 0;	
			AdSetPT(tempPluData.tare, 1);
			break;
			
		case SALE_STATE_PLU_VALID_DATE:
			if (tempPluData.validDate == 0)
			{
				BuzzerSetOnTimes(2);
				tempPluData.itemCode = 0;
				tempPluData.validDate = 0;				
				tempPluData.tare = 0;
				tempPluData.unitPrice = 0;
				SaleData.unitPrice = 0;
				SaleData.saleState = SALE_STATE_NORMAL;
			}
			tempPluData.itemCode = 0;
			tempPluData.tare = 0;
			tempPluData.validDate = 0;
			saleFuncClearPluName();
			break;
		
		case SALE_STATE_PLU_NAME:
			salePluNamePosition++;
			if (salePluNamePosition == PLU_NAME_SIZE)
			{
				saleFuncPluDataWriteInSale(SaleData.pluNum);
			}
			break;

		case SALE_STATE_PLU_ITEMCODE:
			if (tempPluData.itemCode == 0)
			{
				BuzzerSetOnTimes(2);
				tempPluData.itemCode = 0;
				tempPluData.validDate = 0;				
				tempPluData.tare = 0;
				tempPluData.unitPrice = 0;
				SaleData.unitPrice = 0;
				SaleData.saleState = SALE_STATE_NORMAL;
			}
			tempPluData.itemCode = 0;
			tempPluData.tare = 0;
			tempPluData.validDate = 0;
			saleFuncClearPluName();
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
			SaleData.saleState = SALE_STATE_NORMAL;
			break;

		case SALE_STATE_PAY:
		case SALE_STATE_CANCEL:						
			saleReceiveMoney = 0L;			
			break;

		case SALE_STATE_PAY_CHANGE:		
			SaleData.unitPrice = 0L;
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
			PrtPutCRLF(1);
			PrtPutStr("Transaction was canceled");
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

		case SALE_STATE_OPENCLERK_CALL:
			saleClerkNum = 0;
			SaleData.saleState = SALE_STATE_NORMAL;
			break;
		
		default:			
			BuzzerSetOnTimes(2);
			break;
	}
}

/**
********************************************************************************
* @brief    ���� ���¿��� ���� ���� �Է� �Լ�.     
* @param    none
* @return   none
* @remark   �� ���º� ������ �Է�
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
			SaleData.pluNum = EditorInsDecDigit(SaleData.pluNum, key - KEY_NUM_0, 3, DIRECT_PLU_MAX, FALSE);		
			break;

		case SALE_STATE_PLU_TARE:
			tempPluData.tare = EditorInsDecDigit(tempPluData.tare, key - KEY_NUM_0, PRICE_SIZE, UNITPRICE_MAX, FALSE);
			break;
			
		case SALE_STATE_PLU_VALID_DATE:
			tempPluData.validDate = EditorInsDecDigit(tempPluData.validDate, key - KEY_NUM_0, 3, 999, FALSE);
			break;
			
		case SALE_STATE_PLU_ITEMCODE:
			tempPluData.itemCode = EditorInsDecDigit(tempPluData.itemCode, key - KEY_NUM_0, 5, 99999, FALSE);
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
			if (SaleData.weightUnit == 2)	  // g, Oz �϶��� ���ݰ�� ���Ѵ� .
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
			SaleData.pluNum = EditorInsDecDigit(SaleData.pluNum, key - KEY_NUM_0, 3, DIRECT_PLU_MAX, TRUE);
			break;

		case SALE_STATE_OPENCLERK_CALL:
			saleClerkNum = (INT8U)EditorInsDecDigit((INT32U)saleClerkNum, key - KEY_NUM_0, 2, 20, TRUE);
			break;
			
		default:
			break;
	}	

	return SCALE_MODE_SALE;
}


/**
********************************************************************************
* @brief    Direct PLU ������ ȣ���ϴ� �Լ�    
* @param    none
* @return   none
* @remark   Direct PLU Data��  ȣ���Ѵ�.
********************************************************************************
*/
INT8U saleDirectPluCall(KEY_TYPE key)
{
	INT16U tempPluNum;

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
				SaleData.unitPrice = tempPluData.unitPrice;				//delete kkanno 070605
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
					if (saleUsePluValidDateFlag) SaleData.saleState = SALE_STATE_PLU_VALID_DATE;
					else SaleData.saleState = SALE_STATE_PLU_ITEMCODE; 
				}
			}
			else
			{
				saleFuncPluDataWriteInSale(SaleData.pluNum);						
			}				
			break;

		case SALE_STATE_PLU_ITEMCODE:
			SaleData.saleState = SALE_STATE_PLU_NAME; 
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
* @brief    PLU ������ ȣ���ϴ� �Լ�    
* @param    none
* @return   none
* @remark   Direct or Indirect PLU Data��  ȣ���Ѵ�.
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
	
	if (SaleData.unitPrice == 0 || SaleData.unitPrice > DIRECT_PLU_MAX || SaleData.weightUnit == UNIT_OZ)
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
*           		Euro ���� EuroRate Edit
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
			if (saleUsePluValidDateFlag) SaleData.saleState = SALE_STATE_PLU_VALID_DATE;
			else SaleData.saleState = SALE_STATE_PLU_ITEMCODE;
			break;
			
		case SALE_STATE_PLU_VALID_DATE:
			SaleData.saleState = SALE_STATE_PLU_ITEMCODE;
			break;

		case SALE_STATE_PLU_ITEMCODE:
			SaleData.saleState = SALE_STATE_PLU_NAME;
			break;
			
		case SALE_STATE_PLU_NUM:						
			if (saleUsePluNameFlag)			// PLU name �Է� ����
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
							if (saleUsePluValidDateFlag) SaleData.saleState = SALE_STATE_PLU_VALID_DATE;
							else SaleData.saleState = SALE_STATE_PLU_ITEMCODE;
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
* @brief    Euro ��� ������ �����ϴ� �Լ�     
* @param    none
* @return   none
* @remark   Euro ��� ����� ����
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
			if (saleUseEuroFlag == 0)	  // no euro -> euro�� ���� 
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
			else	 					// euro	 -> no euro�� ���� 
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
		case SALE_STATE_PLU_VALID_DATE:
		case SALE_STATE_PLU_ITEMCODE:			
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
* @brief    Battery �ܷ� Ȯ�� �ϴ� �Լ�      
* @param    none
* @return   none
* @remark   Battery �ܷ� Ȯ��
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
* @brief    Back Light ���º��� �ϴ� �Լ�      
* @param    none
* @return   none
* @remark   Back Light ���º���
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
	}
	else
	{
		PowerOff();
	}
}


/**
********************************************************************************
* @brief    Payment ����� �����ϴ� �Լ�       
* @param    none
* @return   none
* @remark   Payment ����� ����
********************************************************************************
*/
void salePayKey(void)
{
	saleAutoTransactionWeightCheck = OFF;
	
	switch (SaleData.saleState)
	{	
	    case SALE_STATE_HOLD:
		case SALE_STATE_SUM_TTP:
			SaleData.totalPrice = SaleData.addPrice;
			saleReceiveMoney = 0L;
			SaleData.saleState = SALE_STATE_PAY;
			break;
			
		case SALE_STATE_PAY:
			if (saleReceiveMoney >= SaleData.totalPrice)
			{
				saleChangeMoney = saleReceiveMoney - SaleData.totalPrice;

				saleEnableSumPrt = 1;
				if(scaleUsePrt == PRT_DEP50)
				{
					salePrtSum();
					salePrtPayment();
				}
				
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
				saleEnableSumPrt = 1;
				salePrtLineFlag = 1;
				salePrtMultiItem();	
				saleFuncClearUnitPrice();		// PrintŰ�� �ǸŰ� �̷���� ��� 
				saleHalfQuartEnable = 1;
				saleMultiCount = 0;			//add kkanno 080326
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
* @brief    SUM ����� �����ϴ� �Լ�       
* @param    none
* @return   none
* @remark   Add data ǥ�ù� Payment ���ȣ�� 	\n
*           Print ������ ���� Print ����
********************************************************************************
*/
void saleSumKey(void)
{
	switch (SaleData.saleState)
	{	
		case SALE_STATE_HOLD:
		case SALE_STATE_NORMAL:
			if (SaleData.addCount == 0) 		
			{
				BuzzerSetOnTimes(2);
				return;
			}
			else
			{
 				SaleData.saleState = SALE_STATE_SUM_TTP;
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
			saleFuncClearUnitPrice();
			SaleData.saleState = SALE_STATE_NORMAL;
			break;

		case SALE_STATE_FIXED_PRICE:
			SaleData.saleState = SALE_STATE_SUM_TTP;
			break;
		default:			
			BuzzerSetOnTimes(2);
			break;
	}
}
/**
********************************************************************************
* @brief    SUM ����� �����ϴ� �Լ�       
* @param    none
* @return   none
* @remark   Add data ǥ�ù� Payment ���ȣ�� 	\n
*           Print ������ ���� Print ����
********************************************************************************
*/
void salePrintKey(void)
{
	switch (SaleData.saleState)
	{	
		case SALE_STATE_HOLD:
		case SALE_STATE_NORMAL:
			if (AdData.stableFlag)
			{
				if (SaleData.totalPrice > TOTALPRICE_MAX)
				{
					DispSetStr(WIN_TOTAL_PRICE, " OVER ");
					BuzzerSetOnTimes(1); 
				}					
				else if (!scaleUsePrt && AdData.rawSetTareData < (INT32S)(AdData.d1 * 20))
				{
					BuzzerSetOnTimes(1); 
				}
				else if (SaleData.totalPrice == 0 && SaleData.unitPrice > 0 && (!SaleData.addCount) && (!SaleData.weight))
				{
					if (saleUseDailyTotalFlag)
					{
						saleFuncDailyTotalSave(0, 1, SaleData.unitPrice);
					}
					saleEnableSumPrt = 1;
					salePrtLineFlag = 1;
					salePrtFixedItem();
					transactionEndFlag = TRUE;
					saleFuncClearUnitPrice();				// PrintŰ�� �ǸŰ� �̷���� ��� 	
					saleHalfQuartEnable = 1;
				}						
				else if (SaleData.totalPrice > 0 && (!SaleData.addCount))
				{				
					if (saleUseDailyTotalFlag)
					{
						saleFuncDailyTotalSave(0, 1, SaleData.totalPrice);
					}
					saleEnableSumPrt = 1;
					salePrtLineFlag = 1;
					salePrtWeightItem();
					transactionEndFlag = TRUE;
					saleFuncClearUnitPrice();		// PrintŰ�� �ǸŰ� �̷���� ��� 
					saleHalfQuartEnable = 1;
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
		
		case SALE_STATE_PAY:
			if (saleReceiveMoney >= SaleData.totalPrice)
			{
				saleChangeMoney = saleReceiveMoney - SaleData.totalPrice;

				saleEnableSumPrt = 1;
				if(scaleUsePrt == PRT_DEP50)
				{
					salePrtSum();
					salePrtPayment();
				}
				
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
				saleEnableSumPrt = 1;
				if(scaleUsePrt == PRT_DEP50) salePrtSum();
				saleReceiveMoney = SaleData.totalPrice;
				if(scaleUsePrt == PRT_DEP50) salePrtPayment();
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
				saleEnableSumPrt = 1;
				salePrtLineFlag = 1;
				salePrtMultiItem();	
				saleFuncClearUnitPrice();		// PrintŰ�� �ǸŰ� �̷���� ��� 
				saleHalfQuartEnable = 1;
				saleMultiCount = 0;			//add kkanno 080326
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
* @brief    Double Zero Key �Լ�    
* @param    none
* @return   none
* @remark   �����¿� ���� ���ϴ� ���� "00"�� �߰��Ѵ�.
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

/**
********************************************************************************
* @brief    ADD Key �Լ�    
* @param    none
* @return   none
* @remark   ���� �Ǹſ� �����ǸŽ� Add ������ �����Ѵ�.			\n
*           Add�� �� 1000ȸ ���� �����ϴ�. 						\n
*			Add �� Print��� ������ ���� ���������� Print �ȴ�.
********************************************************************************
*/
void saleAddKey(void)
{
	INT32U tempAddPrice;
	INT16U tempAddCnt;
		
	switch (SaleData.saleState)
	{
		case SALE_STATE_HOLD:			  
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
			if ((SaleData.totalPrice > 0) && (saleLastAddedPrice == 0))			/* weight Item ���� �Ǹ��� ��� */
			{		
				tempAddCnt = SaleData.addCount + 1;
				tempAddPrice = SaleData.addPrice + SaleData.totalPrice;
							
				if (tempAddPrice <= TOTALPRICE_MAX && tempAddCnt <= ADD_LIMIT_COUNT) // sele by weight	change kkanno 070615
				{	
					SaleData.addPrice = tempAddPrice;
					SaleData.addCount = SaleData.addCount + 1;					

					if (saleUseDailyTotalFlag) 			/* use daily total */
					{	 
						saleFuncDailyTotalSave(0, 1, SaleData.totalPrice);		// plu ���굵 ����
						saleAutoTransactionWeightCheck = OFF;
					}								

					SaleData.saleState = SALE_STATE_ADD;
					if (saleFirstAdd) salePrtLineFlag = 1;
					if (scaleUsePrt == PRT_DEP50)	salePrtWeightItem();
					if (saleFirstAdd) saleFirstAdd = 0;
					saleHalfQuartEnable = 1;
					transactionEndFlag = TRUE;
				}
				else
				{				
					DispSetError(ERROR_NUM_TOTAL_ADDPRICE_OVER);
					SaleData.saleState = SALE_STATE_NORMAL;
				}
			}
			else if((SaleData.totalPrice > 0) && (saleLastAddedPrice != 0))
			{
				tempAddCnt = SaleData.addCount + 1;
				tempAddPrice = SaleData.addPrice + SaleData.totalPrice;
							
				if (tempAddPrice <= TOTALPRICE_MAX && tempAddCnt <= ADD_LIMIT_COUNT) // sele by weight	change kkanno 070615
				{	
					SaleData.addPrice = tempAddPrice;
					SaleData.addCount = SaleData.addCount + 1;					

					if (saleUseDailyTotalFlag) 			/* use daily total */
					{	
						saleFuncDailyTotalSave(1, 1, saleLastAddedPrice);		// plu ���굵 ����
						saleFuncDailyTotalSave(0, 1, SaleData.totalPrice);
						saleAutoTransactionWeightCheck = OFF;
						saleLastAddedPrice = 0;
					}								

					SaleData.saleState = SALE_STATE_ADD;
					if (saleFirstAdd) salePrtLineFlag = 1;
					if (scaleUsePrt == PRT_DEP50)	salePrtWeightItem();
					if (saleFirstAdd) saleFirstAdd = 0;
					saleHalfQuartEnable = 1;
					transactionEndFlag = TRUE;
				}
				else
				{				
					DispSetError(ERROR_NUM_TOTAL_ADDPRICE_OVER);
					SaleData.saleState = SALE_STATE_NORMAL;
				}
			}
			else								/* Fixed  item ���� �Ǹ��� ��� */
			{
				BuzzerSetOnTimes(2);		
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
			
			tempAddCnt = SaleData.addCount + 1;
			tempAddPrice = SaleData.addPrice + SaleData.totalPrice;
			if ((tempAddPrice <= TOTALPRICE_MAX) && (tempAddCnt <= ADD_LIMIT_COUNT) && ((SaleData.unitPrice * saleMultiCount) <= TOTALPRICE_MAX))	//change kkanno 070615
			{					
				SaleData.addPrice = tempAddPrice;
				SaleData.addCount = SaleData.addCount + 1;

				if (saleUseDailyTotalFlag) 			/* use daily total */
				{	 
					saleFuncDailyTotalSave(0, 1, SaleData.totalPrice);		// plu ���굵 ����
				}
				if (saleFirstAdd) salePrtLineFlag = 1;
				salePrtMultiItem();
				saleMultiCount = 0;			//add kkanno 080326
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
	}
}


/**
********************************************************************************
* @brief    FIXED ADD Key �Լ�    
* @param    none
* @return   none
* @remark   fixed Item �ǸŽ� ����Ѵ�.
*          
********************************************************************************
*/
void saleFAddKey(void)
{
	INT32U tempAddPrice;
	INT16U tempAddCnt;
		
	switch (SaleData.saleState)
	{
		case SALE_STATE_HOLD:
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
			if(SaleData.totalPrice == 0)
			{		
				if (SaleData.weight == 0 && SaleData.unitPrice > 0 )
				{				
					tempAddPrice = SaleData.addPrice + SaleData.unitPrice;
					tempAddCnt = SaleData.addCount + 1; 

					if (tempAddPrice <= TOTALPRICE_MAX && tempAddCnt <= ADD_LIMIT_COUNT)	//change kkanno 070615
					{	
						SaleData.addPrice = SaleData.addPrice + SaleData.unitPrice;
						SaleData.addCount = SaleData.addCount + 1;

						if (saleUseDailyTotalFlag)			/* use daily total */
						{	
							saleFuncDailyTotalSave(0, 1, SaleData.unitPrice);	  // plu ���굵 ���� 
						}
						SaleData.saleState = SALE_STATE_FIXED_PRICE;	
						if (saleFirstAdd) salePrtLineFlag = 1;
						if (scaleUsePrt == PRT_DEP50)	salePrtFixedItem();
						if (saleFirstAdd) saleFirstAdd = 0; 					
						saleHalfQuartEnable = 1;
						transactionEndFlag = TRUE;	
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
			else
			{
				BuzzerSetOnTimes(2);		
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
			
			tempAddCnt = SaleData.addCount + 1;
			tempAddPrice = SaleData.addPrice + SaleData.totalPrice;
			if ((tempAddPrice <= TOTALPRICE_MAX) && (tempAddCnt <= ADD_LIMIT_COUNT) && ((SaleData.unitPrice * saleMultiCount) <= TOTALPRICE_MAX))	//change kkanno 070615
			{					
				SaleData.addPrice = tempAddPrice;
				SaleData.addCount = SaleData.addCount + 1;

				if (saleUseDailyTotalFlag)			/* use daily total */
				{	 
					saleFuncDailyTotalSave(0, 1, SaleData.totalPrice);		// plu ���굵 ����
				}
				if (saleFirstAdd) salePrtLineFlag = 1;
				salePrtMultiItem();
				saleMultiCount = 0; 		//add kkanno 080326
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
	}
}




/**
********************************************************************************
* @brief    Mood Key �Լ�    
* @param    none
* @return   none
* @remark   Daily total ���� ������ PLU ���������� Ȯ���Ҽ� �մ� ���� ����
*          ���� �����ʹ� Clear Key�� �����Ѵ�. (PLU�� ����Clear�� �Ұ�����)
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
		case SALE_STATE_MODE_PLU:
			SaleData.pluNum = 0;
			SaleData.totalPrice = 0;
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
* @brief    Cancel Key �Լ�    
* @param    none
* @return   none
* @remark   ���� �Ǹ� ������ ����ϴ� ��� 						\n
*           Total price�� 0�� �ɶ� ���� ��� ����             	\n
*			Add �� Print��� ������ ���� ���������� Print �ȴ�.
********************************************************************************
*/
void saleCancelKey(void)
{
	INT16U	tempCount;
	INT32S	tempPrice;

	switch (SaleData.saleState)
	{	
		case SALE_STATE_HOLD:
		case SALE_STATE_NORMAL:

			if(!saleAutoTransactionWeightCheck)	  
			{
				salePrtWeightItemCancel();
				saleFuncDailyTotalSave(1, 1, SaleData.totalPrice); // ���̸� cancle
				BuzzerSetOnTimes(1);
				SaleData.totalPrice = 0;
				SaleData.unitPrice = 0;
				return;
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
						salePrtWeightItemCancel();
						transactionEndFlag = TRUE;

						if (saleUseDailyTotalFlag) 			/* use daily total */
						{ 
							saleFuncDailyTotalSave(1, 1, SaleData.totalPrice);
						}
						saleFuncClearUnitPrice();
						SaleData.unitPrice = 0L;			// Cancel�� ������ clear
					}
					else
					{
						BuzzerSetOnTimes(2);					
					}
				}
				else  // total price = 0�϶�  -> Fixed price cancel _print								
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
							salePrtFixedItemCancel();
							transactionEndFlag = TRUE;
							if (saleUseDailyTotalFlag) 			/* use daily total */
							{ 
								saleFuncDailyTotalSave(1, 1, SaleData.unitPrice);
							}						
							saleFuncClearUnitPrice();
							SaleData.unitPrice = 0L;		// Cancel�� ������ clear
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
			if (saleReceiveMoney > 0)			// Discount ���� 
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
					salePrtDiscountItem();
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
			SaleData.saleState = SALE_STATE_NORMAL;
			break;
	}	
}

/**
********************************************************************************
* @brief    Multi Key �Լ�    
* @param    none
* @return   none
* @remark   ���� �ǸŽ� �ټ� �Ǹ� ����� ����				\n
*			Add �� Print��� ������ ���� ���������� Print �ȴ�.
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
* @brief    Percent Tare ���� �����ϴ� �Լ�    
* @param    none
* @return   none
* @remark   Tare�� Unit price, Tare %���� �Է¹����� ����
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
* @brief    Percent Tare ���� ȣ�� �ϴ� �Լ�    
* @param    none
* @return   none
* @remark   �Էµ� Tare���� unit price, %Tare ���� ȣ������ ���� ������� ����  	\n
*           ���� Tare���� ���õǰ� ���� ����� Tare���� ����
********************************************************************************
*/
void salePercentTareCallKey(void) 
{	
	saleFuncReadPercentTareData();

	if (salePercentTareCallFlag == OFF  && AdData.tareWeight == 0)		 
	{								
		DispSetConstStrMsg(WIN_WEIGHT, "PRCENT");
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
		DispSetConstStrMsg(WIN_WEIGHT, "PRCENT");
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
* @brief    Prepack Key �Լ�    
* @param    none
* @return   none
* @remark   Prepack ����� �����ϴ� key							\n
*           Unit Price�� �Ǹ��Ŀ��� clear���� ����
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
* @brief    Tare save Key �Լ�    
* @param    none
* @return   none
* @remark   Tare save ����� �����ϴ� key							\n
*           Tare ���� �� �Ǹ��Ŀ��� clear���� ����
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
* @brief    Both(unit price, tare) save Key �Լ�    
* @param    none
* @return   none
* @remark   unit price���� Tare save ����� �����ϴ� key					\n
*           Unit Price, tare �� �Ǹ��Ŀ��� clear���� ����
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
* @brief    print ��������� �����ϴ� Key
* @param    none
* @return   none
* @remark   print ��������� �����ϴ� key�� ���� 							
********************************************************************************
*/
void salePrintEnableKey(void)
{
	if (scaleUsePrt == PRT_NONE || scaleUsePrt == PRT_RESERVED)
	{
		salePrtEnableFlag = 0;
		BuzzerSetOnTimes(2);
	}
	else
	{
		if (SaleData.saleState == SALE_STATE_NORMAL)
		{
			if (salePrtEnableFlag)
			{					
				salePrtEnableFlag = 0;
				DispSetConstStrMsg(WIN_WEIGHT, "P OFF ");
			}
			else
			{
				salePrtEnableFlag = 1;
				DispSetConstStrMsg(WIN_WEIGHT, "P ON");			
			}
			DispSetRunMsgPage(10, 1);
		}
		else
		{
			BuzzerSetOnTimes(2);	
		}
	}
}


/**
********************************************************************************
* @brief    unit ��ȯ Key �Լ�    
* @param    none
* @return   none
* @remark   kg, lb, oz, g ������ ������ ����� 					\n
*           ���� ���￡�� ��밡���� ������ C-4 table ���� 		\n
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

	if ((SaleData.saleState == SALE_STATE_NORMAL || SaleData.saleState == SALE_STATE_UNITCHANGE) 
		&& AdData.tareWeight == 0 && saleHundredGramEnable == 0)
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
* @brief    unit ��ȯ Key �Լ�    
* @param    none
* @return   none
* @remark   kg, lb, oz, g ������ ������ ����� 				\n
*           ���� ���￡�� ��밡���� ������ C-4 table ���� 		\n
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
* @brief    unit ��ȯ Key �Լ�    
* @param    none
* @return   none
* @remark   kg, lb, oz, g ������ ������ ����� 					\n
*           ���� ���￡�� ��밡���� ������ C-4 table ���� 		\n
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
* @brief    unit ��ȯ Key �Լ�    
* @param    none
* @return   none
* @remark   kg, lb, oz, g ������ ������ ����� 				\n
*           ���� ���￡�� ��밡���� ������ C-4 table ���� 		\n
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
* @brief    unit ��ȯ Key �Լ�    
* @param    none
* @return   none
* @remark   kg, lb, oz, g ������ ������ ����� 					\n
*           ���� ���￡�� ��밡���� ������ C-4 table ���� 		\n
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

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

/**
********************************************************************************
* @brief    Number�Է½� Window ������ �Բ� �����Ҽ�  �ִ� �Լ�
* @param    winNum:������ ��ġ,  position:�Ҽ�����ġ, type:�Ҽ��� ���� \n
*			num:display data, 	 thousandPt : 1000���� comma��� ���� 
* @return   none
* @remark   DispSetDecPoint �� DispSetNum�� ���ĳ��� �Լ�
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
* @brief    PLU ����Ÿ read�� ���� üũ 
* @param    inputPluNum: Plu��ȣ  
* @return   none
* @remark   Euro���� unitpirce�� Euro_rate�� ���� ����� 
*			PluDirectData.unitPrice �� PRICE_MAX���� Ŭ���  ����� �����͸� �ʱ�ȭ \n
* 		 	unitPrice = 0;			\n
*			tareValue = 0;			\n
*			pluName = "IP-XXX"
********************************************************************************
*/	
INT8U saleFuncPluDataReadInSale(INT16U inputPluNum)
{
	INT8U i,j,k;	//kkh0225 code optimaization

	if (inputPluNum <= DIRECT_PLU_MAX)
	{
		PluReadDirectPlu(inputPluNum, &PluDirectData);							
										
		if (PluDirectData.unitPrice > UNITPRICE_MAX)
		{
			PluDirectData.unitPrice = 0;
			PluDirectData.tare = 0;	
			PluDirectData.validDate = 0;
		}
		
		for (i = 0, j = 0; i < PLU_NAME_SIZE; i++)
		{
			if (PluDirectData.name[i] == 0x20) j++;
		}
		
		if (j > PLU_NAME_SIZE - 1)
		{
			MakeStrWithNum(saleDefaultPluName, "IP-", inputPluNum, 6);
			for(k = 6; k < 16; k++) //kkh0225 code optimaization
			{
				saleDefaultPluName[k] = ' ';
			}
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
				DispSetError(ERROR_NUM_ALREADY_TARE);	//TODO : �ٸ� ERROR�� ����??
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
			tempPluData.validDate = PluDirectData.validDate;
			tempPluData.itemCode = PluDirectData.itemCode;
		}
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
* @brief    DailyTotal�� �����ϴ� �Լ� 
* @param    sign : ���嵥���Ͱ� Add ����, minus������ ����    	\n
*			count: ����Ǵ� Ƚ���� ����							\n
*			price: ����Ǵ� ���� ����							\n
* @return   none
* @remark   Plu�� ���� �����ϰ�� PLU���� �����͵� �Բ� ó����
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
* @brief    Euro�� �޷��� �����ϴ� �Լ�  
* @param    inputPrice : ������ Euro     	\n
* @return   ����� �޷� �� 
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
* @brief    �޷��� Euro_rate�� ���� �����ϴ� �Լ�  
* @param    inputPrice : ������ �޷� ��     	\n
* @return   ����� Euro�� 
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
* @brief    Euro�� �޷��� �����ϴ� �Լ�  
* @param    inputPrice : ������ Euro     	\n
* @return   ����� �޷� �� 
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
* @brief    ���� sale�Ϸ��� unitprice�� Plu Number�� Ŭ�����ϴ� �Լ�   
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void saleFuncClearUnitPrice(void)
{
	if (salePriceClearFlag && salePercentTareCallFlag == 0)
	{
		SaleData.unitPrice = 0L;
		SaleData.pluNum = 0;	
		saleFuncClearPluName();
	}	
}

/**
********************************************************************************
* @brief    ���� sale�Ϸ��� PLU name�� Ŭ�����ϴ� �Լ�   
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
* @brief    PLU_SAVE Key�� ���� �޾Ƶ��� Data�� �����ϴ� �Լ�    
* @param    inputPluNum : ���� ����� plunum�� ����� ��ġ�� ǥ���� 
* @return   none
* @remark   Plu name�� garbage���� ��������� PLU name�� IP-xxx���� ó���� 
********************************************************************************
*/
void saleFuncPluDataWriteInSale(INT16U inputPluNum)
{
	if (inputPluNum > 0 && inputPluNum <= DIRECT_PLU_MAX)
	{
		PluDirectData = tempPluData;
		tempPluData.tare = 0;
		tempPluData.validDate = 0;
		AdSetPT(tempPluData.tare, 1);
		saleTareType = PLU_TARE_CALL;
		PluWriteDirectPlu(inputPluNum, &PluDirectData);
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

#ifdef USE_PRINTER
/**
********************************************************************************
* @brief    ����Ÿ ��½� start line�� ���� ����     
* @param    none
* @return   none
* @remark   saleUseHeadMsgFlag = head �޼��� ��� ���θ� ������  	 		\n
*			salePrtDayFlag   = ��¥ ���� ��� ���θ� ������
********************************************************************************
*/
const char code salePrtDayStrTable[7][6] = 
{
	"MON. ", "TUE. ", "WED. ", "THU. ", "FRI. ", "SAT. ", "SUN. "
};	

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
	PrtSetTabPos(8, 9);

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
			TimeReadData();
			PrtPutStr("DATE. ");
			
			switch(saleDateType)
			{
				case 0:
				default:	//YYYY / MM / DD / DAY
					PrtPutDecNum(2000 + TimeData.year, 4, PRT_NUM_TYPE_FILL_ZERO);	PrtPutChar('/');
					PrtPutDecNum(TimeData.month, 2, PRT_NUM_TYPE_FILL_ZERO); 		PrtPutChar('/');
					PrtPutDecNum(TimeData.date, 2, PRT_NUM_TYPE_FILL_ZERO); 			PrtPutChar('/');
					PrtPutConstStr(salePrtDayStrTable[TimeData.day - 1]);
					PrtPutCRLF(1);
					break;
				case 1:	//DAY / DD / MM / YYYY
					PrtPutConstStr(salePrtDayStrTable[TimeData.day - 1]);			PrtPutChar('/');
					PrtPutDecNum(TimeData.date, 2, PRT_NUM_TYPE_FILL_ZERO); 		PrtPutChar('/');
					PrtPutDecNum(TimeData.month, 2, PRT_NUM_TYPE_FILL_ZERO); 	PrtPutChar('/');
					PrtPutDecNum(2000 + TimeData.year, 4, PRT_NUM_TYPE_FILL_ZERO);
					PrtPutCRLF(1);
					break;
			}
			PrtPutStr("TIME. ");
			PrtPutDecNum(TimeData.hour, 2, PRT_NUM_TYPE_FILL_ZERO); 		PrtPutChar(':');
			PrtPutDecNum(TimeData.min, 2, PRT_NUM_TYPE_FILL_ZERO); 		PrtPutChar(':');
			PrtPutDecNum(TimeData.sec, 2, PRT_NUM_TYPE_FILL_ZERO); 
			PrtPutCRLF(1);
		}
//		PrtPutConstStrWithCRLF("1t3456789_t2tt56789_t23tt6789_");
//		PrtPutConstStrWithCRLF(" weight   unit price  total  ");		
		PrtPutLine();
		PrtPutTab(1);	// Teb 1
		PrtPutConstStr("Weight/");	//AP OIML
		PrtPutTab(1);	// Teb 2
		PrtPutConstStr("Unit price");
		PrtPutTab(4);	// Teb 3
		PrtPutConstStr("Total");
		PrtPutCRLF(1);	

//		PrtPutConstStrWithCRLF("1t3456789_t2tt56789_t23tt6789_");
//		PrtPutConstStrWithCRLF(" (Kg)        \/Kg       \     ");			
		PrtPutTab(1);
		PrtPutConstStr("Quantity");	// AP OIML	
		PrtPutTab(3);	// Teb 2
		/*	  AP OIML
		PrtPutChar('(');	PrtPutStr(saleUsePriceUnit);	PrtPutChar('/');
		if (salePriceForUnit == 2 )
		{
			PrtPutNum(DecPowerConvTable[salePriceForUnit], 3, 0, 0, 0);
			PrtPutChar('g');		
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
		PrtPutChar(')');
		*/  
		PrtPutTab(2);	// Teb 3
		PrtPutChar('(');	PrtPutStr(saleUsePriceUnit);	PrtPutChar(')');  
		PrtPutCRLF(1);
		PrtPutLine();
		salePrtLineFlag = 0;
	}
}

/**
********************************************************************************
* @brief    ����Ÿ ��½� weight item�� ���� �Ǹ� ���� ���     
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtWeightItem(void)
{	
	INT16U year;
	INT16U month;
	INT16U date;
	if (salePrtEnableFlag)
	{		
		if(scaleUsePrt == PRT_DEP50)
		{
			salePrtStartCondition();			
			if (saleUsePluNameFlag)
			{
				salePrtPluName();
			}
	//		PrtPutConstStrWithCRLF("1t3456789_t2tt56789_t23tt6789_");
	//		PrtPutConstStrWithCRLF(" xxxxx,xx xxxxxx,xx xxxxxxx,xx");
			PrtPutNum(SaleData.weight, 6, SaleData.weightDecPos, weightDotTypeFlag, 0);
			PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);	
			PrtPutTab(8);		
			if (saleUseEuroFlag) PrtPutNum(SaleData.unitPrice, 7, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			else PrtPutNum(SaleData.unitPrice, 7, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);

			PrtPutStr(saleUsePriceUnit);  // AP OIML
			PrtPutChar('/');
			PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);

			if (saleUseEuroFlag) PrtPutNum(SaleData.totalPrice, 8, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			else PrtPutNum(SaleData.totalPrice, 8, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			PrtPutCRLF(1);
			if (saleEnableSumPrt)
			{
				PrtPutLine();
				PrtPutConstStr("Sum(");	PrtPutStr(saleUsePriceUnit);	PrtPutChar(')');
				PrtPutTab(5);		// Teb 3
				if (saleUseEuroFlag) PrtPutNum(SaleData.totalPrice, 9, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
				else PrtPutNum(SaleData.totalPrice, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
				PrtPutCRLF(9);
				saleEnableSumPrt = 0;
			}	
		}		
		else if (scaleUsePrt == PRT_DLP50)
		{
			if (PrtSetting.barcodeType == TYPE_PRICE)            salePrtDlpStartCondition(TYPE_PRICE);	   //Start command price type
			else if (PrtSetting.barcodeType == TYPE_WEIGHT) salePrtDlpStartCondition(TYPE_WEIGHT); //Start command weight type
			salePrtDlp(WIN_WEIGHT);											//V00 : Weight
			PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);					//V01 : Weight Unit
			PrtPutChar(LF);
			salePrtDlp(WIN_UNIT_PRICE);										//V02 : Unit Price
			salePrtDlp(WIN_TOTAL_PRICE);										//V03 : Total Price
			PrtPutDecNum(SaleData.weight, 5, PRT_NUM_TYPE_FILL_ZERO);			//V04 : Weight(Barcode)
			PrtPutChar(LF);
			PrtPutDecNum(SaleData.unitPrice, 6, PRT_NUM_TYPE_FILL_ZERO);		//V05 : Unit Price(Barcode)
			PrtPutChar(LF);
			PrtPutDecNum(SaleData.totalPrice, 5, PRT_NUM_TYPE_FILL_ZERO);		//V06 : Total Price(Barcode)
			PrtPutChar(LF);
			salePrtDlpPluName();												//V07 : PLU name
																			//V08 : PLU number
			PrtPutData(salePriceUnit[saleUseEuroFlag], 1);						//V09 : unit unit
			PrtPutChar('/');
			if(saleHundredGramEnable == 1)
			{
				PrtPutData(saleWeightUnit[4], 4);							   //unit unit : 100g
			}
			else
			{
				PrtPutData(saleWeightUnit[SaleData.weightUnit], 4);			   //unit unit : weight unit
			}
			PrtPutChar(LF);
			PrtPutData(salePriceUnit[saleUseEuroFlag], 1);						//V10 : total price unit
			PrtPutChar(LF);
			//Add location V11 or more...										//V11 : Packed on date
			TimeReadData();
			switch(saleDateType)
			{
				case 0:	//YY.MM.DD
				default:
					PrtPutDecNum(TimeData.year, 2, PRT_NUM_TYPE_FILL_ZERO);
					PrtPutChar('.');
					PrtPutDecNum(TimeData.month, 2, PRT_NUM_TYPE_FILL_ZERO);
					PrtPutChar('.');
					PrtPutDecNum(TimeData.date, 2, PRT_NUM_TYPE_FILL_ZERO);
					PrtPutChar(LF);
					
					//Add location V11 or more...										//V12 : Sell by date
					if(SaleData.pluNum && (tempPluData.validDate > 0))
					{
						year = (INT16U)TimeData.year + 2000;
						month = (INT16U)TimeData.month;
						date = (INT16U)TimeData.date;
						TimeGetNextDay(&year, &month, &date, (tempPluData.validDate - 1));
						PrtPutDecNum(year - 2000, 2, PRT_NUM_TYPE_FILL_ZERO);
						PrtPutChar('.');
						PrtPutDecNum(month, 2, PRT_NUM_TYPE_FILL_ZERO);
						PrtPutChar('.');
						PrtPutDecNum(date, 2, PRT_NUM_TYPE_FILL_ZERO);
					}
					else
					{
						PrtPutData("        ", 8);	
					}
					break;
				case 1:	//DD.MM.YY
					PrtPutDecNum(TimeData.date, 2, PRT_NUM_TYPE_FILL_ZERO);
					PrtPutChar('.');
					PrtPutDecNum(TimeData.month, 2, PRT_NUM_TYPE_FILL_ZERO);
					PrtPutChar('.');
					PrtPutDecNum(TimeData.year, 2, PRT_NUM_TYPE_FILL_ZERO);
					PrtPutChar(LF);
					
					//Add location V11 or more...										//V12 : Sell by date
					if(SaleData.pluNum && (tempPluData.validDate > 0))
					{
						year = (INT16U)TimeData.year + 2000;
						month = (INT16U)TimeData.month;
						date = (INT16U)TimeData.date;
						TimeGetNextDay(&year, &month, &date, (tempPluData.validDate - 1));
						PrtPutDecNum(date, 2, PRT_NUM_TYPE_FILL_ZERO);
						PrtPutChar('.');
						PrtPutDecNum(month, 2, PRT_NUM_TYPE_FILL_ZERO);
						PrtPutChar('.');
						PrtPutDecNum(year - 2000, 2, PRT_NUM_TYPE_FILL_ZERO);
					}
					else
					{
						PrtPutData("        ", 8);	
					}
					break;
			}
			PrtPutChar(LF);			
			PrtPutDecNum(saleWeightPrefix, 2, PRT_NUM_TYPE_FILL_ZERO);			//V13 : Prefix(Barcode)			
			PrtPutChar(LF);			
			if(SaleData.pluNum) PrtPutDecNum(PluDirectData.itemCode, 5, PRT_NUM_TYPE_FILL_ZERO);		//V14 : itemCode(Barcode)
			else PrtPutDecNum(0, 5, PRT_NUM_TYPE_FILL_ZERO);		//V14 : itemCode(Barcode)
			PrtPutChar(LF);
			PrtPutData("P1,1", 4);													//End command		
			PrtPutChar(LF);
		}
	}
}

/**
********************************************************************************
* @brief    ����Ÿ ��½� Fixed_value item�� ���� �Ǹ� ���� ���     
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtFixedItem(void)
{
	INT16U year;
	INT16U month;
	INT16U date;
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
			PrtPutConstStr("Fixed Item(pcs)");	 
			PrtPutTab(4);
			if (saleUseEuroFlag) PrtPutNum(SaleData.unitPrice, 9, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			else PrtPutNum(SaleData.unitPrice, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			PrtPutCRLF(1);
			
			if (saleEnableSumPrt)
			{
				PrtPutLine();
				PrtPutConstStr("Sum(");	PrtPutStr(saleUsePriceUnit);	PrtPutChar(')');
				PrtPutTab(5);		// Teb 3
				if (saleUseEuroFlag) PrtPutNum(SaleData.unitPrice, 9, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
				else PrtPutNum(SaleData.unitPrice, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
				PrtPutCRLF(9);
				saleEnableSumPrt = 0;
			}
		}
		else if (scaleUsePrt == PRT_DLP50)
		{
			if (PrtSetting.barcodeType == TYPE_PRICE)		salePrtDlpStartCondition(TYPE_PRICE);	   //Start command price type
			else if (PrtSetting.barcodeType == TYPE_WEIGHT) salePrtDlpStartCondition(TYPE_WEIGHT); //Start command weight type
			//salePrtDlp(WIN_WEIGHT);											//V00 : Weight			
			PrtPutDecNum(1, 6, PRT_NUM_TYPE_FILL_SPACE);						//V00 : 1 PCS, fixed sale
			PrtPutChar(LF);			
			PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);					//V01 : Weight Unit
			PrtPutChar(LF);			
			//salePrtDlp(WIN_UNIT_PRICE);										//V02 : Unit Price
			PrtPutData("       ", 7);	
			PrtPutChar(LF);			
			salePrtDlp(WIN_UNIT_PRICE);										//V03 : Total Price
			PrtPutDecNum(SaleData.weight, 5, PRT_NUM_TYPE_FILL_ZERO);		//V04 : Weight(Barcode)
			PrtPutChar(LF);			
			PrtPutDecNum(SaleData.unitPrice, 6, PRT_NUM_TYPE_FILL_ZERO);		//V05 : Unit Price(Barcode)
			PrtPutChar(LF);			
			if (PrtSetting.barcodeType == TYPE_PRICE) PrtPutDecNum(SaleData.unitPrice, 5, PRT_NUM_TYPE_FILL_ZERO);		//V06 : Total Price(Barcode)
			else if (PrtSetting.barcodeType == TYPE_WEIGHT) PrtPutDecNum(1, 5, PRT_NUM_TYPE_FILL_ZERO);	//V06 : Total Price(Barcode)
			PrtPutChar(LF);			
			salePrtDlpPluName();												//V07 : PLU name
																			//V08 : PLU number
			PrtPutData(salePriceUnit[saleUseEuroFlag], 1);						//V09 : unit unit
			PrtPutChar('/');
			if (SaleData.weight) PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);
			else PrtPutData("pcs", 3);
			PrtPutChar(LF);			
			PrtPutData(salePriceUnit[saleUseEuroFlag], 1);						//V10 : total price unit
			PrtPutChar(LF);			
			//Add location V11 or more...										//V11 : Packed on date
			TimeReadData();
			switch(saleDateType)
			{
				case 0:	//YY.MM.DD
				default:
					PrtPutDecNum(TimeData.year, 2, PRT_NUM_TYPE_FILL_ZERO);
					PrtPutChar('.');
					PrtPutDecNum(TimeData.month, 2, PRT_NUM_TYPE_FILL_ZERO);
					PrtPutChar('.');
					PrtPutDecNum(TimeData.date, 2, PRT_NUM_TYPE_FILL_ZERO);
					PrtPutChar(LF);
					
					//Add location V11 or more...										//V12 : Sell by date
					if(SaleData.pluNum && (tempPluData.validDate > 0))
					{
						year = (INT16U)TimeData.year + 2000;
						month = (INT16U)TimeData.month;
						date = (INT16U)TimeData.date;
						TimeGetNextDay(&year, &month, &date, (tempPluData.validDate - 1));
						PrtPutDecNum(year - 2000, 2, PRT_NUM_TYPE_FILL_ZERO);
						PrtPutChar('.');
						PrtPutDecNum(month, 2, PRT_NUM_TYPE_FILL_ZERO);
						PrtPutChar('.');
						PrtPutDecNum(date, 2, PRT_NUM_TYPE_FILL_ZERO);
					}
					else
					{
						PrtPutData("        ", 8);	
					}
					break;
				case 1:	//DD.MM.YY
					PrtPutDecNum(TimeData.date, 2, PRT_NUM_TYPE_FILL_ZERO);
					PrtPutChar('.');
					PrtPutDecNum(TimeData.month, 2, PRT_NUM_TYPE_FILL_ZERO);
					PrtPutChar('.');
					PrtPutDecNum(TimeData.year, 2, PRT_NUM_TYPE_FILL_ZERO);
					PrtPutChar(LF);
					
					//Add location V11 or more...										//V12 : Sell by date
					if(SaleData.pluNum && (tempPluData.validDate > 0))
					{
						year = (INT16U)TimeData.year + 2000;
						month = (INT16U)TimeData.month;
						date = (INT16U)TimeData.date;
						TimeGetNextDay(&year, &month, &date, (tempPluData.validDate - 1));
						PrtPutDecNum(date, 2, PRT_NUM_TYPE_FILL_ZERO);
						PrtPutChar('.');
						PrtPutDecNum(month, 2, PRT_NUM_TYPE_FILL_ZERO);
						PrtPutChar('.');
						PrtPutDecNum(year - 2000, 2, PRT_NUM_TYPE_FILL_ZERO);
					}
					else
					{
						PrtPutData("        ", 8);	
					}
					break;
			}
			PrtPutChar(LF);				
			PrtPutDecNum(saleCountPrefix, 2, PRT_NUM_TYPE_FILL_ZERO);			//V13 : Prefix(Barcode)
			PrtPutChar(LF);			
			if(SaleData.pluNum) PrtPutDecNum(PluDirectData.itemCode, 5, PRT_NUM_TYPE_FILL_ZERO);		//V14 : itemCode(Barcode)
			else PrtPutDecNum(0, 5, PRT_NUM_TYPE_FILL_ZERO);		//V14 : itemCode(Barcode)
			PrtPutChar(LF);
			PrtPutData("P1,1", 4);												//End command		
			PrtPutChar(LF);			
		}
	}
}

/**
********************************************************************************
* @brief    ����Ÿ ��½� Mul_ti item�� ���� �Ǹ� ���� ���     
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtMultiItem(void)
{	
	INT16U year;
	INT16U month;
	INT16U date;
	
	if (salePrtEnableFlag)
	{		
		if(scaleUsePrt == PRT_DEP50)
		{
			salePrtStartCondition();
	//		PrtPutConstStrWithCRLF("1t3456789_t2tt56789_t23tt6789_");
	//		PrtPutConstStrWithCRLF(" Fixed Item         xxxxxxxxxx");	
			if (saleUsePluNameFlag)
			{
				salePrtPluName();
			}
			PrtPutTab(1);
			PrtPutConstStr("Fixed Item(pcs)");
			PrtPutCRLF(1);

	//		PrtPutConstStrWithCRLF("1t3456789_t2tt56789_t23tt6789_");
	//		PrtPutConstStrWithCRLF(" xxxxxx   X         xxxxxxx,xx");
			PrtPutNum(saleMultiCount, 4, 0, 0, 0);
			PrtPutConstStr(" X");
			
			if (saleUseEuroFlag) PrtPutNum(SaleData.unitPrice, 9, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			else PrtPutNum(SaleData.unitPrice, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			
			PrtPutStr(saleUsePriceUnit);  // AP OIML
			PrtPutChar('/');
			PrtPutConstStr("pcs");	
						
			if (saleUseEuroFlag) PrtPutNum(SaleData.totalPrice, 8, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			else	PrtPutNum(SaleData.totalPrice, 8, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			
			PrtPutCRLF(1);
			if (saleEnableSumPrt)
			{
				PrtPutLine();
				PrtPutConstStr("Sum(");	PrtPutStr(saleUsePriceUnit);	PrtPutChar(')');
				PrtPutTab(5);		// Teb 3
				if (saleUseEuroFlag) PrtPutNum(SaleData.totalPrice, 9, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
				else PrtPutNum(SaleData.totalPrice, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
				PrtPutCRLF(9);
				saleEnableSumPrt = 0;
			}	
		}
		else if (scaleUsePrt == PRT_DLP50)
		{
			if (PrtSetting.barcodeType == TYPE_PRICE)            salePrtDlpStartCondition(TYPE_PRICE);	   //Start command price type
			else if (PrtSetting.barcodeType == TYPE_WEIGHT) salePrtDlpStartCondition(TYPE_WEIGHT); //Start command weight type
			PrtPutDecNum(saleMultiCount, 6, PRT_NUM_TYPE_FILL_SPACE);			//V00 : PCS
			PrtPutChar(LF);			
			PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);					//V01 : Weight Unit
			PrtPutChar(LF);
			salePrtDlp(WIN_UNIT_PRICE);										//V02 : Unit Price
			salePrtDlp(WIN_TOTAL_PRICE);										//V03 : Total Price
			PrtPutDecNum(SaleData.weight, 5, PRT_NUM_TYPE_FILL_ZERO);			//V04 : Weight(Barcode)
			PrtPutChar(LF);
			PrtPutDecNum(SaleData.unitPrice, 6, PRT_NUM_TYPE_FILL_ZERO);		//V05 : Unit Price(Barcode)
			PrtPutChar(LF);
			if (PrtSetting.barcodeType == TYPE_PRICE) PrtPutDecNum(SaleData.totalPrice, 5, PRT_NUM_TYPE_FILL_ZERO);		//V06 : Total Price(Barcode)
			else if (PrtSetting.barcodeType == TYPE_WEIGHT) PrtPutDecNum(saleMultiCount, 5, PRT_NUM_TYPE_FILL_ZERO);	//V06 : Total Price(Barcode)
			PrtPutChar(LF);
			salePrtDlpPluName();												//V07 : PLU name
																			//V08 : PLU number
			PrtPutData(salePriceUnit[saleUseEuroFlag], 1);						//V09 : unit unit
			PrtPutChar('/');
			PrtPutData("pcs", 3);
			PrtPutChar(LF);
			PrtPutData(salePriceUnit[saleUseEuroFlag], 1);						//V10 : total price unit
			PrtPutChar(LF);
			//Add location V11 or more...										//V11 : Packed on date
			TimeReadData();
			switch(saleDateType)
			{
				case 0:	//YY.MM.DD
				default:
					PrtPutDecNum(TimeData.year, 2, PRT_NUM_TYPE_FILL_ZERO);
					PrtPutChar('.');
					PrtPutDecNum(TimeData.month, 2, PRT_NUM_TYPE_FILL_ZERO);
					PrtPutChar('.');
					PrtPutDecNum(TimeData.date, 2, PRT_NUM_TYPE_FILL_ZERO);
					PrtPutChar(LF);
					
					//Add location V11 or more...										//V12 : Sell by date
					if(SaleData.pluNum && (tempPluData.validDate > 0))
					{
						year = (INT16U)TimeData.year + 2000;
						month = (INT16U)TimeData.month;
						date = (INT16U)TimeData.date;
						TimeGetNextDay(&year, &month, &date, (tempPluData.validDate - 1));
						PrtPutDecNum(year - 2000, 2, PRT_NUM_TYPE_FILL_ZERO);
						PrtPutChar('.');
						PrtPutDecNum(month, 2, PRT_NUM_TYPE_FILL_ZERO);
						PrtPutChar('.');
						PrtPutDecNum(date, 2, PRT_NUM_TYPE_FILL_ZERO);
					}
					else
					{
						PrtPutData("        ", 8);	
					}
					break;
				case 1:	//DD.MM.YY
					PrtPutDecNum(TimeData.date, 2, PRT_NUM_TYPE_FILL_ZERO);
					PrtPutChar('.');
					PrtPutDecNum(TimeData.month, 2, PRT_NUM_TYPE_FILL_ZERO);
					PrtPutChar('.');
					PrtPutDecNum(TimeData.year, 2, PRT_NUM_TYPE_FILL_ZERO);
					PrtPutChar(LF);
					
					//Add location V11 or more...										//V12 : Sell by date
					if(SaleData.pluNum && (tempPluData.validDate > 0))
					{
						year = (INT16U)TimeData.year + 2000;
						month = (INT16U)TimeData.month;
						date = (INT16U)TimeData.date;
						TimeGetNextDay(&year, &month, &date, (tempPluData.validDate - 1));
						PrtPutDecNum(date, 2, PRT_NUM_TYPE_FILL_ZERO);
						PrtPutChar('.');
						PrtPutDecNum(month, 2, PRT_NUM_TYPE_FILL_ZERO);
						PrtPutChar('.');
						PrtPutDecNum(year - 2000, 2, PRT_NUM_TYPE_FILL_ZERO);
					}
					else
					{
						PrtPutData("        ", 8);	
					}
					break;
			}
			PrtPutChar(LF);			
			PrtPutDecNum(saleCountPrefix, 2, PRT_NUM_TYPE_FILL_ZERO);			//V13 : Prefix(Barcode)			
			PrtPutChar(LF);			
			if(SaleData.pluNum) PrtPutDecNum(PluDirectData.itemCode, 5, PRT_NUM_TYPE_FILL_ZERO);		//V14 : itemCode(Barcode)
			else PrtPutDecNum(0, 5, PRT_NUM_TYPE_FILL_ZERO);		//V14 : itemCode(Barcode)
			PrtPutChar(LF);
			PrtPutData("P1,1", 4);												//End command		
			PrtPutChar(LF);
		}
	}
}
		
/**
********************************************************************************
* @brief    ����Ÿ ��½� Discount item�� ���� �Ǹ� ���� ���     
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
		PrtPutConstStr("Discount");
		PrtPutTab(4);
		PrtPutNum(-saleReceiveMoney, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);	
		PrtPutCRLF(1);
	}
}

/**
********************************************************************************
* @brief    ����Ÿ ��½� weight item �� ���� void ���� ���     
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

		PrtPutNum(SaleData.weight, 6, SaleData.weightDecPos, weightDotTypeFlag, 0);
		PrtPutData(saleWeightUnit[SaleData.weightUnit], 2); 
	
		PrtPutTab(8);
		if (saleUseEuroFlag) PrtPutNum(SaleData.unitPrice, 7, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		else PrtPutNum(SaleData.unitPrice, 7, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		
		PrtPutStr(saleUsePriceUnit);  // AP OIML
		PrtPutChar('/');
		PrtPutData(saleWeightUnit[SaleData.weightUnit], 2);

		if (saleUseEuroFlag) PrtPutNum(-SaleData.totalPrice, 8, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		else PrtPutNum(-SaleData.totalPrice, 8, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		PrtPutCRLF(1);
	}
}


/**
********************************************************************************
* @brief    ����Ÿ ��½� Fixed_price item �� ���� void ���� ���     
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
		PrtPutConstStr("Fixed Item(pcs)");
		PrtPutTab(4);
		if (saleUseEuroFlag) PrtPutNum(-SaleData.unitPrice, 9, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		else PrtPutNum(-SaleData.unitPrice, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		PrtPutCRLF(1);
	}
}

/**
********************************************************************************
* @brief    ����Ÿ ��½� Print number ���     
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
* @brief    ����Ÿ ��½� SUM_data ���     
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
		PrtPutConstStr("Sum/");  	
		PrtPutStr(saleUsePriceUnit);
		PrtPutTab(4);   	
		if (saleUseEuroFlag) PrtPutNum(SaleData.addPrice, 9, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		else PrtPutNum(SaleData.addPrice, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		PrtPutCRLF(1);

	//	PrtPutConstStrWithCRLF("1t3456789_t23456789_t23t56789_");
	//	PrtPutConstStrWithCRLF(" Count              xxxxxxx.xx");
		PrtPutTab(1);
		PrtPutConstStr("Count");
		PrtPutTab(6); 
		PrtPutNum(SaleData.addCount, 5, 0, 0, 0);
		PrtPutCRLF(1);
	}
}

/**
********************************************************************************
* @brief    ����Ÿ ��½� Payment_Data ���     
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
		PrtPutConstStr("Payment/");
		PrtPutStr(saleUsePriceUnit);
		PrtPutTab(4);		
		if (saleUseEuroFlag) PrtPutNum(saleReceiveMoney, 9, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		else PrtPutNum(saleReceiveMoney, 9, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
		PrtPutCRLF(1);

	//	PrtPutConstStrWithCRLF("1t3456789_t23456789_t23t56789_");
	//	PrtPutConstStrWithCRLF(" Remains/\          xxxxxxx.xx");
		PrtPutTab(1);
		PrtPutConstStr("Remains/");
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
* @brief    DLP-50 ���� ó���� ������ Protocol �Լ�      
* @param  none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtDlpStartCondition(INT8U printType)
{
	PrtPutChar(LF);	
	PrtPutConstStr("FR");
	PrtPutChar(0x22);	//"
	if(printType == TYPE_WEIGHT) 
	{
		if(saleMultiCount || !SaleData.weight) PrtPutConstStr("ERCW");
		else PrtPutConstStr("ERW");
	}
	else
	{
		if(saleMultiCount || !SaleData.weight) PrtPutConstStr("ERCP");
		else PrtPutConstStr("ERP");
	}
	PrtPutChar(0x22);	//"
	PrtPutChar(LF);
	PrtPutChar(0x3F);	//?
	PrtPutChar(LF);
}

/**
********************************************************************************
* @brief    DLP-50 ���� PLU name�� ������ Protocol �Լ�
* @param  none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtDlpPluName(void)
{
	if(SaleData.pluNum)
	{
		if (SaleData.pluNum <= DIRECT_PLU_MAX)
		{
			if(saleDefaultPluName[0] != 0x00) PrtPutData(saleDefaultPluName, PLU_NAME_SIZE);
			else PrtPutData(tempPluData.name, PLU_NAME_SIZE);
			PrtPutChar(LF);
			PrtPutNum(SaleData.pluNum, 3, 0, 0, 0);
			PrtPutChar(LF);
		}
		else
		{
			PrtPutData("NO_PLU          ", PLU_NAME_SIZE);
			PrtPutChar(LF);
			PrtPutNum(SaleData.pluNum, 3, 0, 0, 0); 
			PrtPutChar(LF);
		}
	}
	else
	{		
		PrtPutData("NO_PLU          ", PLU_NAME_SIZE);
		PrtPutChar(LF);
		PrtPutData("   ", 3);
		PrtPutChar(LF);
	}
}

/**
********************************************************************************
* @brief    DLP-50 ���� Data ������ Protocol �Լ�      
* @param  winNum : ������ �������� ����
* @return   none
* @remark   
********************************************************************************
*/
void salePrtDlp(INT8U winNum)
{
	switch (winNum)
	{
		case WIN_WEIGHT:
			PrtPutNum(SaleData.weight, 6, SaleData.weightDecPos, weightDotTypeFlag, saleUseThousandPtFlag);
			break;
			
		case WIN_UNIT_PRICE:
			if (saleUseEuroFlag) PrtPutNum(SaleData.unitPrice, 7, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			else PrtPutNum(SaleData.unitPrice, 7, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			break;
			
		case WIN_TOTAL_PRICE:
			if (saleUseEuroFlag) PrtPutNum(SaleData.totalPrice, 7, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			else PrtPutNum(SaleData.totalPrice, 7, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);
			break;

		default:
			break;			
	}
	PrtPutChar(LF);
}
#ifdef USE_CLERK_KEY
/**
********************************************************************************
* @brief     Protocol �Լ�      
* @param  none
* @return   none
* @remark   
********************************************************************************
*/
void saleOpenClerkKey(void)
{
	SaleData.saleState = SALE_STATE_OPENCLERK_CALL;
	saleClerkNum = 0;
}

/**
********************************************************************************
* @brief     Protocol �Լ�      
* @param  none
* @return   none
* @remark   
********************************************************************************
*/
void saleSendClerkKey(void)
{
	if (saleClerkNum > 0 && saleClerkNum < 21) 
	{
		saleClerkKey(saleClerkNum);
		saleFuncClearUnitPrice();
		SaleData.saleState = SALE_STATE_NORMAL;
	}
	else 
	{
		BuzzerSetOnTimes(2);
		saleClerkNum = 0;
	}
}

/**
********************************************************************************
* @brief     Protocol �Լ�      
* @param  none
* @return   none
* @remark   
********************************************************************************
*/
void saleClerkKey(INT8U clerkNum)
{
	PrtPutNum(clerkNum, 2, 0, 0, 0);					//clerk number
	PrtPutCRLF(1);

	PrtPutNum(SaleData.pluNum, 3, 0, 0, 0);					//PLU Number
	PrtPutCRLF(1);

	PrtPutNum(SaleData.weight, 6, SaleData.weightDecPos, weightDotTypeFlag, saleUseThousandPtFlag);					//weight
	PrtPutCRLF(1);
	if (saleUseEuroFlag) PrtPutNum(SaleData.unitPrice, 7, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);	//unit price EURO
	else PrtPutNum(SaleData.unitPrice, 7, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);							//unit price NON EURO
	PrtPutCRLF(1);
	if (saleUseEuroFlag) PrtPutNum(SaleData.totalPrice, 7, saleEuroDispDecPos, priceDotTypeFlag, saleUseThousandPtFlag);	//total price EURO
	else PrtPutNum(SaleData.totalPrice, 7, priceDecPos, priceDotTypeFlag, saleUseThousandPtFlag);							//total price NON EURO
	PrtPutCRLF(1);
	saleFuncClearUnitPrice();
}

/**
********************************************************************************
* @brief     Protocol �Լ�      
* @param  none
* @return   none
* @remark   
********************************************************************************
*/
void saleClerkKey1st(void)
{
	saleClerkKey(1);
}

/**
********************************************************************************
* @brief     Protocol �Լ�      
* @param  none
* @return   none
* @remark   
********************************************************************************
*/
void saleClerkKey2nd(void)
{
	saleClerkKey(2);
}

/**
********************************************************************************
* @brief     Protocol �Լ�      
* @param  none
* @return   none
* @remark   
********************************************************************************
*/
void saleClerkKey3rd(void)
{
	saleClerkKey(3);
}
/**
********************************************************************************
* @brief     Protocol �Լ�      
* @param  none
* @return   none
* @remark   
********************************************************************************
*/
void saleClerkKey4th(void)
{
	saleClerkKey(4);
}

/**
********************************************************************************
* @brief     Protocol �Լ�      
* @param  none
* @return   none
* @remark   
********************************************************************************
*/
void saleClerkKey5th(void)
{
	saleClerkKey(5);
}
#endif

#ifdef USE_BARCODE_PRINT
/**
********************************************************************************
* @brief     Protocol �Լ�      
* @param  none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtBarcode(void)
{
	INT8U preFixStr[2];
	INT8U itemCodeStr[5];
	INT8U weightStr[5];
	INT16U even;
	INT16U odd;
	INT16U checkSum;

	if(!SaleData.pluNum)
	{
		BuzzerSetOnTimes(1);
		return;
	}

	Int32sToDecStr(preFixStr, saleWeightPrefix, 2, 0x30, 0, 0, 0);
	Int32sToDecStr(itemCodeStr, PluDirectData.itemCode, 5, 0x30, 0, 0, 0);
	Int32sToDecStr(weightStr, SaleData.weight, 5, 0x30, 0, 0, 0);

	even = 0;
	odd = 0;
	checkSum = 0;
	
	even = (preFixStr[1] - '0') + (itemCodeStr[1] -'0') + (itemCodeStr[3] - '0') + (weightStr[0] - '0') + (weightStr[2] - '0') + (weightStr[4] - '0');
	odd = (preFixStr[0] - '0') + (itemCodeStr[0] - '0') + (itemCodeStr[2] - '0') + (itemCodeStr[4] - '0') + (weightStr[1] - '0') + (weightStr[3] - '0');
	even *= 3;
	checkSum = even + odd;
	checkSum = 10 - (checkSum % 10);
	if (checkSum == 10) checkSum = 0;

	PrtPutChar('A');
	PrtPutDecNum(saleWeightPrefix, 2, PRT_NUM_TYPE_FILL_ZERO);
	PrtPutDecNum(PluDirectData.itemCode, 5, PRT_NUM_TYPE_FILL_ZERO);
	PrtPutDecNum(SaleData.weight, 5, PRT_NUM_TYPE_FILL_ZERO);
	PrtPutDecNum(checkSum, 1, PRT_NUM_TYPE_FILL_ZERO);
	PrtPutChar(0x0d);
}
#endif

#else
/**
********************************************************************************
* @brief    ����Ÿ ��½� start line�� ���� ����     
* @param    none
* @return   none
* @remark   saleUseHeadMsgFlag = head �޼��� ��� ���θ� ������  	 		\n
*			salePrtDayFlag   = ��¥ ���� ��� ���θ� ������
********************************************************************************
*/
void salePrtStartCondition(void)
{
}

/**
********************************************************************************
* @brief    ����Ÿ ��½� weight item�� ���� �Ǹ� ���� ���     
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
* @brief    ����Ÿ ��½� Fixed_value item�� ���� �Ǹ� ���� ���     
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtFixedItem(void)
{
}

/**
********************************************************************************
* @brief    ����Ÿ ��½� Mul_ti item�� ���� �Ǹ� ���� ���     
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtMultiItem(void)
{	
}

/**
********************************************************************************
* @brief    ����Ÿ ��½� Discount item�� ���� �Ǹ� ���� ���     
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtDiscountItem(void)
{
}

/**
********************************************************************************
* @brief    ����Ÿ ��½� weight item �� ���� void ���� ���     
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtWeightItemCancel(void)
{
}

/**
********************************************************************************
* @brief    ����Ÿ ��½� Fixed_price item �� ���� void ���� ���     
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtFixedItemCancel(void)
{
}

/**
********************************************************************************
* @brief    ����Ÿ ��½� Print number ���     
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtPluName(void)
{
}

/**
********************************************************************************
* @brief    ����Ÿ ��½� SUM_data ���     
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtSum(void)
{	
}

/**
********************************************************************************
* @brief    ����Ÿ ��½� Payment_Data ���     
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtPayment(void)
{
}

/**
********************************************************************************
* @brief    DLP-50 ���� ó���� ������ Protocol �Լ�      
* @param  none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtDlpStartCondition(INT8U printType)
{
}

/**
********************************************************************************
* @brief    DLP-50 ���� PLU name�� ������ Protocol �Լ�
* @param  none
* @return   none
* @remark   
********************************************************************************
*/
void salePrtDlpPluName(void)
{
}

/**
********************************************************************************
* @brief    DLP-50 ���� Data ������ Protocol �Լ�      
* @param  winNum : ������ �������� ����
* @return   none
* @remark   
********************************************************************************
*/
void salePrtDlp(INT8U winNum)
{
}
#endif

#endif
