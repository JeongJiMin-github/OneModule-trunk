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
/** @brief Sale ���� Data struct */
SALE_DATA SaleData;

/** @brief C4-2 setting data */
/** @brief bit 2, Tare ���� ��뿩��, 0: Save 1: Clear*/
static BOOLEAN scaleTareClearFlag;	//TODO : ���ξ� sale �ʿ�??


/** @brief C4-3 setting data */
/** @brief bit 3, Gram unit ��뿩��, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseGramUnitFlag;	//TODO : ���ξ� sale �ʿ�??
/** @brief bit 2, Ounce unit ��뿩��, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseOzUnitFlag;	//TODO : ���ξ� sale �ʿ�??
/** @brief bit 1, Pound unit ��뿩��, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseLbUnitFlag;	//TODO : ���ξ� sale �ʿ�??
/** @brief bit 0, Kilo gram unit ��뿩��, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseKgUnitFlag;	//TODO : ���ξ� sale �ʿ�??

#ifdef USE_UNIT_TON_NEWTON_DAN_KN
/** @brief ton unit ��뿩��, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseTonUnitFlag;
/** @brief short ton unit ��뿩��, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseShortTonUnitFlag;
/** @brief long ton unit ��뿩��, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseLongTonUnitFlag;
/** @brief newton unit ��뿩��, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseNewtonUnitFlag;
/** @brief kN unit ��뿩��, 0: Don't Use 1: Use*/
static BOOLEAN scaleUseKNUnitFlag;
/** @brief daN unit ��뿩��, 0: Don't Use 1: Use*/
static BOOLEAN scaleUsedaNUnitFlag;
#endif
/** @brief Tare clear �÷���, 0: save, 1: clear*/
static BOOLEAN saleTareClearFlag;
/** @brief ���� Display���� ����ϴ� Dot type, 0: Dot 1: Comma*/
static BOOLEAN weightDotTypeFlag;	//TODO : ���ξ� sale �ʿ�??
/** @brief Hold��� count*/
static INT16U holdCount;	//Haning type INT16U / TODO : ���ξ� sale �ʿ�??
/** @brief */
static INT16U holdDisplayFlash;	//TODO : ���ξ� sale �ʿ�??
/** @brief Auto Hold & Manual Hold Check flag 1:Hold ok*/
static INT8U AMHoldOkFlag;	//TODO : ���ξ� sale �ʿ�??
/** @brief ���� ����*/
const INT8U code saleWeightUnit[10][4] = { "kg", "lb", "oz", "g ", "T", "ST", "LT", "N", "daN", "KN"};

// for CASTON -->
/** @brief Auto Hold & Manual Hold Check flag 0: Auto Hold, 1: Manual Hold*/
static BOOLEAN AMHoldSelectFlag;	//TODO : ���ξ� sale �ʿ�??
/** @brief Auto Hold restart flag 1:restart*/
static BOOLEAN ReHoldFlag;	//TODO : ���ξ� sale �ʿ�??
/** @brief 'HDOFF' Message display flag*/
static BOOLEAN HoldClearMessageFlag;	//TODO : ���ξ� sale �ʿ�??
/** @brief ���� ����ϴ� Hold type, 0:NORMAL, 1:PEAK, 2 : AVERAGE*/
static INT8U saleHoldTypeFlag;
/** @brief Hold��� count*/
//INT16U holdDispCount;	//TODO : ���ξ� sale �ʿ�??	//not use
/** @brief SUM��� count*/
INT16U sumCount;	//TODO : ���ξ� sale �ʿ�??
/** @brief Sum��� ��� ����, ��հ�*/
INT32S sumWeight;	//TODO : ���ξ� sale �ʿ�??
/** @brief Sum��� ��� ����, ��հ�*/
INT32S sumSaveWeight;	//TODO : ���ξ� sale �ʿ�??
/** @brief */
INT16U sumDisplayFlash;	//TODO : ���ξ� sale �ʿ�??
/** @brief Hold��� ��� ����, sum buffer*/
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
/** @brief ������ ������ raw Data */
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

/** @brief Key�� ���� Sale Function(key input type) ���� */
#define SALE_KEY_MAX_FUNC_NUM	3
#define SALE_COUNT_ENABLE_CAPA	1200
#define SALE_COUNT_LOW_CAPA		1
#define SALE_PERCENT_LOW_CAPA		1800

/** @brief Key�� ���� Sale Function(void type) ���� */
#define SALE_KEY_MAX_VOID_FUNC_NUM	6
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
* @brief    Sale Mode Application Parameter�� default ������ �ʱ�ȭ\n
* @param    type : defualt type
* @return   none
* @remark   Parameter ������ �ʱⰪ�� �ִ´�.\n
*           Cal Mode�� Program ��忡�� ��ü �ʱ�ȭ�� �� �����Ѵ�.
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
* @brief    sale�� ���õ� ��� ������ �ʱ�ȭ �ϴ� �Լ�       
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
    RelayInit(); //Relay port �ʱ�ȭ
    // TM ���� Relay (Relay_1)
    saleHighWeightThres = ParamReadWord(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_COMPARE_MAX_WEIGHT);
    if(CalParam.weightDecPos == 1) 
    {
        saleHighWeightThres = saleHighWeightThres*10;    
    }
    // TM  ���� Relay (Relay_2)
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
#endif

/**
********************************************************************************
* @brief    COMMAND�� ���� Ű������ �����ϴ� �Լ�.
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
* @brief    Sale Mode Indicator �Լ� 
* @param    none
* @return   none
* @remark   ���� Flag�� ���� �����¸� indicatation �Ѵ�.  \n
*           ������ indicator ������ UiDefine.h�� �����Ѵ�. \n
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
                    else if(AbsInt32s(AdData.weightData) <= (CalParam.divFactor*20))    //20 Digit���Ͽ����� ���� ���� ����.
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
* @brief    ���� Sale mode���� Weight data�� Display �ϴ� �Լ�   
* @param    weight data
* @return   none
* @remark   weight data�� Display �Ѵ�.
********************************************************************************
*/
void saleDisplayWeight(INT32S dispWeight)
{
	saleDispNumWithDecPoint(WIN_WEIGHT, SaleData.weightDecPos, weightDotTypeFlag, dispWeight, 0);
	if (SaleData.lastDigitIsInvalid)
	{
		if (SaleData.weightDecPos == 1) { ////2kg�� ��� 0.5g���� 1g���� �ö󰡸� decimal point �����ؾ� �ϹǷ� �ٽ� Display�Լ� �ҷ���
			saleDispNumWithDecPoint(WIN_WEIGHT,  0, weightDotTypeFlag, dispWeight, 0);
		}
		DispSetChar(WIN_WEIGHT, DispGetWinSize(WIN_WEIGHT) - 1, ' ');
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
// TM  ���� Relay (Relay_1)
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
// TM  ���� Relay (Relay_2)
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
		saleInitZeroErrFlagSet(OFF);
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
* @brief    Tare Key �Լ����� Stable�� ��ٸ��� Tare����� �����ϴ� �Լ� 
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
* @brief    Hold key ������ �����Ѵ�. 
* @param    type : defualt type
* @return   none
* @remark   ���԰� "0"�϶��� ������ �ð��Ŀ� "Hold"�� ������ \n
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
                    if (SaleData.weight >= 0)   //Auto HOLD �� Manual HOLD ����
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
* @brief    Clear Key ������ �����Ѵ�.    
* @param    none
* @return   none
* @remark   �� ���º� Clear key ���  ����
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
* @brief    unit ��ȯ Key �Լ�    
* @param    none
* @return   none
* @remark   kg, lb, oz, g ������ ������ ����� 					\n
*           ���� ���￡�� ��밡���� ������ C-4 table ���� 		\n
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
#endif
