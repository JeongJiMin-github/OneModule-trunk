/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Display API Source File\n
* @file    DispApi.c
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
#include "../Common/CommonLib.h"
#include "../Common/RingBuf.h"
#include "../Common/UiDefine.h"
#include "../SystemTimer/TimerApi.h"
#include "../DB/StorageApi.h"
#include "../DB/ParamApi.h"
#include "../Hardware/HardwareConfig.h"
#include "KeyApi.h"
#include "DispApi.h"
#include "DispDrv.h"
#include "BuzzerApi.h"
#include "BackLightDrv.h"

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
#ifdef USE_BL_BRIGHT_DEFAULT_7
#define BL_BRIGHT 7
#else
#define BL_BRIGHT 3
#endif

/*
********************************************************************************
*                       LOCAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert local typedef & structure here */
/** @brief Display API에서 사용하는 Window관련 struct */
typedef _PACKED_DEF struct
{
	/** @brief Window digit size */
	INT8U size;
	/** @brief Window Decimal Position */
	INT8U decPos;
	/** @brief Window Decimal Point Type */
	INT8U decType;
	/** @brief Window Thousand Rel. Position from decPos*/
	INT8U useThousandPt;
	/** @brief Window '-' sign existance flag */
	INT8U minusSignExist;
} DISP_DATA;


/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/
/* Insert file scope variable & tables here */
/** @brief Display API에서 사용하는 Window struct */
static DISP_DATA dispStruct[WIN_MAX_NUM];

//Message Display 관련 변수
/** @brief Message Display : Error 또는 Help가 Display될 Window 번호 */
static INT8U dispMsgWinNum;
/** @brief Message Display : Blink시 켜지는 시간(단위 : 100ms) */
static INT8U dispMsgOn100msTime;
/** @brief Message Display : Blink시 꺼지는 시간(단위 : 100ms) */
static INT8U dispMsgOff100msTime;
/** @brief Message Display : Blink하기 위한 Count(State) */
static INT16U dispMsgBlinkCount;

static BOOLEAN dispSuspendFlag;
static BOOLEAN dispOnOffFlag;
static INT8U dispBlinkCount;
#ifdef USE_CTS_MODE 
BOOLEAN dispMsgStopFlag;
#endif
#ifdef USE_DISPLAY_SUM_TOTAL
INT8U sumDisplayStatus;
#endif
/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */


/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
#ifdef USE_DISPLAY_SUM_TOTAL
extern void changeWeightUnitIndicator(void);
extern void clearWeightUnitIndicator(void);
extern void displayTotalWeight(void);
extern void displayTotalCount(void);
#endif

/**
********************************************************************************
* @brief    Display API Parameter를 default 값으로 초기화\n
* @param    type : defualt type
* @return   none
* @remark   Parameter 영역에 초기값을 넣는다.\n
*           Cal Mode나 Program 모드에서 전체 초기화할 때 실행한다.
********************************************************************************
*/
void DispSetDefaultParam(void)
{
	ParamWriteByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BL_BRIGHT, BL_BRIGHT);
	ParamWriteByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BLINK_COUNT, 0); // Default :Use BATTLED On , USE [Deciamal Point] key
}

/**
********************************************************************************
* @brief    Display API Init 함수\n
*           Display Driver Init\n
*           Display API 관련 변수 초기화(Message, Window, Page)\n
* @param    none
* @return   none
* @remark   초기에 한번 실행 되는 함수\n
********************************************************************************
*/
void DispInit(void)
{	
	BlDrvInit(ParamReadByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BL_BRIGHT));
	
	DispDrvInit(ParamReadByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BL_BRIGHT));

	DispDrvSetEditingPage(DISP_DRV_NORMAL_PAGE);

	dispStruct[WIN_WEIGHT].size = DispDrvGetWinSize(WIN_WEIGHT);
	dispStruct[WIN_UNIT_PRICE].size = DispDrvGetWinSize(WIN_UNIT_PRICE);
	dispStruct[WIN_TOTAL_PRICE].size = DispDrvGetWinSize(WIN_TOTAL_PRICE);
	dispStruct[WIN_TARE].size = DispDrvGetWinSize(WIN_TARE);

	dispStruct[WIN_WEIGHT].minusSignExist = DispDrvGetMinusSignExist(WIN_WEIGHT);
	dispStruct[WIN_UNIT_PRICE].minusSignExist = DispDrvGetMinusSignExist(WIN_UNIT_PRICE);
	dispStruct[WIN_TOTAL_PRICE].minusSignExist = DispDrvGetMinusSignExist(WIN_TOTAL_PRICE);
	dispStruct[WIN_TARE].minusSignExist = DispDrvGetMinusSignExist(WIN_TARE);

	DispSetDecPoint(WIN_WEIGHT, 3, DEC_TYPE_DOT, 0);
	DispSetDecPoint(WIN_UNIT_PRICE, 2, DEC_TYPE_DOT, 0);
	DispSetDecPoint(WIN_TOTAL_PRICE, 2, DEC_TYPE_DOT, 0);
	DispSetDecPoint(WIN_TARE, 0, DEC_TYPE_DOT, 0);

	dispMsgWinNum = DispGetMenuWinByNum(DISP_MENU_STR_WIN_NUM);
	dispMsgOn100msTime = 0;
	dispMsgOff100msTime = 0;
	dispMsgBlinkCount = 0;
	dispSuspendFlag = OFF;
	dispBlinkCount = ParamReadByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BLINK_COUNT);

	DispTurnOnOff(ON);
}

/**
********************************************************************************
* @brief    Display Driver image Clear all (All Page, digit, triangle, decimal)
* @param    none
* @return   none
* @remark   Display Procedure 함수 실행된 후 실제 Display 반영됨
********************************************************************************
*/
void DispClearAll(void)
{
	DispDrvClearBuf();
}

/**
********************************************************************************
* @brief    현재 Page의 전체 Window의 digit만 지움(triangle 및 decimal은 유지)
* @param    none
* @return   none
* @remark    
********************************************************************************
*/
void DispClearAllDigits(void)
{
#ifdef USE_UNIT_OZ_LCD_CLEAR
    DispDrvOzLcdClear();
#endif
	DispSetConstStr(WIN_WEIGHT, "");
	DispSetConstStr(WIN_UNIT_PRICE, "");
	DispSetConstStr(WIN_TOTAL_PRICE, "");
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
void DispSetChar(INT8U winNum, INT8U pos, char cdata)
{
	DispDrvSetChar(winNum, pos, cdata);
}

/**
********************************************************************************
* @brief    Message Page의 전체 Window의 digit만 지움(triangle 및 decimal은 유지)
* @param    none
* @return   none
* @remark   Message 표시 후 다음 표시될 Message를 위해 지움 
********************************************************************************
*/
void DispClearAllDigitsMsg(void)
{
	DispDrvSetEditingPage(DISP_DRV_MSG_PAGE);
	DispClearAllDigits();
	DispDrvSetEditingPage(DISP_DRV_NORMAL_PAGE);
}

/**
********************************************************************************
* @brief    Window에 Setting 되어 있는 digit size 값을 반환
* @param    winNum : Window 번호
* @return   Window digit size
* @remark   
********************************************************************************
*/
INT8U DispGetWinSize(INT8U winNum)
{
	return dispStruct[winNum].size;
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
INT8U DispGetMenuWinByNum(INT8U num)
{
	return DispDrvGetMenuWinByNum(num);
}

/**
********************************************************************************
* @brief    Window에 Setting 되어 있는 Decimal Point Position 값을 반환
* @param    winNum : Window 번호
* @return   Decimal Point Position
* @remark   xxx -> 0, xx.x -> 1, x,xx -> 2
********************************************************************************
*/
INT8U DispGetDecPos(INT8U winNum)
{
	return dispStruct[winNum].decPos;
}

/**
********************************************************************************
* @brief    Window에 Setting 되어 있는 Decimal Point Type 값을 반환
* @param    winNum : Window 번호
* @return   Decimal Point Type
* @remark   DEC_TYPE_DOT, DEC_TYPE_COMMA
********************************************************************************
*/
INT8U DispGetDecType(INT8U winNum)
{
	return dispStruct[winNum].decType;
}

/**
********************************************************************************
* @brief    Window에 Setting 되어 있는 Thousand Point Use Flag를 반환
* @param    winNum : Window 번호
* @return   Decimal Point Type
* @remark   DEC_TYPE_DOT, DEC_TYPE_COMMA
********************************************************************************
*/
INT8U DispGetUseThousandPt(INT8U winNum)
{
	return dispStruct[winNum].useThousandPt;
}

/**
********************************************************************************
* @brief    Window에 Decimal Point Position, Type 값을 Setting
* @param    winNum : Window 번호\n
*           decPos : Decimal Point Position\n
*           decType : Decimal Point Type\n
* @return   Decimal Point Type
* @remark   DEC_TYPE_DOT, DEC_TYPE_COMMA
********************************************************************************
*/
void DispSetDecPoint(INT8U winNum, INT8U decPos, INT8U decType, INT8U useThousandPt)
{
	dispStruct[winNum].decPos = decPos;
	dispStruct[winNum].decType = decType;
	dispStruct[winNum].useThousandPt = useThousandPt;
	DispDrvSetDecPoint(winNum, decPos, decType, useThousandPt);
}

/**
********************************************************************************
* @brief    Window에 String 출력 (normal Page)
* @param    winNum : Window 번호\n
*           str : 출력할 String Pointer
* @return   none
* @remark   String 출력 시 Decimal Point 지움
*           normal page -> sale mode, menu mode에서 기본으로 사용 되는 page
*           Display Procedure 함수 실행된 후 실제 Display 반영됨
********************************************************************************
*/
void DispSetStr(INT8U winNum, char *str)
{
	DispDrvSetStr(winNum, str);
	DispDrvSetDecPoint(winNum, 0, DEC_TYPE_DOT, 0);
}

/**
********************************************************************************
* @brief    Window에 Const String 출력 (normal Page)
* @param    winNum : Window 번호\n
*           str : 출력할 Const String Pointer
* @return   none
* @remark   String 출력 시 Decimal Point 지움\n
*           Const로 선언된 String 사용 시 RAM 용량 늘어나지 않음
*           Display Procedure 함수 실행된 후 실제 Display 반영됨
********************************************************************************
*/
void DispSetConstStr(INT8U winNum, const char *str)
{
	DispDrvSetConstStr(winNum, str);
	DispDrvSetDecPoint(winNum, 0, DEC_TYPE_DOT, 0);
}

/**
********************************************************************************
* @brief    Window에 Decimal Number(숫자) Data 출력 (normal Page)
* @param    winNum : Window 번호\n
*           num : 출력할 data (signed data)
*           fillChar : 유효하지 않은 영역 채울 문자 (0 = space로 감)
* @return   none
* @remark   Window에 기설정된 Decimal Point 출력\n
*           Window 크기 초과 하면 "------" 출력\n
*           data가 음수이면 '-' sign 처리함\n
*           Display Procedure 함수 실행된 후 실제 Display 반영됨
********************************************************************************
*/
void DispSetNum(INT8U winNum, INT32S num, INT8U fillChar)
{
	char str[8];
	INT8U auxMinusSignFlag;
	INT8U decPos;
	INT8U size;
	
	decPos = dispStruct[winNum].decPos;
	size = dispStruct[winNum].size;

	if (CalcDecDigitLen(num) > size)
	{
		//DispSetConstStr(winNum, "------");
		num /= 10;
		auxMinusSignFlag = Int32sToDecStr(str, num, size, fillChar, decPos, 0, 0);
		if (auxMinusSignFlag == 1 && dispStruct[winNum].minusSignExist == 0)
		{
			DispSetConstStr(winNum, "------");
		}
		else
		{
			DispSetStr(winNum, str);
			DispDrvSetIndicator(winNum - INDICATOR_WEIGHT_MINUS, auxMinusSignFlag);
			if ((num / DecPowerConvTable[decPos]) >= 1000)
			{
				DispDrvSetDecPoint(winNum, decPos, dispStruct[winNum].decType, dispStruct[winNum].useThousandPt);
			}
			else
			{
				DispDrvSetDecPoint(winNum, decPos, dispStruct[winNum].decType, 0);
			}			
		}
	}
	else
	{
		auxMinusSignFlag = Int32sToDecStr(str, num, size, fillChar, decPos, 0, 0);
		if (auxMinusSignFlag == 1 && dispStruct[winNum].minusSignExist == 0)
		{
			DispSetConstStr(winNum, "------");
		}
		else
		{
			DispSetStr(winNum, str);
			DispDrvSetIndicator(winNum - INDICATOR_WEIGHT_MINUS, auxMinusSignFlag);
			if ((num / DecPowerConvTable[decPos]) >= 1000)
			{
				DispDrvSetDecPoint(winNum, decPos, dispStruct[winNum].decType, dispStruct[winNum].useThousandPt);
			}
			else
			{
				DispDrvSetDecPoint(winNum, decPos, dispStruct[winNum].decType, 0);
			}			
		}
	}
}

/**
********************************************************************************
* @brief    Window에 Decimal Number(숫자) Data 출력 (normal Page)
* @param    winNum : Window 번호\n
*           num : 출력할 data (signed data)
*           digitSize : data의 digit size (digit size까지는 '0'을 채운다.)
* @return   none
* @remark   Window에 기설정된 Decimal Point 출력\n
*           Display Procedure 함수 실행된 후 실제 Display 반영됨
********************************************************************************
*/
/*
void DispSetDecNum(INT8U winNum, INT32S num, INT8U digitSize)
{
	char str[8];
	INT8U decPos;
	
	decPos = dispStruct[winNum].decPos;
	
	if (digitSize == 0) 
	{
		digitSize = 1;
	}
	if (digitSize <= decPos) 
	{
		digitSize = decPos + 1;
	}

	Int32sToDecStr(str, num, dispStruct[winNum].size, 0, digitSize - 1, 0, 0);
	DispSetStr(winNum, str);
	DispDrvSetIndicator(winNum - INDICATOR_WEIGHT_MINUS, 0);
	
	if ((num / DecPowerConvTable[decPos]) >= 1000)
	{
		DispDrvSetDecPoint(winNum, decPos, dispStruct[winNum].decType, dispStruct[winNum].useThousandPt);
	}
	else
	{
		DispDrvSetDecPoint(winNum, decPos, dispStruct[winNum].decType, 0);
	}
}
*/
/**
********************************************************************************
* @brief    Window에 Decimal Number(숫자) Data 와 const string 출력 (normal Page) 
* @param    winNum : Window 번호\n
*           src : const string pointer\n
*           num : 출력할 data (signed data)\n
*           digitSize : data의 digit size (digit size까지는 '0'을 채운다.)
* @return   none
* @remark   src == 0이면 const string 출력 하지 않음\n
*           Window에 기설정된 Decimal Point 출력\n
*           Display Procedure 함수 실행된 후 실제 Display 반영됨
********************************************************************************
*/
void DispSetConstStrWithDecNum(INT8U winNum, const char *src, INT32S num, INT8U digitSize)
{
	char str[8];
	INT8U decPos;
	
	decPos = dispStruct[winNum].decPos;
	
	if (digitSize == 0) 
	{
		digitSize = 1;
	}
	if (digitSize <= decPos)
	{
		digitSize = decPos + 1;
	}

	MakeStrWithDecNum(str, src, num, dispStruct[winNum].size, digitSize);

	DispSetStr(winNum, str);
	DispDrvSetIndicator(winNum - INDICATOR_WEIGHT_MINUS, 0);
	
	if ((num / DecPowerConvTable[decPos]) >= 1000)
	{
		DispDrvSetDecPoint(winNum, decPos, dispStruct[winNum].decType, dispStruct[winNum].useThousandPt);
	}
	else
	{
		DispDrvSetDecPoint(winNum, decPos, dispStruct[winNum].decType, 0);
	}
}

/**
********************************************************************************
* @brief    Window에 Hexa Number(숫자) Data 출력 (normal Page)
* @param    winNum : Window 번호\n
*           num : 출력할 data (unsigned data)
*           digitSize : data의 digit size (digit size까지는 '0'을 채운다.)
* @return   none
* @remark   Decimal Point 지움\n
*           Display Procedure 함수 실행된 후 실제 Display 반영됨
********************************************************************************
*/
/*
void DispSetHexNum(INT8U winNum, INT32U num, INT8U digitSize)
{
	char str[8];

	Int32uToHexStr(str, num, dispStruct[winNum].size, digitSize);
	DispSetStr(winNum, str);
	DispDrvSetIndicator(winNum - INDICATOR_WEIGHT_MINUS, 0);
	DispDrvSetDecPoint(winNum, 0, 0, 0);
}
*/
/**
********************************************************************************
* @brief    display string data + Hexa data 
* @param    winNum : Window 번호\n
*           src : const string pointer\n
*           num : 출력할 data (unsigned data)\n
*           digitSize : data의 digit size (digit size까지는 '0'을 채운다.)
* @return   none
* @remark   src == 0이면 const string 출력 하지 않음
********************************************************************************
*/
void DispSetConstStrWithHexNum(INT8U winNum, const char *src, INT32U num, INT8U digitSize)
{
	char str[8];

	MakeStrWithHexNum(str, src, num, dispStruct[winNum].size, digitSize);

	DispSetStr(winNum, str);
	DispDrvSetIndicator(winNum - INDICATOR_WEIGHT_MINUS, 0);
	DispDrvSetDecPoint(winNum, 0, 0, 0);
}

/**
********************************************************************************
* @brief    char 데이터의 실제 문자와 char 데이터의 Hexa data를 String으로 만들어 
* @param    winNum : Window 번호\n
*           cData : String으로 변환할 Data\n
* @return   none
* @remark   예) cData = 0x41, display "  A 41"
********************************************************************************
*/
void DispSetCharWithHexNum(INT8U winNum, INT8U cData)
{
	char str[8];
	INT8U size;

	size = dispStruct[winNum].size;
	Int32uToHexStr(str, (INT32U)cData, size, 2);
	if (size > 3) {
		if (cData == 0x00)
		{
			str[size - 4] = 0x20;//prevent null char (null->space)
		}
		else
		{
			str[size - 4] = cData;
		}
	}
	str[size] = 0x00;

	DispSetStr(winNum, str);
}

/**
********************************************************************************
* @brief    Indicator on/off setting 함수 (normal Page)
* @param    num : Indicator 번호\n
*           on : 1 -> 켜기, 0 -> 끄기
* @return   none
* @remark   Indicator 번호 정의되어 있음 (INDICATOR_WEIGHT_MINUS...)
*           Display Procedure 함수 실행된 후 실제 Display 반영됨
********************************************************************************
*/
void DispSetIndicator(INT8U num, INT8U on)
{
	DispDrvSetIndicator(num, on);
}

/**
********************************************************************************
* @brief    Bettery Energy Indicator setting 함수 (normal Page)
* @param    percent : Battery 양 (%)\n
* @return   none
* @remark   Display Procedure 함수 실행된 후 실제 Display 반영됨
********************************************************************************
*/
void DispSetBattEnergy(INT8U percent)
{
	DispDrvSetBattEnergy(percent);
}

/**
********************************************************************************
* @brief    Window에 String 출력 (message Page)
* @param    winNum : Window 번호\n
*           str : 출력할 String Pointer
* @return   none
* @remark   Message Page에 출력, see DispSetStr()
*           DispSetRunMsgPage(...) 실행 하여야 실제 동작함
********************************************************************************
*/
void DispSetStrMsg(INT8U winNum, char *msg)
{
	DispDrvSetEditingPage(DISP_DRV_MSG_PAGE);
	DispSetStr(winNum, msg);
	DispDrvSetEditingPage(DISP_DRV_NORMAL_PAGE);
}

/**
********************************************************************************
* @brief    Window에 Const String 출력 (message Page)
* @param    winNum : Window 번호\n
*           str : 출력할 Const String Pointer
* @return   none
* @remark   Message Page에 출력, see DispSetConstStr()
*           DispSetRunMsgPage(...) 실행 하여야 실제 동작함
********************************************************************************
*/
void DispSetConstStrMsg(INT8U winNum, const char *msg)
{
	DispDrvSetEditingPage(DISP_DRV_MSG_PAGE);
	DispSetConstStr(winNum, msg);
	DispDrvSetEditingPage(DISP_DRV_NORMAL_PAGE);
}

/**
********************************************************************************
* @brief    Window에 Decimal Number(숫자) Data 출력 (message Page)
* @param    winNum : Window 번호\n
*           num : 출력할 data (signed data)
* @return   none
* @remark   Message Page에 출력, see DispSetNum()
*           DispSetRunMsgPage(...) 실행 하여야 실제 동작함
********************************************************************************
*/
void DispSetNumMsg(INT8U winNum, INT32S num, INT8U fillChar)
{
	DispDrvSetEditingPage(DISP_DRV_MSG_PAGE);
	DispSetNum(winNum, num, fillChar);
	DispDrvSetEditingPage(DISP_DRV_NORMAL_PAGE);
}

/**
********************************************************************************
* @brief    Display msg string with number data \n
* @param    cmsg, num
* @return   
* @remark   
********************************************************************************
*/
void DispSetMsgWithNum(const char *cmsg, INT16U num)
{
	char msg[8];

	MakeStrWithNum(msg, cmsg, num, 5);

	DispDrvSetEditingPage(DISP_DRV_MSG_PAGE);
	DispSetStr(dispMsgWinNum, msg);
	DispDrvSetEditingPage(DISP_DRV_NORMAL_PAGE);

	DispSetRunMsgPage(10, 1);
}

#ifdef USE_VERSION_STR_DOT
/**
********************************************************************************
* @brief    Display msg string with version data include dot \n
* @param    cmsg, num
* @return   
* @remark   
********************************************************************************
*/
void DispSetVersionWithDot(INT16U num)
{
	char msg[6];
	INT8U version_leng = 0;

#ifdef USE_VERSION_NUM_3_DIGIT
	version_leng = 4;
#else
	num *= 10; //version 4자리로 변경 220 -> 2200
	version_leng = 5;
#endif

	MakeStrWithNum(msg, "v", num, version_leng);

	DispDrvSetEditingPage(DISP_DRV_MSG_PAGE);
	DispDrvSetStr(dispMsgWinNum, msg);
#ifdef USE_VERSION_NUM_3_DIGIT
	DispDrvSetDecPoint(dispMsgWinNum, 0xFC, DEC_TYPE_DOT, 0);
#else
	DispDrvSetDecPoint(dispMsgWinNum, 0xFE, DEC_TYPE_DOT, 0);
#endif
	DispDrvSetEditingPage(DISP_DRV_NORMAL_PAGE);

	DispSetRunMsgPage(10, 1);
}
#endif

#ifdef USE_UNIT_1_16OZ
/**
********************************************************************************
* @brief    Window에 Fraction 출력 (editting Page)
* @param    INT8U winNum : Window 번호\n
*           INT8U numer :	numerator
*           INT8U denom : denominator
* @return   none
* @remark   
********************************************************************************
*/
void DispSetFraction(INT8U winNum, INT8U numer, INT8U denom)
{
	DispDrvSetFraction(winNum, numer, denom);
}
#endif

/**
********************************************************************************
* @brief    dispMsgWinNum에 "ERROR" + error number 출력 (message Page)
* @param    num : error number
* @return   none
* @remark   Message Page에 출력, Buzzer on, Message 동작 시킴 
********************************************************************************
*/
void DispSetError(INT8U num)
{
	DispSetMsgWithNum("ERR", num);
	BuzzerSetOnTimes(2);
}

/**
********************************************************************************
* @brief    dispMsgWinNum에 "HELP" + error number 출력 (message Page)
* @param    num : error number
* @return   none
* @remark   Message Page에 출력, Buzzer on, Message 동작 시킴 
********************************************************************************
*/
void DispSetHelp(INT8U num)
{
	DispSetMsgWithNum("HELP", num);
	BuzzerSetOnTimes(2);
}

/**
********************************************************************************
* @brief    Message page Setting 및 실행
* @param    onTime100ms : blink시 On 시간 최소값 5 (단위 : 100ms)\n
*           blinkCount : 깜박일 개수 (0: 안깜박임)
* @return   none
* @remark   blink시 Off 시간은 On 시간의 절반\n
*           DispProc()함수 거치지 않고 처음에는 즉시 Display에 적용됨
*           blink동작의 완료는 Display Procedure에서 마무리 됨
********************************************************************************
*/
void DispSetRunMsgPage(INT8U onTime100ms, INT8U blinkCount)
{
	if (onTime100ms < 5) 
	{
		onTime100ms = 5;
	}
#ifdef USE_DISPLAY_SUM_TOTAL	// 합산 정보 표시 중이 때 Hold 키 입력 받을 수 있게
	if( (sumDisplayStatus != DISPLAY_G_TOTAL_WEIGHT) && (sumDisplayStatus != DISPLAY_G_TOTAL_COUNT) ) // 합산정보 표시중이 아닐 때에만 키입력 방지
	{
		KeyEnable(OFF);//key 동작 방지
	}
#else
	KeyEnable(OFF);//key 동작 방지
#endif	

	if (blinkCount == 0)
	{
		dispMsgOn100msTime = onTime100ms;
		dispMsgOff100msTime = 0;
		dispMsgBlinkCount = 0;
	}
	else
	{
		dispMsgOn100msTime = onTime100ms;
		dispMsgOff100msTime = onTime100ms/2;
		dispMsgBlinkCount = blinkCount*2 - 1;
	}

	DispDrvSetPage(DISP_DRV_MSG_PAGE);
	TimerSetDelay100ms(TIMER_DELAY_100MS_DISP_MSG, dispMsgOn100msTime);

	DispDrvProc();//Display에 즉시 적용
}
#ifdef USE_FAST_DISPLAYING_WEIGHT
/**
********************************************************************************
* @brief    Display driver update 즉시 실행
* @param    
* @return   none
* @remark   
********************************************************************************
*/
void DispImmedietly(void)
{
	DispDrvProc();//Display에 즉시 적용
}
#endif
/**
********************************************************************************
* @brief    Display API Procedure\n
* @param    none
* @return   none
* @remark   Display Driver Procedure 실행\n
*           message page 처리 (see DispSetRunMsgPage(...))\n
*           일정 시간에 한번씩 계속 실행 되는 함수
********************************************************************************
*/

void DispProc(void)
{
	static INT16U prevDispTimer100ms = 0;
	INT16U currTimer100ms;
#ifdef USE_CTS_MODE 
	if(dispMsgStopFlag == ON)
	{
		if (KeyCheck())
		{
			TimerSetDelay100ms(TIMER_DELAY_100MS_DISP_MSG,0);
			dispMsgBlinkCount = 0;
			dispMsgStopFlag = OFF;
		}
	}
#endif	
	if (TimerCheckDelay100ms(TIMER_DELAY_100MS_DISP_MSG))
	{
		if (dispMsgBlinkCount == 0)
		{
#ifdef USE_DISPLAY_SUM_TOTAL
			if( (sumDisplayStatus == DISPLAY_TOTAL_WEIGHT) || (sumDisplayStatus == DISPLAY_G_TOTAL_WEIGHT) )
			{
				displayTotalWeight();
				sumDisplayStatus++;
			}
			else if( (sumDisplayStatus == DISPLAY_TOTAL_COUNT) || (sumDisplayStatus == DISPLAY_G_TOTAL_COUNT) )
			{
				displayTotalCount();
				sumDisplayStatus = DISPLAY_END;
			}
			else if(sumDisplayStatus == DISPLAY_INIT)
			{
				DispSetConstStrMsg(WIN_WEIGHT, " INIT");
				DispSetRunMsgPage(10, 0);
				sumDisplayStatus = DISPLAY_END;
			}
			else
			{
#endif
				KeyEnable(ON);//key 동작 허용
				TimerSetDelay100ms(TIMER_DELAY_100MS_DISP_MSG, 0);//Timer Stop
				DispClearAllDigitsMsg();//다음 Message를 위해 Clear
				DispDrvSetPage(DISP_DRV_NORMAL_PAGE);
#ifdef USE_DISPLAY_SUM_TOTAL
				sumDisplayStatus = DISPLAY_IDLE;
			}
#endif
		}
		else
		{
			if (dispMsgBlinkCount % 2 == 0) 
			{
				TimerSetDelay100ms(TIMER_DELAY_100MS_DISP_MSG, dispMsgOn100msTime);
				DispDrvSetPage(DISP_DRV_MSG_PAGE);
			}
			else
			{
				TimerSetDelay100ms(TIMER_DELAY_100MS_DISP_MSG, dispMsgOff100msTime);
				DispDrvSetPage(DISP_DRV_SUSPEND_PAGE);//SUSPEND page와 공유
			}
			dispMsgBlinkCount--;
		}
	}
	else
	{
#ifdef USE_DISPLAY_SUM_TOTAL
		if ((dispMsgBlinkCount == 0) && (sumDisplayStatus == DISPLAY_IDLE))
#else
		if (dispMsgBlinkCount == 0) 
#endif
		{		
			if (dispSuspendFlag)
			{
				DispDrvSetPage(DISP_DRV_SUSPEND_PAGE);
			}
			else
			{
				DispDrvSetPage(DISP_DRV_NORMAL_PAGE);
			}
		}
	}

	currTimer100ms = TimerGetSysTimer100ms();
	if (prevDispTimer100ms != currTimer100ms)
	{
		prevDispTimer100ms = currTimer100ms;

		//100 ms에 한번씩 실행
#ifdef USE_BACK_LIGHT
		if (BlDrvGetStatus()) 
		{
			DispSetIndicator(INDICATOR_BACKLIGHT, ON);
		}
		else
		{
			DispSetIndicator(INDICATOR_BACKLIGHT, OFF);
		}
#endif
		DispDrvProc();
	}
}

/**
********************************************************************************
* @brief    Display Test 함수\n
* @param    dispTestState : Window에 표시할 숫자 data
* @return   none
* @remark   모든 Window에 입력 받은 숫자 data 표시\n
*           짝수 일때 모든 indicator 끔\n
*           홀수 일때 모든 indicator 켬\n
*           battery 잔량 표시 (0..9)
********************************************************************************
*/
void DispTest(INT8U dispTestState)
{
	DispDrvFillChar(dispTestState + '0');
	if (dispTestState % 2) 
	{
		DispDrvSetAllIndicator(OFF);
	}
	else
	{
		DispDrvSetAllIndicator(ON);
	}
}

/**
********************************************************************************
* @brief    Back Light On/Off 함수\n
* @param    onOff (0-off, 1-on)
* @return   none
* @remark   
********************************************************************************
*/
void DispBlSetOnOff(BOOLEAN onOff)
{
	BlDrvSetOnOff(onOff);
}

/**
********************************************************************************
* @brief    Back Light 현재 상태 반환 함수\n
* @param    none
* @return   0-off, 1-on
* @remark   
********************************************************************************
*/
BOOLEAN DispBlGetStatus(void)
{
	return BlDrvGetStatus();
}

//-----------------------------------------------------------------
/**
********************************************************************************
* @brief    Back Light On/Off 함수\n
* @param    onOff (0-off, 1-on)
* @return   none
* @remark   
********************************************************************************
*/
void DispSetBlinkCount(INT8U cnt)
{
	dispBlinkCount = cnt;
}

/**
********************************************************************************
* @brief    Back Light 현재 상태 반환 함수\n
* @param    none
* @return   0-off, 1-on
* @remark   
********************************************************************************
*/
INT8U DispGetBlinkCount(void)
{
	return dispBlinkCount;
}

//----------------------------------------------------------------
/**
********************************************************************************
* @brief    Back Light 사용 유무 설정 함수\n
* @param    onOff (0-off, 1-on)
* @return   none
* @remark   Off일 경우 무조건 BL는 무조건 Off된다.
********************************************************************************
*/
void DispBlSetUseFlag(BOOLEAN onOff)
{
	BlDrvSetUseFlag(onOff);
}

/**
********************************************************************************
* @brief    Back Light 작동 유무 설정 함수\n
* @param    enbale (0-disable, 1-enable)
* @return   none
* @remark   disable일 경우 무조건 BL는 무조건 Off된다.
********************************************************************************
*/
void DispBlSetEnableFlag(BOOLEAN enable)
{
	BlDrvSetEnableFlag(enable);
}


/**
********************************************************************************
* @brief    Back Light Bright 설정 함수\n
* @param    level : 밝기 (1~DispBlGetMaxBright())
* @return   none
* @remark   
********************************************************************************
*/
void DispBlSetBright(INT8U level)
{
#ifdef USE_DISP_BRIGHT
	DispDrvSetBright(level);
#else
	BlDrvSetBright(level);
#endif
}

/**
********************************************************************************
* @brief    Back Light Maxinum Bright 설정값 반환 함수\n
* @param    
* @return   Maxinum Bright
* @remark   
********************************************************************************
*/
INT8U DispBlGetMaxBright(void)
{
	return BlDrvGetMaxBright();
}
/**
********************************************************************************
* @brief    Display on/off 설정 함수\n
* @param    on = 1 / off = 0
* @return   
* @remark   
********************************************************************************
*/
void DispTurnOnOff(BOOLEAN onOff)
{
	DispDrvTurnOnOff(onOff);
	dispOnOffFlag = onOff;
}

/**
********************************************************************************
* @brief    Display on/off 설정 함수\n
* @param    on = 1 / off = 0
* @return   
* @remark   
********************************************************************************
*/
BOOLEAN DispGetOnOffStatus(void)
{
	return dispOnOffFlag;
}

/**
********************************************************************************
* @brief    Display Suspend mode on/off 함수 \n
* @param    on = 1 / off = 0
* @return   
* @remark   
********************************************************************************
*/
void DispSuspendOnOff(BOOLEAN onOff)
{
	if (onOff)
	{
		DispDrvSetEditingPage(DISP_DRV_SUSPEND_PAGE);
		DispSetConstStr(WIN_WEIGHT, "  -  ");
		DispDrvSetEditingPage(DISP_DRV_NORMAL_PAGE);
		dispSuspendFlag = ON;
	}
	else
	{
		DispDrvSetEditingPage(DISP_DRV_SUSPEND_PAGE);
		DispSetConstStr(WIN_WEIGHT, "");
		DispDrvSetEditingPage(DISP_DRV_NORMAL_PAGE);
		dispSuspendFlag = OFF;
	}
}

/**
********************************************************************************
* @brief    Display Suspend mode return 함수 \n
* @param    
* @return   on = 1 / off = 0
* @remark   
********************************************************************************
*/
INT8U DispGetSuspendStatus(void)
{
	return dispSuspendFlag;
}
/**
********************************************************************************
* @brief    Display Tunr Off 함수 \n
* @param    
* @return   
* @remark   
********************************************************************************
*/
#if defined (USE_AC_POWER) || defined (USE_AC_BATT_POWER)
void DispTurnOff(void)
{
    DispClearAll();
	DispDrvProc();//Display에 즉시 적용
	DispDrvProcForInterrupt();
}
#endif
#ifdef USE_CTS_MODE 
/**
********************************************************************************
* @brief    CTS 3시에 울리는 알람을 키를 눌러서 도중에 중지할 수 있도록 하는 기능
* @param    
* @return   
* @remark   
********************************************************************************
*/
void DispSetMsgStop(void)
{
	dispMsgStopFlag = ON;
}
#endif
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
 



