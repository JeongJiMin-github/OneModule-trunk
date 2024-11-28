/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Driver Source File\n
* @file    DispDrv_Efm32g840_Omv3_Lcd.c
* @version 1.0
* @date    2012/09/19
* @author  Deok Hyun Kim (KDH)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2012/09/19 created by KDH \n
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
#ifdef DISP_DRV_HT1621_FW500_LB_OMV3
#include "efm32_cmu.h"
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

//#define DISP_DRV_BUF_SIZE	27
/** @brief Display driver buffer size*/
//#define DISP_DRV_BUF_SIZE	18
#define DISP_DRV_BUF_SIZE	20
//#define DISP_DRV_BUF_SIZE	22

/** @brief Display driver module size*/
#define DSP_DRV_MODULE_SIZE	9
/** @brief Display driver module string size*/
#define DSP_DRV_MODULE_STR_SIZE	6

/** @brief Display driver weight address*/
#define DISP_DRV_WEIGHT_ADDR 1
/** @brief weight display size*/
#define DISP_DRV_WEIGHT_SIZE 5

#define EFM_DISP_DRV_BUF_SIZE	4
//Dot addr is same as digit
//Triangle addr and mask
//   0 ~ 17
//	{8, 0x20}, {8, 0x10}, {7, 0x20}, {7, 0x10}, {6, 0x20}, {6, 0x10}, 
//	{17, 0x20}, {17, 0x10}, {16, 0x20}, {16, 0x10}, {15, 0x20}, {15, 0x10},
//	{26, 0x20}, {26, 0x10}, {25, 0x20}, {25, 0x10}, {24, 0x20}, {24, 0x10} 
//BackLight Indicator addr and mask
//  if total disp window
//  {8, 0x04}

//#define DISP_DRV_ADDR_WEIGHT_MINUS		{0, 0}//RB is minus sign is not exist, so use digit 0 for minus char
//#define DISP_DRV_ADDR_UNIT_PRICE_MINUS	{0xff, 0}//not exist
//#define DISP_DRV_ADDR_TOTAL_PRICE_MINUS	{0xff, 0}//not exist
//#define DISP_DRV_ADDR_TARE_MINUS		{0xff, 0}//not exist
//#define DISP_DRV_ADDR_ZERO				{8, 0x10}//triangle 1
//#define DISP_DRV_ADDR_STABLE			{0xff, 0}//not exist
//#define DISP_DRV_ADDR_NET				{7, 0x10}//triangle 3
//#define DISP_DRV_ADDR_LOW_BATT			{0xff, 0}//not exist

/** @brief MINUS indicator, RB is minus sign is not exist, so use digit 0 for minus char*/
#define DISP_DRV_ADDR_WEIGHT_MINUS		{19, 0x04}
/** @brief Stable indicator*/
#define DISP_DRV_ADDR_STABLE			{19, 0x01}
/** @brief NET indicator*/
#define DISP_DRV_ADDR_NET				{19, 0x08}
/** @brief Low Battery indicator*/
#define DISP_DRV_ADDR_LOW_BATT		{0, 0x01}
/** @brief Ounce indicator*/
#define DISP_DRV_ADDR_OZ				{1, 0x04}
/** @brief Pound indicator*/
#define DISP_DRV_ADDR_LB				{1, 0x08}
/** @brief Kilo gram indicator*/
#define DISP_DRV_ADDR_KG				{0, 0x0c}
/** @brief Gram indicator*/
#define DISP_DRV_ADDR_GRAM			{0, 0x08}
/** @brief PCS indicator, for counting*/
#define DISP_DRV_ADDR_PCS				{0, 0x02}
/** @brief Zero indicator*/
#define DISP_DRV_ADDR_ZERO			{19, 0x02}
/** @brief High indicator, for comparator*/
#define DISP_DRV_ADDR_HI				{18, 0x01}
/** @brief Low indicator, for comparator*/
#define DISP_DRV_ADDR_LOW				{18, 0x06}
/** @brief OK indicator, for comparator*/
#define DISP_DRV_ADDR_OK				{18, 0x0c}
/** @brief OK indicator, for counting*/
#define DISP_DRV_ADDR_LACK				{18, 0x00}
/** @brief OK indicator, for counting*/
#define DISP_DRV_ADDR_FRAC_BAR			{4, 0x01}
/** @brief OK indicator, for counting*/
#define DISP_DRV_ADDR_FRAC_DENOM		{6, 0x02}
/** @brief OK indicator, for counting*/
#define DISP_DRV_ADDR_FRAC_NUMER		{2, 0x01}
/** @brief OK indicator, for counting*/
#define DISP_DRV_ADDR_CTS				{1, 0x02}


/** @brief Turn off both system oscillator and Lcd bias generator*/
#define HT1621_SYS_DIS		0x00
/** @brief Turn on system oscillator*/
#define HT1621_SYS_EN		0x01
/** @brief Turn off Lcd display*/
#define HT1621_OFF			0x02
/** @brief Turn on Lcd display*/
#define HT1621_ON			0x03
/** @brief Disable time base output*/
#define HT1621_TIMER_DIS	0x04
/** @brief Disable WDT time-out flag output*/
#define HT1621_WDT_DIS		0x05
/** @brief Enable time base output*/
#define HT1621_TIMER_EN	0x06
/** @brief Enable WDT time-out flag output*/
#define HT1621_WDT_EN		0x07
/** @brief Turn off tone outputs*/
#define HT1621_TONE_OFF	0x08
/** @brief Turn on tone outputs*/
#define HT1621_TONE_ON 	0x09
/** @brief Clear the contents of the time base generator*/
#define HT1621_CLR_TIMER	0x0c
/** @brief Clear the contents of WDT stage*/
#define HT1621_CLR_WDT		0x0f
/** @brief System clock source, on-chip crystal oscillator*/
#define HT1621_XTAL_32K	0x14
/** @brief System clock source, on-chip RC oscillator*/
#define HT1621_RC_256K		0x18
/** @brief System clock source, external clock source*/
#define HT1621_EXT_256K	0x1c
/** @brief LCD 1/3 bias, 4 commons option*/
#define HT1621_BIAS_1_3 	0x29
/** @brief Tone frequency output : 4kHz*/
#define HT1621_TONE_4K		0x40
/** @brief Tone frequency output : 2kHz*/
#define HT1621_TONE_2K		0x60
/** @brief Disable IRQ output*/
#define HT1621_IRQ_DIS		0x80
/** @brief Enable IRQ output*/
#define HT1621_IRQ_EN		0x88
/** @brief Time base clock output : 1Hz.   The WDT time-out flag after : 4s*/
#define HT1621_F1			0xa0
/** @brief Time base clock output : 2Hz.   The WDT time-out flag after : 2s*/
#define HT1621_F2			0xa1
/** @brief Time base clock output : 4Hz.   The WDT time-out flag after : 1s*/
#define HT1621_F4			0xa2
/** @brief Time base clock output : 8Hz.   The WDT time-out flag after : 1/2s*/
#define HT1621_F8			0xa3
/** @brief Time base clock output : 16Hz.  The WDT time-out flag after : 1/4s*/
#define HT1621_F16			0xa4
/** @brief Time base clock output : 32Hz.  The WDT time-out flag after : 1/8s*/
#define HT1621_F32			0xa5
/** @brief Time base clock output : 64Hz.  The WDT time-out flag after : 1/16s*/
#define HT1621_F64			0xa6
/** @brief Time base clock output : 128Hz. The WDT time-out flag after : 1/32s*/
#define HT1621_F128			0xa7
/** @brief Test mode, user don't use*/
#define HT1621_TEST			0xe0
/** @brief Normal mode*/
#define HT1621_NORMAL		0xe3


/*
	Font bit map
	---4---
	I0	 5I
	---1---
	I2	 6I
	---3---
*/
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
#define FONT_ASC45	0x04
/** @brief FONT_. , not exist*/
#define FONT_ASC46	0x00
/** @brief FONT_/ , not exist*/
#define FONT_ASC47	0x00


/*
       FONT =  DPECFGAB(bit)
        A
    F       B
        G
    E       C
        D        P

*/


/** @brief FONT_0*/
/** @brief FONT_0*/
#define FONT_0	0xBB
/** @brief FONT_1*/
#define FONT_1	0x11
/** @brief FONT_2*/
#define FONT_2	0xA7
/** @brief FONT_3*/
#define FONT_3	0x97
/** @brief FONT_4*/
#define FONT_4	0x1D
/** @brief FONT_5*/
#define FONT_5	0x9E
/** @brief FONT_6*/
#define FONT_6	0xBE
/** @brief FONT_7*/
#define FONT_7	0x1B
/** @brief FONT_8*/
#define FONT_8	0xBF
/** @brief FONT_9*/
#define FONT_9	0x9F





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
#define FONT_ASC64	0xA0

//--------- Alphabet ------------------
/** @brief FONT_A*/
#define FONT_A   0x3F
/** @brief FONT_B*/
#define FONT_B   0xBC
/** @brief FONT_C*/
#define FONT_C   0xAA
/** @brief FONT_D*/
#define FONT_D   0xB5
/** @brief FONT_E*/
#define FONT_E   0xAE
/** @brief FONT_F*/
#define FONT_F   0x2E
/** @brief FONT_G*/
#define FONT_G   0xBA
/** @brief FONT_H*/
#define FONT_H   0x3D
/** @brief FONT_I*/
#define FONT_I   0x10
/** @brief FONT_J*/
#define FONT_J   0xB1
/** @brief FONT_K*/
#define FONT_K   0x2C
/** @brief FONT_L*/
#define FONT_L   0xA8
/** @brief FONT_M*/
#define FONT_M   0x36
/** @brief FONT_N*/
#define FONT_N   0x34
/** @brief FONT_O*/
#define FONT_O   0xB4
/** @brief FONT_P*/
#define FONT_P   0x2F
/** @brief FONT_Q*/
#define FONT_Q   0x1F
/** @brief FONT_R*/
#define FONT_R   0x24
/** @brief FONT_S*/
#define FONT_S   0x9E
/** @brief FONT_T*/
#define FONT_T   0xAC
/** @brief FONT_U*/
#define FONT_U   0xB9
/** @brief FONT_V*/
#define FONT_V   0xB0
/** @brief FONT_W*/
#define FONT_W   0xB2
/** @brief FONT_X*/
#define FONT_X   0x8E
/** @brief FONT_Y*/
#define FONT_Y   0x9D
/** @brief FONT_Z*/
#define FONT_Z   0xA7

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

// OZ 

#define OZ_FONT_0	0xEE
/** @brief FONT_1*/
#define OZ_FONT_1	0x44
/** @brief FONT_2*/
#define OZ_FONT_2	0xB6
/** @brief FONT_3*/
#define OZ_FONT_3	0xD6
/** @brief FONT_4*/
#define OZ_FONT_4	0x5C
/** @brief FONT_5*/
#define OZ_FONT_5	0xDA
/** @brief FONT_6*/
#define OZ_FONT_6	0xFA
/** @brief FONT_7*/
#define OZ_FONT_7	0x4E
/** @brief FONT_8*/
#define OZ_FONT_8	0xFE
/** @brief FONT_9*/
#define OZ_FONT_9	0xDE


//rear Oz

// rear display HT1621




/** @brief FONT_SPACE*/
#define REAR_FONT_SPACE	0x00
/** @brief FONT_! , not exist*/
#define REAR_FONT_ASC33	0x00
/** @brief FONT_" , not exist*/
#define REAR_FONT_ASC34	0x00
/** @brief FONT_# , not exist*/
#define REAR_FONT_ASC35	0x00
/** @brief FONT_$ , not exist*/
#define REAR_FONT_ASC36	0x00
/** @brief FONT_% , not exist*/
#define REAR_FONT_ASC37	0x00
/** @brief FONT_& , not exist*/
#define REAR_FONT_ASC38	0x00
/** @brief FONT_' , not exist*/
#define REAR_FONT_ASC39	0x00
/** @brief FONT_( , not exist*/
#define REAR_FONT_ASC40	0x00
/** @brief FONT_) , not exist*/
#define REAR_FONT_ASC41	0x00
/** @brief FONT_* , not exist*/
#define REAR_FONT_ASC42	0x00
/** @brief FONT_+ , not exist*/
#define REAR_FONT_ASC43	0x00
/** @brief FONT_, , not exist*/
#define REAR_FONT_ASC44	0x00
/** @brief FONT_- , not exist*/
#define REAR_FONT_ASC45	0x20
/** @brief FONT_. , not exist*/
#define REAR_FONT_ASC46	0x00
/** @brief FONT_/ , not exist*/
#define REAR_FONT_ASC47	0x00

/** @brief FONT_0*/
#define REAR_FONT_0	0x5F
/** @brief FONT_1*/
#define REAR_FONT_1	0x50
/** @brief FONT_2*/
#define REAR_FONT_2	0x3D
/** @brief FONT_3*/
#define REAR_FONT_3	0x79
/** @brief FONT_4*/
#define REAR_FONT_4	0x72
/** @brief FONT_5*/
#define REAR_FONT_5	0x6B
/** @brief FONT_6*/
#define REAR_FONT_6	0x6F
/** @brief FONT_7*/
#define REAR_FONT_7	0x53
/** @brief FONT_8*/
#define REAR_FONT_8	0x7F
/** @brief FONT_9*/
#define REAR_FONT_9	0x7B

/** @brief FONT_:, not exist*/
#define REAR_FONT_ASC58	0x00
/** @brief FONT_;, not exist*/
#define REAR_FONT_ASC59	0x00
/** @brief FONT_<, not exist*/
#define REAR_FONT_ASC60	0x00
/** @brief FONT_=, not exist*/
#define REAR_FONT_ASC61	0x00
/** @brief FONT_>, not exist*/
#define REAR_FONT_ASC62	0x00
/** @brief FONT_?, not exist*/
#define REAR_FONT_ASC63	0x00
/** @brief FONT_@, not exist*/
#define REAR_FONT_ASC64	0x0C

//--------- Alphabet ------------------
/** @brief FONT_A*/
#define REAR_FONT_A   0x77
/** @brief FONT_B*/
#define REAR_FONT_B   0x6E
/** @brief FONT_C*/
#define REAR_FONT_C   0x0F
/** @brief FONT_D*/
#define REAR_FONT_D   0x7C
/** @brief FONT_E*/
#define REAR_FONT_E   0x2F
/** @brief FONT_F*/
#define REAR_FONT_F   0x27
/** @brief FONT_G*/
#define REAR_FONT_G   0x4F
/** @brief FONT_H*/
#define REAR_FONT_H   0x76
/** @brief FONT_I*/
#define REAR_FONT_I   0x50
/** @brief FONT_J*/
#define REAR_FONT_J   0x58
/** @brief FONT_K*/
#define REAR_FONT_K   0x26
/** @brief FONT_L*/
#define REAR_FONT_L   0x0E
/** @brief FONT_M*/
#define REAR_FONT_M   0x65
/** @brief FONT_N*/
#define REAR_FONT_N   0x64
/** @brief FONT_O*/
#define REAR_FONT_O   0x6C
/** @brief FONT_P*/
#define REAR_FONT_P   0x37
/** @brief FONT_Q*/
#define REAR_FONT_Q   0x73
/** @brief FONT_R*/
#define REAR_FONT_R   0x24
/** @brief FONT_S*/
#define REAR_FONT_S   0x6B
/** @brief FONT_T*/
#define REAR_FONT_T   0x2E
/** @brief FONT_U*/
#define REAR_FONT_U   0x5E
/** @brief FONT_V*/
#define REAR_FONT_V   0x4C
/** @brief FONT_W*/
#define REAR_FONT_W   0x4D
/** @brief FONT_X*/
#define REAR_FONT_X   0x29
/** @brief FONT_Y*/
#define REAR_FONT_Y   0x7A
/** @brief FONT_Z*/
#define REAR_FONT_Z   0x3D

/** @brief FONT__*/
#define REAR_FONT_ASC91	0x00
/** @brief FONT__*/
#define REAR_FONT_ASC92	0x00
/** @brief FONT__*/
#define REAR_FONT_ASC93	0x00
/** @brief FONT__*/
#define REAR_FONT_ASC94	0x00
/** @brief FONT__*/
#define REAR_FONT_ASC95	0x08
/*
********************************************************************************
*                       LOCAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert local typedef & structure here */
/** @brief Display Driver에서 Segment Display시 사용하는 struct */
typedef _PACKED_DEF struct
{
	/** @brief Image Memory Address */
	INT8U addr;
	/** @brief Image Memory Size */
	INT8U size;
} DISP_DRV_DATA;

/** @brief Display Driver에서 Indicator Display시 사용하는 struct */
typedef _PACKED_DEF struct
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
/** @brief Rear Display Driver에서 사용하는 Segment Font Table */
const INT8U code REAR_FontTable[64] =
{
	REAR_FONT_SPACE, REAR_FONT_ASC33, REAR_FONT_ASC34, REAR_FONT_ASC35, REAR_FONT_ASC36,
	REAR_FONT_ASC37, REAR_FONT_ASC38,	REAR_FONT_ASC39, REAR_FONT_ASC40, REAR_FONT_ASC41,
	REAR_FONT_ASC42, REAR_FONT_ASC43, REAR_FONT_ASC44, REAR_FONT_ASC45,	REAR_FONT_ASC46, REAR_FONT_ASC47,  // 32 - 47               
    REAR_FONT_0,		REAR_FONT_1,		REAR_FONT_2,		REAR_FONT_3,		REAR_FONT_4,
	REAR_FONT_5,		REAR_FONT_6,		REAR_FONT_7,		REAR_FONT_8,		REAR_FONT_9, // 48 - 57  Num 
    REAR_FONT_ASC58, REAR_FONT_ASC59, REAR_FONT_ASC60, REAR_FONT_ASC61, FONT_ASC62,
	REAR_FONT_ASC63, REAR_FONT_ASC64,  // 58 - 64  
    REAR_FONT_A,		REAR_FONT_B,		REAR_FONT_C,		REAR_FONT_D,		REAR_FONT_E,		REAR_FONT_F,		REAR_FONT_G, 
    REAR_FONT_H,		REAR_FONT_I,		REAR_FONT_J,		REAR_FONT_K,		REAR_FONT_L,		REAR_FONT_M,		REAR_FONT_N, 
    REAR_FONT_O,		REAR_FONT_P,		REAR_FONT_Q,		REAR_FONT_R,		REAR_FONT_S,		REAR_FONT_T,		REAR_FONT_U,         
    REAR_FONT_V,		REAR_FONT_W,		REAR_FONT_X,		REAR_FONT_Y,		REAR_FONT_Z,		REAR_FONT_ASC91, REAR_FONT_ASC92, 
    REAR_FONT_ASC93,	REAR_FONT_ASC94, REAR_FONT_ASC95	//  65 -95  alphabet & underbar
};


const INT8U code OzFontTable[26] =
{
	FONT_SPACE, FONT_ASC33, FONT_ASC34, FONT_ASC35, FONT_ASC36,
	FONT_ASC37, FONT_ASC38,	FONT_ASC39, FONT_ASC40, FONT_ASC41,
	FONT_ASC42, FONT_ASC43, FONT_ASC44, FONT_ASC45,	FONT_ASC46, FONT_ASC47,  // 32 - 47               
    OZ_FONT_0,		OZ_FONT_1,		OZ_FONT_2,		OZ_FONT_3,		OZ_FONT_4,
	OZ_FONT_5,		OZ_FONT_6,		OZ_FONT_7,		OZ_FONT_8,		OZ_FONT_9 // 48 - 57  Num 
};

/** @brief Display Driver에서 Segment Display시 사용하는 struct Window 개수 만큼 존재 */
static DISP_DRV_DATA dispDrvStruct[WIN_MAX_NUM];        // 이 함수를 ㅈ자리수 및 소수점 자리수에 쓰기때문에 7로 바꿀수 없음 

/** @brief Image Buffer : 실제 Display Driver IC에 존재하는 메모리의 복사본 */
static INT32U idata efm32DispDrvBuf[DISP_DRV_PAGE_SIZE][EFM_DISP_DRV_BUF_SIZE];
/** @brief Image Buffer : 실제 Display Driver IC에 존재하는 메모리의 복사본 */
static INT8U idata dispDrvBuf[DISP_DRV_PAGE_SIZE][DISP_DRV_BUF_SIZE];

/** @brief Display Driver Mode */
static INT8U dispDrvMode;
/** @brief Display Driver IC에 보낼 Image Buffer 번호 */
static INT8U dispTransPage;
/** @brief Display Driver에서 Edit시 사용하는 Image Buffer 번호 */
static INT8U dispEditingPage;

/** @brief Low Indicator on/off 유무를 저장하는 변수 (OK"OK"와 'O'를 공유하기 때문에)*/
static INT8U dispDrvIndicatorStatusLow;
/** @brief Ok Indicator on/off 유무를 저장하는 변수 (Low"LO"와 'O'를 공유하기 때문에)*/
static INT8U dispDrvIndicatorStatusOk;
/** @brief Gram Indicator on/off 유무를 저장하는 변수 (kg"kg"과 'k'를 공유하기 때문에)*/
static INT8U dispDrvIndicatorStatusGram;
/** @brief Kg Indicator on/off 유무를 저장하는 변수 (gram"g"과 'k'를 공유하기 때문에)*/
static INT8U dispDrvIndicatorStatusKg;





/** 
* @brief Display Driver에서 Indicator Display시 사용하는 struct, Indicator 개수 만큼 존재 
* @remark 배열에서 Indicator위치는 define되어 있음\n
*         임의로 위치 변경하지 마시오.
*         관련 정의를 보려면 DISP_DRV_ADDR_WEIGHT_MINUS를 찾아 보시오.
*/
const INDICATOR_ADDR code dispDrvIndicatorAddrTable[INDICATOR_MAX_NUM+3] =
{
	DISP_DRV_ADDR_WEIGHT_MINUS,			///0 Weight Minus
	{0, 0},								///1 Unit price Minus
	{0, 0},								///2 Total price Minus
	{0, 0},								///3 Tare Minus
	DISP_DRV_ADDR_ZERO,					///4 Zero
	DISP_DRV_ADDR_STABLE,				///5 Stable
	DISP_DRV_ADDR_NET,					///6 Net
	DISP_DRV_ADDR_LOW_BATT,				///7 Low battery
	DISP_DRV_ADDR_OZ,					///8 Ounce
	DISP_DRV_ADDR_LB,					///9 Pound
	DISP_DRV_ADDR_KG,					//10 Kilo gram
	DISP_DRV_ADDR_GRAM,					//11 Gram
	DISP_DRV_ADDR_PCS,					//12 Counting mode
	{0, 0},								//13 Prepack
	DISP_DRV_ADDR_HI,					//14 Compare high
	DISP_DRV_ADDR_LOW,					//15 Compare low
	DISP_DRV_ADDR_OK,					//16 Compare OK	
	{0, 0},								//17 Euro
	{0, 0},								//18 Hold
	{0, 0},								//19 Backlight
	DISP_DRV_ADDR_LACK,					//20 Counting lack
	{0, 0},								//21 Unit price 100Gram
	{0, 0}, 							//22 Tare save
	{0, 0}, 							//23 Both save
	{0, 0}, 							//24 Unit price Kilo Gram
	{0, 0},								//25 Unit price Pound
	{0, 0},								//26 Percent
	{0, 0},								//27 Sum
	{0, 0},								//28 full battery
	{0, 0},								//29 charging battery
	{0, 0},								//30 Euro per Kilogram
	{0, 0},								//31 $
	{0, 0},								//32 $/kg
	{0, 0},								//33 Unit price oz
	{0, 0},								//34 Connected AC
    DISP_DRV_ADDR_CTS,                   //35 CTS ON DISPLAY 
	DISP_DRV_ADDR_FRAC_BAR,				//fraction bar
	DISP_DRV_ADDR_FRAC_NUMER,			//fraction numerator 1
	DISP_DRV_ADDR_FRAC_DENOM			//fraction denominator 1
};

#define INDICATOR_FRAC_BAR			INDICATOR_MAX_NUM
#define INDICATOR_FRAC_NUMER		(INDICATOR_MAX_NUM + 1)
#define INDICATOR_FRAC_DENOM		(INDICATOR_MAX_NUM + 2)


/** @brief Dot addr and mask*/
const INDICATOR_ADDR code dispDrvDotAddrTable[5] =
{
	{16, 0x08}, {13, 0x08}, {11, 0x08}, {9, 0x08}, {7, 0x08} 
};

/** @brief Comma addr and mask*/
const INDICATOR_ADDR code dispDrvCommaAddrTable[5] =
{
	{14, 0x01}, {14, 0x04}, {14, 0x08}, {6, 0x08}, {6, 0x04} 
};

/** @brief s0~s39 중 디지트 시작 주소*/
INT8U code dispDrvCharAddrTable[7] = { 16, 13, 11, 9, 7, 2, 4};

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
	1, 0, 0, 0
};
/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
static void dispDrvSetDot(INT8U winNum, INT8U decPos, INT8U on);
static void dispDrvSetComma(INT8U winNum, INT8U decPos, INT8U on);
void lcd_UseBoost(BOOLEAN useBoost);       // display관련 추가 
void lcdEnableSegment(INT32U com, INT32U bitValue); // display 관련 추가 
void ht1621WriteAllData();
void ht1621TranData(INT8U disdata);
void ht1621TranCommand(INT8U command);
void ht1621WriteCommand(INT8U datcom);
/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
/*
void testfont(void)
{
    INT8U i;
    INT8U temp;
    INT8U tempBit;
    static INT8U FontTemp[64];
    for (i = 0 ; i<64 ; i++)
    {
        temp = FontTable[i];
        tempBit = (FontTable[i]>>0) & 0x01;
        temp = (temp & ~(0x01<<1)) | (tempBit<<1);
        tempBit = (FontTable[i]>>1) & 0x01;
        temp = (temp & ~(0x01<<5)) | (tempBit<<5);
        tempBit = (FontTable[i]>>2) & 0x01;
        temp = (temp & ~(0x01<<2)) | (tempBit<<2);

        tempBit = (FontTable[i]>>3) & 0x01;
        temp = (temp & ~(0x01<<3)) | (tempBit<<3);
        tempBit = (FontTable[i]>>4) & 0x01;
        temp = (temp & ~(0x01<<0)) | (tempBit<<0);
        tempBit = (FontTable[i]>>5) & 0x01;
        temp = (temp & ~(0x01<<4)) | (tempBit<<4);
        tempBit = (FontTable[i]>>6) & 0x01;
        temp = (temp & ~(0x01<<6)) | (tempBit<<6);
        tempBit = (FontTable[i]>>7) & 0x01;
        FontTemp[i] = (temp & ~(0x01<<7)) | (tempBit<<7);
    }
}

*/

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
    /* Disable interrupts */
    LCD->IEN = 0;
    
    /* Enable Boost if required */
    lcd_UseBoost(false);
    /* Turn all segments off */
    DispDrvClearBuf();    
    /* Enable all segment registers */
    LCD->SEGEN = 0x0000003D;
//    LCD->SEGEN = 0x0000001F;        // duckspg seg 연결 
    LCD->CTRL  = LCD_CTRL_EN | LCD_CTRL_UDCTRL_FRAMESTART;
    while (LCD->SYNCBUSY) ;
  
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

	ht1621WriteCommand(HT1621_ON);	
	ht1621WriteCommand(HT1621_SYS_EN); /* Turn on system oscillator*/	
//	ht1622WriteCommand(HT1622_TONE_OFF); /* Turn off tone outputs */		
	ht1621WriteCommand(HT1621_RC_256K); /* System clock source, on-chip RC oscillator*/
//	ht1622WriteCommand(HT1622_TIMER_DIS);
//	ht1622WriteCommand(HT1622_WDT_DIS);
//	ht1622WriteCommand(HT1622_IRQ_DIS);
	ht1621WriteCommand(HT1621_BIAS_1_3);	
//	ht1622WriteCommand(HT1622_IRQ_DIS);	/* Disable IRQ output */	
//	ht1622WriteCommand(HT1622_NORMAL);	/* Normal mode*/
    
    
	DispDrvClearBuf();
    LCD_ContrastSet(CONTRAST_SET_VALUE);
    ht1621WriteAllData();
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

	dispDrvIndicatorStatusLow = OFF;
	dispDrvIndicatorStatusOk = OFF;
	dispDrvIndicatorStatusGram = OFF;
	dispDrvIndicatorStatusKg = OFF;

	for (page = 0; page < DISP_DRV_PAGE_SIZE; page++) 
	{
		for (addr = 0; addr < DISP_DRV_BUF_SIZE; addr++) 
		{
			dispDrvBuf[page][addr] = 0x00;
		}
	}
    
    
	for (page = 0; page < DISP_DRV_PAGE_SIZE; page++) 
	{
		for (addr = 0; addr < EFM_DISP_DRV_BUF_SIZE; addr++) 
		{
			efm32DispDrvBuf[page][addr] = 0x00;
		}
	}

    LCD->SEGD0L = 0x00000000;
    LCD->SEGD0H = 0x00000000;
    LCD->SEGD1L = 0x00000000;
    LCD->SEGD1H = 0x00000000;
    LCD->SEGD2L = 0x00000000;
    LCD->SEGD2H = 0x00000000;
    LCD->SEGD3L = 0x00000000;
    LCD->SEGD3H = 0x00000000;

}


/**
********************************************************************************
* @brief    Window에 Fraction 출력 (editting Page)
* @param    INT8U winNum : Window 번호\n
*           INT8U numer :	numerator
*           INT8U denom : denominator
* @return   none
* @remark   str 채운후 남는 부분은 space로 채움
********************************************************************************
*/
void DispDrvSetFraction(INT8U winNum, INT8U numer, INT8U denom)
{
	if (winNum !=  WIN_WEIGHT) 
	{
		return;
	}

	if (numer >= 20)
	{
		DispDrvSetIndicator(INDICATOR_FRAC_BAR, OFF);
		DispDrvSetIndicator(INDICATOR_FRAC_NUMER, OFF);
		DispDrvSetIndicator(INDICATOR_FRAC_DENOM, OFF);
	 	DispDrvSetChar(winNum, 5, ' ');
		if (denom >= 20)
		{
			DispDrvSetChar(winNum, 6, ' ');
			dispDrvSetDot(winNum, 0, OFF);
		}
		else
		{
			DispDrvSetChar(winNum, 6, (denom%10 + '0'));
			dispDrvSetDot(winNum, (numer - 0xf0), ON);
		}
		return;
	}
	dispDrvSetDot(winNum, 0, OFF);

	DispDrvSetIndicator(INDICATOR_FRAC_BAR, ON);

	if ((numer/10) == 1)
	{
		DispDrvSetIndicator(INDICATOR_FRAC_NUMER, ON);
	}
	else
	{
		DispDrvSetIndicator(INDICATOR_FRAC_NUMER, OFF);
	}

	if ((denom/10) == 1)
	{
		DispDrvSetIndicator(INDICATOR_FRAC_DENOM, ON);
	}	
	else
	{
		DispDrvSetIndicator(INDICATOR_FRAC_DENOM, OFF);
	}

	DispDrvSetChar(winNum, 5, (numer%10 + '0'));
	DispDrvSetChar(winNum, 6, (denom%10 + '0'));
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
void DispDrvSetChar(INT8U winNum, INT8U pos, char cdata)            // duckspg lb
{
	INT8U byte;
	INT8U addr;
	INT32U mask;
    INT8U i=0;
    INT8U dataTemp=0;
    INT8U oz_byte=0;

	addr = dispDrvCharAddrTable[pos];	//14
    byte = REAR_FontTable[cdata - 0x20];	//0x77
    if (addr == 4 | addr == 2)
    {
        mask = dispDrvBuf[dispEditingPage][addr] & 0x01; 
        dispDrvBuf[dispEditingPage][addr] = ((byte >> 3)&0x0E) | mask;
    }
    else
    {
        mask = dispDrvBuf[dispEditingPage][addr] & 0x08; 
        dispDrvBuf[dispEditingPage][addr] = ((byte >> 4)&0x07) | mask;
    }

    addr++;
    if (addr == 14)
        addr++;
    dispDrvBuf[dispEditingPage][addr] = byte;



	addr = dispDrvCharAddrTable[pos];
	byte = FontTable[cdata - 0x20];
    if((cdata - 0x20) < 26)
    {
        oz_byte = OzFontTable[cdata - 0x20];
    }

    for(i=0 ; i<4 ; i++)
    {
        // 변경 fonttable           두개씩 짝을 지어서 재배열 

        if (addr == 13) // us LCD는 주소16이 컴마를 표현함 
        {
            dataTemp = ((byte>>(i<<1))&0x03);
            efm32DispDrvBuf[dispEditingPage][i] = (efm32DispDrvBuf[dispEditingPage][i] & ~(0x5<<addr)) | ((dataTemp&0x01)<<addr) | (((dataTemp>>1)&0x01)<<(addr+2));
        }
        else if (addr == 4 || addr == 2)   
        {
            if (i == 0)
                efm32DispDrvBuf[dispEditingPage][i] = (efm32DispDrvBuf[dispEditingPage][i] & ~(0x2<<addr)) | ((oz_byte>>(i<<1))&0x02)<<addr;
            else
                efm32DispDrvBuf[dispEditingPage][i] = (efm32DispDrvBuf[dispEditingPage][i] & ~(0x3<<addr)) | ((oz_byte>>(i<<1))&0x03)<<addr;
        }
        else

        efm32DispDrvBuf[dispEditingPage][i] = (efm32DispDrvBuf[dispEditingPage][i] & ~(0x3<<addr)) | ((byte>>(i<<1))&0x03)<<addr;
        // 기존 fonttable
//        efm32DispDrvBuf[dispEditingPage][i] = (efm32DispDrvBuf[dispEditingPage][i] & ~(0x3<<addr)) | (((byte & (0x10<<i))>>(4+i))<<(addr));
//        efm32DispDrvBuf[dispEditingPage][i] = (efm32DispDrvBuf[dispEditingPage][i]) | (((byte & (0x01<<i))>>i)<<(addr+1));
    }
//    efm32DispDrvBuf[dispEditingPage][3] = efm32DispDrvBuf[dispEditingPage][3] | mask;
}

/**
********************************************************************************
* @brief    Editting Image에 입력 받은 문자로 채움
* @param    fillChar : 채울 문자
* @return   none
* @remark   
********************************************************************************
*/
void DispDrvFillChar(char fillChar)     // duckspg test
{
	INT8U winNum;
	INT8U pos;
	INT8U size;

	for (winNum = 0; winNum < DISP_DRV_WIN_MAX_NUM; winNum++)
	{
		size = dispDrvStruct[winNum].size;
		for (pos = 0; pos < 7; pos++)       // duckspg test
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

	for (pos = 0; pos < INDICATOR_MAX_NUM+3; pos++)
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

		addr = dispDrvCharAddrTable[i];
		byte = REAR_FontTable[dispChar - 0x20];
		mask = dispDrvBuf[dispEditingPage][addr] & 0x08; 
		dispDrvBuf[dispEditingPage][addr] = (byte >> 4) | mask;
		addr++;
		dispDrvBuf[dispEditingPage][addr] = byte;

        DispDrvSetChar(winNum,i,dispChar);
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
	dataSize = dispDrvStruct[winNum].size;      // duckspg test

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
		byte = REAR_FontTable[dispChar - 0x20];
		mask = dispDrvBuf[dispEditingPage][addr]&0x08; 
		dispDrvBuf[dispEditingPage][addr] = (byte >> 4) | mask;
		addr++;
		dispDrvBuf[dispEditingPage][addr] = byte;
        
        DispDrvSetChar(winNum,i,dispChar);
	}
}
/**
********************************************************************************
* @brief    
* @param    
* @return   
* @remark   Oz 분수 사용시 Clear할때 분수를 지움 자리수를 5개로 이용하기 때문에 oz 분수는 안지워짐 
********************************************************************************
*/
void DispDrvOzLcdClear(void)
{
    DispDrvSetChar(0,5,' ');// duckspg test
    DispDrvSetChar(0,6,' ');// duckspg test
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

	if (decPos == 0xff)//date/time
	{
		dispDrvSetDot(winNum, 2, ON);
		dispDrvSetComma(winNum, 2, ON);
		dispDrvSetDot(winNum, 4, ON);
		dispDrvSetComma(winNum, 4, ON);
		return;
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
	INT8U prevOnNum;
    INT8U i;

	prevOnNum = 0;
	if (num == INDICATOR_LOW) 
	{
		dispDrvIndicatorStatusLow = on;
		//"OK"가 켜있고 "LO"를 끌경우 나중에 "OK"를 켠다
		if (on == OFF && dispDrvIndicatorStatusOk == ON)
		{
			prevOnNum = INDICATOR_OK;
		}
	}
	else if (num == INDICATOR_OK)
	{
		dispDrvIndicatorStatusOk = on;
		//"LO"가 켜있고 "OK"를 끌경우 나중에 "LO"를 켠다
		if (on == OFF && dispDrvIndicatorStatusLow == ON)
		{
			prevOnNum = INDICATOR_LOW;
		}
	}
	else if (num == INDICATOR_GRAM)
	{
		dispDrvIndicatorStatusGram = on;
		//"kg"이 켜있고 "g"를 끌경우 나중에 "kg"을 켠다
		if (on == OFF && dispDrvIndicatorStatusKg == ON)
		{
			prevOnNum = INDICATOR_KG;
		}
	}
	else if (num == INDICATOR_KG)
	{
		dispDrvIndicatorStatusKg = on;
		//"g"이 켜있고 "kg"를 끌경우 나중에 "g"을 켠다
		if (on == OFF && dispDrvIndicatorStatusKg == ON)
		{
			prevOnNum = INDICATOR_GRAM;
		}
	}

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
    
    
	addr = dispDrvIndicatorAddrTable[num].addr;
	mask = dispDrvIndicatorAddrTable[num].mask;

    for (i = 0; i<4 ; i++)
    {
        if (mask & (0x01<<i))
        {
            if (on)
            {
                efm32DispDrvBuf[dispEditingPage][i] = efm32DispDrvBuf[dispEditingPage][i] | (0x01<<addr);
            }
            else
            {
                efm32DispDrvBuf[dispEditingPage][i] = efm32DispDrvBuf[dispEditingPage][i] & ~(0x01<<addr);
            }
        }
    }

	if (prevOnNum != 0)
	{
		addr = dispDrvIndicatorAddrTable[prevOnNum].addr;
		mask = dispDrvIndicatorAddrTable[prevOnNum].mask;	
	
        for (i = 0; i<4 ; i++)
        {
            if (mask & (0x01<<i))
            {
                efm32DispDrvBuf[dispEditingPage][i] = efm32DispDrvBuf[dispEditingPage][i] | (0x01<<addr);
            }
        }
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
    INT8U i = 0;
    
    while (LCD->SYNCBUSY) ;
    LCD->FREEZE = LCD_FREEZE_REGFREEZE_FREEZE;
    
    LCD->SEGD0L &= 0x80000000;
    LCD->SEGD1L &= 0x80000000;
    LCD->SEGD2L &= 0x80000000;
    LCD->SEGD3L &= 0x80000000;
    
    
    for (i = 0 ; i<4 ; i++)
    {
        lcdEnableSegment(i,efm32DispDrvBuf[dispTransPage][i]);
    }
    LCD->FREEZE = LCD_FREEZE_REGFREEZE_UPDATE;
    
    ht1621WriteAllData();
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
		ht1621TranCommand(HT1621_ON);
	}
	else
	{
		ht1621TranCommand(HT1621_OFF);
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
    INT8U i;

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
    
    
    
	addr = dispDrvDotAddrTable[pos].addr;
	mask = dispDrvDotAddrTable[pos].mask;

    for (i = 0; i<4 ; i++)
    {
        if (mask & (0x01<<i))
        {
            if (on)
            {
                efm32DispDrvBuf[dispEditingPage][i] = efm32DispDrvBuf[dispEditingPage][i] | (0x01<<addr);
            }
            else
            {
                efm32DispDrvBuf[dispEditingPage][i] = efm32DispDrvBuf[dispEditingPage][i] & ~(0x01<<addr);
            }
        }
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
	INT8U addr;
	INT8U mask;
	INT8U pos;
    INT8U i;

	pos = dispDrvStruct[winNum].size - decPos - 1;

	addr = dispDrvCommaAddrTable[pos].addr;
	mask = dispDrvCommaAddrTable[pos].mask;

	if (on)
	{
		dispDrvBuf[dispEditingPage][addr] = dispDrvBuf[dispEditingPage][addr] | mask;
	}
	else
	{
		dispDrvBuf[dispEditingPage][addr] = dispDrvBuf[dispEditingPage][addr] & (~mask);	
	}
    
    
	addr = dispDrvCommaAddrTable[pos].addr;
	mask = dispDrvCommaAddrTable[pos].mask;

    for (i = 0; i<4 ; i++)
    {
        if (mask & (0x01<<i))
        {
            if (on)
            {
                efm32DispDrvBuf[dispEditingPage][i] = efm32DispDrvBuf[dispEditingPage][i] | (0x01<<addr);
            }
            else
            {
                efm32DispDrvBuf[dispEditingPage][i] = efm32DispDrvBuf[dispEditingPage][i] & ~(0x01<<addr);
            }
        }
    }
}
/**
********************************************************************************
* @brief    LCD boost 함수
* @param    true  : boost
*           false : not boost
* @return   none
* @remark   lcd 파워가 떨어질때 BOOST를 하고 강제로 올릴수 있는 함수 
********************************************************************************
*/
void lcd_UseBoost(BOOLEAN useBoost)
{
  /* Frame rate is 32Hz, 0.25Khz LFCLK128, QUADRUPLEX mode, FDIV=0 */
  if (!useBoost)
  {
    /* No voltage boost, framerate 32Hz */
    CMU->LCDCTRL = 0;

    LCD->DISPCTRL = LCD_DISPCTRL_MUX_QUADRUPLEX |
                    LCD_DISPCTRL_BIAS_ONETHIRD |
                    LCD_DISPCTRL_WAVE_LOWPOWER |
                    LCD_DISPCTRL_CONLEV_MAX |
                    LCD_DISPCTRL_VLCDSEL_VDD |
                    LCD_DISPCTRL_VBLEV_LEVEL0;
  }
  else
  {
    /* CMU->LCDCTRL voltage boost frequency set to default 8KHz */
    CMU->LCDCTRL  = CMU_LCDCTRL_VBOOSTEN;
    LCD->DISPCTRL = LCD_DISPCTRL_MUX_QUADRUPLEX |
                    LCD_DISPCTRL_BIAS_ONETHIRD |
                    LCD_DISPCTRL_WAVE_LOWPOWER |
                    LCD_DISPCTRL_CONLEV_MAX |
                    LCD_DISPCTRL_VLCDSEL_VEXTBOOST |
                    LCD_DISPCTRL_VBLEV_LEVEL0;
  }
}
/**
********************************************************************************
* @brief    lcd enable segment 함수
* @param    com   bitvalue
* @return   none
* @remark   LCD 레지스터를 직접 써서 LCD컨트롤을 해줌 마지막에 이것을 꼭해야 LCD가 동작함
********************************************************************************
*/
void lcdEnableSegment(INT32U com, INT32U bitValue)
{
    INT32U bitValueTemp = 0;
    
    bitValueTemp = ((bitValue<<4) | (bitValue&0xf));
//    bitValueTemp = (((bitValue<<2) & ~0xff) | (bitValue&0x3f));        // duckspg lcd seg 변경 
    
    switch (com)
    {
        case 0:
            LCD->SEGD0L |= bitValueTemp;        // duckspg lcd com 변경
            break;
        case 1:
            LCD->SEGD1L |= bitValueTemp;
            break;
        case 2:
            LCD->SEGD2L |= bitValueTemp;
            break;
        case 3:
            LCD->SEGD3L |= bitValueTemp;
            break;
        case 4:
            LCD->SEGD0H |= bitValueTemp;
            break;
        case 5:
            LCD->SEGD1H |= bitValueTemp;
            break;
        case 6:
            LCD->SEGD2H |= bitValueTemp;
            break;
        case 7:
            LCD->SEGD3H |= bitValueTemp;
            break;
    }
}
/**
********************************************************************************
* @brief    HT1621 IC에 command 전송 함수
* @param    datcom : command\n
* @return   none
* @remark   
********************************************************************************
*/
void ht1621WriteCommand(INT8U datcom)         
{
	REAR_LCD_CS_CLR;

	REAR_LCD_WR_CLR;	
	REAR_LCD_DATA_SET;
	delay2Clock();
	REAR_LCD_WR_SET;  ///1
	delay2Clock();

	REAR_LCD_WR_CLR;
	REAR_LCD_DATA_CLR;
	delay2Clock();
	REAR_LCD_WR_SET;  //0
	delay2Clock();

	REAR_LCD_WR_CLR;
	REAR_LCD_DATA_CLR;
	delay2Clock();
	REAR_LCD_WR_SET;  //0
	delay2Clock();

	ht1621TranCommand(datcom);

	REAR_LCD_WR_CLR;
	delay2Clock();
	REAR_LCD_WR_SET;  
	delay2Clock();
	REAR_LCD_CS_SET;
}



/**
********************************************************************************
* @brief    HT1621 IC에 dispTransPage의 Image를 Display Driver IC로 전송
* @param    disdata : data\n
* @return   none
* @remark   
********************************************************************************
*/
void ht1621WriteAllData()         
{
	INT8U i;
	INT8U *dispBufPtr;

	REAR_LCD_CS_CLR;
	REAR_LCD_WR_CLR;
	delay2Clock();

	REAR_LCD_DATA_SET;
	delay2Clock();

	REAR_LCD_WR_SET;
	delay2Clock();

	ht1621TranCommand(0x40); //Write from memory address 0

#ifdef SWAP_DISP_DRIVER_SEG_PIN
	dispBufPtr = &dispDrvBuf[dispTransPage][DISP_DRV_BUF_SIZE-1];
	for (i = 0; i < DISP_DRV_BUF_SIZE; i++)
	{   
		ht1621TranData(*dispBufPtr);
		dispBufPtr--;
	}
#else
	dispBufPtr = &dispDrvBuf[dispTransPage][0];
	for (i = 0; i < DISP_DRV_BUF_SIZE; i++)
	{   
		ht1621TranData(*dispBufPtr);
		dispBufPtr++;
	}
#endif
	REAR_LCD_CS_SET;
}



/**
********************************************************************************
* @brief    HT1621 IC에 command byte 전송 함수
* @param    command : command\n
* @return   none
* @remark   
********************************************************************************
*/
void ht1621TranCommand(INT8U command)      
{
    INT8U k;
    
    for (k = 0; k < 8; k++)
	{
		REAR_LCD_WR_CLR;
		//Send most significant bit first 
		if ((command & 0x80) == 0)
		{
			REAR_LCD_DATA_CLR;
		}
		else
		{
			REAR_LCD_DATA_SET;
		}
		delay2Clock();
		REAR_LCD_WR_SET;   //Data are latched into the HT1621
		command <<= 1;
		delay2Clock();
	}  
}

/**
********************************************************************************
* @brief    HT1621 IC에 data byte 전송 함수
* @param    disdata : data\n
* @return   none
* @remark   
********************************************************************************
*/
void ht1621TranData(INT8U disdata)       
{
	INT8U k;

	for (k = 0; k < 4; k++)
	{
		REAR_LCD_WR_CLR;

        if (disdata & 0x01)
            REAR_LCD_DATA_SET;
        else
            REAR_LCD_DATA_CLR;

		REAR_LCD_WR_SET;    
		disdata >>= 1;

		delay2Clock();
	}  
}






#endif//#ifdef DISP_DRV_HT1621_DB2

