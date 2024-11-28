/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Key API Source File\n
* @file    KeyApi.c
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
#include "BuzzerApi.h"
#include "KeyApi.h"
#include "KeyDrv.h"
#include "../Time/TimeApi.h"
#include "../SystemTimer/TimerApi.h"
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


/*
********************************************************************************
*                       LOCAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert local typedef & structure here */


/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/
/* Insert file scope variable & tables here */
/** @brief Key�� ���� ���θ� �˱� ���� ����*/
BOOLEAN keyPressedFlag;
#ifdef REVERSE_DISPLAY
BOOLEAN keyReverseKeyFlag;
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
/**
********************************************************************************
* @brief    Key API Parameter�� default ������ �ʱ�ȭ\n
* @param    type : defualt type
* @return   none
* @remark   Parameter ������ �ʱⰪ�� �ִ´�.\n
*           Cal Mode�� Program ��忡�� ��ü �ʱ�ȭ�� �� �����Ѵ�.
********************************************************************************
*/
void KeySetDefaultParam(void)
{
	KeyDrvSetDefaultFuncTable();
}

/**
********************************************************************************
* @brief    Key API Init �Լ�\n
*           Key Driver Init\n
*           Key Driver Ring Buffer �޾ƿ���\n
* @param    none
* @return   none
* @remark   �ʱ⿡ �ѹ� ���� �Ǵ� �Լ�\n
********************************************************************************
*/
void KeyInit(void)
{
	KeyDrvInit();
	KeyResetPressedFlag();
}

/**
********************************************************************************
* @brief    Key Enable �Լ�\n
* @param    on/off flag
* @return   none
* @remark   Key �Է��� ���(ON)/����(OFF) �Ѵ�.\n
********************************************************************************
*/
void KeyEnable(BOOLEAN on)
{
	KeyDrvEnable(on);
}

/**
********************************************************************************
* @brief    Check Calibration Port �Լ�\n
* @param    none
* @return   1 -> Calibration key on, 0 -> Calibration key off
* @remark   
********************************************************************************
*/
BOOLEAN KeyCheckCalPort(void)
{
	return KeyDrvCheckCalPort();
}

/**
********************************************************************************
* @brief    Check Power Port �Լ�\n
* @param    none
* @return   1 -> Power key on, 0 -> Power key off
* @remark   �ʱ� Power�� �Ӷ� ���
********************************************************************************
*/
//BOOLEAN KeyCheckPowerPort(void)
//{
//	return KeyDrvCheckPowerPort();
//}

/**
********************************************************************************
* @brief    Check Power Key �Լ�\n
* @param    none
* @return   1 -> Power key on, 0 -> Power key off
* @remark   ª��, ��� ������ �����Ͽ� ��� ������ (Key Driver���� ����)\n
*           �Ϲ������� Power Key check�� ���
********************************************************************************
*/
BOOLEAN KeyCheckPowerKey(void)
{
	return KeyDrvCheckPowerKey();
}
#ifdef REVERSE_DISPLAY
/**
********************************************************************************
* @brief    check reverse Key �Լ�\n
* @param    none
* @return   1 -> reverse key on, 0 -> reverse key off
* @remark   
********************************************************************************
*/
BOOLEAN KeyCheckReverseKey(void)
{
	return KeyDrvCheckReverseKey();
}
#endif
/**
********************************************************************************
* @brief    Check Key �Լ�\n
* @param    none
* @return   1 -> key ����, 0 -> key ����
* @remark   Key Driver Ring Buffer�� check��
********************************************************************************
*/
BOOLEAN KeyCheck(void)
{  
	if (RingBufCheckBuf(&KeyDrvRingBuf))
	{
#ifdef USE_CTS_MODE
		TimerSetDelay100ms(TIMER_DELAY_100MS_CTS_LAST_USE_KEY,100);
#endif
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
********************************************************************************
* @brief    Key�� ���ȴ��� Ȯ���ϴ� �Լ�\n
* @param    none
* @return   1 -> ������, 0 -> ������ �ʾ���
* @remark   KeyResetPressedFlag() ȣ�� �������� ����
********************************************************************************
*/
BOOLEAN KeyCheckPressedFlag(void)
{  
	return keyPressedFlag;
}

#ifdef USE_LONG_KEY
/**
********************************************************************************
* @brief    LongKeyEnable �Լ�\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void KeyLongKeyEnable(void)
{  
	KeyDrvLongKeyEnable();
}

/**
********************************************************************************
* @brief    LongKeyDisable �Լ�\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void KeyLongKeyDisable(void)
{  
	KeyDrvLongKeyDisable();
}
#endif
/**
********************************************************************************
* @brief    keyPressedFlag ����\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void KeyResetPressedFlag(void)
{  
	keyPressedFlag = 0;
}

/**
********************************************************************************
* @brief    Key API Set Function Code �Լ�\n
* @param    keyMode :  KEY_MODE_NORMAL, KEY_MODE_ALPHABET, KEY_MODE_HEX...
*           rawData : Raw Key Code 
*           funcNum : Function Key Number
* @return   none
* @remark   ���� ������ KeyDrvSaveFuncTable() ���
********************************************************************************
*/
void KeySetFuncCode(INT8U keyMode, INT8U rawKey, KEY_TYPE funcKey)
{
	KeyDrvSetFuncCode(keyMode, rawKey, funcKey);
}

/**
********************************************************************************
* @brief    Key mode�� ���� Key Function code�� �޴� �Լ�
* @param    keyMode : KEY_MODE_NORMAL, KEY_MODE_ALPHABET, KEY_MODE_HEX
* @return   Key Function code
* @remark   KEY_NUM_0...KEY_NUM_A...KEY_C...KEY_PLU_1...KEY_NOT_PRESSED
********************************************************************************
*/
KEY_TYPE KeyGetFuncCode(INT8U keyMode)
{  
	INT8U rawKey;
	KEY_TYPE funcCode;

	if (keyMode == KEY_MODE_KEY_TYPE)
	{
		funcCode = KeyDrvGetFuncCode(keyMode, 0);
	}
	else
	{
		rawKey = RingBufGetChar(&KeyDrvRingBuf);
		funcCode = KeyDrvGetFuncCode(keyMode, rawKey);
	
		if (funcCode != KEY_NOT_PRESSED) 
		{
			BuzzerSetOnTimes(1);
			keyPressedFlag = 1;
		}
	}
	return funcCode;
}

/**
********************************************************************************
* @brief    Key mode�� ���� Key Function code�� raw key code�� �޴� �Լ�
* @param    keyMode : KEY_MODE_NORMAL, KEY_MODE_ALPHABET, KEY_MODE_HEX
*           funcCode : Key function code (raw key code Ȯ�� ��)
* @return   raw key code (0xff : error, funcCode�� ��ġ���� ����)
* @remark   �ٷ� ���� ���� raw key code�� ��ȯ
********************************************************************************
*/
INT8U KeyGetPrevRawCode(INT8U keyMode, KEY_TYPE funcCode)
{  
	INT8U rawKey;

	RingBufRewindChar(&KeyDrvRingBuf);//Key 1�� �ǵ���
	rawKey = RingBufGetChar(&KeyDrvRingBuf);

	if (funcCode == KeyDrvGetFuncCode(keyMode, rawKey))
	{
		return rawKey;
	}
	else
	{
		return KEY_NOT_PRESSED;
	}
}

/**
********************************************************************************
* @brief    Menu Mode���� ����� Key�� Display Driver�� ���� �޾Ƽ� ��ȯ��\n
* @param    num : menu ��� Key ��� ��ȣ
* @return   Key Driver�� ���� ��ȯ ���� Key ��
* @remark   menu Mode���� ����� Key�� Key Driver�� �����ϱ� ����
*           ��� �Ǵ� �Լ� ��\n
*           KEY_MENU_ENTER_NUM, KEY_MENU_C_NUM, KEY_MENU_UP_NUM, KEY_MENU_DOWN_NUM
********************************************************************************
*/
KEY_TYPE KeyGetPgmKeyByNum(INT8U num)
{
	return KeyDrvGetPgmKeyByNum(num);
}

/**
********************************************************************************
* @brief    �ѹ� ���� Key���� ������ ó���ϱ� ���� �ǵ����� �Լ�\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void KeyRewind(void)
{
	RingBufRewindChar(&KeyDrvRingBuf);
	BuzzerSetOnTimes(-1);
}

/**
********************************************************************************
* @brief    Key API Set Checksum �Լ�\n
* @param    none
* @return   none
* @remark   communication���� eeprom key area write�� ���\n
*           checksum ����� �ʿ������� ���
********************************************************************************
*/
void KeySaveChecksum(void)
{
	KeyDrvSaveFuncTable(UPDATE_CHECKSUM);
}


/**
********************************************************************************
* @brief    KeyGetUlDisplayFlag �Լ�\n
* @param    none
* @return   UlDisplayModeFlag
* @remark   UL ������ �ޱ����� UL ��display flag
********************************************************************************
*/

#ifdef USE_UL_DISPLAY
INT8U KeyGetUlDisplayFlag(void)
{
    return KeyDrvUlDisplayFlag();
}
#endif




/*
void KeyTest(void)
{
	KEY_TYPE	keyData;
	char testStr[5];

	if (KeyCheck())
	{
		keyData = KeyGetFuncCode(KEY_MODE_NORMAL);
		//keyData = RingBufGetChar(&KeyDrvRingBuf);
		ByteToHexStr((INT8U)keyData, testStr);
		DebugDataOut(testStr, 2);
	}
}
*/
#ifdef USE_AUTO_TARE_FUNCTION
/**
********************************************************************************
* @brief    check auto tare enable �Լ�\n
* @param    none
* @return   1 -> auto tare enable, 0 -> auto tare disable
* @remark   
********************************************************************************
*/
BOOLEAN KeyCheckAutoTareFunction(void)
{
	return KeyDrvCheckAutoTareFuction();
}
#endif
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
