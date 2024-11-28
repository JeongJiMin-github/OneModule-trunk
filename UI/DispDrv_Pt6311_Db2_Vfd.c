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
#ifdef DISP_DRV_PT6311_DB2_VFD
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
//#define DISP_DRV_BUF_SIZE	12
#define DISP_DRV_BUF_SIZE	13

/** @brief Display driver weight address*/
#define DISP_DRV_WEIGHT_ADDR 0
/** @brief weight display size*/
#define DISP_DRV_WEIGHT_SIZE 6
/** @brief weight display indicator address*/
#define DISP_DRV_WEIGHT_INDICATOR_ADDR 0
/** @brief Display driver weight address*/
#define DISP_DRV_LED_INDICATOR_ADDR (DISP_DRV_BUF_SIZE - 1)

/** @brief MINUS indicator, RB is minus sign is not exist, so use digit 0 for minus char*/
#define DISP_DRV_ADDR_WEIGHT_MINUS		{0, 0}
/** @brief Unit price minus indicator, not exist*/
#define DISP_DRV_ADDR_UNIT_PRICE_MINUS	{0, 0}
/** @brief Total price minus indicator, not exist*/
#define DISP_DRV_ADDR_TOTAL_PRICE_MINUS	{0, 0}
/** @brief Tare minus indicator, not exist*/
#define DISP_DRV_ADDR_TARE_MINUS			{0, 0}
/** @brief Stable indicator*/
#define DISP_DRV_ADDR_STABLE				{1, 0x02}
/** @brief NET indicator*/
#define DISP_DRV_ADDR_NET					{5, 0x02}
/** @brief Low Battery indicator*/
#define DISP_DRV_ADDR_LOW_BATT			{0, 0}
/** @brief Ounce indicator*/
#define DISP_DRV_ADDR_OZ					{0, 0}
/** @brief Pound indicator*/
#define DISP_DRV_ADDR_LB					{0, 0}
/** @brief Kilo gram indicator*/
#define DISP_DRV_ADDR_KG					{0, 0}
/** @brief Gram indicator*/
#define DISP_DRV_ADDR_GRAM				{0, 0}
/** @brief PCS indicator, for counting*/
#define DISP_DRV_ADDR_PCS					{7, 0x02}
/** @brief Zero indicator*/
#define DISP_DRV_ADDR_ZERO					{3, 0x02}
/** @brief PERCENT indicator*/
#define DISP_DRV_ADDR_PERCENT			{9, 0x02}
/** @brief SUM indicator*/
#define DISP_DRV_ADDR_SUM				{11, 0x02}
/** @brief High indicator, for comparator*/
#define DISP_DRV_ADDR_HI					{DISP_DRV_LED_INDICATOR_ADDR, 0x01}
/** @brief Low indicator, for comparator*/
#define DISP_DRV_ADDR_LOW					{DISP_DRV_LED_INDICATOR_ADDR, 0x04}
/** @brief OK indicator, for comparator*/
#define DISP_DRV_ADDR_OK					{DISP_DRV_LED_INDICATOR_ADDR, 0x02}


#define DISP_DRV_COMMAND_ON     0x08
#define DISP_DRV_COMMAND_OFF   (~DISP_DRV_COMMAND_ON)
#define DISP_DRV_COMMAND_ADDR_SET      0xC0
										 
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
#define FONT_0	0x3F
/** @brief FONT_1*/
#define FONT_1	0x06
/** @brief FONT_2*/
#define FONT_2	0x5B
/** @brief FONT_3*/
#define FONT_3	0x4F
/** @brief FONT_4*/
#define FONT_4	0x66
/** @brief FONT_5*/
#define FONT_5	0x6D
/** @brief FONT_6*/
#define FONT_6	0x7D
/** @brief FONT_7*/
#define FONT_7	0x27
/** @brief FONT_8*/
#define FONT_8	0x7F
/** @brief FONT_9*/
#define FONT_9	0x6F

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
#define FONT_B   0x7C
/** @brief FONT_C*/
#define FONT_C   0x39
/** @brief FONT_D*/
#define FONT_D   0x5E
/** @brief FONT_E*/
#define FONT_E   0x79
/** @brief FONT_F*/
#define FONT_F   0x71
/** @brief FONT_G*/
#define FONT_G   0x3D
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
//#define DISP_DRV_TRIANGLE_MASK	0x80
/** @brief DOT_MASK*/
#define DISP_DRV_DOT_MASK			0x80
/** @brief CHAR_MASK*/
#define DISP_DRV_CHAR_MASK		0x7F

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
//static INT8U dispDrvIndicatorStatusLow;
/** @brief Ok Indicator on/off ������ �����ϴ� ���� (Low"LO"�� 'O'�� �����ϱ� ������)*/
//static INT8U dispDrvIndicatorStatusOk;
/** @brief Gram Indicator on/off ������ �����ϴ� ���� (kg"kg"�� 'k'�� �����ϱ� ������)*/
//static INT8U dispDrvIndicatorStatusGram;
/** @brief Kg Indicator on/off ������ �����ϴ� ���� (gram"g"�� 'k'�� �����ϱ� ������)*/
//static INT8U dispDrvIndicatorStatusKg;

/** 
* @brief Display Driver���� Indicator Display�� ����ϴ� struct, Indicator ���� ��ŭ ���� 
* @remark �迭���� Indicator��ġ�� define�Ǿ� ����\n
*         ���Ƿ� ��ġ �������� ���ÿ�.
*         ���� ���Ǹ� ������ DISP_DRV_ADDR_WEIGHT_MINUS�� ã�� ���ÿ�.
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
	{0, 0}, 							///7 Low battery
	{0, 0}, 							///8 Ounce
	{0, 0}, 							///9 Pound
	{0, 0}, 							//10 Kilo gram
	{0, 0}, 							//11 Gram
	DISP_DRV_ADDR_PCS,					//12 Counting mode
	{0, 0},								//13 Prepack
	DISP_DRV_ADDR_HI, 					//14 Compare high
	DISP_DRV_ADDR_LOW, 					//15 Compare low
	DISP_DRV_ADDR_OK, 					//16 Compare OK 
	{0, 0}, 							//17 Euro
	{0, 0}, 							//18 Hold
	{0, 0}, 							//19 Backlight
	{0, 0}, 							//20 Counting lack
	{0, 0}, 							//21 Unit price 100Gram
	{0, 0}, 							//22 Tare save
	{0, 0}, 							//23 Both save
	{0, 0}, 							//24 Unit price Kilo Gram
	{0, 0},								//25 Unit price Pound
	DISP_DRV_ADDR_PERCENT, 				//26 Percent
	DISP_DRV_ADDR_SUM,					//27 Sum
	{0, 0},								//28 full battery
	{0, 0},								//29 charging battery
	{0, 0},								//30 Euro per Kilogram
	{0, 0}, 							//31 $
	{0, 0}, 							//32 $/kg
	{0, 0}, 							//33 Unit price oz
	{0, 0}								//34 Connected AC
};

/*
const INDICATOR_ADDR code dispDrvIndicatorAddrTable[INDICATOR_MAX_NUM] =
{
	DISP_DRV_ADDR_WEIGHT_MINUS,
	DISP_DRV_ADDR_UNIT_PRICE_MINUS,
	DISP_DRV_ADDR_TOTAL_PRICE_MINUS,
	DISP_DRV_ADDR_TARE_MINUS,
	DISP_DRV_ADDR_ZERO,
	DISP_DRV_ADDR_STABLE,
	DISP_DRV_ADDR_NET,
	DISP_DRV_ADDR_LOW_BATT,
	DISP_DRV_ADDR_OZ,
	DISP_DRV_ADDR_LB,
	DISP_DRV_ADDR_KG,
	DISP_DRV_ADDR_GRAM,
	DISP_DRV_ADDR_PCS,
	DISP_DRV_ADDR_HI,
	DISP_DRV_ADDR_LOW,
	DISP_DRV_ADDR_OK,
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0}
};
/*
/** @brief Dot addr and mask*/
const INDICATOR_ADDR code dispDrvDotAddrTable[6] =
{
	{0, 0x80}, {2, 0x80}, {4, 0x80}, {6, 0x80}, {8, 0x80}, {10, 0x80} 
};

/** @brief Comma addr and mask*/
const INDICATOR_ADDR code dispDrvCommaAddrTable[6] =
{
	{1, 0x01}, {3, 0x01}, {5, 0x01}, {7, 0x01}, {9, 0x01}, {11, 0x01} 
};

/** @brief s0~s39 �� ����Ʈ ���� �ּ�*/
INT8U code dispDrvCharAddrTable[6] = {0, 2, 4, 6, 8, 10};

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
	6, 0, 0, 0
};


/**
* @brief �� Window�� '-' sign ���� Table 
* @remark WIN_WEIGHT, WIN_UNIT_PRICE, WIN_TOTAL_PRICE, WIN_TARE ������ (see WIN_WEIGHT~)
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
static void pt6311WriteCommand(INT8U Data);
static void pt6311WriteByte(char Data);

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

	DispDrvClearBuf();

	VFD_STRB=1;
	VFD_CLK=1;
	pt6311WriteCommand(0x0f);//16 digits, 12 segments (default)
	pt6311WriteCommand(0x40);//normal operation mode, increment address after data has been written, write data to display mode
	DispDrvProc();
    pt6311WriteCommand(0x8f);//display on, pulse width = 14/16
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

	addr = dispDrvCharAddrTable[pos];
	byte = FontTable[cdata - 0x20];
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
	
	INT8U *p;
   	INT8U i;

	p = &dispDrvBuf[dispTransPage][0];
   
	pt6311WriteCommand(0x40);//normal operation mode, increment address after data has been written, write data to display mode

	for(i=0;i<6;i++)
   	{     		
		VFD_STRB = 0;
		delay2Clock();
		pt6311WriteByte(DISP_DRV_COMMAND_ADDR_SET | ((i)*3));
		pt6311WriteByte(*p);
   		p++;  
		pt6311WriteByte(*p);
		p++;
		VFD_STRB = 1;
   	}

	VFD_STRB = 0;
	delay2Clock();
	pt6311WriteByte(0x45);//normal operation mode, fixed address, write data to led port
	delay2Clock();
	pt6311WriteByte(~(*p));
	VFD_STRB = 1;

	pt6311WriteCommand(0x8f);//display on, pulse width = 14/16
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
		pt6311WriteCommand(0x8f);
	}
	else
	{
		pt6311WriteCommand(~0x87);
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
* @brief    HT1621 IC�� command ���� �Լ�
* @param    datcom : command\n
* @return   none
* @remark   
********************************************************************************
*/
void pt6311WriteCommand(char Data)	// Local Command
{     
	VFD_STRB= 0;
	delay2Clock();
	pt6311WriteByte(Data);
	VFD_STRB = 1;
	delay2Clock();
}

/**
********************************************************************************
* @brief    HT1621 IC�� command byte ���� �Լ�
* @param    command : command\n
* @return   none
* @remark   
********************************************************************************
*/
void pt6311WriteByte(char Data)
{
	VFD_CLK = 0;
	delay2Clock();
 	if(Data & 0x01) VFD_DATA = 1;
	else VFD_DATA = 0;
	VFD_CLK = 1;
	delay2Clock();
		
       Data = Data>>1;

	VFD_CLK = 0;
	delay2Clock();
 	if(Data & 0x01) VFD_DATA = 1;
	else VFD_DATA = 0;
	VFD_CLK = 1;
	delay2Clock();
		
       Data = Data>>1;

	VFD_CLK = 0;
	delay2Clock();
 	if(Data & 0x01) VFD_DATA = 1;
	else VFD_DATA = 0;
	VFD_CLK = 1;
	delay2Clock();
		
       Data = Data>>1;

	VFD_CLK = 0;
	delay2Clock();
 	if(Data & 0x01) VFD_DATA = 1;
	else VFD_DATA = 0;
	VFD_CLK = 1;
	delay2Clock();
		
       Data = Data>>1;

	VFD_CLK = 0;
	delay2Clock();
 	if(Data & 0x01) VFD_DATA = 1;
	else VFD_DATA = 0;
	VFD_CLK = 1;
	delay2Clock();
		
       Data = Data>>1;

	VFD_CLK = 0;
	delay2Clock();
 	if(Data & 0x01) VFD_DATA = 1;
	else VFD_DATA = 0;
	VFD_CLK = 1;
	delay2Clock();
		
       Data = Data>>1;

	VFD_CLK = 0;
	delay2Clock();
 	if(Data & 0x01) VFD_DATA = 1;
	else VFD_DATA = 0;
	VFD_CLK = 1;
	delay2Clock();
		
       Data = Data>>1;

	VFD_CLK = 0;
	delay2Clock();
 	if(Data & 0x01) VFD_DATA = 1;
	else VFD_DATA = 0;
	VFD_CLK = 1;
	delay2Clock();
		
       Data = Data>>1;
}        

#endif//#ifdef DISP_DRV_HT1621_DB2

