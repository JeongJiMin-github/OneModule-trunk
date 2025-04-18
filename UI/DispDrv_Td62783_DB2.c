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
#ifdef DISP_DRV_TD62783_DB2
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
#define DISP_DRV_WIN_MAX_NUM	1

/** @brief Display driver buffer size*/
//#define DISP_DRV_BUF_SIZE	6

/** @brief Display driver module size*/
#define DSP_DRV_MODULE_SIZE	9
/** @brief Display driver module string size*/
#define DSP_DRV_MODULE_STR_SIZE	6

/** @brief Display driver weight address*/
#define DISP_DRV_WEIGHT_ADDR 1
/** @brief weight display size*/
//#define DISP_DRV_WEIGHT_SIZE 5
/** @brief weight display indicator address*/
//#define DISP_DRV_WEIGHT_INDICATOR_ADDR 6

/** @brief Stable indicator*/
//#define DISP_DRV_ADDR_STABLE	             {0, 0x20}		//F
/** @brief NET indicator*/
//#define DISP_DRV_ADDR_NET				{0, 0x02}	//B
/** @brief Low Battery indicator*/
//#define DISP_DRV_ADDR_LOW_BATT	     	{0, 0x04}	//C
/** @brief PCS indicator, for counting*/
//#define DISP_DRV_ADDR_PCS				{0, 0x10}	//E
/** @brief Zero indicator*/
//#define DISP_DRV_ADDR_ZERO		     	{0, 0x80}	//DP
/** @brief High indicator, for comparator*/
//#define DISP_DRV_ADDR_HOLD			{0, 0x08}	//D
/** @brief Low indicator, for comparator*/
//#define DISP_DRV_ADDR_LOW			{0, 0x40}	//G

#ifdef USE_WEIGHT_5DIGIT
/** @brief Display driver buffer size*/
#define DISP_DRV_BUF_SIZE			6
/** @brief weight display size*/
#define DISP_DRV_WEIGHT_SIZE 		5

/** @brief MINUS indicator, RB is minus sign is not exist, so use digit 0 for minus char*/
//#define DISP_DRV_ADDR_WEIGHT_MINUS	{0, 0x02}	
/** @brief Stable indicator*/
#define DISP_DRV_ADDR_STABLE			{0, 0x10}	//INDICATOR1, SEG_E OK
/** @brief Zero indicator*/
#define DISP_DRV_ADDR_ZERO			{0, 0x02}	//INDICATOR1, SEG_B OK
/** @brief NET indicator*/
#define DISP_DRV_ADDR_NET				{0, 0x08}	//INDICATOR1, SEG_D OK
/** @brief SUM indicator*/
#define DISP_DRV_ADDR_SUM				{0, 0x04}	//INDICATOR2, SEG_C OK
/** @brief Low Battery indicator*/
#define DISP_DRV_ADDR_LOW_BATT		{0, 0x01}	//INDICATOR2, SEG_A OK
#else
/** @brief Display driver buffer size*/
#define DISP_DRV_BUF_SIZE			8
/** @brief weight display size*/
#define DISP_DRV_WEIGHT_SIZE 		6

/** @brief MINUS indicator, RB is minus sign is not exist, so use digit 0 for minus char*/
//#define DISP_DRV_ADDR_WEIGHT_MINUS	{0, 0x02}	
/** @brief Stable indicator*/
#define DISP_DRV_ADDR_STABLE			{6, 0x40}	//INDICATOR1, SEG_A
/** @brief Zero indicator*/
#define DISP_DRV_ADDR_ZERO			{6, 0x20}	//INDICATOR1, SEG_B
/** @brief NET indicator*/
#define DISP_DRV_ADDR_NET				{6, 0x10}	//INDICATOR1, SEG_C
/** @brief High indicator, for comparator*/
#define DISP_DRV_ADDR_HI				{6, 0x08}	//INDICATOR1, SEG_D
/** @brief OK indicator, for comparator*/
#define DISP_DRV_ADDR_OK				{6, 0x04}	//INDICATOR1, SEG_E
/** @brief Low indicator, for comparator*/
#define DISP_DRV_ADDR_LOW				{6, 0x02}	//INDICATOR1, SEG_F
/** @brief PCS indicator, for counting*/
#define DISP_DRV_ADDR_PCS				{6, 0x01}	//INDICATOR1, SEG_G
/** @brief PERCENT indicator*/
#define DISP_DRV_ADDR_PERCENT			{6, 0x80}	//INDICATOR1, SEG_DP
/** @brief SUM indicator*/
#define DISP_DRV_ADDR_SUM				{7, 0x40}	//INDICATOR2, SEG_A					 
/** @brief Kilo gram indicator*/
#define DISP_DRV_ADDR_KG				{7, 0x20}	//INDICATOR2, SEG_B
/** @brief Pound indicator*/
#define DISP_DRV_ADDR_LB				{7, 0x10}	//INDICATOR2, SEG_C
/** @brief Gram indicator*/
#define DISP_DRV_ADDR_GRAM			{7, 0x08}	//INDICATOR2, SEG_D
/** @brief Low Battery indicator*/
#define DISP_DRV_ADDR_LOW_BATT		{7, 0x04}	//INDICATOR2, SEG_E
#endif

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

/** @brief TRIANGLE_MASK*/
#define DISP_DRV_TRIANGLE_MASK	0x80
/** @brief DOT_MASK*/
#define DISP_DRV_DOT_MASK			0x80
/** @brief CHAR_MASK*/
//#define DISP_DRV_CHAR_MASK		0x7F

/** @brief TRIANGLE_MASK*/
#define DISP_DIGIT_INDICATOR	0
/** @brief TRIANGLE_MASK*/
#define DISP_DIGIT_1			1
/** @brief TRIANGLE_MASK*/
#define DISP_DIGIT_2			2
/** @brief TRIANGLE_MASK*/
#define DISP_DIGIT_3			3
/** @brief TRIANGLE_MASK*/
#define DISP_DIGIT_4			4
/** @brief TRIANGLE_MASK*/
#define DISP_DIGIT_5			5



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
static DISP_DRV_DATA dispDrvStruct[WIN_MAX_NUM];
/** @brief Image Buffer : 실제 Display Driver IC에 존재하는 메모리의 복사본 */
static INT8U idata dispDrvBuf[DISP_DRV_PAGE_SIZE][DISP_DRV_BUF_SIZE];
/** @brief Image Buffer : 실제 Display Driver IC에 존재하는 메모리의 복사본 */
static INT8U idata dispDrvBufForInterrupt[DISP_DRV_BUF_SIZE];
/** @brief Display Driver Mode */
static INT8U dispDrvMode;
/** @brief Display Driver IC에 보낼 Image Buffer 번호 */
static INT8U dispTransPage;
/** @brief Display Driver에서 Edit시 사용하는 Image Buffer 번호 */
static INT8U dispEditingPage;

/** @brief LED digit count for interrupt*/
static INT8U digitCount;
/** @brief LED digit count for interrupt*/
static BOOLEAN dispDrvTurnOnOff;

#ifdef USE_WEIGHT_5DIGIT
/** 
* @brief Display Driver에서 Indicator Display시 사용하는 struct, Indicator 개수 만큼 존재 
* @remark 배열에서 Indicator위치는 define되어 있음\n
*         임의로 위치 변경하지 마시오.
*         관련 정의를 보려면 DISP_DRV_ADDR_WEIGHT_MINUS를 찾아 보시오.
*/
const INDICATOR_ADDR code dispDrvIndicatorAddrTable[INDICATOR_MAX_NUM] =
{
	{0, 0},								///0 Weight Minus
	{0, 0},								///1 Unit price Minus
	{0, 0},								///2 Total price Minus
	{0, 0},								///3 Tare Minus
	DISP_DRV_ADDR_ZERO,					///4 Zero
	DISP_DRV_ADDR_STABLE,				///5 Stable
	DISP_DRV_ADDR_NET,					///6 Net
	DISP_DRV_ADDR_LOW_BATT,				///7 Low battery
	{0, 0},								///8 Ounce
	{0, 0},								///9 Pound
	{0, 0},								//10 Kilo gram
	{0, 0},								//11 Gram 
	{0, 0},								//12 Counting mode
	{0, 0},								//13 Prepack
	{0, 0},								//14 Compare high
	{0, 0},								//15 Compare low
	{0, 0},								//16 Compare OK
	{0, 0},								//17 Euro
	{0, 0},								//18 Hold
	{0, 0},								//19 Backlight
	{0, 0},								//20 Counting lack
	{0, 0},								//21 Unit price 100Gram
	{0, 0},								//22 Tare save
	{0, 0},								//23 Both save
	{0, 0},								//24 Unit price Kilo Gram
	{0, 0},								//25 Unit price Pound
	{0, 0},								//26 Percent
	DISP_DRV_ADDR_SUM,					//27 Sum
	{0, 0},								//28 full battery
	{0, 0},								//29 charging battery
	{0, 0},								//30 Euro per Kilogram
	{0, 0}, 							//31 $
	{0, 0}, 							//32 $/kg
	{0, 0}, 							//33 Unit price oz
	{0, 0}								//34 Connected AC
};		

/** @brief Dot addr and mask*/
const INDICATOR_ADDR code dispDrvDotAddrTable[5] =
{
	//{4, 0x80}, {3, 0x80}, {2, 0x80}, {1, 0x80}, {0, 0x80}
	{1, 0x80}, {2, 0x80}, {3, 0x80}, {4, 0x80}, {5, 0x80}
};

/** @brief s0~s39 중 디지트 시작 주소*/
INT8U code dispDrvCharAddrTable[5] = { 1, 2, 3, 4, 5};
//INT8U code dispDrvCharAddrTable[5] = { 5, 4, 3, 2, 1};

/** 
* @brief Menu Mode에서 사용될 Window 번호 Table
* @remark String Window, Aux. Data window, Data Window 순서임 (see DISP_MENU_STR_WIN_NUM~)\n
*         String Window와 Data Window가 같으면 Menu Mode에서 Window가 1개인 것으로 인식함\n
*         Window가 1개일 경우 반드시 같게 할 것!!!
*/
const INT8U code dispDrvWinTableForMenu[DISP_MAX_WIN_NUM_FOR_MENU] = 
{
	WIN_WEIGHT,	WIN_UNIT_PRICE, WIN_WEIGHT
};

/** 
* @brief 각 Window의 Size Table
* @remark WIN_WEIGHT, WIN_UNIT_PRICE, WIN_TOTAL_PRICE, WIN_TARE 순서임 (see WIN_WEIGHT~)
*/
const INT8U code dispDrvWinSizeTable[WIN_MAX_NUM] = 
{
	5, 0, 0, 0
};

/**
* @brief 각 Window의 '-' sign 유무 Table 
* @remark WIN_WEIGHT, WIN_UNIT_PRICE, WIN_TOTAL_PRICE, WIN_TARE 순서임 (see WIN_WEIGHT~)
*/
const INT8U code dispDrvMinusSignExistTable[WIN_MAX_NUM] = 
{
	0, 0, 0, 0
};
#else
/** 
* @brief Display Driver에서 Indicator Display시 사용하는 struct, Indicator 개수 만큼 존재 
* @remark 배열에서 Indicator위치는 define되어 있음\n
*         임의로 위치 변경하지 마시오.
*         관련 정의를 보려면 DISP_DRV_ADDR_WEIGHT_MINUS를 찾아 보시오.
*/
const INDICATOR_ADDR code dispDrvIndicatorAddrTable[INDICATOR_MAX_NUM] =
{
	{0, 0},								///0	Weight Minus
	{0, 0},								///1 Unit price Minus
	{0, 0},								///2 Total price Minus
	{0, 0},								///3 Tare Minus
	DISP_DRV_ADDR_ZERO,				///4 Zero
	DISP_DRV_ADDR_STABLE,				///5 Stable
	DISP_DRV_ADDR_NET,					///6 Net
	DISP_DRV_ADDR_LOW_BATT,			///7 Low battery
	{0, 0},								///8 Ounce
	DISP_DRV_ADDR_LB,					///9 Pound
	DISP_DRV_ADDR_KG,					//10 Kilo gram
	DISP_DRV_ADDR_GRAM,				//11 Gram 
	DISP_DRV_ADDR_PCS,					//12 Counting mode
	{0, 0},								//13 Prepack
	DISP_DRV_ADDR_HI,					//14 Compare high
	DISP_DRV_ADDR_LOW,					//15 Compare low
	DISP_DRV_ADDR_OK,					//16 Compare OK
	{0, 0},								//17 Euro
	{0, 0},								//18 Hold
	{0, 0},								//19 Backlight
	{0, 0},								//20 Counting lack
	{0, 0},								//21 Unit price 100Gram
	{0, 0},								//22 Tare save
	{0, 0},								//23 Both save
	{0, 0},								//24 Unit price Kilo Gram
	{0, 0},								//25 Unit price Pound
	DISP_DRV_ADDR_PERCENT,				//26 Percent
	DISP_DRV_ADDR_SUM,					//27 Sum
	{0, 0},								//28 full battery
	{0, 0},								//29 charging battery
	{0, 0},								//30 Euro per Kilogram
	{0, 0},								//31
	{0, 0},								//32
	{0, 0}								//33
};		

/** @brief Dot addr and mask*/
const INDICATOR_ADDR code dispDrvDotAddrTable[6] =
{
	{5, 0x80}, {4, 0x80}, {3, 0x80}, {2, 0x80}, {1, 0x80}, {0, 0}
};

/** 
* @brief Menu Mode에서 사용될 Window 번호 Table
* @remark String Window, Aux. Data window, Data Window 순서임 (see DISP_MENU_STR_WIN_NUM~)\n
*         String Window와 Data Window가 같으면 Menu Mode에서 Window가 1개인 것으로 인식함\n
*         Window가 1개일 경우 반드시 같게 할 것!!!
*/
const INT8U code dispDrvWinTableForMenu[DISP_MAX_WIN_NUM_FOR_MENU] = 
{
	WIN_WEIGHT,	WIN_UNIT_PRICE, WIN_WEIGHT
};

/**
* @brief 각 Window의 Size Table
* @remark WIN_WEIGHT, WIN_UNIT_PRICE, WIN_TOTAL_PRICE, WIN_TARE 순서임 (see WIN_WEIGHT~)
*/
const INT8U code dispDrvWinSizeTable[WIN_MAX_NUM] = 
{
	6, 0, 0, 0
};

/**
* @brief 각 Window의 '-' sign 유무 Table 
* @remark WIN_WEIGHT, WIN_UNIT_PRICE, WIN_TOTAL_PRICE, WIN_TARE 순서임 (see WIN_WEIGHT~)
*/
const INT8U code dispDrvMinusSignExistTable[WIN_MAX_NUM] = 
{
	0, 0, 0, 0
};
#endif

/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
static void dispDrvSetDot(INT8U winNum, INT8U decPos, INT8U on);

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
	digitCount = 0;
	
	DispDrvClearBuf();
	DispDrvSetMode(DISP_DRV_NORMAL_MODE);
	DispDrvSetPage(DISP_DRV_NORMAL_PAGE);

	dispDrvStruct[WIN_WEIGHT].addr = DISP_DRV_WEIGHT_ADDR;
	dispDrvStruct[WIN_WEIGHT].size = DISP_DRV_WEIGHT_SIZE;

	dispDrvStruct[WIN_UNIT_PRICE].addr = 0;
	dispDrvStruct[WIN_UNIT_PRICE].size = 0;

	dispDrvStruct[WIN_TOTAL_PRICE].addr = 0;
	dispDrvStruct[WIN_TOTAL_PRICE].size = 0;

	dispDrvStruct[WIN_TARE].addr = 0;
	dispDrvStruct[WIN_TARE].size = 0;

	DispDrvClearBuf();
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
//	INT8U mask;

	addr = dispDrvCharAddrTable[pos];
	byte = FontTable[cdata - 0x20];
	dispDrvBuf[dispEditingPage][addr] = byte;
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
//	INT8U mask;

	if (winNum > DISP_DRV_WIN_MAX_NUM - 1) 
	{
		return;
	}

	strEndFlag = 0;

	dataSize = dispDrvStruct[winNum].size;

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
		addr = dispDrvCharAddrTable[i];
		byte = FontTable[dispChar - 0x20];
		dispDrvBuf[dispEditingPage][addr] = byte;
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
//	INT8U mask;

	if (winNum > DISP_DRV_WIN_MAX_NUM - 1)
	{
		return;
	}
	strEndFlag = 0;
	dataSize = dispDrvStruct[winNum].size;

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
		addr = dispDrvCharAddrTable[i];
		byte = FontTable[dispChar - 0x20];
		dispDrvBuf[dispEditingPage][addr] = byte;
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
	
	size = dispDrvStruct[winNum].size;	//5
	
	for (i = 0; i < size; i++)
	{
		dispDrvSetDot(winNum, i, OFF);
	}
	if (decPos != 0)
	{
		dispDrvSetDot(winNum, decPos, ON);
	}
	if ((useThousandPt == 1) && ((decPos + 3) < size))
	{
		dispDrvSetDot(winNum, 3 + decPos, ON);
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
	INT8U prevOnNum;

	prevOnNum = 0;
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

	if (prevOnNum != 0)
	{
		addr = dispDrvIndicatorAddrTable[prevOnNum].addr;
		mask = dispDrvIndicatorAddrTable[prevOnNum].mask;	
	
		dispDrvBuf[dispEditingPage][addr] = dispDrvBuf[dispEditingPage][addr] | mask;//on			
	}
}

/**
********************************************************************************
* @brief    Bettery Energy Indicator setting 함수 (editting Page)
* @param    percent : Battery 양 (%)\n
* @return   none
* @remark   SW don't use
********************************************************************************
*/
void DispDrvSetBattEnergy(INT8U percent)
{

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
	INT8U i;

	if(dispDrvTurnOnOff)
	{
		for (i = 0; i < DISP_DRV_BUF_SIZE; i++)
		{
			dispDrvBufForInterrupt[i] = dispDrvBuf[dispTransPage][i];
		}
	}
	else
	{
		for (i = 0; i < DISP_DRV_BUF_SIZE; i++)
		{
			dispDrvBufForInterrupt[i] = 0;
		}
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
void DispDrvProcForInterrupt(void)
{
   	COM_CON = 0;
	DIGIT_CON = 0;
	LED_INDICATOR = OFF;
	LED_SEG_DIGIT_1 = OFF;
	LED_SEG_DIGIT_2 = OFF;
	LED_SEG_DIGIT_3 = OFF;	
	LED_SEG_DIGIT_4 = OFF;
	LED_SEG_DIGIT_5 = OFF;

	DIGIT_CON = 1;
	LED_SEG_DATA = dispDrvBufForInterrupt[digitCount];
	DIGIT_CON = 0;
	LED_SEG_DATA = 0x00;

	COM_CON = 1;
	switch (digitCount)
	{
		case DISP_DIGIT_INDICATOR: 
			LED_INDICATOR = ON;
			break;			
		case DISP_DIGIT_1: 
			LED_SEG_DIGIT_1 = ON;
			break;			
		case DISP_DIGIT_2: 
			LED_SEG_DIGIT_2 = ON;
			break;
		case DISP_DIGIT_3: 
			LED_SEG_DIGIT_3 = ON;
			break;
		case DISP_DIGIT_4: 
			LED_SEG_DIGIT_4 = ON;
			break;
		case DISP_DIGIT_5: 
			LED_SEG_DIGIT_5 = ON;
			break;			
		default:
			break;
	} 
	COM_CON = 0;
	
	if(digitCount < DISP_DRV_BUF_SIZE - 1)
	{
		digitCount++;
	}
	else
	{
		digitCount = 0;
	}
}

void DispDrvTurnOnOff (BOOLEAN onOff)
{
	if(onOff)
	{
		dispDrvTurnOnOff = ON;
	}
	else
	{
		dispDrvTurnOnOff = OFF;
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

	pos = dispDrvStruct[winNum].size - decPos - 1;	//5 - 3 - 1 = 1

	addr = dispDrvDotAddrTable[pos].addr;
	mask = dispDrvDotAddrTable[pos].mask;

	if (on)
	{
		dispDrvBuf[dispEditingPage][addr] = dispDrvBuf[dispEditingPage][addr] | mask;
	}
	else
	{
		dispDrvBuf[dispEditingPage][addr] = dispDrvBuf[dispEditingPage][addr] & (~mask);
	}
}
#endif//#ifdef DISP_DRV_TD62783


