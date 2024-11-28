/**
********************************************************************************
* Copyright (c) 2018 CAS
* @brief   Display Driver Source File\n
           for OMV4 SW2W LCD
* @file    DispDrv_Ch462_Omv4_Sw2w_Lcd.c
* @version 1.0
* @date    2018/09/18
* @author  Park Young Min (FDC954)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2018/09/18 created by FDC954 \n
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
#ifdef DISP_DRV_CH462_M0516LDN_SW2W_LCD
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
/** @brief Display ����*/
#define DISP_DRV_WIN_MAX_NUM	1

//#define DISP_DRV_BUF_SIZE	27
/** @brief Display driver buffer size*/
#define DISP_DRV_BUF_SIZE	18
//#define DISP_DRV_BUF_SIZE	22

/** @brief Display driver module size*/
#define DSP_DRV_MODULE_SIZE	9
/** @brief Display driver module string size*/
#define DSP_DRV_MODULE_STR_SIZE	6

/** @brief Display driver weight address*/
#define DISP_DRV_WEIGHT_ADDR 1
/** @brief weight display size*/
//#define DISP_DRV_WEIGHT_SIZE 5
#define DISP_DRV_WEIGHT_SIZE 6

/** @brief weight display indicator address*/
#define DISP_DRV_WEIGHT_INDICATOR_ADDR 6


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
#define DISP_DRV_ADDR_WEIGHT_MINUS		        {16, 0x02}
/** @brief Stable indicator*/
#define DISP_DRV_ADDR_STABLE			        {16, 0x04}
/** @brief NET indicator*/
#define DISP_DRV_ADDR_NET				{16, 0x01}
/** @brief Low Battery indicator*/
#define DISP_DRV_ADDR_LOW_BATT		                {16, 0x08}
/** @brief Ounce indicator*/
#define DISP_DRV_ADDR_OZ				{1, 0x08}
/** @brief Pound indicator*/
#define DISP_DRV_ADDR_LB				{1, 0x04}
/** @brief Kilo gram indicator*/
#define DISP_DRV_ADDR_KG				{3, 0x01} 
/** @brief Gram indicator*/
#define DISP_DRV_ADDR_GRAM			        {1, 0x02}
/** @brief PCS indicator, for counting*/
#define DISP_DRV_ADDR_PCS				{3, 0x04}
/** @brief Zero indicator*/
#define DISP_DRV_ADDR_ZERO			        {17, 0x02}
/** @brief High indicator, for comparator*/
#define DISP_DRV_ADDR_HI				{2, 0x04}
/** @brief Low indicator, for comparator*/
#define DISP_DRV_ADDR_LOW				{2, 0x01}
/** @brief OK indicator, for comparator*/
#define DISP_DRV_ADDR_OK				{2, 0x02}
/** @brief OK indicator, for counting*/
#define DISP_DRV_ADDR_LACK				{17, 0x01}

/** @brief Turn off both system oscillator and Lcd bias generator*/
#define CH462_SYS_DIS		0x00
/** @brief Turn on system oscillator*/
#define CH462_SYS_EN		0x01
/** @brief Turn off Lcd display*/
#define CH462_OFF			0x02
/** @brief Turn on Lcd display*/
#define CH462_ON			0x03
///** @brief Disable time base output*/
//#define CH462_TIMER_DIS	0x04
///** @brief Disable WDT time-out flag output*/
//#define CH462_WDT_DIS		0x05
///** @brief Enable time base output*/
//#define CH462_TIMER_EN	0x06
///** @brief Enable WDT time-out flag output*/
//#define CH462_WDT_EN		0x07
///** @brief Turn off tone outputs*/
//#define CH462_TONE_OFF	0x08
///** @brief Turn on tone outputs*/
//#define CH462_TONE_ON 	0x09
///** @brief Clear the contents of the time base generator*/
//#define CH462_CLR_TIMER	0x0c
///** @brief Clear the contents of WDT stage*/
//#define CH462_CLR_WDT		0x0f
///** @brief System clock source, on-chip crystal oscillator*/
//#define CH462_XTAL_32K	0x14
///** @brief System clock source, on-chip RC oscillator*/
//#define CH462_RC_256K		0x18
///** @brief System clock source, external clock source*/
//#define CH462_EXT_256K	0x1c
/** @brief LCD 1/3 bias, 4 commons option*/
#define CH462_BIAS_1_3 	0x29
///** @brief Tone frequency output : 4kHz*/
//#define CH462_TONE_4K		0x40
///** @brief Tone frequency output : 2kHz*/
//#define CH462_TONE_2K		0x60
///** @brief Disable IRQ output*/
//#define CH462_IRQ_DIS		0x80
///** @brief Enable IRQ output*/
//#define CH462_IRQ_EN		0x88
///** @brief Time base clock output : 1Hz.   The WDT time-out flag after : 4s*/
//#define CH462_F1			0xa0
///** @brief Time base clock output : 2Hz.   The WDT time-out flag after : 2s*/
//#define CH462_F2			0xa1
///** @brief Time base clock output : 4Hz.   The WDT time-out flag after : 1s*/
//#define CH462_F4			0xa2
///** @brief Time base clock output : 8Hz.   The WDT time-out flag after : 1/2s*/
//#define CH462_F8			0xa3
///** @brief Time base clock output : 16Hz.  The WDT time-out flag after : 1/4s*/
//#define CH462_F16			0xa4
///** @brief Time base clock output : 32Hz.  The WDT time-out flag after : 1/8s*/
//#define CH462_F32			0xa5
///** @brief Time base clock output : 64Hz.  The WDT time-out flag after : 1/16s*/
//#define CH462_F64			0xa6
///** @brief Time base clock output : 128Hz. The WDT time-out flag after : 1/32s*/
//#define CH462_F128			0xa7
///** @brief Test mode, user don't use*/
//#define CH462_TEST			0xe0
///** @brief Normal mode*/
//#define CH462_NORMAL		0xe3

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


/** @brief FONT_0*/
#define FONT_0	0xeb
/** @brief FONT_1*/
#define FONT_1	0x60
/** @brief FONT_2*/
#define FONT_2	0xc7
/** @brief FONT_3*/
#define FONT_3	0xe5
/** @brief FONT_4*/
#define FONT_4	0x6c
/** @brief FONT_5*/
#define FONT_5	0xad
/** @brief FONT_6*/
#define FONT_6	0xaf
/** @brief FONT_7*/
#define FONT_7	0xe8
/** @brief FONT_8*/
#define FONT_8	0xef
/** @brief FONT_9*/
#define FONT_9	0xed

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
#define FONT_A   0xee
/** @brief FONT_B*/
#define FONT_B   0x2f
/** @brief FONT_C*/
#define FONT_C   0x8b
/** @brief FONT_D*/
#define FONT_D   0x67
/** @brief FONT_E*/
#define FONT_E   0x8f
/** @brief FONT_F*/
#define FONT_F   0x8e
/** @brief FONT_G*/
#define FONT_G   0xab
/** @brief FONT_H*/
#define FONT_H   0x6e
/** @brief FONT_I*/
#define FONT_I   0x20
/** @brief FONT_J*/
#define FONT_J   0xe3
/** @brief FONT_K*/
#define FONT_K   0x0e //
/** @brief FONT_L*/
#define FONT_L   0x0b
/** @brief FONT_M*/
#define FONT_M   0xa6 //
/** @brief FONT_N*/
#define FONT_N   0x26 //
/** @brief FONT_O*/
#define FONT_O   0x27
/** @brief FONT_P*/
#define FONT_P   0xce
/** @brief FONT_Q*/
#define FONT_Q   0x27
/** @brief FONT_R*/
#define FONT_R   0x06 //
/** @brief FONT_S*/
#define FONT_S   0xad
/** @brief FONT_T*/
#define FONT_T   0x0f //
/** @brief FONT_U*/
#define FONT_U   0x6b 
/** @brief FONT_V*/
#define FONT_V   0x23 //
/** @brief FONT_W*/
#define FONT_W   0xa3 //
/** @brief FONT_X*/
#define FONT_X   0x1a //
/** @brief FONT_Y*/
#define FONT_Y   0x6d
/** @brief FONT_Z*/
#define FONT_Z   0xc7 //

/** @brief FONT__*/
#define FONT_ASC91	0x00
/** @brief FONT__*/
#define FONT_ASC92	0x00
/** @brief FONT__*/
#define FONT_ASC93	0x00
/** @brief FONT__*/
#define FONT_ASC94	0x00
/** @brief FONT__*/
#define FONT_ASC95	0x01

/*
********************************************************************************
*                       LOCAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert local typedef & structure here */
/** @brief Display Driver���� Segment Display�� ����ϴ� struct */
typedef struct
{
	/** @brief Image Memory Address */
	INT8U addr;
	/** @brief Image Memory Size */
	INT8U size;
} DISP_DRV_DATA;

/** @brief Display Driver���� Indicator Display�� ����ϴ� struct */
typedef struct
{
	/** @brief Image Memory Address */
	INT8U addr;
	/** @brief Image Memory Address���� Indicator bit Mask */
	INT8U mask;
} INDICATOR_ADDR;
/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/
/* Insert file scope variable & tables here */
/** @brief Display Driver���� ����ϴ� Segment Font Table */
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
/** @brief Display Driver���� Segment Display�� ����ϴ� struct Window ���� ��ŭ ���� */
static DISP_DRV_DATA dispDrvStruct[WIN_MAX_NUM];
/** @brief Image Buffer : ���� Display Driver IC�� �����ϴ� �޸��� ���纻 */
static INT8U idata dispDrvBuf[DISP_DRV_PAGE_SIZE][DISP_DRV_BUF_SIZE];
/** @brief Display Driver Mode */
static INT8U dispDrvMode;
/** @brief Display Driver IC�� ���� Image Buffer ��ȣ */
static INT8U dispTransPage;
/** @brief Display Driver���� Edit�� ����ϴ� Image Buffer ��ȣ */
static INT8U dispEditingPage;

/** @brief Low Indicator on/off ������ �����ϴ� ���� (OK"OK"�� 'O'�� �����ϱ� ������)*/
static INT8U dispDrvIndicatorStatusLow;
/** @brief Ok Indicator on/off ������ �����ϴ� ���� (Low"LO"�� 'O'�� �����ϱ� ������)*/
static INT8U dispDrvIndicatorStatusOk;
/** @brief Gram Indicator on/off ������ �����ϴ� ���� (kg"kg"�� 'k'�� �����ϱ� ������)*/
static INT8U dispDrvIndicatorStatusGram;
/** @brief Kg Indicator on/off ������ �����ϴ� ���� (gram"g"�� 'k'�� �����ϱ� ������)*/
static INT8U dispDrvIndicatorStatusKg;

/** 
* @brief Display Driver���� Indicator Display�� ����ϴ� struct, Indicator ���� ��ŭ ���� 
* @remark �迭���� Indicator��ġ�� define�Ǿ� ����\n
*         ���Ƿ� ��ġ �������� ���ÿ�.
*         ���� ���Ǹ� ������ DISP_DRV_ADDR_WEIGHT_MINUS�� ã�� ���ÿ�.
*/
const INDICATOR_ADDR code dispDrvIndicatorAddrTable[INDICATOR_MAX_NUM] =
{
	DISP_DRV_ADDR_WEIGHT_MINUS,			///0 Weight Minus
	{0, 0}, 							///1 Unit price Minus
	{0, 0}, 							///2 Total price Minus
	{0, 0}, 							///3 Tare Minus
	DISP_DRV_ADDR_ZERO, 				///4 Zero
	DISP_DRV_ADDR_STABLE,				///5 Stable
	DISP_DRV_ADDR_NET,					///6 Net
	DISP_DRV_ADDR_LOW_BATT, 			///7 Low battery
	DISP_DRV_ADDR_OZ,					///8 Ounce
	DISP_DRV_ADDR_LB,					///9 Pound
	DISP_DRV_ADDR_KG,					//10 Kilo gram
	DISP_DRV_ADDR_GRAM, 				//11 Gram
	DISP_DRV_ADDR_PCS,					//12 Counting mode
	{0, 0}, 							//13 Prepack
	DISP_DRV_ADDR_HI,					//14 Compare high
	DISP_DRV_ADDR_LOW,					//15 Compare low
	DISP_DRV_ADDR_OK,					//16 Compare OK 
	{0, 0}, 							//17 Euro
	{0, 0}, 							//18 Hold
	{0, 0}, 							//19 Backlight
	DISP_DRV_ADDR_LACK,		 			//20 Counting lack
	{0, 0}, 							//21 Unit price 100Gram
	{0, 0},								//22 Tare save
	{0, 0}, 							//23 Both save
	{0, 0}, 							//24 Unit price Kilo Gram
	{0, 0},								//25 Unit price Pound
	{0, 0}, 							//26 Percent
	{0, 0},								//27 Sum
	{0, 0},								//28 full battery
	{0, 0},								//29 charging battery
	{0, 0},								//30 Euro per Kilogram
	{0, 0},								//31 $
	{0, 0},								//32 $/kg
	{0, 0},								//33 Unit price oz
	{0, 0}								//34 Connected AC
};

/** @brief Dot addr and mask*/
const INDICATOR_ADDR code dispDrvDotAddrTable[6] =
{
  	{14, 0x01}, {12, 0x01}, {10, 0x01}, {8, 0x01}, {6, 0x01}, {4, 0x00} 
};

/** @brief Comma addr and mask*/
const INDICATOR_ADDR code dispDrvCommaAddrTable[6] =
{
        {0, 0x02}, {0, 0x01}, {0, 0x08}, {0, 0x04}, {1, 0x01}, {0, 0x00} 
};

/** @brief s0~s39 �� ����Ʈ ���� �ּ�*/
INT8U code dispDrvCharAddrTable[6] = { 14, 12, 10, 8, 6, 4};


/** 
* @brief Menu Mode���� ���� Window ��ȣ Table
* @remark String Window, Aux. Data window, Data Window ������ (see DISP_MENU_STR_WIN_NUM~)\n
*         String Window�� Data Window�� ������ Menu Mode���� Window�� 1���� ������ �ν���\n
*         Window�� 1���� ��� �ݵ�� ���� �� ��!!!
*/
const INT8U code dispDrvWinTableForMenu[DISP_MAX_WIN_NUM_FOR_MENU] = 
{
	WIN_WEIGHT,	WIN_UNIT_PRICE, WIN_WEIGHT
};

/** 
* @brief �� Window�� Size Table
* @remark WIN_WEIGHT, WIN_UNIT_PRICE, WIN_TOTAL_PRICE, WIN_TARE ������ (see WIN_WEIGHT~)
*/
const INT8U code dispDrvWinSizeTable[WIN_MAX_NUM] = 
{
//	5, 0, 0, 0
	6, 0, 0, 0
};


/**
* @brief �� Window�� '-' sign ���� Table 
* @remark WIN_WEIGHT, WIN_UNIT_PRICE, WIN_TOTAL_PRICE, WIN_TARE ������ (see WIN_WEIGHT~)
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
static void ch462WriteCommand(INT8U datcom);
static void ch462TranCommand(INT8U command);      
static void ch462TranData(INT8U disdata);
static void ch462WriteAllData();         

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
/**
********************************************************************************
* @brief    Display Driver Init �Լ�\n
*           Display Driver IC Init\n
*           Display Driver ���� ���� �ʱ�ȭ(Message, Window, Page)\n
* @param    none
* @return   none
* @remark   �ʱ⿡ �ѹ� ���� �Ǵ� �Լ�\n
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

	ch462WriteCommand(CH462_ON);	
	ch462WriteCommand(CH462_SYS_EN); /* Turn on system oscillator*/	
//	ht1622WriteCommand(HT1622_TONE_OFF); /* Turn off tone outputs */		
//	ch462WriteCommand(CH462_RC_256K); /* System clock source, on-chip RC oscillator*/
//	ht1622WriteCommand(HT1622_TIMER_DIS);
//	ht1622WriteCommand(HT1622_WDT_DIS);
//	ht1622WriteCommand(HT1622_IRQ_DIS);
	ch462WriteCommand(CH462_BIAS_1_3);	
//	ht1622WriteCommand(HT1622_IRQ_DIS);	/* Disable IRQ output */	
//	ht1622WriteCommand(HT1622_NORMAL);	/* Normal mode*/

	DispDrvClearBuf();
	ch462WriteAllData();
}

/**
********************************************************************************
* @brief    Display API ���� ����� �� Window ũ�⸦ ��ȯ��\n
* @param    winNum : Window ��ȣ
* @return   Display Driver�� ���� ��ȯ ���� Window Size
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
* @brief    �� Window�� �������� Minus Indicator�� ���� �ִ����� ��ȯ��\n
* @param    winNum : Window ��ȣ
* @return   0 : ����, 1 : ����
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
* @brief    Menu Mode���� ����� Window�� Display Driver�� ���� �޾Ƽ� ��ȯ��\n
* @param    num : menu ��� Window ��� ��ȣ
* @return   Display Driver�� ���� ��ȯ ���� Window ��ȣ
* @remark   menu Mode���� ����� Window�� Display Driver�� �����ϱ� ����
*           ��� �Ǵ� �Լ� ��\n
*           DISP_MENU_STR_WIN_NUM, DISP_MENU_AUX_WIN_NUM, DISP_MENU_DATA_WIN_NUM
********************************************************************************
*/
INT8U DispDrvGetMenuWinByNum(INT8U num)
{
	return dispDrvWinTableForMenu[num];
}

/**
********************************************************************************
* @brief    Display Driver mode ����\n
* @param    mode : display mode ��ȣ
* @return   none
* @remark   DISP_DRV_NORMAL_MODE : �Ϲ� ��� (DISP_DRV_NORMAL_PAGE, DISP_DRV_MSG_PAGE)
*           DISP_DRV_SUSPEND_MODE : ��� ��� (DISP_DRV_SUSPEND_PAGE)
*           ��� ����� ��� Driver �ܿ��� ��� ��� ���� (TO DO)
********************************************************************************
*/
void DispDrvSetMode(INT8U mode)
{
	dispDrvMode = mode;
}

/**
********************************************************************************
* @brief    IC�� ������ Display Driver Image page ����\n
* @param    mode : display image page ��ȣ
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
* @brief    Editting�� Display Driver Image page ����\n
* @param    mode : display image page ��ȣ
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
}

/**
********************************************************************************
* @brief    window�� 1 ���� ����ϴ� �Լ� (editting Page)
* @param    winNum : Window ��ȣ\n
*           pos : Window���� ��ġ (0~ )
*           cdata : ���� data
* @return   none
* @remark   
********************************************************************************
*/
void DispDrvSetChar(INT8U winNum, INT8U pos, char cdata)
{
	INT8U byte;
	INT8U addr;
	INT8U mask;

	addr = dispDrvCharAddrTable[pos];	//14
	byte = FontTable[cdata - 0x20];	//0x77
	mask = dispDrvBuf[dispEditingPage][addr] & 0x01;
	dispDrvBuf[dispEditingPage][addr] = (byte >> 4) | mask;
	addr++;
	dispDrvBuf[dispEditingPage][addr] = byte;
}

/**
********************************************************************************
* @brief    Editting Image�� �Է� ���� ���ڷ� ä��
* @param    fillChar : ä�� ����
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
* @brief    Editting Image�� ��� Indicator ���� �Լ�
* @param    on : 1 -> ��, 0 -> ��
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
* @brief    Window�� String ��� (editting Page)
* @param    winNum : Window ��ȣ\n
*           str : ����� String Pointer
* @return   none
* @remark   str ä���� ���� �κ��� space�� ä��
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
			dispChar = dispChar - 0x20;	//('A' = 'a' - 0x20) �ҹ���->�빮��
		}
		if (dispChar < 0x20 || dispChar > '_')
		{
			dispChar = 0x20;	//space
		}		
		addr = dispDrvCharAddrTable[i];
		byte = FontTable[dispChar - 0x20];
		mask = dispDrvBuf[dispEditingPage][addr] & 0x01;
		dispDrvBuf[dispEditingPage][addr] = (byte >> 4) | mask;
		addr++;
		dispDrvBuf[dispEditingPage][addr] = byte;
	}
}

/**
********************************************************************************
* @brief    Window�� Const String ��� (editting Page)
* @param    winNum : Window ��ȣ\n
*           str : ����� Const String Pointer
* @return   none
* @remark   str ä���� ���� �κ��� space�� ä��
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
			dispChar = dispChar - 0x20;	//('A' = 'a' - 0x20) �ҹ���->�빮��
		}
		if (dispChar < 0x20 || dispChar > 'Z')
		{
			dispChar = 0x20;	//space
		}		
		addr = dispDrvCharAddrTable[i];
		byte = FontTable[dispChar - 0x20];
		mask = dispDrvBuf[dispEditingPage][addr]&0x01;
		dispDrvBuf[dispEditingPage][addr] = (byte >> 4) | mask;
		addr++;
		dispDrvBuf[dispEditingPage][addr] = byte;
	}
}

/**
********************************************************************************
* @brief    Window�� Decimal Point Position, Type ���� Setting (editting Page)
* @param    winNum : Window ��ȣ\n
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
* @brief    Indicator on/off setting �Լ� (editting Page)
* @param    num : Indicator ��ȣ\n
*           on : 1 -> �ѱ�, 0 -> ����
* @return   none
* @remark   Indicator ��ȣ ���ǵǾ� ���� (INDICATOR_WEIGHT_MINUS...)
********************************************************************************
*/
void DispDrvSetIndicator(INT8U num, INT8U on)
{
	INT8U addr;
	INT8U mask;
	INT8U prevOnNum;

	prevOnNum = 0;
	if (num == INDICATOR_LOW) 
	{
		
		dispDrvIndicatorStatusLow = on;
//		//"OK"�� ���ְ� "LO"�� ����� ���߿� "OK"�� �Ҵ�
//		if (on == OFF && dispDrvIndicatorStatusOk == ON)
//		{
//			prevOnNum = INDICATOR_OK;
//		}
	}
	else if (num == INDICATOR_OK)
	{
		dispDrvIndicatorStatusOk = on;
//		//"LO"�� ���ְ� "OK"�� ����� ���߿� "LO"�� �Ҵ�
//		if (on == OFF && dispDrvIndicatorStatusLow == ON)
//		{
//			prevOnNum = INDICATOR_LOW;
//		}
	}
	else if (num == INDICATOR_GRAM)
	{
		dispDrvIndicatorStatusGram = on;
		//"kg"�� ���ְ� "g"�� ����� ���߿� "kg"�� �Ҵ�
		if (on == OFF && dispDrvIndicatorStatusKg == ON)
		{
			prevOnNum = INDICATOR_KG;
		}
	}
	else if (num == INDICATOR_KG)
	{
		dispDrvIndicatorStatusKg = on;
		if (dispDrvIndicatorStatusKg == ON)
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
}

/**
********************************************************************************
* @brief    Bettery Energy Indicator setting �Լ� (editting Page)
* @param    percent : Battery �� (%)\n
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
* @remark   dispTransPage�� Image�� Display Driver IC�� ����
********************************************************************************
*/
void DispDrvProc(void)
{
          ch462WriteCommand(CH462_ON);
          ch462WriteCommand(CH462_SYS_EN); /* Turn on system oscillator*/	
//          ch462WriteCommand(CH462_RC_256K); /* System clock source, on-chip RC oscillator*/
          ch462WriteCommand(CH462_BIAS_1_3);	          
          ch462WriteAllData();                
}

/**
********************************************************************************
* @brief    Display Driver Procedure\n
* @param    none
* @return   none
* @remark   dispTransPage�� Image�� Display Driver IC�� ����
********************************************************************************
*/
void DispDrvTurnOnOff(BOOLEAN onOff)
{
	if(onOff)
	{
		ch462TranCommand(CH462_ON);
	}
	else
	{
		ch462TranCommand(CH462_OFF);
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
* @brief    Dot Indicator setting �Լ� (editting Page)
* @param    winNum : Window ��ȣ\n
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
* @brief    Comma Indicator setting �Լ� (editting Page)
* @param    winNum : Window ��ȣ\n
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
}

/**
********************************************************************************
* @brief    CH462 IC�� command ���� �Լ�
* @param    datcom : command\n
* @return   none
* @remark   
********************************************************************************
*/
void ch462WriteCommand(INT8U datcom)         
{
	CH462_CS_LOW;

	CH462_WR_LOW;	
	CH462_SDA_HIGH;
	delay2Clock();

	CH462_WR_HIGH;  ///1
	delay2Clock();

	CH462_WR_LOW;
	CH462_SDA_LOW;
	delay2Clock();

	CH462_WR_HIGH;  //0
	delay2Clock();

	CH462_WR_LOW ;
	CH462_SDA_LOW ;
	delay2Clock();

	CH462_WR_HIGH;  //0
	delay2Clock();

	ch462TranCommand(datcom);

	CH462_WR_LOW;
	delay2Clock();

	CH462_WR_HIGH;  
	delay2Clock();

	CH462_CS_HIGH;
}

/**
********************************************************************************
* @brief    CH462 IC�� command byte ���� �Լ�
* @param    command : command\n
* @return   none
* @remark   
********************************************************************************
*/
void ch462TranCommand(INT8U command)      
{
    INT8U k;
    
    for (k = 0; k < 8; k++)
	{
		CH462_WR_LOW;
		//Send most significant bit first 
		if ((command & 0x80) == 0)
   		{
			CH462_SDA_LOW;
		}
		else
		{
			CH462_SDA_HIGH;
		}
		delay2Clock();

		CH462_WR_HIGH;   //Data are latched into the CH462
		command <<= 1;
		delay2Clock();
	}  
}

/**
********************************************************************************
* @brief    CH462 IC�� data byte ���� �Լ�
* @param    disdata : data\n
* @return   none
* @remark   
********************************************************************************
*/
void ch462TranData(INT8U disdata)       
{
	INT8U k;

	for (k = 0; k < 4; k++)
	{
		CH462_WR_LOW;
        if(disdata & 0x01)    
        {
            CH462_SDA_HIGH;
        }
        else
        {
            CH462_SDA_LOW;
        }
        delay2Clock();
        
//        LCD_DATA = disdata & 0x01;
		CH462_WR_HIGH;    
		disdata >>= 1;
		delay2Clock();
	}  
}

/**
********************************************************************************
* @brief    CH462 IC�� dispTransPage�� Image�� Display Driver IC�� ����
* @param    disdata : data\n
* @return   none
* @remark   
********************************************************************************
*/
void ch462WriteAllData()         
{
	INT8U i;
	INT8U *dispBufPtr;
        
	CH462_CS_LOW;
	CH462_WR_LOW;
	delay2Clock();

	CH462_SDA_HIGH;
	delay2Clock();

	CH462_WR_HIGH;
	delay2Clock();

	ch462TranCommand(0x40); //Write from memory address 0

#ifdef SWAP_DISP_DRIVER_SEG_PIN
	dispBufPtr = &dispDrvBuf[dispTransPage][DISP_DRV_BUF_SIZE-1];
	for (i = 0; i < DISP_DRV_BUF_SIZE; i++)
	{   
		ch462TranData(*dispBufPtr);
		dispBufPtr--;
	}
#else
	dispBufPtr = &dispDrvBuf[dispTransPage][0];
	for (i = 0; i < DISP_DRV_BUF_SIZE; i++)
	{   
		ch462TranData(*dispBufPtr);
		dispBufPtr++;
	}
#endif
	CH462_CS_HIGH;
}

#endif//#ifdef DISP_DRV_CH462_M0516LDN_SW2W_LCD

