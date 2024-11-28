/**
********************************************************************************
* Copyright (c) 2012 CAS
* @brief   Key Driver Dource File\n
           for PR PLUS LED
* @file    KeyDrv_Pr_Plus_Led.c
* @version 1.0
* @date    2012/09/24
* @author  Young Hoon Lee (LYH)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2012/09/24 created by LYH \n
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
#ifdef KEY_DRV_PR_PLUS_LED
#ifdef	USE_USER_SETTING_FOR_DOUBLEZERO_BATTLED
#include "../UI/DispApi.h"
#endif

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
#define KEY_DRV_MAX_KEY_NUM	20
/** @brief key ring buffer size */
#define KEY_DRV_BUF_SIZE	5
/** @brief �Է� Pin ���� */
#define KEY_DRV_IN_PIN_NUM	 4
/** @brief ��� Pin ���� */
#define KEY_DRV_OUT_PIN_NUM	5
/** @brief KeyDrvProc�� ����Ǵ� �ð� ����, ���� ms*/
#define KEY_DRV_PROC_TIME_INTERVAL	1
/** @brief Key ���� ��ȿ������ ������ �ð�, ���� ms*/
#define KEY_DRV_VALID_TIME	40
/** @brief ��� ���� Key ���� �ٸ� Key�� ��ü�� ���� �ð�, ���� ms*/
#define KEY_DRV_LONG_VALID_TIME	3000 
/** @brief ��� ���� Power Key �ν� ���� �ð�, ���� ms*/
#define KEY_DRV_POWER_KEY_VALID_TIME	1000

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
static data INT8U  keyDrvIndex;//data for high speed in interrupt
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
static data INT16U keyDrvPowerKeyPressCount;//data for high speed in interrupt
/** @brief power key ���� ���� */
static BOOLEAN keyDrvPowerPortStatus;
/** @brief power key on flag */
static BOOLEAN keyDrvPowerKeyFlag;
#ifdef	USE_DECIMAL_POINT	
/** @brief Decimal Point key on flag */
BOOLEAN DecimalPointKeyExistFlag; //csh 20120727
#endif
#ifdef USE_QUICK_STORE_FUNCTION
/** @brief Quick Save key ���ۻ��� flag */
static BOOLEAN keyDrvQuickSaveKeyFlag;
#endif

/** @brief convert raw code to human readable position code */

//PR PLUS 20key pad
static const INT8U code KeyDrvConvTable[KEY_DRV_MAX_KEY_NUM] =
{
	0, 		11,		6,		1,	 	16,
//	PLU1,  	1,		4,		7,		0
	5, 		12,		7,		2, 		9,
//	PLU2,	2, 		5,		8,		DECIMAL,
	10,		13,		8,		3, 		17,
//	PLU3,	3,		6, 		9,   		C,
	15, 	18,		14,		4, 		19,		
//	PLU4,	ZERO, 	TARE,	PLUSAVE,
};

/** @brief raw key(Ű ��ġ ��ȣ)�� key function code�� ��ȯ�ϴ� table */
static KEY_TYPE KeyDrvFuncTable[1][KEY_DRV_MAX_KEY_NUM];

/** @brief raw key(Ű ��ġ ��ȣ)�� key function code�� ��ȯ�ϴ� table�� default �� */
#ifdef USE_DECIMAL_POINT
 #ifdef USE_UNIT_CHANGE_FUNCTION
const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] = 
{
	{
//		KEY_PLU_1,		KEY_NUM_7,		KEY_NUM_8,			KEY_NUM_9,			KEY_X,		///4
		KEY_PLU_1,		KEY_NUM_7,		KEY_NUM_8,			KEY_NUM_9,			KEY_PLU_SAVE,///4
		KEY_PLU_2,		KEY_NUM_4,		KEY_NUM_5,			KEY_NUM_6,			KEY_POINT,//9
		KEY_PLU_3,		KEY_NUM_1,		KEY_NUM_2,			KEY_NUM_3,			KEY_TARE,	//14
		KEY_UNIT_CHANGE,		KEY_NUM_0,		KEY_C,				KEY_ZERO,			KEY_ON_OFF, //19
	}
};
 #else
const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] = 
{
	{
//		KEY_PLU_1,		KEY_NUM_7,		KEY_NUM_8,			KEY_NUM_9,			KEY_X,		///4
		KEY_PLU_1,		KEY_NUM_7,		KEY_NUM_8,			KEY_NUM_9,			KEY_PLU_SAVE,///4
		KEY_PLU_2,		KEY_NUM_4,		KEY_NUM_5,			KEY_NUM_6,			KEY_POINT,//9
		KEY_PLU_3,		KEY_NUM_1,		KEY_NUM_2,			KEY_NUM_3,			KEY_TARE,	//14
		KEY_PLU_4,		KEY_NUM_0,		KEY_C,				KEY_ZERO,			KEY_ON_OFF, //19
	}
};
 #endif	//USE_UNIT_CHANGE_FUNCTION
/** @brief Fixed key�� �ִ� ����*/
#define KEY_DRV_MAX_FIXED_KEY_NUM	16
/** @brief ����ڰ� ������ �� ���� Key���� �迭 */
const KEY_TYPE code KeyDrvFixedFuncTable[KEY_DRV_MAX_FIXED_KEY_NUM] =
{
	KEY_NUM_0,	KEY_NUM_1,	KEY_NUM_2,	KEY_NUM_3,	KEY_NUM_4,
	KEY_NUM_5,	KEY_NUM_6,	KEY_NUM_7,	KEY_NUM_8,	KEY_NUM_9,
	KEY_C,		KEY_ZERO,	KEY_TARE,	KEY_ON_OFF,	KEY_PLU_SAVE,
	KEY_POINT
};

#else
 #ifdef USE_UNIT_CHANGE_FUNCTION
const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] = 
{
	{
//		KEY_PLU_1,		KEY_NUM_7,		KEY_NUM_8,			KEY_NUM_9,			KEY_X,		///4
		KEY_PLU_1,		KEY_NUM_7,		KEY_NUM_8,			KEY_NUM_9,			KEY_PLU_SAVE,///4
		KEY_PLU_2,		KEY_NUM_4,		KEY_NUM_5,			KEY_NUM_6,			KEY_POINT,//9
		KEY_PLU_3,		KEY_NUM_1,		KEY_NUM_2,			KEY_NUM_3,			KEY_TARE,	//14
		KEY_UNIT_CHANGE,		KEY_NUM_0,		KEY_C,				KEY_ZERO,			KEY_ON_OFF, //19
	}
};
 #else
const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] = 
{
	{
//		KEY_PLU_1,		KEY_NUM_7,		KEY_NUM_8,			KEY_NUM_9,			KEY_X,		///4
		KEY_PLU_1,		KEY_NUM_7,		KEY_NUM_8,			KEY_NUM_9,			KEY_PLU_SAVE,///4
		KEY_PLU_2,		KEY_NUM_4,		KEY_NUM_5,			KEY_NUM_6,			KEY_NUM_00,//9
		KEY_PLU_3,		KEY_NUM_1,		KEY_NUM_2,			KEY_NUM_3,			KEY_TARE,	//14
		KEY_PLU_4,		KEY_NUM_0,		KEY_C,				KEY_ZERO,			KEY_ON_OFF, //19
	}
};
 #endif //USE_UNIT_CHANGE_FUNCTION
/** @brief Fixed key�� �ִ� ����*/
#define KEY_DRV_MAX_FIXED_KEY_NUM	16
/** @brief ����ڰ� ������ �� ���� Key���� �迭 */
const KEY_TYPE code KeyDrvFixedFuncTable[KEY_DRV_MAX_FIXED_KEY_NUM] =
{
	KEY_NUM_0,	KEY_NUM_1,	KEY_NUM_2,	KEY_NUM_3,	KEY_NUM_4,
	KEY_NUM_5,	KEY_NUM_6,	KEY_NUM_7,	KEY_NUM_8,	KEY_NUM_9,
	KEY_C,		KEY_ZERO,	KEY_TARE,	KEY_ON_OFF,	KEY_PLU_SAVE,
	KEY_NUM_00
};

#endif	//USE_DECIMAL_POINT
/**
* @brief menu mode���� ����� key func code 
* @remark Enter, C, MenuUp, MenuDown, INCrease, ROTate ������ (see KEY_MENU_ENTER_NUM~) \n
*         ���� : ���� Key ��� �� �ݵ�� INC, ROT�� KEY_NOT_EXISTED���� �ؾ���
*/
const KEY_TYPE code KeyDrvFuncTableForPgm[KEY_MAX_KEY_NUM_FOR_PGM] = 
{
	KEY_TARE,	KEY_C,	KEY_ZERO,	KEY_PLU_SAVE,	KEY_NOT_EXISTED,	KEY_NOT_EXISTED, KEY_NOT_EXISTED, KEY_NOT_EXISTED, KEY_NOT_EXISTED
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

	keyDrvIndex = 0;
	keyDrvRawData = 0xff;
	keyDrvValidRawData = 0xff;
	keyDrvPressCount = 0;

	keyDrvPowerKeyFlag = OFF;
	keyDrvPowerPortStatus = OFF;
	keyDrvPowerKeyPressCount = 0;

	keyDrvValidCount = KEY_DRV_VALID_TIME / (KEY_DRV_OUT_PIN_NUM * KEY_DRV_PROC_TIME_INTERVAL);
	KeyDrvLongValidCount = KEY_DRV_POWER_KEY_VALID_TIME / (KEY_DRV_OUT_PIN_NUM * KEY_DRV_PROC_TIME_INTERVAL);

#ifdef USE_QUICK_STORE_FUNCTION
	keyDrvQuickSaveKeyFlag = OFF;
#endif

	GPIO_Init(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4, GPIO_Mode_Out_PP_High_Slow);
	GPIO_Init(GPIOD, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 , GPIO_Mode_In_PU_No_IT);	
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
#ifdef	USE_DECIMAL_POINT		
#ifdef	USE_USER_SETTING_FOR_DOUBLEZERO_BATTLED
	INT8U usedoublezero;
	usedoublezero = ParamReadByte(PARAM_DISP_AREA_NUM, DISP_PARAM_OFFSET_BLINK_COUNT);
#endif
	DecimalPointKeyExistFlag = OFF; //csh 20120823
#endif
	error = 0;

	keyModelNum = ParamReadWord(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_MODEL);
	checkSum = ParamReadByte(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_CHECKSUM);
	ParamReadData(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_TABLE, (INT8U *)&KeyDrvFuncTable[0][0], KEY_DRV_MAX_KEY_NUM);

	//Default table���� Fixed Key�� Setting�Ǿ� ������ ������ Default�� load
	for (i = 0; i < KEY_DRV_MAX_KEY_NUM; i++)
	{
#ifdef	USE_DECIMAL_POINT	
#ifdef	USE_USER_SETTING_FOR_DOUBLEZERO_BATTLED
	if(usedoublezero & 0x01); // Use double zero[00] key
	else // Use [Decimal Point] key : default
	{
		if (KeyDrvFuncTable[0][i] == KEY_POINT) //csh 20120823
		{
			DecimalPointKeyExistFlag = ON;
		}
	}
#else
	if (KeyDrvFuncTable[0][i] == KEY_POINT) //csh 20120823
	{
		DecimalPointKeyExistFlag = ON;
	}
#endif
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
*            ��) 4x3 key�� ���\n
*               0  1  2  3\n
*               4  5  6  7\n
*               8  9  10 11\n
*           ��) 6x1 key�� ���\n
*               0  1  2  3  4  5  6
********************************************************************************
*/
//Key scan out table
const INT8U code keyOutScanTable[KEY_DRV_OUT_PIN_NUM] = { 0xFE, 0xFD, 0xFB, 0xF7, 0xEF};
char KeyProccount=0;
char KeyProcdata=0;

void  KeyDrvProc(void)
{  
	INT8U keyInByte;

	keyInByte = 0;
	keyInByte = KEY_IN_PORT;
	keyInByte = keyInByte & KEY_IN_MASK;	
	keyInByte = keyInByte ^ KEY_IN_MASK;

	switch (keyInByte)
	{
		case 0x01:
			keyDrvRawData = keyDrvIndex;
			break;
		case 0x02:
			keyDrvRawData = (1 * KEY_DRV_OUT_PIN_NUM) + keyDrvIndex;
			break;
		case 0x04:
			keyDrvRawData = (2 * KEY_DRV_OUT_PIN_NUM) + keyDrvIndex;
			break;
		case 0x08:
			keyDrvRawData = (3 * KEY_DRV_OUT_PIN_NUM) + keyDrvIndex;
			break;
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
#ifdef USE_QUICK_STORE_FUNCTION
				keyDrvQuickSaveKeyFlag =  OFF;
#endif
				
			}
		}
		keyDrvRawData = 0xff;

		//check power key
		if (POWER_KEY_CHK != 0)
		{
			if (keyDrvPowerPortStatus) 
			{
				keyDrvPowerKeyPressCount++;
				if (keyDrvPowerKeyPressCount > (KEY_DRV_POWER_KEY_VALID_TIME / (KEY_DRV_OUT_PIN_NUM * KEY_DRV_PROC_TIME_INTERVAL)))					
				{
					keyDrvPowerKeyFlag = ON;
					//���� power key�� �ٸ� �뵵�� ��� �� �Ʒ��� ���� ����Ѵ�.
					//KeyDrvGetFuncCode()���� keyDrvPowerKeyFlagó�� �Ѵ�.
					//��, Power key check�� KeyDrvGetFuncCode ȣ���Ͽ���������
					//power key check �Լ� ���� �ʿ���
					RingBufPutCharForKeyInterrupt(&KeyDrvRingBuf, 19);//KEY_ON_OFF raw code
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
	
	keyDrvIndex++;
	keyDrvIndex = keyDrvIndex % KEY_DRV_OUT_PIN_NUM;
	KEY_OUT_PORT = keyOutScanTable[keyDrvIndex];
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
#ifdef USE_PRINTER_FOR_CHINA	
	if(funcKey == KEY_PRINT || funcKey == KEY_PLU_4)
	{
		KeyDrvFuncTable[0][rawKey] = funcKey;
		KeyDrvSaveFuncTable(UPDATE_KEY_TABLE);	
	}
#endif
#ifdef UNLOCK_ADD_SUM_PAY_FUNCTION	
	if(funcKey == KEY_ADD || funcKey == KEY_SUM || funcKey == KEY_PAY ||
		funcKey == KEY_PLU_1 || funcKey == KEY_PLU_2 || funcKey == KEY_PLU_3 || funcKey == KEY_PLU_4 )
	{
		KeyDrvFuncTable[0][rawKey] = funcKey;
		KeyDrvSaveFuncTable(UPDATE_KEY_TABLE);	
	}
#endif
#ifdef USE_UNIT_CHANGE_FUNCTION
	if(funcKey == KEY_UNIT_CHANGE)
	{
		KeyDrvFuncTable[0][rawKey] = funcKey;
		KeyDrvSaveFuncTable(UPDATE_KEY_TABLE);	
	}
#endif
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

	funcKey = KeyDrvFuncTable[0][rawKey];
	if (keyMode == KEY_MODE_HEX)
	{
		switch (rawKey)
		{
			case 0:
				funcKey = KEY_NUM_A;
				break;
			case 15:
				funcKey = KEY_NUM_D;
				break;
			case 5:
				funcKey = KEY_NUM_B;
				break;
			case 4:
				funcKey = KEY_NUM_E;
				break;
			case 10:
				funcKey = KEY_NUM_C;
				break;
			case 9:
				funcKey = KEY_NUM_F;
				break;
		}
	}
	//PLU SAVE Key Long Key ó��
	if ( funcKey != KEY_ZERO && funcKey != KEY_TARE &&(keyDrvPressCount != 0) && !(RingBufCheckBuf(&KeyDrvRingBuf)))		
	{
		if (keyDrvPressCount > KeyDrvLongValidCount)
		{
			switch(funcKey)
			{
				case KEY_PLU_SAVE:
					funcKey = KEY_BAT_CHECK;
					break;
#ifdef USE_QUICK_STORE_FUNCTION					
				case KEY_PLU_1:	
				case KEY_PLU_2:	
				case KEY_PLU_3:	
				case KEY_PLU_4:						
					if(keyDrvQuickSaveKeyFlag == OFF)
					{
						RingBufPutCharForKeyInterrupt(&KeyDrvRingBuf, rawKey);
						funcKey = KEY_PLU_DIRECT_SAVE;
						keyDrvQuickSaveKeyFlag = ON;
					}
					break;
#endif
#ifdef USE_LONG_KEY_FOR_CHINAMARKET
				case KEY_C:
					funcKey = KEY_SET_PROTOCOL;
					break;
				case KEY_POINT:
				case KEY_NUM_00:					
					funcKey = KEY_SEND_PROTOCOL;
					break;
#endif					
				default:
					break;
			}
		}
		else
		{
			funcKey = KEY_NOT_PRESSED;
			RingBufRewindChar(&KeyDrvRingBuf);
		}
	}
/*
	
	if ((funcKey == KEY_PLU_SAVE) && (keyDrvPressCount != 0) && !(RingBufCheckBuf(&KeyDrvRingBuf)))		
	{
		if (keyDrvPressCount > KeyDrvLongValidCount)
		{
			funcKey = KEY_BAT_CHECK;
		}
		else
		{
			funcKey = KEY_NOT_PRESSED;
			RingBufRewindChar(&KeyDrvRingBuf);
		}
	}
#ifdef USE_QUICK_STORE_FUNCTION
		if ( ((funcKey >= KEY_PLU_1) && (funcKey <= KEY_PLU_4)) && (keyDrvPressCount != 0) && !(RingBufCheckBuf(&KeyDrvRingBuf)))
		{
			if (keyDrvPressCount > KeyDrvLongValidCount)
			{
				switch(funcKey)
				{
					default:
						if((funcKey >= KEY_PLU_1) && (funcKey <= KEY_PLU_29) && (keyDrvQuickSaveKeyFlag == OFF))
						{
							RingBufPutCharForKeyInterrupt(&KeyDrvRingBuf, rawKey);
							funcKey = KEY_PLU_DIRECT_SAVE;
							keyDrvQuickSaveKeyFlag = ON;
						}
				}
			}
			else
			{
				funcKey = KEY_NOT_PRESSED;
				RingBufRewindChar(&KeyDrvRingBuf);
			}
		}
#endif
#ifdef USE_LONG_KEY_FOR_CHINAMARKET
			if ((funcKey == KEY_TARE) || (funcKey == KEY_POINT) || (funcKey == KEY_NUM_00) && (keyDrvPressCount != 0) && !(RingBufCheckBuf(&KeyDrvRingBuf)))
			{
				if (keyDrvPressCount > KeyDrvLongValidCount)
				{
					switch(funcKey)
					{
						case KEY_TARE: // setting send type 0 : no send 1: continuous send, 2: stable and send protocol.
							funcKey = KEY_SET_PROTOCOL;
							break;
						case KEY_POINT: // Start /Stop Sending Protocol
						case KEY_NUM_00:						
							funcKey = KEY_SEND_PROTOCOL;
							break;
					}
				}
				else
				{
					funcKey = KEY_NOT_PRESSED;
					RingBufRewindChar(&KeyDrvRingBuf);
				}
			}
#endif
*/
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

#endif//#ifdef KEY_DRV_PR_PLUS_LED

