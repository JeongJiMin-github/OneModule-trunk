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
#include "../UI/RelayApi.h"
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

#ifdef USE_SALE_HANGING
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
/** @brief bit 3, Gram unit 사용여부, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseGramUnitFlag;	//TODO : 접두어 sale 필요??
/** @brief bit 2, Ounce unit 사용여부, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseOzUnitFlag;	//TODO : 접두어 sale 필요??
/** @brief bit 1, Pound unit 사용여부, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseLbUnitFlag;	//TODO : 접두어 sale 필요??
/** @brief bit 0, Kilo gram unit 사용여부, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseKgUnitFlag;	//TODO : 접두어 sale 필요??

#ifdef USE_UNIT_TON_NEWTON_DAN_KN
/** @brief ton unit 사용여부, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseTonUnitFlag;
/** @brief short ton unit 사용여부, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseShortTonUnitFlag;
/** @brief long ton unit 사용여부, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseLongTonUnitFlag;
/** @brief newton unit 사용여부, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseNewtonUnitFlag;
/** @brief kN unit 사용여부, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseKNUnitFlag;
/** @brief daN unit 사용여부, 0: Don't Use 1: Use*/
static BOOLEAN scaleUsedaNUnitFlag;
#endif
/** @brief Tare clear 플래그, 0: save, 1: clear*/
static BOOLEAN saleTareClearFlag;
/** @brief 무게 Display에서 사용하는 Dot type, 0: Dot 1: Comma*/
static BOOLEAN weightDotTypeFlag;	//TODO : 접두어 sale 필요??
/** @brief Hold기능 count*/
static INT16U holdCount;	//Haning type INT16U / TODO : 접두어 sale 필요??
/** @brief */
static INT16U holdDisplayFlash;	//TODO : 접두어 sale 필요??
/** @brief Auto Hold & Manual Hold Check flag 1:Hold ok*/
static INT8U AMHoldOkFlag;	//TODO : 접두어 sale 필요??
/** @brief 무게 단위*/
const INT8U code saleWeightUnit[10][4] = { "kg", "lb", "oz", "g ", "T", "ST", "LT", "N", "daN", "KN"};

// for CASTON -->
/** @brief Auto Hold & Manual Hold Check flag 0: Auto Hold, 1: Manual Hold*/
static BOOLEAN AMHoldSelectFlag;	//TODO : 접두어 sale 필요??
/** @brief Auto Hold restart flag 1:restart*/
static BOOLEAN ReHoldFlag;	//TODO : 접두어 sale 필요??
/** @brief 'HDOFF' Message display flag*/
static BOOLEAN HoldClearMessageFlag;	//TODO : 접두어 sale 필요??
/** @brief 현재 사용하는 Hold type, 0:NORMAL, 1:PEAK, 2 : AVERAGE*/
static INT8U saleHoldTypeFlag;
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
/** @brief Hold기능 사용 무게, sum buffer*/
INT32S holdWeightSumBuf;
//<-- for CASTON

/** @brief Tare key is pressed flag*/
static BOOLEAN saleTareKeyPressed;
static BOOLEAN saleTareKeyPressedFlag;
/** @brief Zero key is pressed flag*/
static BOOLEAN saleZeroKeyPressed;

/** @brief Last digit invalid function enable  flag*/
static BOOLEAN saleLastDigitInvalidEnable;

/** @brief Scale Initial zero error state flag*/
static BOOLEAN zeroErrFlag;
#ifdef	USE_RELAY
static INT16U saleHighWeightThres;
static INT16U saleLowWeightThres;
#endif

static INT32S saleOldHoldData;

#ifdef USE_KEY_LOCK
/** @brief 이전에 눌려진 raw Data */
static INT8U  keyDrvValidLockData;//data for high speed in interrupt
#endif
#if MODEL_NUM == MODEL_OMV3_TM_LCD
/** @brief Country Select*/
static INT8U  CountrySelect;
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


/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
/*
static INT8U saleNumKey(KEY_TYPE key);
static INT8U saleDirectPluCall(KEY_TYPE key);
*/
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
static void saleSumKey(void);

static void saleUnitChangeKey(void);

static void 	saleDispNumWithDecPoint(INT8U winNum, INT8U position, INT8U type, INT32S num, BOOLEAN thousandPt);

static void 	saleInitZeroErrFlagSet(BOOLEAN flag);

/** @brief Key에 따른 Sale Function(key input type) 개수 */
#define SALE_KEY_MAX_FUNC_NUM	3
#define SALE_COUNT_ENABLE_CAPA	1200
#define SALE_COUNT_LOW_CAPA		1
#define SALE_PERCENT_LOW_CAPA		1800

/** @brief Key에 따른 Sale Function(void type) 개수 */
#define SALE_KEY_MAX_VOID_FUNC_NUM	6
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
	{KEY_C, 				saleClearKey},
	{KEY_SUM,				saleSumKey},
	//{KEY_STAR,				saleUnitChangeKey}
	{KEY_UNIT_CHANGE,		saleUnitChangeKey}
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
    #ifdef USE_KEY_LOCK
    ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_KEY_LOCK,0);
    #endif
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
#if MODEL_NUM == MODEL_OMV3_TM_LCD
	VERSION version;

	ParamReadData(PARAM_VERSION_AREA_NUM, 0, (INT8U *)&version, sizeof(version));	
	CountrySelect = version.countryNum;
#endif
	
	AMHoldOkFlag = 0;	
    SaleData.weightUnit = AdData.weightUnit;
	SaleData.weightDecPos = AdData.weightDecPos;	
	saleHoldTypeFlag = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_HOLD_TYPE);	

	
//  C4-2
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

	scaleUseGramUnitFlag = (readSetDataInSale & 0x08) >> 3;
	scaleUseOzUnitFlag = (readSetDataInSale & 0x04) >> 2;
	#ifdef COUNTRY_USA
    scaleUseLbUnitFlag = (readSetDataInSale & 0x02) >> 1;
	#endif
	scaleUseKgUnitFlag = (readSetDataInSale & 0x01) >> 0;	
#if MODEL_NUM == MODEL_OMV3_TM_LCD
	switch (CountrySelect)
	{
		case KOREA:	  
   			scaleUseLbUnitFlag = 0;	
			tempInt8u = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_EXPANDED_WEIGHT_UNIT);
			while (1)
			{
				if(AdSetWeightUnitFlag(UNIT_KG))
				{
					break;  
				}
			}
			break;
		default:
		  	scaleUseLbUnitFlag = (readSetDataInSale & 0x02) >> 1;
		  	tempInt8u = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_EXPANDED_WEIGHT_UNIT);		
			scaleUseTonUnitFlag = (tempInt8u & EXPAND_UNIT_MASK_TON);
			scaleUseShortTonUnitFlag = (tempInt8u & EXPAND_UNIT_MASK_SHORT_TON) >> 1;
			scaleUseLongTonUnitFlag = (tempInt8u & EXPAND_UNIT_MASK_LONG_TON) >> 2;
			scaleUseNewtonUnitFlag = (tempInt8u & EXPAND_UNIT_MASK_NEWTON) >> 3;
			scaleUseKNUnitFlag = (tempInt8u & EXPAND_UNIT_MASK_KN) >> 4;
			scaleUsedaNUnitFlag = (tempInt8u & EXPAND_UNIT_MASK_DAN) >> 5;
			break;
	}	
#endif	

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

	SaleData.saleState = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS);

	saleTareClearFlag = scaleTareClearFlag;

	SaleData.weight = 0;

	SaleData.holdWeight = 0;
	holdCount = 0;
	holdDisplayFlash = 0;
	
	sumWeight = 0;
	sumCount = 0;
	sumSaveWeight=0;
	sumDisplayFlash=0;

	saleTareKeyPressed = 0;
	saleZeroKeyPressed = 0;
    #ifdef	USE_RELAY
    RelayInit(); //Relay port 초기화
    // TM 상한 Relay (Relay_1)
    saleHighWeightThres = ParamReadWord(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MAX_WEIGHT);
    if(CalParam.weightDecPos == 1) 
    {
        saleHighWeightThres = saleHighWeightThres*10;    
    }
    // TM  하한 Relay (Relay_2)
    saleLowWeightThres = ParamReadWord(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MIN_WEIGHT);
    if(CalParam.weightDecPos == 1) 
    {
        saleLowWeightThres = saleLowWeightThres*10;    
    }    
    #endif
    #ifdef USE_KEY_LOCK
    keyDrvValidLockData = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_KEY_LOCK);
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
	INT8U tempInt8u;
    static INT8U tempHoldCnt;
	KEY_TYPE Key;
	KEY_TYPE minKeyNum;

    SaleData.weight = AdData.weightData;
	SaleData.weightUnit = AdData.weightUnit;
	SaleData.weightDecPos = AdData.weightDecPos;	

	if(saleLastDigitInvalidEnable) SaleData.lastDigitIsInvalid = AdData.lastDigitIsInvalid;
	else SaleData.lastDigitIsInvalid = FALSE;

	retValue = SCALE_MODE_SALE;
	
	//tempInt8u = ParamReadByte(PARAM_PRT_AREA_NUM, PRT_PARAM_OFFSET_1ST);
	if (KeyCheck())
	{
#ifdef USE_KEY_LOCK
        if(!keyDrvValidLockData)
        {
            Key = KeyGetFuncCode(KEY_MODE_NORMAL);
        }
#else
        Key = KeyGetFuncCode(KEY_MODE_NORMAL);
#endif
		#if MODEL_NUM == MODEL_OMV3_TM_LCD
		if(CountrySelect == KOREA)
		{
		  if (Key == KEY_UNIT_CHANGE)
			{
				Key = KEY_NOT_PRESSED;  
			}
		}
		#endif
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

	if (AdData.stableFlag)
	{
//		#ifdef USE_KEY_LOCK
//        if ((saleTareKeyPressed) && !keyDrvValidLockData)
//		{
//			saleWeightTareProc();
//			saleTareKeyPressed = 0;
//			TimerSetDelay100ms(TIMER_DELAY_100MS_WAIT_STABLE, 0);//stop timer
//		}
//        if ((saleZeroKeyPressed) && !keyDrvValidLockData)
//		{
//			saleZeroKeyProc();
//			saleZeroKeyPressed = 0;
//			TimerSetDelay100ms(TIMER_DELAY_100MS_WAIT_STABLE, 0);//stop timer
//		}
//        #else
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
//        #endif
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
#endif

/**
********************************************************************************
* @brief    COMMAND에 따라 키동작을 수행하는 함수.
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
#if MODEL_NUM == MODEL_OMV3_TM_LCD
void SaleUnitChangeExtKey(void)
{
  	if(CountrySelect == KOREA)
	{
		return;  
	}
	saleUnitChangeKey();
}
#endif

void SaleHoldExtKey(void)
{
	saleHoldKey();
}

void saleInitZeroErrFlagSet(BOOLEAN flag)
{
	if(flag == ON)
		zeroErrFlag = 1;
	else
		zeroErrFlag = 0;
}

INT8U GetSaleInitZeroErrFlagState(void)
{
	return zeroErrFlag;
}

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

	DispSetIndicator(INDICATOR_KG, OFF);
	DispSetIndicator(INDICATOR_LB, OFF);

	DispSetIndicator(INDICATOR_TON, OFF);
	DispSetIndicator(INDICATOR_SHORT_TON, OFF);
	DispSetIndicator(INDICATOR_LONG_TON, OFF);
	DispSetIndicator(INDICATOR_NEWTON, OFF);
	DispSetIndicator(INDICATOR_KN, OFF);
	DispSetIndicator(INDICATOR_DAN, OFF);


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
#ifdef USE_UNIT_TON_NEWTON_DAN_KN
			case UNIT_TON:
				DispSetIndicator(INDICATOR_TON, ON);
				break;                
			case UNIT_SHORT_TON:
				DispSetIndicator(INDICATOR_SHORT_TON, ON);
				break;                
			case UNIT_LONG_TON:
				DispSetIndicator(INDICATOR_LONG_TON, ON);
				break;                
			case UNIT_NEWTON:
				DispSetIndicator(INDICATOR_NEWTON, ON);
				break;                
			case UNIT_KN:
				DispSetIndicator(INDICATOR_KN, ON);
				break;                
			case UNIT_DAN:
				DispSetIndicator(INDICATOR_DAN, ON);
				break;          
#endif                 
			default:
				break;
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
	static INT8U Aholdflag;
	static INT8U pholdflag;
    static INT8U autoHoldonFlag;
    static INT16U cmpareCnt;
	
	if (AdData.grossZeroFlag && saleTareClearFlag)
	{
		returnData = AdSetTare();
	}

	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:
			if(HoldClearMessageFlag && AMHoldOkFlag)
			{
				DispSetConstStrMsg(WIN_WEIGHT, "HDOFF");
				DispSetRunMsgPage(10, 1);
				SaleData.holdWeight = 0;
				HoldClearMessageFlag=0;
				AMHoldOkFlag=0;
				holdCount=0;
				ReHoldFlag=0;
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
						
						SaleData.sumSaveWeight = sumSaveWeight;
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
				SaleData.sumSaveWeight=0;
				DispSetConstStrMsg(WIN_WEIGHT, "CLEAR");
				DispSetRunMsgPage(10, 1);
			}
			else
			{
				sumCount = 2 ;
			}		
			break;		

		case SALE_STATE_HOLD:
			if(saleHoldTypeFlag == SALE_HOLD_TYPE_AVERAGE)
			{
				if (SaleData.weight > 0 && holdCount == 0)
				{	
					DispSetConstStrMsg(WIN_WEIGHT, " HOLD");
					DispSetRunMsgPage(10, 1);
					HoldClearMessageFlag=1;
					holdWeightSumBuf = 0;
					SaleData.holdWeight =0;
					AMHoldOkFlag=1;
				}
				if( (SaleData.weight <= 0) || (AdData.overloadFlag == 1) )
				{
					SaleData.saleState = SALE_STATE_NORMAL;
					holdWeightSumBuf = 0;
					SaleData.holdWeight = 0;
				}		
				if (holdCount <= 150)
				{
					holdCount++;
					holdWeightSumBuf = holdWeightSumBuf + SaleData.weight;
					if(holdCount< 50)
					{	
						DispSetStr(WIN_WEIGHT, "- - -");	
					}
					if((holdCount >= 50) && (holdCount< 100))
					{	
						DispSetStr(WIN_WEIGHT, " - - ");	
					}
					if((holdCount >= 100) && (holdCount< 150))
					{	
						DispSetStr(WIN_WEIGHT, "- - -");	
					}
					if (holdCount == 150)
					{	
						SaleData.holdWeight	= holdWeightSumBuf / 150;
						SaleData.holdWeight = AdGetRoundedWeight(SaleData.holdWeight);//round off
						saleDisplayWeight(SaleData.holdWeight);
					}
				}
			}
			else if(saleHoldTypeFlag == SALE_HOLD_TYPE_PEAK)
			{
				if (SaleData.weight >= 0 && holdCount == 0)
				{	
					DispSetConstStrMsg(WIN_WEIGHT, "PHOLD");
					DispSetRunMsgPage(10, 1);
					holdCount=1;
					HoldClearMessageFlag=1;
				}
				else if (holdCount)
				{
					if(SaleData.holdWeight < SaleData.weight) 	
					{
						SaleData.holdWeight =  SaleData.weight; 
						pholdflag = 1;
					}
					if(pholdflag)
					{
						if(AdData.overloadFlag == 1)	
						{
							SaleData.saleState = SALE_STATE_NORMAL;
							holdCount=0;
						}
						else if(SaleData.weight < 0)
						{
							SaleData.saleState = SALE_STATE_NORMAL;
							holdCount=0;
						}
					}
				}
			}			
			else if(saleHoldTypeFlag == SALE_HOLD_TYPE_NORMAL)
			{
				if ((SaleData.weight > 0) && !AMHoldOkFlag)   //manual HOLD
				{	
                    DispSetConstStrMsg(WIN_WEIGHT, "MHOLD");
                    DispSetRunMsgPage(10, 1);
					AMHoldSelectFlag = 1;
					HoldClearMessageFlag=1;
					AMHoldOkFlag=1;
					SaleData.holdWeight =  SaleData.weight;
				}
                else if ((SaleData.weight == 0) && (!AMHoldOkFlag))   //Auto HOLD
				{	
                    DispSetConstStrMsg(WIN_WEIGHT, "AHOLD");
                    DispSetRunMsgPage(10, 1);
					AMHoldSelectFlag = 0;
					HoldClearMessageFlag=1;
					AMHoldOkFlag = 2;
				}

                if ((AdData.overloadFlag == 1 || SaleData.weight <= 0)  && (AMHoldOkFlag ==1))	
				{
					SaleData.saleState = SALE_STATE_NORMAL;
					holdCount=0;
					DispSetConstStrMsg(WIN_WEIGHT, "HDOFF");
					DispSetRunMsgPage(10, 1);
					SaleData.holdWeight = 0;
					HoldClearMessageFlag=0;
                    AMHoldOkFlag=0;
				}
				if(!AMHoldSelectFlag && (AMHoldOkFlag ==2))
				{
					if (AdData.weightData <= 0)
					{
						holdWeightSumBuf = 0; 
						holdCount=0;
						SaleData.holdWeight =  AdData.weightData;	
						saleDisplayWeight(SaleData.holdWeight);
					}
                    else if(AbsInt32s(AdData.weightData) <= (CalParam.divFactor*20))    //20 Digit이하에서는 동작 하지 않음.
                    {
                        holdWeightSumBuf = 0; 
                        SaleData.holdWeight = 0;
                        holdCount=0;
                        saleDisplayWeight(0);
                    }
                    else
                    {
                        if(!autoHoldonFlag)
                        {
                            if(AbsInt32s(saleOldHoldData - AdData.weightData) < (CalParam.divFactor*4))
                            { 		
                                holdCount++;
                                holdWeightSumBuf += AdData.weightData; 
                                if(holdCount == 1000)
                                {
                                    SaleData.holdWeight	= holdWeightSumBuf / 1000;
                                    saleDisplayWeight(SaleData.holdWeight);
                                    autoHoldonFlag = 1;
                                } 
                                if(holdCount< 500)	DispSetStr(WIN_WEIGHT, "- - -");	
                                if((holdCount >= 500) && (holdCount< 1000))	DispSetStr(WIN_WEIGHT, " - - ");	
                            }				
                        }
                        else if(AbsInt32s(SaleData.holdWeight - AdData.weightData) > (CalParam.divFactor*20))
                        {
                            autoHoldonFlag = 0;
                            holdCount = 0;
                            SaleData.holdWeight = 0;
                            holdWeightSumBuf = 0;           	
                        }
                    }
                    if(cmpareCnt == 600)
                    {
                        //saleOldHoldData = AdData.rawSetTareData;
                         saleOldHoldData = AdData.weightData;
                        cmpareCnt = 0;
                    }
                    else    cmpareCnt++;
				} 	
			}
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
void saleDisplayFunc(void)
{
#ifdef	USE_RELAY
    static INT16U buzzerLowCnt;
    static INT16U buzzerHighCnt;
#endif
    
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

		case SALE_STATE_CANCEL:
			SaleData.saleState = SALE_STATE_NORMAL;
			break;
		
		case SALE_STATE_MODE_SUM:
			
			sumDisplayFlash++;
			if (sumDisplayFlash == 400)
			{
					SaleData.saleState = SALE_STATE_NORMAL;
			}
			if (sumDisplayFlash % 50 < 25)
			{
					saleDisplayWeight(sumSaveWeight);
			}
			else
			{
					DispSetStr(WIN_WEIGHT, "	  ");
			}				
			break;

		case SALE_STATE_HOLD:
			if(saleHoldTypeFlag == SALE_HOLD_TYPE_AVERAGE)			
			{
				DispSetIndicator(INDICATOR_HOLD, ON);
			}

			else if(saleHoldTypeFlag == SALE_HOLD_TYPE_PEAK)
			{
				saleDisplayWeight(SaleData.holdWeight);
//                if(SaleData.weight == 0)
//                {
//                    SaleData.holdWeight = SaleData.weight;
//                }
				DispSetIndicator(INDICATOR_HOLD, ON);					
			}
			else if(saleHoldTypeFlag == SALE_HOLD_TYPE_NORMAL)
			{
				if(AMHoldSelectFlag)
				{
					saleDisplayWeight(SaleData.holdWeight);
				}
				DispSetIndicator(INDICATOR_HOLD, ON);					
			}
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
#ifdef	USE_RELAY
// TM  상한 Relay (Relay_1)
if(saleHighWeightThres)
{
	if(SaleData.weight >= saleHighWeightThres)
	{
        RelaySet(RELAY_HIGH, ON);
		DispSetIndicator(INDICATOR_HIGH, ON);
        if(buzzerHighCnt == 0x100)
        {
            BuzzerSetOnTimes(1);
            buzzerHighCnt = 0;
        }
        buzzerHighCnt++;
	}
	else	
	{
        RelaySet(RELAY_HIGH, OFF);
		DispSetIndicator(INDICATOR_HIGH, OFF);
	}
}
// TM  하한 Relay (Relay_2)
if(saleLowWeightThres)
{
	if((SaleData.weight!=0)&&(SaleData.weight >= saleLowWeightThres))
	{
        RelaySet(RELAY_LOW, ON);
		DispSetIndicator(INDICATOR_LOW, ON);
        if(buzzerLowCnt == 0x200)
        {
            BuzzerSetOnTimes(1);
            buzzerLowCnt = 0;
        }
        buzzerLowCnt++;
	}
	else	
	{
        RelaySet(RELAY_LOW, OFF);
		DispSetIndicator(INDICATOR_LOW, OFF);
	}
}
if(AdData.overloadFlag)
{
    RelaySet(RELAY_OK, ON);
}
else
{
    RelaySet(RELAY_OK, OFF);
}
#endif
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
		saleInitZeroErrFlagSet(OFF);
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
			saleInitZeroErrFlagSet(ON);
			break;			

		default:
			BuzzerSetOnTimes(2);
			saleInitZeroErrFlagSet(OFF);
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
		saleInitZeroErrFlagSet(OFF);
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

	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:			
			saleZeroKeyPressed = 0;
			saleTareKeyPressed = 1;
			TimerSetDelay100ms(TIMER_DELAY_100MS_WAIT_STABLE, 50);
			saleInitZeroErrFlagSet(ON);
			break;

		default:
			BuzzerSetOnTimes(2);
			saleInitZeroErrFlagSet(OFF);
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
	if(SaleData.weight < 0)
	{
		BuzzerSetOnTimes(2);
	}
	else
	{
        if(saleHoldTypeFlag == SALE_HOLD_TYPE_AVERAGE)
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
                    }
                    else
                    {
                        BuzzerSetOnTimes(2);
                    }
                    break;
                
                default: 
                    if (SaleData.weight > 0)
                    {
                        holdCount = 0;
                        SaleData.holdWeight = 0;
                        holdDisplayFlash = 0;
                        SaleData.saleState = SALE_STATE_NORMAL;
                    }
                    BuzzerSetOnTimes(2);
                    break;
            }
    
        }
        else if(saleHoldTypeFlag == SALE_HOLD_TYPE_PEAK)
        {
            switch (SaleData.saleState)
            {
                case SALE_STATE_NORMAL:
                    if (SaleData.weight >= 0)
                    {
                        holdCount = 0;
                        SaleData.holdWeight = 0;
                        holdDisplayFlash = 0;
                        SaleData.saleState = SALE_STATE_HOLD;
                    }
                    else
                    {
                        BuzzerSetOnTimes(2);
                    }
                    break;
                
                default: 
                    if (SaleData.weight >= 0)
                    {
                        holdCount = 0;
                        SaleData.holdWeight = 0;
                        holdDisplayFlash = 0;
                        SaleData.saleState = SALE_STATE_NORMAL;
                        HoldClearMessageFlag=1;
                        AMHoldOkFlag=1;
                    }
                    else    BuzzerSetOnTimes(2);
                    break;
            }
        }
        else if(saleHoldTypeFlag == SALE_HOLD_TYPE_NORMAL)
        {
            switch (SaleData.saleState)
            {
                case SALE_STATE_NORMAL:
                    if (SaleData.weight >= 0)   //Auto HOLD 및 Manual HOLD 시작
                    {
                        holdCount = 0;
                        SaleData.holdWeight = 0;
                        holdDisplayFlash = 0;
                        SaleData.saleState = SALE_STATE_HOLD;
                    
                    }
                    else
                    {
                        BuzzerSetOnTimes(2);
                    }
                    break;	
                default: 
                    if (SaleData.weight >= 0)
                    {
                        holdCount = 0;
                        SaleData.holdWeight = 0;
                        holdDisplayFlash = 0;
                        SaleData.saleState = SALE_STATE_NORMAL;
                    }
                    else
                    {
                        BuzzerSetOnTimes(2);
                    }
                    break;
            }
        }
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
		
	switch (SaleData.saleState)	 	//clear
	{
		case SALE_STATE_NORMAL:			
			sumCount=0;
			SaleData.saleState = SALE_STATE_CANCEL;
			break;

		default:			
			BuzzerSetOnTimes(2);
			break;
	}
}

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
	switch (SaleData.saleState)
	{	
		case SALE_STATE_NORMAL:
				sumDisplayFlash = 0;
				sumCount=0;
				sumWeight=0;
				SaleData.saleState = SALE_STATE_MODE_SUM;
				break;
			
		default: 	
			break;
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
#if MODEL_NUM == MODEL_OMV3_TM_LCD

void saleUnitChangeKey(void)
{
  	VERSION version;
  	INT8U weightUnitNumTableForChange[8] = {UNIT_KG, UNIT_LB, UNIT_TON, UNIT_SHORT_TON, UNIT_LONG_TON, UNIT_NEWTON, UNIT_KN, UNIT_DAN};
	INT8U unitIndex[8];	 // { kg, lb, ton, short ton, long ton, N, kN, daN}
	INT8U step;	

	unitIndex[7] = scaleUsedaNUnitFlag;
	unitIndex[6] = scaleUseKNUnitFlag; 
	unitIndex[5] = scaleUseNewtonUnitFlag;
	unitIndex[4] = scaleUseLongTonUnitFlag;
	unitIndex[3] = scaleUseShortTonUnitFlag;
	unitIndex[2] = scaleUseTonUnitFlag; 
	unitIndex[1] = scaleUseLbUnitFlag;
	unitIndex[0] = scaleUseKgUnitFlag;
	
	if ((SaleData.saleState == SALE_STATE_NORMAL || SaleData.saleState == SALE_STATE_UNITCHANGE) && AdData.tareWeight == 0)
	{	
		for (step = 0; step < 8; step++)
		{
			if (SaleData.weightUnit == weightUnitNumTableForChange[step]) break;
		}
		
		if (step == 8) return;
		  
		while (1)
		{
			step++;
			step %= 8;
			if (unitIndex[step] == 1)
			{
				if (AdSetWeightUnitFlag(weightUnitNumTableForChange[step])) 
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
/*
#elif defined (USE_UNIT_KG_ONLY)
INT8U weightUnitNumTableForChange[8] = {UNIT_KG, UNIT_LB, UNIT_TON, UNIT_SHORT_TON, UNIT_LONG_TON, UNIT_NEWTON, UNIT_KN, UNIT_DAN};
void saleUnitChangeKey(void)
{
    ;
}
*/
#else
void saleUnitChangeKey(void)
{
	INT8U unitIndex[4];	 // { kg, lb, oz, g}
	INT8U step;
	
	unitIndex[3] = scaleUseGramUnitFlag;
	unitIndex[2] = scaleUseOzUnitFlag; 
	unitIndex[1] = scaleUseLbUnitFlag;
	unitIndex[0] = scaleUseKgUnitFlag;

	if ((SaleData.saleState == SALE_STATE_NORMAL || SaleData.saleState == SALE_STATE_UNITCHANGE) && AdData.tareWeight == 0)
	{	
		step = SaleData.weightUnit;	
		while (1)
		{
			step++;
			step %= 2;

			if (unitIndex[step] == 1)
			{
				if (AdSetWeightUnitFlag(step)) 
				{
					#ifdef COUNTRY_USA
					if(step == 1)
						DispSetConstStrMsg(WIN_WEIGHT, "U- lb");
					else
						DispSetConstStrMsg(WIN_WEIGHT, "U- kg");	
					DispSetRunMsgPage(20, 1);
					#endif
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
#endif
