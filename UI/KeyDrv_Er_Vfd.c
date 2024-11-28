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
#ifdef KEY_DRV_ER_VFD

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
#define KEY_DRV_MAX_KEY_NUM			25
/** @brief key ring buffer size */
#define KEY_DRV_BUF_SIZE				5
/** @brief �Է� Pin ���� */
#define KEY_DRV_IN_PIN_NUM	 			4
/** @brief ��� Pin ���� */
#define KEY_DRV_OUT_PIN_NUM			6
/** @brief KeyDrvProc�� ����Ǵ� �ð� ����, ���� ms*/
#define KEY_DRV_PROC_TIME_INTERVAL	1
//#define KEY_DRV_PROC_TIME_INTERVAL	3
/** @brief Key ���� ��ȿ������ ������ �ð�, ���� ms*/
//#define KEY_DRV_VALID_TIME				18
#define KEY_DRV_VALID_TIME				40
/** @brief ��� ���� Key ���� �ٸ� Key�� ��ü�� ���� �ð�, ���� ms*/
#define KEY_DRV_LONG_VALID_TIME		3000 
/** @brief �Է� Pin ���� */
#define KEY_DRV_FIRST_OUTPUT			12

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
static data BOOLEAN KeyDrvEnableFlag;
/** @brief key Driver Ring Buffer���� ����� Byte Array */
static idata char  keyDrvData[KEY_DRV_BUF_SIZE];
/** @brief Key�� Index */
static data INT8U  keyDrvIndex;//data for high speed in interrupt
/** @brief Key Interrupt Time check */
static data INT16U  keyDrvCount;
/** @brief ��ȯ table��ġ�� ���� raw(���ι迭) Data */
static data INT8U  keyDrvRawData; // Compiler ������ ���� data�� ������, �񱳹� ���� �߻�
/** @brief ��ȯ table��ġ�� ���� raw(���ι迭) Data */
static data INT8U  keyDrvClnData;
/** @brief ������ ������ raw Data */
static data INT8U  keyDrvValidRawData;//data for high speed in interrupt
/** @brief ������ raw data�� ������ Ƚ�� (KeyDrvProc ���� ���ݿ� ����)*/
static data INT16U keyDrvPressCount;//data for high speed in interrupt
/** @brief raw data�� ��ȿ������ ������ ���� Ƚ�� (KeyDrvProc ���� ���ݿ� ����)*/
static data INT16U keyDrvValidCount;//data for high speed in interrupt
/** @brief raw data�� ��� ���� Key������ ��ü�� ���� Ƚ�� (KeyDrvProc ���� ���ݿ� ����)*/
static data INT16U KeyDrvLongValidCount;
/** @brief power key �� ������ Ƚ�� (KeyDrvProc ���� ���ݿ� ����)*/
static data INT16U keyDrvPowerKeyPressCount;//data for high speed in interrupt
/** @brief power key ���� ���� */
static data BOOLEAN keyDrvPowerPortStatus;
/** @brief power key on flag */
static data BOOLEAN keyDrvPowerKeyFlag;
#ifdef	USE_DECIMAL_POINT	
/** @brief Decimal Point key on flag */
BOOLEAN DecimalPointKeyExistFlag; //csh 20120730
#endif

/** @brief convert raw code to human readable position code */


/** @brief raw key(Ű ��ġ ��ȣ)�� key function code�� ��ȯ�ϴ� table */
static KEY_TYPE idata KeyDrvFuncTable[1][KEY_DRV_MAX_KEY_NUM];
/** @brief raw key(Ű ��ġ ��ȣ)�� key function code�� ��ȯ�ϴ� table�� default ��, ���� Ű �е�*/
#ifdef USE_S2000JR_USA_KEY
const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] = 
{
	{
		KEY_PLU_1,		KEY_PLU_2,		KEY_PLU_3,		KEY_PLU_SAVE,	KEY_PLU_CALL,///4
		KEY_NUM_7,		KEY_NUM_8,		KEY_NUM_9,		KEY_BOTH_SAVE,	KEY_UNIT_CHANGE,//9
		KEY_NUM_4,		KEY_NUM_5,		KEY_NUM_6,		KEY_HALF,		KEY_QUARTER,//14
		KEY_NUM_1,		KEY_NUM_2,		KEY_NUM_3,		KEY_C,			KEY_SUM,//19
		KEY_NUM_0,		KEY_NUM_00, 	KEY_ZERO,		KEY_TARE,		KEY_ON_OFF//24
	}
};
#else
#ifdef USE_DIRECT_PLUKEY_TYPE_UN
//�������
const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] = 
{
	{
		KEY_PLU_1,	KEY_PLU_2,	KEY_PLU_3,	KEY_PLU_4,		KEY_PLU_5, ///4
		KEY_NUM_7,	KEY_NUM_8,	KEY_NUM_9,	KEY_PLU_SAVE,	KEY_PLU_CALL,//9
		KEY_NUM_4,	KEY_NUM_5,	KEY_NUM_6,	KEY_ADD,		KEY_CANCEL,//14
		KEY_NUM_1,	KEY_NUM_2,	KEY_NUM_3,	KEY_X,			KEY_SUM,//19
		KEY_NUM_0,	KEY_C,		KEY_ZERO,	KEY_TARE,		KEY_ON_OFF//24
	}
};

#elif defined (USE_DIRECT_PLUKEY_TYPE_M1_M7)
// ��Ű��� 
const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] = 
{
	{
		KEY_PLU_1, 	KEY_PLU_2, 	KEY_PLU_3, 	KEY_PLU_4, 		KEY_PLU_5, 	//4
		KEY_NUM_7,	KEY_NUM_8,	KEY_NUM_9,	KEY_PLU_6,		KEY_PLU_7,	//9
		KEY_NUM_4,	KEY_NUM_5,	KEY_NUM_6,	KEY_PLU_SAVE,	KEY_PLU_CALL,	//14
		KEY_NUM_1,	KEY_NUM_2,	KEY_NUM_3,	KEY_BAT_CHECK,	KEY_NOT_EXISTED,	//19
		KEY_NUM_0,	KEY_C,		KEY_ZERO,	KEY_TARE,		KEY_ON_OFF,	//24
	}
};

#elif defined (USE_DIRECT_PLUKEY_TYPE_M1_M9)

//Netherlands Key Array
const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] = 
{
	{
		KEY_PLU_1, 	KEY_PLU_2, 	KEY_PLU_3, 	KEY_PLU_4, 		KEY_PLU_5, 	//4
		KEY_NUM_7,	KEY_NUM_8,	KEY_NUM_9,	KEY_PLU_6,		KEY_PLU_7,	//9
		KEY_NUM_4,	KEY_NUM_5,	KEY_NUM_6,	KEY_PLU_8,		KEY_PLU_9,	//14
		KEY_NUM_1,	KEY_NUM_2,	KEY_NUM_3,	KEY_PLU_SAVE,	KEY_PLU_CALL,	//19
		KEY_NUM_0,	KEY_C,		KEY_ZERO,	KEY_TARE,		KEY_ON_OFF,	//24
	}
};
#endif //DIRECT PLUKEY TYPE

#endif
static const INT8U code KeyDrvConvTable[KEY_DRV_MAX_KEY_NUM] =
{
	//	NUM3	NUM7	X		CANCEL	M1		M2
		17, 		5,		18, 		14, 		0,		1,
	//	NUM2	NUM6	C		TARE	M4		M5
		16, 		12, 		21, 		23, 		3,		4,
	//	NUM1	NUM5	NUM9	ZERO	M3		CALL
		15, 		11, 		7,		22, 		2,		9,
	//	NUM0	NUM4	NUM8	TOTAL	ADD 	SAVE
		20, 		10, 		6,		19, 		13, 		8,
	//	POWER
		24
};

/** @brief Fixed key�� �ִ� ����*/
#define KEY_DRV_MAX_FIXED_KEY_NUM	14
/** @brief ����ڰ� ������ �� ���� Key���� �迭 */
const KEY_TYPE code KeyDrvFixedFuncTable[KEY_DRV_MAX_FIXED_KEY_NUM] =
{
	KEY_NUM_0, KEY_NUM_1, KEY_NUM_2, KEY_NUM_3, KEY_NUM_4,
	KEY_NUM_5, KEY_NUM_6, KEY_NUM_7, KEY_NUM_8, KEY_NUM_9,
	KEY_C, 	   KEY_ZERO,  KEY_TARE, KEY_ON_OFF
};

/** 
* @brief menu mode���� ����� key func code 
* @remark Enter, C, MenuUp, MenuDown, INCrease, ROTate ������ (see KEY_MENU_ENTER_NUM~) \n
*         ���� : ���� Key ��� �� �ݵ�� INC, ROT�� KEY_NOT_EXISTED���� �ؾ���
*/
const KEY_TYPE code KeyDrvFuncTableForPgm[KEY_MAX_KEY_NUM_FOR_PGM] = 
{
	KEY_TARE,	KEY_C,	KEY_ZERO,	KEY_CANCEL,	KEY_NOT_EXISTED,	KEY_NOT_EXISTED, KEY_NOT_EXISTED, KEY_NOT_EXISTED, KEY_NOT_EXISTED
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
	KeyDrvLoadFuncTable();
	RingBufInit(&KeyDrvRingBuf, &keyDrvData[0], KEY_DRV_BUF_SIZE);
	KeyDrvEnable(ON);

	keyDrvCount = 0;
	keyDrvRawData = 0xff;
	keyDrvClnData = 0xff;
	keyDrvValidRawData = 0xff;
	keyDrvPressCount = 0;

	keyDrvPowerKeyFlag = OFF;
	keyDrvPowerPortStatus = OFF;
	keyDrvPowerKeyPressCount = 0;

//	keyDrvValidCount = KEY_DRV_VALID_TIME / (KEY_DRV_OUT_PIN_NUM * KEY_DRV_PROC_TIME_INTERVAL);
//	KeyDrvLongValidCount = KEY_DRV_LONG_VALID_TIME / (KEY_DRV_OUT_PIN_NUM * KEY_DRV_PROC_TIME_INTERVAL);
	keyDrvValidCount = KEY_DRV_VALID_TIME / (18 * KEY_DRV_PROC_TIME_INTERVAL);
	KeyDrvLongValidCount = KEY_DRV_LONG_VALID_TIME / (18 * KEY_DRV_PROC_TIME_INTERVAL);

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
	INT16U keyModelNum;
	INT8U checkSum;
	INT8U sum;
	INT8U error;

	error = 0;
#ifdef	USE_DECIMAL_POINT		
	DecimalPointKeyExistFlag = OFF; //csh 20120823
#endif
	keyModelNum = ParamReadWord(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_MODEL);
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

	if (keyModelNum != MODEL_NUM)
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
* @param    INT8U mode
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
	INT8U keyInByte;

	if( KeyOutputPortStatus < KEY_DRV_FIRST_OUTPUT && KeyOutputPortStatus > (KEY_DRV_FIRST_OUTPUT + KEY_DRV_OUT_PIN_NUM - 1)) return;

	keyDrvIndex = KeyOutputPortStatus - KEY_DRV_FIRST_OUTPUT;

	keyInByte = KEY_IN_PORT & KEY_IN_MASK;

	switch (keyInByte) 
	{
		case 0x08: 
			keyDrvClnData = 0; 
			break;
		case 0x04: 
			keyDrvClnData = KEY_DRV_OUT_PIN_NUM; 
			break;
		case 0x02: 
			keyDrvClnData = 2 * KEY_DRV_OUT_PIN_NUM; 
			break;			
		case 0x01: 
			keyDrvClnData = 3 * KEY_DRV_OUT_PIN_NUM; 
			break;
	}

	if ( keyInByte)
	{
		 keyDrvRawData =  keyDrvClnData + (5 - keyDrvIndex);
	}

	if (keyDrvIndex == (KEY_DRV_OUT_PIN_NUM - 1))
	{
		if (keyDrvRawData == 0xff)
		{
			keyDrvValidRawData = 0xff;
			keyDrvPressCount = 0;
		}
		else
		{
			//Key is pressed
			if (keyDrvRawData == keyDrvValidRawData)
			{
				keyDrvPressCount++;
				if (keyDrvPressCount == keyDrvValidCount)
				{
					if (KeyDrvEnableFlag)
					{
						RingBufPutCharForKeyInterrupt(&KeyDrvRingBuf, KeyDrvConvTable[keyDrvRawData]);
					}
				}
			}
			else
			{
				keyDrvValidRawData = keyDrvRawData;
				keyDrvPressCount = 0;
			}
		}
		keyDrvRawData = 0xff;
		
		//check power key
		if (POWER_KEY_CHK == 0)
		{
			if (keyDrvPowerPortStatus) 
			{
				keyDrvPowerKeyPressCount++;
				if (keyDrvPowerKeyPressCount == keyDrvValidCount)
				{
					keyDrvPowerKeyFlag = ON;
					//���� power key�� �ٸ� �뵵�� ��� �� �Ʒ��� ���� ����Ѵ�.
					//KeyDrvGetFuncCode()���� keyDrvPowerKeyFlagó�� �Ѵ�.
					//��, Power key check�� KeyDrvGetFuncCode ȣ���Ͽ���������
					//power key check �Լ� ���� �ʿ���
					//RingBufPutCharForKeyInterrupt(&KeyDrvRingBuf, 47);//KEY_ON_OFF
				}
			}
			else
			{
				keyDrvPowerPortStatus = ON;
				keyDrvPowerKeyPressCount = 0;
			}
		}
		else
		{
			keyDrvPowerPortStatus = OFF;
			keyDrvPowerKeyPressCount = 0;
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
#if MODEL_NUM == MODEL_S2000JR_VFD
	if (keyMode == KEY_MODE_HEX)
	{
		switch (rawKey)
		{
			case 3:
				funcKey = KEY_NUM_A;
				break;
			case 4:
				funcKey = KEY_NUM_D;
				break;
			case 8:
				funcKey = KEY_NUM_B;
				break;
			case 9:
				funcKey = KEY_NUM_E;
				break;
			case 13:
				funcKey = KEY_NUM_C;
				break;
			case 14:
				funcKey = KEY_NUM_F;
				break;
		}
	}
#else
	if (keyMode == KEY_MODE_HEX)
	{
		switch (rawKey)
		{
			case 8:
				funcKey = KEY_NUM_A;
				break;
			case 9:
				funcKey = KEY_NUM_D;
				break;
			case 13:
				funcKey = KEY_NUM_B;
				break;
			case 14:
				funcKey = KEY_NUM_E;
				break;
			case 18:
				funcKey = KEY_NUM_C;
				break;
			case 19:
				funcKey = KEY_NUM_F;
				break;
		}
	}
#endif
	//Long Key ó��
	//if ((funcKey == KEY_SUM) && (keyDrvPressCount != 0) && !(RingBufCheckBuf(&KeyDrvRingBuf)))	
	if (((funcKey == KEY_SUM) || (funcKey == KEY_PLU_CALL)) && (keyDrvPressCount != 0) && !(RingBufCheckBuf(&KeyDrvRingBuf)))
	{
		if (keyDrvPressCount > KeyDrvLongValidCount)
		{
			//funcKey = KEY_MODE;			
			switch(funcKey)
			{
				case KEY_SUM:
					funcKey = KEY_MODE;
					break;
				case KEY_PLU_CALL:
					funcKey = KEY_BAT_CHECK;
					break;
			}
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
	INT8U i;

	for (i = 0; i < KEY_DRV_MAX_KEY_NUM; i++)
	{
		if (KeyDrvFuncTableForPgm[num] != KEY_NOT_EXISTED)
		{
			if (KeyDrvDefaultFuncTable[0][i] == KeyDrvFuncTableForPgm[num])//find default key
			{
				return KeyDrvFuncTable[0][i]; //return current key
			}
		}
	}
	return KEY_NOT_EXISTED;
}
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

#endif//#ifdef KEY_DRV_ER_VFD
