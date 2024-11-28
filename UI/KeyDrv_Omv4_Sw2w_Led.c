/**
********************************************************************************
* Copyright (c) 2018 CAS
* @brief   Key Driver Source File\n
           for OMV4 SW LED
* @file    KeyDrv_Omv4_Sw2w_Led.c
* @version 1.0
* @date    2018/09/18
* @author  Park Young Min (FDC954)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2018/09/18 created by FDC954 \n
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

#ifdef KEY_DRV_OMV4_SW2W_LED
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
/** @brief Key ���� ��ȿ������ ������ �ð� */
#define KEY_DRV_VALID_TIME	40	//���� ms
/** @brief KeyDrvProc�� ����Ǵ� �ð� ���� */
#define KEY_DRV_PROC_TIME_INTERVAL	1//���� ms

/*
********************************************************************************
*                       LOCAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert local typedef & structure here */
/** @brief ��� ���� Key ���� �ٸ� Key�� ��ü�� ���� �ð�, ���� ms*/
#define KEY_DRV_LONG_VALID_TIME	3000


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
/** @brief ��ȯ table��ġ�� ���� raw Data */
static data INT8U  keyDrvRawData;//data for high speed in interrupt
/** @brief ������ ������ raw Data */
static data INT8U  keyDrvValidRawData;//data for high speed in interrupt
/** @brief ������ raw data�� ������ ���� */
static data INT16U  keyDrvPressCount;//data for high speed in interrupt
/** @brief raw data�� ��ȿ������ ������ Ƚ�� (KeyDrvProc ���� ���ݿ� ����)*/
static data INT16U keyDrvValidCount;//data for high speed in interrupt
/** @brief raw data�� ��� ���� Key������ ��ü�� ���� Ƚ�� (KeyDrvProc ���� ���ݿ� ����)*/
static INT16U KeyDrvLongValidCount;

/** @brief power key on flag */
static BOOLEAN keyDrvPowerKeyFlag;

/** @brief long key on flag */
static BOOLEAN keyDrvLongKeyFlag;

/** @brief key type flag (0:hold, 1:mode, 2:unit change) */
static data INT8U keyDrvType;


/** @brief convert raw code to human readable position code */
static const INT8U code KeyDrvConvTable[KEY_DRV_MAX_KEY_NUM] =
{
//	0x04 0x08 0x10  pwr  0x18
	0,	 1,	  2,	3,   4 
};

//#ifdef USE_MODE_KEY
///** @brief raw key(Ű ��ġ ��ȣ)�� key function code�� ��ȯ�ϴ� table */
//const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] =
//{
//	{
//		KEY_ZERO,	KEY_TARE,	KEY_MODE, 	KEY_ON_OFF, 	KEY_NOT_EXISTED
//	}
//};
//
///**
//* @brief menu mode���� ����� key func code 
//* @remark Enter, C, MenuUp, MenuDown, INCrease, ROTate ������ (see KEY_MENU_ENTER_NUM~)
//*/
//const KEY_TYPE code KeyDrvFuncTableForPgm[KEY_MAX_KEY_NUM_FOR_PGM] = 
//{
//	KEY_ZERO,	KEY_NOT_EXISTED,	KEY_TARE,	KEY_MODE,	KEY_MODE,	KEY_TARE, KEY_NOT_EXISTED, KEY_NOT_EXISTED, KEY_NOT_EXISTED
//};
//#endif
//
//#ifdef USE_HOLD_KEY
///** @brief raw key(Ű ��ġ ��ȣ)�� key function code�� ��ȯ�ϴ� table */
//const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] =
//{
//	{
//		KEY_ZERO,	KEY_TARE,	KEY_HOLD, 	KEY_ON_OFF, 	KEY_NOT_EXISTED
//	}
//};
//
///**
//* @brief menu mode���� ����� key func code 
//* @remark Enter, C, MenuUp, MenuDown, INCrease, ROTate ������ (see KEY_MENU_ENTER_NUM~)
//*/
//const KEY_TYPE code KeyDrvFuncTableForPgm[KEY_MAX_KEY_NUM_FOR_PGM] = 
//{
//	KEY_ZERO,	KEY_NOT_EXISTED,	KEY_TARE,	KEY_HOLD,	KEY_HOLD,	KEY_TARE, KEY_NOT_EXISTED, KEY_NOT_EXISTED, KEY_NOT_EXISTED
//};
//#endif
//
//#ifdef USE_HOLD_UNIT_KEY
///** @brief raw key(Ű ��ġ ��ȣ)�� key function code�� ��ȯ�ϴ� table */
//const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] =
//{
//	{
//		KEY_ZERO,	KEY_TARE,	KEY_HOLD, 	KEY_ON_OFF, 	KEY_UNIT_CHANGE
//	}
//};
//
///**
//* @brief menu mode���� ����� key func code 
//* @remark Enter, C, MenuUp, MenuDown, INCrease, ROTate ������ (see KEY_MENU_ENTER_NUM~)
//*/
//const KEY_TYPE code KeyDrvFuncTableForPgm[KEY_MAX_KEY_NUM_FOR_PGM] = 
//{
//	KEY_ZERO,	KEY_NOT_EXISTED,	KEY_TARE,	KEY_HOLD,	KEY_HOLD,	KEY_TARE, KEY_NOT_EXISTED, KEY_NOT_EXISTED, KEY_NOT_EXISTED
//};
//#endif

/** @brief raw key(Ű ��ġ ��ȣ)�� key function code�� ��ȯ�ϴ� table */
const KEY_TYPE code KeyDrvDefaultFuncTable[3][1][KEY_DRV_MAX_KEY_NUM] =
{	
	{
		KEY_ZERO,	KEY_TARE,	KEY_HOLD, 	KEY_ON_OFF, 	KEY_DUMMY
	},
	{
		KEY_ZERO,	KEY_TARE,	KEY_MODE, 	KEY_ON_OFF, 	KEY_DUMMY
	},
	{
		KEY_ZERO,	KEY_TARE,	KEY_HOLD, 	KEY_ON_OFF, 	KEY_UNIT_CHANGE
	}
};

/**
* @brief menu mode���� ����� key func code 
* @remark Enter, C, MenuUp, MenuDown, INCrease, ROTate ������ (see KEY_MENU_ENTER_NUM~)
*/
const KEY_TYPE code KeyDrvFuncTableForPgm[3][KEY_MAX_KEY_NUM_FOR_PGM] = 
{
	{
		KEY_ZERO,	KEY_DUMMY,	KEY_TARE,	KEY_HOLD,	KEY_HOLD,	KEY_TARE, KEY_NOT_EXISTED, KEY_NOT_EXISTED, KEY_NOT_EXISTED
	},
	{
		KEY_ZERO,	KEY_DUMMY,	KEY_TARE,	KEY_MODE,	KEY_MODE,	KEY_TARE, KEY_NOT_EXISTED, KEY_NOT_EXISTED, KEY_NOT_EXISTED
	},
	{
		KEY_ZERO,	KEY_UNIT_CHANGE,	KEY_TARE,	KEY_HOLD,	KEY_HOLD,	KEY_TARE, KEY_NOT_EXISTED, KEY_NOT_EXISTED, KEY_NOT_EXISTED
	}
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
    while (POWER_KEY_CHK == 0);	//wait Pwr key off
	RingBufInit(&KeyDrvRingBuf, &keyDrvData[0], KEY_DRV_BUF_SIZE);
	KeyDrvEnable(ON);
	keyDrvRawData = 0xff;
	keyDrvValidRawData = 0xff;
	keyDrvPressCount = 0;
	keyDrvPowerKeyFlag = OFF;
        keyDrvLongKeyFlag = ON;
	keyDrvValidCount = KEY_DRV_VALID_TIME / KEY_DRV_PROC_TIME_INTERVAL;	
	keyDrvType = ParamReadByte(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_TYPE);
	if (keyDrvType > 2) 
	{
		keyDrvType = 0;
		KeyDrvSetFuncCode(KEY_MODE_KEY_TYPE, 0, 0);
	}
        KeyDrvLongValidCount = KEY_DRV_LONG_VALID_TIME / (5 * KEY_DRV_PROC_TIME_INTERVAL);
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
* @brief    keyDrvLongKeyEnable  �Լ�\n
* @param    none
* @return   1 -> LongKey ON, 0 -> LongKey Enable off
* @remark   
********************************************************************************
*/
BOOLEAN KeyDrvLongKeyEnable(void)
{
        keyDrvLongKeyFlag = ON;
}

/**
********************************************************************************
* @brief    keyDrvLongKeyEnable  �Լ�\n
* @param    none
* @return   1 -> LongKey ON, 0 -> LongKey Enable off
* @remark   
********************************************************************************
*/
BOOLEAN KeyDrvLongKeyDisable(void)
{
        keyDrvLongKeyFlag = OFF;
}



/**
********************************************************************************
* @brief    Key Driver Procedure �Լ�\n
* @param    none
* @return   none
* @remark   keys, scan �Ͽ� Key�� ���� ��ġ ���� Ring Buffer�� �Է�\n
*           Timer Interrupt���� ����� (per 1 ms) (\n
*           ���� ��ġ = ����� �ν��� �� �ִ� ��ġ\n
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
		if (KEY_PORT_MODE_HOLD == 0)
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
* @brief    Key Driver Set Function Code �Լ�\n
* @param    keyMode :  KEY_MODE_NORMAL, KEY_MODE_ALPHABET, KEY_MODE_HEX...
*           rawData : Raw Key Code 
*           funcNum : Function Key Number
* @return   none
* @remark   ���� ������ KeyDrvSaveFuncTable() ���
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
* @brief    Key mode�� ���� Key Function code�� �޴� �Լ�
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
                
                if (keyDrvLongKeyFlag == ON)
                {
                  if (((funcKey == KEY_ZERO)|| (funcKey == KEY_TARE)||(funcKey == KEY_HOLD)) && (keyDrvPressCount != 0) && !(RingBufCheckBuf(&KeyDrvRingBuf)))
  //                if ((funcKey == KEY_HOLD) && (keyDrvPressCount != 0))
                  {
                          if (keyDrvPressCount > KeyDrvLongValidCount)
                          {
                                  switch(funcKey)
                                  {
                                          case KEY_ZERO:
                                                  funcKey = KEY_UNIT_CHANGE;
                                                  break;
//                                          case KEY_TARE:
//                                                  funcKey = KEY_BACK_LIGHT;
                                                  break;
                                          case KEY_HOLD:
                                                  funcKey = KEY_MODE;
                                                  break;
                                  }
                          }
                          else
                          {
                                  funcKey = KEY_NOT_PRESSED;
                                  RingBufRewindChar(&KeyDrvRingBuf);
                          }
                  }
                }
	}	
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
	return KeyDrvFuncTableForPgm[keyDrvType][num];
}
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

#endif//#ifdef KEY_DRV_OMV4_SW2W_LED

