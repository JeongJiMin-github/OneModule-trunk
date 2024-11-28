/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Window API Source File\n
* @file    WinApi.c
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
#include "../Common/RingBuf.h"
#include "../Common/CommonLib.h"
#include "../Common/UiDefine.h"
#include "../Communication/DebugApi.h"
#include "../Application/ScaleModeApp.h"
#include "../SystemTimer/TimerApi.h"
#ifdef USE_DECIMAL_POINT
#include "../Common/SaleData.h" // csh 20120731
#include "../Application/SaleModeApp.h" // csh 20120731
#include "KeyDrv.h"
#endif
#include "KeyApi.h"
#include "DispApi.h"
#include "BuzzerApi.h"
#include "EditorApi.h"
#ifdef USE_USER_SETTING_FOR_DOUBLEZERO_BATTLED
#include "../DB/StorageApi.h"
#include "../DB/ParamApi.h"
#endif

#ifdef USE_NUMERIC_EDITOR
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
/** @brief Key Type : Numeric */
#define KEY_TYPE_NUMERIC 		0
/** @brief Key Type : Non-Numeric (use MenuUp)*/
#define KEY_TYPE_NON_NUMERIC	1

/** @brief Insert Type : Insert */
#define INSERT 		0
/** @brief Insert Type : Delete */
#define DELETE 		1

/** @brief editor string size */
#define EDITOR_MAX_STR_SIZE	32

/*
********************************************************************************
*                       LOCAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert local typedef & structure here */
/** @brief Editor struct */
typedef struct
{
	/** @brief Editor Status (EDITOR_STOP, EDITOR_RUN) */
	INT8U status;
	/** @brief Editor���� ����� Window ��ȣ */
	INT8U winNum;
	/** @brief Editor Type (DEC, HEX, STR ...) */
	INT8U type;
	/** @brief Edit�� Data */
	INT32U editData;
	/** @brief Edit�� String Data */
	INT8U editStr[EDITOR_MAX_STR_SIZE];
	/** @brief Edit�� Data�� �ּҰ� */
	INT32U min;
	/** @brief Edit�� Data�� �ִ밪, String Data �ִ� ���� */
	INT32U max;
	/** @brief Edit�� Data�� Decimal Position */
	INT8U decPos;
	/** @brief Edit�� Data�� Decimal Type (DEC_TYPE_DOT, DEC_TYPE_COMMA) */
	INT8U decType;
	/** @brief Edit�� Data�� thousnad point ��� ���� */
	INT8U useThousandPt;
	/** @brief Edit window�� ǥ���� const string�� pointer */
	const char *constStr;
} EDITOR_DATA;

/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/
/* Insert file scope variable & tables here */
/** @brief Editor struct */
static EDITOR_DATA	editorData;

/** @brief Editor���� ENTER Key�� ����� key function code */
static KEY_TYPE editorKeyEnter;
/** @brief Editor���� C Key�� ����� key function code */
static KEY_TYPE editorKeyC;
/** @brief Editor���� MENU_UP Key�� ����� key function code */
static KEY_TYPE editorKeyMenuUp;
/** @brief Editor���� MENU_DOWN Key�� ����� key function code */
static KEY_TYPE editorKeyMenuDown;
/** @brief Editor���� INCrease Key�� ����� key function code */
//static KEY_TYPE editorKeyInc;
/** @brief Editor���� ROTate�� ����� key function code */
//static KEY_TYPE editorKeyRot;
/** @brief Editor���� DECrease Key�� ����� key function code */
//static KEY_TYPE editorKeyDec;
/** @brief Editor���� ROTate Left �� ����� key function code */
//static KEY_TYPE editorKeyRotL;
/** @brief Editor���� Exit�� ����� key function code */
static KEY_TYPE editorKeyExit;

/** @brief Editor Number Type (DEC, HEX,STR) */
static INT8U editorNumType;
/** @brief Editor Key Type (KEY_TYPE_NUMERIC, KEY_TYPE_NON_NUMERIC) */
//static INT8U editorKeyType;
/** @brief Editor Insert Type (INSERT, DELETE) */
static INT8U editorInsType;//for future use
/** @brief Edit�� Data digit ����, String ���� */
static INT8U editorDigitLen;
/** @brief Edit�� digit ��ȣ */
static INT8U editorDigitPos;
/** @brief Edit�� Window�� digit size */
static INT8U editorWinSize;
/** @brief Window�� digit blink state */
static INT8U digitblinkState;

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
/**
********************************************************************************
* @brief    Editor API Init �Լ�\n
*           Editor���� ����� Key Init\n
*           Editor���� ���� �ʱ�ȭ\n
* @param    none
* @return   none
* @remark   �ʱ⿡ �ѹ� ���� �Ǵ� �Լ�\n
********************************************************************************
*/
void EditorInit(void)
{
	editorKeyEnter = KeyGetPgmKeyByNum(KEY_MENU_ENTER_NUM);
	editorKeyC = KeyGetPgmKeyByNum(KEY_MENU_C_NUM);
	editorKeyMenuUp = KeyGetPgmKeyByNum(KEY_MENU_UP_NUM);
	editorKeyMenuDown = KeyGetPgmKeyByNum(KEY_MENU_DOWN_NUM);

//	editorKeyInc = KeyGetPgmKeyByNum(KEY_INC_NUM);
//	editorKeyRot = KeyGetPgmKeyByNum(KEY_ROT_NUM);
//	editorKeyDec = KeyGetPgmKeyByNum(KEY_DEC_NUM);
//	editorKeyRotL = KeyGetPgmKeyByNum(KEY_ROTL_NUM);
	editorKeyExit = KeyGetPgmKeyByNum(KEY_EXIT_NUM);
/*	
	if (editorKeyInc == KEY_NOT_EXISTED || editorKeyRot == KEY_NOT_EXISTED)
	{
		editorKeyType = KEY_TYPE_NUMERIC;
	}
	else
	{
		editorKeyType = KEY_TYPE_NON_NUMERIC;
	}
*/
	EditorSetData(WIN_UNIT_PRICE, DEC, 0, 0, 99999, 2, 0, 0);
	EditorStop();
}

/**
********************************************************************************
* @brief    Editor�� setting�ϰ� �����Ű�� �Լ�\n
* @param    see EditorSetData(...)
* @return   none
* @remark   
********************************************************************************
*/
			
void EditorSetDataAndRun(INT8U winNum, INT8U type, INT32U editData, INT32U min, INT32U max, INT8U decPos, INT8U decType, INT8U useThousandPt)
{
	EditorSetData(winNum, type, editData, min, max, decPos, decType, useThousandPt);
	EditorRun();
}

/**
********************************************************************************
* @brief    Editor�� setting�ϴ� �Լ�\n
* @param    winNum : Window ��ȣ
*           type : DEC, HEX
*           editData : edit�� data
*           min : data �ּҰ� ����
*           max : data �ִ밪 ����
*           decPos : decimal position (xxx -> 0, xx.x -> 1, x,xx -> 2)
*           decType : dicimal type (DEC_TYPE_DOT, DEC_TYPE_COMMA)
* @return   none
* @remark   editing digit length�� max�� Ȯ���Ͽ� �ڵ����� �����
********************************************************************************
*/
void EditorSetData(INT8U winNum, INT8U type, INT32U editData, INT32U min, INT32U max, INT8U decPos, INT8U decType, INT8U useThousandPt)
{
	INT8U i;
	INT8U *str;
	
	editorData.winNum = winNum;
	editorData.type = type;
	editorData.min = min;
	editorData.decPos = decPos;
	editorData.decType = decType;
	editorData.useThousandPt = useThousandPt;
	
	editorNumType = type;

//for future use
/*
	if (type == DEC || type == HEX) 
	{
		editorInsType = INSERT;
	}
	else
	{
		editorInsType = DELETE;
	}
*/
	if (editorNumType == STR)
	{
		if (max > EDITOR_MAX_STR_SIZE)
		{
			max = EDITOR_MAX_STR_SIZE;
		}

		str = (INT8U *)editData;	
		for ( i = 0; i < max; i++) 
		{
			editorData.editStr[i] = str[i];
		}
		editorData.editStr[i] = 0; 
	} 
	else
	{
		editorData.editData = editData;
	}
	editorData.max = max;


	editorWinSize = DispGetWinSize(winNum);

	if (editorNumType == HEX)
	{
		editorDigitLen = CalcHexDigitLen(max);
	}
	else if (editorNumType == STR)
	{
		editorDigitLen = editorWinSize;
	}
	else//DEC
	{
		editorDigitLen = CalcDecDigitLen(max);
	}
/*
	if (editorKeyType == KEY_TYPE_NUMERIC) 
	{
*/
		editorDigitPos = 0;
/*	}
	else //editorKeyType == KEY_TYPE_NON_NUMERIC
	{
		if (editorNumType == STR) 
		{
			editorDigitPos = 0;
		}
		else
		{
			editorDigitPos = editorDigitLen - 1;
		}
	}

	if (editorKeyType == KEY_TYPE_NON_NUMERIC)
	{
		TimerSetDelay100ms(TIMER_DELAY_100MS_EDITOR, 7);//0.7 s
		digitblinkState = 0;
	}
*/
	editorData.constStr = 0;//default
}

/**
********************************************************************************
* @brief    Editor�� String�� setting�ϴ� �Լ�\n
* @param    cStr : const chat pointer
* @return   none
* @remark   ex) "C41" -> "C41XX" XX�� editing ����
********************************************************************************
*/
void EditorSetConstStr(const char *cStr)
{
	editorData.constStr = cStr;
}

/**
********************************************************************************
* @brief    Editor Run\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void EditorRun(void)
{
	editorData.status = EDITOR_RUN;
}

/**
********************************************************************************
* @brief    Editor Stop\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void EditorStop(void)
{
	editorData.status = EDITOR_STOP;
}

/**
********************************************************************************
* @brief    Check Editor Status\n
* @param    none
* @return   EDITOR_RUN, EDITOR_STOP
* @remark   
********************************************************************************
*/
INT8U EditorCheckStatus(void)
{
	return editorData.status;
}

/**
********************************************************************************
* @brief    Editor data ��ȯ\n
* @param    dataPtr : �Է� ���� data pointer
* @return   none
* @remark   
********************************************************************************
*/
void EditorGetData(INT32U *dataPtr)
{
	INT8U i;
	INT8U *str;

	if (editorNumType == STR)
	{
		str = (INT8U *)dataPtr;
		for ( i = 0; i < editorData.max; i++) 
		{
			str[i] = editorData.editStr[i];
		}
	}
	else
	{
		*dataPtr = editorData.editData;
	}
}

#ifdef USE_DECIMAL_POINT
/**
********************************************************************************
* @brief    1 decimal number�� �Է� �޾Ƽ� ������ digit�� data�� �߰�\n
* @param    inData : data
*           decNum : �߰��� ����
*           leng : inData digit length
*           max : inData �ִ밪
*           denyOver : �ִ밪 ������ �ֻ��� digit ����(1 -> �ȹ���, 0 -> ����)
* @return   �����
* @remark   denyOver == FALSE�̰� �ִ밪 ���ѿ� �ɸ��� inData �״�� ��ȯ
********************************************************************************
*/
INT32U EditorInsDecDigit(INT32U inData, INT8U decNum, INT8U leng, INT32U max, INT8U denyOver) // csh 20120731
{
	INT32U result;
#ifdef USE_USER_SETTING_FOR_DOUBLEZERO_BATTLED
	INT8U useDoublezerokey;	

	useDoublezerokey = ParamReadByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BLINK_COUNT); // if(useDoublezerokey& 0x02)  0 : Use [Decimal Point] key, 1: Use DoubleZero[00] key 
	if(useDoublezerokey & 0x01) // Use Double Zero[00] key
	{
		result = inData * 10L + (INT32U)decNum;
		
		if ((result > max) && (denyOver != FALSE))
		{
			//return inData;
			return 0;		//change kkanno 070615
		}
		else
		{
			if (result >= DecPowerConvTable[leng])
			{
				result %= DecPowerConvTable[leng];
			}
			if (result > max)
			{
				result %= DecPowerConvTable[leng-1];
			}
			return result;
		}
	}
	else // Use [Decimal Point] key
	{
		if ((DecimalPointKeyExistFlag) && ((DispGetDecPos(WIN_UNIT_PRICE)) || (SaleData.saleState == SALE_STATE_FIXED_PRICE)))
		{ 
			if(SaleData.saleState == SALE_STATE_FIXED_PRICE)
			{
				SaleDecimalPointKeyOnOff = OFF;
			}
			if (SaleDecimalPointKeyOnOff == ON)
			{
				if (SaleNumberPositonFlag >= 0 && SaleData.priceDecPos > SaleNumberPositonFlag)
				{
					result = inData + (INT32U)decNum * DecPowerConvTable[SaleNumberPositonFlag];
					SaleNumberPositonFlag--;
				}
				else
				{
					SaleDecimalPointKeyOnOff = OFF;
					SaleNumberPositonFlag =0;
					return 0;
				}
			}
			else
			{
					result = inData * 10L + (INT32U)decNum * DecPowerConvTable[SaleData.priceDecPos];
			}
		}
		else
		{
			result = inData * 10L + (INT32U)decNum;
		}
		if ((result > max) && (denyOver != FALSE))
		{
			return 0;
		}
		else
		{
			if (result >= DecPowerConvTable[leng])
			{
				result %= DecPowerConvTable[leng];
			}
			if (result > max)
			{
				result %= DecPowerConvTable[leng-1];
			}
			return result;
		}
	}
#else
	if ((DecimalPointKeyExistFlag) && ((DispGetDecPos(WIN_UNIT_PRICE)) || (SaleData.saleState == SALE_STATE_FIXED_PRICE)))
	{ 
		if(SaleData.saleState == SALE_STATE_FIXED_PRICE)
		{
			SaleDecimalPointKeyOnOff = OFF;
		}
		if (SaleDecimalPointKeyOnOff == ON)
		{
			if (SaleNumberPositonFlag >= 0 && SaleData.priceDecPos > SaleNumberPositonFlag)
			{
				result = inData + (INT32U)decNum * DecPowerConvTable[SaleNumberPositonFlag];
				SaleNumberPositonFlag--;
			}
			else
			{
				SaleDecimalPointKeyOnOff = OFF;
				SaleNumberPositonFlag =0;
				return 0;
			}
		}
		else
		{
			result = inData * 10L + (INT32U)decNum * DecPowerConvTable[SaleData.priceDecPos];
		}

	}
	else
	{ 
		result = inData * 10L + (INT32U)decNum;
	}

	if ((result > max) && (denyOver != FALSE))
	{
		return 0;
	}
	else
	{
		if (result >= DecPowerConvTable[leng])
		{
			result %= DecPowerConvTable[leng];
		}
		if (result > max)
		{
			result %= DecPowerConvTable[leng-1];
		}
		return result;
	}
#endif
}

/**
********************************************************************************
* @brief    1 decimal number�� �Է� �޾Ƽ� ������ digit�� data�� �߰�\n
* @param    inData : data
*           decNum : �߰��� ����
*           leng : inData digit length
*           max : inData �ִ밪
*           denyOver : �ִ밪 ������ �ֻ��� digit ����(1 -> �ȹ���, 0 -> ����)
* @return   �����
* @remark   denyOver == FALSE�̰� �ִ밪 ���ѿ� �ɸ��� inData �״�� ��ȯ
********************************************************************************
*//*
INT32U EditorInsDecDigit(INT32U inData, INT8U decNum, INT8U leng, INT32U max, INT8U denyOver) // csh 20120731
{
	INT32U result;
	
	if (DecimalPointKeyExistFlag) 
	{
		if (DispGetDecPos(WIN_UNIT_PRICE)) 
		{ 
			if (SaleDecimalPointKeyOnOff == ON)
			{
				if (SaleNumberPositonFlag >= 0 && SaleData.priceDecPos > SaleNumberPositonFlag)
				{
					result = inData + (INT32U)decNum * DecPowerConvTable[SaleNumberPositonFlag];
					SaleNumberPositonFlag--;
				}
				else
				{
					SaleDecimalPointKeyOnOff = OFF;
					SaleNumberPositonFlag =0;
					return 0;
				}
			}
			else
			{
					result = inData * 10L + (INT32U)decNum * DecPowerConvTable[SaleData.priceDecPos];
			}
		}
		else
		{ 
			result = inData * 10L + (INT32U)decNum;
		}
		if ((result > max) && (denyOver != FALSE))
		{
			return 0;
		}
		else
		{
			if (result >= DecPowerConvTable[leng])
			{
				result %= DecPowerConvTable[leng];
			}
			if (result > max)
			{
				result %= DecPowerConvTable[leng-1];
			}
			return result;
		}
	}
	else
	{
		result = inData * 10L + (INT32U)decNum;
		
		if ((result > max) && (denyOver != FALSE))
		{
			//return inData;
			return 0;		//change kkanno 070615
		}
		else
		{
			if (result >= DecPowerConvTable[leng])
			{
				result %= DecPowerConvTable[leng];
			}
			if (result > max)
			{
				result %= DecPowerConvTable[leng-1];
			}
			return result;
		}
	}
}
*/
#else 
/**
********************************************************************************
* @brief    1 decimal number�� �Է� �޾Ƽ� ������ digit�� data�� �߰�\n
* @param    inData : data
*           decNum : �߰��� ����
*           leng : inData digit length
*           max : inData �ִ밪
*           denyOver : �ִ밪 ������ �ֻ��� digit ����(1 -> �ȹ���, 0 -> ����)
* @return   �����
* @remark   denyOver == FALSE�̰� �ִ밪 ���ѿ� �ɸ��� inData �״�� ��ȯ
********************************************************************************
*/ // csh 20120731
INT32U EditorInsDecDigit(INT32U inData, INT8U decNum, INT8U leng, INT32U max, INT8U denyOver)
{
	INT32U result;
	
	result = inData * 10L + (INT32U)decNum;

	if ((result > max) && (denyOver != FALSE))
	{
		//return inData;
		return 0;		//change kkanno 070615
	}
	else
	{
		if (result >= DecPowerConvTable[leng])
		{
			result %= DecPowerConvTable[leng];
		}
		if (result > max)
		{
			result %= DecPowerConvTable[leng-1];
		}
		return result;
	}
}

#endif

/**
********************************************************************************
* @brief    1 hexa number�� �Է� �޾Ƽ� ������ digit�� data�� �߰�\n
* @param    inData : data
*           hexNum : �߰��� ����
*           leng : inData digit length
*           max : inData �ִ밪
*           denyOver : �ִ밪 ������ �ֻ��� digit ����(1 -> �ȹ���, 0 -> ����)
* @return   �����
* @remark   denyOver == FALSE�̰� �ִ밪 ���ѿ� �ɸ��� inData �״�� ��ȯ
********************************************************************************
*/
INT32U EditorInsHexDigit(INT32U inData, INT8U hexNum, INT8U leng, INT32U max, INT8U denyOver)
{
	INT32U result;
	
	result = inData * 0x10L + (INT32U)hexNum;

	if ((result > max) && (denyOver != FALSE))
	{
		return inData;
	}
	else
	{
		if (result >= HexPowerConvTable[leng]) 
		{
			result %= HexPowerConvTable[leng];
		}
		if (result > max)
		{
			result %= HexPowerConvTable[leng-1];
		}
		return result;
	}
}

/**
********************************************************************************
* @brief    decimal number�� �Է� �޾Ƽ� digit position digit�� data����\n
* @param    inData : data
*           pos : ������ digit ��ġ (0-ù��° �ڸ�, 1~ )
* @return   �����
* @remark   ��) inData = 12345, pos = 3�̸� return = 13345
********************************************************************************
*/
INT32U EditorIncDecDigit(INT32U inData, INT8U pos)
{
	INT32U result;
	INT32U changeNum;
	
	result = (INT32U)inData;

	changeNum = result + DecPowerConvTable[pos];
	changeNum = changeNum % DecPowerConvTable[pos + 1];
	
	result = result / DecPowerConvTable[pos + 1];
	result = result * DecPowerConvTable[pos + 1];

	result = result + changeNum;

	return result;
}

/**
********************************************************************************
* @brief    hexa number�� �Է� �޾Ƽ� digit position digit�� data����\n
* @param    inData : data
*           pos : ������ digit ��ġ (0-ù��° �ڸ�, 1~ )
* @return   �����
* @remark   ��) inData = 0xABCDEF, pos = 3�̸� return = 0xABDDEF
********************************************************************************
*/
INT32U EditorIncHexDigit(INT32U inData, INT8U pos)
{
	INT32U result;
	INT32U changeNum;
	
	result = (INT32U)inData;

	changeNum = result + HexPowerConvTable[pos];
	changeNum = changeNum % HexPowerConvTable[pos + 1];
	
	result = result / HexPowerConvTable[pos + 1];
	result = result * HexPowerConvTable[pos + 1];

	result = result + changeNum;

	return result;
}

/**
********************************************************************************
* @brief    decimal number�� �Է� �޾Ƽ� digit position digit�� data ����\n
* @param    inData : data
*           pos : ������ digit ��ġ (0-ù��° �ڸ�, 1~ )
* @return   �����
* @remark   ��) inData = 12345, pos = 3�̸� return = 11345
********************************************************************************
*/
INT32U EditorDecDecDigit(INT32U inData, INT8U pos)
{
	INT32U result;
	INT32U changeNum;
	
	result = (INT32U)inData;
	
	changeNum = result + DecPowerConvTable[pos + 1] - DecPowerConvTable[pos];
	changeNum = changeNum % DecPowerConvTable[pos + 1];
	
	result = result / DecPowerConvTable[pos + 1];
	result = result * DecPowerConvTable[pos + 1];

	result = result + changeNum;

	return result;
}

/**
********************************************************************************
* @brief    hexa number�� �Է� �޾Ƽ� digit position digit�� data����\n
* @param    inData : data
*           pos : ������ digit ��ġ (0-ù��° �ڸ�, 1~ )
* @return   �����
* @remark   ��) inData = 0xABCDEF, pos = 3�̸� return = 0xABBDEF
********************************************************************************
*/
INT32U EditorDecHexDigit(INT32U inData, INT8U pos)
{
	INT32U result;
	INT32U changeNum;
	
	result = (INT32U)inData;
	
	changeNum = result + HexPowerConvTable[pos + 1] - HexPowerConvTable[pos];
	changeNum = changeNum % HexPowerConvTable[pos + 1];
	
	result = result / HexPowerConvTable[pos + 1];
	result = result * HexPowerConvTable[pos + 1];

	result = result + changeNum;

	return result;
}


/**
********************************************************************************
* @brief    decimal number�� �Է� �޾Ƽ� digit position digit�� data �مK\n
* @param    inData : data
*           pos : �ٲ� digit ��ġ (0-ù��° �ڸ�, 1~ )
* @return   �����
* @remark   ��) inData = 12345, pos = 3�̸� return = 13345
********************************************************************************
*/
INT32U EditorChangeDecDigit(INT32U inData, INT8U decNum, INT8U pos)
{
	INT32U result;
	INT32U changeNum;
	
	result = (INT32U)inData;

	changeNum = result % DecPowerConvTable[pos];
	changeNum = decNum * DecPowerConvTable[pos] + changeNum;
	
	result = result / DecPowerConvTable[pos + 1];
	result = result * DecPowerConvTable[pos + 1];

	result = result + changeNum;

	return result;
}

/**
********************************************************************************
* @brief    hex number�� �Է� �޾Ƽ� digit position digit�� data �مK\n
* @param    inData : data
*           pos : �ٲ� digit ��ġ (0-ù��° �ڸ�, 1~ )
* @return   �����
* @remark   ��) inData = 12345, pos = 3�̸� return = 13345
********************************************************************************
*/
INT32U EditorChangeHexDigit(INT32U inData, INT8U hexNum, INT8U pos)
{
	INT32U result;
	INT32U changeNum;
	
	result = (INT32U)inData;

	changeNum = result % HexPowerConvTable[pos];
	changeNum = hexNum * HexPowerConvTable[pos] + changeNum;
	
	result = result / HexPowerConvTable[pos + 1];
	result = result * HexPowerConvTable[pos + 1];

	result = result + changeNum;

	return result;
}

/**
********************************************************************************
* @brief    Editor API Procedure\n
* @param    none
* @return   none
* @remark   �Է� ���� Key�� ���� Editing data ���� Ȥ�� Exit
*           ���� �ð��� �ѹ��� ��� ���� �Ǵ� �Լ�
********************************************************************************
*/
void EditorProc(void)
{
	static INT8U cFlag;
	static INT8U initFlag;
	static INT8U decPosBackup;
	static INT8U decTypeBackup;
	static INT8U useThousandPtBackup;
	KEY_TYPE key;
//	INT8U tempByte;

	if (editorData.status == EDITOR_STOP) 
	{
		initFlag = 0;
		return;
	}

	if (initFlag == 0)
	{
		initFlag = 1;
		//Initialize Code Here
		decPosBackup = DispGetDecPos(editorData.winNum);
		decTypeBackup = DispGetDecType(editorData.winNum);
		useThousandPtBackup = DispGetUseThousandPt(editorData.winNum);
		DispSetDecPoint(editorData.winNum, editorData.decPos, editorData.decType, editorData.useThousandPt);
		cFlag = FALSE;
	}

	if (KeyCheck())
	{
		if (editorNumType == HEX)
		{
			key = KeyGetFuncCode(KEY_MODE_HEX);
		}
		else//DEC, STR
		{
			key = KeyGetFuncCode(KEY_MODE_NORMAL);
		}

		if (editorNumType == STR)
		{
			digitblinkState = 0;
			TimerSetDelay100ms(TIMER_DELAY_100MS_EDITOR, 7);//0.7 s
			
			if (key == editorKeyEnter || key == editorKeyExit)
			{
				editorData.status = EDITOR_STOP;				
				KeyRewind();								
			}
/*
			else if (key == editorKeyRot)
			{
				if (editorDigitPos < editorData.max - 1)//null ???
				{
					editorDigitPos++;
				}
				else
				{
					editorDigitPos = editorData.max - 1;
				}
			}
			else if (key == editorKeyRotL)
			{
				if (editorDigitPos > 0)
				{
					editorDigitPos--;
				}
				else
				{
					editorDigitPos = 0;
				}
			}			
*/
			else if (key == editorKeyC)
			{
				if (cFlag)
				{
					cFlag = FALSE;
					editorData.editStr[0] = 0;
					editorDigitPos = 0;
				}
				else
				{
					editorData.status = EDITOR_STOP;
					KeyRewind();
				}
			}
/*
			else if (key == editorKeyInc)
			{
				cFlag = TRUE;
				tempByte = editorData.editStr[editorDigitPos];
				*/
				/*
				if (tempByte < 'A')
				{
					tempByte = 'A';
				}
				else if (tempByte < 'Z')
				{
					tempByte++;
				} 
				else
				{
					tempByte = 'Z';
				}
				*/
/*
				if (tempByte < ' ')
				{
					tempByte = ' ';
				}
				else if (tempByte ==  ' ')
				{
					tempByte = '0';
				}
				else if (tempByte == '9')
				{
					tempByte = 'A';
				}
				else if (tempByte < 'Z')
				{
					tempByte++;
				}
				else if (tempByte == 'Z')
				{
					tempByte = ' ';
				}
				editorData.editStr[editorDigitPos] = tempByte;
			}
*/
/*
			else if (key == editorKeyDec)
			{
				cFlag = TRUE;
				tempByte = editorData.editStr[editorDigitPos];
*/
				/*
				if (tempByte > 'Z')
				{
					tempByte = 'Z';
				}
				else if (tempByte > 'A')
				{
					tempByte--;
				} 
				else
				{
					tempByte = 'A';
				}
				*/		
/*
				if (tempByte > 'Z')
				{
					tempByte = 'Z';
				}
				else if (tempByte == 'A')
				{
					tempByte = '9';
				}
				else if (tempByte == '0')
				{
					tempByte = ' ';
				}
				else if (tempByte > ' ')
				{
					tempByte--;
				}
				else if (tempByte == ' ')
				{
					tempByte = 'Z';
				}
				editorData.editStr[editorDigitPos] = tempByte;
			}	
*/
		}
		else//DEC, HEX
		{
/*			if (editorKeyType == KEY_TYPE_NUMERIC)
			{
*/
				if ((key == editorKeyEnter) || (key == editorKeyMenuUp) || (key == editorKeyMenuDown))
				{
					if ((editorData.editData < editorData.min) || (editorData.editData > editorData.max))
					{
						cFlag = FALSE;				
					} 
					else
					{
						editorData.status = EDITOR_STOP;				
						KeyRewind();								
					}
				} 
				else if (key == editorKeyC)
				{
					if (cFlag)
					{
						cFlag = FALSE;
						editorData.editData = 0L;
					}
					else
					{
						editorData.status = EDITOR_STOP;
						KeyRewind();
					}
				}
				else
				{
					if ((key >= KEY_NUM_0) && (key <= KEY_NUM_9))
					{ 
						if (!cFlag) editorData.editData = 0L;
						cFlag = TRUE;
						if (editorNumType == HEX)
						{
							editorData.editData = EditorInsHexDigit(editorData.editData, key - KEY_NUM_0, editorDigitLen, editorData.max, FALSE);
						}
						else
						{
							editorData.editData = EditorInsDecDigit(editorData.editData, key - KEY_NUM_0, editorDigitLen, editorData.max, FALSE);
						}
					}
					else if ((key >= KEY_NUM_A) && (key <= KEY_NUM_F) && (editorNumType == HEX))
					{
						if (!cFlag)
						{
							editorData.editData = 0L;
						}
						cFlag = TRUE;
						editorData.editData = EditorInsHexDigit(editorData.editData, key - KEY_NUM_A + 10, editorDigitLen, editorData.max, FALSE);
					}
				}
/*			}
			else	//(editorKeyType == KEY_TYPE_NON_NUMERIC)
			{
				digitblinkState = 0;
				TimerSetDelay100ms(TIMER_DELAY_100MS_EDITOR, 7);//0.7 s

				if (key == editorKeyEnter)
				{
					if ((editorData.editData < editorData.min) || (editorData.editData > editorData.max))
					{
						editorData.editData = editorData.min;
						editorDigitPos = editorDigitLen - 1;
						cFlag = FALSE;				
					} 
					else
					{
						editorData.status = EDITOR_STOP;				
						KeyRewind();								
					}
				}
*/
/*
				else if (key == editorKeyRot)
				{
					if (editorDigitPos > 0)
					{
						editorDigitPos--;
					}
					else
					{
						editorDigitPos = editorDigitLen - 1;
					}
				}
				else if (key == editorKeyRotL)
				{
					if (editorDigitPos < editorDigitLen)
					{
						editorDigitPos++;
					}
					else
					{
						editorDigitPos = 0;
					}
				}
*/
/*
				else if (key == editorKeyC)
				{
					if (cFlag)
					{
						cFlag = FALSE;
						editorData.editData = 0L;
						editorDigitPos = editorDigitLen - 1;
					}
					else
					{
						editorData.status = EDITOR_STOP;
						KeyRewind();
					}
				}	
*/
/*
				else if (key == editorKeyInc)
				{
					cFlag = TRUE;
					if (editorNumType == HEX)
					{
						editorData.editData = EditorIncHexDigit(editorData.editData, editorDigitPos);
					}
					else
					{
						editorData.editData = EditorIncDecDigit(editorData.editData, editorDigitPos);
					}
				}
				else if (key == editorKeyDec)
				{
					cFlag = TRUE;
					if (editorNumType == HEX)
					{
						editorData.editData = EditorDecHexDigit(editorData.editData, editorDigitPos);
					}
					else
					{
						editorData.editData = EditorDecDecDigit(editorData.editData, editorDigitPos);
					}
				}

			}
*/
		}
	}//if (KeyCheck())

	if (editorNumType == STR)
	{
		DispSetStr(editorData.winNum, &editorData.editStr[editorDigitPos]);
	}
	else if (editorNumType == HEX)
	{
		//DispSetHexNum(editorData.winNum, editorData.editData, editorDigitLen);
		DispSetConstStrWithHexNum(editorData.winNum, editorData.constStr, editorData.editData, editorDigitLen);
	}
	else//DEC
	{
/*		if (editorKeyType == KEY_TYPE_NUMERIC) 
		{
*/
			//this mode is not allowed adding const string
			DispSetNum(editorData.winNum, editorData.editData, 0);
/*		}
		else 
		{
			//DispSetDecNum(editorData.winNum, editorData.editData, editorDigitLen);
			DispSetConstStrWithDecNum(editorData.winNum, editorData.constStr, editorData.editData, editorDigitLen);
		}
*/
	}
/*
	if (editorKeyType == KEY_TYPE_NON_NUMERIC)
	{
		if (TimerCheckDelay100ms(TIMER_DELAY_100MS_EDITOR) == 1)
		{
			if (digitblinkState == 0)
			{
				digitblinkState = 1;
				TimerSetDelay100ms(TIMER_DELAY_100MS_EDITOR, 3);//0.3 s
			}
			else 
			{
				digitblinkState = 0;
				TimerSetDelay100ms(TIMER_DELAY_100MS_EDITOR, 7);//0.7 s
			}
		}
		if (digitblinkState == 1)
		{
			if (editorNumType == STR)
			{
				DispSetChar(editorData.winNum, 0, 0x20);//space
			} 
			else//DEC, HEX
			{
				DispSetChar(editorData.winNum, (editorWinSize - editorDigitPos - 1), 0x20);//space
			}
		}
	}
*/
	if (editorData.status == EDITOR_STOP)
	{
		//Ending Code Here
		DispSetDecPoint(editorData.winNum, decPosBackup, decTypeBackup, useThousandPtBackup);
		TimerSetDelay100ms(TIMER_DELAY_100MS_EDITOR, 0);//editor timer off
		initFlag = 0;
	}
}


/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
#endif //#ifdef USE_NUMERIC_EDITOR
