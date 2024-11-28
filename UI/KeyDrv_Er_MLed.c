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

#ifdef KEY_DRV_ER_MLED
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
#define KEY_DRV_MAX_KEY_NUM	16
/** @brief key ring buffer size */
#define KEY_DRV_BUF_SIZE	6
/** @brief �Է� Pin ���� */
#define KEY_DRV_IN_PIN_NUM	 4
/** @brief ��� Pin ���� */
#define KEY_DRV_OUT_PIN_NUM	4
/** @brief KeyDrvProc�� ����Ǵ� �ð� ����, ���� ms*/
#define KEY_DRV_PROC_TIME_INTERVAL	1
/** @brief Key ���� ��ȿ������ ������ �ð�, ���� ms*/
#define KEY_DRV_VALID_TIME	40
/** @brief ��� ���� Key ���� �ٸ� Key�� ��ü�� ���� �ð�, ���� ms*/
#define KEY_DRV_LONG_VALID_TIME	3000 

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
#ifdef	USE_DECIMAL_POINT	
/** @brief Decimal Point key on flag */
BOOLEAN DecimalPointKeyExistFlag; //csh 20120730
#endif

/** @brief power key �� ������ Ƚ�� (KeyDrvProc ���� ���ݿ� ����)*/
//static data INT16U keyDrvPowerKeyPressCount;//data for high speed in interrupt
/** @brief power key ���� ���� */
//static BOOLEAN keyDrvPowerPortStatus;
/** @brief power key on flag */
// static BOOLEAN keyDrvPowerKeyFlag;
// INT8U keyInByte;

/** @brief convert raw code to human readable position code */

static const INT8U code KeyDrvConvTable[KEY_DRV_MAX_KEY_NUM] =
{
//	12,    13,     14,    15,//    23,
//	8,   9,    10,      11,  //  7,
//	4,    5,    6,      7, //    10,
//	0,    1,     2,    3 //    24

	0 , 1, 2, 3,
	4 , 5, 6, 7,
	8 , 9,10,11,
	12,13,14,15,
/**
	0,0,0,0,
	0,0,0,0,
	0,0,0,0,
	0,0,0,0, */
};

/*
static const INT8U code KeyDrvConvTable[KEY_DRV_MAX_KEY_NUM] =
{
//	0x00 0x01 0x02 0x03 0x04
	1,	 7,	  13,    0,     6,
//	0x05 0x06 0x07 0x08 0x09
	16,    22,   15,    18,   12,
//   0x0A 0x0B 0x0C 0x0D 0x0E
	23,   4,      10,    19,    2,
//	0x0F 0x10 0x11 0x12 0x13
	5,     11,    17,   3,     20,
//	0x14 0x15 0x16 0x17 0x18
	21,    9,     14,   8,     24
};
*/
/** @brief raw key(Ű ��ġ ��ȣ)�� key function code�� ��ȯ�ϴ� table */
static KEY_TYPE KeyDrvFuncTable[1][KEY_DRV_MAX_KEY_NUM];
/** @brief raw key(Ű ��ġ ��ȣ)�� key function code�� ��ȯ�ϴ� table�� default �� */
const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] = 
{
	{
		KEY_NUM_7,		KEY_NUM_8,		KEY_NUM_9,		KEY_PAY,	//KEY_PLU_CALL,
		KEY_NUM_4,		KEY_NUM_5,		KEY_NUM_6,		KEY_ADD,		//KEY_CANCEL,
		KEY_NUM_1,		KEY_NUM_2,		KEY_NUM_3,		KEY_SUM,			//KEY_SUM,
		KEY_NUM_0,		KEY_C,			KEY_ZERO,		KEY_TARE,		// KEY_POWER
	}
};


/** @brief Maximum Number of Fixed key*/
#define KEY_DRV_MAX_FIXED_KEY_NUM	13
/** @brief ����ڰ� ������ �� ���� Key���� �迭 */
const KEY_TYPE code KeyDrvFixedFuncTable[KEY_DRV_MAX_FIXED_KEY_NUM] =
{
	KEY_NUM_0, KEY_NUM_1, KEY_NUM_2, KEY_NUM_3, KEY_NUM_4,
	KEY_NUM_5, KEY_NUM_6, KEY_NUM_7, KEY_NUM_8, KEY_NUM_9,
	KEY_C, KEY_ZERO, KEY_TARE
};

/** 
* @brief menu mode���� ����� key func code 
* @remark Enter, C, MenuUp, MenuDown, INCrease, ROTate ������ (see KEY_MENU_ENTER_NUM~) \n
*         ���� : ���� Key ��� �� �ݵ�� INC, ROT�� KEY_NOT_EXISTED���� �ؾ���
*/
const KEY_TYPE code KeyDrvFuncTableForPgm[KEY_MAX_KEY_NUM_FOR_PGM] = 
{
	KEY_TARE,	KEY_C,	KEY_ZERO,	KEY_SUM,	KEY_PAY, KEY_ADD, KEY_NOT_EXISTED, KEY_NOT_EXISTED, KEY_NOT_EXISTED
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
	KeyDrvLoadFuncTable();
	RingBufInit(&KeyDrvRingBuf, &keyDrvData[0], KEY_DRV_BUF_SIZE);
	KeyDrvEnable(ON);

//	keyDrvIndex = 0;
	keyDrvRawData = 0xff;
	keyDrvValidRawData = 0xff;
	keyDrvPressCount = 0;
//	keyDrvPowerKeyFlag = OFF;
//	keyDrvPowerPortStatus = OFF;
//	keyDrvPowerKeyPressCount = 0;

	keyDrvValidCount = KEY_DRV_VALID_TIME / (KEY_DRV_OUT_PIN_NUM * KEY_DRV_PROC_TIME_INTERVAL);
	KeyDrvLongValidCount = KEY_DRV_LONG_VALID_TIME / (KEY_DRV_OUT_PIN_NUM * KEY_DRV_PROC_TIME_INTERVAL);
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
	INT8U i;
	INT8U j;
	INT16U modelNum;
	INT8U checkSum;
	INT8U sum;
	INT8U error;

	error = 0;
#ifdef	USE_DECIMAL_POINT		
	DecimalPointKeyExistFlag = OFF; //csh 20120823
#endif

	modelNum = ParamReadWord(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_MODEL);
	checkSum = ParamReadByte(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_CHECKSUM);
	ParamReadData(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_TABLE, (INT8U *)&KeyDrvFuncTable[0][0], KEY_DRV_MAX_KEY_NUM);

	//Default table���� Fixed Key�� Setting�Ǿ� ������ ������ Default�� load
	for (i = 0; i < KEY_DRV_MAX_KEY_NUM; i++)
	{
#ifdef	USE_DECIMAL_POINT	
		if (KeyDrvFuncTable[0][i] == KEY_POINT) //csh 20120823
		{
			DecimalPointKeyExistFlag = ON;
		}
#endif
		for (j = 0; j < KEY_DRV_MAX_FIXED_KEY_NUM; j++)
		{
			if (KeyDrvDefaultFuncTable[0][i] == KeyDrvFixedFuncTable[j])
			{
				KeyDrvFuncTable[0][i] = KeyDrvDefaultFuncTable[0][i];
				break;
			}
		}
	}
	
/*	for (i = 0; i < KEY_DRV_MAX_KEY_NUM; i++) //csh 20120730
	{
		if (KeyDrvFuncTable[0][i] == KEY_POINT)
		{
			DecimalPointKeyExistFlag = ON;
			break;
		}
		else 
		{
			DecimalPointKeyExistFlag = OFF;
		}
	}*/

	if (modelNum != MODEL_ER_LED)
	{
		error = 1;
	}

	sum = 0;
	for (i = 0; i < KEY_DRV_MAX_KEY_NUM; i++)
	{
		sum = sum + KeyDrvFuncTable[0][i];
	}

	if (checkSum != sum) 
	{
		error = 1;
	}

	if (error == 1)
	{
		KeyDrvSetDefaultFuncTable();
	}
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
	INT8U i;
	INT8U j;
	INT8U sum;

	if (mode == UPDATE_CHECKSUM)
	{
		ParamReadData(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_TABLE, (INT8U *)&KeyDrvFuncTable[0][0], KEY_DRV_MAX_KEY_NUM);
	}

	sum = 0;
	for (i = 0; i < KEY_DRV_MAX_KEY_NUM; i++)
	{
		for (j = 0; j < KEY_DRV_MAX_FIXED_KEY_NUM; j++)
		{
			//Default table���� Fixed Key�� Setting�Ǿ� ������ ������ Default�� Save
			if (KeyDrvDefaultFuncTable[0][i] == KeyDrvFixedFuncTable[j])
			{
				KeyDrvFuncTable[0][i] = KeyDrvDefaultFuncTable[0][i];
				break;
			}
			//���� table���� Fixed Key�� Setting�Ǿ� ������ ������ Default�� Save
			//Fixed key�� Default table������ ���� (���� �Ұ�)
			else if (KeyDrvFuncTable[0][i] == KeyDrvFixedFuncTable[j])
			{
				KeyDrvFuncTable[0][i] = KeyDrvDefaultFuncTable[0][i];
			}
		}
		sum = sum + KeyDrvFuncTable[0][i];
	}

	ParamWriteWord(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_MODEL, MODEL_NUM);
	ParamWriteByte(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_CHECKSUM, sum);
	ParamWriteData(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_TABLE, (INT8U *)&KeyDrvFuncTable[0][0], KEY_DRV_MAX_KEY_NUM);
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
	INT8U i;

	for (i = 0; i < KEY_DRV_MAX_KEY_NUM; i++)
	{
		KeyDrvFuncTable[0][i] = KeyDrvDefaultFuncTable[0][i];
	}
	KeyDrvSaveFuncTable(UPDATE_KEY_TABLE);
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
//	if (POWER_CHK == 0)
//	{
//		return ON;
//	}
//	else
//	{
//		return OFF;
//	}
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
	return TRUE;
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
//Key scan out table
// const INT8U code keyOutScanTable[KEY_DRV_OUT_PIN_NUM] = { 0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF};
extern INT8U digitCount;
extern INT8U digitID;
//INT8U keyDrvMap[4];
//INT8U keytempByte;

void  KeyDrvProc(void)
{
	INT8U keyInByte;

	keyInByte=KEY_IN_PORT;
	keyInByte=~keyInByte;
	keyInByte&=0xf0;
	if (digitCount==0) return;
	if (digitID) {
		if (keyInByte) goto M1;
	}
	if (digitCount<5) {
		if (keyInByte==0) digitID=digitCount;
	}
	if (digitCount==8) {
		if (keyInByte==0) digitID=0;
		goto M1;
	}
	if (digitID  ==0) return;
	if (keyInByte==0) return;
M1:
	KeyOutputPortStatus=0xff;
	if (keyInByte==0x10) KeyOutputPortStatus=digitID-1;
	if (keyInByte==0x20) KeyOutputPortStatus=digitID+3;
	if (keyInByte==0x40) KeyOutputPortStatus=digitID+7;
	if (keyInByte==0x80) KeyOutputPortStatus=digitID+11;

	if (KeyOutputPortStatus!=0xff) keyDrvRawData=KeyOutputPortStatus;
	if (keyDrvRawData == 0xff) {
		keyDrvValidRawData = 0xff;
		keyDrvPressCount = 0;
	} else {
		// Key is pressed
		if (keyDrvRawData == keyDrvValidRawData) {
			keyDrvPressCount++;
			if (keyDrvPressCount==keyDrvValidCount) {
				if (KeyDrvEnableFlag)
				{ 
					RingBufPutCharForKeyInterrupt(&KeyDrvRingBuf, KeyDrvConvTable[keyDrvRawData]);
				}
//				keyDrvPressCount=0;
			}
		}
		else
		{
			keyDrvValidRawData = keyDrvRawData;
			keyDrvPressCount = 0;
		}
		keyDrvRawData = 0xff;
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
	if (keyMode != KEY_MODE_NORMAL) 
	{
		return;	//RB�� Normal Table�� ����
	}
	if (rawKey >= KEY_DRV_MAX_KEY_NUM) 
	{
		return;
	}
	KeyDrvFuncTable[0][rawKey] = funcKey;
	KeyDrvSaveFuncTable(UPDATE_KEY_TABLE);
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

	//funcKey = KeyDrvDefaultFuncTable[0][rawKey];
	funcKey = KeyDrvFuncTable[0][rawKey];//RB has only normal key table

	if (keyMode == KEY_MODE_HEX)
	{
		switch (rawKey)
		{
			case 28:
				funcKey = KEY_NUM_A;
				break;
			case 29:
				funcKey = KEY_NUM_D;
				break;
			case 34:
				funcKey = KEY_NUM_B;
				break;
			case 35:
				funcKey = KEY_NUM_E;
				break;
			case 40:
				funcKey = KEY_NUM_C;
				break;
			case 41:
				funcKey = KEY_NUM_F;
				break;
		}
	}

	//Long Key ó��
	if ((funcKey == KEY_SUM) && (keyDrvPressCount != 0) && !(RingBufCheckBuf(&KeyDrvRingBuf)))
	{
		if (keyDrvPressCount > KeyDrvLongValidCount)
		{
			funcKey = KEY_MODE;
		}
		else
		{
			funcKey = KEY_NOT_PRESSED;
			RingBufRewindChar(&KeyDrvRingBuf);
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
	return KeyDrvFuncTableForPgm[num];
}
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

#endif//#ifdef KEY_DRV_RB_LCD


