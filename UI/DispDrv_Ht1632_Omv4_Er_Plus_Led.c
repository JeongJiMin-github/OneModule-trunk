/**
********************************************************************************
* Copyright (c) 2016 CAS
* @brief   Display Driver Source File\n
           for OMV4 ER PLUS M LED
* @file    DispDrv_Ht1622_Omv4_Er_Plus_Led.c
* @version 1.0
* @date    2016/12/06
* @author  Park Young Min (FDC954)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2016/12/06 created by FDC954 \n
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
#ifdef DISP_DRV_HT1632_M0516LDN_ER_LED
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
#define DISP_DRV_BUF_SIZE	22

/** @brief Display driver module size*/
#define DSP_DRV_MODULE_SIZE		9
/** @brief Display driver module string size*/
#define DSP_DRV_MODULE_STR_SIZE	6

/** @brief Display driver weight address*/
#define DISP_DRV_WEIGHT_ADDR 0
/** @brief weight display size*/
#define DISP_DRV_WEIGHT_SIZE 6

/** @brief Display driver Unit price address*/
#define DISP_DRV_UNIT_PRICE_ADDR 6
/** @brief Unit price display size*/
#define DISP_DRV_UNIT_PRICE_SIZE 6

/** @brief Display driver Total price address*/
#define DISP_DRV_TOTAL_PRICE_ADDR 12
/** @brief Total price display size*/
#define DISP_DRV_TOTAL_PRICE_SIZE 8

//Dot addr is same as digit
//Triangle addr and mask
//   0 ~ 17
//	{8, 0x20}, {8, 0x10}, {7, 0x20}, {7, 0x10}, {6, 0x20}, {6, 0x10}, 
//	{17, 0x20}, {17, 0x10}, {16, 0x20}, {16, 0x10}, {15, 0x20}, {15, 0x10},
//	{26, 0x20}, {26, 0x10}, {25, 0x20}, {25, 0x10}, {24, 0x20}, {24, 0x10} 
//BackLight Indicator addr and mask
//  if total disp window
//  {8, 0x04}

/** @brief MINUS indicator, ER has no minus sign, so use digit 0 for minus char*/
/** @brief Zero indicator */
#define DISP_DRV_ADDR_ZERO				{20, 0x01}//1j weight 
/** @brief NET indicator */	
#define DISP_DRV_ADDR_NET					{20, 0x02}//2j weight
/** @brief Low Battery indicator*/
#define DISP_DRV_ADDR_LOW_BATT			{20, 0x40}//1j unit price
/** @brief Euro per Kg indicator */
#define DISP_DRV_ADDR_EURO_KG			{20, 0x80}//2j unit price 
/** @brief Kg indicator*/
#define DISP_DRV_ADDR_KG					{20, 0x04}//3j weight
/** @brief lb indicator*/
#define DISP_DRV_ADDR_LB					{20, 0x08}//4j weight
/** @brief tare save indicator*/
#define DISP_DRV_ADDR_TARESAVE			{20, 0x20}//6j weight
/** @brief EURO indicator*/
#define DISP_DRV_ADDR_EURO				{21, 0x10}//3j total price 
/** @brief 100g KEY indicator*/
#define DISP_DRV_ADDR_UNITPRICE_100G	{21, 0x02}//4j unit price
/** @brief unit price kg indicator*/
#define DISP_DRV_ADDR_UNITPRICE_KG		{21, 0x04}//5j unit price
/** @brief unit price lb indicator*/
#define DISP_DRV_ADDR_UNITPRICE_LB		{21, 0x08}//6j unit price
/** @brief 100g KEY indicator*/
#define DISP_DRV_ADDR_BOTHSAVE			{21, 0x20}//4j total price
/** @brief Prepack indicator*/
#define DISP_DRV_ADDR_PREPACK			{21, 0x10}//3j total price same with EURO indicator.(alternate use)


/** @brief Turn off both system oscillator and LED bias generator*/
#define HT1632_SYS_DIS	0x00
/** @brief Turn on system oscillator*/
#define HT1632_SYS_EN	0x01
/** @brief Turn off LED display*/
#define HT1632_OFF		0x02
/** @brief Turn on LED display*/
#define HT1632_ON		0x03
/** @brief Turn off blinking function*/
#define HT1632_BLINK_OFF	0x08
/** @brief Turn on blinking function*/
#define HT1632_BLINK_ON	 	0x09
/** @brief Set slave mode and use external clock*/
#define HT1632_SLAVE_MODE		0x10
/** @brief Set master mode and use on-chip osc*/
#define HT1632_MASTER_MODE	0x14
/** @brief System clock source, on-chip RC oscillator*/
#define HT1632_RC		0x18
/** @brief System clock source, external clock source*/
#define HT1632_EXT_CLK		0x1c
/** @brief N-MOS open drain output and 8 com*/
#define HT1632_NMOS_8COM		0x20
/** @brief N-MOS open drain output and 16 com*/
#define HT1632_NMOS_16COM		0x24
/** @brief P-MOS open drain output and 8 com*/
#define HT1632_PMOS_8COM		0x28
/** @brief P-MOS open drain output and 16 com*/
#define HT1632_PMOS_16COM		0x2c
/** @brief PWM 1/16 duty*/
#define HT1632_PWM1		0xa0
/** @brief PWM 2/16 duty*/
#define HT1632_PWM2		0xa1
/** @brief PWM 3/16 duty*/
#define HT1632_PWM3		0xa2
/** @brief PWM 4/16 duty*/
#define HT1632_PWM4		0xa3
/** @brief PWM 5/16 duty*/
#define HT1632_PWM5		0xa4
/** @brief PWM 6/16 duty*/
#define HT1632_PWM6		0xa5
/** @brief PWM 7/16 duty*/
#define HT1632_PWM7		0xa6
/** @brief PWM 8/16 duty*/
#define HT1632_PWM8		0xa7
/** @brief PWM 9/16 duty*/
#define HT1632_PWM9		0xa8
/** @brief PWM 10/16 duty*/
#define HT1632_PWM10 	0xa9
/** @brief PWM 11/16 duty*/
#define HT1632_PWM11 	0xaa
/** @brief PWM 12/16 duty*/
#define HT1632_PWM12 	0xab
/** @brief PWM 13/16 duty*/
#define HT1632_PWM13 	0xac
/** @brief PWM 14/16 duty*/
#define HT1632_PWM14 	0xad
/** @brief PWM 15/16 duty*/
#define HT1632_PWM15 	0xae
/** @brief PWM 16/16 duty*/
#define HT1632_PWM16 	0xaf


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
#define FONT_ASC45	0x40
/** @brief FONT_. , not exist*/
#define FONT_ASC46	0x00
/** @brief FONT_/ , not exist*/
#define FONT_ASC47	0x00

/** @brief FONT_0*/
#define FONT_0	0x3f
/** @brief FONT_1*/
#define FONT_1	0x06
/** @brief FONT_2*/
#define FONT_2	0x5b
/** @brief FONT_3*/
#define FONT_3	0x4f
/** @brief FONT_4*/
#define FONT_4	0x66
/** @brief FONT_5*/
#define FONT_5	0x6d
/** @brief FONT_6*/
#define FONT_6	0x7d
/** @brief FONT_7*/
#define FONT_7	0x27
/** @brief FONT_8*/
#define FONT_8	0x7f
/** @brief FONT_9*/
#define FONT_9	0x6f

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
#define FONT_A   0x77
/** @brief FONT_B*/
#define FONT_B   0x7c
/** @brief FONT_C*/
#define FONT_C   0x39
/** @brief FONT_D*/
#define FONT_D   0x5e
/** @brief FONT_E*/
#define FONT_E   0x79
/** @brief FONT_F*/
#define FONT_F   0x71
/** @brief FONT_G*/
#define FONT_G   0x6f
/** @brief FONT_H*/
#define FONT_H   0x76
/** @brief FONT_I*/
#define FONT_I   0x04
/** @brief FONT_J*/
#define FONT_J   0x1E
/** @brief FONT_K*/
#define FONT_K   0x70
/** @brief FONT_L*/
#define FONT_L   0x38
/** @brief FONT_M*/
#define FONT_M   0x55
/** @brief FONT_N*/
#define FONT_N   0x54
/** @brief FONT_O*/
#define FONT_O   0x5C
/** @brief FONT_P*/
#define FONT_P   0x73
/** @brief FONT_Q*/
#define FONT_Q   0x67
/** @brief FONT_R*/
#define FONT_R   0x50
/** @brief FONT_S*/
#define FONT_S   0x6D
/** @brief FONT_T*/
#define FONT_T   0x78
/** @brief FONT_U*/
#define FONT_U   0x3E
/** @brief FONT_V*/
#define FONT_V   0x1C
/** @brief FONT_W*/
#define FONT_W   0x1D
/** @brief FONT_X*/
#define FONT_X   0x49
/** @brief FONT_Y*/
#define FONT_Y   0x6E
/** @brief FONT_Z*/
#define FONT_Z   0x5B

/** @brief FONT__*/
#define FONT_ASC91	0x00
/** @brief FONT__*/
#define FONT_ASC92	0x00
/** @brief FONT__*/
#define FONT_ASC93	0x00
/** @brief FONT__*/
#define FONT_ASC94	0x00
/** @brief FONT__*/
#define FONT_ASC95	0x08

/** @brief DOT_MASK*/
#define DISP_DRV_DOT_MASK		0x80
/** @brief Character_MASK*/
#define DISP_DRV_CHAR_MASK	0x7F

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
const INT8U code FontTable[64] =
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
    FONT_V,		FONT_W,		FONT_X,		FONT_Y,		FONT_Z,		FONT_ASC91, FONT_ASC92, 
    FONT_ASC93,	FONT_ASC94, FONT_ASC95	//  65 -95  alphabet & underbar
};

/** @brief Display Driver에서 Segment Display시 사용하는 struct Window 개수 만큼 존재 */
static DISP_DRV_DATA idata dispDrvStruct[DISP_DRV_WIN_MAX_NUM];
/** @brief Image Buffer : 실제 Display Driver IC에 존재하는 메모리의 복사본 */
static INT8U idata dispDrvBuf[DISP_DRV_PAGE_SIZE][DISP_DRV_BUF_SIZE];
/** @brief Display Driver Mode */
static INT8U idata dispDrvMode;
/** @brief Display Driver IC에 보낼 Image Buffer 번호 */
static INT8U idata dispTransPage;
/** @brief Display Driver에서 Edit시 사용하는 Image Buffer 번호 */
static INT8U idata dispEditingPage;

/** 
* @brief Display Driver에서 Indicator Display시 사용하는 struct, Indicator 개수 만큼 존재 
* @remark 배열에서 Indicator위치는 define되어 있음\n
*         임의로 위치 변경하지 마시오.
*         관련 정의를 보려면 DISP_DRV_ADDR_WEIGHT_MINUS를 찾아 보시오.
*/
	const INDICATOR_ADDR code dispDrvIndicatorAddrTable[INDICATOR_MAX_NUM] =
	{
		{0, 0}, 						///0 Weight Minus
		{0, 0}, 						///1 Unit price Minus
		{0, 0}, 						///2 Total price Minus
		{0, 0}, 						///3 Tare Minus
		DISP_DRV_ADDR_ZERO, 			///4 Zero
		{0, 0}, 						///5 Stable
		DISP_DRV_ADDR_NET,				///6 Net
		DISP_DRV_ADDR_LOW_BATT, 		///7 Low battery
		{0, 0}, 						///8 Ounce
		DISP_DRV_ADDR_LB,				///9 Pound
		DISP_DRV_ADDR_KG,				//10 Kilo gram
		{0, 0}, 						//11 Gram
		{0, 0}, 						//12 Counting mode
		DISP_DRV_ADDR_PREPACK, 			//13 Prepack
		{0, 0}, 						//14 Compare high
		{0, 0}, 						//15 Compare low
		{0, 0}, 						//16 Compare OK 
		DISP_DRV_ADDR_EURO, 			//17 Euro
		{0, 0}, 						//18 Hold
		{0, 0}, 						//19 Backlight
		{0, 0}, 						//20 Counting lack
		DISP_DRV_ADDR_UNITPRICE_100G,	//21 Unit price 100Gram
		DISP_DRV_ADDR_TARESAVE, 		//22 Tare save
		DISP_DRV_ADDR_BOTHSAVE, 		//23 Both save
		DISP_DRV_ADDR_UNITPRICE_KG, 	//24 Unit price Kilo Gram
		DISP_DRV_ADDR_UNITPRICE_LB, 	//25 Unit price Pound
		{0, 0}, 						//26 Percent
		{0, 0}, 						//27 Sum
		{0, 0}, 						//28 full battery
		{0, 0}, 						//29 charging battery
		DISP_DRV_ADDR_EURO_KG,			//30 Euro per Kilogram
		{0, 0}, 						//31 $
		{0, 0}, 						//32 $/kg
		{0, 0},							//33 Unit price oz
		{0, 0}							//34 Connected AC		
	};

//Comma addr and mask
//   0 ~ 17
//	{29, 0x08}, {29, 0x04}, {29, 0x02}, {29, 0x01}, {31, 0x01}
//	{13, 0x08}, {13, 0x04}, {13, 0x02}, {13, 0x01}, {15, 0x01}
//	{13, 0x08}, {13, 0x04}, {13, 0x02}, {13, 0x01}, {15, 0x01}
//relative address per module
/** @brief Dot addr and mask*/
const INDICATOR_ADDR code dispDrvDotAddrTable[6] =
{
	{0, 0x80}, {1, 0x80}, {2, 0x80}, {3, 0x80}, {4, 0x80}, {5, 0x80} 
};

/** @brief s0~s39 중 디지트 시작 주소*/
//INT8U code dispDrvCharAddrTable[5] = { 1, 2, 3, 4, 5};
//INT8U code dispDrvCharAddrTable[5] = { 5, 4, 3, 2, 1};
//INT8U code dispDrvCharAddrTable[6] = { 0, 1, 2, 3, 4, 5};


//Battery addr and mask
//  if total disp window, 0 = battery outline, 1~4 = energy amount
//	{25, 0x04}, {25, 0x01}, {25, 0x02}, {26, 0x01}, {26, 0x02}
/*
const INDICATOR_ADDR code dispDrvBattAddrTable[5] =
{
	{25, 0x04}, {26, 0x02}, {26, 0x01}, {25, 0x02}, {25, 0x01}
};
*/
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
	6, 6, 8, 0
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
//static void dispDrvSetComma(INT8U winNum, INT8U decPos, INT8U on);
//static void dispDrvSetBattIndicator(INT8U num, INT8U on);
static void ht1632WriteCommand(INT8U datcom);
static void ht1632TranCommand(INT8U command);      
static void ht1632TranData(INT8U disdata);
static void ht1632WriteAllData();         

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

	ht1632WriteCommand(HT1632_ON);	
	ht1632WriteCommand(HT1632_SYS_EN); /* Turn on system oscillator*/	
	ht1632WriteCommand(HT1632_NMOS_8COM);
	DispDrvClearBuf();
	ht1632WriteAllData();

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
	INT8U addr;
	INT8U tempByte;

	addr = dispDrvStruct[winNum].addr;

	addr += pos;		
	
	tempByte = dispDrvBuf[dispEditingPage][addr];//DOT살리기
	tempByte &= (~DISP_DRV_CHAR_MASK);
	dispDrvBuf[dispEditingPage][addr] = (FontTable[cdata - 0x20] & DISP_DRV_CHAR_MASK) | tempByte;
}

/**
********************************************************************************
* @brief    Editting Image에 입력 받은 문자로 채움
* @param  fillChar : 채울 문자
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
//			dispDrvSetComma(winNum, pos, on);
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
	INT8U tempByte;

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
				
		tempByte = dispDrvBuf[dispEditingPage][addr];//DOT살리기
		tempByte &= (~DISP_DRV_CHAR_MASK);
		dispDrvBuf[dispEditingPage][addr + i] = (FontTable[dispChar - 0x20] & DISP_DRV_CHAR_MASK) | tempByte; 
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
	INT8U tempByte;

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
				
		tempByte = dispDrvBuf[dispEditingPage][addr];//DOT살리기
		tempByte &= (~DISP_DRV_CHAR_MASK);
		dispDrvBuf[dispEditingPage][addr + i] = (FontTable[dispChar - 0x20] & DISP_DRV_CHAR_MASK) | tempByte; 
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
//		dispDrvSetComma(winNum, i, OFF);
	}
	if (decPos != 0)
	{
		dispDrvSetDot(winNum, decPos, ON);
	/*	if (decType == DEC_TYPE_COMMA)
		{
//			dispDrvSetComma(winNum, decPos, ON);
		}
		*/
	}
	if ((useThousandPt == 1) && ((decPos + 3) < size))
	{
		dispDrvSetDot(winNum, 3 + decPos, ON);
//dispDrvSetComma(winNum, 3 + decPos, ON);
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
	switch (percent)
	{
		default:
		case 100:
			/*
			dispDrvSetBattIndicator(4, ON);
			dispDrvSetBattIndicator(3, ON);
			dispDrvSetBattIndicator(2, ON);
			dispDrvSetBattIndicator(1, ON);
			dispDrvSetBattIndicator(0, ON);
			*/
			break;
		case 50:
			/*
			dispDrvSetBattIndicator(4, OFF);
			dispDrvSetBattIndicator(3, ON);
			dispDrvSetBattIndicator(2, ON);
			dispDrvSetBattIndicator(1, ON);
			dispDrvSetBattIndicator(0, ON);
			*/
			break;
		case 20:
			/*
			dispDrvSetBattIndicator(4, OFF);
			dispDrvSetBattIndicator(3, OFF);
			dispDrvSetBattIndicator(2, ON);
			dispDrvSetBattIndicator(1, ON);
			dispDrvSetBattIndicator(0, ON);
			*/
			break;
		case 10:
			/*
			dispDrvSetBattIndicator(4, OFF);
			dispDrvSetBattIndicator(3, OFF);
			dispDrvSetBattIndicator(2, OFF);
			dispDrvSetBattIndicator(1, ON);
			dispDrvSetBattIndicator(0, ON);
			*/
			break;
		case 0:
			/*
			dispDrvSetBattIndicator(4, OFF);
			dispDrvSetBattIndicator(3, OFF);
			dispDrvSetBattIndicator(2, OFF);
			dispDrvSetBattIndicator(1, OFF);
			dispDrvSetBattIndicator(0, ON);
			*/
			break;
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
//	ht1632WriteCommand(HT1632_ON);	
//	ht1632WriteCommand(HT1632_SYS_EN); /* Turn on system oscillator*/	
//	ht1632WriteCommand(HT1632_NMOS_8COM);
	ht1632WriteAllData();
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
		ht1632TranCommand(HT1632_ON);
	}
	else
	{
		ht1632TranCommand(HT1632_OFF);
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
	INT8U mask;
	INT8U pos;

	addr = dispDrvStruct[winNum].addr;
	pos = dispDrvStruct[winNum].size - decPos - 1;

	addr += pos;

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
/*
void dispDrvSetComma(INT8U winNum, INT8U decPos, INT8U on)
{
	INT8U addr;
	INT8U mask;
	INT8U pos;

	addr = dispDrvStruct[winNum].addr;
	pos = dispDrvStruct[winNum].size - decPos - 1;
	

	if (on)
	{
		if (winNum == WIN_UNIT_PRICE)
		{
			addr += dispDrvCommaAddrTable[pos].addr;	
			mask = dispDrvCommaAddrTable[pos].mask;
			mask <<= 4;
		}
		else if(winNum == WIN_TOTAL_PRICE)
		{
			addr += dispDrvCommaAddrTableForTotal[pos].addr;
			mask = dispDrvCommaAddrTableForTotal[pos].mask;
		}
		else
		{
			addr += dispDrvCommaAddrTable[pos].addr;	
			mask = dispDrvCommaAddrTable[pos].mask;
		}
		dispDrvBuf[dispEditingPage][addr] = dispDrvBuf[dispEditingPage][addr] | mask;
	}
	else
	{
		if (winNum == WIN_UNIT_PRICE)
		{
			addr += dispDrvCommaAddrTable[pos].addr;	
			mask = dispDrvCommaAddrTable[pos].mask;
			mask <<= 4;
		}
		else if(winNum == WIN_TOTAL_PRICE)
		{
			addr += dispDrvCommaAddrTableForTotal[pos].addr;
			mask = dispDrvCommaAddrTableForTotal[pos].mask;
		}
		else
		{
			addr += dispDrvCommaAddrTable[pos].addr;	
			mask = dispDrvCommaAddrTable[pos].mask;
		}
		dispDrvBuf[dispEditingPage][addr] = (dispDrvBuf[dispEditingPage][addr] & (~mask));
	}
	
}
*/

/**
********************************************************************************
* @brief    Bettery Energy Indicator setting 함수 (editting Page)
* @param    num : Battery Indicator Number \n
* @return   none
* @remark   
********************************************************************************
*/
/*
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
*/
/**
********************************************************************************
* @brief    HT1622 IC에 command 전송 함수
* @param    datcom : command\n
* @return   none
* @remark   
********************************************************************************
*/
void ht1632WriteCommand(INT8U datcom)         
{
//	IEN0 = 0x0a;		// Disable all, Enable T1,0 Timer overflow

	LED_CS = 0;

	LED_WR = 0;	
	LED_DATA = 1;
	delay2Clock();
	LED_WR = 1;  ///1
	delay2Clock();
	
	LED_WR = 0;
	LED_DATA = 0;
	delay2Clock();
	LED_WR = 1;  //0
	delay2Clock();

	LED_WR = 0;
	LED_DATA = 0;
	delay2Clock();
	LED_WR = 1;  //0
	delay2Clock();

	ht1632TranCommand(datcom);

	LED_WR = 0;
	delay2Clock();
	LED_WR = 1;  
	delay2Clock();
	LED_CS = 1;
	
//	IEN0 = 0x8a;		// Enable all, Enable T1,0 Timer overflow
}

/**
********************************************************************************
* @brief    HT1622 IC에 command byte 전송 함수
* @param    command : command\n
* @return   none
* @remark   
********************************************************************************
*/
void ht1632TranCommand(INT8U command)      
{
    INT8U k;
    
	for (k = 0; k < 8; k++)		
	{
		LED_WR = 0;
		//Send most significant bit first 
		if ((command & 0x80) == 0)
		{
			LED_DATA = 0;
		}
		else
		{
			LED_DATA = 1;
		}
		delay2Clock();
		LED_WR = 1;   //Data are latched into the HT1622
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
void ht1632TranData(INT8U disdata)       
{
	INT8U k;

	for (k = 0; k < 8; k++)
	{
		LED_WR = 0;

		LED_DATA = disdata & 0x01;

		LED_WR = 1;    
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
void ht1632WriteAllData()         
{
	INT8U i;
	INT8U *dispBufPtr;

//	IEN0 = 0x0a;		// Disable all, Enable T1,0 Timer overflow

	LED_CS = 0;
	LED_WR = 0;
	delay2Clock();

	LED_DATA = 1;
	delay2Clock();

	LED_WR = 1;
	delay2Clock();
	
	LED_WR = 0;
	delay2Clock();

	LED_DATA = 0;
	delay2Clock();

	LED_WR = 1;
	delay2Clock();

	ht1632TranCommand(0x80); //Write from memory address 0

	dispBufPtr = &dispDrvBuf[dispTransPage][0];
	for (i = 0; i < DISP_DRV_BUF_SIZE; i++)
	{   
		ht1632TranData(*dispBufPtr);
		dispBufPtr++;
	}
	LED_CS = 1;
	
//	IEN0 = 0x8a;        // Enable all, Enable T1,0 Timer overflow
}
#endif//#ifdef DISP_DRV_HT1632_M0516LDN_ER_LED

