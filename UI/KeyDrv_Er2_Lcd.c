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
//#include "../Application/SaleModeApp.h"

#include "../Hardware/HardwareConfig.h"
#ifdef KEY_DRV_ER2_LCD

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
#define KEY_DRV_MAX_KEY_NUM	20
/** @brief key ring buffer size */
#define KEY_DRV_BUF_SIZE	5
/** @brief 입력 Pin 개수 */
#define KEY_DRV_IN_PIN_NUM	 4
/** @brief 출력 Pin 개수 */
#define KEY_DRV_OUT_PIN_NUM	5
/** @brief KeyDrvProc가 실행되는 시간 간격, 단위 ms*/
#define KEY_DRV_PROC_TIME_INTERVAL	1
/** @brief Key 값을 유효값으로 인정할 시간, 단위 ms*/
#define KEY_DRV_VALID_TIME	40
/** @brief 길게 누른 Key 값을 다른 Key로 대체할 기준 시간, 단위 ms*/
#define KEY_DRV_LONG_VALID_TIME	3000 
/** @brief 길게 누른 Power Key 인식 기준 시간, 단위 ms*/
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
/** @brief key Driver Ring Buffer에서 사용할 Byte Array */
static char  keyDrvData[KEY_DRV_BUF_SIZE];
/** @brief Key열 Index */
static data INT8U  keyDrvIndex;//data for high speed in interrupt
/** @brief 변환 table거치지 않은 raw Data */
static data INT8U  keyDrvRawData; // Compiler 오류로 인해 data로 선언함, 비교문 오류 발생
/** @brief 이전에 눌려진 raw Data */
static data INT8U  keyDrvValidRawData;//data for high speed in interrupt
/** @brief 동일한 raw data가 눌려진 횟수 (KeyDrvProc 실행 간격에 의존)*/
static data INT16U keyDrvPressCount;//data for high speed in interrupt
/** @brief raw data가 유효값으로 인정할 기준 횟수 (KeyDrvProc 실행 간격에 의존)*/
static data INT16U keyDrvValidCount;//data for high speed in interrupt
/** @brief raw data가 길게 누른 Key값으로 대체될 기준 횟수 (KeyDrvProc 실행 간격에 의존)*/
static INT16U KeyDrvLongValidCount;

/** @brief power key 가 눌려진 횟수 (KeyDrvProc 실행 간격에 의존)*/
static data INT16U keyDrvPowerKeyPressCount;//data for high speed in interrupt
/** @brief power key 이전 상태 */
static BOOLEAN keyDrvPowerPortStatus;
/** @brief power key on flag */
static BOOLEAN keyDrvPowerKeyFlag;
#ifdef	USE_DECIMAL_POINT	
/** @brief Decimal Point key on flag */
BOOLEAN DecimalPointKeyExistFlag; // csh 20120731
#endif			
#ifdef USE_QUICK_STORE_FUNCTION
/** @brief Quick Save key 동작상태 flag */
static BOOLEAN keyDrvQuickSaveKeyFlag;
#endif
//
/** @brief convert raw code to human readable position code */
/*static const INT8U code KeyDrvConvTable[KEY_DRV_MAX_KEY_NUM] =
{
	13, 18, 17, 16, 8,
	7, 6, 3, 2, 1,
	12, 11, 15, 5, 0,
	4, 9, 10, 14, 19
};*/
static const INT8U code KeyDrvConvTable[KEY_DRV_MAX_KEY_NUM] =
{
	16,  11,  12,  13, 6,
//    0,      1,  2,    3,  4
	7,    8,   1,    2,   3,
//    5,    6,   7,   8,   9
	0,    5,   10,  15,  19,
//   M1,  M2, M3, M4,   not exist(POWER)
	17,  18,  14,  9,  4,		
//   C,   Zero, Tare, Add, X
};

/** @brief raw key(키 위치 번호)를 key function code로 변환하는 table */
static KEY_TYPE KeyDrvFuncTable[1][KEY_DRV_MAX_KEY_NUM];

/** @brief raw key(키 위치 번호)를 key function code로 변환하는 table의 default 값 */
#if defined(USE_KEY_USA)
const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] = 
{
	{
//		KEY_PLU_1,		KEY_NUM_7,		KEY_NUM_8,			KEY_NUM_9,			KEY_X,		///4
		KEY_PLU_1,		KEY_NUM_7,		KEY_NUM_8,			KEY_NUM_9,			KEY_SUM,	///4
		KEY_PLU_2,		KEY_NUM_4,		KEY_NUM_5,			KEY_NUM_6,			KEY_UNIT_CHANGE,	//9
		KEY_PLU_3,		KEY_NUM_1,		KEY_NUM_2,			KEY_NUM_3,			KEY_TARE,	//14
		KEY_BOTH_SAVE,	KEY_NUM_0,		KEY_C,				KEY_ZERO,			KEY_ON_OFF,	//19
	}
};
#elif defined(USE_KEY_TURKEY)
const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] = 
{
	{
//		KEY_PLU_1,		KEY_NUM_7,		KEY_NUM_8,			KEY_NUM_9,			KEY_X,		///4
		KEY_PLU_1,		KEY_NUM_7,		KEY_NUM_8,			KEY_NUM_9,			KEY_BAT_CHECK,	///4
		KEY_PLU_2,		KEY_NUM_4,		KEY_NUM_5,			KEY_NUM_6,			KEY_BACK_LIGHT,	//9
		KEY_PLU_3,		KEY_NUM_1,		KEY_NUM_2,			KEY_NUM_3,			KEY_TARE,	//14
		KEY_SUM,		KEY_NUM_0,		KEY_C,				KEY_ZERO,			KEY_ON_OFF,	//19
	}
};

#elif defined(USE_ER2_TURKEY_COMP_KEY)
const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] = 
{
	{
//		KEY_PLU_1,		KEY_NUM_7,		KEY_NUM_8,			KEY_NUM_9,			KEY_X,		///4
		KEY_PLU_1,		KEY_NUM_7,		KEY_NUM_8,			KEY_NUM_9,			KEY_BAT_CHECK,	///4
		KEY_PLU_2,		KEY_NUM_4,		KEY_NUM_5,			KEY_NUM_6,			KEY_MODE,	//9
		KEY_PLU_3,		KEY_NUM_1,		KEY_NUM_2,			KEY_NUM_3,			KEY_TARE,	//14
		KEY_PRINT,		KEY_NUM_0,		KEY_C,				KEY_ZERO,			KEY_ON_OFF,	//19
	}
};

#elif defined(USE_KEY_RUSSIA)
const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] = 
{
	{
//		KEY_PLU_1,		KEY_NUM_7,		KEY_NUM_8,			KEY_NUM_9,			KEY_X,		///4
		KEY_PLU_1,		KEY_NUM_7,		KEY_NUM_8,			KEY_NUM_9,			KEY_SUM,	///4
		KEY_PLU_2,		KEY_NUM_4,		KEY_NUM_5,			KEY_NUM_6,			KEY_ADD,	//9
		KEY_PLU_3,		KEY_NUM_1,		KEY_NUM_2,			KEY_NUM_3,			KEY_TARE,	//14
		KEY_POINT,		KEY_NUM_0,		KEY_C,				KEY_ZERO,			KEY_ON_OFF,	//19
	}
};

#else
const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] = 
{
	{
//		KEY_PLU_1,		KEY_NUM_7,		KEY_NUM_8,			KEY_NUM_9,			KEY_X,		///4
		KEY_PLU_1,		KEY_NUM_7,		KEY_NUM_8,			KEY_NUM_9,			KEY_SUM,	///4
		KEY_PLU_2,		KEY_NUM_4,		KEY_NUM_5,			KEY_NUM_6,			KEY_ADD,	//9
		KEY_PLU_3,		KEY_NUM_1,		KEY_NUM_2,			KEY_NUM_3,			KEY_TARE,	//14
		KEY_PLU_4,		KEY_NUM_0,		KEY_C,				KEY_ZERO,			KEY_ON_OFF,	//19
	}
};
#endif
/** @brief Fixed key의 최대 개수*/
#define KEY_DRV_MAX_FIXED_KEY_NUM	14
/** @brief 사용자가 변경할 수 없는 Key들의 배열 */
const KEY_TYPE code KeyDrvFixedFuncTable[KEY_DRV_MAX_FIXED_KEY_NUM] =
{
	KEY_NUM_0, KEY_NUM_1, KEY_NUM_2, KEY_NUM_3, KEY_NUM_4,
	KEY_NUM_5, KEY_NUM_6, KEY_NUM_7, KEY_NUM_8, KEY_NUM_9,
	KEY_C, KEY_ZERO, KEY_TARE, KEY_ON_OFF
};

/**
* @brief menu mode에서 사용할 key func code 
* @remark Enter, C, MenuUp, MenuDown, INCrease, ROTate 순서임 (see KEY_MENU_ENTER_NUM~) \n
*         주의 : 숫자 Key 사용 시 반드시 INC, ROT는 KEY_NOT_EXISTED으로 해야함
*/
const KEY_TYPE code KeyDrvFuncTableForPgm[KEY_MAX_KEY_NUM_FOR_PGM] = 
{
	KEY_TARE,	KEY_C,	KEY_ZERO,	KEY_ADD,	KEY_NOT_EXISTED,	KEY_NOT_EXISTED, KEY_NOT_EXISTED, KEY_NOT_EXISTED, KEY_NOT_EXISTED
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
* @brief    Key Driver Init 함수\n
*           Key Driver Ring Buffer Init\n
* @param    none
* @return   none
* @remark   초기에 한번 실행 되는 함수\n
********************************************************************************
*/
void KeyDrvInit(void)
{
    while (POWER_KEY_CHK == 0);	//wait Pwr key off
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

#ifdef USE_QUICK_STORE_FUNCTION
	keyDrvQuickSaveKeyFlag = OFF;
#endif

	keyDrvValidCount = KEY_DRV_VALID_TIME / (KEY_DRV_OUT_PIN_NUM * KEY_DRV_PROC_TIME_INTERVAL);
	//KeyDrvLongValidCount = KEY_DRV_LONG_VALID_TIME / (KEY_DRV_OUT_PIN_NUM * KEY_DRV_PROC_TIME_INTERVAL);
	KeyDrvLongValidCount = KEY_DRV_POWER_KEY_VALID_TIME / (KEY_DRV_OUT_PIN_NUM * KEY_DRV_PROC_TIME_INTERVAL);
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

	//Default table에서 Fixed Key로 Setting되어 있으면 무조건 Default로 load
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

/*#ifdef	USE_DECIMAL_POINT	
	for (i = 0; i < KEY_DRV_MAX_KEY_NUM; i++) // csh 20120731
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
	}
#endif*/		

	if ((keyModelNum != MODEL_ER2_LCD) && (keyModelNum != MODEL_ER2_LCD_ECR)&&(keyModelNum != MODEL_ER2_LCD_COMPARE))
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
* @brief    Key Driver Function Table Save 함수\n
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
			//Default table에서 Fixed Key로 Setting되어 있으면 무조건 Default로 Save
			if (KeyDrvDefaultFuncTable[0][i] == KeyDrvFixedFuncTable[j])
			{
				KeyDrvFuncTable[0][i] = KeyDrvDefaultFuncTable[0][i];
				break;
			}
			//현재 table에서 Fixed Key로 Setting되어 있으면 무조건 Default로 Save
			//Fixed key는 Default table에서만 가능 (변경 불가)
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
* @brief    Key Driver Function Table default 함수\n
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
* @remark   8x6 = 48 keys, scan 하여 Key의 절대 위치 값을 Ring Buffer에 입력\n
*           Timer Interrupt에서 실행됨 (per 1 ms) (\n
*           절대 위치 = 사람이 인식할 수 있는 위치\n
*           예) 4x3 key의 경우\n
*               0  1  2  3\n
*               4  5  6  7\n
*               8  9  10 11\n
*           예) 6x1 key의 경우\n
*               0  1  2  3  4  5  6
********************************************************************************
*/
//Key scan out table
const INT8U code keyOutScanTable[KEY_DRV_OUT_PIN_NUM] = { 0xFE, 0xFD, 0xFB, 0xF7, 0xEF};
//const INT8U code keyOutScanTable[KEY_DRV_OUT_PIN_NUM] = { 0xEF, 0xF7, 0xFB, 0xFD, 0xFE};
//const INT8U code keyOutScanTable[KEY_DRV_OUT_PIN_NUM] = { 0x01, 0x02, 0x04, 0x08, 0x10};
//const INT8U code keyOutScanTable[KEY_DRV_OUT_PIN_NUM] = { 0x10, 0x08, 0x04, 0x02, 0x01};

void  KeyDrvProc(void)
{  
	INT8U keyInByte;

	keyInByte = KEY_IN_PORT;
	keyInByte = keyInByte & KEY_IN_MASK;

	switch (keyInByte)
	{
		case 0x38:
			keyDrvRawData = keyDrvIndex;
			break;
		case 0x34:
			keyDrvRawData = (1 * KEY_DRV_OUT_PIN_NUM) + keyDrvIndex;
			break;
		case 0x2C:
			keyDrvRawData = (2 * KEY_DRV_OUT_PIN_NUM) + keyDrvIndex;
			break;
		case 0x1C:
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
		if (POWER_KEY_CHK == 0)
		{
			if (keyDrvPowerPortStatus) 
			{
				keyDrvPowerKeyPressCount++;
				if (keyDrvPowerKeyPressCount == keyDrvValidCount)
				{
					//keyDrvPowerKeyFlag = ON;
					//만약 power key를 다른 용도로 사용 시 아래와 같이 사용한다.
					//KeyDrvGetFuncCode()에서 keyDrvPowerKeyFlag처리 한다.
					//단, Power key check시 KeyDrvGetFuncCode 호출하여야함으로
					//power key check 함수 수정 필요함
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
* @brief    Key Driver Set Function Code 함수\n
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
		return;	//RB는 Normal Table만 있음
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
* @brief    Key mode에 따른 Key Function code를 받는 함수
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

	//Long Key 처리
	/*
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
	*/
	
	//Long Key 처리
#ifndef USE_QUICK_STORE_FUNCTION
 #ifndef USE_ER2_TURKEY_COMP_KEY
	if (((funcKey == KEY_SUM) || (funcKey == KEY_ADD)) && (keyDrvPressCount != 0) && !(RingBufCheckBuf(&KeyDrvRingBuf)))
 #else
	if (((funcKey == KEY_SUM) || (funcKey == KEY_PRINT) || (funcKey == KEY_ADD)) && (keyDrvPressCount != 0) && !(RingBufCheckBuf(&KeyDrvRingBuf)))
 #endif
#else
 #ifndef USE_ER2_TURKEY_COMP_KEY
	if (((funcKey == KEY_SUM) || (funcKey == KEY_ADD) || ((funcKey >= KEY_PLU_1) && (funcKey <= KEY_PLU_29))) && (keyDrvPressCount != 0) && !(RingBufCheckBuf(&KeyDrvRingBuf)))
 #else
	if (((funcKey == KEY_SUM) || (funcKey == KEY_PRINT) || (funcKey == KEY_ADD) || ((funcKey >= KEY_PLU_1) && (funcKey <= KEY_PLU_29))) && (keyDrvPressCount != 0) && !(RingBufCheckBuf(&KeyDrvRingBuf)))
 #endif
#endif
	{
		if (keyDrvPressCount > KeyDrvLongValidCount)
		{
			switch(funcKey)
			{
				case KEY_SUM:
					funcKey = KEY_PLU_SAVE;
					break;
				case KEY_ADD:
					funcKey = KEY_BAT_CHECK;
					break;
#ifdef USE_ER2_TURKEY_COMP_KEY
				case KEY_PRINT:
					funcKey = KEY_PLU_SAVE;
					break;
#endif
#ifdef USE_QUICK_STORE_FUNCTION
				default:
					if((funcKey >= KEY_PLU_1) && (funcKey <= KEY_PLU_29) && (keyDrvQuickSaveKeyFlag == OFF))
					{
						RingBufPutCharForKeyInterrupt(&KeyDrvRingBuf, rawKey);
						funcKey = KEY_PLU_DIRECT_SAVE;
						keyDrvQuickSaveKeyFlag = ON;
					}
#endif
			}
		}
		else
		{
			funcKey = KEY_NOT_PRESSED;
			RingBufRewindChar(&KeyDrvRingBuf);
		}
	}
//아래 주석부분을 위와 같이 수정---------------------------------------------------------------------------------------------
/*
	if ((funcKey == KEY_SUM) && (keyDrvPressCount != 0) && !(RingBufCheckBuf(&KeyDrvRingBuf)))
	{
		if (keyDrvPressCount > KeyDrvLongValidCount)
		{
			funcKey = KEY_PLU_SAVE;
		}
		else
		{
			funcKey = KEY_NOT_PRESSED;
			RingBufRewindChar(&KeyDrvRingBuf);
		}
	}
	//ADD key Long Key 처리
	if ((funcKey == KEY_ADD) && (keyDrvPressCount != 0) && !(RingBufCheckBuf(&KeyDrvRingBuf)))
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
#ifdef USE_ER2_TURKEY_COMP_KEY
	//Printer Key Long Key 처리
	if ((funcKey == KEY_PRINT) && (keyDrvPressCount != 0) && !(RingBufCheckBuf(&KeyDrvRingBuf)))
	{
		if (keyDrvPressCount > KeyDrvLongValidCount)
		{
			funcKey = KEY_PLU_SAVE;
		}
		else
		{
			funcKey = KEY_NOT_PRESSED;
			RingBufRewindChar(&KeyDrvRingBuf);
		}
	}
#endif	
*/
//-----------------------------------------------------------------------------------------------------------------------
//#endif
	//Power Key Long Key 처리
	if (funcKey == KEY_ON_OFF) 
	{
		if (keyDrvPowerKeyPressCount != 0)
		{
			if (keyDrvPowerKeyPressCount > (KEY_DRV_POWER_KEY_VALID_TIME / (KEY_DRV_OUT_PIN_NUM * KEY_DRV_PROC_TIME_INTERVAL)))
			{
				keyDrvPowerKeyFlag = ON;
			}
			funcKey = KEY_NOT_PRESSED;
			RingBufRewindChar(&KeyDrvRingBuf);
		}
		else
		{
			funcKey = KEY_BACK_LIGHT;
		}
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

#endif//#ifdef KEY_DRV_ER2_LCD


