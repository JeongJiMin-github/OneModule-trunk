/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Key Driver Dource File\n
           for ER-1 LED
* @file    KeyDrv.c
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
#include "../Common/UiDefine.h"
#include "../DB/StorageApi.h"
#include "../DB/ParamApi.h"
#include "KeyDrv.h"

#include "../Hardware/HardwareConfig.h"
#include "../Hardware/HardwareDrv.h"

#include "../UI/DispApi.h"

#ifdef KEY_DRV_IDP_LED
// INT8U keyInByte;
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
/** @brief key ���� */
#define KEY_DRV_MAX_KEY_NUM	5
/** @brief key ring buffer size */
#define KEY_DRV_BUF_SIZE	5
/** @brief �Է� Pin ���� */
#define KEY_DRV_IN_PIN_NUM	 4
/** @brief ��� Pin ���� */
#define KEY_DRV_OUT_PIN_NUM	4
/** @brief KeyDrvProc�� ����Ǵ� �ð� ����, ���� ms*/
#define KEY_DRV_PROC_TIME_INTERVAL	1
/** @brief Key ���� ��ȿ������ ������ �ð�, ���� ms*/
#define KEY_DRV_VALID_TIME	40
/** @brief ��� ���� Key ���� �ٸ� Key�� ��ü�� ���� �ð�, ���� ms*/
//#define KEY_DRV_LONG_VALID_TIME	3000 

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
/** @brief key Driver Ring Buffer���� ����� Byte Array */
static char  keyDrvData[KEY_DRV_BUF_SIZE];
/** @brief Key�� Index */
//static data INT8U  keyDrvIndex;//data for high speed in interrupt
// static INT8U keyDrvIndex;
/** @brief ��ȯ table��ġ�� ���� raw Data */
static data INT8U  keyDrvRawData; // Compiler ������ ���� data�� ������, �񱳹� ���� �߻�
/** @brief ������ ������ raw Data */
static data INT8U  keyDrvValidRawData;//data for high speed in interrupt
/** @brief ������ raw data�� ������ Ƚ�� (KeyDrvProc ���� ���ݿ� ����)*/
static data INT16U keyDrvPressCount;//data for high speed in interrupt
/** @brief raw data�� ��ȿ������ ������ ���� Ƚ�� (KeyDrvProc ���� ���ݿ� ����)*/
static data INT16U keyDrvValidCount;//data for high speed in interrupt
/** @brief raw data�� ��� ���� Key������ ��ü�� ���� Ƚ�� (KeyDrvProc ���� ���ݿ� ����)*/
static INT16U KeyDrvLongValidCount;

/** @brief power key �� ������ Ƚ�� (KeyDrvProc ���� ���ݿ� ����)*/
//static data INT16U keyDrvPowerKeyPressCount;//data for high speed in interrupt
/** @brief power key ���� ���� */
//BOOLEAN keyDrvPowerPortStatus;
/** @brief power key on flag */
BOOLEAN keyDrvPowerKeyFlag;
// INT8U keyInByte;
static const INT8U code KeyDrvConvTable[KEY_DRV_MAX_KEY_NUM] =
{
//	0x04 0x08 0x10  pwr  0x18
	0,	 1,	  2,	3,   4 
};
/** @brief convert raw code to human readable position code */
const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] =
{
	{
		KEY_ZERO,	KEY_TARE,	KEY_MODE, 	KEY_ON_OFF, 	KEY_NOT_EXISTED
	}
};

/**
* @brief menu mode���� ����� key func code 
* @remark Enter, C, MenuUp, MenuDown, INCrease, ROTate ������ (see KEY_MENU_ENTER_NUM~)
*/
const KEY_TYPE code KeyDrvFuncTableForPgm[KEY_MAX_KEY_NUM_FOR_PGM] = 
{
	KEY_TARE,	KEY_NOT_EXISTED,	KEY_ZERO,	KEY_MODE,	KEY_MODE,	KEY_ZERO, KEY_NOT_EXISTED, KEY_NOT_EXISTED, KEY_NOT_EXISTED
};
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
* @brief    Key Driver Init �Լ�\n
*           Key Driver Ring Buffer Init\n
* @param    none
* @return   none
* @remark   �ʱ⿡ �ѹ� ���� �Ǵ� �Լ�\n
********************************************************************************
*/

void KeyDrvInit(void)
{
//	while (POWER_KEY_CHK == 0);	//wait Pwr key off

	RingBufInit(&KeyDrvRingBuf, &keyDrvData[0], KEY_DRV_BUF_SIZE);
	KeyDrvEnable(ON);

//	keyDrvRawData = 0xff;
//	keyDrvValidRawData = 0xff;
//	keyDrvPressCount = 0;
//	keyDrvPowerKeyFlag = OFF;
//	keyDrvValidCount = KEY_DRV_VALID_TIME / (KEY_DRV_OUT_PIN_NUM * KEY_DRV_PROC_TIME_INTERVAL);
//	KeyDrvLongValidCount = KEY_DRV_LONG_VALID_TIME / (KEY_DRV_OUT_PIN_NUM * KEY_DRV_PROC_TIME_INTERVAL);
	keyDrvRawData = 0xff;
	keyDrvValidRawData = 0xff;
	keyDrvPressCount = 0;
	keyDrvPowerKeyFlag = OFF;
	keyDrvValidCount = KEY_DRV_VALID_TIME / KEY_DRV_PROC_TIME_INTERVAL;
}

/**
********************************************************************************
* @brief    Key Driver Function Table load �Լ�\n
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
* @brief    Key Driver Function Table Save �Լ�\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void KeyDrvSaveFuncTable(INT8U mode)
{
}

/**
********************************************************************************
* @brief    Key Driver Function Table default �Լ�\n
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
* @brief    Key Driver Enable �Լ�\n
* @param    on/off flag
* @return   none
* @remark   Key �Է��� ���(ON)/����(OFF) �Ѵ�.\n
********************************************************************************
*/
void KeyDrvEnable(BOOLEAN on)
{
	KeyDrvEnableFlag = on;
}

/**
********************************************************************************
* @brief    Check Calibration Port �Լ�\n
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
* @brief    check Power Port �Լ�\n
* @param    none
* @return   1 -> power Port on, 0 -> power Port off
* @remark   
********************************************************************************
*/

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

/**
********************************************************************************
* @brief    check Power Key �Լ�\n
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
* @brief    Key Driver Procedure �Լ�\n
* @param    none
* @return   none
* @remark   8x6 = 48 keys, scan �Ͽ� Key�� ���� ��ġ ���� Ring Buffer�� �Է�\n
*           Timer Interrupt���� ����� (per 1 ms) (\n
*           ���� ��ġ = ����� �ν��� �� �ִ� ��ġ\n
*           ��) 4x3 key�� ���\n
*               0  1  2  3\n
*               4  5  6  7\n
*               8  9  10 11\n
*           ��) 6x1 key�� ���\n
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
		if (KEY_PORT_MODE == 0)
		{
			keyDrvRawData = 4;
		}
		else
		{
			keyDrvRawData = 1;
		}
	}
	else if (POWER_KEY_CHK == 0)
	{
		keyDrvRawData = 3;
	}
	else if (KEY_PORT_MODE == 0)
	{
		keyDrvRawData = 2;
	}	
	else
	{
		keyDrvRawData = 0xff;
	}
//	digitValue=keyDrvRawData;
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
* @brief    Key Driver Set Function Code �Լ�\n
* @param    keyMode :  KEY_MODE_NORMAL, KEY_MODE_ALPHABET, KEY_MODE_HEX...
*           rawData : Raw Key Code 
*           funcNum : Function Key Number
* @return   none
* @remark   
********************************************************************************
*/
void KeyDrvSetFuncCode(INT8U keyMode, INT8U rawKey, KEY_TYPE funcKey)
{
}

/**
********************************************************************************
* @brief    Key mode�� ���� Key Function code�� �޴� �Լ�
* @param    keyMode : KEY_MODE_NORMAL, KEY_MODE_ALPHABET, KEY_MODE_HEX...
*           rawKey : raw Key Data
* @return   Key Function code
* @remark   KEY_NUM_0...KEY_NUM_A...KEY_C...KEY_PLU_1...KEY_NOT_PRESSED
********************************************************************************
*/
KEY_TYPE KeyDrvGetFuncCode(INT8U keyMode, INT8U rawKey)
{  
 	KEY_TYPE funcKey;
	
	if (rawKey >= KEY_DRV_MAX_KEY_NUM)
	{
		return KEY_NOT_PRESSED;
	}

	funcKey = KeyDrvDefaultFuncTable[0][rawKey];	//SW has only normal key table

	return funcKey;
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
KEY_TYPE KeyDrvGetPgmKeyByNum(INT8U num)
{
	return KeyDrvFuncTableForPgm[num];
}
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

#endif//#ifdef KEY_DRV_ER_MLED

