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
#ifdef KEY_DRV_ER_PRO_LED

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
#define KEY_DRV_MAX_KEY_NUM				65
/** @brief key 저장 개수 (key table param 공간 최대 60 Byte)*/
#define KEY_DRV_MAX_KEY_NUM_IN_PARAM	60
/** @brief key ring buffer size */
#define KEY_DRV_BUF_SIZE					6
/** @brief 입력 Pin 개수 */
#define KEY_DRV_IN_PIN_NUM	 				8
/** @brief 출력 Pin 개수 */
#define KEY_DRV_OUT_PIN_NUM				8
/** @brief KeyDrvProc가 실행되는 시간 간격, 단위 ms*/
#define KEY_DRV_PROC_TIME_INTERVAL		1
/** @brief Key 값을 유효값으로 인정할 시간, 단위 ms*/
#define KEY_DRV_VALID_TIME					40
/** @brief 길게 누른 Key 값을 다른 Key로 대체할 기준 시간, 단위 ms*/
#define KEY_DRV_LONG_VALID_TIME			3000 
/** @brief 길게 누른 Power Key 인식 기준 시간, 단위 ms*/
#define KEY_DRV_POWER_KEY_VALID_TIME		1000

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
#ifdef	USE_DECIMAL_POINT	
/** @brief Decimal Point key on flag */
BOOLEAN DecimalPointKeyExistFlag; //csh 20120730
#endif

/** @brief power key 가 눌려진 횟수 (KeyDrvProc 실행 간격에 의존)*/
static data INT16U keyDrvPowerKeyPressCount;//data for high speed in interrupt
/** @brief power key 이전 상태 */
static BOOLEAN keyDrvPowerPortStatus;
/** @brief power key on flag */
static BOOLEAN keyDrvPowerKeyFlag;
/** @brief Backlight key Exist flag */
static BOOLEAN KeyDrvBLKeyExist;

/** @brief convert raw code to human readable position code */

static const INT8U code KeyDrvConvTable[KEY_DRV_MAX_KEY_NUM] =
{
	0,	1,	53,	41,	54,	50,	36,	37,
	2,	3,	52,	48,	57,	59,	39,	40,
	4,	5,	51,	47,	43,	58,	38,	45,
	6,	7,	56,	46,	42,	55,	49,	44,
	9,	10,	11,	12,	13,	14,	15,	16,
	18,	19,	20,	21,	22,	23,	24,	25,
	27,	28,	29,	30,	31,	32,	33, 34,
	8, 	17, 26,	35, 61,	62,	63, 64, 60
};

/** @brief raw key(키 위치 번호)를 key function code로 변환하는 table */
static KEY_TYPE KeyDrvFuncTable[1][KEY_DRV_MAX_KEY_NUM];

/** @brief raw key(키 위치 번호)를 key function code로 변환하는 table의 default 값 */
//영공사양
#ifdef USE_DIRECT_PLUKEY_TYPE_UN
const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] = 
{
	{
		KEY_PLU_6,		KEY_PLU_7,		KEY_PLU_8,		KEY_PLU_9,		KEY_PLU_10,	KEY_PLU_11,	KEY_PLU_12,	KEY_PLU_13,	KEY_PLU_14, //9		,		
		KEY_PLU_15,	KEY_PLU_16,	KEY_PLU_17,  	KEY_PLU_18,	KEY_PLU_19,	KEY_PLU_20,	KEY_PLU_21,	KEY_PLU_22,	KEY_PLU_23, //18
		KEY_PLU_24,	KEY_PLU_25,	KEY_PLU_26, 	KEY_PLU_27, 	KEY_PLU_28,	KEY_PLU_29,	KEY_PLU_30, 	KEY_PLU_31,	KEY_PLU_32, //27
		KEY_PLU_33,	KEY_PLU_34,	KEY_PLU_35,	KEY_PLU_36,	KEY_PLU_37,	KEY_PLU_38,	KEY_PLU_39,	KEY_PLU_40,	KEY_PLU_41, //36
		
		KEY_PLU_1, 	KEY_PLU_2, 	KEY_PLU_3, 	KEY_PLU_4, 	KEY_PLU_5, 	//41
		KEY_NUM_7,	KEY_NUM_8,	KEY_NUM_9,	KEY_PLU_SAVE,	KEY_PLU_CALL,	//46
		KEY_NUM_4,	KEY_NUM_5,	KEY_NUM_6,	KEY_ADD,		KEY_CANCEL,	//51
		KEY_NUM_1,	KEY_NUM_2,	KEY_NUM_3,	KEY_X,			KEY_SUM,	//56
		KEY_NUM_0,	KEY_C,			KEY_ZERO,		KEY_TARE,		KEY_ON_OFF,	//61
		KEY_NO_FUNC,	KEY_NO_FUNC,	KEY_NO_FUNC,	KEY_NO_FUNC
	}
};

#elif defined (USE_DIRECT_PLUKEY_TYPE_M1_M7)
// 터키사양 
const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] = 
{
	{
		KEY_PLU_8,	KEY_PLU_9,	KEY_PLU_10,	KEY_PLU_11,	KEY_PLU_12,	KEY_PLU_13,	KEY_PLU_14,	KEY_PLU_15,	KEY_PLU_16, //8		,		
		KEY_PLU_17,	KEY_PLU_18,	KEY_PLU_19,  	KEY_PLU_20,	KEY_PLU_21,	KEY_PLU_22,	KEY_PLU_23,	KEY_PLU_24,	KEY_PLU_25, //17
		KEY_PLU_26,	KEY_PLU_27,	KEY_PLU_28, 	KEY_PLU_29, 	KEY_PLU_30,	KEY_PLU_31,	KEY_PLU_32, 	KEY_PLU_33,	KEY_PLU_34, //26
		KEY_PLU_35,	KEY_PLU_36,	KEY_PLU_37,	KEY_PLU_38,	KEY_PLU_39,	KEY_PLU_40,	KEY_PLU_41,	KEY_PLU_42,	KEY_PLU_43, //35
		
		KEY_PLU_1, 	KEY_PLU_2, 	KEY_PLU_3, 	KEY_PLU_4, 	KEY_PLU_5, 	//40
		KEY_NUM_7,	KEY_NUM_8,	KEY_NUM_9,	KEY_PLU_6,	KEY_PLU_7,	//45
		KEY_NUM_4,	KEY_NUM_5,	KEY_NUM_6,	KEY_PLU_SAVE,	KEY_PLU_CALL,	//50
		KEY_NUM_1,	KEY_NUM_2,	KEY_NUM_3,	KEY_BAT_CHECK,	KEY_NOT_EXISTED,	//55
		KEY_NUM_0,	KEY_C,		KEY_ZERO,	KEY_TARE,	KEY_ON_OFF,	//60
		KEY_NO_FUNC,	KEY_NO_FUNC,	KEY_NO_FUNC,	KEY_NO_FUNC
	}
};

#elif defined (USE_DIRECT_PLUKEY_TYPE_M1_M9)

//Netherlands Key Array
const KEY_TYPE code KeyDrvDefaultFuncTable[1][KEY_DRV_MAX_KEY_NUM] = 
{
	{
		KEY_PLU_10,	KEY_PLU_11,	KEY_PLU_12,	KEY_PLU_13,	KEY_PLU_14,	KEY_PLU_15,	KEY_PLU_16,	KEY_PLU_17,	KEY_PLU_18, //9		,		
		KEY_PLU_19,	KEY_PLU_20,	KEY_PLU_21,  	KEY_PLU_22,	KEY_PLU_23,	KEY_PLU_24,	KEY_PLU_25,	KEY_PLU_26,	KEY_PLU_27, //18
		KEY_PLU_28,	KEY_PLU_29,	KEY_PLU_30, 	KEY_PLU_31, 	KEY_PLU_32,	KEY_PLU_33,	KEY_PLU_34, 	KEY_PLU_35,	KEY_PLU_36, //27
		KEY_PLU_37,	KEY_PLU_38,	KEY_PLU_39,	KEY_PLU_40,	KEY_PLU_41,	KEY_PLU_42,	KEY_PLU_43,	KEY_PLU_44,	KEY_PLU_45, //36
		
		KEY_PLU_1, 	KEY_PLU_2, 	KEY_PLU_3, 	KEY_PLU_4, 		KEY_PLU_5, 	//41
		KEY_NUM_7,	KEY_NUM_8,	KEY_NUM_9,	KEY_PLU_6,			KEY_PLU_7,		//46
		KEY_NUM_4,	KEY_NUM_5,	KEY_NUM_6,	KEY_PLU_8,			KEY_PLU_9,		//51
		KEY_NUM_1,	KEY_NUM_2,	KEY_NUM_3,	KEY_PLU_SAVE,		KEY_PLU_CALL,	//56
		KEY_NUM_0,	KEY_C,			KEY_ZERO,		KEY_TARE,			KEY_ON_OFF,	//61
		KEY_NO_FUNC,	KEY_NO_FUNC,	KEY_NO_FUNC,	KEY_NO_FUNC 						//65
	}
};

#endif //DIRECT PLUKEY TYPE

/** @brief Fixed key의 최대 개수*/
#define KEY_DRV_MAX_FIXED_KEY_NUM	15
/** @brief 사용자가 변경할 수 없는 Key들의 배열 */
const KEY_TYPE code KeyDrvFixedFuncTable[KEY_DRV_MAX_FIXED_KEY_NUM] =
{
	KEY_NUM_0, KEY_NUM_1, KEY_NUM_2, KEY_NUM_3, KEY_NUM_4,
	KEY_NUM_5, KEY_NUM_6, KEY_NUM_7, KEY_NUM_8, KEY_NUM_9,
	KEY_C, KEY_ZERO, KEY_TARE, KEY_ON_OFF, KEY_NO_FUNC
};

/** 
* @brief menu mode에서 사용할 key func code 
* @remark Enter, C, MenuUp, MenuDown, INCrease, ROTate 순서임 (see KEY_MENU_ENTER_NUM~) \n
*         주의 : 숫자 Key 사용 시 반드시 INC, ROT는 KEY_NOT_EXISTED으로 해야함
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

	keyDrvValidCount = KEY_DRV_VALID_TIME / (KEY_DRV_OUT_PIN_NUM * KEY_DRV_PROC_TIME_INTERVAL);
	KeyDrvLongValidCount = KEY_DRV_LONG_VALID_TIME / (KEY_DRV_OUT_PIN_NUM * KEY_DRV_PROC_TIME_INTERVAL);
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
	//ParamReadData(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_TABLE, (INT8U *)&KeyDrvFuncTable[0][0], KEY_DRV_MAX_KEY_NUM);
	ParamReadData(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_TABLE, (INT8U *)&KeyDrvFuncTable[0][0], KEY_DRV_MAX_KEY_NUM_IN_PARAM);
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

	if (keyModelNum != MODEL_ER_PRO_LED)
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
	
	KeyDrvBLKeyExist = FALSE;
		
	for (i = 0; i < KEY_DRV_MAX_KEY_NUM; i++)
	{
		if(KeyDrvFuncTable[0][i] == KEY_BACK_LIGHT) 
		{
			KeyDrvBLKeyExist = TRUE;
			break;
		}
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
		//ParamReadData(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_TABLE, (INT8U *)&KeyDrvFuncTable[0][0], KEY_DRV_MAX_KEY_NUM);
		ParamReadData(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_TABLE, (INT8U *)&KeyDrvFuncTable[0][0], KEY_DRV_MAX_KEY_NUM_IN_PARAM);
	}

	KeyDrvBLKeyExist = FALSE;
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
		
		if(KeyDrvFuncTable[0][i] == KEY_BACK_LIGHT) 
		{
			KeyDrvBLKeyExist = TRUE;
		}			
	}

	ParamWriteWord(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_MODEL, MODEL_NUM);
	ParamWriteByte(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_CHECKSUM, sum);
	//ParamWriteData(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_TABLE, (INT8U *)&KeyDrvFuncTable[0][0], KEY_DRV_MAX_KEY_NUM);
	ParamWriteData(PARAM_KEY_AREA_NUM, KEY_PARAM_OFFSET_TABLE, (INT8U *)&KeyDrvFuncTable[0][0], KEY_DRV_MAX_KEY_NUM_IN_PARAM);
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
* @brief    Key Driver Function Table Turkey default 함수\n
* @param    none
* @return   none
* @remark
********************************************************************************
*/
#ifdef USE_ER_PLUS_M_TURKEY_KEY_DEFINE
void KeyDrvSetTurkeyFuncTable(void)
{
	INT8U i;

	for (i = 0; i < 36; i++)
	{
		KeyDrvFuncTable[0][i] = KEY_PLU_8 + i;
	}
	for (i = 36; i < KEY_DRV_MAX_KEY_NUM; i++)
	{
		KeyDrvFuncTable[0][i] = KeyDrvDefaultFuncTable[0][i];
	}
	KeyDrvFuncTable[0][44] =  KEY_PLU_6;
	KeyDrvFuncTable[0][45] =  KEY_PLU_7;
	KeyDrvFuncTable[0][49] =  KEY_PLU_SAVE;
	KeyDrvFuncTable[0][50] =  KEY_PLU_CALL;
	KeyDrvFuncTable[0][54] =  KEY_BAT_CHECK;
	KeyDrvFuncTable[0][55] =  KEY_BACK_LIGHT;
	KeyDrvSaveFuncTable(UPDATE_KEY_TABLE);
}
#endif

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
void  KeyDrvProc(void)
{  
	INT8U keyInByte;
	INT8U keyInByteTemp;

	keyInByte = KEY_IN_PORT;
	keyInByteTemp = (INT8U)((INT8U)KEY_IN_PORT_REMAIN1 << 6) & 0x40;
	keyInByteTemp |= ((INT8U)((INT8U)KEY_IN_PORT_REMAIN2 << 7) & 0x80);	//add
	keyInByte = ((keyInByte >> 2) & KEY_IN_MASK);
	keyInByte = keyInByte | keyInByteTemp; 
	switch (keyInByte)
	{
		case 0xFE:
			keyDrvRawData = keyDrvIndex;
			break;
		case 0xFD:
			keyDrvRawData = (1 * KEY_DRV_OUT_PIN_NUM) + keyDrvIndex;
			break;
		case 0xFB:
			keyDrvRawData = (2 * KEY_DRV_OUT_PIN_NUM) + keyDrvIndex;
			break;
		case 0xF7:
			keyDrvRawData = (3 * KEY_DRV_OUT_PIN_NUM) + keyDrvIndex;
			break;			
		case 0xEF:
			keyDrvRawData = (4 * KEY_DRV_OUT_PIN_NUM) + keyDrvIndex;
			break;
		case 0xDF:
			keyDrvRawData = (5 * KEY_DRV_OUT_PIN_NUM) + keyDrvIndex;
			break;
		case 0xBF:
			keyDrvRawData = (6 * KEY_DRV_OUT_PIN_NUM) + keyDrvIndex;
			break;
		case 0x7F:
			keyDrvRawData = (7 * KEY_DRV_OUT_PIN_NUM) + keyDrvIndex;
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
					//만약 power key를 다른 용도로 사용 시 아래와 같이 사용한다.
					//KeyDrvGetFuncCode()에서 keyDrvPowerKeyFlag처리 한다.
					//단, Power key check시 KeyDrvGetFuncCode 호출하여야함으로
					//power key check 함수 수정 필요함
					RingBufPutCharForKeyInterrupt(&KeyDrvRingBuf, 60);//KEY_ON_OFF raw code
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
		
	KEY_OUT0 = keyDrvIndex & 0x01;
	KEY_OUT1 = keyDrvIndex & 0x02;
	KEY_OUT2 = keyDrvIndex & 0x04;
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
			case 44:	//KEY_PLU_SAVE
				funcKey = KEY_NUM_A;
				break;
			case 49:	//KEY_ADD
				funcKey = KEY_NUM_B;
				break;
			case 54:	//KEY_MUL
				funcKey = KEY_NUM_C;
				break;
			case 45:	//KEY_PLU_CALL
				funcKey = KEY_NUM_D;
				break;
			case 50:	//KEY_CANCEL
				funcKey = KEY_NUM_E;
				break;
			case 55:	//KEY_SUM
				funcKey = KEY_NUM_F;
				break;
		}
	}
	//Long Key 처리
	if (((funcKey == KEY_SUM) || (funcKey == KEY_PLU_CALL)) && (keyDrvPressCount != 0) && !(RingBufCheckBuf(&KeyDrvRingBuf)))
	{
		if (keyDrvPressCount > KeyDrvLongValidCount)
		{
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
	//Power Key Long Key 처리
	/*
	if (funcKey == KEY_ON_OFF) 
	{
		if(!KeyDrvBLKeyExist)
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
		else
		{
			keyDrvPowerKeyFlag = ON;
		}
	}	*/
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

#endif//#ifdef KEY_DRV_ER_LCD

