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
/** @brief Display API���� ����ϴ� Window���� struct */
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
/** @brief Display API���� ����ϴ� Window struct */
static DISP_DATA dispStruct[WIN_MAX_NUM];

//Message Display ���� ����
/** @brief Message Display : Error �Ǵ� Help�� Display�� Window ��ȣ */
static INT8U dispMsgWinNum;
/** @brief Message Display : Blink�� ������ �ð�(���� : 100ms) */
static INT8U dispMsgOn100msTime;
/** @brief Message Display : Blink�� ������ �ð�(���� : 100ms) */
static INT8U dispMsgOff100msTime;
/** @brief Message Display : Blink�ϱ� ���� Count(State) */
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
* @brief    Display API Parameter�� default ������ �ʱ�ȭ\n
* @param    type : defualt type
* @return   none
* @remark   Parameter ������ �ʱⰪ�� �ִ´�.\n
*           Cal Mode�� Program ��忡�� ��ü �ʱ�ȭ�� �� �����Ѵ�.
********************************************************************************
*/
void DispSetDefaultParam(void)
{
	ParamWriteByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BL_BRIGHT, BL_BRIGHT);
	ParamWriteByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BLINK_COUNT, 0); // Default :Use BATTLED On , USE [Deciamal Point] key
}

/**
********************************************************************************
* @brief    Display API Init �Լ�\n
*           Display Driver Init\n
*           Display API ���� ���� �ʱ�ȭ(Message, Window, Page)\n
* @param    none
* @return   none
* @remark   �ʱ⿡ �ѹ� ���� �Ǵ� �Լ�\n
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
* @remark   Display Procedure �Լ� ����� �� ���� Display �ݿ���
********************************************************************************
*/
void DispClearAll(void)
{
	DispDrvClearBuf();
}

/**
********************************************************************************
* @brief    ���� Page�� ��ü Window�� digit�� ����(triangle �� decimal�� ����)
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
* @brief    window�� 1 ���� ����ϴ� �Լ� (editting Page)
* @param    winNum : Window ��ȣ\n
*           pos : Window���� ��ġ (0~ )
*           cdata : ���� data
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
* @brief    Message Page�� ��ü Window�� digit�� ����(triangle �� decimal�� ����)
* @param    none
* @return   none
* @remark   Message ǥ�� �� ���� ǥ�õ� Message�� ���� ���� 
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
* @brief    Window�� Setting �Ǿ� �ִ� digit size ���� ��ȯ
* @param    winNum : Window ��ȣ
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
* @brief    Menu Mode���� ����� Window�� Display Driver�� ���� �޾Ƽ� ��ȯ��\n
* @param    num : menu ��� Window ��� ��ȣ
* @return   Display Driver�� ���� ��ȯ ���� Window ��ȣ
* @remark   menu Mode���� ����� Window�� Display Driver�� �����ϱ� ����
*           ��� �Ǵ� �Լ� ��\n
*           DISP_MENU_STR_WIN_NUM, DISP_MENU_AUX_WIN_NUM, DISP_MENU_DATA_WIN_NUM
********************************************************************************
*/
INT8U DispGetMenuWinByNum(INT8U num)
{
	return DispDrvGetMenuWinByNum(num);
}

/**
********************************************************************************
* @brief    Window�� Setting �Ǿ� �ִ� Decimal Point Position ���� ��ȯ
* @param    winNum : Window ��ȣ
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
* @brief    Window�� Setting �Ǿ� �ִ� Decimal Point Type ���� ��ȯ
* @param    winNum : Window ��ȣ
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
* @brief    Window�� Setting �Ǿ� �ִ� Thousand Point Use Flag�� ��ȯ
* @param    winNum : Window ��ȣ
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
* @brief    Window�� Decimal Point Position, Type ���� Setting
* @param    winNum : Window ��ȣ\n
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
* @brief    Window�� String ��� (normal Page)
* @param    winNum : Window ��ȣ\n
*           str : ����� String Pointer
* @return   none
* @remark   String ��� �� Decimal Point ����
*           normal page -> sale mode, menu mode���� �⺻���� ��� �Ǵ� page
*           Display Procedure �Լ� ����� �� ���� Display �ݿ���
********************************************************************************
*/
void DispSetStr(INT8U winNum, char *str)
{
	DispDrvSetStr(winNum, str);
	DispDrvSetDecPoint(winNum, 0, DEC_TYPE_DOT, 0);
}

/**
********************************************************************************
* @brief    Window�� Const String ��� (normal Page)
* @param    winNum : Window ��ȣ\n
*           str : ����� Const String Pointer
* @return   none
* @remark   String ��� �� Decimal Point ����\n
*           Const�� ����� String ��� �� RAM �뷮 �þ�� ����
*           Display Procedure �Լ� ����� �� ���� Display �ݿ���
********************************************************************************
*/
void DispSetConstStr(INT8U winNum, const char *str)
{
	DispDrvSetConstStr(winNum, str);
	DispDrvSetDecPoint(winNum, 0, DEC_TYPE_DOT, 0);
}

/**
********************************************************************************
* @brief    Window�� Decimal Number(����) Data ��� (normal Page)
* @param    winNum : Window ��ȣ\n
*           num : ����� data (signed data)
*           fillChar : ��ȿ���� ���� ���� ä�� ���� (0 = space�� ��)
* @return   none
* @remark   Window�� �⼳���� Decimal Point ���\n
*           Window ũ�� �ʰ� �ϸ� "------" ���\n
*           data�� �����̸� '-' sign ó����\n
*           Display Procedure �Լ� ����� �� ���� Display �ݿ���
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
* @brief    Window�� Decimal Number(����) Data ��� (normal Page)
* @param    winNum : Window ��ȣ\n
*           num : ����� data (signed data)
*           digitSize : data�� digit size (digit size������ '0'�� ä���.)
* @return   none
* @remark   Window�� �⼳���� Decimal Point ���\n
*           Display Procedure �Լ� ����� �� ���� Display �ݿ���
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
* @brief    Window�� Decimal Number(����) Data �� const string ��� (normal Page) 
* @param    winNum : Window ��ȣ\n
*           src : const string pointer\n
*           num : ����� data (signed data)\n
*           digitSize : data�� digit size (digit size������ '0'�� ä���.)
* @return   none
* @remark   src == 0�̸� const string ��� ���� ����\n
*           Window�� �⼳���� Decimal Point ���\n
*           Display Procedure �Լ� ����� �� ���� Display �ݿ���
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
* @brief    Window�� Hexa Number(����) Data ��� (normal Page)
* @param    winNum : Window ��ȣ\n
*           num : ����� data (unsigned data)
*           digitSize : data�� digit size (digit size������ '0'�� ä���.)
* @return   none
* @remark   Decimal Point ����\n
*           Display Procedure �Լ� ����� �� ���� Display �ݿ���
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
* @param    winNum : Window ��ȣ\n
*           src : const string pointer\n
*           num : ����� data (unsigned data)\n
*           digitSize : data�� digit size (digit size������ '0'�� ä���.)
* @return   none
* @remark   src == 0�̸� const string ��� ���� ����
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
* @brief    char �������� ���� ���ڿ� char �������� Hexa data�� String���� ����� 
* @param    winNum : Window ��ȣ\n
*           cData : String���� ��ȯ�� Data\n
* @return   none
* @remark   ��) cData = 0x41, display "  A 41"
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
* @brief    Indicator on/off setting �Լ� (normal Page)
* @param    num : Indicator ��ȣ\n
*           on : 1 -> �ѱ�, 0 -> ����
* @return   none
* @remark   Indicator ��ȣ ���ǵǾ� ���� (INDICATOR_WEIGHT_MINUS...)
*           Display Procedure �Լ� ����� �� ���� Display �ݿ���
********************************************************************************
*/
void DispSetIndicator(INT8U num, INT8U on)
{
	DispDrvSetIndicator(num, on);
}

/**
********************************************************************************
* @brief    Bettery Energy Indicator setting �Լ� (normal Page)
* @param    percent : Battery �� (%)\n
* @return   none
* @remark   Display Procedure �Լ� ����� �� ���� Display �ݿ���
********************************************************************************
*/
void DispSetBattEnergy(INT8U percent)
{
	DispDrvSetBattEnergy(percent);
}

/**
********************************************************************************
* @brief    Window�� String ��� (message Page)
* @param    winNum : Window ��ȣ\n
*           str : ����� String Pointer
* @return   none
* @remark   Message Page�� ���, see DispSetStr()
*           DispSetRunMsgPage(...) ���� �Ͽ��� ���� ������
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
* @brief    Window�� Const String ��� (message Page)
* @param    winNum : Window ��ȣ\n
*           str : ����� Const String Pointer
* @return   none
* @remark   Message Page�� ���, see DispSetConstStr()
*           DispSetRunMsgPage(...) ���� �Ͽ��� ���� ������
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
* @brief    Window�� Decimal Number(����) Data ��� (message Page)
* @param    winNum : Window ��ȣ\n
*           num : ����� data (signed data)
* @return   none
* @remark   Message Page�� ���, see DispSetNum()
*           DispSetRunMsgPage(...) ���� �Ͽ��� ���� ������
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
	num *= 10; //version 4�ڸ��� ���� 220 -> 2200
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
* @brief    Window�� Fraction ��� (editting Page)
* @param    INT8U winNum : Window ��ȣ\n
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
* @brief    dispMsgWinNum�� "ERROR" + error number ��� (message Page)
* @param    num : error number
* @return   none
* @remark   Message Page�� ���, Buzzer on, Message ���� ��Ŵ 
********************************************************************************
*/
void DispSetError(INT8U num)
{
	DispSetMsgWithNum("ERR", num);
	BuzzerSetOnTimes(2);
}

/**
********************************************************************************
* @brief    dispMsgWinNum�� "HELP" + error number ��� (message Page)
* @param    num : error number
* @return   none
* @remark   Message Page�� ���, Buzzer on, Message ���� ��Ŵ 
********************************************************************************
*/
void DispSetHelp(INT8U num)
{
	DispSetMsgWithNum("HELP", num);
	BuzzerSetOnTimes(2);
}

/**
********************************************************************************
* @brief    Message page Setting �� ����
* @param    onTime100ms : blink�� On �ð� �ּҰ� 5 (���� : 100ms)\n
*           blinkCount : ������ ���� (0: �ȱ�����)
* @return   none
* @remark   blink�� Off �ð��� On �ð��� ����\n
*           DispProc()�Լ� ��ġ�� �ʰ� ó������ ��� Display�� �����
*           blink������ �Ϸ�� Display Procedure���� ������ ��
********************************************************************************
*/
void DispSetRunMsgPage(INT8U onTime100ms, INT8U blinkCount)
{
	if (onTime100ms < 5) 
	{
		onTime100ms = 5;
	}
#ifdef USE_DISPLAY_SUM_TOTAL	// �ջ� ���� ǥ�� ���� �� Hold Ű �Է� ���� �� �ְ�
	if( (sumDisplayStatus != DISPLAY_G_TOTAL_WEIGHT) && (sumDisplayStatus != DISPLAY_G_TOTAL_COUNT) ) // �ջ����� ǥ������ �ƴ� ������ Ű�Է� ����
	{
		KeyEnable(OFF);//key ���� ����
	}
#else
	KeyEnable(OFF);//key ���� ����
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

	DispDrvProc();//Display�� ��� ����
}
#ifdef USE_FAST_DISPLAYING_WEIGHT
/**
********************************************************************************
* @brief    Display driver update ��� ����
* @param    
* @return   none
* @remark   
********************************************************************************
*/
void DispImmedietly(void)
{
	DispDrvProc();//Display�� ��� ����
}
#endif
/**
********************************************************************************
* @brief    Display API Procedure\n
* @param    none
* @return   none
* @remark   Display Driver Procedure ����\n
*           message page ó�� (see DispSetRunMsgPage(...))\n
*           ���� �ð��� �ѹ��� ��� ���� �Ǵ� �Լ�
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
				KeyEnable(ON);//key ���� ���
				TimerSetDelay100ms(TIMER_DELAY_100MS_DISP_MSG, 0);//Timer Stop
				DispClearAllDigitsMsg();//���� Message�� ���� Clear
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
				DispDrvSetPage(DISP_DRV_SUSPEND_PAGE);//SUSPEND page�� ����
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

		//100 ms�� �ѹ��� ����
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
* @brief    Display Test �Լ�\n
* @param    dispTestState : Window�� ǥ���� ���� data
* @return   none
* @remark   ��� Window�� �Է� ���� ���� data ǥ��\n
*           ¦�� �϶� ��� indicator ��\n
*           Ȧ�� �϶� ��� indicator ��\n
*           battery �ܷ� ǥ�� (0..9)
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
* @brief    Back Light On/Off �Լ�\n
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
* @brief    Back Light ���� ���� ��ȯ �Լ�\n
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
* @brief    Back Light On/Off �Լ�\n
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
* @brief    Back Light ���� ���� ��ȯ �Լ�\n
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
* @brief    Back Light ��� ���� ���� �Լ�\n
* @param    onOff (0-off, 1-on)
* @return   none
* @remark   Off�� ��� ������ BL�� ������ Off�ȴ�.
********************************************************************************
*/
void DispBlSetUseFlag(BOOLEAN onOff)
{
	BlDrvSetUseFlag(onOff);
}

/**
********************************************************************************
* @brief    Back Light �۵� ���� ���� �Լ�\n
* @param    enbale (0-disable, 1-enable)
* @return   none
* @remark   disable�� ��� ������ BL�� ������ Off�ȴ�.
********************************************************************************
*/
void DispBlSetEnableFlag(BOOLEAN enable)
{
	BlDrvSetEnableFlag(enable);
}


/**
********************************************************************************
* @brief    Back Light Bright ���� �Լ�\n
* @param    level : ��� (1~DispBlGetMaxBright())
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
* @brief    Back Light Maxinum Bright ������ ��ȯ �Լ�\n
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
* @brief    Display on/off ���� �Լ�\n
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
* @brief    Display on/off ���� �Լ�\n
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
* @brief    Display Suspend mode on/off �Լ� \n
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
* @brief    Display Suspend mode return �Լ� \n
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
* @brief    Display Tunr Off �Լ� \n
* @param    
* @return   
* @remark   
********************************************************************************
*/
#if defined (USE_AC_POWER) || defined (USE_AC_BATT_POWER)
void DispTurnOff(void)
{
    DispClearAll();
	DispDrvProc();//Display�� ��� ����
	DispDrvProcForInterrupt();
}
#endif
#ifdef USE_CTS_MODE 
/**
********************************************************************************
* @brief    CTS 3�ÿ� �︮�� �˶��� Ű�� ������ ���߿� ������ �� �ֵ��� �ϴ� ���
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
 



