/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD API Header File\n
* @file    AdApi.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
*          v1.0 2006/11/01 modified by kkwan \n
********************************************************************************
*/

#ifndef _AD_API_H
#define _AD_API_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */

/** @brief KR :Unstable\n
		  US : Unstable\n
		  CN :									*/
#define UNSTABLE	0

/** @brief KR : stable\n
		  US : stable\n
		  CN :									*/
#define STABLE		1

/** @brief KR : 무게Tare\n
		  US : \n
		  CN :									*/
#define TARE_TYPE_WEIGHT	0

/** @brief KR : Preset Tare\n
		  US : \n
		  CN :									*/
#define TARE_TYPE_PRESET	1

/** @brief KR : CAPA에 적합한 Tare limit\n
		  US : \n
		  CN :									*/
#define TARE_TYPE_PROPER				0

/** @brief KR : (+)방향으로 연속 Tare setting\n
		  US : \n
		  CN :									*/
#define TARE_TYPE_SUCCESSIVE_PLUS		1

/** @brief KR : (-)방향으로 연속 Tare setting\n
		  US : \n
		  CN :									*/
#define TARE_TYPE_SUCCESSIVE_MINUS	2

/** @brief KR : (+),(-)방향으로 연속 Tare setting\n
		  US : \n
		  CN :									*/
#define TARE_TYPE_SUCCESSIVE_ALL		3

/** @brief KR : dual일 경우 변경점, single일 경우 full\n
		  US : \n
		  CN :									*/
#define TARE_LIMIT_TYPE_DEFAULT		0

/** @brief KR : user가 tare 내부값 제한\n
		  US : \n
		  CN :									*/
#define TARE_LIMIT_TYPE_USER			1

/** @brief KR : Gross zero marking\n
		  US : \n
		  CN :									*/
#define ZERO_MARK_TYPE_GROSS			0

/** @brief KR : Net zero marking\n
		  US : \n
		  CN :									*/
#define ZERO_MARK_TYPE_NET			1

/** @brief KR : Gross and Net zero marking\n
		  US : \n
		  CN :									*/
#define ZERO_MARK_TYPE_ALL				2

/** @brief KR : Gross zero tracking\n
		  US : \n
		  CN :									*/
#define ZERO_TRACKING_TYPE_GROSS		0

/** @brief KR : Net zero tracking\n
		  US : \n
		  CN :									*/
#define ZERO_TRACKING_TYPE_NET		1

/** @brief KR : Error없음\n
		  US : \n
		  CN :									*/
#define ERR_NONE			0x00

/** @brief KR : Calibration unstable error\n
		  US : Calibration unstable error\n
		  CN :									*/
#define ERR_CAL_UNSTABLE	0x01

/** @brief KR : Over range error\n
		  US : Over range error\n
		  CN :									*/
#define ERR_OVER_RANGE		0x02

/** @brief KR : Invalid value error\n
		  US : Invalid value error\n
		  CN :									*/
#define ERR_INVALID_VALUE	0x03

/** @brief KR : Unstable error\n
		  US : Unstable error\n
		  CN :									*/
#define ERR_UNSTABLE		0x04

/** @brief KR : Tare가 이미 설정된 상태 error\n
		  US : \n
		  CN :									*/
#define ERR_ALREADY_TARE	0x05

/** @brief KR : Tare가 없는 상태 error\n
		  US : \n
		  CN :									*/
#define ERR_NO_TARE			0x06

/** @brief KR : Calibration range error\n
		  US : Calibration range error\n
		  CN :									*/
#define ERR_CAL_RANGE		0x07

/** @brief KR : Tare type error\n
		  US : Tare type error\n
		  CN :									*/
#define ERR_TARE_TYPE		0xB0

/** @brief KR : Tare interval error\n
		  US : Tare interval error\n
		  CN :									*/
#define ERR_TARE_INTERVAL	0xB0

/** @brief KR : Calibration Parameter offset\n
		  US : Calibration Parameter offset\n
		  CN :									*/
#define AD_PARAM_OFFSET_CAL		0
/** @brief KR : Calibration Parameter size\n
		  US : Calibration Parameter size\n
		  CN :									*/
#define PARAM_CAL_AREA_SIZE	0x20//32 > 32


/** @brief KR : Range Parameter offset\n
		  US : Range Parameter offset\n
		  CN :									*/
#define AD_PARAM_OFFSET_RANGE		(AD_PARAM_OFFSET_CAL + PARAM_CAL_AREA_SIZE)
/** @brief KR : Range Parameter size\n
		  US : Range Parameter size\n
		  CN :									*/
#define PARAM_RANGE_AREA_SIZE		0x18//24 > 18


/** @brief KR : Display filter Parameter offset\n
		  US : Display filter Parameter offset\n
		  CN :									*/
#define AD_PARAM_OFFSET_DISP_FILT	(AD_PARAM_OFFSET_RANGE + PARAM_RANGE_AREA_SIZE)
/** @brief KR : Display filter Parameter size\n
		  US : Display filter Parameter size\n
		  CN :									*/
#define PARAM_DISP_FILT_AREA_SIZE	0x10//16 > 15


/** @brief KR : Hysteresis Parameter offset\n
		  US : Hysteresis Parameter offset\n
		  CN :									*/
#define AD_PARAM_OFFSET_HYST		(AD_PARAM_OFFSET_DISP_FILT + PARAM_DISP_FILT_AREA_SIZE)
/** @brief KR : Hysteresis Parameter size\n
		  US : Hysteresis Parameter size\n
		  CN :									*/
#define PARAM_HYST_AREA_SIZE		0x18//24 > 17


/** @brief KR : Temperature compensation Parameter offset\n
		  US : Temperature compensation Parameter offset\n
		  CN :									*/
#define AD_PARAM_OFFSET_TEMP		(AD_PARAM_OFFSET_HYST + PARAM_HYST_AREA_SIZE)
/** @brief KR : Temperature compensation Parameter size\n
		  US : Temperature compensation Parameter size\n
		  CN :									*/
#define PARAM_TEMP_AREA_SIZE		0x20//32 > 30


/** @brief KR : Creep compensation Parameter offset\n
		  US : Creep compensation Parameter offset\n
		  CN :									*/
#define AD_PARAM_OFFSET_CREEP		(AD_PARAM_OFFSET_TEMP + PARAM_TEMP_AREA_SIZE)
/** @brief KR : Creep compensation Parameter size\n
		  US : Creep compensation Parameter size\n
		  CN :									*/
#define PARAM_CREEP_AREA_SIZE		0x08//8 > 3


/** @brief KR : Linear compensation Parameter offset\n
		  US : Linear compensation Parameter offset\n
		  CN :									*/
#define AD_PARAM_OFFSET_LINEAR		(AD_PARAM_OFFSET_CREEP + PARAM_CREEP_AREA_SIZE)
/** @brief KR : Linear compensation Parameter size\n
		  US : Linear compensation Parameter size\n
		  CN :									*/
#define PARAM_LINEAR_AREA_SIZE		0x10//16 = 16  full!!!


/** @brief KR : Backup Parameter offset\n
		  US : Backup Parameter offset\n
		  CN :									*/
#define AD_PARAM_OFFSET_BACKUP				(AD_PARAM_OFFSET_LINEAR + PARAM_LINEAR_AREA_SIZE)


/** @brief KR : Sum weight backup Parameter offset\n
		  US : Sum weight backup Parameter offset\n
		  CN :									*/
#define AD_PARAM_OFFSET_SUM_WEIGHT_BACKUP	(AD_PARAM_OFFSET_LINEAR + PARAM_LINEAR_AREA_SIZE)


/** @brief KR : Weight backup Parameter size\n
		  US : Weight backup Parameter size\n
		  CN :									*/
#define PARAM_WEIGHT_BACKUP_SIZE				4


/** @brief KR : Sum count backup Parameter offset\n
		  US : Sum count backup Parameter offset\n
		  CN :									*/
#define AD_PARAM_OFFSET_SUM_COUNT_BACKUP	(AD_PARAM_OFFSET_SUM_WEIGHT_BACKUP + PARAM_WEIGHT_BACKUP_SIZE)


/** @brief KR : Sum count backup Parameter size\n
		  US : Sum count backup Parameter size\n
		  CN :									*/
#define PARAM_COUNT_BACKUP_SIZE				4

#ifdef USE_CTS_MODE_OLD
/** @brief KR : Sum serialnum Parameter offset\n
		  US : Sum count backup Parameter offset\n
		  CN :									*/
#define AD_PARAM_OFFSET_SERIALNUM_BACKUP	(AD_PARAM_OFFSET_SUM_COUNT_BACKUP + PARAM_COUNT_BACKUP_SIZE)


/** @brief KR : Sum count backup Parameter size\n
		  US : Sum count backup Parameter size\n
		  CN :									*/
#define PARAM_SERIALNUM_BACKUP_SIZE				12
#endif

/** @brief KR : Backup Parameter size\n
		  US : Backup Parameter size\n
		  CN :									*/
#define PARAM_BACKUP_AREA_SIZE		0x20//32 > 8

/** @brief KR : History Parameter size\n
		  US : History Parameter size\n
		  CN :									*/
#define AD_PARAM_OFFSET_HISTORY				(AD_PARAM_OFFSET_BACKUP + PARAM_BACKUP_AREA_SIZE)

#define CAL_HISTORY_MAX_NUM					3
#define CAL_HISTORY_SIZE					21
#define AD_PARAM_OFFSET_HISTORY_COUNT		(AD_PARAM_OFFSET_HISTORY + (CAL_HISTORY_MAX_NUM*CAL_HISTORY_SIZE))
#define AD_PARAM_OFFSET_OVERLOAD_COUNT		(AD_PARAM_OFFSET_HISTORY_COUNT + 1)
#define AD_PARAM_OFFSET_OVERLOAD_VALUE		(AD_PARAM_OFFSET_OVERLOAD_COUNT + 1)

/** @brief KR : History Parameter size\n
		  US : History Parameter size\n
		  CN :									*/
#define PARAM_HISTORY_AREA_SIZE		0x46//70 = 70 (= 21 X 3 + 1 + 1 + 4 + 1(reserved))

/** @brief KR : EEPROM Initial Signal offset\n
		  US : EEPROM Initial Signal offset\n
		  CN :									*/
#define AD_PARAM_OFFSET_INIT_SIGNAL		0xFE

/** @brief KR : EEPROM Version offset\n
		  US : EEPROM Version offset\n
		  CN :									*/
#define AD_PARAM_OFFSET_INIT_VER		0xFF

/** @brief KR : EEPROM Initial complete\n
		  US : EEPROM Initial complete\n
		  CN :									*/
#define INIT_EEP_COMPLETE		0x49

/** @brief KR : EEPROM Version\n
		  US : EEPROM Version\n
		  CN :									*/
#define INIT_EEP_VER	0x01

/** @brief KR : AD Parameter total size\n
		  US : AD Parameter total size\n
		  CN :									*/
#define AD_PARAM_TOTAL_USING_SIZE	(AD_PARAM_OFFSET_HISTORY + PARAM_HISTORY_AREA_SIZE + 2)

/** @brief KR : AD Parameter total size OVER message\n
		  US : AD Parameter total size OVER message\n
		  CN :									*/
#if (AD_PARAM_TOTAL_USING_SIZE > PARAM_AD_AREA_SIZE)
#error Ad Parameter area size is over!!!!!!
#endif

/** @brief KR : Calibration unit offset\n
		  US : Calibration unit offset\n
		  CN :									*/
#define AD_PARAM_CAL_UNIT_OFFSET	(AD_PARAM_OFFSET_CAL)

/** @brief KR : Using unit offset\n
		  US : Using unit offset\n
		  CN :									*/
#define AD_PARAM_USING_UNIT_OFFSET	(AD_PARAM_OFFSET_CAL + 1)

/** @brief KR : Initial zero offset\n
		  US : Initial zero offset\n
		  CN :									*/
#define INIT_ZERO_NUM							1

/** @brief KR : Calibration Gravity offset\n
		  US : Calibration Gravity offset\n
		  CN :									*/
#define CAL_G_NUM 								2

/** @brief KR : Using Gravity offset\n
		  US : Using Gravity offset\n
		  CN :									*/
#define USING_G_NUM 							3

/** @brief KR : Hysteresis compensation enable offset\n
		  US : Hysteresis compensation enable offset\n
		  CN :									*/
#define HYST_COMP_ENABLE						4

/** @brief KR : Temperature compensation enable offset\n
		  US : Temperature compensation enable offset\n
		  CN :									*/
#define TEMP_COMP_ENABLE						5

/** @brief KR : Full tare enable offset\n
		  US : Full tare enable offset\n
		  CN :									*/
#define FULL_TARE_FLAG							6

/** @brief KR : Resolution set offset\n
		  US : Resolution set offset\n
		  CN :									*/
#define RESOLUTION								7

/** @brief KR : Successive tare enable offset\n
		  US : Successive tare enable offset\n
		  CN :									*/
#define TARE_SUCCESSIVE						8

/** @brief KR : Calibration unit offset\n
		  US : Calibration unit offset\n
		  CN :									*/
#define CAL_UNIT								9

/** @brief KR : Using unit offset\n
		  US : Using unit offset\n
		  CN :									*/
#define USING_UNIT								10

/** @brief KR : Dual interval enable offset\n
		  US : Dual interval enable offset\n
		  CN :									*/
#define DUAL_INTERVAL							11

/** @brief KR : Creep compensation enable offset\n
		  US : Creep compensation enable offset\n
		  CN :									*/
#define CREEP_COMP_TIME						12

/** @brief KR : CAPA set offset\n
		  US : CAPA set offset\n
		  CN :									*/
#define CAPA										13

/** @brief KR : Overload limit set offset\n
		  US : Overload limit set offset\n
		  CN :									*/
#define OVER_LOAD_LIMIT						14

/** @brief KR : Zero set offset\n
		  US : Zero set offset\n
		  CN :									*/
#define ZERO										15

/** @brief KR : Weight decimal point set offset\n
		  US : Weight decimal point set offset\n
		  CN :									*/
#define WEIGHT_DEC_POS							16

/** @brief KR : Division factor(1d) set offset\n
		  US : Division factor(1d) set offset\n
		  CN :									*/
#define DIV_FACTOR								17

/** @brief KR : Linear ratio set offset\n
		  US : Linear ratio set offset\n
		  CN :									*/
#define LINEAR_RATIO							18

/** @brief KR : Linear ratio set offset\n
		  US : Linear ratio set offset\n
		  CN :									*/
#define ZERO_MARK_INDICATOR					19

/** @brief KR : Creep compensation value offset\n
		  US : Creep compensation value offset\n
		  CN :									*/
#define CREEP_COMP_VALUE						20

/** @brief KR : Hyst compensation order value offset\n
											*/
#define HYST_COMP_ORDER						21




/** @brief KR : 온도보상 Calibration에서 온도 범위를 벗어나거나 온도보상을 하지 않는 상태\n
		  US : \n
		  CN :									*/
#define CAL_NO_TEMP		0

/** @brief KR : 온도보상 Calibration에서 온도 범위가 저온인 상태\n
		  US : \n
		  CN :									*/
#define CAL_LOW_TEMP	1

/** @brief KR : 온도보상 Calibration에서 온도 범위가 상온인 상태\n
		  US : \n
		  CN :									*/
#define CAL_MID_TEMP	2

/** @brief KR : 온도보상 Calibration에서 온도 범위가 고온인 상태\n
		  US : \n
		  CN :									*/
#define CAL_HIGH_TEMP	3

/** @brief KR : EXPANDED UNIT OPTION BIT MASK\n
*/
#define EXPAND_UNIT_MASK_TON        0x01
#define EXPAND_UNIT_MASK_SHORT_TON  0x02
#define EXPAND_UNIT_MASK_LONG_TON   0x04
#define EXPAND_UNIT_MASK_NEWTON     0x08
#define EXPAND_UNIT_MASK_KN         0x10
#define EXPAND_UNIT_MASK_DAN        0x20

/*
********************************************************************************
*                       GLOBAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert global typedef & struct here */

/** @brief KR : Calibration History 저장에 필요한 변수 Structure\n
		  US : \n
		  CN :									*/
typedef _PACKED_DEF struct
{
	INT8U 	id;				
	INT8U	cType;					
	INT8U	year;					
	INT8U	month;					
	INT8U	date;					
	INT32S	calZeroValue;
	INT32S	calMidUpValue;
	INT32S	calMidDownValue;
	INT32S	calSpanValue;
} CAL_HISTORY_STRUCT;

/** @brief KR : Calibration에 필요한 변수 Structure\n
		  US : \n
		  CN :									*/
typedef _PACKED_DEF struct
{
	/** @brief KR : Calibration시에 사용된 단위 0 : kg, 1 : lb\n
			  US : \n
			  CN :									*/
	INT8U 		calUnit;				

	/** @brief KR : 현재 사용하는 단위 0 : kg, 1 : lb\n
			  US : \n
			  CN :									*/
	INT8U		usingUnit;					

	/** @brief KR : Dual interval 사용 여부\n
			  US : \n
			  CN :									*/
	INT8U		dualInterval;					

	/** @brief KR :사용 Capa \n
			  US : \n
			  CN :									*/
	INT16U		capa;

	/** @brief KR :내부 Max resoulution\n
			  US : \n
			  CN :									*/
	INT32S		resolution;

	/** @brief KR : 스판 calibration 영점 값\n
			  US : \n
			  CN :									*/
	INT32S		calZeroValue;				

	/** @brief KR : 스판 calibration 스판 값\n
			  US : \n
			  CN :									*/
	INT32S		calSpanValue;				

	/** @brief KR : 스판 calibration시에 영점 제한 값\n
			  US : \n
			  CN :									*/
	INT32S		calZeroLimit;					

	/** @brief KR : 스판 calibration시에 스판 제한  값\n
			  US : \n
			  CN :									*/
	INT32S		calSpanLimit;					

	/** @brief KR : 스판 calibration시에 Calibration 시 중력 보정값 (9.XXXX)\n
			  US : \n
			  CN :									*/
	INT16S		calG;					

	/** @brief KR :스판 calibration시에 사용지 중력 보정값 (9.XXXX) \n
			  US : \n
			  CN :									*/
	INT16S		usingG;

	/** @brief KR :외부 1눈금 (1d)\n
			  US : \n
			  CN :									*/
	INT8U		divFactor;

	/** @brief KR : 무게 decimal position\n
			  US : \n
			  CN :									*/
	INT8S		weightDecPos;

	/** @brief KR : 무게 initZeroResetEnable\n
			  US : \n
			  CN :									*/
	INT8U		initZeroResetEnable;
} CAL_PARAM_STRUCT;//32 byte <= PARAM_CAL_AREA_SIZE	 //full

/** @brief KR : Tare 세팅시에 사용되는 변수 Structure\n
		  US : \n
		  CN :									*/
typedef _PACKED_DEF struct
{	
	/** @brief KR : Overload limit 값 (단위 : d)\n
			  US : \n
			  CN :									*/
	INT8U		overloadLimit;				

	/** @brief KR : 초기 제로값 범위 (단위 : %)\n
			  US : \n
			  CN :									*/
	INT8U		initialZeroPercent;				

	/** @brief KR : 제로 범위 (단위 : %)\n
			  US : \n
			  CN :									*/
	INT8U		zeroPercent;						

	/** @brief KR : Tare limit 내부값 (KG 단위)\n
			  US : \n
			  CN :									*/
	INT32S		tareLimit;						

	/** @brief KR : Tare limit 내부값 (LB 단위)\n
			  US : \n
			  CN :									*/
	INT32S		tareLimitLb;					

	/** @brief KR : Tare Type\n
			  US : \n
			  CN :									*/
	INT8U		tareType;	

	/** @brief KR : Zero Mark Type\n
			  US : \n
			  CN :									*/
	INT8U		zeroMarkType;

	/** @brief KR : Zero Tracking Type\n
			  US : \n
			  CN :									*/
	INT8U		zeroTrackingType;

	/** @brief KR : Tare Limit Type\n
			  US : \n
			  CN :									*/
	INT8U		tareLimitType;

	/** @brief KR : Zero tracking 범위 (0.5d 단위)\n
			  US : \n
			  CN :									*/
	INT8U		zeroTrackingRangeSetting;
#ifdef USE_CTS_MODE_OLD
    /** @brief KR : e값\n
			  US : \n
			  CN :									*/
	INT8U		eValue;
    /** @brief KR : d값\n
			  US : \n
			  CN :									*/
	INT8U		dValue;
#endif
}RANGE_PARAM_STRUCT;//16 byte <= PARAM_RANGE_AREA_SIZE


/** @brief KR : Display filter관련  변수 Structure\n
		  US : \n
		  CN :									*/
typedef _PACKED_DEF struct
{
	/** @brief KR : Motion Band stable 기준 AD 값\n
			  US : \n
			  CN :									*/
	INT8U		stableMotionBand;		

	/** @brief KR : disp update stable일때 조금 변할 때, Disp갱신 시간\n
			  US : \n
			  CN :									*/
	INT8U		dispUpdateTimeForSmallChange;			

	/** @brief KR : disp update stable일때 많이 변할 때, Disp갱신 시간\n
			  US : \n
			  CN :									*/
	INT8U		dispUpdateTimeForLargeChange; 			

	/** @brief KR : unstable에서 stable로 갈 조건, motion band 안에 들어야하는 시간\n
			  US : \n
			  CN :									*/
	INT8U		stableUpdateTimeForEntering;

	/** @brief KR : Filter index\n
			  US : Filter index\n
			  CN :									*/
	INT8U		filtIndex;

	/** @brief KR : init zero buffer backup enable flag\n
			  US : init zero buffer backup enable flag\n
			  CN :									*/
	INT8U 		zeroBufBackupEnable;

	/** @brief KR : init zero buffer backup cal unit internal value\n
			  US : init zero buffer backup cal unit internal value\n
			  CN :									*/
	INT32S		zeroBufBackupValue;					

	/** @brief KR : tare buffer backup cal unit internal value\n
			  US : tare buffer backup cal unit internal value\n
			  CN :									*/
	INT32S		tareBufBackupValue;	

	/** @brief KR : Over load Counter\n
			  US : Over load Counter\n
			  CN :									*/
	INT8U 		autozeroIndex;//overLoadCount < ver 2.14;					
}DISP_FILTER_PARAM_STRUCT;//15 byte <= PARAM_DISP_FILT_AREA_SIZE

/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLE EXTERNALS
********************************************************************************
*/
/* Insert global variable externals here */
extern CAL_PARAM_STRUCT CalParam;
extern RANGE_PARAM_STRUCT RangeParam;
extern DISP_FILTER_PARAM_STRUCT DispFiltParam;
#ifdef USE_AD_READ_COMPLETE_CHECK
extern BOOLEAN AdDataReadCompleteFlag;
#endif
/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */
#ifdef DEBUG_AD_API
extern void AdWriteDataForDebug(INT32S lAd);
#endif

extern BOOLEAN AdCheckInitZero(void);
extern INT8U AdSetZero(void);
extern INT8U AdSetTare(void);
#ifdef USE_SATO_PRINTER_PROTOCOL
extern INT8U AdSetTareClear(void);
#endif
extern INT8U AdCheckPTInterval(INT32S presetTare);
extern INT8U AdSetPT(INT32S presetTare, INT8U constraintFlag);
extern INT8U AdSetPercentTare(INT8U percent);
extern INT32S AdGetRoundedWeight(INT32S extWeight);
extern void AdSetDefaultParam(INT8U type);

extern void AdInit(void);
extern void AdInitZeroReset(void);
extern void AdProc(INT8U checkInitZero, BOOLEAN calFlag);

extern INT32S AdReadParam(INT8U structNum);
extern void AdWriteParam(INT8U structNum, INT32S inputData);

extern INT32S AdNormalToRaw(INT32S ad);
extern INT8U AdFindCapaIndex(INT8U calUnit, INT16U capa);
extern INT8U AdFindDivTableIndex(INT8U divFactor);
extern BOOLEAN AdSetWeightUnitFlag(INT8U weightUnit);
extern void AdWriteHistoryParam(INT8U cType, INT32S zeroValue, INT32S midUpValue, INT32S midDownValue, INT32S spanValue);
#ifdef USE_EXP_HYST_COMP
extern INT8U AdWriteHystCalParam(INT32S *hystValue);
#else
extern INT8U AdWriteHystCalParam(INT32S zeroValue, INT32S midUpValue, INT32S midDownValue, INT32S spanValue);
#endif
extern INT8U AdWriteLinearCalParam(INT32S zeroValue, INT32S midUpValue, INT32S spanValue);
extern INT8U AdWriteSpanCalParam(INT8U cType, INT16U cData, INT32S zeroValue, INT32S spanValue);
extern INT32S AdIntToExtValueExtern (INT32S interval);
extern INT32S AdExtToIntValueExtern (INT32S interval);


extern void CalSetDefaultParam(void);
extern void CalWriteParam(void);
extern void CalReadParam(void);
extern void RangeSetDefaultParam(void);
extern void RangeWriteParam(void);
extern void RangeReadParam(void);
extern void DispFiltSetDefaultParam(void);
extern void DispFiltWriteParam(void);
extern void DispFiltReadParam(void);

#ifdef AUTO_ZERO
extern INT8U AutoSetZero(void);
#endif
#endif /* _AD_API_H */
