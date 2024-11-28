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

#ifdef USE_SALE_EBPT
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

/** @brief C4-4 setting data */

/** @brief C4-5 setting data */
/** @brief bit 1 & bit 0, ��� ������ ����, 00 : Don't Use, 01: Use DEP, 10: Use DLP, 11: reserve*/
static INT8U scaleUsePrt;	//TODO : ���ξ� sale �ʿ�??

/** @brief ����Ʈ ���� ���� �÷���, 0: Don't use, 1: Use*/
static BOOLEAN salePrtEnableFlag;
/** @brief Tare clear �÷���, 0: save, 1: clear*/
static BOOLEAN saleTareClearFlag;
/** @brief ���� Display���� ����ϴ� Dot type, 0: Dot 1: Comma*/
static BOOLEAN weightDotTypeFlag;	//TODO : ���ξ� sale �ʿ�??
/** @brief ���� Battery ���а�*/
static INT8U saleBatteryCapacity;
/** @brief Battery üũ �Լ� ������ ���� �÷���*/
static BOOLEAN saleBatteryCheckFlag;

/** @brief �����ð� ���� state delay�ϴ� ���� state*/
static INT8U saleNextState;
/** @brief �����ð� ���Ŀ� ���� data����� �ٽ� ���� �ϴ� flag*/
static BOOLEAN saleUnitPriceRenewalFlag = ON;

/** @brief Tare key is pressed flag*/
static BOOLEAN saleTareKeyPressed;
/** @brief Zero key is pressed flag*/
static BOOLEAN saleZeroKeyPressed;

/** @brief Last digit invalid function enable  flag*/
static BOOLEAN saleLastDigitInvalidEnable;

#ifdef	USE_DECIMAL_POINT	
/** @brief bit 0, Decimal Point ��������, 0; No Pressed  1: Pressed*/
BOOLEAN SaleDecimalPointKeyOnOff; //csh 20120730
/** @brief bit 0, �Է��� ������ġ  */
INT8S SaleNumberPositonFlag; //csh 20120730
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
static INT8U saleNumKey(KEY_TYPE key);

static void saleSetIndicator(void);
static void saleDisplayWeight(INT32S dispWeight);
static void saleDisplayFunc(void);
static void saleWeightFunc(void);
static void saleZeroKeyProc(void);
static void saleZeroKey(void);
#ifdef	USE_DECIMAL_POINT	
static void saleDecimalPointKey(void); //csh 20120730
#endif

static void saleWeightTareProc(void);
static void saleTareKey(void);
static void saleClearKey(void);

static void saleBattCheckKey(void);
static void saleDoubleZeroKey(void);
static void saleBackLightKey(void);
static void salePrintEnableKey(void);

static void 	saleDispNumWithDecPoint(INT8U winNum, INT8U position, INT8U type, INT32S num, BOOLEAN thousandPt);

/** @brief Key�� ���� Sale Function(key input type) ���� */
#define SALE_KEY_MAX_FUNC_NUM	2
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
//	{KEY_PLU_1, 	KEY_PLU_24, 	saleDirectPluCall},
};

//
/** @brief Key�� ���� Sale Function(void type) ���� */
#ifndef	USE_DECIMAL_POINT	
#define SALE_KEY_MAX_VOID_FUNC_NUM	7
#else
#define SALE_KEY_MAX_VOID_FUNC_NUM	8 //csh 20120730
#endif

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
	{KEY_C, 				saleClearKey},
	{KEY_BAT_CHECK, 		saleBattCheckKey},
	{KEY_NUM_00,			saleDoubleZeroKey},
#ifdef	USE_DECIMAL_POINT	
	{KEY_POINT, 		saleDecimalPointKey}, //csh20120730
#endif
	{KEY_BACK_LIGHT, 		saleBackLightKey},
	{KEY_PRINT_ENABLE, 		salePrintEnableKey},
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

		default: 		// OIML
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST, 0x38);
			ParamWriteByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND, 0x31);
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
* @brief    sale�� ���õ� ��� ������ �ʱ�ȭ �ϴ� �Լ�       
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void SaleModeClearAllData(void)
{
	/** @brief PLU Data & Report Data Clear*/
	//PluClearAllPlu();

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

#ifdef	USE_DECIMAL_POINT	
	SaleDecimalPointKeyOnOff = OFF; //csh 20120730
	SaleNumberPositonFlag = 0;	//csh 20120730
#endif
	SaleData.weightUnit = AdData.weightUnit;
	SaleData.weightDecPos = AdData.weightDecPos;	

//  C4-2
	readSetDataInSale = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_1ST); 
	scaleTareClearFlag = (readSetDataInSale & 0x04) >> 2;
	
// 	C4-3
	readSetDataInSale = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_2ND);

	weightDotTypeFlag = (readSetDataInSale & 0x80) >> 7;				
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
	saleLastDigitInvalidEnable = (readSetDataInSale & 0x08) >> 3;		// reserve		

// 	C4-5
   	readSetDataInSale = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_4TH);

#ifdef NO_USE_COUNT
	SaleData.saleState = ParamReadByte(PARAM_SALE_AREA_NUM, SALE_PARAM_OFFSET_SALE_STATUS);
#endif

	saleTareClearFlag = scaleTareClearFlag;
	SaleData.weight = 0;
	SaleData.unitPrice = 0;
	SaleData.pluNum = 0;
	SaleData.totalPrice = 0;

	saleBatteryCapacity = 0;
	saleBatteryCheckFlag = 0;

	saleTareKeyPressed = 0;
	saleZeroKeyPressed = 0;
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

	if (KeyCheck())
	{
			Key = KeyGetFuncCode(KEY_MODE_NORMAL);
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
//	saleHoldKey();
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

	DispSetIndicator(INDICATOR_KG, OFF);
	DispSetIndicator(INDICATOR_LB, OFF);
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

	if (AdData.grossZeroFlag && saleTareClearFlag)
	{
		returnData = AdSetTare();
	}

	switch (SaleData.saleState)
	{
		case SALE_STATE_NORMAL:
			AdSetPercentTare(SaleData.unitPrice);			
			SaleData.totalPrice = AdData.weightData;
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

	if (SaleData.saleState != SALE_STATE_NORMAL && SaleData.saleState != SALE_STATE_COUNT && SaleData.saleState != SALE_STATE_PERCENT)
	{
		PowerSaverResetTimer(TIMER_DELAY_100MS_STAND_BY);
	}

	switch (SaleData.saleState)		/*	display	*/
	{
		case SALE_STATE_NORMAL:	
			saleDisplayWeight(SaleData.weight + AdData.tareWeight);
			saleDispNumWithDecPoint(WIN_UNIT_PRICE,  0, weightDotTypeFlag, SaleData.unitPrice, 0);
			saleDispNumWithDecPoint(WIN_TOTAL_PRICE, SaleData.weightDecPos, weightDotTypeFlag, SaleData.totalPrice, 0);	
			if (SaleData.lastDigitIsInvalid)
			{
				if (SaleData.weightDecPos == 1) { ////2kg�� ��� 0.5g���� 1g���� �ö󰡸� decimal point �����ؾ� �ϹǷ� �ٽ� Display�Լ� �ҷ���
					saleDispNumWithDecPoint(WIN_TOTAL_PRICE,  0, weightDotTypeFlag, SaleData.totalPrice, 0);
				}
				DispSetChar(WIN_TOTAL_PRICE, DispGetWinSize(WIN_TOTAL_PRICE) - 1, ' ');
			}

			DispSetIndicator(INDICATOR_HOLD, OFF);
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

	returnData = AdSetTare();
	
	if (returnData != ERR_NONE)
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
			saleZeroKeyPressed = 0;
			saleTareKeyPressed = 1;
			TimerSetDelay100ms(TIMER_DELAY_100MS_WAIT_STABLE, 50);
			break;

		default:
			BuzzerSetOnTimes(2);
	}
}

/* Insert global functions here */

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
#ifdef	USE_DECIMAL_POINT	
	SaleDecimalPointKeyOnOff = OFF;	 //csh 20120730
#endif

	switch (SaleData.saleState)	 	/*clear*/
	{
		case SALE_STATE_NORMAL:			
			SaleData.unitPrice = 0L;
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
		case SALE_STATE_NORMAL:
			if (saleUnitPriceRenewalFlag == ON)
			{
				SaleData.unitPrice = 0;
#ifdef	USE_DECIMAL_POINT	
				SaleDecimalPointKeyOnOff = OFF;  //csh 20120730
#endif
			}

			SaleSetUnitPriceRenewalTimer();
			SaleData.unitPrice = EditorInsDecDigit(SaleData.unitPrice, key - KEY_NUM_0, PERCENT_TARE_SIZE, PERCENT_TARE_MAX, TRUE);
			
			break;

		default:
			break;
	}	

	return SCALE_MODE_SALE;
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
		BuzzerSetOnTimes(2);	
		DispSetConstStrMsg(WIN_WEIGHT, " NO BL");
		DispSetRunMsgPage(20, 0);
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
			break;

		default : 
			BuzzerSetOnTimes(2);
			break;
	}
}

#ifdef	USE_DECIMAL_POINT	
/**
********************************************************************************
* @brief    Decimal Point Key �Լ�    
* @param    none
* @return   none
* @remark   �Ҽ��� Ű�� ������ �����ϴ� �Լ� 
                  Ű ������ �˷��ִ� �÷��׸� ON��Ű�� �Ҽ��� �ڸ� ������ ���ڰ� �ԷµǷ�
                  ��ġ �������ִ� ���� ��ġ �÷��� ������.
********************************************************************************
*/
void saleDecimalPointKey(void) //csh 20120730
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
#endif

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
	if (scaleUsePrt == PRT_RESERVED)
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

#endif
