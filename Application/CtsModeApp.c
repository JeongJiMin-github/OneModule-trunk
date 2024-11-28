/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Cts Mode Applicaion Source File\n
* @file    CtsModeApp.c
* @version 1.0
* @date    2013/01/25
* @author  Deok Hyun Kim (KDH)
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
#include "../Common/CommonLib.h"
#include "../Time/TimeApi.h"
#include "CtsModeApp.h"

#ifdef USE_CTS_MODE
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

/** @brief CTS CMD : CAL enter enable */
#define CAL_K           0xCA082012
/** @brief CTS CMD : Set CTS mode enable */
#define CTSON_K         0xCB419324
/** @brief CTS CMD : Set CAL mode disable */
#define CTSOFF_K        0xCC398743
/** @brief CTS CMD : Set CAL complete */
#define CALCOMP_K       0xCD137423
/** @brief CTS CMD : Set CTS period */
#define PERIOD_K        0x34190000
/** @brief Encryption key : Time */
#define K_TIME          0x00083419
/** @brief Encryption key : Zero */
#define K_ZERO          0x41908300
/** @brief Encryption key : Span */
#define K_SPAN          0x19830419

/** @brief Flag : CTS initialized */
#define CTS_INIT_COMPLETED 0x83


/*
********************************************************************************
*                       LOCAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert local typedef & structure here */

/** @brief Time data*/
typedef struct {
	/** @brief Second*/
	INT8U sec;
	/** @brief Minute*/
	INT8U min;
	/** @brief Hour*/
	INT8U hour;
	/** @brief Date*/
	INT8U date;
	/** @brief Month*/
	INT8U month;
	/** @brief Day, 0~6*/
	INT8U day;
	/** @brief year*/
	INT8U year;	
} TIME_DATA;

/** @brief eeprom에 저장되는 CTS 데이터*/
typedef struct {
	INT8U  ctsEnable;			// 0
	INT8U  ctsVersion;			// 1
	INT8U  initialized;			// 2
	INT8U  ctsExpired;			// 3
	INT8U  ctsCalYear;			// 4
	INT8U  ctsCalMonth;			// 5
	INT8U  ctsCalDate;			// 6
	INT8U  reserve1;			// 7
	INT16U ctsPeriod;			// 8
	INT16U reserve2;			// 10
	INT8U  serialNo[16];		// 12
	INT8U  productCode[16];		// 28
	INT32U boardId;				// 44
	INT32U encryptKey1;			// 48
	INT8U  reserve[26];			// 52
	INT8U  bccErrFlag;			// 78	// bccError가 발생할 경우 CTS_ERROR 1로 기록
	INT8U  bcc;					// 79
} CTS_STRUCT;					//size 80, rom 데이터

/** @brief 암호화에 사용되는 저울정보 백업*/
typedef struct {
	INT32S zero;
	INT32S midUp;
	INT32S midDn;
	INT32S span;
	INT32U time;
} CTS_ENCRYPT_BUF_STRUCT;  	// size 12, ram 데이터, 암호화에 사용되는 데이터

/** @brief 저울 정보*/
typedef struct {
	INT8U  modelName[12];		//0
	INT32U scaleVersion;		//12
	INT32S zero;				//16
	INT32S midUp;				//20
	INT32S midDn;				//24
	INT32S span;				//28
	INT16U gravityFactory;		//32
	INT16U gravityLocal;		//34
	INT32U capa;				//36
	INT8U  e;					//40
	INT8U  d;					//41
	INT8U  wdp;					//42
	INT8U  calState;			//43
	INT8U  unit[2];				//44
} CTS_INFO_STRUCT;				// size 46, ram 데이터, 저울 정보

/** @brief 외부로 전달될 CMD*/
typedef struct {
	INT8U  calEnable;
	INT8U  calComp;
} CTS_CMD_STRUCT;


/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/
/* Insert file scope variable & tables here */
/** @brief 저울 정보*/
CTS_INFO_STRUCT data CtsInfoStruct; // OMV2 환경 xdata 부족하여 data 영역 사용. OMV4에서는 영향 없음 확인.
/** @brief 외부에 저장된 CTS 데이터의 포인터*/
CTS_STRUCT *CtsStructPtr;
/** @brief 암호화에 사용되는 저울정보 백업*/
CTS_ENCRYPT_BUF_STRUCT CtsEncryptBufStruct; 
/** @brief 외부로 전달될 CMD*/
CTS_CMD_STRUCT CtsCmdStruct;
/** @brief 시간 정보 백업*/
TIME_DATA TimeDataBuf;

/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
void ctsEncryptKeyBufInit(void);
INT32U ctsDecrypt(INT32U key);

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/

/**
********************************************************************************
* @brief    CTS update scale data 함수\n
*           저울 정보를 업데이트 하는 함수\n
* @param    id : 데이터 인덱스\n
*			src : 데이터 포인트\n
* @return   0: No error, 1: Error
* @remark   \n
********************************************************************************
*/
INT8U CtsUpdateScaleData(INT8U id, INT8U *src)
{
	INT32U buf;
	
	switch (id)
	{
		case 0:
			CLMemCopy(CtsInfoStruct.modelName, src, sizeof(CtsInfoStruct.modelName));
			break;
		case 1:
			CtsInfoStruct.scaleVersion = *(INT32U *)(src);
			break;
		case 2:
			buf = *(INT32S *)(src);
			CtsInfoStruct.zero =buf;
			break;
		case 3:
			CtsInfoStruct.midUp = *(INT32S *)(src);
			break;
		case 4:
			CtsInfoStruct.midDn = *(INT32S *)(src);
			break;
		case 5:
			CtsInfoStruct.span = *(INT32S *)(src);
			break;
		case 6:
			CtsInfoStruct.gravityFactory = *(INT16U *)(src);
			break;
		case 7:
			CtsInfoStruct.gravityLocal = *(INT16U *)(src);
			break;
		case 8:
			CtsInfoStruct.capa = *(INT32U *)(src);
			break;
		case 9:
			CtsInfoStruct.e = *src;
			break;
		case 10:
			CtsInfoStruct.d = *src;
			break;
		case 11:
			CtsInfoStruct.wdp = *src;
			break;
		case 12:
			CtsInfoStruct.calState = *src;
			break;
		case 13:
			CLMemCopy(CtsInfoStruct.unit, src, sizeof(CtsInfoStruct.unit));
			break;
		case 20:
			CtsEncryptBufStruct.zero = *(INT32U *)(src);
			break;
		case 21:
			CtsEncryptBufStruct.span = *(INT32U *)(src);
			break;
		case 22:
			CtsEncryptBufStruct.time = *(INT32U *)(src);
			break;
		default:
			return CTS_ERROR;
	}
	return CTS_NO_ERROR;
}

/**
********************************************************************************
* @brief   Cts mode init 함수\n
*           cts mode enable disable 을 결정함 \n
* @param    CTS data buffer pointer
* @return   CTS_NOT_NEED_UPDATE 0, CTS_NEED_UPDATE 0xFF
* @remark   초기에 실행 되는 함수\n
********************************************************************************
*/
INT8U CtsModeInit(INT8U *ctsBuf)
{
	INT8U returnVal = CTS_NOT_NEED_UPDATE;
	
	CtsStructPtr = (CTS_STRUCT *)ctsBuf;
	
	CtsCmdStruct.calEnable = 0;
	CtsCmdStruct.calComp = 0;
	
	if ((*CtsStructPtr).initialized != CTS_INIT_COMPLETED || (*CtsStructPtr).bcc != CalcBcc((INT8U*)CtsStructPtr,CTS_STRUCT_SIZE - 1))
	{
		if ((*CtsStructPtr).initialized == CTS_INIT_COMPLETED)
		{
			(*CtsStructPtr).bccErrFlag = CTS_ERROR;
		}
		else
		{
			(*CtsStructPtr).bccErrFlag = CTS_NO_ERROR;
		}
		(*CtsStructPtr).ctsEnable = OFF;
		(*CtsStructPtr).ctsVersion = 2;
		(*CtsStructPtr).initialized = CTS_INIT_COMPLETED;
		(*CtsStructPtr).ctsExpired = 0;
		(*CtsStructPtr).ctsCalYear = 1;
		(*CtsStructPtr).ctsCalMonth = 1;
		(*CtsStructPtr).ctsCalDate = 1;
		(*CtsStructPtr).reserve1 = 0xFF;
		(*CtsStructPtr).ctsPeriod = 0;
		(*CtsStructPtr).reserve2 = 0xFFFF;
		CLMemSet((*CtsStructPtr).serialNo, 0, sizeof((*CtsStructPtr).serialNo));
		CLMemSet((*CtsStructPtr).productCode, 0, sizeof((*CtsStructPtr).productCode));
		(*CtsStructPtr).boardId = 0;
		(*CtsStructPtr).encryptKey1 = K_TIME; 
		CLMemSet((*CtsStructPtr).reserve, 0xFF, sizeof((*CtsStructPtr).reserve));
		(*CtsStructPtr).bcc = CalcBcc((INT8U*)CtsStructPtr, CTS_STRUCT_SIZE - 1);
		returnVal = CTS_NEED_UPDATE;
	}
	
	ctsEncryptKeyBufInit();
	
	return returnVal;
}

/**
********************************************************************************
* @brief	Check CTS expired 함수 \n
* @param    TimeDataPtr \n
* @return   CTS_NOT_EXPIRED	0, CTS_EXPIRED 1, CTS_NEED_UPDATE 0xFF \n
* @remark   \n
********************************************************************************
*/
INT8U CtsCheckExpired(INT8U *TimeDataPtr)
{
	INT16U calMonth;
	INT16U curMonth;
	INT16U ctsExpiredYear;
	INT16U ctsExpiredMonth;
	INT16U ctsExpiredDate;
	INT16U expiredMonth;
	INT8U  returnVal = CTS_NOT_EXPIRED;
	
	if ((*CtsStructPtr).ctsEnable == OFF)       // cts enable check
		return CTS_NOT_EXPIRED;
	
	CtsTimeSync(TimeDataPtr);
	
	if ((*CtsStructPtr).ctsExpired == OFF)
	{
		if ((*CtsStructPtr).ctsPeriod != 0)
		{
			calMonth = 12 * (*CtsStructPtr).ctsCalYear + (*CtsStructPtr).ctsCalMonth;
			curMonth = 12 * TimeDataBuf.year + TimeDataBuf.month;

			ctsExpiredYear = (*CtsStructPtr).ctsCalYear + 2000;
		    ctsExpiredMonth = (*CtsStructPtr).ctsCalMonth;
		    ctsExpiredDate = (*CtsStructPtr).ctsCalDate;
			
			TimeGetNextDay(&ctsExpiredYear, &ctsExpiredMonth, &ctsExpiredDate, (*CtsStructPtr).ctsPeriod);
			expiredMonth = 12 * (ctsExpiredYear - 2000) + ctsExpiredMonth;
			
			if ((curMonth < calMonth) ||
				(curMonth > expiredMonth) ||
				((curMonth == expiredMonth) && (TimeDataBuf.date >= ctsExpiredDate)))
			{
                (*CtsStructPtr).ctsExpired = CTS_ON;
				(*CtsStructPtr).bcc = CalcBcc((INT8U*)CtsStructPtr, CTS_STRUCT_SIZE - 1);
				returnVal = CTS_NEED_UPDATE;
			}
		}
	}
	else
	{
		returnVal = CTS_EXPIRED;;
	}
	return returnVal;
}

/**
********************************************************************************
* @brief	Check CTS expired 함수 \n
* @param    \n
* @return   CTS_DISABLED 0, CTS_ENABLED 1\n
* @remark   \n
********************************************************************************
*/
INT8U CtsCheckEnabled(void)
{
	if ((*CtsStructPtr).ctsEnable)
	{
		return CTS_ENABLED;
	}
	else
	{
		return CTS_DISABLED;
	}
}

/**
********************************************************************************
* @brief	Check CTS CAL enter enabled 함수 \n
* @param    \n
* @return   CTS_DISABLED 0, CTS_ENABLED 1\n
* @remark   \n
********************************************************************************
*/
INT8U CtsCheckCalEnterEnabled(void)
{
	if (CtsCmdStruct.calEnable)
	{
		CtsCmdStruct.calEnable = CTS_DISABLED;
		return CTS_ENABLED;
	}
	else
	{
		return CTS_DISABLED;
	}
}

/**
********************************************************************************
* @brief	CTS communication function \n
* @param    Read/Write, Address, Input data, Output data pointer\n
* @return   CTS_NO_ERROR 0, CTS_COMM_WRONG_REQ 1, CTS_COMM_CMD_ERROR 2\n
* @remark   \n
********************************************************************************
*/
INT8U CtsCommFunc(INT8U rw, INT32U addr, INT32S data32s, INT32S *sendDataPtr)
{
	INT8U error = CTS_NO_ERROR;
	INT8U offset;
	INT32U cmdBuf;
	
	if (rw == 'R') 
	{
		switch (addr)
		{
			case CTS_ADDR_MODEL1:
				*sendDataPtr = *((INT32U *)(&CtsInfoStruct.modelName[0]));
				break;
			case CTS_ADDR_MODEL2:
				*sendDataPtr = *((INT32U *)(&CtsInfoStruct.modelName[4]));
				break;
			case CTS_ADDR_MODEL3:
				*sendDataPtr = *((INT32U *)(&CtsInfoStruct.modelName[8]));
				break;
			case CTS_ADDR_PROGRAM_VER:
				*sendDataPtr = CtsInfoStruct.scaleVersion;
				break;
			case CTS_ADDR_SERIALNUM1:
				*sendDataPtr = *((INT32U *)(&(*CtsStructPtr).serialNo[0]));
				break;
			case CTS_ADDR_SERIALNUM2:
				*sendDataPtr = *((INT32U *)(&(*CtsStructPtr).serialNo[4]));
				break;
			case CTS_ADDR_SERIALNUM3:
				*sendDataPtr = *((INT32U *)(&(*CtsStructPtr).serialNo[8]));
				break;
			case CTS_ADDR_SERIALNUM4:
				*sendDataPtr = *((INT32U *)(&(*CtsStructPtr).serialNo[12]));
				break;
			case CTS_ADDR_YEAR:
				*sendDataPtr = (*CtsStructPtr).ctsCalYear;
				break;
			case CTS_ADDR_MONTH:
				*sendDataPtr = (*CtsStructPtr).ctsCalMonth;
				break;
			case CTS_ADDR_DATE:
				*sendDataPtr = (*CtsStructPtr).ctsCalDate;
				break;
			case CTS_ADDR_ZERO:
				*sendDataPtr = CtsEncryptBufStruct.zero;
				break;
			case CTS_ADDR_MIDUP:
				*sendDataPtr = CtsEncryptBufStruct.midUp;
				break;
			case CTS_ADDR_MIDDOWN:
				*sendDataPtr = CtsEncryptBufStruct.midDn;
				break;
			case CTS_ADDR_SPAN:
				*sendDataPtr = CtsEncryptBufStruct.span;
				break;
			case CTS_ADDR_CAPA:
				*sendDataPtr = CtsInfoStruct.capa;
				break;
			case CTS_ADDR_EVALUE:
				*sendDataPtr = CtsInfoStruct.e;
				break;
			case CTS_ADDR_DVALUE:
				*sendDataPtr = CtsInfoStruct.d;
				break;
			case CTS_ADDR_WDP:
				*sendDataPtr = CtsInfoStruct.wdp;
				break;
			case CTS_ADDR_UNIT:
				*sendDataPtr = *(INT16U *)(CtsInfoStruct.unit);
				break;
			case CTS_ADDR_CALG:
				*sendDataPtr = CtsInfoStruct.gravityFactory;
				break;
			case CTS_ADDR_USINGG:
				*sendDataPtr = CtsInfoStruct.gravityLocal;
				break;
			case CTS_ADDR_CTSENABLE:
				*sendDataPtr = (*CtsStructPtr).ctsEnable;
				break;
			case CTS_ADDR_CTSPERIOD:
				*sendDataPtr = (*CtsStructPtr).ctsPeriod;
				break;
			case CTS_ADDR_CAL_STATE:
				*sendDataPtr = CtsInfoStruct.calState;
				break;
			case CTS_ADDR_CTS_VERSION:
				*sendDataPtr = (*CtsStructPtr).ctsVersion;
				break;
			case CTS_ADDR_PRODUCT_CODE1:
				*sendDataPtr = *((INT32U *)(&(*CtsStructPtr).productCode[0]));
				break;
			case CTS_ADDR_PRODUCT_CODE2:
				*sendDataPtr = *((INT32U *)(&(*CtsStructPtr).productCode[4]));
				break;
			case CTS_ADDR_PRODUCT_CODE3:
				*sendDataPtr = *((INT32U *)(&(*CtsStructPtr).productCode[8]));
				break;
			case CTS_ADDR_PRODUCT_CODE4:
				*sendDataPtr = *((INT32U *)(&(*CtsStructPtr).productCode[12]));
				break;
			case CTS_ADDR_BOARDID:
				*sendDataPtr = (*CtsStructPtr).boardId;
				break;
			case CTS_ADDR_BCC_ERR:
				*sendDataPtr = (*CtsStructPtr).bccErrFlag;
				break;
			default:
				error = CTS_COMM_WRONG_REQ;
				break;
		}
	} 
	else if (rw == 'W')
	{
		switch (addr)
		{
			case CTS_ADDR_SERIALNUM1:
			case CTS_ADDR_SERIALNUM2:
			case CTS_ADDR_SERIALNUM3:
			case CTS_ADDR_SERIALNUM4:
				switch(addr)
				{
					case CTS_ADDR_SERIALNUM1:
						offset= 0;
						break;
					case CTS_ADDR_SERIALNUM2:
						offset= 4;
						break;
					case CTS_ADDR_SERIALNUM3:
						offset= 8;
						break;
					case CTS_ADDR_SERIALNUM4:
						offset= 12;
						break;
				}
				CLMemCopy(&(*CtsStructPtr).serialNo[offset], (INT8U *)(&data32s), 4);
				(*CtsStructPtr).bcc = CalcBcc((INT8U*)CtsStructPtr, CTS_STRUCT_SIZE - 1);
				*sendDataPtr = data32s;
				break;
			case CTS_ADDR_PRODUCT_CODE1:
			case CTS_ADDR_PRODUCT_CODE2:
			case CTS_ADDR_PRODUCT_CODE3:
			case CTS_ADDR_PRODUCT_CODE4:
				switch(addr)
				{
					case CTS_ADDR_PRODUCT_CODE1:
						offset= 0;
						break;
					case CTS_ADDR_PRODUCT_CODE2:
						offset= 4;
						break;
					case CTS_ADDR_PRODUCT_CODE3:
						offset= 8;
						break;
					case CTS_ADDR_PRODUCT_CODE4:
						offset= 12;
						break;
				}
				CLMemCopy(&(*CtsStructPtr).productCode[offset], (INT8U *)(&data32s), 4);
				(*CtsStructPtr).bcc = CalcBcc((INT8U*)CtsStructPtr, CTS_STRUCT_SIZE - 1);
				*sendDataPtr = data32s;
				break;
			case CTS_ADDR_BOARDID:
				(*CtsStructPtr).boardId = data32s;
				(*CtsStructPtr).bcc = CalcBcc((INT8U*)CtsStructPtr, CTS_STRUCT_SIZE - 1);
				*sendDataPtr = data32s;
				break;			
			case CTS_ADDR_ENCRYPTKEY:
				(*CtsStructPtr).encryptKey1 = data32s;
				(*CtsStructPtr).bcc = CalcBcc((INT8U*)CtsStructPtr, CTS_STRUCT_SIZE - 1);
				*sendDataPtr = data32s;
				break;
			case CTS_ADDR_CTSCMD:
				cmdBuf = ctsDecrypt(data32s);
				*sendDataPtr = CTS_COMM_CMD_ERROR;
				if (cmdBuf == CAL_K)
				{
					CtsCmdStruct.calEnable = CTS_ENABLED;
					*sendDataPtr = CTS_COMM_CMD_NO_ERROR;
				}
				else if (cmdBuf == CTSON_K) 
				{
					(*CtsStructPtr).ctsEnable = CTS_ENABLED;
					(*CtsStructPtr).bcc = CalcBcc((INT8U*)CtsStructPtr, CTS_STRUCT_SIZE - 1);
					*sendDataPtr = CTS_COMM_CMD_NO_ERROR;
				}					
				else if (cmdBuf == CTSOFF_K)
				{
					(*CtsStructPtr).ctsEnable = CTS_DISABLED;
					(*CtsStructPtr).bcc = CalcBcc((INT8U*)CtsStructPtr, CTS_STRUCT_SIZE - 1);
					*sendDataPtr = CTS_COMM_CMD_NO_ERROR;
				}
				else if (cmdBuf == CALCOMP_K) 
				{
					(*CtsStructPtr).ctsCalYear = TimeDataBuf.year;
					(*CtsStructPtr).ctsCalMonth = TimeDataBuf.month;
					(*CtsStructPtr).ctsCalDate = TimeDataBuf.date;
					(*CtsStructPtr).ctsExpired = CTS_OFF;
					(*CtsStructPtr).bcc = CalcBcc((INT8U*)CtsStructPtr, CTS_STRUCT_SIZE - 1);
					ctsEncryptKeyBufInit();
					*sendDataPtr = CTS_COMM_CMD_NO_ERROR;
				}
				else if ((cmdBuf & 0xFFFF0000) == PERIOD_K)
				{
					(*CtsStructPtr).ctsPeriod = (INT16U)(cmdBuf & 0x0000FFFF);
					(*CtsStructPtr).bcc = CalcBcc((INT8U*)CtsStructPtr, CTS_STRUCT_SIZE - 1);
					*sendDataPtr = CTS_COMM_CMD_NO_ERROR;
				}
				break;
			case CTS_ADDR_RESET:
				*sendDataPtr = data32s;
				break;
			case CTS_ADDR_CTS_DATA_INIT:	// Test용 프로토콜. Trunk 머지 시 해당 프로토콜 삭제
				(*CtsStructPtr).initialized = 0;
				*sendDataPtr = data32s;
				break;
			default:
				error = CTS_COMM_WRONG_REQ;
				break;
		}
	}
	return error;
}

/**
********************************************************************************
* @brief	CTS Time Sync \n
* @param    TimeDataPtr\n
* @return   \n
* @remark   \n
********************************************************************************
*/
void CtsTimeSync(INT8U *TimeDataPtr)
{
	CLMemCopy((INT8U *)(&TimeDataBuf), TimeDataPtr, sizeof(TIME_DATA));
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

/**
********************************************************************************
* @brief     Cts cal 진입용 Key 암호 해독 함수\n
* @param    
* @return    해독된 Key 값
* @remark  
********************************************************************************
*/
INT32U ctsDecrypt(INT32U key)
{
    INT32U decryptedkey;

    decryptedkey = (key ^ (CtsEncryptBufStruct.time * (*CtsStructPtr).encryptKey1 ) ^ (CtsEncryptBufStruct.zero * K_ZERO) ^ (CtsEncryptBufStruct.span * K_SPAN));
	return decryptedkey;
}

/**
********************************************************************************
* @brief     Cts 암호화용 데이터 버퍼 초기화\n
* @param    
* @return    
* @remark  
********************************************************************************
*/
void ctsEncryptKeyBufInit(void)
{
	CtsEncryptBufStruct.zero = CtsInfoStruct.zero;
	CtsEncryptBufStruct.midUp = CtsInfoStruct.midUp;
	CtsEncryptBufStruct.midDn = CtsInfoStruct.midDn;
	CtsEncryptBufStruct.span = CtsInfoStruct.span;
	CtsEncryptBufStruct.time = (INT32U)((*CtsStructPtr).ctsCalYear) * 0x1000000;
	CtsEncryptBufStruct.time += (INT32U)((*CtsStructPtr).ctsCalMonth) * 0x10000;
	CtsEncryptBufStruct.time += (INT32U)((*CtsStructPtr).ctsCalDate) * 0x100;
}

#ifdef USE_CTS_OMV2
void reverse_array(char *src, int size) { //OMV2 환경에서 엔디언이 달라 사용. CTS의 경우 연동 프로그램에서 엔디언 변경이 아닌 펌웨어에서 엔디안 변경 진행. 
	unsigned char buf[6];
	 int i = 0;
	 for(i=0;i<size;i++)
	 {
		buf[i] = *(src+i);
	 }
	 for(i=0;i<size;i++)
	 {
		 *(src+i) = buf[(size-1)-i];
	 }
}
#endif
#elif defined(USE_CTS_MODE_OMV3)
#include "../DB/StorageApi.h"
#include "../DB/ParamApi.h"
#include "../AD/AdApi.h"
#include "../AD/LinearCompApi.h"
#include "../Time/TimeApi.h"
#include "../SystemTimer/TimerApi.h"
#include "../Common/TimeData.h"
#include "../Common/UiDefine.h"
#include "../UI/DispApi.h"
#include "../Hardware/HardwareConfig.h"
#include "../Power/PowerApi.h"
#include "../UI/BuzzerApi.h"
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

static INT8U ctsModeFlag = 0;
static CAL_HISTORY_STRUCT calHistoryTemp;
static INT8U calWarringFlag = 0;

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

/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */

#define CELLPHONE_CAL_ENTER     0x81
#define TO_CELLPHONE_INFO       0x01
#define TO_DATA_OK              0xE0
#define AUX_PARAM_OFFSET_CTSCHECK       0
#define CTS_DELAY_CHECK_100MS_NUM   5
#define MASK_OMV3_CAL         0x11
#define MASK_OMV3_INFO        0x12
#define MASK_OMV3_PEROD       0x13
#define SERIAL_WRITE          0x14


#define MASK_OMV3_CTSON       0x82
#define MASK_OMV3_CTSOFF      0x83
#define MASK_OMV3_COMP        0x85

#define MASK_OMV3_TIME1        0x70
#define MASK_OMV3_TIME2        0x71

#define ADDR_CELLPHONE_COMM   500
#define COMM_OK             0xF0
#define COMM_FAIL           0xF1


/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/**
********************************************************************************
* @brief   Cts mode init 함수\n
*           cts mode enable disable 을 결정함 \n
* @param    none
* @return   none
* @remark   초기에 실행 되는 함수\n
*           cts를 초기에 활성화 할지 말지를 결정한다. 
********************************************************************************
*/

void CtsModeInit(void)
{
    if (StorageReadByte(ADDR_CELLPHONE_COMM) != COMM_OK)        // 핸드폰 명력어의 주소 초기 셋팅 
        StorageWriteByte(ADDR_CELLPHONE_COMM, COMM_OK);
    
    if (ParamReadByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_CAL_PASSWORD_ENABLE_KEY) == 1)       // cts enable check
        ctsModeFlag = 1;                

    TimerSetDelay100ms(TIMER_DELAY_100MS_CTS_CHECK,CTS_DELAY_CHECK_100MS_NUM);
}


/**
********************************************************************************
* @brief   Cts mode 중 cal 진입 함수\n

* @param    none
* @return   none
* @remark   cal 진입하기전 계속 체크를 해서 상황을 판단한다\n

********************************************************************************
*/

void CtsCalEnter(void)
{
    static INT8U ctsComm;
    INT8U ctsCommCheckFlag = 0;   
    
    if (ctsModeFlag)
    {
        while(1)
        {
            if (TimerCheckDelay100ms(TIMER_DELAY_100MS_CTS_CHECK))
            {
                if (ctsCommCheckFlag == 0)
                {
                    ctsComm = StorageReadByte(ADDR_CELLPHONE_COMM);
                    if (ctsComm != COMM_OK)
                        ctsCommCheckFlag = 1;
                }
                else
                {
                    if (ctsComm == MASK_OMV3_CAL)
                    {
                        StorageWriteByte(ADDR_CELLPHONE_COMM, COMM_OK);     // 메모리에 항상 업데이트 되고 있으니까 바로 ok를 기록 
                        break;
                    }
                    else if (ctsComm == MASK_OMV3_INFO)
                        StorageWriteByte(ADDR_CELLPHONE_COMM, COMM_FAIL);
                    else
                        StorageWriteByte(ADDR_CELLPHONE_COMM, COMM_FAIL);
                    ctsCommCheckFlag = 0;
                }
         
                TimerSetDelay100ms(TIMER_DELAY_100MS_CTS_CHECK, CTS_DELAY_CHECK_100MS_NUM);
            }
            PowerProc();
        }
    }
}

/**
********************************************************************************
* @brief   Cts mode proc 함수\n
*           매번 체크한후 cts의 결고를 결정한다 \n
* @param    none
* @return   none
* @remark   매번 실행되는 함수 \n
********************************************************************************
*/



void CtsModeProc(void)
{
    static INT8U ctsComm;
    static INT8U ctsCommCheckFlag = 0;
    CAL_HISTORY_STRUCT calHistory;
    CAL_HISTORY_STRUCT calHistory_temp;
    INT8U counter;
    INT8U counter_org;
    INT8U counter_temp;
    INT8U counter_temp_org;

    
    if (TimerCheckDelay100ms(TIMER_DELAY_100MS_CTS_CHECK))
    {
        if (ctsCommCheckFlag == 0)
        {
            ctsComm = StorageReadByte(ADDR_CELLPHONE_COMM);
            if (ctsComm != COMM_OK)
                ctsCommCheckFlag = 1;
        }
        else
        {
            if (ctsComm == MASK_OMV3_INFO)
                StorageWriteByte(ADDR_CELLPHONE_COMM, COMM_OK);     // 메모리에 항상 업데이트 되고 있으니까 바로 ok를 기록 
            else
            {
                switch(ctsComm)
                {
                    case MASK_OMV3_PEROD:
                        counter_org = ParamReadByte(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_HISTORY_COUNT);
                        counter = counter_org % CAL_HISTORY_MAX_NUM;
                        ParamReadData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_HISTORY + counter * sizeof(calHistory), (INT8U *)&calHistory, sizeof(calHistory));
                        AdWriteHistoryParam(0, calHistory.calZeroValue, calHistory.calMidUpValue, calHistory.calMidDownValue, calHistory.calSpanValue);
                        ParamWriteByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_VALID_START_WAIT_COUNT,0);
                        ParamWriteByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_CAL_PASSWORD_ENABLE_KEY,1);
                        StorageWriteByte(ADDR_CELLPHONE_COMM, COMM_OK);
                        delay50Clock();
                        delay50Clock();
                        TimeReadData();
                        while(1)
                        {
                            delay50Clock();
                            delay50Clock();
                            delay50Clock();
                            delay50Clock();
                            delay50Clock();
                            delay50Clock();
                            counter_temp_org = ParamReadByte(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_HISTORY_COUNT);
                            counter_temp = counter_temp_org % CAL_HISTORY_MAX_NUM;
                            ParamReadData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_HISTORY + counter_temp * sizeof(calHistory_temp), (INT8U *)&calHistory_temp, sizeof(calHistory_temp));
                            
                            if ((counter_org+1) == counter_temp_org)
                                if (TimeData.year == calHistory_temp.year)
                                    if (TimeData.month == calHistory_temp.month)
                                        if (calHistory.calZeroValue == calHistory_temp.calZeroValue)
                                            if (calHistory.calMidUpValue == calHistory_temp.calMidUpValue)
                                                if (calHistory.calMidDownValue == calHistory_temp.calMidDownValue)
                                                    if (calHistory.calSpanValue == calHistory_temp.calSpanValue)
                                                        break;
                            ParamWriteByte(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_HISTORY_COUNT, counter_org);
                            AdWriteHistoryParam(0, calHistory.calZeroValue, calHistory.calMidUpValue, calHistory.calMidDownValue, calHistory.calSpanValue);
                            ParamWriteByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_VALID_START_WAIT_COUNT,0);
                            ParamWriteByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_CAL_PASSWORD_ENABLE_KEY,1);
                        }
                        break;
                    case SERIAL_WRITE:
                        StorageWriteByte(ADDR_CELLPHONE_COMM, COMM_OK);
                        break;
                    case COMM_FAIL:
                        break;
                    default:  
                        StorageWriteByte(ADDR_CELLPHONE_COMM, COMM_FAIL);
                        break;
                }
            }
            ctsCommCheckFlag = 0;
        }
 
        TimerSetDelay100ms(TIMER_DELAY_100MS_CTS_CHECK, CTS_DELAY_CHECK_100MS_NUM);
    }
}
/**
********************************************************************************
* @brief   Cts cal mode proc 함수\n
*           cal 로 진입한후 매번 체크한후 cts의 결고를 결정한다 \n
* @param    none
* @return   none
* @remark   매번 실행되는 함수 \n
CtsModeProc 모드와 같이 안쓴 이유는 평소에는 시간쓰는것이 존재하면 안된다. cal진입에서만 
시간 셋팅이 이루어져야하기 떄문에 
********************************************************************************
*/



void CtsModeCalProc(void)
{
    static INT8U ctsComm;
    static INT8U ctsCommCheckFlag = 0;
    CAL_HISTORY_STRUCT calHistory;
    CAL_HISTORY_STRUCT calHistory_temp;
    INT8U counter;
    INT8U counter_org;
    INT8U counter_temp;
    INT8U counter_temp_org;
    INT8U testValue = 0;
    INT8U i ;

    if (TimerCheckDelay100ms(TIMER_DELAY_100MS_CTS_CHECK))
    {
        if (ctsCommCheckFlag == 0)
        {
            ctsComm = StorageReadByte(ADDR_CELLPHONE_COMM);
            if (ctsComm != COMM_OK)
                ctsCommCheckFlag = 1;
        }
        else
        {
            switch(ctsComm)
            {
                case MASK_OMV3_INFO:
                    StorageWriteByte(ADDR_CELLPHONE_COMM, COMM_OK);
                    break;
                case SERIAL_WRITE:
                    StorageWriteByte(ADDR_CELLPHONE_COMM, COMM_OK);
                    break;
                case MASK_OMV3_PEROD:
                    counter_org = ParamReadByte(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_HISTORY_COUNT);
                    counter = counter_org % CAL_HISTORY_MAX_NUM;
                    ParamReadData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_HISTORY + counter * sizeof(calHistory), (INT8U *)&calHistory, sizeof(calHistory));
                    AdWriteHistoryParam(0, calHistory.calZeroValue, calHistory.calMidUpValue, calHistory.calMidDownValue, calHistory.calSpanValue);
                    ParamWriteByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_VALID_START_WAIT_COUNT,0);
                    ParamWriteByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_CAL_PASSWORD_ENABLE_KEY,1);
                    StorageWriteByte(ADDR_CELLPHONE_COMM, COMM_OK);
                    delay50Clock();
                    delay50Clock();
                    TimeReadData();
                    while(1)
                   {
                     delay50Clock();
                     delay50Clock();
                     delay50Clock();
                     delay50Clock();
                     delay50Clock();
                     delay50Clock();
                     counter_temp_org = ParamReadByte(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_HISTORY_COUNT);
                      counter_temp = counter_temp_org % CAL_HISTORY_MAX_NUM;
                      ParamReadData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_HISTORY + counter_temp * sizeof(calHistory_temp), (INT8U *)&calHistory_temp, sizeof(calHistory_temp));
                        if ((counter_org+1) == counter_temp_org)
                          if (TimeData.year == calHistory_temp.year)
                            if (TimeData.month == calHistory_temp.month)
                              if (calHistory.calZeroValue == calHistory_temp.calZeroValue)
                                if (calHistory.calMidUpValue == calHistory_temp.calMidUpValue)
                                  if (calHistory.calMidDownValue == calHistory_temp.calMidDownValue)
                                    if (calHistory.calSpanValue == calHistory_temp.calSpanValue)
                                       break;
                        ParamWriteByte(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_HISTORY_COUNT, counter_org);
                        AdWriteHistoryParam(0, calHistory.calZeroValue, calHistory.calMidUpValue, calHistory.calMidDownValue, calHistory.calSpanValue);
                        ParamWriteByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_VALID_START_WAIT_COUNT,0);
                        ParamWriteByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_CAL_PASSWORD_ENABLE_KEY,1);
                    }
                    break;
                case MASK_OMV3_TIME1:
                    CalReadParam();
                    //ParamWriteByte(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_RANGE+16, CalParam.divFactor); // divFactor값을 evalue에 쓰기(개발 초기) 
                    //ParamWriteByte(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_RANGE+17, CalParam.divFactor); // divFactor값을 dvalue에 쓰기(개발 초기) 
                    TimeData.year = ParamReadByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_SCALE_TEST_DATA);
                    TimeData.month = ParamReadByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_SCALE_TEST_DATA+1);
                    TimeData.date = ParamReadByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_SCALE_TEST_DATA+2);
                    TimeData.hour = StorageReadByte(ADDR_CELLPHONE_COMM+1);
                    TimeData.min = StorageReadByte(ADDR_CELLPHONE_COMM+2);
                    TimeData.sec = 0;
                    StorageWriteByte(ADDR_CELLPHONE_COMM, COMM_OK);
                    TimeWriteData();
                    CalReadParam();
                    RangeReadParam();
                    LinearReadParam();
                    break;
                case COMM_FAIL:
                        break;    
                default:
                    StorageWriteByte(ADDR_CELLPHONE_COMM, COMM_FAIL);
                    break;
            }
            ctsCommCheckFlag = 0;
        }
 
        TimerSetDelay100ms(TIMER_DELAY_100MS_CTS_CHECK, CTS_DELAY_CHECK_100MS_NUM);
    }
}

void CalWarringOn(void)
{
    DispSetIndicator(INDICATOR_CTS, ON);
}


void CtsSetDefaultParam(INT8U type)
{
    char modelName[12]={0,0,0,0,0,0,0,0,0,0,0,0};
    ParamWriteByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_VALID_START_WAIT_COUNT,0);
    ParamWriteByte(PARAM_LOG_AREA_NUM, LOG_PARAM_OFFSET_CAL_PASSWORD_ENABLE_KEY,0);
    switch(MODEL_NUM)
    {
        case MODEL_OMV3_SW1SCU_LCD:
            sprintf(modelName,"%s","SW-1s");
            break;
        case MODEL_OMV3_SWLRWR_LED:
            sprintf(modelName,"%s","SW-LR");
            break;
        case MODEL_OMV3_TM_LCD:
            sprintf(modelName,"%s","TM300");
            break;
        case MODEL_OMV3_FW500S_LCD:
            sprintf(modelName,"%s","FW-500C");
            break;
        case MODEL_OMV3_FW500S_LED:
            sprintf(modelName,"%s","FW-500E");
            break;
        case MODEL_OMV3_FW500S_LCD_LB:
            sprintf(modelName,"%s","FW-500C");
            break;
        case MODEL_OMV3_FW500S_LCD_RS232:
            sprintf(modelName,"%s","FW-500C");
            break;
        default:
            break;
    }
    ParamWriteData(PARAM_DISP_AREA_NUM, DISP_PARAM_MODEL_NAME, modelName, DISP_PARAM_MODEL_NAME_SIZE); 
}


void CtsSetModelSerialParam(INT8U type)
{
    char serialNum[12];
    
    
    switch(MODEL_NUM)
    {
        case MODEL_OMV3_SW1SCU_LCD:
            sprintf(serialNum,"%s","12345678");
            break;
        case MODEL_OMV3_SWLRWR_LED:
            sprintf(serialNum,"%s","12345678");
            break;
        case MODEL_OMV3_TM_LCD:
            sprintf(serialNum,"%s","12345678");
            break;
        case MODEL_OMV3_FW500S_LCD:
            sprintf(serialNum,"%s","12345678");
            break;
        case MODEL_OMV3_FW500S_LED:
            sprintf(serialNum,"%s","12345678");
            break;
        case MODEL_OMV3_FW500S_LCD_RS232:
            sprintf(serialNum,"%s","12345678");
            break;
    }
    ParamWriteData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_SERIALNUM_BACKUP, serialNum, sizeof(serialNum)); 
}



/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

#endif