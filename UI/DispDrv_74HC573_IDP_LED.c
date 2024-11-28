/* @author  Yi Phyo Hong (HYP)
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
#include "../Hardware/HardwareDrv.h"

#ifdef DISP_DRV_74HC573_IDP_LED

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
/** @brief Display driver buffer size*/
#define DISP_DRV_BUF_SIZE		8

/** @brief Display driver module size*/
/*not used*///#define DSP_DRV_MODULE_SIZE	9
/** @brief Display driver module string size*/
/*not used*///#define DSP_DRV_MODULE_STR_SIZE	6

/** @brief Display driver weight address*/
#define DISP_DRV_WEIGHT_ADDR 0
/** @brief weight display size*/
#define DISP_DRV_WEIGHT_SIZE 6
/** @brief weight display indicator address*/
/*not used*///#define DISP_DRV_WEIGHT_INDICATOR_ADDR 28

/** @brief Display driver Unit price address*/
#define DISP_DRV_UNIT_PRICE_ADDR 0
/** @brief Unit price display size*/
#define DISP_DRV_UNIT_PRICE_SIZE 0
/** @brief Unit price display indicator address*/
/*not used*///#define DISP_DRV_UNIT_PRICE_INDICATOR_ADDR 12

/** @brief Display driver Total price address*/
#define DISP_DRV_TOTAL_PRICE_ADDR 0
/** @brief Total price display size*/
#define DISP_DRV_TOTAL_PRICE_SIZE 0

/** @brief Stable indicator*/
#define DISP_DRV_ADDR_STABLE			{6, 0x02}
/** @brief NET indicator*/
#define DISP_DRV_ADDR_NET				{6, 0x04}
/** @brief PCS indicator, for counting*/
#define DISP_DRV_ADDR_PCS				{6, 0x80}
/** @brief Zero indicator*/
#define DISP_DRV_ADDR_ZERO				{6, 0x01}
#define DISP_DRV_ADDR_LOW_BATT			{6, 0x08}
#define DISP_DRV_ADDR_BATT_CHARGE		{6, 0x10}
/** @brief High indicator, for comparator*/
#define DISP_DRV_ADDR_HI				{6, 0x20}
/** @brief Low indicator, for comparator*/
#define DISP_DRV_ADDR_LOW				{6, 0x40}
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
/** @brief FONT_B */
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


/** @brief TRIANGLE_MASK*/
//#define DISP_DRV_TRIANGLE_MASK	0x80
/** @brief DOT_MASK*/
//#define DISP_DRV_DOT_MASK			0x80
/** @brief CHAR_MASK*/
//#define DISP_DRV_CHAR_MASK		0x7F

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
static DISP_DRV_DATA dispDrvStruct[DISP_DRV_WIN_MAX_NUM];
/** @brief Image Buffer : ���� Display Driver IC�� �����ϴ� �޸��� ���纻 */
static INT8U idata dispDrvBuf[DISP_DRV_PAGE_SIZE][DISP_DRV_BUF_SIZE];
/** @brief Image Buffer : ���� Display Driver IC�� �����ϴ� �޸��� ���纻 */
//  static INT8U idata dispDrvBufForInterrupt[DISP_DRV_BUF_SIZE];
/** @brief Display Driver Mode */
// static INT8U dispDrvMode;
/** @brief Display Driver IC�� ���� Image Buffer ��ȣ */
static INT8U  dispTransPage;
/** @brief Display Driver���� Edit�� ����ϴ� Image Buffer ��ȣ */
static INT8U dispEditingPage;

/** @brief LED digit count for interrupt*/
INT8U digitCount;
static INT8U idata digitMask,digit_calDMask,digit_calSMask,digitRun;
static INT8U idata digitViewPort[8];//,digitChanged;
INT8U displaySleep;//,digitValue;
/**
INT8U digitCount;
static INT8U digitMask,digit_calDMask,digit_calSMask,digitRun;
static INT32U digitViewPort[8];
static INT8U digitChanged;
**/
// static INT8U keyInByte,keyInByte1;
//static INT8U digitP1,digitP2,digitP3;//,digitV;
//const  INT8U  code digitMasks[8] ={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
/** @brief LED */
// static UNION_INT32U digitOut;
// static INT32U digitOut;

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
	DISP_DRV_ADDR_LOW_BATT, 			///7 Low battery
	{0, 0}, 							///8 Ounce
	{0, 0},								///9 Pound
	{0, 0},								//10 Kilo gram
	{0, 0}, 							//11 Gram
	DISP_DRV_ADDR_PCS,					//12 Counting mode
	{0, 0}, 							//13 Prepack
	DISP_DRV_ADDR_HI, 					//14 Compare high
	DISP_DRV_ADDR_LOW,					//15 Compare low
	{0, 0}, 							//16 Compare OK 
	{0, 0}, 							//17 Euro
	{0, 0}, 							//18 Hold
	{0, 0}, 							//19 Backlight
	{0, 0}, 							//20 Counting lack
	{0, 0}, 							//21 Unit price 100Gram
	{0, 0},								//22 Tare save
	{0, 0}, 							//23 Both save
	{0, 0}, 							//24 Unit price Kilo Gram
	{0, 0},								//25 Unit price Pound
	{0, 0}, 							//26 Percent
	{0, 0},								//27 Sum
	{0, 0},								//28 full battery
	DISP_DRV_ADDR_BATT_CHARGE,			//29 charging battery
	{0, 0},								//30 Euro per Kilogram
	{0, 0},								//31 $
	{0, 0},								//32 $/kg
	{0, 0},								//33 Unit price oz
	{0, 0}								//34 Connected AC
};

/** @brief Dot addr and mask*/
const INDICATOR_ADDR code dispDrvDotAddrTable[6] =
{
	{0, 0x80}, {1, 0x80}, {2, 0x80}, {3, 0x80}, {4, 0x80}, {5, 0x80} 
};

/** @brief s0~s39 �� ����Ʈ ���� �ּ�*/
//INT8U code dispDrvCharAddrTable[5] = { 1, 2, 3, 4, 5};
//INT8U code dispDrvCharAddrTable[5] = { 5, 4, 3, 2, 1};
INT8U code dispDrvCharAddrTable[6] = { 0, 1, 2, 3, 4, 5};


/** 
* @brief Menu Mode���� ���� Window ��ȣ Table
* @remark String Window, Aux. Data window, Data Window ������ (see DISP_MENU_STR_WIN_NUM~)\n
*         String Window�� Data Window�� ������ Menu Mode���� Window�� 1���� ������ �ν���\n
*         Window�� 1���� ��� �ݵ�� ���� �� ��!!!
*/
const INT8U code dispDrvWinTableForMenu[DISP_MAX_WIN_NUM_FOR_MENU] = 
{
	WIN_WEIGHT,	0,0,
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
 **/

void DispDrvInit(INT8U brightness)
{
	for (digitCount=0; digitCount<8; digitCount++) {
		digitViewPort[digitCount]=0;
	}
	digitCount = 0;
	digitMask  = 0x01;
	digitRun   = 0;
	displaySleep=0;
	DispDrvClearBuf();
	DispDrvSetMode(DISP_DRV_NORMAL_MODE);
	DispDrvSetPage(DISP_DRV_NORMAL_PAGE);

	dispDrvStruct[WIN_WEIGHT].addr = DISP_DRV_WEIGHT_ADDR;
	dispDrvStruct[WIN_WEIGHT].size = DISP_DRV_WEIGHT_SIZE;
//	dispDrvStruct[WIN_WEIGHT].addr = 0;
//	dispDrvStruct[WIN_WEIGHT].size = 6;

//	dispDrvStruct[WIN_UNIT_PRICE].addr = DISP_DRV_UNIT_PRICE_ADDR;
//	dispDrvStruct[WIN_UNIT_PRICE].size = DISP_DRV_UNIT_PRICE_SIZE;

//	dispDrvStruct[WIN_TOTAL_PRICE].addr = DISP_DRV_TOTAL_PRICE_ADDR;
//	dispDrvStruct[WIN_TOTAL_PRICE].size = DISP_DRV_TOTAL_PRICE_SIZE;

	//dispDrvStruct[WIN_TARE].addr = 0;
	//dispDrvStruct[WIN_TARE].size = 0;

	DispDrvClearBuf();
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
//	dispDrvMode = mode;
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
			if (dispDrvBuf[page][addr]) {
//				digitChanged=1;
			}
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
//	INT8U mask;

	addr = dispDrvStruct[winNum].addr;
	addr += dispDrvCharAddrTable[pos];
	byte = FontTable[cdata - 0x20];
	if (dispDrvBuf[dispEditingPage][addr]!=byte) {
//		digitChanged=1;
	}
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
//	DispDrvProc();
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
		addr = dispDrvStruct[winNum].addr;		
		addr += dispDrvCharAddrTable[i];
		byte = FontTable[dispChar - 0x20];
		if (dispDrvBuf[dispEditingPage][addr]!=byte) {
//			digitChanged=1;
		}
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
		addr = dispDrvStruct[winNum].addr;
		addr += dispDrvCharAddrTable[i];
		byte = FontTable[dispChar - 0x20];
		if (dispDrvBuf[dispEditingPage][addr]!=byte) {
//			digitChanged=1;
		}
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

	if (displaySleep) return;

	if (winNum > DISP_DRV_WIN_MAX_NUM - 1)
	{
		return;
	}
	
	size = dispDrvStruct[winNum].size;
	
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
	if (num>=INDICATOR_MAX_NUM) num=INDICATOR_MAX_NUM-1;
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
//	digitChanged=1;
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
	INT8U si,di,pos,dv;

	if (displaySleep) {
		for (si=0; si<8; si++) {
			digitViewPort[si]=0;
		}
		digitViewPort[0]=0x20;	// a
		digitViewPort[1]=0x20;	// b
		digitViewPort[2]=0x20;	// c
		digitViewPort[3]=0x20;	// d
		digitViewPort[4]=0x20;	// e
		digitViewPort[5]=0x20;	// f
		if (dispDrvBuf[dispTransPage][6]&0x10) {
			digitViewPort[4]|=0x40;
		}
		return;
	}
	if (digitRun) return;
	di=0;
	digit_calDMask=0x01;
REPT2:
	digit_calSMask     =0x01;
	digitViewPort[di]=0;
/*	for (si=0; si<7; si++) {
		dv = dispDrvBuf[dispTransPage][si];
		dv&= digit_calDMask;
		if (dv) {
			digitViewPort[di]|=digit_calSMask;
		}
		digit_calSMask<<=1;
		if (digit_calSMask==0) digit_calSMask=0x01;
	}
 */
	dv = dispDrvBuf[dispTransPage][0]&digit_calDMask;
	if (dv) digitViewPort[di]|=0x01;
	dv = dispDrvBuf[dispTransPage][1]&digit_calDMask;
	if (dv) digitViewPort[di]|=0x02;
	dv = dispDrvBuf[dispTransPage][2]&digit_calDMask;
	if (dv) digitViewPort[di]|=0x04;
	dv = dispDrvBuf[dispTransPage][3]&digit_calDMask;
	if (dv) digitViewPort[di]|=0x08;
	dv = dispDrvBuf[dispTransPage][4]&digit_calDMask;
	if (dv) digitViewPort[di]|=0x10;
	dv = dispDrvBuf[dispTransPage][5]&digit_calDMask;
	if (dv) digitViewPort[di]|=0x20;
	dv = dispDrvBuf[dispTransPage][6]&digit_calDMask;
	if (dv) digitViewPort[di]|=0x40;

	digit_calDMask<<=1;
	di++;
	if (di<8) goto REPT2;
}

/**
********************************************************************************
* @brief    Display Driver Procedure\n
* @param    none
* @return   none
* @remark   dispTransPage�� Image�� Display Driver IC�� ����
********************************************************************************
*/
void DispDrvProcForInterrupt(void)
{
//	char digit1;

	digitRun=1;
	if (digitMask==0) digitMask=0x01;
	if (digitCount==8) digitCount=0;

	KEY_PORT_ZERO=1;
	KEY_PORT_TARE=1;
	KEY_PORT_MODE=1;

	LED_DIGIT_OUT_ENABLE = 0;	// 0:Output Enable
	LED_SEG_OUT_ENABLE   = 0;	// 0:Output Enable

//	digit1=3*digitCount;
	LED_SEG_DATA   = 0;
	LED_SEG1_LATCH = 1;
	delay1Clock();
	LED_SEG1_LATCH = 0;

	// show digit
	LED_DIGIT_DATA  = digitMask;
	LED_DIGIT_LATCH = 1;	// 0:Latch Enable
	delay1Clock();
	LED_DIGIT_LATCH = 0;	// 0:Latch Enable

	LED_SEG_DATA   = digitViewPort[digitCount];
	LED_SEG1_LATCH = 1;	// 0:Latch Enable
	delay1Clock();
	LED_SEG1_LATCH = 0;

	digitMask<<=1;
	digitCount++;
	digitRun=0; 
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
		//ht1622TranCommand(HT1622_ON);
	}
	else
	{
		//ht1622TranCommand(HT1622_OFF);
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

	if (displaySleep) return;
	pos = dispDrvStruct[winNum].size - decPos - 1;

	addr  = dispDrvStruct[winNum].addr;
	addr += dispDrvDotAddrTable[pos].addr;
	mask  = dispDrvDotAddrTable[pos].mask;

	if (on)
	{
		dispDrvBuf[dispEditingPage][addr] = dispDrvBuf[dispEditingPage][addr] | mask;
	}
	else
	{
		dispDrvBuf[dispEditingPage][addr] = dispDrvBuf[dispEditingPage][addr] & (~mask);
	}
}

#endif //#ifdef DISP_DRV_74HC573_ER_MLED

