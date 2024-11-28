/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Driver Source File\n
* @file    KeyDrv_Omv3_Sw_LCD.c
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
#include "../Common/RingBuf.h"
#include "../Common/UiDefine.h"
#include "../DB/StorageApi.h"
#include "../DB/ParamApi.h"
#include "KeyDrv.h"
#include "../Hardware/HardwareConfig.h"
#include "../AD/AdDrv.h"
#include "../AD/AdDrv_Efm32g840.h"
#ifdef KEY_DRV_OMV3_FW500_LCD

/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief key Driver Ring Buffer */
RING_BUF  KeyDrvRingBuf;
/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief key 개수 */
#define KEY_DRV_MAX_KEY_NUM	5

/** @brief key ring buffer size */
#define KEY_DRV_BUF_SIZE	5
/** @brief Key 값을 유효값으로 인정할 시간 */
#define KEY_DRV_VALID_TIME	40	//단위 ms
/** @brief KeyDrvProc가 실행되는 시간 간격 */
#define KEY_DRV_PROC_TIME_INTERVAL	1//단위 ms
/** @brief POWERCHECK 의 Avr size*/
#define POWERCHECK_AD_AVR_SIZE 3


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
/** @brief key Driver Enable flag */
static BOOLEAN KeyDrvEnableFlag;
/** @brief key Driver Ring Buffer에서 사용할 Byte Array */
static char  keyDrvData[KEY_DRV_BUF_SIZE];
/** @brief 변환 table거치지 않은 raw Data */
static data INT8U  keyDrvRawData;//data for high speed in interrupt
/** @brief 이전에 눌려진 raw Data */
static data INT8U  keyDrvValidRawData;//data for high speed in interrupt
/** @brief 동일한 raw data가 눌려진 개수 */
static data INT16U  keyDrvPressCount;//data for high speed in interrupt
/** @brief raw data가 유효값으로 인정할 횟수 (KeyDrvProc 실행 간격에 의존)*/
static data INT16U keyDrvValidCount;//data for high speed in interrupt

/** @brief power key on flag */
static BOOLEAN keyDrvPowerKeyFlag;

/** @brief key type flag (0:hold, 1:mode, 2:unit change) */
static data INT8U keyDrvType;
/** @brief convert raw code to human readable position code */
static const INT8U code KeyDrvConvTable[KEY_DRV_MAX_KEY_NUM] =
{
//	0x04 0x08 0x10  pwr  0x18
	0,	 1,	  2,	3,   4 
};

/** @brief power check ring buffer 주소 리턴 */
static RING_BUF *powerCheckRingBufPtr;

static INT8U UlDisplayModeFlag;   // 0:U mode , 1:UL mode 




//#ifdef USE_UNITCHANGE_KEY
///** @brief raw key(키 위치 번호)를 key function code로 변환하는 table */
//const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] =
//{
//	{
//		KEY_ZERO, KEY_TARE,	KEY_UNIT_CHANGE, KEY_ON_OFF, KEY_NOT_EXISTED
//	}
//};
///**
//* @brief menu mode에서 사용할 key func code 
//* @remark Enter, C, MenuUp, MenuDown, INCrease, ROTate 순서임 (see KEY_MENU_ENTER_NUM~)
//*/
//const KEY_TYPE code KeyDrvFuncTableForPgm[KEY_MAX_KEY_NUM_FOR_PGM] = 
//{
//	KEY_ZERO, KEY_NOT_EXISTED, KEY_TARE,	 KEY_UNIT_CHANGE, KEY_UNIT_CHANGE, KEY_TARE, KEY_NOT_EXISTED, KEY_NOT_EXISTED, KEY_NOT_EXISTED
//};
//#endif
//
//#ifdef USE_HOLD_KEY
///** @brief raw key(키 위치 번호)를 key function code로 변환하는 table */
//const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] =
//{
//	{
//		KEY_ZERO, KEY_TARE,	KEY_HOLD, KEY_ON_OFF, KEY_NOT_EXISTED
//	}
//};
///**
//* @brief menu mode에서 사용할 key func code 
//* @remark Enter, C, MenuUp, MenuDown, INCrease, ROTate 순서임 (see KEY_MENU_ENTER_NUM~)
//*/
//const KEY_TYPE code KeyDrvFuncTableForPgm[KEY_MAX_KEY_NUM_FOR_PGM] = 
//{
//	KEY_ZERO, KEY_NOT_EXISTED, KEY_TARE,	 KEY_HOLD, KEY_HOLD, KEY_TARE, KEY_NOT_EXISTED, KEY_NOT_EXISTED, KEY_NOT_EXISTED
//};
//#endif
//
//#ifdef USE_MODE_KEY
///** @brief raw key(키 위치 번호)를 key function code로 변환하는 table */
//const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] =
//{
//	{
//		KEY_ZERO, KEY_TARE, KEY_MODE, KEY_ON_OFF, KEY_NOT_EXISTED
//	}
//};
///**
//* @brief menu mode에서 사용할 key func code 
//* @remark Enter, C, MenuUp, MenuDown, INCrease, ROTate 순서임 (see KEY_MENU_ENTER_NUM~)
//*/
//const KEY_TYPE code KeyDrvFuncTableForPgm[KEY_MAX_KEY_NUM_FOR_PGM] = 
//{
//	KEY_ZERO, KEY_NOT_EXISTED, KEY_TARE,	 KEY_MODE, KEY_MODE, KEY_TARE, KEY_NOT_EXISTED, KEY_NOT_EXISTED, KEY_NOT_EXISTED
//};
//#endif

/** @brief raw key(키 위치 번호)를 key function code로 변환하는 table */
const KEY_TYPE code KeyDrvDefaultFuncTable[3][1][KEY_DRV_MAX_KEY_NUM] =
{
	{
		KEY_ZERO, KEY_TARE,	KEY_HOLD, KEY_ON_OFF, KEY_DUMMY
	},
	{
		KEY_ZERO, KEY_TARE, KEY_MODE, KEY_ON_OFF, KEY_DUMMY
	},
	{
		KEY_ZERO, KEY_TARE,	KEY_UNIT_CHANGE, KEY_ON_OFF, KEY_DUMMY
	}
};
/**
* @brief menu mode에서 사용할 key func code 
* @remark Enter, C, MenuUp, MenuDown, INCrease, ROTate 순서임 (see KEY_MENU_ENTER_NUM~)
*/
const KEY_TYPE code KeyDrvFuncTableForPgm[3][KEY_MAX_KEY_NUM_FOR_PGM] = 
{
	{
		KEY_ZERO, KEY_DUMMY, KEY_TARE,	 KEY_HOLD, KEY_HOLD, KEY_TARE, KEY_NOT_EXISTED, KEY_NOT_EXISTED, KEY_NOT_EXISTED
	},
	{
		KEY_ZERO, KEY_DUMMY, KEY_TARE,	 KEY_MODE, KEY_MODE, KEY_TARE, KEY_NOT_EXISTED, KEY_NOT_EXISTED, KEY_NOT_EXISTED
	},
	{
		KEY_ZERO, KEY_DUMMY, KEY_TARE,	 KEY_UNIT_CHANGE, KEY_UNIT_CHANGE, KEY_TARE, KEY_NOT_EXISTED, KEY_NOT_EXISTED, KEY_NOT_EXISTED
	}
};


/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
INT8U powerCheckEnable(void);

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
/**
********************************************************************************
* @brief    Key Driver Init 함수\n
*           Key Driver Ring Buffer Init\n
* @param    none
* @return   none
* @remark   초기에 한번 실행 되는 함수\n
********************************************************************************
*/
void KeyDrvInit(void)
{
    powerCheckRingBufPtr = (&AdDrvRingBufForPowerCheck);
    while (powerCheckEnable() == 1);	//wait Pwr key off      // duckspg FW lcd로 따로 쓰기때문에 0에서 1로 그냥 바꿈 
	RingBufInit(&KeyDrvRingBuf, &keyDrvData[0], KEY_DRV_BUF_SIZE);
	KeyDrvEnable(ON);
	keyDrvRawData = 0xff;
	keyDrvValidRawData = 0xff;
	keyDrvPressCount = 0;
	keyDrvPowerKeyFlag = OFF;
	keyDrvValidCount = KEY_DRV_VALID_TIME / KEY_DRV_PROC_TIME_INTERVAL;
	keyDrvType = ParamReadByte(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_TYPE);
        UlDisplayModeFlag = 0; // UL모드가 아닐때 flag는 0이 되어야 함 
	if (keyDrvType == 3) 
	{
		keyDrvType = 2;
		UlDisplayModeFlag  = 1;   // UL 모드일때는 flag를 1로 만들어준다. 
	}
	if (keyDrvType > 2) 
	{
		keyDrvType = 0;
		KeyDrvSetFuncCode(KEY_MODE_KEY_TYPE, 0, 0);
	}
}

/**
********************************************************************************
* @brief    Key Driver Function Table load 함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void KeyDrvLoadFuncTable(void)
{
}

/**
********************************************************************************
* @brief    Key Driver Function Table Save 함수\n
* @param    INT8U
* @return   none
* @remark   
********************************************************************************
*/
void KeyDrvSaveFuncTable(INT8U mode)
{
}

/**
********************************************************************************
* @brief    Key Driver Function Table default 함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void KeyDrvSetDefaultFuncTable(void)
{
}

/**
********************************************************************************
* @brief    Key Driver Enable 함수\n
* @param    on/off flag
* @return   none
* @remark   Key 입력을 허용(ON)/불허(OFF) 한다.\n
********************************************************************************
*/
void KeyDrvEnable(BOOLEAN on)
{
	KeyDrvEnableFlag = on;
}

/**
********************************************************************************
* @brief    Check Calibration Port 함수\n
* @param    none
* @return   1 -> Calibration Port on, 0 -> Calibration Port off
* @remark   
********************************************************************************
*/
BOOLEAN KeyDrvCheckCalPort(void)
{
	if (SW_CAL == 0)
	{
		return ON;
	}
	else
	{
		return OFF;
	}
}
/**
********************************************************************************
* @brief    KeyUlDisplayFlag 함수\n
* @param    none
* @return   UlDisplayModeFlag
* @remark   UL 인증을 받기위한 UL 야display flag
********************************************************************************
*/


INT8U KeyDrvUlDisplayFlag(void)
{
    return UlDisplayModeFlag;
}

/**
********************************************************************************
* @brief    check Power Port 함수\n
* @param    none
* @return   1 -> power Port on, 0 -> power Port off
* @remark   
********************************************************************************
*/
/*
BOOLEAN KeyDrvCheckPowerPort(void)
{
	if (POWER_KEY_CHK == 0) 
	{
		return ON;
	}
	else
	{
		return OFF;
	}
}
*/

/**
********************************************************************************
* @brief    check Power Key 함수\n
* @param    none
* @return   1 -> power key on, 0 -> power key off
* @remark   
********************************************************************************
*/
BOOLEAN KeyDrvCheckPowerKey(void)
{
	if (keyDrvPowerKeyFlag)
	{
		keyDrvPowerKeyFlag = OFF;
		return ON;
	}
	else
	{
		return OFF;
	}
}

/**
********************************************************************************
* @brief    Key Driver Procedure 함수\n
* @param    none
* @return   none
* @remark   keys, scan 하여 Key의 절대 위치 값을 Ring Buffer에 입력\n
*           Timer Interrupt에서 실행됨 (per 1 ms) (\n
*           절대 위치 = 사람이 인식할 수 있는 위치\n
*           예) 6x1 key의 경우\n
*               0  1  2  3  4  5  6
********************************************************************************
*/
void  KeyDrvProc(void)
{  

	if (KEY_PORT_ZERO == 0)
	{
		keyDrvRawData = 0;
	}
	else if (KEY_PORT_TARE == 0)
	{
		if (KEY_PORT_MODE_HOLD == 0)
		{
			keyDrvRawData = 4;
		}
		else
		{
			keyDrvRawData = 1;
		}
	}
	else if (powerCheckEnable() == 1)        // duckspg FW lcd로 따로 쓰기때문에 0에서 1로 그냥 바꿈 
	{
		keyDrvRawData = 3;
	}
	else if (KEY_PORT_MODE_HOLD == 0)
	{
		keyDrvRawData = 2;
	}	
	else
	{
		keyDrvRawData = 0xff;
	}
	
	if (keyDrvRawData == 0xff)
	{
		keyDrvValidRawData = 0xff;
		keyDrvPressCount = 0;
	}
	else
	{
		//Key is pressed
		if (keyDrvValidRawData == keyDrvRawData)
		{
			keyDrvPressCount++;
			if (keyDrvPressCount == keyDrvValidCount)
			{
				if (keyDrvRawData == 3)		//power key 
				{
					keyDrvPowerKeyFlag = ON;
				}
				else
				{
					if (KeyDrvEnableFlag)
					{
						RingBufPutCharForKeyInterrupt(&KeyDrvRingBuf, KeyDrvConvTable[keyDrvRawData]);
					}
				}
			}
		}
		else
		{
			keyDrvValidRawData = keyDrvRawData;
			keyDrvPressCount = 0;
		}
	}
}

/**
********************************************************************************
* @brief    Key Driver Set Function Code 함수\n
* @param    keyMode :  KEY_MODE_NORMAL, KEY_MODE_ALPHABET, KEY_MODE_HEX...
*           rawData : Raw Key Code 
*           funcNum : Function Key Number
* @return   none
* @remark   영구 저장은 KeyDrvSaveFuncTable() 사용
********************************************************************************
*/
void KeyDrvSetFuncCode(INT8U keyMode, INT8U rawKey, KEY_TYPE funcKey)
{
	if (keyMode == KEY_MODE_KEY_TYPE)
	{
		ParamWriteByte(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_TYPE, (INT8U)funcKey);				
	}
}

/**
********************************************************************************
* @brief    Key mode에 따른 Key Function code를 받는 함수
* @param    keyMode : KEY_MODE_NORMAL, KEY_MODE_ALPHABET, KEY_MODE_HEX
*           rawKey : raw Key Data
* @return   Key Function code
* @remark   KEY_NUM_0...KEY_NUM_A...KEY_C...KEY_PLU_1...KEY_NOT_PRESSED
********************************************************************************
*/
KEY_TYPE KeyDrvGetFuncCode(INT8U keyMode, INT8U rawKey)
{  
	KEY_TYPE funcKey;

	if (keyMode == KEY_MODE_KEY_TYPE)
	{	
		funcKey = (KEY_TYPE)ParamReadByte(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_TYPE);
	}
	else
	{
		if (rawKey >= KEY_DRV_MAX_KEY_NUM)
		{
			return KEY_NOT_PRESSED;
		}
	
		funcKey = KeyDrvDefaultFuncTable[keyDrvType][0][rawKey];	//SW has only normal key table
	}	
	return funcKey;
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
KEY_TYPE KeyDrvGetPgmKeyByNum(INT8U num)
{
	return KeyDrvFuncTableForPgm[keyDrvType][num];
}
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
/**
********************************************************************************
* @brief    power check를 A/D로 하기 \n
* @param    num : menu 모드 Key 기능 번호
* @return   Key Driver로 부터 반환 받은 Key 값
* @remark   menu Mode에서 사용할 Key를 Key Driver가 결정하기 위해
*           사용 되는 함수 임\n
*           KEY_MENU_ENTER_NUM, KEY_MENU_C_NUM, KEY_MENU_UP_NUM, KEY_MENU_DOWN_NUM
********************************************************************************
*/

INT8U powerCheckEnable(void)
{
    static INT32U powerFiltValue = 0;
    static INT8U powerCheckFlag = 1;
    INT32U avrPowerCheckVolt = 0;
    INT8U size = 0;
    INT8U i = 0;
    
    size = RingBufCountSize(powerCheckRingBufPtr);
	if (size >= POWERCHECK_AD_AVR_SIZE)
	{
        avrPowerCheckVolt = 0;
        for (i = 0; i < POWERCHECK_AD_AVR_SIZE; i++)
        {
            avrPowerCheckVolt = avrPowerCheckVolt + RingBufGetInt32s(powerCheckRingBufPtr);
        }
        
        powerFiltValue = avrPowerCheckVolt / POWERCHECK_AD_AVR_SIZE;
        
        if (powerFiltValue > 200)
        {
            powerCheckFlag = 1;
        }
        else
        {
            powerCheckFlag = 0;
        }
    }
    return powerCheckFlag;
    
}



#endif//#ifdef KEY_DRV_OMV3_SW_LCD
