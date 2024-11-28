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
#ifdef KEY_DRV_AP_VFD

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
#define KEY_DRV_MAX_KEY_NUM	56
/** @brief key ring buffer size */
#define KEY_DRV_BUF_SIZE	5
/** @brief �Է� Pin ���� */
#define KEY_DRV_IN_PIN_NUM		4
/** @brief ��� Pin ���� */
#define KEY_DRV_OUT_PIN_NUM		14
/** @brief KeyDrvProc�� ����Ǵ� �ð� ����, ���� ms*/
#define KEY_DRV_PROC_TIME_INTERVAL	1
/** @brief Key ���� ��ȿ������ ������ �ð�, ���� ms*/
#define KEY_DRV_VALID_TIME	 9	//������: 40 (Key ���� �ӵ� ����)
/** @brief ��� ���� Key ���� �ٸ� Key�� ��ü�� ���� �ð�, ���� ms*/
#define KEY_DRV_LONG_VALID_TIME	300
/** @brief �Է� Pin ���� */
#define KEY_DRV_FIRST_OUTPUT	5
	
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
static idata char  keyDrvData[KEY_DRV_BUF_SIZE];
/** @brief Key�� Index */
static data INT8U  keyDrvIndex;//data for high speed in interrupt
/** @brief Key Interrupt Time check */
//static data INT16U  keyDrvCount;
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
static idata INT16U KeyDrvLongValidCount;
/** @brief power key �� ������ Ƚ�� (KeyDrvProc ���� ���ݿ� ����)*/
static data INT16U keyDrvPowerKeyPressCount;//data for high speed in interrupt
/** @brief power key ���� ���� */
static BOOLEAN keyDrvPowerPortCurrStatus;
/** @brief power key ���� ���� */
static BOOLEAN keyDrvPowerPortStatus;
/** @brief power key on flag */
static BOOLEAN keyDrvPowerKeyFlag;
/** @brief power off Enable flag (Power Off ���� ����)*/
static BOOLEAN powerOffEnFlag;

/** @brief convert raw code to human readable position code */


/** @brief raw key(Ű ��ġ ��ȣ)�� key function code�� ��ȯ�ϴ� table */
static KEY_TYPE idata KeyDrvFuncTable[1][KEY_DRV_MAX_KEY_NUM];

/** @brief raw key(Ű ��ġ ��ȣ)�� key function code�� ��ȯ�ϴ� table�� default ��, ���� Ű �е�*/
const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] = 
{
	{
		KEY_NUM_0,			KEY_NUM_1,			KEY_NUM_2,			KEY_NUM_3,	///3
		KEY_NUM_4,			KEY_NUM_5,			KEY_NUM_6,			KEY_NUM_7,	//7	
		KEY_NUM_8,			KEY_NUM_9,			KEY_C,			    KEY_NUM_00,		//11
		KEY_ON_OFF,			KEY_ZERO,			KEY_TARE,			KEY_NOT_EXISTED	,		//15
		KEY_ADD,			KEY_FIXED_ADD,		KEY_SUM,			KEY_PAY,	//19  18 IS TOTAL PRICE CALL

		KEY_PLU_SAVE,		KEY_PLU_CALL,		KEY_CANCEL,			KEY_MODE,	 //23
		KEY_NO_FUNC,		KEY_NOT_EXISTED	, 	KEY_NOT_EXISTED,	KEY_NOT_EXISTED	, //27  
		KEY_PLU_1,			KEY_PLU_8,			KEY_PLU_15,			KEY_PLU_22,	 //31
		KEY_PLU_2,      	KEY_PLU_9,			KEY_PLU_16,			KEY_PLU_23,  //35
		KEY_PLU_3,			KEY_PLU_10,			KEY_PLU_17,			KEY_PLU_24,  //39
		KEY_PLU_4,			KEY_PLU_11,			KEY_PLU_18,			KEY_PLU_25,	 //43
		KEY_PLU_5,			KEY_PLU_12,			KEY_PLU_19,			KEY_PLU_26,  //47
		KEY_PLU_6,			KEY_PLU_13,			KEY_PLU_20,			KEY_PLU_27,  //51
		KEY_PLU_7,			KEY_PLU_14,			KEY_PLU_21,			KEY_PLU_28,	 //55
	}
};
/// OM�� �ٸ� ���ݰ�� ������� �޸� 
/// AP���� KEY_SUM�� Ű�е� ���� ttp call key�̸� �Ѿ� ȣ���� ��ɸ� ������
/// KEY_PRINTŰ�� Ű�е� ���� ��(sum)Ű �̸� ���� �����Ǹų� DLP�μ�� �����

static const INT8U code KeyDrvConvTable[KEY_DRV_MAX_KEY_NUM] =
{
//	NUM0	NUM4	NUM8	POWER	ADD		MW		*	  M1	       M2	 M3    M4	 M5    M6    M7	  //AP Key Circuit
	0,	    4,      8,      12,     16,     20,     24,   28,   32,  36,   40,   44,   48,	 52,	

//	NUM1	NUM5	NUM9	ZERO	FAD	       MR		N_EX  M8  	M9	 M10   M11	 M12   M13	 M14
	1,      5,      9,      13,     17,     21,     25,   29,   33,  37,   41,   45,   49,   53, 

//	NUM2	NUM6	C	    TARE	      TTP	      CAN		N_EX  M15	M16	 M17   M18   M19   M20	 M21
	2,      6,      10,     14,     18,     22,     26,   30,   34,  38,   42,   46,   50,   54,
	
//	NUM3	NUM7	00      N_EX	      PAY	     MODE 	N_EX  M22   M23  M24   M25   M26   M27	 M28
	3,      7,      11,     15,	    19,     23,     27,   31,   35,  39,   43,   47,   51,   55
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
	KEY_TARE,	KEY_C,	KEY_ZERO,	KEY_CANCEL,	 KEY_NOT_EXISTED,	KEY_NOT_EXISTED, KEY_NOT_EXISTED, KEY_NOT_EXISTED, KEY_NOT_EXISTED
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
//    while (POWER_KEY_CHK == 0);	//wait Pwr key off

	KeyDrvLoadFuncTable();
	RingBufInit(&KeyDrvRingBuf, &keyDrvData[0], KEY_DRV_BUF_SIZE);
	KeyDrvEnable(ON);
	
//	keyDrvCount = 0;
	keyDrvRawData = 0xff;
	keyDrvClnData = 0xff;
	keyDrvValidRawData = 0xff;
	keyDrvPressCount = 0;

	keyDrvPowerKeyFlag = OFF;
	keyDrvPowerPortCurrStatus = OFF;
	keyDrvPowerPortStatus = OFF;
	keyDrvPowerKeyPressCount = 0;

	//keyDrvValidCount = KEY_DRV_VALID_TIME / (KEY_DRV_OUT_PIN_NUM * KEY_DRV_PROC_TIME_INTERVAL);
	//KeyDrvLongValidCount = KEY_DRV_LONG_VALID_TIME / (KEY_DRV_OUT_PIN_NUM * KEY_DRV_PROC_TIME_INTERVAL);

	keyDrvValidCount = KEY_DRV_VALID_TIME / (3 * KEY_DRV_PROC_TIME_INTERVAL);	//40/(3*1)=13.3ms (AP PLUS: 9/(3*1)=3ms)
	KeyDrvLongValidCount = KEY_DRV_LONG_VALID_TIME / (3 * KEY_DRV_PROC_TIME_INTERVAL);
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
	modelNum = ParamReadWord(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_MODEL);
	checkSum = ParamReadByte(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_CHECKSUM);
	ParamReadData(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_TABLE, (INT8U *)&KeyDrvFuncTable[0][0], KEY_DRV_MAX_KEY_NUM);

	//Default table���� Fixed Key�� Setting�Ǿ� ������ ������ Default�� load
	for (i = 0; i < KEY_DRV_MAX_KEY_NUM; i++)
	{
		for (j = 0; j < KEY_DRV_MAX_FIXED_KEY_NUM; j++)
		{
			if (KeyDrvDefaultFuncTable[0][i] == KeyDrvFixedFuncTable[j])
			{
				KeyDrvFuncTable[0][i] = KeyDrvDefaultFuncTable[0][i];
				break;
			}
		}
	}

	if (modelNum != MODEL_AP_VFD)
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
	if (keyDrvPowerKeyFlag && powerOffEnFlag)	//���� ��ư ���� �� �� ��� Power OFF (PowerOffEnFlag �߰� )
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
//Key scan out table
//const INT8U code keyOutScanTable[KEY_DRV_OUT_PIN_NUM] = { 0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F};
void  KeyDrvProc(void)
{  
	INT8U keyInByte;
	INT8U Byte;
	 
	if( KeyOutputPortStatus < KEY_DRV_FIRST_OUTPUT && KeyOutputPortStatus > (KEY_DRV_FIRST_OUTPUT + KEY_DRV_OUT_PIN_NUM - 1)) return;
	
	//keyDrvIndex = KeyOutputPortStatus;
	keyDrvIndex = KeyOutputPortStatus - KEY_DRV_FIRST_OUTPUT;
	
	keyInByte = KEY_IN_PORT & KEY_IN_MASK;

	if (keyDrvIndex == 10) 
	{
		if (keyInByte & 0x01 == 0x01) 
		{
			keyDrvPowerPortCurrStatus = ON;
			powerOffEnFlag = OFF;	//Power key ��� ���� ��� ���� ON -> OFF �Ǵ� ���� ��� ���� Flag
		} 
		else 
		{
			keyDrvPowerPortCurrStatus = OFF;
			powerOffEnFlag = ON;
		}
		keyInByte &= 0x0E;
	}

	switch (keyInByte) 
	{
		case 0x08: 
			//keyDrvClnData = 3; 
			keyDrvClnData = 3 * KEY_DRV_OUT_PIN_NUM;
			break;
		case 0x04: 
			//keyDrvClnData = 2; 
			keyDrvClnData = 2 * KEY_DRV_OUT_PIN_NUM;
			break;
		case 0x02: 
			//keyDrvClnData = 1; 
			keyDrvClnData = 1 * KEY_DRV_OUT_PIN_NUM;
			break;			
		case 0x01: 
			//keyDrvClnData = 0; 
			keyDrvClnData = 0;
			break;
		default : 
			//keyDrvClnData = 0xFF;
			break;
	}
	
	if ( keyInByte)
	{
		 keyDrvRawData =  keyDrvClnData + (13 - keyDrvIndex);
	}
/*	
	if(keyDrvClnData == 0xFF)
	{			
		keyDrvCount++;
		if(keyDrvCount == 500)
		{
			keyDrvCount = 0;
			keyDrvValidRawData = 0xFF;
		}
	}
	else
	{
	
		if(4 < keyDrvIndex < 19)
		{		
		if (keyDrvIndex == (KEY_DRV_OUT_PIN_NUM - 1))
		{
			 Byte = keyDrvIndex - KEY_DRV_FIRST_OUTPUT;
			 keyDrvRawData =  (keyDrvClnData * KEY_DRV_OUT_PIN_NUM) + (13 - Byte);
		}
		if (keyDrvRawData == 0xFF)
		{
			keyDrvValidRawData = 0xFF;
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
		*/
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
//						if (keyDrvRawData == 3) //KEY_ON_OFF
//						{
//							keyDrvPowerKeyFlag = ON;
//						}
//						else
//						{
							RingBufPutCharForKeyInterrupt(&KeyDrvRingBuf, KeyDrvConvTable[keyDrvRawData]);						
//						}
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
		if (keyDrvPowerPortCurrStatus)
		{
			if (keyDrvPowerPortStatus) 
			{
				keyDrvPowerKeyPressCount++;
				if (keyDrvPowerKeyPressCount == 5*keyDrvValidCount)	//������ ���� ���� Power key�� ��� ���� �ð� ���� (15count)
				{
					keyDrvPowerKeyFlag = ON;
							
					//���� power key�� �ٸ� �뵵�� ��� �� �Ʒ��� ���� ����Ѵ�.
					//KeyDrvGetFuncCode()���� keyDrvPowerKeyFlagó�� �Ѵ�.
					//��, Power key check�� KeyDrvGetFuncCode ȣ���Ͽ���������
					//power key check �Լ� ���� �ʿ���
					//	RingBufPutCharForKeyInterrupt(&KeyDrvRingBuf, 47);//KEY_POWER
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

	if (keyMode == KEY_MODE_HEX)
	{
		switch (rawKey)
		{
			case 32:					//PLU2 Law Key 
			case 16:					//ADD Law Key
				funcKey = KEY_NUM_A;
				break;
			case 33:					//PLU9 Law Key
			case 17:					//FAD Law Key
				funcKey = KEY_NUM_B;
				break;
			case 34:					//PLU16 Law Key 
			case 18:					//SUM Law Key
				funcKey = KEY_NUM_C;
				break;
			case 35:					//PLU23 Law Key
			case 19:					//PAY Law Key
				funcKey = KEY_NUM_D;
				break;
			case 36:					//PLU30 Law Key
			case 20:					//PLU CALL(MR) Law Key
				funcKey = KEY_NUM_E;
				break;
			case 37:					//PLU37 Law Key
			case 21:					//PLU SAVE(MW) Law Key
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
