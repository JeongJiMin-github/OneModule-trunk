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
#ifdef DISP_DRV_AS1106_DB2
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
#define DISP_DRV_WIN_MAX_NUM		1

//#define DISP_DRV_BUF_SIZE	27

/** @brief Display driver module size*/
#define DSP_DRV_MODULE_SIZE		9
/** @brief Display driver module string size*/
#define DSP_DRV_MODULE_STR_SIZE	6

/** @brief Display driver weight address*/
#define DISP_DRV_WEIGHT_ADDR 		0

#ifdef USE_WEIGHT_5DIGIT
/** @brief Display driver buffer size*/
#define DISP_DRV_BUF_SIZE			7

/** @brief weight display size*/
#define DISP_DRV_WEIGHT_SIZE 		5

/** @brief MINUS indicator, RB is minus sign is not exist, so use digit 0 for minus char*/
//#define DISP_DRV_ADDR_WEIGHT_MINUS	{0, 0x02}	
/** @brief Stable indicator*/
#define DISP_DRV_ADDR_STABLE			{6, 0x04}	//INDICATOR1, SEG_E
/** @brief Zero indicator*/
#define DISP_DRV_ADDR_ZERO			{6, 0x20}	//INDICATOR1, SEG_B
/** @brief NET indicator*/
#define DISP_DRV_ADDR_NET				{6, 0x08}	//INDICATOR1, SEG_D
/** @brief SUM indicator*/
#define DISP_DRV_ADDR_SUM				{6, 0x10}	//INDICATOR2, SEG_C
/** @brief Low Battery indicator*/
#define DISP_DRV_ADDR_LOW_BATT		{6, 0x40}	//INDICATOR2, SEG_A
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

/** @brief No operation*/
#define AS1106_NO_OP				0x00
/** @brief Digit 0 Address*/
#define AS1106_DIG0					0x01
/** @brief Digit 1 Address*/
#define AS1106_DIG1					0x02
/** @brief Digit 2 Address*/
#define AS1106_DIG2					0x03
/** @brief Digit 3 Address*/
#define AS1106_DIG3					0x04
/** @brief Digit 4 Address*/
#define AS1106_DIG4					0x05
/** @brief Digit 5 Address*/
#define AS1106_DIG5 				0x06
/** @brief Digit 6 Address*/
#define AS1106_DIG6					0x07
/** @brief Digit 7 Address*/
#define AS1106_DIG7					0x08
/** @brief Decode mode*/
#define AS1106_DECODE_MODE	 	0x09
/** @brief Intensity control*/
#define AS1106_INTENSITY_CONTROL	0x0A
/** @brief Sacn limit*/
#define AS1106_SCAN_LIMIT			0x0B
/** @brief Shut down*/
#define AS1106_SHUT_DOWN			0x0C
/** @brief Feature*/
#define AS1106_FEATURE				0x0E
/** @brief Display test*/
#define AS1106_DISPLAY_TEST		0x0F

/** @brief Shutdown Register(Shut Down Mode, Reset Feature Register to Default Setting)*/
#define AS1106_SHUT_DOWN_DEFAULT	0x00
/** @brief Shutdown Register(Shut Down Mode, Feature Register Unchanged)*/
#define AS1106_SHUT_DOWN_UNCHANGE	0x80
/** @brief Shutdown Register(Normal Mode, Reset Feature Register to Default Setting)*/
#define AS1106_NORMAL_DEFAULT			0x01
/** @brief Shutdown Register(Normal Mode, Feature Register Unchanged)*/
#define AS1106_NORMAL_UNCHANGE		0x81

/** @brief Shutdown Register(Shut Down Mode, Reset Feature Register to Default Setting)*/
#define AS1106_DECODE_NO_DECODE		0x00

/** @brief Display test ON*/
#define AS1106_DISPLAY_TEST_ON		0x01
/** @brief Display test OFF*/
#define AS1106_DISPLAY_TEST_OFF		0x00

/** @brief Intensity control 1/32(min)*/
#define AS1106_INTENSITY_CONTROL_1_32	0x00
/** @brief Intensity control 3/32*/
#define AS1106_INTENSITY_CONTROL_3_32	0x01
/** @brief Intensity control 5/32*/
#define AS1106_INTENSITY_CONTROL_5_32	0x02
/** @brief Intensity control 7/32*/
#define AS1106_INTENSITY_CONTROL_7_32	0x03
/** @brief Intensity control 9/32*/
#define AS1106_INTENSITY_CONTROL_9_32	0x04
/** @brief Intensity control 11/32*/
#define AS1106_INTENSITY_CONTROL_11_32	0x05
/** @brief Intensity control 13/32*/
#define AS1106_INTENSITY_CONTROL_13_32	0x06
/** @brief Intensity control 15/32*/
#define AS1106_INTENSITY_CONTROL_15_32	0x07
/** @brief Intensity control 17/32*/
#define AS1106_INTENSITY_CONTROL_17_32	0x08
/** @brief Intensity control 19/32*/
#define AS1106_INTENSITY_CONTROL_19_32	0x09
/** @brief Intensity control 21/32*/
#define AS1106_INTENSITY_CONTROL_21_32	0x0A
/** @brief Intensity control 23/32*/
#define AS1106_INTENSITY_CONTROL_23_32	0x0B
/** @brief Intensity control 25/32*/
#define AS1106_INTENSITY_CONTROL_25_32	0x0C
/** @brief Intensity control 27/32*/
#define AS1106_INTENSITY_CONTROL_27_32	0x0D
/** @brief Intensity control 29/32*/
#define AS1106_INTENSITY_CONTROL_29_32	0x0E
/** @brief Intensity control 31/32(max)*/
#define AS1106_INTENSITY_CONTROL_31_32	0x0F

/** @brief Sacn limit(scan all)*/
#define AS1106_SCAN_ALL					0x07

/** @brief Sacn limit(feature default)*/
// FEATURE = [blink_start|sync|blink_freq_sel|blink_en|spi_en|decode_sel|reg_res|clk_en] 7..0
// blink_start 		= Start blinking with display enabled phase. When bit D4(blink_en) is set, bit D7 determines how blinking starts.
// sync 			= Synchronizes blinking on the rising edge of pin LOAD/CSN. The multiples and blink timing counter is cleared 
//                            on the rising edge of pin LOAD/CSN. By setting this bit in multiple AS1106/AS1107 devices, the blink timing 
//                            can be synchronized across all the devices.
// blink_freq_sel 	= Sets blink with low frequency (with the internal oscillator enabled), 
//                            0 = blink period 1 second(0.5s on, 0.5s off)
//                            1 = blink period 2 second(1s on, 1s off)
// blink_en 		= Enables blinking
//                            0 = disable
//                            1 = enable
// spi_en 		= Enables the SPI-compatible interface.
//                            0 = disable
//                            1 = enable
// decode_sel 	= Selects display decoding.
//                            0 = Code-B decoding
//                            1 = HEX decoding
// reg_res 		= Resets all control registers except the Feature Register.
//                            0 = Reset Disabled. Normal operation
//                            1 = All control registers are reset to default state identically after power up.
// clk_en 		= External clock active.
//                            0 = internal oscillator is used for system clock.
//                            1 = Pin CLK of the setial interface operates as system clock input.
#define AS1106_FEATURE_DEFAULT				0x00

/** @brief AS1106_START_COMMAND*/
#define AS1106_CMD_START 		0
/** @brief AS1106_END_COMMAND*/
#define AS1106_CMD_END 		1


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
#define FONT_ASC45	0x01
/** @brief FONT_. , not exist*/
#define FONT_ASC46	0x00
/** @brief FONT_/ , not exist*/
#define FONT_ASC47	0x00

/** @brief FONT_0*/
#define FONT_0	0x7e
/** @brief FONT_1*/
#define FONT_1	0x30
/** @brief FONT_2*/
#define FONT_2	0x6d
/** @brief FONT_3*/
#define FONT_3	0x79
/** @brief FONT_4*/
#define FONT_4	0x33
/** @brief FONT_5*/
#define FONT_5	0x5b
/** @brief FONT_6*/
#define FONT_6	0x5f
/** @brief FONT_7*/
#define FONT_7	0x72
/** @brief FONT_8*/
#define FONT_8	0x7f
/** @brief FONT_9*/
#define FONT_9	0x7b

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
#define FONT_B   0x1f
/** @brief FONT_C*/
#define FONT_C   0x4e
/** @brief FONT_D*/
#define FONT_D   0x3d
/** @brief FONT_E*/
#define FONT_E   0x4f
/** @brief FONT_F*/
#define FONT_F   0x47
/** @brief FONT_G*/
#define FONT_G   0x5e
/** @brief FONT_H*/
#define FONT_H   0x37
/** @brief FONT_I*/
#define FONT_I   0x10
/** @brief FONT_J*/
#define FONT_J   0x3c
/** @brief FONT_K*/
#define FONT_K   0x07
/** @brief FONT_L*/
#define FONT_L   0x0e
/** @brief FONT_M*/
#define FONT_M   0x55
/** @brief FONT_N*/
#define FONT_N   0x15
/** @brief FONT_O*/
#define FONT_O   0x1d
/** @brief FONT_P*/
#define FONT_P   0x67
/** @brief FONT_Q*/
#define FONT_Q   0x73
/** @brief FONT_R*/
#define FONT_R   0x05
/** @brief FONT_S*/
#define FONT_S   0x5b
/** @brief FONT_T*/
#define FONT_T   0x0f
/** @brief FONT_U*/
#define FONT_U   0x3e
/** @brief FONT_V*/
#define FONT_V   0x1c
/** @brief FONT_W*/
#define FONT_W   0x5c
/** @brief FONT_X*/
#define FONT_X   0x4b
/** @brief FONT_Y*/
#define FONT_Y   0x3b
/** @brief FONT_Z*/
#define FONT_Z   0x6d

/** @brief FONT__*/
#define FONT_ASC91	0x00
/** @brief FONT__*/
#define FONT_ASC92	0x00
/** @brief FONT__*/
#define FONT_ASC93	0x00
/** @brief FONT__*/
#define FONT_ASC94	0x00
/** @brief FONT__*/
#define FONT_ASC95	0x07

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
static DISP_DRV_DATA dispDrvStruct[WIN_MAX_NUM];
/** @brief Image Buffer : 실제 Display Driver IC에 존재하는 메모리의 복사본 */
static INT8U idata dispDrvBuf[DISP_DRV_PAGE_SIZE][DISP_DRV_BUF_SIZE];
/** @brief Display Driver Mode */
static INT8U dispDrvMode;
/** @brief Display Driver IC에 보낼 Image Buffer 번호 */
static INT8U dispTransPage;
/** @brief Display Driver에서 Edit시 사용하는 Image Buffer 번호 */
static INT8U dispEditingPage;

#ifdef USE_WEIGHT_5DIGIT
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
	{0, 0}, 							///9 Pound
	{0, 0}, 							//10 Kilo gram
	{0, 0}, 							//11 Gram
	{0, 0}, 							//12 Counting mode
	{0, 0}, 							//13 Prepack
	{0, 0}, 							//14 Compare high
	{0, 0}, 							//15 Compare low
	{0, 0}, 							//16 Compare OK 
	{0, 0}, 							//17 Euro
	{0, 0}, 							//18 Hold
	{0, 0}, 							//19 Backlight
	{0, 0}, 							//20 Counting lack
	{0, 0}, 							//21 Unit price 100Gram
	{0, 0}, 							//22 Tare save
	{0, 0}, 							//23 Both save
	{0, 0}, 							//24 Unit price Kilo Gram
	{0, 0}, 							//25 Unit price Pound
	{0, 0}, 							//26 Percent
	DISP_DRV_ADDR_SUM,					//27 Sum
	{0, 0}, 							//28 full battery
	{0, 0}, 							//29 charging battery
	{0, 0}, 							//30 Euro per Kilogram
	{0, 0}, 							//31 $
	{0, 0}, 							//32 $/kg
	{0, 0}, 							//33 Unit price oz
	{0, 0} 								//34 Connected AC
};

/** @brief Dot addr and mask*/
const INDICATOR_ADDR code dispDrvDotAddrTable[5] =
{
	{4, 0x80}, {3, 0x80}, {2, 0x80}, {1, 0x80}, {0, 0}
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
static void as1106TranCmdData(INT8U command, INT8U startFlag);      
static void as1106WriteAllData();         

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

	dispDrvStruct[WIN_UNIT_PRICE].addr = 0;
	dispDrvStruct[WIN_UNIT_PRICE].size = 0;

	dispDrvStruct[WIN_TOTAL_PRICE].addr = 0;
	dispDrvStruct[WIN_TOTAL_PRICE].size = 0;

	dispDrvStruct[WIN_TARE].addr = 0;
	dispDrvStruct[WIN_TARE].size = 0;

	as1106TranCmdData(AS1106_SHUT_DOWN, AS1106_CMD_START);
	as1106TranCmdData(AS1106_NORMAL_UNCHANGE, AS1106_CMD_END);
	
	as1106TranCmdData(AS1106_DECODE_MODE, AS1106_CMD_START);
	as1106TranCmdData(AS1106_DECODE_NO_DECODE, AS1106_CMD_END);

	DispDrvSetBright(brightness);
	
	as1106TranCmdData(AS1106_SCAN_LIMIT, AS1106_CMD_START);
	as1106TranCmdData(AS1106_SCAN_ALL, AS1106_CMD_END);

	as1106TranCmdData(AS1106_FEATURE, AS1106_CMD_START);
	as1106TranCmdData(AS1106_FEATURE_DEFAULT, AS1106_CMD_END);

	DispDrvClearBuf();
	as1106WriteAllData();
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
	INT8U mask;

	byte = FontTable[cdata - 0x20];
	dispDrvBuf[dispEditingPage][dispDrvStruct[winNum].size - pos - 1] = byte;
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
	INT8U mask;

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
		//addr = dispDrvCharAddrTable[i];
		byte = FontTable[dispChar - 0x20];
		dispDrvBuf[dispEditingPage][dataSize - i - 1] = byte;
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
	INT8U mask;

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
		//addr = dispDrvCharAddrTable[i];
		byte = FontTable[dispChar - 0x20];
		dispDrvBuf[dispEditingPage][dataSize - i - 1] = byte;
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
	}

	if (decPos == 0xff)//date/time
	{
		dispDrvSetDot(winNum, 2, ON);
		dispDrvSetDot(winNum, 4, ON);
		return;
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
	as1106WriteAllData();
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
	switch(level)
	{
		case 0:
			level = AS1106_INTENSITY_CONTROL_3_32;
			break;
		case 1:
			level = AS1106_INTENSITY_CONTROL_5_32;
			break;
		case 2:
			level = AS1106_INTENSITY_CONTROL_9_32;
			break;
		case 3:
		default:
			level = AS1106_INTENSITY_CONTROL_13_32;
			break;
		case 4:
			level = AS1106_INTENSITY_CONTROL_17_32;
			break;
		case 5:
			level = AS1106_INTENSITY_CONTROL_21_32;
			break;
		case 6:
			level = AS1106_INTENSITY_CONTROL_25_32;
			break;
		case 7:
			level = AS1106_INTENSITY_CONTROL_31_32;
			break;		
	}
       as1106TranCmdData(AS1106_INTENSITY_CONTROL, AS1106_CMD_START);
	as1106TranCmdData(level, AS1106_CMD_END);
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

	pos = dispDrvStruct[winNum].size - decPos - 1;

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

/**
********************************************************************************
* @brief    AS1106 IC에 command byte 전송 함수
* @param    command : command\n
* @return   none
* @remark   
********************************************************************************
*/
void as1106TranCmdData(INT8U command, INT8U startFlag)      
{
	INT8U k;
	
	if(startFlag == AS1106_CMD_START) LED_LOAD = 0;
	for (k = 0; k < 8; k++)
	{
		LED_CLK = 0;
		//Send most significant bit first 
		if ((command & 0x80) == 0)
		{
			LED_DIN = 0;
		}
		else
		{
			LED_DIN = 1;
		}
		LED_CLK = 1;
		command <<= 1;
	}	
	if(startFlag == AS1106_CMD_END) LED_LOAD = 1;	
}

/**
********************************************************************************
* @brief    AS1106 IC에 dispTransPage의 Image를 Display Driver IC로 전송
* @param    disdata : data\n
* @return   none
* @remark   
********************************************************************************
*/
void as1106WriteAllData()         
{
	INT8U i;

	for (i = 0; i < DISP_DRV_BUF_SIZE; i++)
	{
		as1106TranCmdData(AS1106_DIG0 + i, AS1106_CMD_START);
		as1106TranCmdData(dispDrvBuf[dispTransPage][i], AS1106_CMD_END);
	}
}
#endif//#ifdef DISP_DRV_AS1106_DB2

