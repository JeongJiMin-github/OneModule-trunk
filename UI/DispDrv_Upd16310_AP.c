/**
********************************************************************************
* Copyright (c) 2007 CAS
* @brief   Display Driver Source File\n
           for AP
* @file    DispDrv.c
* @version 1.0
* @date    2007/06/05
* @author  JAE JOON Hong (HJJ)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2007/04/05 created by HJJ \n
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
#ifdef	DISP_DRV_UPD16310_AP_VFD
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
/** @brief Display의 최대 밝기 값 */
#define DISP_DRV_MAX_BRIGHT	7
/** @brief Display의 Default 밝기 값 */
#define DISP_DRV_NORMAL_BRIGHT	1
/** @brief Display의 suspend mode 밝기 값 */
#define DISP_DRV_SUSPEND_BRIGHT	0

/** @brief Display 갯수*/
#define DISP_DRV_WIN_MAX_NUM	3

/** @brief Display driver buffer size*/
#define DISP_DRV_BUF_SIZE	19

/** @brief Display driver Unit price address*/
#define DISP_DRV_UNIT_PRICE_ADDR 0
#define DISP_DRV_UNIT_PRICE_LASTADDR 5
/** @brief Unit price display size*/
#define DISP_DRV_UNIT_PRICE_SIZE 6

/** @brief Display driver weight address*/
#define DISP_DRV_WEIGHT_ADDR 6
#define DISP_DRV_WEIGHT_LASTADDR 11
/** @brief weight display size*/
#define DISP_DRV_WEIGHT_SIZE 6

/** @brief Display driver Total price address*/
#define DISP_DRV_TOTAL_PRICE_ADDR 12
#define DISP_DRV_TOTAL_PRICE_LASTADDR 18
/** @brief Total price display size*/
#define DISP_DRV_TOTAL_PRICE_SIZE 7

/** @brief VFD MAXIMUM SIZE*/
#define DISP_DRV_MAX_VFD_DIGIT 19

/** @brief Euro indicator */
#define DISP_DRV_ADDR_EURO				{16, 0x0200}//5j total price
/** @brief USD indicator*/
#define DISP_DRV_ADDR_USD				{15, 0x0200}//4j total price
/** @brief bothsave indicator*/
#define DISP_DRV_ADDR_BOTHSAVE			{13, 0x0200}//2j total price
/** @brief Prepack indicator*/
#define DISP_DRV_ADDR_PREPACK			{12, 0x0200}//1j total price
/** @brief TareSave indicator */
#define DISP_DRV_ADDR_TARESAVE			{11, 0x0200}//6 weight
/** @brief Zero indicator */
#define DISP_DRV_ADDR_ZERO				{10, 0x0200}//5j weight
/** @brief NET indicator */
#define DISP_DRV_ADDR_NET				{9, 0x0200}//4j weight
/** @brief lb indicator*/
#define DISP_DRV_ADDR_LB				{7, 0x0200}//2j weight
/** @brief Kilogram indicator*/
#define DISP_DRV_ADDR_KG				{6, 0x0200}//1j weight
/** @brief Euro per kg indicator */
#define DISP_DRV_ADDR_EURO_KG			{4, 0x0200}//5j unit price
/** @brief Euro per kg indicator */
#define DISP_DRV_ADDR_USD_KG			{3, 0x0200}//4j unit price
/** @brief Currency per 100g indicator*/
#define DISP_DRV_ADDR_UNITPRICE_100G	{2, 0x0200}//3j unit price
/** @brief $ per lb  indicator */
#define DISP_DRV_ADDR_UNITPRICE_LB		{1, 0x0200}//2j unit price
/** @brief $ per kg indicator*/
#define DISP_DRV_ADDR_UNITPRICE_KG		{0, 0x0200}//1j unit price


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

/** @brief DOT_MASK*/
#define DISP_DRV_DOT_MASK		0x80
/** @brief CHAR_MASK*/
#define DISP_DRV_CHAR_MASK		0x7F
/** @brief DOT_MASK*/
#define DISP_DRV_COMMA_MASK		0x0100

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
	INT16U mask;
} INDICATOR_ADDR;
/*
union
{
	INT32U	VFD_Scan;
	INT8U 	VFD_Digit[4];
} dis_position;
*/
/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/
/* Insert file scope variable & tables here */
/** @brief Back Light 밝기 조절 Count 변수 */
static data INT8U dispPwmCount;//data for high speed in interrupt
/** @brief Back Light 밝기 조절 Max Count 변수 */
static data INT8U dispPwmMaxCount;//data for high speed in interrupt

static const INT8U code dispOnTable[2][DISP_DRV_MAX_BRIGHT] =
{
//	{ 0, 0, 0, 0, 0, 0, 0},//0
//	{ 1, 0, 0, 0, 0, 0, 0},//1
	{ 1, 0, 0, 1, 0, 0, 0},//2
//	{ 1, 0, 1, 0, 1, 0, 0},//3
	{ 1, 1, 0, 1, 1, 0, 0},//4
//	{ 1, 1, 1, 0, 1, 1, 0},//5
//	{ 1, 1, 1, 1, 1, 1, 0},//6
//	{ 1, 1, 1, 1, 1, 1, 1} //7
};

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
static DISP_DRV_DATA idata dispDrvStruct[DISP_DRV_WIN_MAX_NUM];
/** @brief Image Buffer : 실제 Display Driver IC에 존재하는 메모리의 복사본 */
static INT16U dispDrvBuf[DISP_DRV_PAGE_SIZE-1][DISP_DRV_BUF_SIZE];//except suspend page
/** @brief Image Buffer : 실제 Display Driver IC에 존재하는 메모리의 복사본 */
static INT16U idata dispDrvBufForInterrupt[DISP_DRV_BUF_SIZE];
/** @brief Display Driver Mode */
static INT8U idata dispDrvMode;
/** @brief Display Driver IC에 보낼 Image Buffer 번호 */
static INT8U idata dispTransPage;
/** @brief Display Driver에서 Edit시 사용하는 Image Buffer 번호 */
static INT8U idata dispEditingPage;
/** @brief Display Driver에서 Display를 순차적으로 동작시키기 위한 Counting Buffer 번호 */
static INT8U idata dispIndex;

static UNION_INT32U idata dispPosition;

				 
/** 
* @brief Display Driver에서 Indicator Display시 사용하는 struct, Indicator 개수 만큼 존재 
* @remark 배열에서 Indicator위치는 define되어 있음\n
*         임의로 위치 변경하지 마시오.
*         관련 정의를 보려면 DISP_DRV_ADDR_WEIGHT_MINUS를 찾아 보시오.
*/
const INDICATOR_ADDR code dispDrvIndicatorAddrTable[INDICATOR_MAX_NUM+2] =
{
	{0, 0}, 							///0 Weight Minus
	{0, 0}, 							///1 Unit price Minus
	{0, 0}, 							///2 Total price Minus
	{0, 0}, 							///3 Tare Minus
	DISP_DRV_ADDR_ZERO,					///4 Zero
	{0, 0},								///5 Stable
	DISP_DRV_ADDR_NET,					///6 Net
	{0, 0},								///7 Low battery
	{0, 0}, 							///8 Ounce
	DISP_DRV_ADDR_LB, 					///9 Pound
	DISP_DRV_ADDR_KG, 		 			//10 Kilo gram
	{0, 0}, 							//11 Gram
	{0, 0}, 							//12 Counting mode
	DISP_DRV_ADDR_PREPACK,				//13 Prepack
	{0, 0}, 							//14 Compare high
	{0, 0}, 							//15 Compare low
	{0, 0}, 							//16 Compare OK 
	DISP_DRV_ADDR_EURO,					//17 Euro								
	{0, 0}, 							//18 Hold
	{0, 0},								//19 Backlight
	{0, 0}, 							//20 Counting lack
	DISP_DRV_ADDR_UNITPRICE_100G,		//21 Unit price 100Gram
	DISP_DRV_ADDR_TARESAVE,				//22 Tare save
	DISP_DRV_ADDR_BOTHSAVE,				//23 Both save
	DISP_DRV_ADDR_UNITPRICE_KG, 		//24 Unit price Kilo Gram
	DISP_DRV_ADDR_UNITPRICE_LB, 		//25 Unit price Pound
	{0, 0}, 							//26 Percent
	{0, 0},								//27 Sum
	{0, 0}, 							//28 full battery
	{0, 0},								//29 charging battery
	DISP_DRV_ADDR_EURO_KG,				//30 Euro per Kilogram
	DISP_DRV_ADDR_USD,					//31 $
	DISP_DRV_ADDR_USD_KG,				//32 $/kg
	{0, 0}, 							//33 Unit price oz
	{0, 0}								//34 Connected AC
};

/** 
* @brief Menu Mode에서 사용될 Window 번호 Table
* @remark String Window, Aux. Data window, Data Window 순서임 (see DISP_MENU_STR_WIN_NUM~)
*/
const INT8U code dispDrvWinTableForMenu[DISP_MAX_WIN_NUM_FOR_MENU] = 
{
	WIN_WEIGHT, WIN_UNIT_PRICE, WIN_TOTAL_PRICE
};

/**
* @brief 각 Window의 Size Table
* @remark WIN_WEIGHT, WIN_UNIT_PRICE, WIN_TOTAL_PRICE, WIN_TARE 순서임 (see WIN_WEIGHT~)
*/
const INT8U code dispDrvWinSizeTable[WIN_MAX_NUM] = 
{
	6, 6, 7, 0
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
//static void dispDrvSetBattIndicator(INT8U num, INT8U on);
      
static void upd16310TranSeg(void);
static void upd16310TranDigit(void);
		   
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
	dispIndex = 0;

	DispDrvClearBuf();
	DispDrvSetMode(DISP_DRV_NORMAL_MODE);
	DispDrvSetPage(DISP_DRV_NORMAL_PAGE);

	dispDrvStruct[WIN_UNIT_PRICE].addr = DISP_DRV_UNIT_PRICE_ADDR; //0
	dispDrvStruct[WIN_UNIT_PRICE].size = DISP_DRV_UNIT_PRICE_SIZE; //6

	dispDrvStruct[WIN_WEIGHT].addr = DISP_DRV_WEIGHT_ADDR; //6
	dispDrvStruct[WIN_WEIGHT].size = DISP_DRV_WEIGHT_SIZE; //5
	
	dispDrvStruct[WIN_TOTAL_PRICE].addr = DISP_DRV_TOTAL_PRICE_ADDR; //11
	dispDrvStruct[WIN_TOTAL_PRICE].size = DISP_DRV_TOTAL_PRICE_SIZE; //6

	DispDrvClearBuf();

	dispPwmMaxCount = DISP_DRV_NORMAL_BRIGHT;
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
	if (page < (DISP_DRV_PAGE_SIZE - 1))
	{
		dispTransPage = page;
	}
	if (page == DISP_DRV_SUSPEND_PAGE)
	{
		dispPwmMaxCount = DISP_DRV_SUSPEND_BRIGHT;
	}
	else
	{
		dispPwmMaxCount = DISP_DRV_NORMAL_BRIGHT;
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

	for (page = 0; page < (DISP_DRV_PAGE_SIZE - 1); page++) //except suspend page 
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
	INT16U byte;
	INT8U addr;

	if (dispEditingPage == DISP_DRV_SUSPEND_PAGE) return;//suspend page isn't exist

	//addr = dispDrvStruct[winNum].addr;
	if(winNum == WIN_WEIGHT)
	{						  
	 	addr = DISP_DRV_WEIGHT_LASTADDR;
	}
	else if(winNum == WIN_UNIT_PRICE)
	{
		addr = DISP_DRV_UNIT_PRICE_LASTADDR;
	}
	else if(winNum == WIN_TOTAL_PRICE)		
	{
		addr = DISP_DRV_TOTAL_PRICE_LASTADDR;
	}

	byte = dispDrvBuf[dispEditingPage][addr - pos];
 	byte = byte & (~DISP_DRV_CHAR_MASK);

	dispDrvBuf[dispEditingPage][addr - pos] = (FontTable[cdata - 0x20] & DISP_DRV_CHAR_MASK) | byte; //0x20 space
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
void DispDrvSetStr(INT8U winNum, char *str)	 	   //string 출력 함수
{
	INT8U i;
	INT8U dispChar;
	INT16U tempWord;
	INT8U addr;
	INT8U strEndFlag;
	INT8U dataSize;

	if (dispEditingPage == DISP_DRV_SUSPEND_PAGE) return;//suspend page isn't exist

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
	if(winNum == WIN_WEIGHT)
	{						  
	 	addr = DISP_DRV_WEIGHT_LASTADDR;
	}
	else if(winNum == WIN_UNIT_PRICE)
	{
		addr = DISP_DRV_UNIT_PRICE_LASTADDR;
	}
	else if(winNum == WIN_TOTAL_PRICE)		
	{
		addr = DISP_DRV_TOTAL_PRICE_LASTADDR;
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
		tempWord = dispDrvBuf[dispEditingPage][addr - i];
		tempWord &= ~DISP_DRV_CHAR_MASK;
		dispDrvBuf[dispEditingPage][addr - i] = (FontTable[dispChar - 0x20] & DISP_DRV_CHAR_MASK) | tempWord;	
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
	INT16U tempWord;
	INT8U addr;
	INT8U strEndFlag;
	INT8U dataSize;

	if (dispEditingPage == DISP_DRV_SUSPEND_PAGE) return;//suspend page isn't exist

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
	 
 	if(winNum == WIN_WEIGHT)
	{						  
	 	addr = DISP_DRV_WEIGHT_LASTADDR;
	}
	else if(winNum == WIN_UNIT_PRICE)
	{
		addr = DISP_DRV_UNIT_PRICE_LASTADDR;
	}
	else if(winNum == WIN_TOTAL_PRICE)		
	{
		addr = DISP_DRV_TOTAL_PRICE_LASTADDR;
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
		tempWord = dispDrvBuf[dispEditingPage][addr - i];
		tempWord &= ~DISP_DRV_CHAR_MASK;
		dispDrvBuf[dispEditingPage][addr - i] = (FontTable[dispChar - 0x20] & DISP_DRV_CHAR_MASK) | tempWord;
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
	INT16U mask;

	if (dispEditingPage == DISP_DRV_SUSPEND_PAGE) return;//suspend page isn't exist

	addr = dispDrvIndicatorAddrTable[num].addr;
	mask = dispDrvIndicatorAddrTable[num].mask;

	if (on)
	{
		dispDrvBuf[dispEditingPage][addr] = dispDrvBuf[dispEditingPage][addr] | mask;
	}
	else
	{
		dispDrvBuf[dispEditingPage][addr] = dispDrvBuf[dispEditingPage][addr] & ~mask;
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

	if (dispTransPage < DISP_DRV_SUSPEND_PAGE) //dispTransPage is not exist (just blank)
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
			dispDrvBufForInterrupt[i] = 0x00;
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
 
	//if (dispEditingPage == DISP_DRV_SUSPEND_PAGE) return;//suspend page isn't exist

	addr = dispDrvStruct[winNum].addr + decPos;

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
	INT8U addr;
 
	//if (dispEditingPage == DISP_DRV_SUSPEND_PAGE) return;//suspend page isn't exist

	addr = dispDrvStruct[winNum].addr + decPos;

	if (on)
	{
		dispDrvBuf[dispEditingPage][addr] = dispDrvBuf[dispEditingPage][addr] | DISP_DRV_COMMA_MASK;
	}
	else
	{
		dispDrvBuf[dispEditingPage][addr] = dispDrvBuf[dispEditingPage][addr] & (~DISP_DRV_COMMA_MASK);
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
/*
void dispDrvSetBattIndicator(INT8U num, INT8U on)
{
	INT8U addr;
	INT8U mask;

	if (dispEditingPage == DISP_DRV_SUSPEND_PAGE) return;//suspend page isn't exist

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
* @brief    HT1622 IC에 command byte 전송 함수
* @param    segment : data\n
* @return   none
* @remark   
********************************************************************************
*/

void upd16310TranSeg(void)      
{
	INT16U reverse, tempInt16u;

	tempInt16u = dispDrvBufForInterrupt[dispIndex];

	reverse = ~tempInt16u;				 //함수 처리속도 개선을 위한 변수 분리

	VFD_CLK = 1;
	VFD_DATA = reverse & 0x200;
	VFD_CLK = 0;				

	VFD_CLK = 1;
	VFD_DATA = reverse & 0x100;
	VFD_CLK = 0;				

	VFD_CLK = 1;
	VFD_DATA = reverse & 0x80;
	VFD_CLK = 0;				

	VFD_CLK = 1;
	VFD_DATA = reverse & 0x40;
	VFD_CLK = 0;				

	VFD_CLK = 1;
	VFD_DATA = reverse & 0x20;
	VFD_CLK = 0;				

	VFD_CLK = 1;
	VFD_DATA = reverse & 0x10;
	VFD_CLK = 0;				

	VFD_CLK = 1;
	VFD_DATA = reverse & 0x08;
	VFD_CLK = 0;				

	VFD_CLK = 1;
	VFD_DATA = reverse & 0x04;
	VFD_CLK = 0;				

	VFD_CLK = 1;
	VFD_DATA = reverse & 0x02;
	VFD_CLK = 0;				

	VFD_CLK = 1;
	VFD_DATA = reverse & 0x01;
	VFD_CLK = 0;				

	VFD_DATA = 0;
}

/**
********************************************************************************
* @brief    UPD16310 IC에 data byte 전송 함수
* @param    disdata : data\n
* @return   none
* @remark   
********************************************************************************
*/
const INT32U code dispDrvDigitTable[DISP_DRV_MAX_VFD_DIGIT] = 
{
	0x000001, 0x000002, 0x000004, 0x000008, 0x000010, 0x000020, 0x000040, 0x000080,
	0x000100, 0x000200, 0x000400, 0x000800, 0x001000, 0x002000, 0x004000, 0x008000,
	0x010000, 0x020000, 0x040000
};

void upd16310TranDigit(void)       
{
	INT8U firstPos;
	INT8U secondPos;
	INT8U thirdPos;
	
	dispPosition.dword = ~(dispDrvDigitTable[dispIndex]);

	thirdPos 		= dispPosition.byteArray.byte[3];	  //함수 처리속도 개선을 위한 변수 분리
	secondPos 	= dispPosition.byteArray.byte[2];
	firstPos 		= dispPosition.byteArray.byte[1];

	VFD_CLK = 1;
	VFD_DATA = thirdPos & 0x01;
	VFD_CLK = 0;			

	VFD_CLK = 1;
	VFD_DATA = thirdPos & 0x02;
	VFD_CLK = 0;			

	VFD_CLK = 1;
	VFD_DATA = thirdPos & 0x04;
	VFD_CLK = 0;			

	VFD_CLK = 1;
	VFD_DATA = thirdPos & 0x08;
	VFD_CLK = 0;			

	VFD_CLK = 1;
	VFD_DATA = thirdPos & 0x10;
	VFD_CLK = 0;			

	VFD_CLK = 1;
	VFD_DATA = thirdPos & 0x20;
	VFD_CLK = 0;			

	VFD_CLK = 1;
	VFD_DATA = thirdPos & 0x40;
	VFD_CLK = 0;			

	VFD_CLK = 1;
	VFD_DATA = thirdPos & 0x80;
	VFD_CLK = 0;			


	VFD_CLK = 1;
	VFD_DATA = secondPos & 0x01;
	VFD_CLK = 0;			

	VFD_CLK = 1;
	VFD_DATA = secondPos & 0x02;
	VFD_CLK = 0;			

	VFD_CLK = 1;
	VFD_DATA = secondPos & 0x04;
	VFD_CLK = 0;			

	VFD_CLK = 1;
	VFD_DATA = secondPos & 0x08;
	VFD_CLK = 0;			

	VFD_CLK = 1;
	VFD_DATA = secondPos & 0x10;
	VFD_CLK = 0;			

	VFD_CLK = 1;
	VFD_DATA = secondPos & 0x20;
	VFD_CLK = 0;			

	VFD_CLK = 1;
	VFD_DATA = secondPos & 0x40;
	VFD_CLK = 0;			

	VFD_CLK = 1;
	VFD_DATA = secondPos & 0x80;
	VFD_CLK = 0;			


	VFD_CLK = 1;
	VFD_DATA = firstPos & 0x01;
	VFD_CLK = 0;				

	VFD_CLK = 1;
	VFD_DATA = firstPos & 0x02;
	VFD_CLK = 0;				

	VFD_CLK = 1;
	VFD_DATA = firstPos & 0x04;
	VFD_CLK = 0;		
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
	if (dispOnTable[dispPwmMaxCount][dispPwmCount])
	{
		VFD_POWER = 1;
	}
	else
	{
		VFD_POWER = 0;
	}
	dispPwmCount++;
	dispPwmCount = dispPwmCount % DISP_DRV_MAX_BRIGHT;
	
	upd16310TranDigit(); //19

	upd16310TranSeg();	//10

	VFD_STRB = 0;
	delay1Clock();
	VFD_STRB = 1;
	delay1Clock();
	VFD_BLK = 0; 	
	
	KeyOutputPortStatus = dispIndex;

	dispIndex++;
	dispIndex %= DISP_DRV_MAX_VFD_DIGIT;
}
#endif//#ifdef DISP_DRV_Upd16310_AP


