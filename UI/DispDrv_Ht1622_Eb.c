/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Display Driver Source File\n
           for ER-1 LED
* @file    DispDrv.c
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
#include "../Common/UiDefine.h"
#include "DispDrv.h"

#include "../Hardware/HardwareConfig.h"
#ifdef DISP_DRV_HT1622_EB
#ifdef USE_TOTAL_6DIGIT
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */


/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief Display 갯수*/
#define DISP_DRV_WIN_MAX_NUM	3

/** @brief Display driver buffer size*/
#define DISP_DRV_BUF_SIZE	27

/** @brief Display driver module size*/
#define DSP_DRV_MODULE_SIZE	9
/** @brief Display driver module string size*/
#define DSP_DRV_MODULE_STR_SIZE	6

/** @brief Display driver weight address*/
#define DISP_DRV_WEIGHT_ADDR 0
/** @brief weight display size*/
#define DISP_DRV_WEIGHT_SIZE 6
/** @brief weight display indicator address*/
#define DISP_DRV_WEIGHT_INDICATOR_ADDR 6

/** @brief Display driver Unit price address*/
#define DISP_DRV_UNIT_PRICE_ADDR 9
/** @brief Unit price display size*/
#define DISP_DRV_UNIT_PRICE_SIZE 6
/** @brief Unit price display indicator address*/
#define DISP_DRV_UNIT_PRICE_INDICATOR_ADDR 15

/** @brief Display driver Total price address*/
#define DISP_DRV_TOTAL_PRICE_ADDR 18
/** @brief Total price display size*/
#define DISP_DRV_TOTAL_PRICE_SIZE 6
/** @brief Total price display indicator address*/
#define DISP_DRV_TOTAL_PRICE_INDICATOR_ADDR 24

//Dot addr is same as digit
//Triangle addr and mask
//   0 ~ 17
//	{8, 0x20}, {8, 0x10}, {7, 0x20}, {7, 0x10}, {6, 0x20}, {6, 0x10}, 
//	{17, 0x20}, {17, 0x10}, {16, 0x20}, {16, 0x10}, {15, 0x20}, {15, 0x10},
//	{26, 0x20}, {26, 0x10}, {25, 0x20}, {25, 0x10}, {24, 0x20}, {24, 0x10} 
//BackLight Indicator addr and mask
//  if total disp window
//  {8, 0x04}

/** @brief MINUS indicator, RB is minus sign is not exist, so use digit 0 for minus char*/
/** @brief Zero indicator, triangle 1*/
#define DISP_DRV_ADDR_ZERO				{8, 0x10}
/** @brief Stable indicator, triangle 2*/
#define DISP_DRV_ADDR_STABLE				{8, 0x20}
/** @brief NET indicator, triangle 3*/
#define DISP_DRV_ADDR_NET					{7, 0x20}
/** @brief Low Battery indicator*/
#define DISP_DRV_ADDR_LOW_BATT			{26, 0x10}
/** @brief Euro indicator, triangle 5*/
#define DISP_DRV_ADDR_EURO				{26, 0x20}//6j total price
/** @brief Euro per kilogram  indicator */
#define DISP_DRV_ADDR_EURO_KG				{17, 0x20}//6j unit price
/** @brief Prepack indicator*/
#define DISP_DRV_ADDR_PREPACK				{25, 0x10}
/** @brief Backlight indicator*/
#define DISP_DRV_ADDR_BACKLIGHT			{26, 0x04}
/** @brief 100g KEY indicator*/
#define DISP_DRV_ADDR_KG					{6, 0x20}//3j weight
//#define DISP_DRV_ADDR_KG					{0, 0}//3j weight
/** @brief 100g KEY indicator*/
#define DISP_DRV_ADDR_LB					{6, 0x10}//2j weight
//#define DISP_DRV_ADDR_LB					{0, 0}//2j weight
/** @brief 100g KEY indicator*/
#define DISP_DRV_ADDR_TARESAVE			{7, 0x10}//1j weight
/** @brief 100g KEY indicator*/
#define DISP_DRV_ADDR_UNITPRICE_KG		{15, 0x20}//2j unit price
/** @brief 100g KEY indicator*/
#define DISP_DRV_ADDR_UNITPRICE_LB		{15, 0x10}//1j unit price
/** @brief 100g KEY indicator*/
#define DISP_DRV_ADDR_UNITPRICE_100G		{16, 0x10}//3j unit price
/** @brief 100g KEY indicator*/
#define DISP_DRV_ADDR_BOTHSAVE			{25, 0x20}//4j total price

/** @brief Turn off both system oscillator and Lcd bias generator*/
#define HT1622_SYS_DIS	0x00
/** @brief Turn on system oscillator*/
#define HT1622_SYS_EN	0x01
/** @brief Turn off Lcd display*/
#define HT1622_OFF		0x02
/** @brief Turn on Lcd display*/
#define HT1622_ON		0x03
/** @brief Disable time base output*/
#define HT1622_TIMER_DIS	0x04
/** @brief Disable WDT time-out flag output*/
#define HT1622_WDT_DIS	0x05
/** @brief Enable time base output*/
#define HT1622_TIMER_EN	0x06
/** @brief Enable WDT time-out flag output*/
#define HT1622_WDT_EN	0x07
/** @brief Turn off tone outputs*/
#define HT1622_TONE_OFF	0x08
/** @brief Clear the contents of the time base generator*/
#define HT1622_CLR_TIMER	0x0d
/** @brief Clear the contents of WDT stage*/
#define HT1622_CLR_WDT	0x0f
/** @brief System clock source, on-chip RC oscillator*/
#define HT1622_RC_32K	0x18
/** @brief System clock source, external clock source*/
#define HT1622_EXT_32K	0x1c
/** @brief Tone frequency output : 4kHz*/
#define HT1622_TONE_4K	0x40
/** @brief Tone frequency output : 2kHz*/
#define HT1622_TONE_2K	0x60
/** @brief Disable IRQ output*/
#define HT1622_IRQ_DIS	0x80
/** @brief Enable IRQ output*/
#define HT1622_IRQ_EN	0x88
/** @brief Time base clock output : 1Hz.   The WDT time-out flag after : 4s*/
#define HT1622_F1		0xa0
/** @brief Time base clock output : 2Hz.   The WDT time-out flag after : 2s*/
#define HT1622_F2		0xa1
/** @brief Time base clock output : 4Hz.   The WDT time-out flag after : 1s*/
#define HT1622_F4		0xa2
/** @brief Time base clock output : 8Hz.   The WDT time-out flag after : 1/2s*/
#define HT1622_F8		0xa3
/** @brief Time base clock output : 16Hz.  The WDT time-out flag after : 1/4s*/
#define HT1622_F16		0xa4
/** @brief Time base clock output : 32Hz.  The WDT time-out flag after : 1/8s*/
#define HT1622_F32		0xa5
/** @brief Time base clock output : 64Hz.  The WDT time-out flag after : 1/16s*/
#define HT1622_F64		0xa6
/** @brief Time base clock output : 128Hz. The WDT time-out flag after : 1/32s*/
#define HT1622_F128		0xa7
/** @brief Test mode, user don't use*/
#define HT1622_TEST		0xe0
/** @brief Normal mode*/
#define HT1622_NORMAL	0xe3

/** @brief FONT_SPACE*/
#define FONT_SPACE	0x00
/** @brief FONT_! , not exist*/
#define FONT_ASC33	0x00
/** @brief FONT_" , not exist*/
#define FONT_ASC34	0x00
/** @brief FONT_# , not exist*/
#define FONT_ASC35	0x00
/** @brief FONT_$ , not exist*/
#define FONT_ASC36	0x00
/** @brief FONT_% , not exist*/
#define FONT_ASC37	0x00
/** @brief FONT_& , not exist*/
#define FONT_ASC38	0x00
/** @brief FONT_' , not exist*/
#define FONT_ASC39	0x00
/** @brief FONT_( , not exist*/
#define FONT_ASC40	0x00
/** @brief FONT_) , not exist*/
#define FONT_ASC41	0x00
/** @brief FONT_* , not exist*/
#define FONT_ASC42	0x00
/** @brief FONT_+ , not exist*/
#define FONT_ASC43	0x00
/** @brief FONT_, , not exist*/
#define FONT_ASC44	0x00
/** @brief FONT_- , not exist*/
#define FONT_ASC45	0x08
/** @brief FONT_. , not exist*/
#define FONT_ASC46	0x00
/** @brief FONT_/ , not exist*/
#define FONT_ASC47	0x00

/** @brief FONT_0*/
#define FONT_0	0x77
/** @brief FONT_1*/
#define FONT_1	0x12
/** @brief FONT_2*/
#define FONT_2	0x6B
/** @brief FONT_3*/
#define FONT_3	0x5B
/** @brief FONT_4*/
#define FONT_4	0x1E
/** @brief FONT_5*/
#define FONT_5	0x5D
/** @brief FONT_6*/
#define FONT_6	0x7D
/** @brief FONT_7*/
#define FONT_7	0x13
/** @brief FONT_8*/
#define FONT_8	0x7F
/** @brief FONT_9*/
#define FONT_9	0x5F

/** @brief FONT_:, not exist*/
#define FONT_ASC58	0x00
/** @brief FONT_;, not exist*/
#define FONT_ASC59	0x00
/** @brief FONT_<, not exist*/
#define FONT_ASC60	0x00
/** @brief FONT_=, not exist*/
#define FONT_ASC61	0x00
/** @brief FONT_>, not exist*/
#define FONT_ASC62	0x00
/** @brief FONT_?, not exist*/
#define FONT_ASC63	0x00
/** @brief FONT_@, not exist*/
#define FONT_ASC64	0x00

//--------- Alphabet ------------------
/** @brief FONT_A*/
#define FONT_A   0x3F
/** @brief FONT_B*/
#define FONT_B   0x7C
/** @brief FONT_C*/
#define FONT_C   0x65
/** @brief FONT_D*/
#define FONT_D   0x7A
/** @brief FONT_E*/
#define FONT_E   0x6D
/** @brief FONT_F*/
#define FONT_F   0x2D
/** @brief FONT_G*/
#define FONT_G   0x5F
/** @brief FONT_H*/
#define FONT_H   0x3E
/** @brief FONT_I*/
#define FONT_I   0x12
/** @brief FONT_J*/
#define FONT_J   0x72
/** @brief FONT_K*/
#define FONT_K   0x2C
/** @brief FONT_L*/
#define FONT_L   0x64
/** @brief FONT_M*/
#define FONT_M   0x39
/** @brief FONT_N*/
#define FONT_N   0x38
/** @brief FONT_O*/
#define FONT_O   0x78
/** @brief FONT_P*/
#define FONT_P   0x2F
/** @brief FONT_Q*/
#define FONT_Q   0x1F
/** @brief FONT_R*/
#define FONT_R   0x28
/** @brief FONT_S*/
#define FONT_S   0x5D
/** @brief FONT_T*/
#define FONT_T   0x6C
/** @brief FONT_U*/
#define FONT_U   0x76
/** @brief FONT_V*/
//#define FONT_V   0x0E
#define FONT_V   0x70
/** @brief FONT_W*/
#define FONT_W   0x4E
/** @brief FONT_X*/
#define FONT_X   0x36
/** @brief FONT_Y*/
#define FONT_Y   0x5E
/** @brief FONT_Z*/
#define FONT_Z   0x6B

/** @brief TRIANGLE_MASK*/
#define DISP_DRV_TRIANGLE_MASK	0x80
/** @brief DOT_MASK*/
#define DISP_DRV_DOT_MASK		0x80
/** @brief CHAR_MASK*/
#define DISP_DRV_CHAR_MASK		0x7F

/*
********************************************************************************
*                       LOCAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert local typedef & structure here */
/** @brief Display Driver에서 Segment Display시 사용하는 struct */
typedef struct
{
	/** @brief Image Memory Address */
	INT8U addr;
	/** @brief Image Memory Size */
	INT8U size;
} DISP_DRV_DATA;

/** @brief Display Driver에서 Indicator Display시 사용하는 struct */
typedef struct
{
	/** @brief Image Memory Address */
	INT8U addr;
	/** @brief Image Memory Address에서 Indicator bit Mask */
	INT8U mask;
} INDICATOR_ADDR;
/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/
/* Insert file scope variable & tables here */
/** @brief Display Driver에서 사용하는 Segment Font Table */
const INT8U code FontTable[59] =
{
	FONT_SPACE, FONT_ASC33, FONT_ASC34, FONT_ASC35, FONT_ASC36,
	FONT_ASC37, FONT_ASC38,	FONT_ASC39, FONT_ASC40, FONT_ASC41,
	FONT_ASC42, FONT_ASC43, FONT_ASC44, FONT_ASC45,	FONT_ASC46, FONT_ASC47,  // 32 - 47               
    FONT_0,		FONT_1,		FONT_2,		FONT_3,		FONT_4,
	FONT_5,		FONT_6,		FONT_7,		FONT_8,		FONT_9, // 48 - 57  Num 
    FONT_ASC58, FONT_ASC59, FONT_ASC60, FONT_ASC61, FONT_ASC62,
	FONT_ASC63, FONT_ASC64,  // 58 - 64  
    FONT_A,		FONT_B,		FONT_C,		FONT_D,		FONT_E,		FONT_F,		FONT_G, 
    FONT_H,		FONT_I,		FONT_J,		FONT_K,		FONT_L,		FONT_M,		FONT_N, 
    FONT_O,		FONT_P,		FONT_Q,		FONT_R,		FONT_S,		FONT_T,		FONT_U,         
    FONT_V,		FONT_W,		FONT_X,		FONT_Y,		FONT_Z //  65 -90  alphabet 
};
/** @brief Display Driver에서 Segment Display시 사용하는 struct Window 개수 만큼 존재 */
static DISP_DRV_DATA dispDrvStruct[DISP_DRV_WIN_MAX_NUM];
/** @brief Image Buffer : 실제 Display Driver IC에 존재하는 메모리의 복사본 */
static INT8U idata dispDrvBuf[DISP_DRV_PAGE_SIZE][DISP_DRV_BUF_SIZE];
/** @brief Display Driver Mode */
static INT8U dispDrvMode;
/** @brief Display Driver IC에 보낼 Image Buffer 번호 */
static INT8U dispTransPage;
/** @brief Display Driver에서 Edit시 사용하는 Image Buffer 번호 */
static INT8U dispEditingPage;

/** 
* @brief Display Driver에서 Indicator Display시 사용하는 struct, Indicator 개수 만큼 존재 
* @remark 배열에서 Indicator위치는 define되어 있음\n
*         임의로 위치 변경하지 마시오.
*         관련 정의를 보려면 DISP_DRV_ADDR_WEIGHT_MINUS를 찾아 보시오.
*/
const INDICATOR_ADDR code dispDrvIndicatorAddrTable[INDICATOR_MAX_NUM] =
{
	{0, 0}, 							///0 Weight Minus
	{0, 0}, 							///1 Unit price Minus
	{0, 0}, 							///2 Total price Minus
	{0, 0}, 							///3 Tare Minus
	DISP_DRV_ADDR_ZERO, 				///4 Zero
	DISP_DRV_ADDR_STABLE,				///5 Stable
	DISP_DRV_ADDR_NET,					///6 Net
	DISP_DRV_ADDR_LOW_BATT, 			///7 Low battery
	{0, 0}, 							///8 Ounce
	DISP_DRV_ADDR_LB, 					///9 Pound
	DISP_DRV_ADDR_KG, 					//10 Kilo gram
	{0, 0}, 							//11 Gram
	{0, 0}, 							//12 Counting mode
	DISP_DRV_ADDR_PREPACK,				//13 Prepack
	{0, 0}, 							//14 Compare high
	{0, 0}, 							//15 Compare low
	{0, 0}, 							//16 Compare OK 
	DISP_DRV_ADDR_EURO, 				//17 Euro
	{0, 0}, 							//18 Hold
	DISP_DRV_ADDR_BACKLIGHT,			//19 Backlight
	{0, 0}, 							//20 Counting lack
	DISP_DRV_ADDR_UNITPRICE_100G,		//21 Unit price 100Gram
	DISP_DRV_ADDR_TARESAVE, 			//22 Tare save
	DISP_DRV_ADDR_BOTHSAVE, 			//23 Both save
	DISP_DRV_ADDR_UNITPRICE_KG,			//24 Unit price Kilo Gram
	DISP_DRV_ADDR_UNITPRICE_LB,			//25 Unit price Pound
	{0, 0}, 							//26 Percent
	{0, 0},								//27 Sum
	{0, 0},								//28 full battery
	{0, 0},								//29 charging battery
	DISP_DRV_ADDR_EURO_KG,				//30 Euro per Kilogram
	{0, 0},								//31 $
	{0, 0},								//32 $/kg
	{0, 0},								//33 Unit price oz
	{0, 0}								//34 Connected AC
};

//Comma addr and mask
//   0 ~ 17
//	{8, 0x80}, {8, 0x40}, {7, 0x80}, {7, 0x40}, {6, 0x80}, {6, 0x40}, 
//	{17, 0x80}, {17, 0x40}, {16, 0x80}, {16, 0x40}, {15, 0x80}, {15, 0x40}, 
//	{26, 0x80}, {26, 0x40}, {25, 0x80}, {25, 0x40}, {24, 0x80}, {24, 0x40} 
//relative address per module
const INDICATOR_ADDR code dispDrvCommaAddrTable[6] =
{
	{2, 0x40}, {2, 0x80}, {1, 0x40}, {1, 0x80}, {0, 0x40}, {0, 0x80}, 
};

//Battery addr and mask
//  if total disp window, 0 = battery outline, 1~4 = energy amount
//	{25, 0x04}, {25, 0x01}, {25, 0x02}, {26, 0x01}, {26, 0x02}
const INDICATOR_ADDR code dispDrvBattAddrTable[5] =
{
	{25, 0x04}, {26, 0x02}, {26, 0x01}, {25, 0x02}, {25, 0x01} 
};

/** 
* @brief Menu Mode에서 사용될 Window 번호 Table
* @remark String Window, Aux. Data window, Data Window 순서임 (see DISP_MENU_STR_WIN_NUM~)
*/
const INT8U code dispDrvWinTableForMenu[DISP_MAX_WIN_NUM_FOR_MENU] = 
{
	WIN_WEIGHT,	WIN_UNIT_PRICE, WIN_TOTAL_PRICE
};

/**
* @brief 각 Window의 Size Table
* @remark WIN_WEIGHT, WIN_UNIT_PRICE, WIN_TOTAL_PRICE, WIN_TARE 순서임 (see WIN_WEIGHT~)
*/
const INT8U code dispDrvWinSizeTable[WIN_MAX_NUM] = 
{
	6, 6, 6, 0
};

/**
* @brief 각 Window의 '-' sign 유무 Table 
* @remark WIN_WEIGHT, WIN_UNIT_PRICE, WIN_TOTAL_PRICE, WIN_TARE 순서임 (see WIN_WEIGHT~)
*/
const INT8U code dispDrvMinusSignExistTable[WIN_MAX_NUM] = 
{
	0, 0, 0, 0
};

/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
static void dispDrvSetDot(INT8U winNum, INT8U decPos, INT8U on);
static void dispDrvSetComma(INT8U winNum, INT8U decPos, INT8U on);
static void dispDrvSetBattIndicator(INT8U num, INT8U on);
static void ht1622WriteCommand(INT8U datcom);
static void ht1622TranCommand(INT8U command);      
static void ht1622TranData(INT8U disdata);
static void ht1622WriteAllData();         

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
/**
********************************************************************************
* @brief    Display Driver Init 함수\n
*           Display Driver IC Init\n
*           Display Driver 관련 변수 초기화(Message, Window, Page)\n
* @param    none
* @return   none
* @remark   초기에 한번 실행 되는 함수\n
********************************************************************************
*/
void DispDrvInit(INT8U brightness)
{
	DispDrvClearBuf();
	DispDrvSetMode(DISP_DRV_NORMAL_MODE);
	DispDrvSetPage(DISP_DRV_NORMAL_PAGE);

	dispDrvStruct[WIN_WEIGHT].addr = DISP_DRV_WEIGHT_ADDR;
	dispDrvStruct[WIN_WEIGHT].size = DISP_DRV_WEIGHT_SIZE;

	dispDrvStruct[WIN_UNIT_PRICE].addr = DISP_DRV_UNIT_PRICE_ADDR;
	dispDrvStruct[WIN_UNIT_PRICE].size = DISP_DRV_UNIT_PRICE_SIZE;

	dispDrvStruct[WIN_TOTAL_PRICE].addr = DISP_DRV_TOTAL_PRICE_ADDR;
	dispDrvStruct[WIN_TOTAL_PRICE].size = DISP_DRV_TOTAL_PRICE_SIZE;

//	dispDrvStruct[WIN_TARE].addr = 0;
//	dispDrvStruct[WIN_TARE].size = 0;

	ht1622WriteCommand(HT1622_ON);	
	ht1622WriteCommand(HT1622_SYS_EN); /* Turn on system oscillator*/	
//	ht1622WriteCommand(HT1622_TONE_OFF); /* Turn off tone outputs */		
//	ht1622WriteCommand(HT1622_RC_32K); /* System clock source, on-chip RC oscillator*/
//	ht1622WriteCommand(HT1622_TIMER_DIS);
//	ht1622WriteCommand(HT1622_WDT_DIS);
//	ht1622WriteCommand(HT1622_IRQ_DIS);
//	ht1622WriteCommand(HT1622_BIAS_1_3);	
//	ht1622WriteCommand(HT1622_IRQ_DIS);	/* Disable IRQ output */	
//	ht1622WriteCommand(HT1622_NORMAL);	/* Normal mode*/

	DispDrvClearBuf();
	ht1622WriteAllData();
}

/**
********************************************************************************
* @brief    Display API 에서 사용할 각 Window 크기를 반환함\n
* @param    winNum : Window 번호
* @return   Display Driver로 부터 반환 받은 Window Size
* @remark   
********************************************************************************
*/
INT8U DispDrvGetWinSize(INT8U winNum)
{
	if (winNum >= WIN_MAX_NUM)
	{
		return 0;
	}
	else
	{
		return dispDrvWinSizeTable[winNum];
	}
}

/**
********************************************************************************
* @brief    각 Window가 독립적은 Minus Indicator를 갖고 있는지를 반환함\n
* @param    winNum : Window 번호
* @return   0 : 없음, 1 : 있음
* @remark   
********************************************************************************
*/
BOOLEAN DispDrvGetMinusSignExist(INT8U winNum)
{
	if (winNum >= WIN_MAX_NUM)
	{
		return 0;
	}
	else
	{
		return dispDrvMinusSignExistTable[winNum];
	}
}

/**
********************************************************************************
* @brief    Menu Mode에서 사용할 Window를 Display Driver로 부터 받아서 반환함\n
* @param    num : menu 모드 Window 기능 번호
* @return   Display Driver로 부터 반환 받은 Window 번호
* @remark   menu Mode에서 사용할 Window를 Display Driver가 결정하기 위해
*           사용 되는 함수 임\n
*           DISP_MENU_STR_WIN_NUM, DISP_MENU_AUX_WIN_NUM, DISP_MENU_DATA_WIN_NUM
********************************************************************************
*/
INT8U DispDrvGetMenuWinByNum(INT8U num)
{
	return dispDrvWinTableForMenu[num];
}

/**
********************************************************************************
* @brief    Display Driver mode 설정\n
* @param    mode : display mode 번호
* @return   none
* @remark   DISP_DRV_NORMAL_MODE : 일반 모드 (DISP_DRV_NORMAL_PAGE, DISP_DRV_MSG_PAGE)
*           DISP_DRV_SUSPEND_MODE : 대기 모드 (DISP_DRV_SUSPEND_PAGE)
*           대기 모드일 경우 Driver 단에서 대기 모드 동작 (TO DO)
********************************************************************************
*/
void DispDrvSetMode(INT8U mode)
{
	dispDrvMode = mode;
}

/**
********************************************************************************
* @brief    IC에 전송할 Display Driver Image page 설정\n
* @param    mode : display image page 번호
* @return   none
* @remark   DISP_DRV_NORMAL_PAGE, DISP_DRV_MSG_PAGE, DISP_DRV_SUSPEND_PAGE
********************************************************************************
*/
void DispDrvSetPage(INT8U page)
{
	if (page < DISP_DRV_PAGE_SIZE)
	{
		dispTransPage = page;
	}
}

/**
********************************************************************************
* @brief    Editting할 Display Driver Image page 설정\n
* @param    mode : display image page 번호
* @return   none
* @remark   DISP_DRV_NORMAL_PAGE, DISP_DRV_MSG_PAGE, DISP_DRV_SUSPEND_PAGE
********************************************************************************
*/
void DispDrvSetEditingPage(INT8U page)
{
	dispEditingPage = page;
}

/**
********************************************************************************
* @brief    Display Driver image Clear all (All Page)
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void DispDrvClearBuf(void)
{
	INT8U page;
	INT8U addr;

	for (page = 0; page < DISP_DRV_PAGE_SIZE; page++) 
	{
		for (addr = 0; addr < DISP_DRV_BUF_SIZE; addr++) 
		{
			dispDrvBuf[page][addr] = 0x00;
		}
	}
}

/**
********************************************************************************
* @brief    window에 1 문자 출력하는 함수 (editting Page)
* @param    winNum : Window 번호\n
*           pos : Window에서 위치 (0~ )
*           cdata : 문자 data
* @return   none
* @remark   
********************************************************************************
*/
void DispDrvSetChar(INT8U winNum, INT8U pos, char cdata)
{
	INT8U byte;
	INT8U addr;

	addr = dispDrvStruct[winNum].addr;

	byte = dispDrvBuf[dispEditingPage][addr + pos];
	byte = byte & (~DISP_DRV_CHAR_MASK);
	dispDrvBuf[dispEditingPage][addr + pos] = (FontTable[cdata - 0x20] & DISP_DRV_CHAR_MASK) | byte;
}

/**
********************************************************************************
* @brief    Editting Image에 입력 받은 문자로 채움
* @param    fillChar : 채울 문자
* @return   none
* @remark   
********************************************************************************
*/
void DispDrvFillChar(char fillChar)
{
	INT8U winNum;
	INT8U pos;
	INT8U size;

	for (winNum = 0; winNum < DISP_DRV_WIN_MAX_NUM; winNum++)
	{
		size = dispDrvStruct[winNum].size;
		for (pos = 0; pos < size; pos++) 
		{
			DispDrvSetChar(winNum, pos, fillChar);
		}
	}
}

/**
********************************************************************************
* @brief    Editting Image에 모든 Indicator 제어 함수
* @param    on : 1 -> 켬, 0 -> 끔
* @return   none
* @remark   
********************************************************************************
*/
void DispDrvSetAllIndicator(INT8U on)
{
	INT8U winNum;
	INT8U pos;
	INT8U size;

	for (pos = 0; pos < INDICATOR_MAX_NUM; pos++)
	{
		DispDrvSetIndicator(pos, on);
	}
	for (winNum = 0; winNum < DISP_DRV_WIN_MAX_NUM; winNum++)
	{	
		size = dispDrvStruct[winNum].size;
		for (pos = 0; pos < size; pos++)
		{
			dispDrvSetDot(winNum, pos, on);
			dispDrvSetComma(winNum, pos, on);
		}
	}
}

/**
********************************************************************************
* @brief    Window에 String 출력 (editting Page)
* @param    winNum : Window 번호\n
*           str : 출력할 String Pointer
* @return   none
* @remark   str 채운후 남는 부분은 space로 채움
********************************************************************************
*/
void DispDrvSetStr(INT8U winNum, char *str)	 
{
	INT8U i;
	INT8U dispChar;
	INT8U byte;
	INT8U addr;
	INT8U strEndFlag;
	INT8U dataSize;

	if (winNum > DISP_DRV_WIN_MAX_NUM - 1)
	{
		return;
	}

	strEndFlag = 0;

	addr = dispDrvStruct[winNum].addr;
	dataSize = dispDrvStruct[winNum].size;
	
	if ((addr + dataSize) > DISP_DRV_BUF_SIZE) 
	{
		return;
	}

	for (i = 0; i < dataSize; i++) 
	{
		if ((str[i] == 0x00) || (strEndFlag == 1)) 
		{
			strEndFlag = 1;
			dispChar = 0x20;
		}
		else 
		{
			dispChar = str[i];
		}

		if (dispChar >= 'a' && dispChar <= 'z')
		{
			dispChar = dispChar - 0x20;	//('A' = 'a' - 0x20) 소문자->대문자
		}
		if (dispChar < 0x20 || dispChar > 'Z')
		{
			dispChar = 0x20;	//space
		}
		byte = dispDrvBuf[dispEditingPage][addr + i];
		byte = byte & (~DISP_DRV_CHAR_MASK);
		dispDrvBuf[dispEditingPage][addr + i] = (FontTable[dispChar - 0x20] & DISP_DRV_CHAR_MASK) | byte;
	}
}

/**
********************************************************************************
* @brief    Window에 Const String 출력 (editting Page)
* @param    winNum : Window 번호\n
*           str : 출력할 Const String Pointer
* @return   none
* @remark   str 채운후 남는 부분은 space로 채움
********************************************************************************
*/
void DispDrvSetConstStr(INT8U winNum, const char *str)	 
{
	INT8U i;
	INT8U dispChar;
	INT8U byte;
	INT8U addr;
	INT8U strEndFlag;
	INT8U dataSize;

	if (winNum > DISP_DRV_WIN_MAX_NUM - 1)
	{
		return;
	}
	strEndFlag = 0;
						
	addr = dispDrvStruct[winNum].addr;
	dataSize = dispDrvStruct[winNum].size;

	if ((addr + dataSize) > DISP_DRV_BUF_SIZE)
	{
		return;
	}

	for (i = 0; i < dataSize; i++) 
	{
		if ((str[i] == 0x00) || (strEndFlag == 1)) 
		{
			strEndFlag = 1;
			dispChar = 0x20;
		}
		else 
		{
			dispChar = str[i];
		}
		if (dispChar >= 'a' && dispChar <= 'z')
		{
			dispChar = dispChar - 0x20;	//('A' = 'a' - 0x20) 소문자->대문자
		}
		if (dispChar < 0x20 || dispChar > 'Z')
		{
			dispChar = 0x20;	//space
		}
		byte = dispDrvBuf[dispEditingPage][addr + i];
		byte = byte & (~DISP_DRV_CHAR_MASK);
		dispDrvBuf[dispEditingPage][addr + i] = (FontTable[dispChar - 0x20] & DISP_DRV_CHAR_MASK) | byte;
	}
}

/**
********************************************************************************
* @brief    Window에 Decimal Point Position, Type 값을 Setting (editting Page)
* @param    winNum : Window 번호\n
*           decPos : Decimal Point Position\n
*           decType : Decimal Point Type\n
*           useThousandPt : use flag for thousnad point
* @return   Decimal Point Type
* @remark   position : xxx -> 0, xx.x -> 1, x,xx -> 2
*           type : DEC_TYPE_DOT, DEC_TYPE_COMMA
********************************************************************************
*/
void DispDrvSetDecPoint(INT8U winNum, INT8U decPos, INT8U decType, INT8U useThousandPt)
{
	INT8U i;
	INT8U size;

	if (winNum > DISP_DRV_WIN_MAX_NUM - 1)
	{
		return;
	}

	size = dispDrvStruct[winNum].size;
	
	for (i = 0; i < size; i++)
	{
		dispDrvSetDot(winNum, i, OFF);
		dispDrvSetComma(winNum, i, OFF);
	}
	if (decPos != 0)
	{
		dispDrvSetDot(winNum, decPos, ON);
		if (decType == DEC_TYPE_COMMA)
		{
			dispDrvSetComma(winNum, decPos, ON);
		}
	}
	if ((useThousandPt == 1) && ((decPos + 3) < size))
	{
		dispDrvSetDot(winNum, 3 + decPos, ON);
		dispDrvSetComma(winNum, 3 + decPos, ON);
	}
}

/**
********************************************************************************
* @brief    Indicator on/off setting 함수 (editting Page)
* @param    num : Indicator 번호\n
*           on : 1 -> 켜기, 0 -> 끄기
* @return   none
* @remark   Indicator 번호 정의되어 있음 (INDICATOR_WEIGHT_MINUS...)
********************************************************************************
*/
void DispDrvSetIndicator(INT8U num, INT8U on)
{
	INT8U addr;
	INT8U mask;

	addr = dispDrvIndicatorAddrTable[num].addr;
	mask = dispDrvIndicatorAddrTable[num].mask;

	if (on)
	{
		dispDrvBuf[dispEditingPage][addr] = dispDrvBuf[dispEditingPage][addr] | mask;
	}
	else
	{
		dispDrvBuf[dispEditingPage][addr] = dispDrvBuf[dispEditingPage][addr] & (~mask);
	}
}

/**
********************************************************************************
* @brief    Bettery Energy Indicator setting 함수 (editting Page)
* @param    percent : Battery 양 (%)\n
* @return   none
* @remark   Battery Indicator들의 조합으로 전체 표시됨
********************************************************************************
*/
void DispDrvSetBattEnergy(INT8U percent)
{
	if (percent == 0)
	{
		dispDrvSetBattIndicator(4, OFF);
		dispDrvSetBattIndicator(3, OFF);
		dispDrvSetBattIndicator(2, OFF);
		dispDrvSetBattIndicator(1, OFF);
		dispDrvSetBattIndicator(0, ON);
	}
	else if (percent < 25)
	{
		dispDrvSetBattIndicator(4, OFF);
		dispDrvSetBattIndicator(3, OFF);
		dispDrvSetBattIndicator(2, OFF);
		dispDrvSetBattIndicator(1, ON);
		dispDrvSetBattIndicator(0, ON);
	}
	else if (percent < 50)
	{
		dispDrvSetBattIndicator(4, OFF);
		dispDrvSetBattIndicator(3, OFF);
		dispDrvSetBattIndicator(2, ON);
		dispDrvSetBattIndicator(1, ON);
		dispDrvSetBattIndicator(0, ON);
	}
	else if (percent < 75)
	{
		dispDrvSetBattIndicator(4, OFF);
		dispDrvSetBattIndicator(3, ON);
		dispDrvSetBattIndicator(2, ON);
		dispDrvSetBattIndicator(1, ON);
		dispDrvSetBattIndicator(0, ON);
	}
	else// if (percent <= 100)
	{
		dispDrvSetBattIndicator(4, ON);
		dispDrvSetBattIndicator(3, ON);
		dispDrvSetBattIndicator(2, ON);
		dispDrvSetBattIndicator(1, ON);
		dispDrvSetBattIndicator(0, ON);
	}
}

/**
********************************************************************************
* @brief    Display Driver Procedure\n
* @param    none
* @return   none
* @remark   dispTransPage의 Image를 Display Driver IC로 전송
********************************************************************************
*/
void DispDrvProc(void)
{
	ht1622WriteAllData();
}

/**
********************************************************************************
* @brief    Display Driver Procedure\n
* @param    none
* @return   none
* @remark   dispTransPage의 Image를 Display Driver IC로 전송
********************************************************************************
*/
void DispDrvTurnOnOff(BOOLEAN onOff)
{
	if(onOff)
	{
		ht1622TranCommand(HT1622_ON);
	}
	else
	{
		ht1622TranCommand(HT1622_OFF);
	}
}

/**
********************************************************************************
* @brief    Display Driver BrightnessLevel\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void DispDrvSetBright(INT8U level)
{	
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
/**
********************************************************************************
* @brief    Dot Indicator setting 함수 (editting Page)
* @param    winNum : Window 번호\n
*           decPos : Decimal Point Position\n
*           on : 1 -> on, 0 -> off
* @return   none
* @remark   
********************************************************************************
*/
void dispDrvSetDot(INT8U winNum, INT8U decPos, INT8U on)
{
	INT8U addr;

	addr = dispDrvStruct[winNum].size - decPos - 1;
	addr = dispDrvStruct[winNum].addr + addr;

	if (on)
	{
		dispDrvBuf[dispEditingPage][addr] = dispDrvBuf[dispEditingPage][addr] | DISP_DRV_DOT_MASK;
	}
	else
	{
		dispDrvBuf[dispEditingPage][addr] = dispDrvBuf[dispEditingPage][addr] & (~DISP_DRV_DOT_MASK);
	}
}

/**
********************************************************************************
* @brief    Comma Indicator setting 함수 (editting Page)
* @param    winNum : Window 번호\n
*           decPos : Decimal Point Position\n
*           on : 1 -> on, 0 -> off
* @return   none
* @remark   
********************************************************************************
*/
void dispDrvSetComma(INT8U winNum, INT8U decPos, INT8U on)
{
	INT8U pos;
	INT8U addr;
	INT8U mask;
	INT8U noUseDigit;

	
	noUseDigit = DSP_DRV_MODULE_STR_SIZE - dispDrvStruct[winNum].size;
	pos = dispDrvStruct[winNum].size - decPos - 1;
	pos = pos + noUseDigit;

	addr = DISP_DRV_WEIGHT_INDICATOR_ADDR + winNum * DSP_DRV_MODULE_SIZE;
	addr = addr + dispDrvCommaAddrTable[pos].addr;
	mask = dispDrvCommaAddrTable[pos].mask;

	if (on)
	{
		dispDrvBuf[dispEditingPage][addr] = dispDrvBuf[dispEditingPage][addr] | mask;
	}
	else
	{
		dispDrvBuf[dispEditingPage][addr] = dispDrvBuf[dispEditingPage][addr] & (~mask);
	}

}

/**
********************************************************************************
* @brief    Bettery Energy Indicator setting 함수 (editting Page)
* @param    num : Battery Indicator Number \n
* @return   none
* @remark   
********************************************************************************
*/
void dispDrvSetBattIndicator(INT8U num, INT8U on)
{
	INT8U addr;
	INT8U mask;

	addr = dispDrvBattAddrTable[num].addr;
	mask = dispDrvBattAddrTable[num].mask;

	if (on)
	{
		dispDrvBuf[dispEditingPage][addr] = dispDrvBuf[dispEditingPage][addr] | mask;
	}
	else
	{
		dispDrvBuf[dispEditingPage][addr] = dispDrvBuf[dispEditingPage][addr] & (~mask);
	}
}

/**
********************************************************************************
* @brief    HT1622 IC에 command 전송 함수
* @param    datcom : command\n
* @return   none
* @remark   
********************************************************************************
*/
void ht1622WriteCommand(INT8U datcom)         
{
    LCD_CS = 0;

 	LCD_WR = 0;	
	LCD_DATA = 1;
	delay2Clock();
	LCD_WR = 1;  ///1
	delay2Clock();

	LCD_WR = 0;
	LCD_DATA = 0;
	delay2Clock();
	LCD_WR = 1;  //0
	delay2Clock();

	LCD_WR = 0;
	LCD_DATA = 0;
	delay2Clock();
	LCD_WR = 1;  //0
	delay2Clock();

    ht1622TranCommand(datcom);

	LCD_WR = 0;
	delay2Clock();
	LCD_WR = 1;  
	delay2Clock();
    LCD_CS = 1;
}

/**
********************************************************************************
* @brief    HT1622 IC에 command byte 전송 함수
* @param    command : command\n
* @return   none
* @remark   
********************************************************************************
*/
void ht1622TranCommand(INT8U command)      
{
    INT8U k;
    
    for (k = 0; k < 8; k++)
	{
		LCD_WR = 0;
		//Send most significant bit first 
		if ((command & 0x80) == 0)
		{
			LCD_DATA = 0;
		}
		else
		{
			LCD_DATA = 1;
		}
		delay2Clock();
		LCD_WR = 1;   //Data are latched into the HT1622
		command <<= 1;
		delay2Clock();
	}  
}

/**
********************************************************************************
* @brief    HT1622 IC에 data byte 전송 함수
* @param    disdata : data\n
* @return   none
* @remark   
********************************************************************************
*/
void ht1622TranData(INT8U disdata)       
{
    INT8U k;
    
    for (k = 0; k < 8; k++)
	{
        LCD_WR = 0;

		LCD_DATA = disdata & 0x01;

		LCD_WR = 1;    
		disdata >>= 1;

		delay2Clock();
	}  
}

/**
********************************************************************************
* @brief    HT1622 IC에 dispTransPage의 Image를 Display Driver IC로 전송
* @param    disdata : data\n
* @return   none
* @remark   
********************************************************************************
*/
void ht1622WriteAllData()         
{
    INT8U i;
	INT8U *dispBufPtr;

    LCD_CS = 0;
	LCD_WR = 0;
	delay2Clock();

	LCD_DATA = 1;
	delay2Clock();

	LCD_WR = 1;
	delay2Clock();

    ht1622TranCommand(0x40); //Write from memory address 0

    dispBufPtr = &dispDrvBuf[dispTransPage][0];
	for (i = 0; i < DISP_DRV_BUF_SIZE; i++)
	{   
        ht1622TranData(*dispBufPtr);
		dispBufPtr++;
	}
    LCD_CS = 1;
}
#endif//#ifdef USE_TOTAL_6DIGIT
#endif//#ifdef DISP_DRV_HT1622_EB


