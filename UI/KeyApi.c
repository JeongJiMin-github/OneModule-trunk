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
/** @brief Key의 누름 여부를 알기 위한 변수*/
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
* @brief    Key API Parameter를 default 값으로 초기화\n
* @param    type : defualt type
* @return   none
* @remark   Parameter 영역에 초기값을 넣는다.\n
*           Cal Mode나 Program 모드에서 전체 초기화할 때 실행한다.
********************************************************************************
*/
void KeySetDefaultParam(void)
{
	KeyDrvSetDefaultFuncTable();
}

/**
********************************************************************************
* @brief    Key API Init 함수\n
*           Key Driver Init\n
*           Key Driver Ring Buffer 받아오기\n
* @param    none
* @return   none
* @remark   초기에 한번 실행 되는 함수\n
********************************************************************************
*/
void KeyInit(void)
{
	KeyDrvInit();
	KeyResetPressedFlag();
}

/**
********************************************************************************
* @brief    Key Enable 함수\n
* @param    on/off flag
* @return   none
* @remark   Key 입력을 허용(ON)/불허(OFF) 한다.\n
********************************************************************************
*/
void KeyEnable(BOOLEAN on)
{
	KeyDrvEnable(on);
}

/**
********************************************************************************
* @brief    Check Calibration Port 함수\n
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
* @brief    Check Power Port 함수\n
* @param    none
* @return   1 -> Power key on, 0 -> Power key off
* @remark   초기 Power를 켤때 사용
********************************************************************************
*/
//BOOLEAN KeyCheckPowerPort(void)
//{
//	return KeyDrvCheckPowerPort();
//}

/**
********************************************************************************
* @brief    Check Power Key 함수\n
* @param    none
* @return   1 -> Power key on, 0 -> Power key off
* @remark   짧게, 길게 누름을 구분하여 사용 가능함 (Key Driver조건 따름)\n
*           일반적으로 Power Key check시 사용
********************************************************************************
*/
BOOLEAN KeyCheckPowerKey(void)
{
	return KeyDrvCheckPowerKey();
}
#ifdef REVERSE_DISPLAY
/**
********************************************************************************
* @brief    check reverse Key 함수\n
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
* @brief    Check Key 함수\n
* @param    none
* @return   1 -> key 있음, 0 -> key 없음
* @remark   Key Driver Ring Buffer를 check함
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
* @brief    Key가 눌렸는지 확인하는 함수\n
* @param    none
* @return   1 -> 눌렸음, 0 -> 눌리지 않았음
* @remark   KeyResetPressedFlag() 호출 전까지는 유지
********************************************************************************
*/
BOOLEAN KeyCheckPressedFlag(void)
{  
	return keyPressedFlag;
}

#ifdef USE_LONG_KEY
/**
********************************************************************************
* @brief    LongKeyEnable 함수\n
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
* @brief    LongKeyDisable 함수\n
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
* @brief    keyPressedFlag 지움\n
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
* @brief    Key API Set Function Code 함수\n
* @param    keyMode :  KEY_MODE_NORMAL, KEY_MODE_ALPHABET, KEY_MODE_HEX...
*           rawData : Raw Key Code 
*           funcNum : Function Key Number
* @return   none
* @remark   영구 저장은 KeyDrvSaveFuncTable() 사용
********************************************************************************
*/
void KeySetFuncCode(INT8U keyMode, INT8U rawKey, KEY_TYPE funcKey)
{
	KeyDrvSetFuncCode(keyMode, rawKey, funcKey);
}

/**
********************************************************************************
* @brief    Key mode에 따른 Key Function code를 받는 함수
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
* @brief    Key mode에 따른 Key Function code와 raw key code를 받는 함수
* @param    keyMode : KEY_MODE_NORMAL, KEY_MODE_ALPHABET, KEY_MODE_HEX
*           funcCode : Key function code (raw key code 확인 용)
* @return   raw key code (0xff : error, funcCode와 일치하지 않음)
* @remark   바로 전에 읽은 raw key code를 반환
********************************************************************************
*/
INT8U KeyGetPrevRawCode(INT8U keyMode, KEY_TYPE funcCode)
{  
	INT8U rawKey;

	RingBufRewindChar(&KeyDrvRingBuf);//Key 1개 되돌림
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
* @brief    Menu Mode에서 사용할 Key를 Display Driver로 부터 받아서 반환함\n
* @param    num : menu 모드 Key 기능 번호
* @return   Key Driver로 부터 반환 받은 Key 값
* @remark   menu Mode에서 사용할 Key를 Key Driver가 결정하기 위해
*           사용 되는 함수 임\n
*           KEY_MENU_ENTER_NUM, KEY_MENU_C_NUM, KEY_MENU_UP_NUM, KEY_MENU_DOWN_NUM
********************************************************************************
*/
KEY_TYPE KeyGetPgmKeyByNum(INT8U num)
{
	return KeyDrvGetPgmKeyByNum(num);
}

/**
********************************************************************************
* @brief    한번 읽은 Key값을 다음에 처리하기 위해 되돌리는 함수\n
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
* @brief    Key API Set Checksum 함수\n
* @param    none
* @return   none
* @remark   communication에서 eeprom key area write시 사용\n
*           checksum 계산이 필요함으로 사용
********************************************************************************
*/
void KeySaveChecksum(void)
{
	KeyDrvSaveFuncTable(UPDATE_CHECKSUM);
}


/**
********************************************************************************
* @brief    KeyGetUlDisplayFlag 함수\n
* @param    none
* @return   UlDisplayModeFlag
* @remark   UL 인증을 받기위한 UL 야display flag
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
* @brief    check auto tare enable 함수\n
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
