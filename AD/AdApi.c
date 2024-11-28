/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD API Source File\n
* @file    AdApi.c
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
#include "../Common/AdData.h"
#include "../Common/CommonLib.h"
#include "../Common/UiDefine.h"
#include "../Common/TimeData.h"
#include "../UI/DispApi.h"
#include "../UI/KeyApi.h"
#include "../DB/StorageApi.h"
#include "../DB/ParamApi.h"
#include "../SystemTimer/TimerApi.h"
#include "../Communication/DebugApi.h"
#include "../Time/TimeApi.h"
#include "FilterApi.h"
#include "CreepCompApi.h"
#include "TempCompApi.h"
#include "HystCompApi.h"
#include "LinearCompApi.h"
#ifdef USE_CHECK_AD_VENDOR
#include "../Hardware/HardwareConfig.h"
#endif
#include "AdApi.h"

#include "AdDrv.h"
#include "TempDrv.h"
#include "../Common/SaleData.h"
#ifdef USE_CTS_MODE
#include "../Application/CtsModeApp.h"
#endif

/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief KR : 외부에서 사용 가능한 AD data structure \n
		  US : AD data structure for external file use \n
		  CN :									*/
AD_DATA 	AdData;

#ifdef USE_SATO_PRINTER_PROTOCOL
extern INT8U RecordeTareflag;
#endif
#ifdef USE_AD_READ_COMPLETE_CHECK
BOOLEAN AdDataReadCompleteFlag; // AdData read Complete check flag
#endif

/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief KR :adRawProc() 가 수행되는 주기 \n
		  US : adRawProc() Process cycle (Unit : Hz) \n
		  CN :									*/
#ifndef AD_RAW_PROC_RATE
#define AD_RAW_PROC_RATE	8
#endif

/** @brief KR : AD값을 적당한 LEVEL로 맞춰주기 위해 SHIFT하는 BIT 값 	\n
		  US : 												\n
                CN : 												*/


#ifdef AD_DATA_NOTSHIFT               
#define AD_DIVIDING_FACTOR_BIT		0       // duckspg ads1230 에서는 shift를 하지 않는다. 
#else
#ifdef USE_CHECK_AD_VENDOR
    #ifdef AD_DRV_XM24L_3_3V
        #define AD_DIVIDING_FACTOR_BIT              4
    #elif defined AD_DRV_XM24L    //ADC VDD=5V
        #define AD_DIVIDING_FACTOR_BIT              5
    #else //AD_DRV_OB1020
        #define AD_DIVIDING_FACTOR_BIT              5
    #endif
#else 
#define AD_DIVIDING_FACTOR_BIT		5
#endif
#endif




/** @brief KR : AD_DIVIDING_FACTOR_BIT의 실제 값 \n
		  US : 												\n
                CN : 												*/
#define AD_DIVIDING_FACTOR	(0x01 << AD_DIVIDING_FACTOR_BIT)

/** @brief KR : 60000 이하에서 2 normAD값이 2count씩 움직일 수 있음\n
           			방지하기 위해 소수점 아래자리 수를 AD raw Proc로 보냄 \n
           			사용하지 않은 경우 0으로 설정\n
		  US : 												\n
                CN : 												*/

#ifdef AD_DATA_NOTSHIFT
#define AD_UNDER_DEC_PT_BIT			0       
#define AD_UNDER_DEC_PT_BIT_MASK		0x00
#else
#define AD_UNDER_DEC_PT_BIT			2
#define AD_UNDER_DEC_PT_BIT_MASK		0x03
#endif

/** @brief KR : 현재 MAF 버퍼 크기를 증가 시키위한 제한값(미만), Normal AD 기준, 외부 7500 기준\n
		  US : 												\n
                CN : 												*/
#define MAF_WIN_INC_THRESHOLD	(6 * AD_DIVIDING_FACTOR)

/** @brief KR : 현재 MAF 버퍼 크기를 1로 감소 시키위한 제한값(미만), Normal AD 기준, 외부 7500 기준\n
		  US : 												\n
                CN : 												*/
#define MAF_WIN_BREAK_THRESHOLD	(8 * AD_DIVIDING_FACTOR)

/** @brief KR : AD Drvier 정상 유무 판정 기준 시간 (단위 : 100ms)\n
		  US : 												\n
                CN : 												*/
#define AD_MAX_WAIT_100MS_TIME	3//300 ms

/** @brief KR : Raw stable을 체크 하는 범위 \n
		  US : 												\n
                CN : 												*/
#define RAW_STABLE_RANGE			5

/** @brief KR : 초기 영점 잡을 때의 Stable 기준 (단위 : AD sampling 회수) \n
		  US : 												\n
                CN : 												*/
#define AD_INIT_STABLE_CHECK_COUNT	(AD_RAW_PROC_RATE - 1) //7

/** @brief KR : Shock Filter Index 배열 최대값 \n
		  US : 												\n
                CN : 												*/
#define AD_MAF_SHOCK_TABLE_INDEX_MAX	6

/** @brief KR : MAF Filter Index 배열 최대값  \n
		  US : 												\n
                CN : 												*/
#define AD_MAF_SET_TABLE_INDEX_MAX		6

/** @brief KR : 내부 최대값  \n
		  US : 												\n
                CN : 												*/
#define INTERNAL_FULL_VALUE	60000

/** @brief KR : AD Underload 제한값  \n
		  US : 												\n
                CN : 												*/
#define AD_UNDER_LIMIT		20

#ifdef USE_UNDERLOAD_10DIGIT
#define AD_UNDER_10LIMIT		10
#endif

#ifndef AD_FILT_DEFAULT_VALUE
/** @brief KR : FILTER 값이 미리 정의되지 않으면 정해지는 값   \n
		  US : 												\n
                CN : 												*/
#define AD_FILT_DEFAULT_VALUE	11
#endif

#ifdef USE_CAL_SPAN_EXT_LIMIT_PR_PLUS
/** @brief KR : Calibration span limit 값   \n
		  US : 												\n
                CN : 												*/
#define CAL_SPAN_LIMIT_VALUE 900000L
#else
#define CAL_SPAN_LIMIT_VALUE 450000L
#endif

/*
********************************************************************************
*                       LOCAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert local typedef & structure here */
/** @brief KR : Capa data를 사용하기 위한 Structure  \n
		  US : 												\n
                CN : 												*/
typedef _PACKED_DEF struct
{
	/** @brief KR : 사용 Capa  \n
			  US :												\n
			  CN :												*/
	INT16U capa;
	/** @brief KR : 무게 decimal point 변화값 \n
			  US :												\n
			  CN :												*/
	INT8S weightDecPosOffset;
	/** @brief KR : 외부 1눈금 (1d) table index 변화값 \n
			  US :												\n
			  CN :												*/
	INT8S divTableOffset;
} CAPA_DATA;

/** @brief KR : MAF shock remove를 사용하기 위한 Structure \n
		  US :												\n
		  CN :												*/
typedef _PACKED_DEF struct
{
	/** @brief KR : 사용 shock 제거 시간 \n
			  US :												\n
			  CN :												*/
	INT8U shock100msTime;
	/** @brief KR : 사용 shock 제거 Threshold value \n
			  US :												\n
			  CN :												*/
	INT32S shockThres;
} AD_MAF_SHOCK_TABLE;

/** @brief KR : MAF를 사용하기 위한 Structure\n
		  US :												\n
		  CN :												*/
typedef _PACKED_DEF struct 
{
	/** @brief KR : Average 최소 시간 \n
			  US :												\n
			  CN :												*/
	INT8U min100msTime;
	/** @brief KR : Average 최대 시간\n
			  US :												\n
			  CN :												*/
	INT8U max100msTime;
	/** @brief KR : 증가 Threshold \n
			  US :												\n
			  CN :												*/
	INT32S incThres;
	/** @brief KR : Buffer break Threshold \n
			  US : Buffer break Threshold \n
			  CN :												*/
	INT32S breakThres;
} AD_MAF_SET_TABLE;

/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/
/* Insert file scope variable & tables here */
#ifdef USE_UNIT_1_16OZ
/** @brief KR : Capa table max \n
		  US : Capa table max \n
		  CN :												*/
#define CAPA_TABLE_SIZE			7
/** @brief KR : 저울에서 사용되는 MAX CAPA TABLE {KG, LB, OZ, GRAM, TON,1_16OZ}, CAPA추가시 Table에도 추가 요망, check CAPA_TABLE_SIZE \n
		  US : \n
		  CN :												*/
const CAPA_DATA code CapaTable[CAPA_TABLE_SIZE][6] = 
{
	{{2, 0, 0}, {5, 0, 1}, {100, -2, -1}, {2000, -3, 0}, {0, 0, 0}, {1875, -3, 0}},//117.x oz limit 80, d = 1/16, 1/16
	{{5, 0, 0}, {10, 0, 1}, {200, -2, 0}, {5000, -3, 0}, {0, 0, 0}, {3750, -3, 0}},//242.x oz limit 160, d = 1/16, 1/8
    {{6, 0, 0}, {15, 0, 1}, {200, -2, -1},   {6000, -3, 0},  {0, 0, 0}, {3750, -3, 0}},//242.x oz limit 160, d = 1/16, 1/8
	{{10, 0, 0}, {20, 0, 1}, {400, -2, -1},  {10000, -3, 0}, {0, 0, 0}, {7500, -3, 0}},//468.x oz limit 400, d = 1/8, 1/4
    {{15, 0, 0}, {30, 0, 1}, {400, -2, -1},  {15000, -3, 0}, {0, 0, 0}, {7500, -3, 0}},//468.x oz limit 400, d = 1/8, 1/4
	{{20, 0, 0}, {50, 0, 1}, {1000, -2, -1}, {20000, -3, 0}, {0, 0, 0}, {15000, -3, 0}},//937.x oz limit 800, d = 1/4, 1/2
	{{30, 0, 0}, {60, 0, 1}, {1000, -2, -1}, {30000, -3, 0}, {0, 0, 0}, {20000, -3, 0}},//1250 oz limit 1000, d = 1/4, 1/2
};
/** @brief KR : 저울에서 사용되는 Ounce 단위 실제 CAPA TABLE \n
		  US : \n
		  CN :												*/
const INT16U code DispOzCapaTable[CAPA_TABLE_SIZE] =
{
	80,///2 kg
	160,//5 kg
    160,//6 kg
	400,//10 kg
    400,// 15kg
	800, //20 kg
	1000,//30 kg
};
#elif defined (USE_UNIT_TON_NEWTON_DAN_KN)
/** @brief KR : Capa table max \n
		  US : Capa table max \n
		  CN :												*/
#define CAPA_TABLE_SIZE			14
/** @brief KR : 저울에서 사용되는 MAX CAPA TABLE {KG, LB, OZ, GRAM, matric TON, 1_16OZ, short TON, long TON, N(Newton), kN, daN}, CAPA추가시 Table에도 추가 요망, check CAPA_TABLE_SIZE \n
		  US : \n
		  CN :												*/
const CAPA_DATA code CapaTable[CAPA_TABLE_SIZE][11] = 
{
  // KG,          LB,           OZ,        GRAM,      matric TON, 1_16OZ,      short TON, long TON,  N(Newton),      kN,         daN
	{{500, 0, 0}, {1000, -1, -2}, {0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0},   {0, 0, 0}, {0, 0, 0}, {5000, -1, 0},  {5, 2, 0},  {500, 0, 0}},
	{{1000, 0, 0}, {2000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {1, 3, 0}, {0, 0, 0},   {1, 3, 0}, {1, 3, 0}, {10000, -1, 0}, {10, 2, 0}, {1000, 0, 0}},
	{{3000, 0, 0}, {6000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {3, 3, 0}, {0, 0, 0},   {3, 3, 0}, {3, 3, 0}, {30000, -1, 0}, {30, 2, 0}, {3000, 0, 0}},
	{{5000, 0, 0}, {10000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {5, 3, 0}, {0, 0, 0},  {5, 3, 0}, {5, 3, 0}, {50000, -1, 0}, {50, 2, 0}, {5000, 0, 0}},

	{{10000, 0, 0}, {20000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {10, 3, 0}, {0, 0, 0}, {10, 3, 0}, {10, 3, 0}, {0, 0, 0}, {100, 2, 0}, {10000, 0, 0}},
	{{15000, 0, 0}, {30000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {15, 3, 0}, {0, 0, 0}, {15, 3, 0}, {15, 3, 0}, {0, 0, 0}, {150, 2, 0}, {15000, 0, 0}},
    {{20000, 0, 0}, {60000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {20, 3, 0}, {0, 0, 0}, {20, 3, 0}, {20, 3, 0}, {0, 0, 0}, {200, 2, 0}, {20000, 0, 0}},
	{{25000, 0, 0}, {50000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {25, 3, 0}, {0, 0, 0}, {25, 3, 0}, {25, 3, 0}, {0, 0, 0}, {250, 2, 0}, {25000, 0, 0}},
	{{30000, 0, 0}, {60000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {30, 3, 0}, {0, 0, 0}, {30, 3, 0}, {30, 3, 0}, {0, 0, 0}, {300, 2, 0}, {30000, 0, 0}},
	{{50000, 0, 0}, {0, 0, 0},     {0, 0, 0}, {0, 0, 0}, {50, 3, 0}, {0, 0, 0}, {50, 3, 0}, {50, 3, 0}, {0, 0, 0}, {450, 2, 0}, {45000, 0, 0}},
    
    {{0, 0, 0}, {0, 0, 0},     {0, 0, 0}, {0, 0, 0}, {80, 3, 0}, {0, 0, 0}, {80, 3, 0}, {80, 3, 0}, {0, 0, 0}, {720, 2, 0}, {0, 0, 0}},
    {{0, 0, 0}, {0, 0, 0},     {0, 0, 0}, {0, 0, 0}, {100, 3, 0}, {0, 0, 0}, {100, 3, 0}, {100, 3, 0}, {0, 0, 0}, {900, 2, 0}, {0, 0, 0}},
    {{0, 0, 0}, {0, 0, 0},     {0, 0, 0}, {0, 0, 0}, {120, 3, 0}, {0, 0, 0}, {120, 3, 0}, {120, 3, 0}, {0, 0, 0}, {1080, 2, 0}, {0, 0, 0}},
    {{0, 0, 0}, {0, 0, 0},     {0, 0, 0}, {0, 0, 0}, {150, 3, 0}, {0, 0, 0}, {150, 3, 0}, {150, 3, 0}, {0, 0, 0}, {1350, 2, 0}, {0, 0, 0}},
};
/** @brief KR : 저울에서 사용되는 Ounce 단위 실제 CAPA TABLE \n
		  US : \n
		  CN :												*/
const INT16U code DispOzCapaTable[9] =
{
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};

#elif defined (USE_UNIT_KG_ONLY)
/** @brief KR : Capa table max \n
		  US : Capa table max \n
		  CN :												*/
#define CAPA_TABLE_SIZE			14
/** @brief KR : 저울에서 사용되는 MAX CAPA TABLE {KG, LB, OZ, GRAM, matric TON, 1_16OZ, short TON, long TON, N(Newton), kN, daN}, CAPA추가시 Table에도 추가 요망, check CAPA_TABLE_SIZE \n
		  US : \n
		  CN :												*/
const CAPA_DATA code CapaTable[CAPA_TABLE_SIZE][11] = 
{
  // KG,          LB,           OZ,        GRAM,      matric TON, 1_16OZ,      short TON, long TON,  N(Newton),      kN,         daN
	{{500, 0, 0}, {1000, -1, -2}, {0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0},   {0, 0, 0}, {0, 0, 0}, {5000, -1, 0},  {5, 2, 0},  {500, 0, 0}},
	{{1000, 0, 0}, {2000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {1, 3, 0}, {0, 0, 0},   {1, 3, 0}, {1, 3, 0}, {10000, -1, 0}, {10, 2, 0}, {1000, 0, 0}},
	{{3000, 0, 0}, {6000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {3, 3, 0}, {0, 0, 0},   {3, 3, 0}, {3, 3, 0}, {30000, -1, 0}, {30, 2, 0}, {3000, 0, 0}},
	{{5000, 0, 0}, {10000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {5, 3, 0}, {0, 0, 0},  {5, 3, 0}, {5, 3, 0}, {50000, -1, 0}, {50, 2, 0}, {5000, 0, 0}},

	{{10000, 0, 0}, {20000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {10, 3, 0}, {0, 0, 0}, {10, 3, 0}, {10, 3, 0}, {0, 0, 0}, {100, 2, 0}, {10000, 0, 0}},
	{{15000, 0, 0}, {30000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {15, 3, 0}, {0, 0, 0}, {15, 3, 0}, {15, 3, 0}, {0, 0, 0}, {150, 2, 0}, {15000, 0, 0}},
    {{20000, 0, 0}, {60000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {20, 3, 0}, {0, 0, 0}, {20, 3, 0}, {20, 3, 0}, {0, 0, 0}, {200, 2, 0}, {20000, 0, 0}},
	{{25000, 0, 0}, {50000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {25, 3, 0}, {0, 0, 0}, {25, 3, 0}, {25, 3, 0}, {0, 0, 0}, {250, 2, 0}, {25000, 0, 0}},
	{{30000, 0, 0}, {60000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {30, 3, 0}, {0, 0, 0}, {30, 3, 0}, {30, 3, 0}, {0, 0, 0}, {300, 2, 0}, {30000, 0, 0}},
	{{50000, 0, 0}, {0, 0, 0},     {0, 0, 0}, {0, 0, 0}, {50, 3, 0}, {0, 0, 0}, {50, 3, 0}, {50, 3, 0}, {0, 0, 0}, {450, 2, 0}, {45000, 0, 0}},
    
    {{0, 0, 0}, {0, 0, 0},     {0, 0, 0}, {0, 0, 0}, {80, 3, 0}, {0, 0, 0}, {80, 3, 0}, {80, 3, 0}, {0, 0, 0}, {720, 2, 0}, {0, 0, 0}},
    {{0, 0, 0}, {0, 0, 0},     {0, 0, 0}, {0, 0, 0}, {100, 3, 0}, {0, 0, 0}, {100, 3, 0}, {100, 3, 0}, {0, 0, 0}, {900, 2, 0}, {0, 0, 0}},
    {{0, 0, 0}, {0, 0, 0},     {0, 0, 0}, {0, 0, 0}, {120, 3, 0}, {0, 0, 0}, {120, 3, 0}, {120, 3, 0}, {0, 0, 0}, {1080, 2, 0}, {0, 0, 0}},
    {{0, 0, 0}, {0, 0, 0},     {0, 0, 0}, {0, 0, 0}, {150, 3, 0}, {0, 0, 0}, {150, 3, 0}, {150, 3, 0}, {0, 0, 0}, {1350, 2, 0}, {0, 0, 0}},
};
/** @brief KR : 저울에서 사용되는 Ounce 단위 실제 CAPA TABLE \n
		  US : \n
		  CN :												*/
const INT16U code DispOzCapaTable[9] =
{
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};

#elif defined (USE_UNIT_0_1KG_CAPA_TABLE)
/** @brief KR : Capa table max \n
		  US : Capa table max \n
		  CN :												*/
#define CAPA_TABLE_SIZE			13
/** @brief KR : 저울에서 사용되는 MAX CAPA TABLE {KG, LB, OZ, GRAM, TON,1_16OZ}, CAPA추가시 Table에도 추가 요망, check CAPA_TABLE_SIZE \n
		  US : \n
		  CN :												*/
const CAPA_DATA code CapaTable[CAPA_TABLE_SIZE][5] = 
{
	{{10, 0, 0}, {2, 0, 1}, {0, 0, 0}, {1000, -3, 0}, {0, 0, 0}}, 
	{{15, 0, 0}, {3, 0, 1}, {40, -2, -1}, {1500, -3, 0}, {0, 0, 0}},
	{{20, 0, 0}, {5, 0, 1}, {100, -2, -1}, {2000, -3, 0}, {0, 0, 0}},
	{{25, 0, 0}, {5, 0, 1}, {100, -2, -1}, {2500, -3, 0}, {0, 0, 0}},
	{{30, 0, 0}, {6, 0, 1}, {100, -2, -1}, {3000, -3, 0}, {0, 0, 0}},
	{{50, 0, 0}, {10, 0, 1}, {200, -2, -1}, {5000, -3, 0}, {0, 0, 0}},
	{{60, 0, 0}, {15, 0, 1}, {200, -2, -1}, {6000, -3, 0}, {0, 0, 0}},
	{{100, 0, 0}, {20, 0, 1}, {400, -2, -1}, {10000, -3, 0}, {0, 0, 0}},
	{{120, 0, 0}, {25, 0, 1}, {0, 0, 0}, {12000, -3, 0}, {0, 0, 0}},
	{{150, 0, 0}, {30, 0, 1}, {400, -2, -1}, {15000, -3, 0}, {0, 0, 0}},
	{{200, 0, 0}, {50, 0, 1}, {1000, -2, -1}, {20000, -3, 0}, {0, 0, 0}},
	{{250, 0, 0}, {50, 0, 1}, {0, 0, 0}, {25000, -3, 0}, {0, 0, 0}},
	{{300, 0, 0}, {60, 0, 1}, {1000, -2, -1}, {30000, -3, 0}, {0, 0, 0}},
};
/** @brief KR : 저울에서 사용되는 Ounce 단위 실제 CAPA TABLE \n
		  US : \n
		  CN :												*/
const INT16U code DispOzCapaTable[CAPA_TABLE_SIZE] =
{
	0, ////1 kg 
	40, ////1.5 kg 
	80,///2 kg
	80,///2.5 kg
	100,///3 kg
	160,//5 kg
	160, //6 kg
	400,//10 kg
	0, //12 kg
	400, //15 kg
	800, //20 kg
	0, //25 kg
	1000,//30 kg
};

#elif defined (USE_VISS_UNIT_CHANGE)
/** @brief KR : Capa table max \n
		  US : Capa table max \n
		  CN :												*/
#define CAPA_TABLE_SIZE			8
/** @brief KR : 저울에서 사용되는 MAX CAPA TABLE {KG, LB, OZ, GRAM, TON,1_16OZ}, CAPA추가시 Table에도 추가 요망, check CAPA_TABLE_SIZE \n
		  US : \n
		  CN :												*/
const CAPA_DATA code CapaTable[CAPA_TABLE_SIZE][12] = 
{
	   // KG,	    LB, 		 OZ,   	       GRAM                                                                                      VISS
	{{2, 0, 0}, {5, 0, 1}, {100, -2, -1}, {2000, -3, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {3, 0, 0}},
	{{3, 0, 0}, {6, 0, 1}, {100, -2, -1}, {3000, -3, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {5, 0, 1}},
	{{5, 0, 0}, {10, 0, 1}, {200, -2, 0}, {5000, -3, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {8, 0, 1}},
    {{6, 0, 0}, {15, 0, 1}, {200, -2, -1}, {6000, -3, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {10, 0, 1}},
	{{10, 0, 0}, {20, 0, 1}, {400, -2, -1}, {10000, -3, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {15, 0, 0}},
    {{15, 0, 0}, {30, 0, 1}, {400, -2, -1}, {15000, -3, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {25, 0, 1}},
	{{20, 0, 0}, {50, 0, 1}, {1000, -2, -1}, {20000, -3, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {30, 0, 0}},
	{{30, 0, 0}, {60, 0, 1}, {1000, -2, -1}, {30000, -3, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {50, 0, 1}},
};

#elif defined (USE_PDN_USA_SETTING)
/** @brief KR : Capa table max \n
		  US : Capa table max \n
		  CN :												*/
#ifdef	USE_32KG_CAPA
#define CAPA_TABLE_SIZE			33
#else
#define CAPA_TABLE_SIZE			32
#endif
/** @brief KR : 저울에서 사용되는 MAX CAPA TABLE {KG, LB, OZ, GRAM, TON,1_16OZ}, CAPA추가시 Table에도 추가 요망, check CAPA_TABLE_SIZE \n
		  US : \n
		  CN :												*/
const CAPA_DATA code CapaTable[CAPA_TABLE_SIZE][5] = 
{
	{{1, 0, 0}, {2, 0, 1}, {0, 0, 0}, {1000, -3, 0}, {0, 0, 0}}, 
	{{2, 0, 0}, {5, 0, 1}, {100, -2, -1}, {2000, -3, 0}, {0, 0, 0}},
	{{3, 0, 0}, {6, 0, 1}, {100, -2, -1}, {3000, -3, 0}, {0, 0, 0}},
	{{5, 0, 0}, {10, 0, 1}, {200, -2, -1}, {5000, -3, 0}, {0, 0, 0}},

	{{6, 0, 0}, {12, 0, 1}, {200, -2, -1}, {6000, -3, 0}, {0, 0, 0}},
	{{10, 0, 0}, {20, 0, 1}, {400, -2, -1}, {10000, -3, 0}, {0, 0, 0}},
	{{12, 0, 0}, {30, 0, 1}, {400, -2, -1}, {12000, -3, 0}, {0, 0, 0}},
	{{15, 0, 0}, {30, 0, 1}, {400, -2, -1}, {15000, -3, 0}, {0, 0, 0}},
	{{20, 0, 0}, {50, 0, 1}, {1000, -2, -1}, {20000, -3, 0}, {0, 0, 0}},
	{{25, 0, 0}, {50, 0, 1}, {0, 0, 0}, {25000, -3, 0}, {0, 0, 0}},
	{{30, 0, 0}, {60, 0, 1}, {1000, -2, -1}, {30000, -3, 0}, {0, 0, 0}},
#ifdef	USE_32KG_CAPA
	{{32, 0, 0}, {60, 0, 1}, {1000, -2, -1}, {30000, -3, 0}, {0, 0, 0}},
#endif
	{{50, 0, 0}, {100, 0, 1}, {0, 0, 0}, {50000, -3, 0}, {0, 0, 0}},
	{{60, 0, 0}, {150, 0, 1}, {2400, -2, -1}, {60000, -3, 0}, {0, 0, 0}},

	{{100, 0, 0}, {200, 0, 1}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
	{{150, 0, 0}, {300, 0, 1}, {4800, -2, -1}, {0, 0, 0}, {0, 0, 0}},
	{{200, 0, 0}, {500, 0, 1}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
	{{300, 0, 0}, {600, 0, 1}, {0, 0, 0}, {300000, -3, 0}, {0, 0, 0}},
	{{500, 0, 0}, {1000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
	{{600, 0, 0}, {1500, 0, 1}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},							     
	
	#ifdef USE_UNIT_KG_LB_DEC_CHANGE_CAPA_TABLE							     
	{{1000, 0, 0}, {2000, -1, -2}, {0, 0, 0}, {0, 0, 0}, {1, 3, 0}},
	{{1500, 0, 0}, {3000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {1, 3, 0}},
	{{2000, 0, 0}, {5000, -1, -2}, {0, 0, 0}, {0, 0, 0}, {2, 3, 0}},
    #else
    {{1000, 0, 0}, {2000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {1, 3, 0}},
	{{1500, 0, 0}, {3000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {1, 3, 0}},
	{{2000, 0, 0}, {5000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {2, 3, 0}},
    #endif
	{{3000, 0, 0}, {6000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {3, 3, 0}},
	{{4000, 0, 0}, {8000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {4, 3, 0}},
	{{5000, 0, 0}, {10000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {5, 3, 0}},

	{{10000, 0, 0}, {20000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {10, 3, 0}},
	{{15000, 0, 0}, {30000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {15, 3, 0}},
	{{20000, 0, 0}, {40000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {20, 3, 0}},
	{{30000, 0, 0}, {60000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {30, 3, 0}},
	{{40000, 0, 0}, {80000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {40, 3, 0}},
	{{50000, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {50, 3, 0}},
	{{60000, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {60, 3, 0}},
};
/** @brief KR : 저울에서 사용되는 Ounce 단위 실제 CAPA TABLE \n
		  US : \n
		  CN :												*/
const INT16U code DispOzCapaTable[15] =
{
	0, ////1 kg 
	80,///2 kg
	100,	///3 kg
	160,//5 kg
	200, //6 kg
	400,//10 kg
	400, //12 kg
	400, //15 kg
	800, //20 kg
	0, //25 kg
	1000,//30 kg
	0,//50 kg
	2400,//60 kg
	0,//100 kg
	4800//150 kg
};

#else
/** @brief KR : Capa table max \n
		  US : Capa table max \n
		  CN :												*/
#ifdef	USE_32KG_CAPA
#define CAPA_TABLE_SIZE			33
#else
#define CAPA_TABLE_SIZE			32
#endif
/** @brief KR : 저울에서 사용되는 MAX CAPA TABLE {KG, LB, OZ, GRAM, TON,1_16OZ}, CAPA추가시 Table에도 추가 요망, check CAPA_TABLE_SIZE \n
		  US : \n
		  CN :												*/
const CAPA_DATA code CapaTable[CAPA_TABLE_SIZE][5] = 
{
	{{1, 0, 0}, {2, 0, 1}, {0, 0, 0}, {1000, -3, 0}, {0, 0, 0}}, 
	{{2, 0, 0}, {5, 0, 1}, {100, -2, -1}, {2000, -3, 0}, {0, 0, 0}},
	{{3, 0, 0}, {6, 0, 1}, {100, -2, -1}, {3000, -3, 0}, {0, 0, 0}},
	{{5, 0, 0}, {10, 0, 1}, {200, -2, -1}, {5000, -3, 0}, {0, 0, 0}},

	{{6, 0, 0}, {15, 0, 1}, {200, -2, -1}, {6000, -3, 0}, {0, 0, 0}},
	{{10, 0, 0}, {20, 0, 1}, {400, -2, -1}, {10000, -3, 0}, {0, 0, 0}},
	{{12, 0, 0}, {25, 0, 1}, {0, 0, 0}, {12000, -3, 0}, {0, 0, 0}},
	{{15, 0, 0}, {30, 0, 1}, {400, -2, -1}, {15000, -3, 0}, {0, 0, 0}},
	{{20, 0, 0}, {50, 0, 1}, {1000, -2, -1}, {20000, -3, 0}, {0, 0, 0}},
	{{25, 0, 0}, {50, 0, 1}, {0, 0, 0}, {25000, -3, 0}, {0, 0, 0}},
	{{30, 0, 0}, {60, 0, 1}, {1000, -2, -1}, {30000, -3, 0}, {0, 0, 0}},
#ifdef	USE_32KG_CAPA
	{{32, 0, 0}, {60, 0, 1}, {1000, -2, -1}, {30000, -3, 0}, {0, 0, 0}},
#endif
	{{50, 0, 0}, {100, 0, 1}, {0, 0, 0}, {50000, -3, 0}, {0, 0, 0}},
	{{60, 0, 0}, {150, 0, 1}, {2400, -2, -1}, {60000, -3, 0}, {0, 0, 0}},

	{{100, 0, 0}, {200, 0, 1}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
	{{150, 0, 0}, {300, 0, 1}, {4800, -2, -1}, {0, 0, 0}, {0, 0, 0}},
	{{200, 0, 0}, {500, 0, 1}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
	{{300, 0, 0}, {600, 0, 1}, {0, 0, 0}, {300000, -3, 0}, {0, 0, 0}},
	{{500, 0, 0}, {1000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
	{{600, 0, 0}, {1500, 0, 1}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},							     
	
	#ifdef USE_UNIT_KG_LB_DEC_CHANGE_CAPA_TABLE							     
	{{1000, 0, 0}, {2000, -1, -2}, {0, 0, 0}, {0, 0, 0}, {1, 3, 0}},
	{{1500, 0, 0}, {3000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {1, 3, 0}},
	{{2000, 0, 0}, {5000, -1, -2}, {0, 0, 0}, {0, 0, 0}, {2, 3, 0}},
    #else
    {{1000, 0, 0}, {2000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {1, 3, 0}},
	{{1500, 0, 0}, {3000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {1, 3, 0}},
	{{2000, 0, 0}, {5000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {2, 3, 0}},
    #endif
	{{3000, 0, 0}, {6000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {3, 3, 0}},
	{{4000, 0, 0}, {8000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {4, 3, 0}},
	{{5000, 0, 0}, {10000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {5, 3, 0}},

	{{10000, 0, 0}, {20000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {10, 3, 0}},
	{{15000, 0, 0}, {30000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {15, 3, 0}},
	{{20000, 0, 0}, {40000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {20, 3, 0}},
	{{30000, 0, 0}, {60000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {30, 3, 0}},
	{{40000, 0, 0}, {80000, 0, 1}, {0, 0, 0}, {0, 0, 0}, {40, 3, 0}},
	{{50000, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {50, 3, 0}},
	{{60000, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {60, 3, 0}},
};
/** @brief KR : 저울에서 사용되는 Ounce 단위 실제 CAPA TABLE \n
		  US : \n
		  CN :												*/
const INT16U code DispOzCapaTable[15] =
{
	0, ////1 kg 
	80,///2 kg
	100,	///3 kg
	160,//5 kg
	160, //6 kg
	400,//10 kg
	0, //12 kg
	400, //15 kg
	800, //20 kg
	0, //25 kg
	1000,//30 kg
	0,//50 kg
	2400,//60 kg
	0,//100 kg
	4800//150 kg
};
#endif

/** @brief KR : Division table max\n
		  US : Division table max\n
		  CN :												*/
#define DIV_TABLE_SIZE				6

/** @brief KR : division table (1d)\n
		  US : division table (1d)\n
		  CN :												*/
const INT16U code divTable[DIV_TABLE_SIZE] = {1, 2, 5, 10, 20, 50};
#ifdef USE_UNIT_1_16OZ
/** @brief KR : division table 1/16 oz (1d)\n
		  US : division table 1/16 oz (1d)\n
		  CN :												*/
const INT16U code divTable16oz[DIV_TABLE_SIZE] = {1, 2, 4, 8, 16, 32};
#endif
/** @brief KR : AD 값을 넣을 Ring Buffer \n
		  US :  \n
		  CN :												*/
static RING_BUF *adRingBufPtr;

/** @brief KR : AD Driver의 sampling rate (Hz) for filter \n
		  US : AD Driver's sampling rate (Hz) for filter \n
		  CN :								 				*/
static INT16U adDrvSamplingRate;

/** @brief KR : AD Raw Procedure의 sampling rate (Hz) for weight \n
		  US : AD Raw Procedure's sampling rate (Hz) for weight \n
		  CN :								 				*/
static INT16U adRawProcSamplingRate;

/** @brief KR : 0.5d \n
		  US : 0.5d \n
		  CN : 0.5d*/
static INT16U d0_5;

/** @brief KR : Initial Zero 여부 확인 플래그 \n
		  US :  \n
		  CN :												*/
static BOOLEAN flagInitZero;

/** @brief KR : 현재 사용 단위 \n
		  US :  \n
		  CN :												*/
static INT8U currUnit;

/** @brief KR : Capa 설정 인덱스\n
		  US :  \n
		  CN :												*/
static INT8U capaIndex;

/** @brief KR : 현재 Display Capa (계산하기위한 값)\n
		  US :  \n
		  CN :												*/
static INT32S currDispCapaForCalc;

/** @brief KR : 현재 Display Capa (표시하기위한 값)\n
		  US :  \n
		  CN :												*/
static INT32S currDispCapaForDisp;

/** @brief KR : dual interval이 적용되는 지점\n
		  US :  \n
		  CN :												*/
static INT32S dualThreshold;

/** @brief KR : dual threshold 이상에 적용되는 1d값 \n
		  US :  \n
		  CN :												*/
static INT16U divHigh;

/** @brief KR : dual threshold 이하에 적용되는 1d값 \n
		  US :  \n
		  CN :												*/
static INT16U divLow;

/** @brief KR : division table중에서 Cal Unit에 적용된 배열 번호 \n
		  US :  \n
		  CN :												*/
static INT8U divTableIndex;

/** @brief KR : 최초 제로 값 \n
		  US :  \n
		  CN :												*/
static INT32S initZeroBufCalUnit;

/** @brief KR : 제로 버퍼값 \n
		  US :  \n
		  CN :												*/
static INT16S zeroBufCalUnit;

/** @brief KR : 최초 제로 값 \n
		  US :  \n
		  CN :												*/
static INT32S initZeroBuf;

/** @brief KR : 제로 버퍼값 \n
		  US :  \n
		  CN :												*/
static INT16S zeroBuf;

/** @brief KR : Rezero(Zero tracking) limit \n
		  US :  \n
		  CN :												*/
static INT32S rezeroLimit; 

/** @brief KR : Zero tracking 동작 범위 \n
		  US :  \n
		  CN :												*/
static INT16U zeroTrackingRange;

/** @brief KR : Zero flag 동작 범위 \n
		  US :  \n
		  CN :												*/
static INT16U zeroCheckingRange;

#ifdef USE_CHANGE_ZERO_RANGE_SETTING
/** @brief KR : 초기 (+)Zero 범위 \n
		  US :  \n
		  CN :												*/
static INT32S initialZeroRangePlus;

/** @brief KR : 초기 (-)Zero 범위 \n
		  US :  \n
		  CN :												*/
static INT32S initialZeroRangeMinus;
#else
/** @brief KR : 초기 Zero 범위 \n
		  US :  \n
		  CN :												*/
static INT32S initialZeroRange;
#endif
/** @brief KR : Zero tracking 진입 카운트\n
		  US :  \n
		  CN :												*/
static INT8U zeroTrackingEnterCount;

/** @brief KR : Tare 버퍼 데이터\n
		  US :  \n
		  CN :												*/
static INT32S tareBuf;

/** @brief KR : Tare 버퍼 데이터\n
		  US :  \n
		  CN :												*/
static INT32S tareBufCalUnit;

/** @brief KR : 현재 Tare 타입\n
		  US :  \n
		  CN :												*/
static INT8U tareType;

/** @brief KR : Percent Tare 값\n
		  US :  \n
		  CN :												*/
static INT8U tarePercent;

/** @brief KR : Percent Tare 구해진값\n
		  US :  \n
		  CN :												*/
static INT32S tareBufPercent;

/** @brief KR : Percent Tare 구해진값\n
		  US :  \n
		  CN :												*/
static INT32S tareBufPercentCalUnit;

/** @brief KR : 현재 tare limit 값 (이하 허용) - using unit 기준 normalized AD\n
		  US :  \n
		  CN :												*/
static INT32S currTareLimit;

/** @brief KR : tare 제거 범위 (미만 허용)\n
		  US :  \n
		  CN :												*/
static INT16U tareRemovableRange;

/** @brief KR : 내부분해능값으로 변환 하기 위한 기울기 변수\n
		  US :  \n
		  CN :												*/
static FP32 convSlope;

/** @brief KR : Weight unit이 변한것을 알려주는 Flag\n
		  US :  \n
		  CN :												*/
static INT8U adChangeWeightUnitFlag;

/** @brief KR : increase Overload count flag\n
		  US :  \n
		  CN :												*/
static INT8U overloadCountIncFlag;

#ifdef USE_CHECK_AD_VENDOR
/** @brief KR : ad 벤더를 체크하는 변수 \n
		  US :  \n
		  CN :												*/
static INT8U adVendorSelect;
#endif
#ifdef DEBUG_AD_API
/** @brief KR : AD Debug flag\n
		  US :  AD Debug flag\n
		  CN :												*/
static BOOLEAN AdDebugFlag;

/** @brief KR : AD Debug data\n
		  US :  AD Debug data\n
		  CN :												*/
static INT32S AdDebugData;
#endif

/** @brief KR : 충격 필터 인덱스\n
		  US :  \n
		  CN :												*/
static INT8U adMAFshockIndex;

/** @brief KR : 흔들림 필터 인덱스\n
		  US :  \n
		  CN :												*/
static INT8U adMAFsetIndex;

/** @brief KR : overload된 무게 최대값(내부) \n
		  US :  \n
		  CN :												*/
static INT32U overloadMaxValue;

/** @brief KR : 충격 필터 인덱스 테이블\n
		  US :  \n
		  CN :			
*/
#ifdef USE_UNDERLOAD_10DIGIT
INT8U UnderLoad10dEnable;
#endif

const AD_MAF_SHOCK_TABLE code adMAFshockTable[AD_MAF_SHOCK_TABLE_INDEX_MAX] = 
{
	{ 0, 60000 * MAF_WIN_BREAK_THRESHOLD},		//Shock OFF
	{ 2, 2 * MAF_WIN_BREAK_THRESHOLD},			//0.2 sec
	{ 3, 2 * MAF_WIN_BREAK_THRESHOLD},			//0.3 sec
	{ 4, 2 * MAF_WIN_BREAK_THRESHOLD},			//0.4 sec
	{ 5, 2 * MAF_WIN_BREAK_THRESHOLD},			//0.5 sec
	{ 5, 3 * MAF_WIN_BREAK_THRESHOLD},			//0.5 sec, large range
};

/** @brief KR : 흔들림 필터 인덱스 테이블\n
		  US :  \n
		  CN :												*/
const AD_MAF_SET_TABLE code adMAFsetTable[AD_MAF_SET_TABLE_INDEX_MAX] = 
{
	{ 4, 25, MAF_WIN_INC_THRESHOLD, MAF_WIN_BREAK_THRESHOLD}, 			//0.4 sec, 
	{ 5, 25, MAF_WIN_INC_THRESHOLD, MAF_WIN_BREAK_THRESHOLD}, 			//0.5 sec, 
	{ 6, 25, MAF_WIN_INC_THRESHOLD, MAF_WIN_BREAK_THRESHOLD}, 			//0.6 sec, 
	{ 8, 30, MAF_WIN_INC_THRESHOLD, MAF_WIN_BREAK_THRESHOLD}, 			//0.8 sec, 
	{ 10, 30, MAF_WIN_INC_THRESHOLD, MAF_WIN_BREAK_THRESHOLD},			// 1 sec, 
	{ 10, 30, 2*MAF_WIN_INC_THRESHOLD, 2*MAF_WIN_BREAK_THRESHOLD},		// 1 sec, large range
};

/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
static void adInitVariable(void);
static void adFiltInit(void);
static void adCheckInitZeroProc(void);
static void adSetIndicator(void);
extern INT32S adFilterProc(INT32S lAd);
static void adRawProc(INT32S rawFilteredData);
static void adCalcConvSlope(void);
static FP32 adConvInternalUnit(FP32 fAd, INT8U unit1, INT8U unit2);
static FP32 adNomalization(FP32 unitFactorizedData);
static FP32 adRawInternalization(FP32 normalizedData);
static void adZeroTracking(INT32S dispFilteredData);
static INT32S adIntToExtValue(INT32S interVal);
static INT32S adExtToIntValue(INT32S exterVal);
static BOOLEAN adChecklastDigitIsInvalid(INT32S weightVal);
static BOOLEAN adCheckGrossZero(INT32S setZeroData);
static BOOLEAN adCheckNetZero(INT32S setTareData);
static BOOLEAN adCheckOverload(INT32S setTareData);
static BOOLEAN adCheckUnderload(INT32S setTareData);
static INT32S adPercentTareProc(INT32S setTareData);

static void adTareChangeLimit(INT8U currUnit);
static BOOLEAN adTareCheckCondition(INT32S currTare, INT32S newTare);

static BOOLEAN adCheckRawStable(INT32S rawAd);
static INT32S adDispFiltAndCheckStable(INT32S rawInternalData);

static BOOLEAN adSetWeightUnit(INT8U weightUnit);

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
/**
********************************************************************************
* @brief    AD API Init Parameter function\n
* @param    type : defualt type
* @return   none
* @remark   KR : Parameter 영역에 초기값을 넣는다.\n
*                       Cal Mode나 Program 모드에서 전체 초기화할 때 실행한다.
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void AdSetDefaultParam(INT8U type)
{
	type = 1;			// dummy value for no waring
	HystSetDefaultParam();
#if defined(USE_TEMP_COMP) || defined(USE_TEMP_COMP_MWIIH)
	TempSetDefaultParam();
#endif
#ifdef USE_CREEP_COMP
	CreepSetDefaultParam();
#endif
	LinearSetDefaultParam();
	CalSetDefaultParam();
	RangeSetDefaultParam();
	DispFiltSetDefaultParam();

	ParamWriteByte(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_OVERLOAD_COUNT, 0);
	ParamWriteDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_OVERLOAD_VALUE, 0);

	ParamWriteByte(PARAM_AD_AREA_NUM,AD_PARAM_OFFSET_INIT_SIGNAL,INIT_EEP_COMPLETE);
	ParamWriteByte(PARAM_AD_AREA_NUM,AD_PARAM_OFFSET_INIT_VER,INIT_EEP_VER);
}

#ifdef USE_CHECK_AD_VENDOR
/**
********************************************************************************
* @brief    Check AD Vendor.\n
* @param    none
* @return   OB1020: 0, XM24L: 1
* @remark   
********************************************************************************
*/
INT8U checkAdVendor()
{
	INT8U vendor;

	vendor = AD_VENDOR_OB1020;

	if(PCB_AD_CHK) vendor = AD_VENDOR_OB1020;
	else vendor = AD_VENDOR_XM24L;

	return  vendor;
}
#endif

/**
********************************************************************************
* @brief    Initialize AD API.\n
*           1. Init AD Driver\n
*           2. Get AD Ring Buffer
* @param    none
* @return   none
* @remark   KR : Run Initail Time \n
*                 US : Run Initail Time \n
*                 CN : \n
********************************************************************************
*/
void AdInit(void)
{
#ifdef DEBUG_AD_API
	AdDebugFlag = 0;
#endif
	overloadCountIncFlag = 0;
	overloadMaxValue = 0;

#ifdef USE_CHECK_AD_VENDOR
		adVendorSelect = checkAdVendor();

		if(adVendorSelect == AD_VENDOR_OB1020)
        {
            AdDrvInit();
            adRingBufPtr = &AdDrvRingBuf;
            adDrvSamplingRate = AdDrvGetSamplingRate();
        }
        else if(adVendorSelect == AD_VENDOR_XM24L)
        {
            XM24L_AdDrvInit();
            adRingBufPtr = &XM24L_AdDrvRingBuf;
            adDrvSamplingRate = XM24L_AdDrvGetSamplingRate();
        }
#else
	AdDrvInit();
	adRingBufPtr = &AdDrvRingBuf;
	adDrvSamplingRate = AdDrvGetSamplingRate();
#endif

	adInitVariable();
#if defined(USE_TEMP_COMP) || defined(USE_TEMP_COMP_MWIIH)
	if (TempDrvInit() == 0)
	{
		TempParam.tempCompEnable = 0;
	}

	TempInitVariable();
#endif
}

/**
********************************************************************************
* @brief    Initial zero reset\n
* @param  none
* @return   none
* @remark
********************************************************************************
*/

void AdInitZeroReset(void)
{
	INT32S tempInt32s;
	
	if (AdData.stableFlag)	
	{
		if (CalParam.initZeroResetEnable)
		{
			tempInt32s = CalParam.calZeroValue;
			CalParam.calZeroValue = AdData.rawFilteredData;
			tempInt32s = AdData.rawFilteredData - tempInt32s;
			CalParam.calSpanValue +=  tempInt32s;
			CalParam.initZeroResetEnable = 0;
			CalWriteParam();
			AdWriteHistoryParam(10, CalParam.calZeroValue, 0, 0, CalParam.calSpanValue);
			adInitVariable();
		}
	}
}

#ifdef DEBUG_AD_API
/**
********************************************************************************
* @brief       KR : Ad 값을 설정하는 함수 (for Debug)\n
*                 US : \n
*                 CN : \n
* @param    KR : lAd : 내부값\n
*                 US : \n
*                 CN : \n
* @return   none
* @remark   
********************************************************************************
*/
void AdWriteDataForDebug(INT32S lAd)
{
	if (lAd == 0)
	{
		AdDebugFlag = 0;//off
	}	
	else
	{
		AdDebugFlag = 1;
		AdDebugData = lAd;
		AdData.rawFilteredData = AdDebugData;
	}
}
#endif

/*
********************************************************************************
* @brief    AD API Procedure\n
* @param    none
* @return   none
* @remark   KR : 일정 시간에 한번씩 계속 실행 되는 함수\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void AdProc(INT8U checkInitZero, BOOLEAN calFlag)
{
	static INT16U prevAdReadTimer100ms = 0;
	INT16U currTimer100ms;
	data INT32S lAd;
	static INT8U prevAdRawProcCount = 0;
	static INT8U adFiltProcCount = 0;
	INT8U adRawProcCount;
	INT8U tempInt8u;
	INT32U tempInt32u;
	
//	char dbgStr[20];

	currTimer100ms = TimerGetSysTimer100ms();
#ifdef USE_CHECK_AD_VENDOR
	if(adVendorSelect == AD_VENDOR_OB1020) AdDrvProc();
	else if (adVendorSelect == AD_VENDOR_XM24L)  XM24L_AdDrvProc();
#else
	AdDrvProc();
#endif	

	if (RingBufCheckBuf(adRingBufPtr))
	{
		prevAdReadTimer100ms = currTimer100ms;

		//--------------------------------------//
		// temperature sensor porcedure
		//--------------------------------------//
#if defined(USE_TEMP_COMP) || defined(USE_TEMP_COMP_MWIIH)
		TempDrvProc(currTimer100ms);
#endif
		//--------------------------------------//
		// copy raw data for Debug
		//--------------------------------------//
		lAd = RingBufGetInt32s(adRingBufPtr);
		AdData.rawData = lAd >> AD_DIVIDING_FACTOR_BIT;

		//--------------------------------------//
		// Filtering
		//--------------------------------------//
		lAd = adFilterProc(lAd);

		//--------------------------------------//
		// Check AD raw Procedure Entering time
		// for low speed CPU
		//--------------------------------------//
		adRawProcCount = (INT8U)(((INT16U)adFiltProcCount * adRawProcSamplingRate) / adDrvSamplingRate);
		
		adFiltProcCount++;
		if (adFiltProcCount >= adDrvSamplingRate) adFiltProcCount = 0; 

		if (adRawProcCount == prevAdRawProcCount) {
			return;
		}
		prevAdRawProcCount = adRawProcCount;

		//--------------------------------------//
		// Change Weight Unit
		//--------------------------------------//
		if (adChangeWeightUnitFlag & 0x80)
		{
			adSetWeightUnit(adChangeWeightUnitFlag & 0x7F);
			ParamWriteByte(PARAM_AD_AREA_NUM, AD_PARAM_USING_UNIT_OFFSET, CalParam.usingUnit);
			adChangeWeightUnitFlag = 0;
		}

		//--------------------------------------//
		// shrink to (24-AD_DIVIDING_FACTOR_BIT)
		//--------------------------------------//
		lAd = lAd >> (AD_DIVIDING_FACTOR_BIT - AD_UNDER_DEC_PT_BIT);

		//--------------------------------------//
		// make weight
		//--------------------------------------//
		adRawProc(lAd);
#ifdef USE_AD_READ_COMPLETE_CHECK
    	AdDataReadCompleteFlag = TRUE;
#endif
		//--------------------------------------//
		// Check underload
		//--------------------------------------//		
#ifndef USE_NOT_UNDERLOAD_ERROR
		if ((AdData.underloadFlag == 1) && (calFlag == 0) && (flagInitZero == 1))		 
		{
			DispSetConstStrMsg(WIN_WEIGHT, "------");		//-20d 이하일 경우 Underload 표시 (OIML 2006) 
			DispSetRunMsgPage(10, 1);	
			KeyEnable(ON);		//key 동작 허용
		}
#endif
		//--------------------------------------//
		// Check overload
		//--------------------------------------//
		if ((AdData.overloadFlag == 1) && (calFlag == 0)) 
		{
			DispSetError(ERROR_NUM_OVERLOAD);
			if (overloadMaxValue < AdData.weightData) 
			{
				overloadMaxValue = AdData.weightData;
			}
			overloadCountIncFlag = 1;
		}

		if(AdData.overloadFlag == 0)
		{
			if(overloadCountIncFlag == 1)
			{
				tempInt8u = ParamReadByte(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_OVERLOAD_COUNT);
				if (tempInt8u == 0xFF) tempInt8u = 0;//if eeprom is not init //ver < v2.14 
				tempInt8u++;
				ParamWriteByte(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_OVERLOAD_COUNT, tempInt8u);
				tempInt32u = ParamReadDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_OVERLOAD_VALUE);
				if (tempInt32u == 0xFFFFFFFF) tempInt32u = 0;//if eeprom is not init //ver < v2.14 
				if (tempInt32u < overloadMaxValue) 
				{
					ParamWriteDword(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_OVERLOAD_VALUE, overloadMaxValue);
				}
				overloadMaxValue = 0;
				overloadCountIncFlag = 0;
			}
		}

		//--------------------------------------//
		// Check Initial zero range
		//--------------------------------------//
		if (checkInitZero)// intial zero 통과 여부 확인 플래그
		{
#ifdef USE_CONTINUE_INIT_ZERO_ERROR
			if (!AdData.initZeroError) //영점통과에러 경우 AD 상태 체크 X (Error 표시 유지)
			{
				adCheckInitZeroProc();
			}
#else
			adCheckInitZeroProc();
#endif
		}

		//--------------------------------------//
		// display
		//--------------------------------------//
		if (flagInitZero) 
		{
			adSetIndicator();
		}
		else
		{
			AdData.weightData = 0;
		}

#ifdef USE_FAST_DISPLAYING_WEIGHT
		DispImmedietly();
#endif
	}

	if ((INT16U)(currTimer100ms - prevAdReadTimer100ms) > AD_MAX_WAIT_100MS_TIME)
	{
		prevAdReadTimer100ms = currTimer100ms;
#ifdef USE_CHECK_AD_VENDOR
        if(adVendorSelect == AD_VENDOR_OB1020) AdDrvInit();
        else if (adVendorSelect == AD_VENDOR_XM24L) XM24L_AdDrvInit();
#else
		AdDrvInit();
#endif
	}
}

/**
********************************************************************************
* @brief       KR : Initial Zero 통과를 검사하는 함수\n
*                 US : \n
*                 CN : \n
* @param    none
* @return     KR : Initial Zero 통과 시 '1' 미 통과 시 '0'\n
*                 US : \n
*                 CN : \n
* @remark   
********************************************************************************
*/
BOOLEAN AdCheckInitZero(void)
{
	return flagInitZero;	
}

/**
********************************************************************************
* @brief       KR : 현재 AD 값을 Rezero하는 함수, Zero Key를 입력 받을 경우 실행함\n
*                 US : \n
*                 CN : \n
* @param    none
* @return     KR : Rezero 범위 초과 시 Fail '0', Stable 아니면 Fail '0', 범위 안에 있을 경우 Rezero 완료 '1'\n
*                 US : \n
*                 CN : \n
* @remark   
********************************************************************************
*/
INT8U AdSetZero(void)
{
	if (!AdData.stableFlag)
	{
		return ERR_UNSTABLE;
	}
	else if (tareBuf)
	{
		return ERR_ALREADY_TARE;
	}
#ifdef USE_KEY_ZERO_FOR_CERTIFICATION
	else if (AbsInt32s(AdData.rawInitZeroData - zeroBuf) > rezeroLimit)
#elif defined USE_SRILANKA_ZERO_RANGE
  else if (AbsInt32s(AdData.rawInitZeroData) > (rezeroLimit + d0_5 - 1))
#else
	else if (AbsInt32s(AdData.rawInitZeroData) > rezeroLimit)
#endif
	{
		return ERR_OVER_RANGE;
	}
#ifdef USE_SRILANKA_ZERO_RANGE
	else if ((RangeParam.zeroPercent == 3) && (AbsInt32s(AdData.rawInitZeroData - zeroBuf) > (rezeroLimit + d0_5 - 1)))
#else
	else if ((RangeParam.zeroPercent == 3) && (AbsInt32s(AdData.rawInitZeroData - zeroBuf) > rezeroLimit))
#endif
	{
		return ERR_OVER_RANGE;
	}
	else
	{
		HystSetZero(AdData.rawFilteredData);
		LinearSetZero(AdData.rawFilteredData);
		zeroBuf = AdData.rawInitZeroData;
		zeroBufCalUnit = adConvInternalUnit(zeroBuf, currUnit, CalParam.calUnit);
#ifdef USE_WEIGHT_BACKUP
		DispFiltParam.zeroBufBackupValue = initZeroBufCalUnit + zeroBufCalUnit;
		DispFiltWriteParam();
#endif
#ifdef USE_AD_READ_COMPLETE_CHECK
		AdDataReadCompleteFlag = FALSE;
#endif
		return ERR_NONE;	
	}
}

#ifdef AUTO_ZERO
/**
********************************************************************************
* @brief       KR : 현재 AD 값을 Auto Rezero하는 함수,자동으로 실행함 \n
*                 US : \n
*                 CN : \n
* @param    none
* @return     KR : Rezero 범위 초과 시 Fail '0', Stable 아니면 Fail '0', 범위 안에 있을 경우 Rezero 완료 '1'\n
*                 US : \n
*                 CN : \n
* @remark   
********************************************************************************
*/
INT8U AutoSetZero(void)
{
    static INT16U prevAutoDelay = 0;
    static INT16U currAutoDelay = 0;
    static INT8U direction = 0;
    static INT8U activeSec = 0;
    static INT8U dValue = 0;
    static INT8U startFlag = 0;

    if (DispFiltParam.autozeroIndex == 0x00)
        return ERR_NONE;

    if (startFlag == 0)
    {
        direction = DispFiltParam.autozeroIndex / 100;
        activeSec = ((DispFiltParam.autozeroIndex % 100)/10)*10;
        dValue = (DispFiltParam.autozeroIndex % 100)%10;
        startFlag = 1;
    }

	if (!AdData.stableFlag)
	{
        prevAutoDelay = TimerGetSysTimer100ms();
        currAutoDelay = prevAutoDelay;
		return ERR_NONE;
	}
	else if (tareBuf)
	{
		return ERR_NONE;
	}
	else if (AbsInt32s(AdData.rawInitZeroData) > rezeroLimit)
	{
		return ERR_NONE;
	}
	else if ((RangeParam.zeroPercent == 3) && (AbsInt32s(AdData.rawInitZeroData - zeroBuf) > rezeroLimit))
	{
		return ERR_NONE;
	}
	else
	{
        if ((INT16U)(currAutoDelay - prevAutoDelay) > activeSec)
        {
            if (!direction)
            {
               if (adIntToExtValue(AdData.rawInitZeroData) >= 0)
                   return ERR_NONE;
            }
    
            if (AbsInt32s(adIntToExtValue(AdData.rawInitZeroData)) <= (divLow * dValue))
            {
                HystSetZero(AdData.rawFilteredData);
                LinearSetZero(AdData.rawFilteredData);
                zeroBuf = AdData.rawInitZeroData;
                zeroBufCalUnit = adConvInternalUnit(zeroBuf, currUnit, CalParam.calUnit);
            }
        }
        else
        {
            currAutoDelay = TimerGetSysTimer100ms();
        }
		return ERR_NONE;	
	}
}
#endif



/**
********************************************************************************
* @brief       KR : 현재 AD값을 Tare로 Setting하는 함수, Tare Key를 입력 받을 경우 실행함\n
*                 US : \n
*                 CN : \n
* @param    none
* @return     KR : Tare 범위 초과 시 Fail, Stable 아니면 Fail?????, 범위 안에 있을 경우 Tare 완료\n
*                 US : \n
*                 CN : \n
* @remark   
********************************************************************************
*/
INT8U AdSetTare(void)
{
    INT16U checkTareRemoveValue;

    // User Tare Limit 이면서 dual Interval 인 경우, tare 값을 dualThreshold 이상으로 설정하면 tareRemovableRange 는 divHigh 값이 기준이어야 한다.
    if((RangeParam.tareLimitType == TARE_LIMIT_TYPE_USER) && (CalParam.dualInterval) && (tareBuf > dualThreshold))
        checkTareRemoveValue = (INT16U)adExtToIntValue(divHigh) / 2;
    else
        checkTareRemoveValue = tareRemovableRange;
       
	if (!AdData.stableFlag) 
	{
		return ERR_UNSTABLE;
	}
	else
	{   
        if (AdData.rawSetZeroData <= (0 - (INT32S)checkTareRemoveValue))
		{
			return ERR_OVER_RANGE;
		}
		else if (AdData.rawSetZeroData < (INT32S)checkTareRemoveValue)
		{
			if (tareBuf)
			{
				tareBuf = 0;	//Tare Clear
				tareBufCalUnit = 0;
#ifdef USE_TURKEY_POS_PROTOCOL	// Zero set procedure (눈금 변할 경우 무게 잔량 남는 문제 해결)
				HystSetZero(AdData.rawFilteredData);
				LinearSetZero(AdData.rawFilteredData);
				zeroBuf = AdData.rawInitZeroData;
				zeroBufCalUnit = adConvInternalUnit(zeroBuf, currUnit, CalParam.calUnit);
#endif
#if defined(DIALOG6) || defined(USE_SMART_BT_PROTOCOL) || defined(USE_SATO_PRINTER_PROTOCOL)
				SaleData.keyTare = adIntToExtValue(tareBuf);
#endif
			}
			else
			{
				return ERR_NO_TARE;
			}
			tareType = TARE_TYPE_WEIGHT;
		}
		else if (AdData.rawSetZeroData <= currTareLimit)	
		{
			if (tareType == TARE_TYPE_WEIGHT) 
			{
				if (adTareCheckCondition(tareBuf, AdData.rawSetZeroData)) 
				{
					tareBuf = AdData.rawSetZeroData;
					tareBufCalUnit = adConvInternalUnit(tareBuf, currUnit, CalParam.calUnit);
#if defined(DIALOG6) || defined(USE_SMART_BT_PROTOCOL) || defined(USE_SATO_PRINTER_PROTOCOL)		
					SaleData.keyTare = adIntToExtValue(tareBuf);
#endif
				}
				else 
				{
					return ERR_ALREADY_TARE;
				}
			}
			else
			{
				return ERR_TARE_TYPE;
			}
		}
		else
		{
			return ERR_OVER_RANGE;
		}
	}
#ifdef USE_WEIGHT_BACKUP
	DispFiltParam.zeroBufBackupValue = initZeroBufCalUnit + zeroBufCalUnit;
	DispFiltParam.tareBufBackupValue = tareBufCalUnit;
	DispFiltWriteParam();
#endif
#ifdef USE_AD_READ_COMPLETE_CHECK
	AdDataReadCompleteFlag = FALSE;
#endif
	return ERR_NONE;
}

#ifdef USE_SATO_PRINTER_PROTOCOL
/**
********************************************************************************
* @brief       KR : TARE CLEAR 함수
*                 US : \n
*                 CN : \n
* @param    none
* @return     KR : 현재 TARE CLEAR 동작만 수행
*                 US : \n
*                 CN : \n
* @remark   
********************************************************************************
*/
INT8U AdSetTareClear(void)
{
    INT16U checkTareRemoveValue;

    // User Tare Limit 이면서 dual Interval 인 경우, tare 값을 dualThreshold 이상으로 설정하면 tareRemovableRange 는 divHigh 값이 기준이어야 한다.
    if((RangeParam.tareLimitType == TARE_LIMIT_TYPE_USER) && (CalParam.dualInterval) && (tareBuf > dualThreshold))
        checkTareRemoveValue = (INT16U)adExtToIntValue(divHigh) / 2;
    else
        checkTareRemoveValue = tareRemovableRange;
       
	if (!AdData.stableFlag) 
	{
		return ERR_UNSTABLE;
	}
	else
	{   
        if (AdData.rawSetZeroData < (INT32S)checkTareRemoveValue)
		{
			if (tareBuf)
			{
				tareBuf = 0;	//Tare Clear
				tareBufCalUnit = 0;
				SaleData.keyTare = adIntToExtValue(tareBuf);
			}
			else
			{
				return ERR_NO_TARE;
			}
			tareType = TARE_TYPE_WEIGHT;
		}
		else
		{
			return ERR_OVER_RANGE;
		}
	}
	return ERR_NONE;
}
#endif
/**
********************************************************************************
* @brief       KR : PT값이 무게 Interval값을 확인하여 옳은 값인지 확인\n
*                 US : \n
*                 CN : \n
* @param    KR : 외부값\n
*                 US : \n
*                 CN : \n
* @return     KR : 에러값\n
*                 US : \n
*                 CN : \n
* @remark   
********************************************************************************
*/
INT8U AdCheckPTInterval(INT32S presetTare)
{
	INT32S tare;
	INT16U div;

	if (presetTare > adIntToExtValue(currTareLimit))
	{
		return ERR_OVER_RANGE;
	}

	tare = adExtToIntValue(presetTare);
	if (tare > currTareLimit)
	{
		return ERR_OVER_RANGE;
	}

	if (CalParam.dualInterval)
	{
		if (AbsInt32s(presetTare) > dualThreshold)
		{
			div = divHigh;				
		}
		else
		{
			div = divLow;
		}		
	}
	else
	{
		div = divHigh;
	}
	
	if (presetTare % div)
	{
		return ERR_TARE_INTERVAL;
	}
	else
	{
		return ERR_NONE;
	}
}

/**
********************************************************************************
* @brief       KR : Preset tare 값을 세팅\n
*                 US : \n
*                 CN : \n
* @param     KR : Preset tare(외부데이터)\n
*                 US : \n
*                 CN : \n
* @param    KR : constraintFlag -> 강제로 tare 기능을 수행한다.\n
*                 US : \n
*                 CN : \n
* @return     KR : Tare 범위 초과 시 Fail, Stable 아니면 Fail??????', 범위 안에 있을 경우 Tare 완료\n
*                 US : \n
*                 CN : \n
* @remark   
********************************************************************************
*/
INT8U AdSetPT(INT32S presetTare, INT8U constraintFlag)
{
	INT32S tare;
	if (presetTare > adIntToExtValue(currTareLimit))
	{
		return ERR_OVER_RANGE;
	}

	/*받아온 preset tare값을 반올림하고 Internal resolution으로 변환*/
	tare = adExtToIntValue(presetTare);
	tare = adIntToExtValue(tare);
	tare = adExtToIntValue(tare);

	if ((tareBuf != 0) && (tareType == TARE_TYPE_WEIGHT) && constraintFlag != 1)
	{
		return ERR_TARE_TYPE;
	}
	
	if (tare <= currTareLimit)
	{
		if (tareBuf && constraintFlag != 1)
		{
			return ERR_ALREADY_TARE;
		}
		else 
		{
			tareBuf = tare;
			tareBufCalUnit = adConvInternalUnit(tareBuf, currUnit, CalParam.calUnit);
#if defined (DIALOG6) || defined(USE_SATO_PRINTER_PROTOCOL)
			SaleData.keyTare = adIntToExtValue(tareBuf);
    #ifdef USE_SATO_PRINTER_PROTOCOL
            if(RecordeTareflag)
                RecordeTareflag = OFF;
    #endif
#endif
			if (tare == 0)
			{
				tareType = TARE_TYPE_WEIGHT;				
			}
			else 
			{
				tareType = TARE_TYPE_PRESET;
			}
		}
	}
	else
	{
		return ERR_OVER_RANGE;
	}
	
	return ERR_NONE;	
}

/**
********************************************************************************
* @brief       KR : Percent tare 값을 세팅\n
*                 US : \n
*                 CN : \n
* @param    Percent (%)
* @return   
* @remark   
********************************************************************************
*/
INT8U AdSetPercentTare(INT8U percent)
{
	tarePercent = percent;
	return ERR_NONE;
}

/**
********************************************************************************
* @brief       KR : 입력받은 무게값을 division에 맞게 반올림 \n
*                 US : \n
*                 CN : \n
* @param    KR : 외부값\n
*                 US : \n
*                 CN : \n
* @return     KR : 에러값\n
*                 US : \n
*                 CN : \n
* @remark   
********************************************************************************
*/
INT32S AdGetRoundedWeight(INT32S extWeight)
{
	INT32S intWeight;

	intWeight = adExtToIntValue(extWeight);
	return adIntToExtValue(intWeight);
}

/**
********************************************************************************
* @brief       KR : Ad에서 사용되는 Parameter들을 외부 파일에서 읽게해주는 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : 읽고자 하는 Paramemer number (AdApi.h 에 선언)
*                 US : \n
*                 CN : \n
* @return     KR : Parameter값
*                 US : \n
*                 CN : \n
* @remark   
********************************************************************************
*/
INT32S AdReadParam(INT8U structNum)
{
	switch (structNum)
	{
		case INIT_ZERO_NUM:
			return (INT32S)RangeParam.initialZeroPercent;
			
		case USING_G_NUM:
			return (INT32S)CalParam.usingG;
			
		case CAL_G_NUM:
			return (INT32S)CalParam.calG;
			
		case HYST_COMP_ENABLE:
			return (INT32S)HystParam.hystEnable;
                        
#ifdef USE_EXP_HYST_COMP
		case HYST_COMP_ORDER:
			return (INT32S)HystParam.hystOrder;
#endif                        
#if defined(USE_TEMP_COMP) || defined(USE_TEMP_COMP_MWIIH)			
		case TEMP_COMP_ENABLE:
			return (INT32S)TempParam.tempCompEnable;
#endif			
		case FULL_TARE_FLAG:
			if (CalParam.calUnit == 0)
			{
				return RangeParam.tareLimit;
			}
			else if (CalParam.calUnit == 1)
			{
				return RangeParam.tareLimitLb;
			}
			break;
			
		case RESOLUTION:
			return CalParam.resolution;
			
		case TARE_SUCCESSIVE:
			return (INT32S)RangeParam.tareType;
			
		case CAL_UNIT:
			return (INT32S)CalParam.calUnit;
			
		case USING_UNIT:
			return (INT32S)CalParam.usingUnit;
			
		case DUAL_INTERVAL:
			return (INT32S)CalParam.dualInterval;
			
		case CREEP_COMP_TIME:
			return (INT32S)CreepParam.creepTime;
			
		case CREEP_COMP_VALUE:
			return (INT32S)CreepParam.creep;

		case CAPA:
			return (INT32S)CalParam.capa;
			
		case OVER_LOAD_LIMIT:
			return (INT32S)RangeParam.overloadLimit;
			
		case ZERO:
			return (INT32S)RangeParam.zeroPercent;
			
		case DIV_FACTOR:
			return (INT32S)CalParam.divFactor;
			
		case LINEAR_RATIO:
			return (INT32S)((CalParam.capa / LinearParam.linearRatio.word.low) * LinearParam.linearRatio.word.high);

		case WEIGHT_DEC_POS:
			return (INT32S)CalParam.weightDecPos;

		case ZERO_MARK_INDICATOR:
			return (INT32S)RangeParam.zeroMarkType;
			
		default :
			break;
	}
}

/**
********************************************************************************
* @brief       KR : Ad에서 사용되는 Parameter들을 외부 파일에서 Write 해주는 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : Write하고자 하는 Paramemer number (AdApi.h 에 선언), Write하고자 하는 Parameter값\n
*                 US : \n
*                 CN : \n
* @return   NONE
* @remark   
********************************************************************************
*/
void AdWriteParam(INT8U structNum, INT32S inputData)
{
#ifdef USE_CTS_MODE
	INT8U ctsUpdateBuf[4];
#endif
	
	switch (structNum)
	{
		case INIT_ZERO_NUM:
			RangeParam.initialZeroPercent = (INT8U)inputData;
			RangeWriteParam();
			break;
			
		case USING_G_NUM:
			CalParam.usingG = (INT16S)inputData;
			CalWriteParam();
			break;
			
		case CAL_G_NUM:
			CalParam.calG = (INT16S)inputData;
			CalWriteParam();
			break;
			
		case HYST_COMP_ENABLE:
			HystParam.hystEnable = (INT8U)inputData;
			HystWriteParam();
			break;
#ifdef USE_EXP_HYST_COMP
		case HYST_COMP_ORDER:
			HystParam.hystOrder = (INT8U)inputData;
			HystWriteParam();
			break;                        
#endif
#if defined(USE_TEMP_COMP) || defined(USE_TEMP_COMP_MWIIH)
		case TEMP_COMP_ENABLE:
			TempParam.tempCompEnable = (INT8U)inputData;
			TempWriteParam();
			break;
#endif
		case FULL_TARE_FLAG:
			if (inputData == 1)
			{
				if (CalParam.calUnit == UNIT_LB)
				{
					RangeParam.tareLimitLb = CalParam.resolution;
				}
				else
				{
					RangeParam.tareLimit = CalParam.resolution;
				}
			}
			else
			{
				if (CalParam.calUnit == UNIT_LB)
				{
					RangeParam.tareLimitLb = 20000;
				}
				else
				{
					RangeParam.tareLimit = 20000;
				}
			}
			RangeWriteParam();
			break;
			
		case RESOLUTION:
			CalParam.resolution = inputData;
			CalWriteParam();
			break;
			
		case TARE_SUCCESSIVE:
#ifdef USE_SRILANKA_TARE
			RangeParam.tareType = TARE_TYPE_PROPER;
#else
			RangeParam.tareType = (INT8U)inputData;
#endif
			RangeWriteParam();
			break;
			
		case CAL_UNIT:
			CalParam.calUnit = (INT8U)inputData;
			CalWriteParam();
			break;
			
		case USING_UNIT:
			CalParam.usingUnit = (INT8U)inputData;
			CalWriteParam();
			break;
			
		case DUAL_INTERVAL:
			CalParam.dualInterval = (INT8U)inputData;
			CalWriteParam();
			break;
#ifdef USE_CREEP_COMP
		case CREEP_COMP_TIME:
			CreepParam.creepTime = (INT8U)inputData;
			CreepWriteParam();
			CreepInitVariable(adRawProcSamplingRate);//TO DO : move or not
			break;
			
		case CREEP_COMP_VALUE:
			CreepParam.creep = (INT16S)inputData;
			CreepWriteParam();
			CreepInitVariable(adRawProcSamplingRate);//TO DO : move or not
			break;
#endif
		case CAPA:
			CalParam.capa = (INT16U)inputData;
			CalWriteParam();
			break;	
			
		case OVER_LOAD_LIMIT:
			RangeParam.overloadLimit = (INT8U)inputData;
			RangeWriteParam();
			break;
			
		case ZERO:
			RangeParam.zeroPercent = (INT8U)inputData;
			RangeWriteParam();
			break;
			
		case DIV_FACTOR:
			CalParam.divFactor = (INT8U)inputData;
			CalWriteParam();
			break;
			
		case LINEAR_RATIO:
			LinearParam.linearRatio.word.low = CalParam.capa;
			LinearParam.linearRatio.word.high = (INT16U)inputData;
			LinearWriteParam();
			break;

		case WEIGHT_DEC_POS:
			CalParam.weightDecPos = (INT8U)inputData;
			CalWriteParam();
			break;

		case ZERO_MARK_INDICATOR:
			RangeParam.zeroMarkType = (INT8U)inputData;
			RangeWriteParam();
			break;
			
		default :
			break;
	}
	
#ifdef USE_CTS_MODE
	switch (structNum)
	{
		case USING_G_NUM:
			*((INT16U*)ctsUpdateBuf) = CalParam.usingG;
			CtsUpdateScaleData(7, ctsUpdateBuf);	// 7 Local gravity factor
			break;
			
		case CAL_G_NUM:
			*((INT16U*)ctsUpdateBuf) = CalParam.calG;
			CtsUpdateScaleData(6, ctsUpdateBuf);	// 6 Factory gravity factor
			break;
			
		case CAL_UNIT:
			switch (CalParam.calUnit)
			{
				case 0:
					MakeStrCopy(ctsUpdateBuf, "kg", 2);
					break;
				case 1:
					MakeStrCopy(ctsUpdateBuf, "lb", 2);
					break;
				case 2:
					MakeStrCopy(ctsUpdateBuf, "oz", 2);
					break;
				case 3:
					MakeStrCopy(ctsUpdateBuf, "g", 1);
					break;
				case 4:
					MakeStrCopy(ctsUpdateBuf, "t", 1);
					break;
			}
			CtsUpdateScaleData(13, ctsUpdateBuf);	// 13 Unit
			break;
			
		case CAPA:
			CtsUpdateScaleData(8, ctsUpdateBuf);	// 8 CAPA
			break;	
			
		case DIV_FACTOR:
			CtsUpdateScaleData(9, &CalParam.divFactor);	// 9 e
			CtsUpdateScaleData(10, &CalParam.divFactor);	// 10 d
			break;
			
		case WEIGHT_DEC_POS:
			CtsUpdateScaleData(11, &CalParam.weightDecPos);	// 11 Weight decimal point
			break;

		default :
			break;
	}
#endif
}

/**
********************************************************************************
* @brief       KR : Nomalized data를 Raw data로 변환하는 함수\n
*                 US : \n
*                 CN : \n
* @param    Nomalized data
* @return     Raw data
* @remark   
********************************************************************************
*/
INT32S AdNormalToRaw(INT32S ad)
{
	FP32 ftemp;
	
	ftemp = (FP32)ad / convSlope;
	if (ftemp > 0) 
	{
		ftemp += 0.5;
	}
	else
	{
		ftemp -= 0.5;
	}
	ad = (INT32S)ftemp;
	
	return ad;	
}

/**
********************************************************************************
* @brief       KR : Capa table에서 capaIndex를 찾아 주는 함수\n
*                 US : \n
*                 CN : \n
* @param    calUnit :
*                 capa :  
* @return     KR : CapaTable의 Index값 (capaIndex)
*                 US : \n
*                 CN : \n
* @remark   
********************************************************************************
*/
INT8U AdFindCapaIndex(INT8U unit, INT16U capa)
{
	INT8U i;

	if (capa == 0) 
	{
		return CAPA_TABLE_SIZE;
	}

	for (i = 0; i < CAPA_TABLE_SIZE; i++)
	{
		if (CapaTable[i][unit].capa == capa)
		{
			break;
		}
	}

	if (i == CAPA_TABLE_SIZE) 
	{
		return 0xff;
	}
	else 
	{
		return i;
	}
}

/**
********************************************************************************
* @brief       KR : Capa table에서 divTableIndex를 찾아 주는 함수\n
*                 US : \n
*                 CN : \n
* @param    divFactor :
* @return     KR : divTable의 Index값 (divTableIndex)\n
*                 US : \n
*                 CN : \n
* @remark   KR : 1/16 oz table은 제외함\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
INT8U AdFindDivTableIndex(INT8U divFactor)
{
	INT8U i;

	for (i = 0; i < DIV_TABLE_SIZE; i++)
	{
		if (divTable[i] == divFactor) 
		{
			break;
		}
	}

	if (i == DIV_TABLE_SIZE) 
	{
		return 0xff;
	}
	else
	{
		return i;
	}
}

/**
********************************************************************************
* @brief       KR : Weight Unit Flag를 변경해주는 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : 변경하고자 하는 Unit (AdData.h에 선언)\n
*                 US : \n
*                 CN : \n
* @return     KR : 변경 가능 여부 (0-변경안됨, 1-변경됨)\n
*                 US : \n
*                 CN : \n
* @remark   KR : divTableIndex, capaIndex는 미리 정해져야 함\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
BOOLEAN AdSetWeightUnitFlag(INT8U weightUnit)
{
	if (CapaTable[capaIndex][weightUnit].capa == 0) 
	{
		return FALSE;
	}

	if (AdData.weightUnit != weightUnit) //for reducing eeprom writing time in adproc routine
	{
		adChangeWeightUnitFlag = 0x80 | weightUnit;
	}
	return TRUE;
}

/**
********************************************************************************
* @brief       KR : Calibration 이력을 저장한다.\n
*                 US : \n
*                 CN : \n
* @param    KR : rawAd데이터\n
*                 US : \n
*                 CN : \n
* @return   none
* @remark   
********************************************************************************
*/
void AdWriteHistoryParam(INT8U cType, INT32S zeroValue, INT32S midUpValue, INT32S midDownValue, INT32S spanValue)
{
	INT8U counter;
	CAL_HISTORY_STRUCT calHistory;
#ifdef USE_CTS_MODE
	INT8U ctsUpdateBuf[4];
#endif

#ifdef USE_CTS_MODE
	*((INT32U*)ctsUpdateBuf) = zeroValue;
	CtsUpdateScaleData(2, ctsUpdateBuf);		// 2 Zero
	*((INT32U*)ctsUpdateBuf) = midUpValue;
	CtsUpdateScaleData(3, ctsUpdateBuf);		// 3 Mid up
	*((INT32U*)ctsUpdateBuf) = midDownValue;
	CtsUpdateScaleData(4, ctsUpdateBuf);	// 4 Mid dn
	*((INT32U*)ctsUpdateBuf) = spanValue;
	CtsUpdateScaleData(5, ctsUpdateBuf);		// 5 Span
#endif
	
	counter = ParamReadByte(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_HISTORY_COUNT);
	counter++;
	ParamWriteByte(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_HISTORY_COUNT, counter);
	calHistory.id = counter;
	calHistory.cType = cType;
#ifdef USE_RTC
	TimeReadData();
	calHistory.year = TimeData.year;
	calHistory.month = TimeData.month;
	calHistory.date = TimeData.date;
#else
	calHistory.year = 0;
	calHistory.month = 0;
	calHistory.date = 0;
#endif
	calHistory.calZeroValue = zeroValue;
	calHistory.calMidUpValue = midUpValue;
	calHistory.calMidDownValue = midDownValue;
	calHistory.calSpanValue = spanValue;

	counter = counter % CAL_HISTORY_MAX_NUM;
	ParamWriteData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_HISTORY + counter * sizeof(calHistory), (INT8U *)&calHistory, sizeof(calHistory));
}

/**
********************************************************************************
* @brief       KR : Hysteresis 보상을 위한 Calibration을 수행한다.\n
			     1. 영점 데이터를 저장한다.\n
			     2. 중간값(일반적으로 Full span의 1/3)을 저장한다.\n
			     3. 스판값을 저장한다.\n
		 	     4. 2번의 중간값을 다시 저장한다.\n
*                 US : \n
*                 CN : \n
* @param    KR : rawAd데이터\n
*                 US : \n
*                 CN : \n
* @return   none
* @remark   
********************************************************************************
*/
#ifdef USE_EXP_HYST_COMP
INT8U AdWriteHystCalParam(INT32S *hystValue)
{
        INT8U i;
        INT8U order;
        
        if (HystCompSetRatio4Capa(CalParam.capa) != ERR_NONE)  return ERR_CAL_RANGE;   

        order = HystParam.hystOrder;

	//if (CalParam.calSpanLimit < spanValue || CalParam.calZeroLimit > zeroValue 
	//	|| zeroValue > midUpValue || zeroValue > midDownValue || zeroValue > spanValue 
	//	|| midUpValue > spanValue || midDownValue > spanValue)         
	if (CalParam.calSpanLimit < hystValue[order]  || CalParam.calZeroLimit > hystValue[0] 
		|| hystValue[0] > hystValue[order/2+1] || hystValue[0] > hystValue[order/2+order] || hystValue[0] > hystValue[order] 
		|| hystValue[order/2+1] > hystValue[order] || hystValue[order/2+order] > hystValue[order]) 
	{
		return ERR_CAL_RANGE;
	}
	CalParam.usingG = CalParam.calG;

        for (i = 0; i < 10; i++) 
        {
                HystParam.hystAd[i] = hystValue[i];
        }
        
	TempParam.reCalTemp = TempDrvReadData();

	HystParam.hystEnable = 1;	
	LinearParam.linearZeroValue = hystValue[0];
	LinearParam.linearMidValue = hystValue[order/2+1];
	LinearParam.linearSpanValue = hystValue[order];
	CalParam.calZeroValue = hystValue[0];
	CalParam.calSpanValue = hystValue[order];

	HystWriteParam();
	LinearWriteParam();							
	CalWriteParam();
#if defined(USE_TEMP_COMP) || defined(USE_TEMP_COMP_MWIIH)
	TempWriteParam();
#endif
	HystSetZero(HystParam.hystAd[0]);
	LinearSetZero(LinearParam.linearZeroValue);
	LinearCalcVariable();

#ifdef USE_WEIGHT_BACKUP
	DispFiltParam.zeroBufBackupValue = 0;
	//DispFiltParam.tareBufBackupValue = 0; //when next initial zero state, automatically set zero
	DispFiltWriteParam();
#endif

        //	AdWriteHistoryParam(0, zeroValue, midUpValue, midDownValue, spanValue);//CAL_PROC_HYST
	AdWriteHistoryParam(0, hystValue[0],  hystValue[order/2+1], hystValue[order/2+order], hystValue[order]);//CAL_PROC_HYST

	return ERR_NONE;
}
#else
INT8U AdWriteHystCalParam(INT32S zeroValue, INT32S midUpValue, INT32S midDownValue, INT32S spanValue)
{
	if (CalParam.calSpanLimit < spanValue || CalParam.calZeroLimit > zeroValue 
		|| zeroValue > midUpValue || zeroValue > midDownValue || zeroValue > spanValue 
		|| midUpValue > spanValue || midDownValue > spanValue) 
	{
		return ERR_CAL_RANGE;
	}
	CalParam.usingG = CalParam.calG;

	HystParam.hystZeroValue = zeroValue;
	HystParam.hystSpanValue = spanValue;
	HystParam.hystMidUpValue = midUpValue;
	HystParam.hystMidDownValue = midDownValue;
#if defined(USE_TEMP_COMP) || defined(USE_TEMP_COMP_MWIIH)
	TempParam.reCalTemp = TempDrvReadData();
#endif
	HystParam.hystEnable = 1;	
	LinearParam.linearZeroValue = HystParam.hystZeroValue;
	LinearParam.linearMidValue = HystParam.hystMidDownValue;
	LinearParam.linearSpanValue = HystParam.hystSpanValue;
	CalParam.calZeroValue = LinearParam.linearZeroValue;
	CalParam.calSpanValue = LinearParam.linearSpanValue;

	HystWriteParam();
	LinearWriteParam();							
	CalWriteParam();
#if defined(USE_TEMP_COMP) || defined(USE_TEMP_COMP_MWIIH)
	TempWriteParam();
#endif
	HystSetZero(HystParam.hystZeroValue);
	LinearSetZero(LinearParam.linearZeroValue);
	LinearCalcVariable();

#ifdef USE_WEIGHT_BACKUP
	DispFiltParam.zeroBufBackupValue = 0;
	//DispFiltParam.tareBufBackupValue = 0; //when next initial zero state, automatically set zero
	DispFiltWriteParam();
#endif

	AdWriteHistoryParam(0, zeroValue, midUpValue, midDownValue, spanValue);//CAL_PROC_HYST

	return ERR_NONE;
}
#endif
/**
********************************************************************************
* @brief       KR : 직선성 보상을 위한 Calibration을 수행한다.\n
			     1. 영점 데이터를 저장한다.\n
			     2. 중간값(일반적으로 Full span의 1/3)을 저장한다.\n
			     3. 스판값을 저장한다.\n
*                 US : \n
*                 CN : \n
* @param    KR : rawAd데이터\n
*                 US : \n
*                 CN : \n
* @return   none
* @remark   
********************************************************************************
*/
INT8U AdWriteLinearCalParam(INT32S zeroValue, INT32S midUpValue, INT32S spanValue)
{
	if (CalParam.calSpanLimit < spanValue || CalParam.calZeroLimit > zeroValue 
		|| zeroValue > midUpValue || zeroValue > spanValue 
		|| midUpValue > spanValue) 
	{
		return ERR_CAL_RANGE;
	}
	CalParam.usingG = CalParam.calG;

	LinearParam.linearZeroValue = zeroValue;
	LinearParam.linearMidValue = midUpValue;
	LinearParam.linearSpanValue = spanValue;

	CalParam.calZeroValue = LinearParam.linearZeroValue;
	CalParam.calSpanValue = LinearParam.linearSpanValue;
#if defined(USE_TEMP_COMP) || defined(USE_TEMP_COMP_MWIIH)
	TempParam.reCalTemp = TempDrvReadData();
#endif
	CalWriteParam();
	LinearWriteParam();
#if defined(USE_TEMP_COMP) || defined(USE_TEMP_COMP_MWIIH)
	TempWriteParam();
#endif
	LinearSetZero(LinearParam.linearZeroValue);
	LinearCalcVariable();
		
#ifdef USE_WEIGHT_BACKUP
	DispFiltParam.zeroBufBackupValue = 0;
	//DispFiltParam.tareBufBackupValue = 0; //when next initial zero state, automatically set zero
	DispFiltWriteParam();
#endif

	AdWriteHistoryParam(1, zeroValue, midUpValue, 0, spanValue);//CAL_PROC_LINEAR

	return ERR_NONE;
}

/**
********************************************************************************
* @brief       KR : Calibration을 수행한다.\n
			     1. 영점 데이터를 저장한다.\n
			     2. 스판값을 저장한다.\n
*                 US : \n
*                 CN : \n
* @param    KR : rawAd데이터\n
*                 US : \n
*                 CN : \n
* @return   none
* @remark   
********************************************************************************
*/
INT8U AdWriteSpanCalParam(INT8U cType, INT16U cData, INT32S zeroValue, INT32S spanValue)
{
	if ((CalParam.calSpanLimit < spanValue) || (CalParam.calZeroLimit > zeroValue)
		|| (zeroValue > spanValue))
	{
		return ERR_CAL_RANGE;
	}

	CalParam.usingG = CalParam.calG;

	CalParam.calZeroValue = zeroValue;
	CalParam.calSpanValue = spanValue;
#if defined(USE_TEMP_COMP) || defined(USE_TEMP_COMP_MWIIH)
	TempParam.reCalTemp = TempDrvReadData();
	TempWriteParam();
#endif
	CalWriteParam();

#ifdef USE_WEIGHT_BACKUP
	DispFiltParam.zeroBufBackupValue = 0;
	//DispFiltParam.tareBufBackupValue = 0; //when next initial zero state, automatically set zero
	DispFiltWriteParam();
#endif

	AdWriteHistoryParam(cType, zeroValue, (INT32U)cData, 0, spanValue);//CAL_PROC_SPAN, CAL_PROC_PERCENT, CAL_PROC_WEIGHT

	return ERR_NONE;
}

/**
********************************************************************************
* @brief       KR : 내부값을 외부값으로 변경하는 외부 사용 가능 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : interVal : 내부값\n
*                 US : \n
*                 CN : \n
* @return    KR : 외부값\n
*                 US : \n
*                 CN : \n
* @remark   
********************************************************************************
*/
INT32S AdIntToExtValueExtern (INT32S interval)
{
	return adIntToExtValue(interval);
}

/**
********************************************************************************
* @brief       KR : 외부값을 내부값으로 변경하는 외부 사용 가능 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : interVal : 외부값\n
*                 US : \n
*                 CN : \n
* @return    KR : 외부값\n
*                 US : \n
*                 CN : \n
* @remark   
********************************************************************************
*/
INT32S AdExtToIntValueExtern (INT32S interval)
{
	return adExtToIntValue(interval);
}


/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

/**
********************************************************************************
* @brief       KR : AD관련 Flag를 검사하여 Indicator를 동작하는 함수\n
*                 US : \n
*                 CN : \n
* @param    none
* @return     none
* @remark   KR : Stable, Zero, Tare등을 검사함\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
#ifdef USE_GROSS_KEY
BOOLEAN AdGrossIndicatorOnFlag = 0;
#endif
void adSetIndicator(void)
{
	if (AdData.stableFlag) 
	{
		DispSetIndicator(INDICATOR_STABLE, ON);
	}
	else 
	{
		DispSetIndicator(INDICATOR_STABLE, OFF);
	}
#ifdef USE_GROSS_KEY
	if(AdGrossIndicatorOnFlag)
	{
		DispSetIndicator(INDICATOR_NET, OFF);
	}
	else
	{
		if (AdData.tareWeight)
		{
			DispSetIndicator(INDICATOR_NET, ON);
		}
		else
		{
			DispSetIndicator(INDICATOR_NET, OFF);
		}
	}
#else
	if (AdData.tareWeight)
	{
		DispSetIndicator(INDICATOR_NET, ON);
	}
	else
	{
		DispSetIndicator(INDICATOR_NET, OFF);
	}
#endif	
	if (((RangeParam.zeroMarkType == ZERO_MARK_TYPE_GROSS) && (AdData.grossZeroFlag == 1))
		|| ((RangeParam.zeroMarkType == ZERO_MARK_TYPE_NET) && (AdData.netZeroFlag == 1))
		|| ((RangeParam.zeroMarkType == ZERO_MARK_TYPE_ALL) && ((AdData.netZeroFlag == 1) ||(AdData.grossZeroFlag == 1))))
	{
		DispSetIndicator(INDICATOR_ZERO, ON);
	}
	else
	{
		DispSetIndicator(INDICATOR_ZERO, OFF);
	}	
}

/**
********************************************************************************
* @brief       KR : 필터링된 Raw데이터의 Stable유무를 검사한다.\n
*                 US : \n
*                 CN : \n
* @param    KR : rawAd데이터\n
*                 US : \n
*                 CN : \n
* @return     KR : Param으로 받은 데이터를 그대로 리턴한다. \n
*                 US : \n
*                 CN : \n
* @remark   KR : 이전 필터링 Ad와 차이를 비교하여 검사 한다.\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
BOOLEAN adCheckRawStable(INT32S rawAd)
{
	static INT32S prevRawAd;
	BOOLEAN stable;

	if (AbsInt32s(rawAd - prevRawAd) <= RAW_STABLE_RANGE) 
	{
		stable = 1;
	}
	else
	{
		stable = 0;
	}

	prevRawAd = rawAd;
	return stable;
}

/**
********************************************************************************
* @brief       KR : 내부 분해능을 Calibration range(Span값 - Zero값)으로 나눈 값을 구하는 함수, 중력보상값도 적용\n
*                 US : \n
*                 CN : \n
* @param    KR : 선형 보상된 데이터\n
*                 US : \n
*                 CN : \n
* @return     KR : 단위 변환된 데이터\n
*                 US : \n
*                 CN : \n
* @remark   KR : 내부에서 사용하는 Parameter가 변경되면 실행하여야 함\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void adCalcConvSlope(void)
{
	convSlope = (FP32)CalParam.resolution / (FP32)(CalParam.calSpanValue - CalParam.calZeroValue);
#ifndef USE_MOON_GRAVITY
	convSlope *= ((FP32)(90000L + CalParam.calG) / (FP32)(90000L + CalParam.usingG));
#else
	convSlope *= ((FP32)(10000L + CalParam.usingG) /(FP32)(90000L + CalParam.calG)); //천문대에서 사용시 중력값.
#endif
}

/**
********************************************************************************
* @brief       KR : Calibration 단위와 현재 사용단위가 다르면 단위를 맞춰주기 위해 수행되는 함수\n
			     1. Kg(0) -> Lb(1)\n
			     2. Lb(1) -> Kg(0)\n
			     3. oz(2)\n
			     4. g(3)\n
*                 US : \n
*                 CN : \n
* @param    KR : 선형 보상된 데이터(FP32로 cast), 현재 단위, 변경 단위\n
*                 US : \n
*                 CN : \n
* @return     KR : 단위 변환된 데이터\n
*                 US : \n
*                 CN : \n
* @remark   KR : Unit Conversion (internal value) ex) 60,000(=15kg) -> 60,000(=30lb)\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
#ifdef USE_UNIT_TON_NEWTON_DAN_KN
//do not use gram, oz, 1/16 oz
FP32 adConvInternalUnit(FP32 fAd, INT8U unit1, INT8U unit2)
{
	FP32 tempFp32;

	if (unit1 != unit2)
	{
		tempFp32 = (FP32)((FP32)CapaTable[capaIndex][unit1].capa
				/(FP32)CapaTable[capaIndex][unit2].capa);

		switch (unit1)
		{
			case UNIT_TON:
				tempFp32 *= 1000.0;
				break;
            case UNIT_KN:
				tempFp32 *= 1000.0;
				break;
            case UNIT_DAN:
				tempFp32 *= 10.0;
				break;
		}

		switch (unit2)
		{
			case UNIT_TON:
				tempFp32 *= 0.001;
				break;
            case UNIT_KN:
				tempFp32 *= 0.001;
				break;
            case UNIT_DAN:
				tempFp32 *= 0.1;
				break;
		}

		switch (unit1)
		{
			case UNIT_KG:
			case UNIT_TON:
				switch (unit2)
				{
					case UNIT_LB:
						fAd *= 2.204622;//1kg = 2.204622622 lb
						break;

					case UNIT_NEWTON:
					case UNIT_KN:
					case UNIT_DAN:
						fAd *= 9.80665;//1kg = 9.80665 N
						break;   
					case UNIT_SHORT_TON:
						fAd *= 0.001102311;//1kg = 0.001102311 short ton
						break;                        
					case UNIT_LONG_TON:
						fAd *= 0.000954207;//1kg = 0.000954207 long ton
						break;                        
					default: //UNIT_KG, UNIT_TON
						break;
				}
				fAd *= tempFp32;
				break;								  
			case UNIT_LB:
				switch (unit2)
				{
					case UNIT_KG:
					case UNIT_TON:
						fAd *= 0.4535925;//1lb = 0.45359237 kg
						break;
					case UNIT_NEWTON:
					case UNIT_KN:
					case UNIT_DAN:
						fAd *= 4.448222;//1lb = 4.448221615 N
						break;   
					case UNIT_SHORT_TON:
						fAd *= 0.0005;//1lb = 0.0005 short ton
						break;                        
					case UNIT_LONG_TON:
						fAd *= 0.000446429;//1lb = 0.000446429 long ton
						break;                        
					default: //UNIT_LB
						break;
				}
				fAd *= tempFp32;
				break;
			case UNIT_SHORT_TON:
				switch (unit2)
				{
					case UNIT_KG:
					case UNIT_TON:
						fAd *= 907.1847;//1 short ton = 907.18474 kg
						break;                  
					case UNIT_LB:
						fAd *= 2000.000;//1 short ton = 2000.000000003 lb
						break;
					case UNIT_NEWTON:
					case UNIT_KN:
					case UNIT_DAN:
						fAd *= 8896.443;//1 short ton = 8896.443230521 N
						break;   
					case UNIT_LONG_TON:
						fAd *= 0.8928571;//1 short ton = 0.892857143 long ton
						break;                        
					default: //UNIT_SHORT_TON
						break;
				}
				fAd *= tempFp32;
				break;	
			case UNIT_LONG_TON:
				switch (unit2)
				{
					case UNIT_KG:
					case UNIT_TON:
						fAd *= 1016.047;//1 long ton = 1016.046908799 kg
						break;                  
					case UNIT_LB:
						fAd *= 2240.0;//1 long ton = 2240 lb
						break;
					case UNIT_NEWTON:
					case UNIT_KN:
					case UNIT_DAN:
						fAd *= 9964.016;//1 long ton = 9964.016418171 N
						break;   
					case UNIT_SHORT_TON:
						fAd *= 1.12;//1 long ton = 1.12 short ton
						break;                        
					default: //UNIT_LONG_TON
						break;
				}
				fAd *= tempFp32;
				break;                
			case UNIT_NEWTON:
			case UNIT_KN:
			case UNIT_DAN:
				switch (unit2)
				{
					case UNIT_KG:
					case UNIT_TON:
						fAd *= 0.1019716;//1N = 0.101971621 kg
						break;
					case UNIT_LB:
						fAd *= 0.2248089;//1N = 0.224808943 lb
						break;                        
					case UNIT_SHORT_TON:
						fAd *= 0.000112404;//1N = 0.000112404 short ton
						break;                        
					case UNIT_LONG_TON:
						fAd *= 0.000100361;//1N = 0.000100361 long ton
						break;                        
					default: //UNIT_NEWTON, UNIT_KN, UNIT_DAN
						break;
				}
				fAd *= tempFp32;
				break;                
			default:
				break;
		}
	}
	return fAd;
}
#elif defined (USE_UNIT_KG_ONLY)
//do not use gram, oz, 1/16 oz
FP32 adConvInternalUnit(FP32 fAd, INT8U unit1, INT8U unit2)
{
    return fAd;
}

#else
FP32 adConvInternalUnit(FP32 fAd, INT8U unit1, INT8U unit2)
{
	FP32 tempFp32;

	if (unit1 != unit2)
	{
		tempFp32 = (FP32)((FP32)CapaTable[capaIndex][unit1].capa
				/(FP32)CapaTable[capaIndex][unit2].capa);

		switch (unit1)
		{
			case UNIT_TON:
				tempFp32 *= 1000.0;
				break;
			case UNIT_GRAM:
				tempFp32 *= 0.001;
				break;
		}

		switch (unit2)
		{
			case UNIT_TON:
				tempFp32 *= 0.001;
				break;
			case UNIT_GRAM:
				tempFp32 *= 1000.0;
				break;
		}

		switch (unit1)
		{
			case UNIT_KG:
#ifdef USE_UNIT_0_1KG_CAPA_TABLE
				tempFp32 /= 10.0;//0.1kg
#endif
			case UNIT_TON:
			case UNIT_GRAM:
				switch (unit2)		// duckspg unit
				{
					case UNIT_LB:
#ifdef USE_LIBRA_ESPANOLA_UNIT_CHANGE
						fAd *= 2.173913;//1kg = 2.173913 libra espanola
#else
						fAd *= 2.204622;//1kg = 2.204622622 lb
#endif
						break;
					case UNIT_OZ:
#ifdef USE_LIBRA_ESPANOLA_UNIT_CHANGE
						fAd *= 34.782608;//1kg = 34.7826082.173913 libra espanola
#else
						fAd *= 35.273961;//1kg = 35.27396195 oz
#endif
						break;
#ifdef USE_UNIT_1_16OZ
					case UNIT_1_16OZ:
						fAd *= 564.383376; //1kg = 35.273961 * 16.0 (1/16 oz)
						break;
#endif
#ifdef USE_UNIT_0_1KG_CAPA_TABLE
					case UNIT_KG:
						fAd *= 10.0;//0.1kg
						break;
#endif
#ifdef USE_VISS_UNIT_CHANGE
					case UNIT_VISS:
						fAd *= 0.612245;//1viss = 1.633333kg
						break;
#endif

					default: //UNIT_KG, UNIT_GRAM, UNIT_TON
						break;
				}
				fAd *= tempFp32;
				break;								  
			case UNIT_LB:
				switch (unit2)
				{
					case UNIT_KG:
#ifdef USE_UNIT_0_1KG_CAPA_TABLE
						tempFp32 *= 10.0;//0.1kg
#endif
					case UNIT_TON:
					case UNIT_GRAM:
#ifdef USE_LIBRA_ESPANOLA_UNIT_CHANGE
						fAd *= 0.460000009;//1lb = 0.460000009 kg	 libra espanola
#else
						fAd *= 0.4535925;//1lb = 0.45359237 kg
#endif
						break;
					case UNIT_OZ:
						fAd *= 16.0;///1 lb = 16 oz	 
						break;
#ifdef USE_UNIT_1_16OZ
					case UNIT_1_16OZ:
						fAd *= 256.0;///1 lb = 256 (1/16 oz)
						break;
#endif
#ifdef USE_VISS_UNIT_CHANGE
					case UNIT_VISS:
						fAd *= 0.4535925 * 0.612245;///1 lb = kg(lb) * 0.612245
						break;
#endif
					default: //UNIT_LB
						break;
				}
				fAd *= tempFp32;
				break;
			case UNIT_OZ:
				switch (unit2)
				{
					case UNIT_KG:
#ifdef USE_UNIT_0_1KG_CAPA_TABLE
						tempFp32 *= 10.0;//0.1kg
#endif
					case UNIT_TON:
					case UNIT_GRAM:
#ifdef USE_LIBRA_ESPANOLA_UNIT_CHANGE
						fAd *= 0.02875;//1kg = 0.02875 kg libra espanola
#else
						fAd *= 0.028349;// 1 oz = 0.028349 kg
#endif
						break;
#ifdef USE_UNIT_1_16OZ
					case UNIT_1_16OZ:
						fAd *= 16;/// 1 oz = 16 (1/16 oz)
						break;
#endif
					case UNIT_LB:
						fAd *= 0.0625;// 1 oz = 0.0625 lb
						break;
#ifdef USE_VISS_UNIT_CHANGE
					case UNIT_VISS:
						fAd *= 0.028349 * 0.612245;///1 oz = kg(oz) * 0.612245
						break;
#endif						
					default: //UNIT_OZ
						break;
				}
				fAd *= tempFp32;
				break;
				
#ifdef USE_UNIT_1_16OZ
			case UNIT_1_16OZ:
				switch (unit2)
				{
					case UNIT_KG:
#ifdef USE_UNIT_0_1KG_CAPA_TABLE
						tempFp32 *= 10.0;//0.1kg
#endif
					case UNIT_TON:
					case UNIT_GRAM:
						fAd *= 0.001771;// 1/16 oz = 0.001771 kg
						break;
					case UNIT_OZ:
						fAd *= 0.0625;/// 1/16 oz = 0.0625 oz
						break;
					case UNIT_LB:
						fAd *= 0.003906;// 1/16 oz = 0.003906 lb
						break;
					default: //UNIT_OZ
						break;
				}
				fAd *= tempFp32;
				break;
#endif
			case UNIT_VISS:
				switch (unit2)
				{
					case UNIT_KG:
#ifdef USE_UNIT_0_1KG_CAPA_TABLE
						tempFp32 *= 10.0;//0.1kg
#endif
					case UNIT_TON:
					case UNIT_GRAM:
						fAd *= 0.612395;// 1 viss = 0.612395 kg
						break;
					case UNIT_LB:
						fAd *= 1.3501;///1 viss = 1.3501 lb
						break;
#ifdef USE_UNIT_1_16OZ
					case UNIT_1_16OZ:
						fAd *= 256.0;///1 lb = 256 (1/16 oz)
						break;
#endif
					default: //UNIT_VISS
						break;
				}
				fAd *= tempFp32;
				break;

			default:
				break;
		}
	}
	return fAd;
}
#endif
/**
********************************************************************************
* @brief       KR : 단위 적용 값을 Nomalization 하는 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : 단위 적용된 데이터(FP32로 cast)\n
*                 US : \n
*                 CN : \n
* @return     KR : Nomalized 데이터\n
*                 US : \n
*                 CN : \n
* @remark   
********************************************************************************
*/
FP32 adNomalization(FP32 unitFactorizedData)
{
	return (((unitFactorizedData - 0.5) * convSlope) + 0.5);
}

/**
********************************************************************************
* @brief       KR : Nomalized된 값을 내부 값으로 변환 하는 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : Nomalized 데이터(FP32로 cast)\n
*                 US : \n
*                 CN : \n
* @return     KR : 내부 값(Raw internal value)\n
*                 US : \n
*                 CN : \n
* @remark   
********************************************************************************
*/
FP32 adRawInternalization(FP32 normalizedData)
{
	return (((normalizedData - 0.5) - ((FP32)CalParam.calZeroValue * convSlope)) + 0.5);
}

/**
********************************************************************************
* @brief       KR : Zero Tracking 수행 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : 0.25d값, Display Filtered data\n
*                 US : 0.25d value, Display Filtered data\n
*                 CN : \n
* @return   none
* @remark   
********************************************************************************
*/
void adZeroTracking(INT32S dispFilteredData)
{
	INT16U zeroBufBackup;
	static INT8U count = 0;

	count++;
	
	if (count >= zeroTrackingEnterCount)
	{
		count = 0;
		if (AdData.stableFlag)
		{
			zeroBufBackup = zeroBuf;			// Backup
			if (AbsInt32s(dispFilteredData - zeroBuf) < (INT32S)(zeroTrackingRange) && (dispFilteredData - zeroBuf) != 0 && (RangeParam.zeroTrackingType == ZERO_TRACKING_TYPE_GROSS)) 
			{									// Zero tracking (0.5d)	// Modified by CJk 20051208
				if ((dispFilteredData - zeroBuf) > 0)
				{
					zeroBuf++;
				}
				else 
				{
					zeroBuf--;
				}
			}
			// Zero Tracking 2 (Net Zero)
			else if (AbsInt32s(dispFilteredData - zeroBuf - tareBuf) < (INT32S)(zeroTrackingRange) && (dispFilteredData - zeroBuf - tareBuf) != 0 && (RangeParam.zeroTrackingType == ZERO_TRACKING_TYPE_NET)) 
			{										// Zero tracking (0.5d)	// Modified by CJk 20051208
				if (dispFilteredData - zeroBuf - tareBuf > 0) 
				{
					zeroBuf++;
				}
				else 
				{
					zeroBuf--;
				}
			}
			if (AbsInt32s(zeroBuf) > rezeroLimit) 
			{
				zeroBuf = zeroBufBackup;	// Restore
			}
		}
	}
}

/**
********************************************************************************
* @brief       KR : 초기영점값을 체크하는 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : 무게값\n
*                 US : \n
*                 CN : \n
* @return   none
* @remark   
********************************************************************************
*/
void adCheckInitZeroProc()
{
	static INT8U stableCount = 0;

	if (!flagInitZero)
	{	
		if (AdData.stableFlag)
		{
			stableCount++;
		}
		else
		{
			stableCount = 0;
		}

		if (stableCount >= AD_INIT_STABLE_CHECK_COUNT)
		{
#ifdef USE_WEIGHT_BACKUP
			if (DispFiltParam.zeroBufBackupEnable == 1)
			{
				flagInitZero = 1;
				AdData.overloadFlag = 0;
				if (DispFiltParam.zeroBufBackupValue == 0)
				{
					initZeroBuf = AdData.rawInternalData;
					initZeroBufCalUnit = adConvInternalUnit(initZeroBuf, currUnit, CalParam.calUnit);
					DispFiltParam.zeroBufBackupValue = initZeroBufCalUnit;
					DispFiltParam.tareBufBackupValue = 0;
					DispFiltWriteParam();
				}
				else
				{
					initZeroBufCalUnit = DispFiltParam.zeroBufBackupValue;
					initZeroBuf = adConvInternalUnit(initZeroBufCalUnit, CalParam.calUnit, currUnit);
					tareBufCalUnit = DispFiltParam.tareBufBackupValue;
					tareBuf = adConvInternalUnit(tareBufCalUnit, CalParam.calUnit, currUnit);
				}
			}
			else
			{
	#ifdef USE_CHANGE_ZERO_RANGE_SETTING
				if((AdData.rawInternalData <= initialZeroRangePlus) && (AdData.rawInternalData >= initialZeroRangeMinus))	//(-)Zero범위와 (+)Zero 범위 사이인 경우
				{
	#else
				if(AdData.rawInternalData <= initialZeroRange)	
				{
	#endif
					flagInitZero = 1;
					initZeroBuf = AdData.rawInternalData;
					initZeroBufCalUnit = adConvInternalUnit(initZeroBuf, currUnit, CalParam.calUnit);
					AdData.overloadFlag = 0;
				}
				else
				{
					AdData.overloadFlag = 1;
				}
			}
#else		
	#ifdef USE_CHANGE_ZERO_RANGE_SETTING
			if((AdData.rawInternalData <= initialZeroRangePlus) && (AdData.rawInternalData  >= initialZeroRangeMinus))	//(-)Zero범위와 (+)Zero 범위 사이인 경우
			{
	#else
			if(AdData.rawInternalData <= initialZeroRange)	
			{
	#endif
				flagInitZero = 1;
				initZeroBuf = AdData.rawInternalData;
				initZeroBufCalUnit = adConvInternalUnit(initZeroBuf, currUnit, CalParam.calUnit);
				AdData.overloadFlag = 0;
			}
			else
			{
				AdData.overloadFlag = 1;
			}
#endif
		}
	}
}

/**
********************************************************************************
* @brief       KR : Gross 영점값을 체크하는 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : 0.25d값, 영점 설정 값
*                 US : \n
*                 CN : \n
* @return   none
* @remark   
********************************************************************************
*/
BOOLEAN adCheckGrossZero(INT32S setZeroData)
{
	if (((AbsInt32s(setZeroData) * 2) <= (INT32S)zeroCheckingRange))
	{
		return 1;
	}
	else
	{
		return 0;		
	}
}

/**
********************************************************************************
* @brief       KR : Tare 설정 가능 점검 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : 현재 용기 설정 값, 새로운 용기 설정 값\n
*                 US : \n
*                 CN : \n
* @return     KR : 가능 여부 (True or False)\n
*                 US : \n
*                 CN : \n
* @remark   
********************************************************************************
*/
BOOLEAN adTareCheckCondition(INT32S currTare, INT32S newTare)
{
	if (currTare == 0) 
	{
		return TRUE;
	}
	if ( (RangeParam.tareType == TARE_TYPE_SUCCESSIVE_MINUS && currTare >= newTare)
			|| (RangeParam.tareType == TARE_TYPE_SUCCESSIVE_PLUS && currTare <= newTare)
			|| (RangeParam.tareType == TARE_TYPE_SUCCESSIVE_ALL))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/**
********************************************************************************
* @brief       KR : Net zero 체크 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : 0.25d, 용기 설정 완료 값\n
*                 US : \n
*                 CN : \n
* @return   none
* @remark   
********************************************************************************
*/
BOOLEAN adCheckNetZero(INT32S setTareData)
{
	if (((AbsInt32s(setTareData) * 2) <= (INT32S)zeroCheckingRange))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
********************************************************************************
* @brief       KR : Overload 체크 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : 용기 설정 완료 값\n
*                 US : \n
*                 CN : \n
* @return   none
* @remark   
********************************************************************************
*/
BOOLEAN adCheckOverload(INT32S setTareData)
{
	INT32S intVal;

	intVal = adExtToIntValue((INT32S)(currDispCapaForDisp + (INT32S)(RangeParam.overloadLimit * divHigh)));
	if ((setTareData + tareBuf) > intVal ) 
	{
		return 1;
	}
	else 
	{
		return 0;
	}
}

/**
********************************************************************************
* @brief       KR : Under Flow 체크 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : 용기 설정 완료 값\n
*                 US : \n
*                 CN : \n
* @return   none
* @remark   Weight data -20d 이하 인 경우 체크 (2006년판 OIML법규)
		     Tare 설정일 경우  TARE + 20d이하일 경우 체크
********************************************************************************
*/
BOOLEAN adCheckUnderload(INT32S setTareData)
{
	/* Minus Weight Value(-20d) cannot display (OIML) */
	INT32S intVal;
	
#ifdef USE_UNDERLOAD_10DIGIT
	if(UnderLoad10dEnable)
	{
		if (CalParam.dualInterval)
		{
			intVal = adExtToIntValue(-(INT32S)(AD_UNDER_10LIMIT * divLow));	//-10d
		}
		else
		{
			intVal = adExtToIntValue(-(INT32S)(AD_UNDER_10LIMIT * divHigh));	//-10d
		}
	}
	else
	{
		if (CalParam.dualInterval)
		{
			intVal = adExtToIntValue(-(INT32S)(AD_UNDER_LIMIT * divLow));	//-20d
		}
		else
		{
			intVal = adExtToIntValue(-(INT32S)(AD_UNDER_LIMIT * divHigh));	//-20d
		}
	}
#else
	if (CalParam.dualInterval)
	{
		intVal = adExtToIntValue(-(INT32S)(AD_UNDER_LIMIT * divLow));	//-20d
	}
	else
	{
		intVal = adExtToIntValue(-(INT32S)(AD_UNDER_LIMIT * divHigh));	//-20d
	}
#endif
	if ((setTareData + tareBuf) < intVal)
	{
		return 1;
	}
	else 
	{
		return 0;
	}																									
}

/**
********************************************************************************
* @brief       KR : Percent tare 구하는 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : Overload검사 완료 된 AD값\n
*                 US : \n
*                 CN : \n
* @return     KR : Percent tare적용된 AD값\n
*                 US : \n
*                 CN : \n
* @remark   
********************************************************************************
*/
INT32S adPercentTareProc(INT32S setTareData)
{
	if (tarePercent)
	{
		if (setTareData > 0)
		{
			tareBufPercent = setTareData * (INT32S)tarePercent / 100L;
			tareBufPercentCalUnit = adConvInternalUnit(tareBufPercent, currUnit, CalParam.calUnit);
		}
		else
		{
			tareBufPercent = 0;
			tareBufPercentCalUnit = 0;
		}
		setTareData = setTareData - tareBufPercent;
		return setTareData;
	}
	else
	{
		tareBufPercent = 0;
		tareBufPercentCalUnit = 0;
		setTareData = setTareData - tareBufPercent;
		return setTareData;
	}
}


/**
********************************************************************************
* @brief       KR : 내부값을 외부값으로 변경하는 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : interVal : 내부값\n
*                 US : \n
*                 CN : \n
* @return    KR : 외부값\n
*                 US : \n
*                 CN : \n
* @remark   
********************************************************************************
*/
INT32S adIntToExtValue(INT32S interVal)
{
	data INT16U div;
	data INT32S val;

	if (CalParam.dualInterval)
	{
		if (AbsInt32s(interVal) > dualThreshold)
		{
			div = divHigh;
		}
		else
		{
			div = divLow;
		}		
	}
	else
	{
		div = divHigh;				
	}
	
//	val = adExtToIntValue(div);
//	val = interVal * 10l / val;
// For example, div = 7.5
	val = adExtToIntValue(div * 10);
	val = interVal * 100l / val;

	if (val > 0)
	{
		val += 5;
	}
	else
	{
		val -= 5;
	}
	val /= 10l;

	val = val * div;
	
	return (val);
}

/**
********************************************************************************
* @brief       KR : 외부값을 내부값으로 변환 하는 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : 외부값\n
*                 US : \n
*                 CN : \n
* @return     KR : 내부값\n
*                 US : \n
*                 CN : \n
* @remark   resolution 64000 이상이면 계산 overflow 발생 주의
********************************************************************************
*/
INT32S adExtToIntValue(INT32S exterVal)
{	
	INT32S tempInt32s;
	INT32S div;

	div = exterVal / 32000;
	exterVal = exterVal % 32000;
			
	tempInt32s = CalParam.resolution * 32000;
	tempInt32s /= currDispCapaForCalc;
	tempInt32s = tempInt32s * div;

	exterVal = CalParam.resolution * exterVal;
	exterVal /= currDispCapaForCalc;
	exterVal = exterVal + tempInt32s;

	return (exterVal);
}

/**
********************************************************************************
* @brief       KR : Dual interval사용시에 상위 구간에서 유효하지 않은 한자리를 삭제하는  함수\n
*                 US : \n
*                 CN : \n
* @param    KR : interVal : 내부값\n
*                 US : \n
*                 CN : \n
* @return     KR : 삭제여부\n
*                 US : \n
*                 CN : \n
* @remark   
********************************************************************************
*/
BOOLEAN adChecklastDigitIsInvalid(INT32S weightVal)
{
	if (CalParam.dualInterval)
	{
		if (divHigh == 10)
		{
			if ((AbsInt32s(weightVal) >= adIntToExtValue(dualThreshold)) && (AdData.weightDecPos != 0))
			{
				return 1;
			}
		}
	}
	return 0;
}

/**
********************************************************************************
* @brief       KR : 용기 최대값을 변경하는 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : 단위값\n
*                 US : \n
*                 CN : \n
* @return   none
* @remark   KR : 기준은 calUnit 임\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void adTareChangeLimit(INT8U currUnit)
{
	if(RangeParam.tareLimitType == TARE_LIMIT_TYPE_USER)
	{
		if (currUnit == UNIT_LB)
		{
            if(CalParam.dualInterval)
            {
                if(RangeParam.tareLimit > dualThreshold)
                    currTareLimit = RangeParam.tareLimitLb  + (adExtToIntValue(divHigh) / 2) - 1;
                else
                    currTareLimit = RangeParam.tareLimitLb  + (adExtToIntValue(divLow) / 2) - 1;
                    
            }
            else
			    currTareLimit = RangeParam.tareLimitLb  + d0_5 - 1;
		}
		else	 //UNIT_KG, UNIT_G
		{
            if(CalParam.dualInterval)
            {
                if(RangeParam.tareLimit > dualThreshold)
                    currTareLimit = RangeParam.tareLimit  + (adExtToIntValue(divHigh) / 2) - 1;
                else
                    currTareLimit = RangeParam.tareLimit  + (adExtToIntValue(divLow) / 2) - 1;
                    
            }
            else
			    currTareLimit = RangeParam.tareLimit  + d0_5 - 1;
		}
	}
	else //TARE_LIMIT_TYPE_DEFAULT
	{		
		//TODO : 현재 dual일 경우 division 변경점으로 고정했음
		if (CalParam.dualInterval)
		{
			currTareLimit = dualThreshold - d0_5 - 1;
		}
		else		//TODO : single일 경우 cal unit 기준의 setting 된 값으로 변경됨(옳은가?)
		{
			currTareLimit = adExtToIntValue((INT32S)(currDispCapaForDisp)) + d0_5 - 1;
		}
	}
}


/**
********************************************************************************
* @brief       KR : Weight Unit을 변경해주는 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : 변경하고자 하는 Unit (AdData.h에 선언)\n
*                 US : \n
*                 CN : \n
* @return     KR : 변경 여부 (0-변경안됨, 1-변경됨)\n
*                 US : \n
*                 CN : \n
* @remark   KR : divTableIndex, capaIndex는 미리 정해져야 함\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
BOOLEAN adSetWeightUnit(INT8U weightUnit)
{
	INT8S offset;

#ifdef USE_UNIT_1_16OZ
	if(weightUnit > UNIT_1_16OZ) return FALSE;
#elif defined(USE_UNIT_TON_NEWTON_DAN_KN)
	if(weightUnit > UNIT_DAN) return FALSE;
	if(weightUnit == UNIT_GRAM) return FALSE;
	if(weightUnit == UNIT_OZ) return FALSE;
	if(weightUnit == UNIT_1_16OZ) return FALSE;
#elif defined (USE_UNIT_KG_ONLY)
	if(weightUnit > UNIT_DAN) return FALSE;
	if(weightUnit == UNIT_GRAM) return FALSE;
	if(weightUnit == UNIT_OZ) return FALSE;
	if(weightUnit == UNIT_1_16OZ) return FALSE;
#elif defined (USE_VISS_UNIT_CHANGE)
	if(weightUnit > UNIT_VISS) return FALSE;
#else    
	if(weightUnit > UNIT_TON) return FALSE;
#endif

	if (CapaTable[capaIndex][weightUnit].capa == 0) 
	{
		return FALSE;
	}

	currUnit = weightUnit;
	CalParam.usingUnit = currUnit;
	AdData.weightUnit = currUnit;

	offset = CapaTable[capaIndex][currUnit].weightDecPosOffset - CapaTable[capaIndex][CalParam.calUnit].weightDecPosOffset;
	AdData.weightDecPos = CalParam.weightDecPos + offset;	

	offset = CapaTable[capaIndex][currUnit].divTableOffset - CapaTable[capaIndex][CalParam.calUnit].divTableOffset;
	if (AdData.weightDecPos < 0) 
	{
		offset = offset + 3 * (-AdData.weightDecPos);	//1,2,5,10,20,50 (1->10)
		AdData.weightDecPos = 0;
	}

	if (CalParam.dualInterval)
	{
		if (divTableIndex + offset <= 0) 
		{
			offset = offset + 3;
			AdData.weightDecPos++;
		}
	}
	else
	{
		if (divTableIndex + offset < 0) 
		{
			offset = offset + 3;
			AdData.weightDecPos++;
		}
	}		
	
#ifdef USE_UNIT_1_16OZ
	if (weightUnit == UNIT_1_16OZ) // decimal position offset and divtable offset is equal to gram
	{
		if (AdData.weightDecPos > 0)
		{
			divHigh = divTable16oz[0]; 
			divLow = divHigh;
			AdData.weightDecPos = 0;
		}
		else
		{
			divHigh = divTable16oz[divTableIndex + offset];
			divLow = divTable16oz[divTableIndex + offset - 1];
		}
	}
	else
	{
		divHigh = divTable[divTableIndex + offset];
		divLow = divTable[divTableIndex + offset - 1];
	}
#else
	divHigh = divTable[divTableIndex + offset];
	divLow = divTable[divTableIndex + offset - 1];
#endif

	currDispCapaForCalc = (INT32S)CapaTable[capaIndex][currUnit].capa * (INT32S)DecPowerConvTable[AdData.weightDecPos];
#ifdef USE_UNIT_0_1KG_CAPA_TABLE
	if (currUnit == UNIT_KG)
	{
		currDispCapaForCalc /= 10;
	}
#endif
#ifdef USE_VISS_UNIT_CHANGE
	currDispCapaForDisp = currDispCapaForCalc;
#else
	if (weightUnit == UNIT_OZ)
	{
		currDispCapaForDisp = DispOzCapaTable[capaIndex] * (INT32S)DecPowerConvTable[AdData.weightDecPos];
	}
 #ifdef USE_UNIT_1_16OZ
	else if (weightUnit == UNIT_1_16OZ) 
	{
		currDispCapaForDisp = DispOzCapaTable[capaIndex] * 16;
	}
 #endif
	else //UNIT_KG, UNIT_LB, UNIT_GRAM, UNIT_TON
	{
		currDispCapaForDisp = currDispCapaForCalc;
	}	
#endif
	dualThreshold = (INT32S)((currDispCapaForDisp / divHigh) * divLow);
	dualThreshold = adExtToIntValue(dualThreshold);	
	AdData.maxCapa = currDispCapaForDisp;

	if (CalParam.dualInterval)
	{
		d0_5 = (INT16U)adExtToIntValue(divLow) / 2;
	}
	else
	{
		d0_5 = (INT16U)adExtToIntValue(divHigh) / 2;
	}

	AdData.d1= d0_5 * 2;

#ifdef USE_TURKEY_POS_PROTOCOL
	tareRemovableRange = d0_5 * 4;	// 2d (Turkey request)
#else
	tareRemovableRange = d0_5;
#endif
#ifdef USE_ZERO_TRACKING_RANGE_SETTING
	if (RangeParam.zeroTrackingRangeSetting > 10)
	{
		RangeParam.zeroTrackingRangeSetting	= 1;
	}
	zeroTrackingRange = d0_5 * RangeParam.zeroTrackingRangeSetting;
#elif defined USE_ZERO_TRACKING_RANGE_2D
	zeroTrackingRange = d0_5 * 4;//2d
#else
	zeroTrackingRange = d0_5;
#endif
	zeroCheckingRange = d0_5;

	initZeroBuf = adConvInternalUnit(initZeroBufCalUnit, CalParam.calUnit, currUnit);
	zeroBuf = adConvInternalUnit(zeroBufCalUnit, CalParam.calUnit, currUnit);

	tareBuf = adConvInternalUnit(tareBufCalUnit, CalParam.calUnit, currUnit);
	tareBufPercent = adConvInternalUnit(tareBufPercentCalUnit, CalParam.calUnit, currUnit);
		
	adTareChangeLimit(currUnit);//use currDispCapaForDisp

	//separate from this function, because this param is always written to eeprom in initial routine.
	//ParamWriteByte(PARAM_AD_AREA_NUM, AD_PARAM_USING_UNIT_OFFSET, CalParam.usingUnit);

	//fix Bug, when weight is near zero, some digit is wrong at short time because weight is always stable
       //so, make broken stable
	AdData.stableFlag = FALSE;
	
	return TRUE;
}


/**
********************************************************************************
* @brief       KR : Filter를 초기화 해주는 함수\n
*                 US : \n
*                 CN : \n
* @param    NONE
* @return    NONE
* @remark   
********************************************************************************
*/
void adFiltInit(void)
{
	INT32S winIncThres;
	INT32S winBreakThres;
	INT32S winShockThres;
	INT32S winThresIncThres;

//OIML 인증 
//MAFInit((adDrvSamplingRate*5)/10, (adDrvSamplingRate*25)/10, MAF_WIN_INC_THRESHOLD, MAF_WIN_BREAK_THRESHOLD);
//이전 필터와 동일한 기능 (shock 방지 안할 경우)
//TO DO : BW low capa body의 경우 shock filter 적용 불가, 확인필요 , 스폰지 추가 후 shock 적용
//SW CHINA 적용 20070816 // TO DO : 필터 setting menu
//MAFSInit((adDrvSamplingRate*4)/10, (adDrvSamplingRate*25)/10, MAF_WIN_INC_THRESHOLD, MAF_WIN_BREAK_THRESHOLD, (adDrvSamplingRate*2)/10, MAF_WIN_BREAK_THRESHOLD*2);

	if(DispFiltParam.filtIndex != 0xFF)
	{
		adMAFshockIndex = DispFiltParam.filtIndex /10;
		adMAFsetIndex = DispFiltParam.filtIndex %10;
		if (adMAFshockIndex > AD_MAF_SHOCK_TABLE_INDEX_MAX-1)
		{
			adMAFshockIndex = AD_MAF_SHOCK_TABLE_INDEX_MAX-1; 
		}
		if (adMAFsetIndex > AD_MAF_SET_TABLE_INDEX_MAX-1)
		{
			adMAFsetIndex = AD_MAF_SET_TABLE_INDEX_MAX-1;
		}
	}
	else
	{
		DispFiltParam.filtIndex = AD_FILT_DEFAULT_VALUE;		
		adMAFshockIndex = (AD_FILT_DEFAULT_VALUE / 10);
		adMAFsetIndex = (AD_FILT_DEFAULT_VALUE % 10);
		DispFiltWriteParam();		
	}

	if ((CalParam.calSpanValue - CalParam.calZeroValue) < 60000)
	{
		winIncThres = adMAFsetTable[adMAFsetIndex].incThres;
		winBreakThres = adMAFsetTable[adMAFsetIndex].breakThres;
		winShockThres = adMAFshockTable[adMAFshockIndex].shockThres;
	}
	else
	{
		winIncThres = AdNormalToRaw(adMAFsetTable[adMAFsetIndex].incThres);
		winBreakThres = AdNormalToRaw(adMAFsetTable[adMAFsetIndex].breakThres);	
		winShockThres = AdNormalToRaw(adMAFshockTable[adMAFshockIndex].shockThres);
	}
	winThresIncThres = ((CalParam.calSpanValue + CalParam.calZeroValue) * AD_DIVIDING_FACTOR) / 2;//30000 

	MAFSInit((adDrvSamplingRate*adMAFsetTable[adMAFsetIndex].min100msTime)/10,
			 (adDrvSamplingRate*adMAFsetTable[adMAFsetIndex].max100msTime)/10,
			  winIncThres,
			  winBreakThres, 
	         (adDrvSamplingRate*adMAFshockTable[adMAFshockIndex].shock100msTime)/10, 
			  winShockThres, 
			  winThresIncThres);
}

/**
********************************************************************************
* @brief       KR : AD API에서 사용되는 변수들을 초기화 하는 함수\n
*                 US : \n
*                 CN : \n
* @param    none
* @return    none
* @remark   
********************************************************************************
*/
void adInitVariable(void)
{

	adRawProcSamplingRate = AD_RAW_PROC_RATE;

	flagInitZero = 0;

	initZeroBufCalUnit = 0;
	initZeroBuf = 0;
	
	zeroBufCalUnit = 0;
	zeroBuf = 0;
	
	zeroTrackingEnterCount = (adRawProcSamplingRate*5)/10; ////2Hz

	tareBuf	= 0;
	tareBufCalUnit = 0;
	
	tareType = TARE_TYPE_WEIGHT;
	tarePercent = 0;
	tareBufPercent = 0;
	tareBufPercentCalUnit = 0;

#ifdef USE_START_UNIT_1_16OZ
	adChangeWeightUnitFlag = 0x85;
#else
	adChangeWeightUnitFlag = 0;
#endif
		
	AdData.overloadFlag = 0;
#ifndef USE_NOT_UNDERLOAD_ERROR
	AdData.underloadFlag = 0;
#endif
	AdData.stableFlag = FALSE;
	AdData.rawStableFlag = FALSE;
	AdData.initZeroError = FALSE;	//add for CASTON

	if (ParamReadByte(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_INIT_SIGNAL) == INIT_EEP_COMPLETE)
	{
		HystReadParam();
#if defined(USE_TEMP_COMP) || defined(USE_TEMP_COMP_MWIIH)
		TempReadParam();
#endif
#ifdef USE_CREEP_COMP
		CreepReadParam();
#endif
		LinearReadParam();
		CalReadParam();
		RangeReadParam();
		DispFiltReadParam();
	}
	else
	{
		AdSetDefaultParam(OIML);
	}

	adCalcConvSlope();

	LinearInitVariable();
	HystInitVariable();
#ifdef USE_CREEP_COMP
  	CreepInitVariable(adRawProcSamplingRate);
#endif
	LinearCalcVariable();

	divTableIndex = AdFindDivTableIndex(CalParam.divFactor);
	capaIndex = AdFindCapaIndex(CalParam.calUnit, CalParam.capa);

	rezeroLimit = (RangeParam.zeroPercent * CalParam.resolution / 100L); 

#ifdef USE_CHANGE_ZERO_RANGE_SETTING
	if(RangeParam.initialZeroPercent == 15)	//initial Zero가 15%인 경우 
	{	
		initialZeroRangePlus = (INT32S)(15) * CalParam.resolution / 100L;		//+15%적용	
		initialZeroRangeMinus = (INT32S)(-5) * CalParam.resolution / 100L;   //-5% 적
	}
	else 
	{
		initialZeroRangePlus = (INT32S)RangeParam.initialZeroPercent * CalParam.resolution / 100L;		
		initialZeroRangeMinus = -((INT32S)RangeParam.initialZeroPercent * CalParam.resolution / 100L);
	}
#else
	initialZeroRange = (INT32S)RangeParam.initialZeroPercent * CalParam.resolution / 100L;
#endif
	if(adSetWeightUnit(CalParam.usingUnit) == FALSE) 
	{
		adSetWeightUnit(CalParam.calUnit);
		ParamWriteByte(PARAM_AD_AREA_NUM, AD_PARAM_USING_UNIT_OFFSET, CalParam.usingUnit);
	}

	adFiltInit();
}

/**
********************************************************************************
* @brief       KR : AD Proc 중 filter 부분\n
*                 US : \n
*                 CN : \n
* @param    KR : raw ad data\n
*                 US : \n
*                 CN : \n
* @return     KR : filtered data\n
*                 US : \n
*                 CN : \n
* @remark   
********************************************************************************
*/
INT32S adFilterProc(INT32S raw)
{
	data INT32S lAd;
//	char dbgStr[12];//for test

	lAd = raw;

//Debug
	//	sprintf(dbgStr, "%ld	", lAd>>AD_DIVIDING_FACTOR_BIT);
	//	DebugStrOut(dbgStr);
	//Pre-Filter
	//lAd = MedianFiltProc(lAd);
	//lAd = FIRFiltProc(lAd);

//Debug
//		sprintf(dbgStr, "%ld	", lAd>>AD_DIVIDING_FACTOR_BIT);
//		DebugStrOut(dbgStr);

	//lAd = MAFProc(lAd); //OIML 인증 
	lAd = MAFSProc(lAd);

//Debug
//		sprintf(dbgStr, "%ld\r\n", lAd>>AD_DIVIDING_FACTOR_BIT);
//		DebugStrOut(dbgStr);
	return lAd;
}

/**
********************************************************************************
* @brief       KR : AD API에서 사용되는 함수들이 실제로 사용되어지는 부분\n
*                 US : \n
*                 CN : \n
* @param    KR : raw filtered data\n
*                 US : \n
*                 CN : \n
* @return     none
* @remark   
********************************************************************************
*/
//TODO : 보상하는 위치의 정확성 및 상대, 절대성 분석 필요
void adRawProc(INT32S rawFilteredData)
{
	INT32S numUnderDecPt;
	INT32S tempInt32s;

//--------------------------------------//
// calculate additional number under dec point 
//--------------------------------------//
	numUnderDecPt = rawFilteredData & AD_UNDER_DEC_PT_BIT_MASK;
	rawFilteredData = rawFilteredData >> AD_UNDER_DEC_PT_BIT;

//--------------------------------------//
// copy filterd data for calc. weight
//--------------------------------------//
	AdData.rawFilteredData = rawFilteredData;
#ifdef DEBUG_AD_API
	if (AdDebugFlag)
	{
		AdData.rawFilteredData = AdDebugData;
	}
#endif

//--------------------------------------//
// Stable of Raw-Filtered-Ad
//--------------------------------------//	
	AdData.rawStableFlag = adCheckRawStable(AdData.rawFilteredData);

//--------------------------------------//
// Hysteresis Calibration(zero,middle,full) 하는 위치 
//--------------------------------------//

//	adHystCalibration(AdData.rawFilteredData);	

//--------------------------------------//
// Hysteresis Compensation 
//--------------------------------------//
	AdData.rawHystCompData = HystCompProc(AdData.rawFilteredData);

//--------------------------------------//
// linear Calibration(zero,middle,full) 하는 위치
//--------------------------------------//
//	adLinearCalibration(AdData.rawHystCompData);

//--------------------------------------//
// Creep Compensation 
//--------------------------------------//
#ifdef USE_CREEP_COMP
	AdData.rawCreepCompData = CreepCompProc(AdData.rawHystCompData, AdData.rawStableFlag);
#endif
//--------------------------------------//
// Linearization 
//--------------------------------------//	
#ifdef USE_CREEP_COMP
	AdData.rawLinearCompData = LinearCompProc(AdData.rawCreepCompData);
#else
	AdData.rawLinearCompData = LinearCompProc(AdData.rawHystCompData);
#endif
//--------------------------------------//
// Temperature Compensation 
//--------------------------------------//
#if defined(USE_TEMP_COMP) || defined(USE_TEMP_COMP_MWIIH)
	AdData.temp = TempDrvReadData();
	AdData.rawTempCompData = TempCompProc(AdData.rawLinearCompData, AdData.temp);
#endif
//--------------------------------------//
// Span Calibration(zero,full) 하는 위치
//--------------------------------------//	
//	adSpanCalibration(AdData.rawLinearCompData);

//--------------------------------------//
// Normalization (make internal resolution)
//--------------------------------------//	
#if defined(USE_TEMP_COMP) || defined(USE_TEMP_COMP_MWIIH)
	tempInt32s = (AdData.rawTempCompData << AD_UNDER_DEC_PT_BIT) + numUnderDecPt;
#else
	tempInt32s = (AdData.rawLinearCompData << AD_UNDER_DEC_PT_BIT) + numUnderDecPt;
#endif
	tempInt32s = (INT32S)(adNomalization((FP32)tempInt32s));
	AdData.rawNormalizedData = tempInt32s >> AD_UNDER_DEC_PT_BIT;	

//--------------------------------------//
// internalization (rawNormalizedData - calZero)
//--------------------------------------//
	AdData.rawInternalData = (INT32S)(adRawInternalization((FP32)AdData.rawNormalizedData));

//--------------------------------------//
// Unit Conversion (internal value) ex) 60,000(=15kg) -> 60,000(=30lb)
//--------------------------------------//	
	AdData.rawInternalData = (INT32S)adConvInternalUnit((FP32)AdData.rawInternalData, CalParam.calUnit, currUnit);

//--------------------------------------//
// Stable Check
//--------------------------------------//
	AdData.rawDispFilteredData = adDispFiltAndCheckStable(AdData.rawInternalData);

//--------------------------------------//
// Set Zero
//--------------------------------------//
	AdData.rawInitZeroData = AdData.rawDispFilteredData - initZeroBuf;

//--------------------------------------//
// Zero Tracking
//--------------------------------------//
	adZeroTracking(AdData.rawInitZeroData);	
	AdData.rawSetZeroData = AdData.rawInitZeroData - zeroBuf;	

//--------------------------------------//
// Zero check 1 (Gross Zero : 0.25d이내)
//--------------------------------------//
	AdData.grossZeroFlag = adCheckGrossZero(AdData.rawSetZeroData);	

//--------------------------------------//
// Set Tare
//--------------------------------------//
	AdData.rawSetTareData = AdData.rawSetZeroData - tareBuf;

//--------------------------------------//
// Zero check 2 (Net Zero : 0.25d이내)
//--------------------------------------//
	AdData.netZeroFlag = adCheckNetZero(AdData.rawSetTareData);

//--------------------------------------//
// Overload check
//--------------------------------------//
	AdData.overloadFlag = adCheckOverload(AdData.rawSetTareData);

//--------------------------------------//
// Underload check
//--------------------------------------//
	AdData.underloadFlag = adCheckUnderload(AdData.rawSetTareData);

//--------------------------------------//
// Set Percent Tare
//--------------------------------------//
	AdData.rawSetTareData = adPercentTareProc(AdData.rawSetTareData);

//--------------------------------------//
// Internal to external (weight)
//--------------------------------------//
	AdData.weightData = adIntToExtValue(AdData.rawSetTareData);

//--------------------------------------//
// Check Last digit valid
//--------------------------------------//
	AdData.lastDigitIsInvalid = adChecklastDigitIsInvalid(AdData.weightData);

//--------------------------------------//
// external tare weight
//--------------------------------------//
	AdData.tareWeight = adIntToExtValue(tareBuf + tareBufPercent);
}


/*
********************************************************************************
						AD Parameter Area
********************************************************************************
*/
/** @brief  KR : Calibration structure를 사용하기 위한 변수\n
		   US :  \n
		   CN :											  */
CAL_PARAM_STRUCT CalParam;

/** @brief  KR : Range structure를 사용하기 위한 변수\n
		   US :  \n
		   CN :										   */
RANGE_PARAM_STRUCT RangeParam;

/** @brief  KR : display filter structure를 사용하기 위한 변수\n
		   US :  \n
		   CN :										   */
DISP_FILTER_PARAM_STRUCT DispFiltParam;

/**
********************************************************************************
* @brief       KR : Calibration parameter들의 default 값을 세팅 해주는 함수\n
*                 US : \n
*                 CN : \n
* @param    NONE
* @return     NONE
* @remark   
********************************************************************************
*/
void CalSetDefaultParam(void)
{
	CalParam.calUnit = 0;		// 0 : kg	1 : lb
	CalParam.usingUnit = 0;		// 0 : kg	1 : lb
	CalParam.dualInterval = 1;	// 0:single, 1:dual
#ifdef USE_UNIT_0_1KG_CAPA_TABLE
	CalParam.capa = 150;
#else
	CalParam.capa = 15;
#endif
	CalParam.resolution = 60000;
	CalParam.calZeroValue = 0;		// Zero	
	CalParam.calSpanValue = 60000;	// Full	
	CalParam.calZeroLimit = 0;				
#ifdef USE_AD_SPAN_LIMIT_PR_PLUS
	CalParam.calSpanLimit = CAL_SPAN_LIMIT_VALUE;
#else
	CalParam.calSpanLimit = 370000L;
#endif
#ifdef USE_DEFAULT_KOREA_GRAVITY
	CalParam.calG = 7994;			//상해(자산공장) = 9.7940, 한국 = 9.7994
	CalParam.usingG = 7994;		
#else
	CalParam.calG = 7940;			//상해(자산공장) = 9.7940, 한국 = 9.7994
	CalParam.usingG = 7940;		
#endif
	CalParam.divFactor = 5;
	CalParam.weightDecPos = 3;
	CalParam.initZeroResetEnable = 1;
	CalWriteParam();
}

/**
********************************************************************************
* @brief       KR : Calibration parameter들을 EEPROM으로 Write하는 함수\n
*                 US : \n
*                 CN : \n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void CalWriteParam(void)
{
	ParamWriteData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_CAL, (INT8U *)&CalParam, sizeof(CalParam));
	adCalcConvSlope();
}

/**
********************************************************************************
* @brief       KR : EEPROM의 Calibration parameter들을 RAM으로 Read하는 함수\n
*                 US : \n
*                 CN : \n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void CalReadParam(void)
{
	ParamReadData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_CAL, (INT8U *)&CalParam, sizeof(CalParam));
}

/**
********************************************************************************
* @brief       KR : Range parameter들의 default 값을 세팅 해주는 함수\n
*                 US : \n
*                 CN : \n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void RangeSetDefaultParam(void)
{
	RangeParam.overloadLimit = 9;
#ifdef USE_FIFTEEN_ZERO_RANGE
	RangeParam.initialZeroPercent = 15;
#elif defined (USE_THREE_ZERO_RANGE)
	RangeParam.initialZeroPercent = 3;
#else
	RangeParam.initialZeroPercent = 10;
#endif
#ifdef USE_SRILANKA_ZERO_RANGE
	RangeParam.zeroPercent = 3;
#else
	RangeParam.zeroPercent = 2;
#endif
#ifdef USE_SRILANKA_TARE		// Half Tare
	RangeParam.tareLimit = 30000; 
	RangeParam.tareLimitLb = 30000;
#else
	RangeParam.tareLimit = 60000; 
	RangeParam.tareLimitLb = 60000;
#endif	
	RangeParam.tareType = TARE_TYPE_PROPER;
	RangeParam.zeroMarkType = ZERO_MARK_TYPE_GROSS;
	RangeParam.zeroTrackingType = ZERO_TRACKING_TYPE_GROSS;
#ifdef USE_ZERO_TRACKING_RANGE_SETTING
	RangeParam.zeroTrackingRangeSetting = 1;
#endif
	RangeWriteParam();
}

/**
********************************************************************************
* @brief       KR : Range parameter들을 EEPROM으로 Write하는 함수\n
*                 US : \n
*                 CN : \n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void RangeWriteParam(void)
{
	ParamWriteData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_RANGE, (INT8U *)&RangeParam, sizeof(RangeParam));
}

/**
********************************************************************************
* @brief       KR : EEPROM의 Range parameter들을 RAM으로 Read하는 함수\n
*                 US : \n
*                 CN : \n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void RangeReadParam(void)
{
	ParamReadData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_RANGE, (INT8U *)&RangeParam, sizeof(RangeParam));
}

/**
********************************************************************************
* @brief       KR : Display filter parameter들의 default 값을 세팅 해주는 함수\n
*                 US : \n
*                 CN : \n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void DispFiltSetDefaultParam(void)
{
	DispFiltParam.stableMotionBand = 8; 	// Motion Band stable 기준 AD 값
	DispFiltParam.dispUpdateTimeForSmallChange = AD_RAW_PROC_RATE; 	// disp update stable일때 조금 변할 때, Disp갱신 시간
	DispFiltParam.dispUpdateTimeForLargeChange = AD_RAW_PROC_RATE/2; 	// disp update stable일때 많이 변할 때, Disp갱신 시간  
	DispFiltParam.stableUpdateTimeForEntering = AD_RAW_PROC_RATE/2;	// unstable에서 stable로 갈 조건, motion band 안에 들어야하는 시간 
	DispFiltParam.filtIndex = AD_FILT_DEFAULT_VALUE;
	//DispFiltParam.overLoadCount = 0; // 사용하지 않음 CAL HISTORY 영역으로  이동 (v2.14 이후)

#ifdef AUTO_ZERO
    DispFiltParam.autozeroIndex = 0x00;
#endif
    
#ifdef USE_WEIGHT_BACKUP
	DispFiltParam.zeroBufBackupEnable = 0;
	DispFiltParam.zeroBufBackupValue = 0;
	DispFiltParam.tareBufBackupValue = 0;
#endif

	DispFiltWriteParam();
}

/**
********************************************************************************
* @brief       KR : Display filter parameter들을 EEPROM으로 Write하는 함수\n
*                 US : \n
*                 CN : \n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void DispFiltWriteParam(void)
{
	ParamWriteData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_DISP_FILT, (INT8U *)&DispFiltParam, sizeof(DispFiltParam));
}

/**
********************************************************************************
* @brief       KR : EEPROM의 Display filter parameter들을 RAM으로 Read하는 함수\n
*                 US : \n
*                 CN : \n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void DispFiltReadParam(void)
{
	ParamReadData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_DISP_FILT, (INT8U *)&DispFiltParam, sizeof(DispFiltParam));
}

/**
********************************************************************************
* @brief    내부값으로 안정상태를 검사하고 Display Filter를 수행하는 함수
* @param    rawInternalData
* @return   Display Filtered data
* @remark   
********************************************************************************
*/
INT32S adDispFiltAndCheckStable(INT32S rawInternalData)
{
	/** @brief  KR :  이전 디스플레이된 Raw ad 데이터 */
	static INT32S prevDispRawAd = 0;
	
	/** @brief  KR :  이전 필터링된 Raw ad 데이터 */
	static INT32S prevFiltRawAd = 0;
	
	/** @brief  KR :  disp update stable일때 조금 변할 때, Disp갱신 시간 counter\n
			            unstable 에서 stable로 변할 때 motion Band에 들어오는 시간 counter */
	static INT8U cntUpdateTime1 = AD_RAW_PROC_RATE/2;
	
	/** @brief  KR :  disp update stable일때 많이 변할 때, Disp갱신 시간 counter */
	static INT8U cntUpdateTime2 = AD_RAW_PROC_RATE/2;
	
	INT32S longValue;

	longValue = AbsInt32s(rawInternalData - prevDispRawAd);	
	if (AdData.stableFlag) //현재 stable 이면
	{
		if (longValue < DispFiltParam.stableMotionBand)	//Stable 범위 안에서
		{
			if (longValue < 2) //현재 Display 값과 차이가 작으면(0~1)
			{
				cntUpdateTime2 = DispFiltParam.dispUpdateTimeForLargeChange;
				if (--cntUpdateTime1 == 0) //업데이트1 시간 후 현재값 Display
				{
					cntUpdateTime1 = DispFiltParam.dispUpdateTimeForSmallChange;
					prevFiltRawAd = rawInternalData;
				}
				else //업데이트1 시간 전 까지 이전 Display 유지
				{
					rawInternalData = prevDispRawAd;
				}
			}
			else  //현재 Display 값과 차이가 2이상이고
			{
				cntUpdateTime1 = DispFiltParam.dispUpdateTimeForSmallChange;	
				longValue = AbsInt32s(rawInternalData - prevFiltRawAd);
				if (longValue < 2) //이전 필터값과 차이가 작으면(0~1)
				{
					if (--cntUpdateTime2 == 0) //업데이트2 시간 후 현재 값 Display
					{
						cntUpdateTime2 = DispFiltParam.dispUpdateTimeForLargeChange;	
						prevFiltRawAd = rawInternalData;
					}
					else //업데이트2 시간 전 까지 이전 Display 유지
					{
						rawInternalData = prevDispRawAd;
					}
				}
				else  //이전 필터값과 차이가 2이상이면 이전 Display 유지
				{
					cntUpdateTime2 = DispFiltParam.dispUpdateTimeForLargeChange;	
					prevFiltRawAd = rawInternalData;
					rawInternalData = prevDispRawAd;
				}
			}
		}
		else //Stable 범위 밖이면 Unstable 상태로 변경 후 현재 값 Display
		{
			AdData.stableFlag = FALSE;
			cntUpdateTime1 = DispFiltParam.stableUpdateTimeForEntering;	
			prevFiltRawAd = rawInternalData;
		}
	}
	else  //현재 Unstable 이면
	{
		if (longValue < DispFiltParam.stableMotionBand)	//Stable 범위 안에서
		{
			if (--cntUpdateTime1 == 0) //Stable 시간 확인 뒤 Stable 상태로 변경, 현재 값 Display
			{
				cntUpdateTime1 = DispFiltParam.dispUpdateTimeForSmallChange;	
				cntUpdateTime2 = DispFiltParam.dispUpdateTimeForLargeChange;	
				AdData.stableFlag = TRUE;
				prevFiltRawAd = rawInternalData;
			}
			else //현재 값 Display 
			{
				
			}
		}
		else //Stable 범위 밖이면 현재 값 Display
		{
			cntUpdateTime1 = DispFiltParam.stableUpdateTimeForEntering;
			prevFiltRawAd = rawInternalData;
		}
	}
	prevDispRawAd = rawInternalData;
	
	return rawInternalData;
}
