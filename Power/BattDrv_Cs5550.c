/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Battery Driver Soruce File\n
           for xx
* @file    BattDrv.c
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
#include "../Common/CommonLib.h"
#include "../Common/RingBuf.h"

#include "../Ad/AdDrv.h"
#include "BattDrv.h"

#include "../Hardware/HardwareConfig.h"
#ifndef USE_AC_POWER

#ifdef BATT_DRV_CS5550
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
#ifdef BATT_TYPE_PB_6V
/** @brief KR : Battery Cut off Voltage\n
*        		  ex) 560 -> 5.60 Volt\n
*        		  SW-WR의 경우 5.3V 이하는 측정값 오차 발생\n
*        		  전압을 5V이하로 낮춰도 5V이하 표시 불가\n
*		  US : \n
*		  CN :									*/
#define BATT_CUT_OFF_VOLTAGE 				560 

/** @brief KR : Battery Full Voltage (100 %) 충전시 100% 기준 전압\n
*        			ex) 705 -> 7.05 Volt\n
*		  US : Battery Full Voltage (100 %) on charge\n
*        			ex) 705 -> 7.05 Volt\n
*		  CN :									*/
#define BATT_FULL_VOLTAGE_IN_CHARGE 		705

/** @brief KR : Battery Full Voltage (100 %) 방전시 100% 기준 전압\n
*        			ex) 655 -> 6.55 Volt\n
*		  US : Battery Full Voltage (100 %) on discharge\n
*        			ex) 655 -> 6.55 Volt\n
*		  CN :									*/
#define BATT_FULL_VOLTAGE_IN_DISCHARGE 	655
#endif

#ifdef BATT_TYPE_DRY_9V
/** @brief KR : Battery Cut off Voltage\n
*        			ex) 520 -> 5.20 Volt\n
*        			PW-II, SW-1S1C의 경우 5.1V 이하는 측정값 오차 발생\n
*        			Backlight사용하거나 LED type인 경우 특성 관찰 필요\n
*        			전압을 5V이하로 낮춰도 5V이하 표시 불가\n
*        			Dry battery사용시 역삽방지용 다이오드로 인하여 0.3V전압 강하 발생\n
*       			PW-II, SW-1S1C의 경우 실제 컷오프 전압 5.5V\n
*		  US :\n
*		  CN :									*/
#define BATT_CUT_OFF_VOLTAGE 				520

/** @brief KR : Battery Full Voltage (100 %) 충전시 100% 기준 전압\n
*			        ex) 870 -> 8.70 Volt\n
*		  US : Battery Full Voltage (100 %) on charge\n
*			        ex) 870 -> 8.70 Volt\n
*		  CN :									*/
#define BATT_FULL_VOLTAGE_IN_CHARGE 		870

/** @brief KR : Battery Full Voltage (100 %)  방전시 100% 기준 전압\n
*			        ex) 870 -> 8.70 Volt\n
*		  US : Battery Full Voltage (100 %)  on discharge\n
*			        ex) 870 -> 8.70 Volt\n
*		  CN :									*/
#define BATT_FULL_VOLTAGE_IN_DISCHARGE 	870
#endif

#ifdef BATT_TYPE_PB_12V
/** @brief KR : Battery Cut off Voltage\n
*        			ex) 1063 -> 10.63 Volt\n
*		  US :\n
*		  CN :									*/
#define BATT_CUT_OFF_VOLTAGE 				1063

/** @brief KR : Battery Full Voltage (100 %) 충전시 100% 기준 전압\n
*        			ex) 1313 -> 13.13 Volt\n
*		  US :Battery Full Voltage (100 %)  on charge\n
*		  CN :									*/
#define BATT_FULL_VOLTAGE_IN_CHARGE  		1313	//충전시  

/** @brief KR : Battery Full Voltage (100 %) 방전시 100% 기준 전압\n
*        			ex) 1313 -> 13.13 Volt\n
*		  US :Battery Full Voltage (100 %)  on discharge\n
*		  CN :									*/
#define BATT_FULL_VOLTAGE_IN_DISCHARGE 	1313 //방전시 //TO DO : check voltage
#endif

/** @brief KR : Battery Threshold voltage magnitude for knowing charge or discharge state \n
*        			B/L, Suspend mode에 따른 전압 변동값 감안하여야함\n
*        			ex) 10 -> 0.10 Volt\n
*		  US :\n
*		  CN :									*/
#define BATT_VOLTAGE_THRES_FOR_SLOPE		1//0.01

/** @brief KR : Battery AD값을 Averaging할 개수 \n
*        			BATT_AD_AVR_SIZE < AD_BUF_SIZE_FOR_BATT, check this constant\n
*		  US :\n
*		  CN :									*/
#define BATT_AD_AVR_SIZE 					3

/** @brief KR : Battery Low Voltage Check Threshold when go down(단위 : %) \n
*		  US :\n
*		  CN :									*/
#define BATT_LOW_THRESHOLD_DOWN 			10

/** @brief KR : Battery Low Voltage Check Threshold when go Up(단위 : %) \n
*		  US :\n
*		  CN :									*/
#define BATT_LOW_THRESHOLD_UP			30

/** @brief KR : Maximum voltage of AD Converting \n
*		  US : Maximum voltage of AD Converting\n
*		  CN :									*/
#define BATT_AD_MAX_VOLT	2500L // 25.00

/** @brief KR : 정확한 Battery Voltage 측정을 위해 AD Offset 제거하지 위한 Calibration 기준 전압 (단위 : 0.00 V) \n
*		  US : \n
*		  CN :									*/
#define BATT_AD_DEFAULT_OFFSET_VOLT	500L

/** @brief KR : 이상적일 경우(Ad Offset = 0)에 Calibration 기준 전압의 AD값 (16 bit) \n
*				13107L	= 500*65535L/2500L
*		  US : \n
*		  CN :									*/
#define BATT_AD_IDEAL_OFFSET	((BATT_AD_DEFAULT_OFFSET_VOLT * 65535L) / BATT_AD_MAX_VOLT)

/** @brief KR : Calibration하지 않거나 저장된 값이 이상할 경우 사용할 기준 전압의 AD값 (16 bit) \n
*				5.00 Volt일때 측정치 10043,10170,10325,10536,10647 counts (일 반 적 이 지 않 음 )
*		  US : \n
*		  CN :									*/
#define BATT_AD_DEFAULT_OFFSET	10600L

/** @brief KR : Battery의 Calibration offset의 허용 범위 (단위 : AD 값), 10%\n
*		  US : \n
*		  CN :									*/
#define BATT_AD_OFFSET_ERROR_RANGE	6554L

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
/** @brief KR : AD Chip의 AD값을 가져올 Ring Buffer pointer\n
*		  US : \n
*		  CN :									*/
static RING_BUF *BattAdRingBufPtr;

/** @brief KR : AD값의 평균값\n
*		  US : \n
*		  CN :									*/
static INT32S BattAdFiltValue;

/** @brief KR : BATT_AD_DEFAULT_OFFSET_VOLT에서의 AD Offset 값\n
*		  US : \n
*		  CN :									*/
static INT32S BattAdOffset;

/** @brief KR : battery가 charging 중임을 나타내는 Flag \n
*		  US : \n
*		  CN :									*/
static BOOLEAN BattChargeFlag;

/** @brief KR : battery voltage 이전 값\n
*		  US : \n
*		  CN :									*/
static INT16S BattPrevVolt;

/** @brief KR : battery low  flag 값\n
*		  US : \n
*		  CN :									*/
static BOOLEAN BattLowFlag;

/** @brief KR : Battery charge 판정 기준 전압 재셋팅 타이머 변수 \n
*		  US : \n
*		  CN :									*/
static INT8U BattResetChargingRefVoltTimer100ms;
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
* @brief       KR : BattAdOffset의Default값 반환 \n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : BattAdOffset (INT16U)\n
*                 US : BattAdOffset (INT16U)\n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
INT16U BattDrvGetDefaultAdOffset(void)
{
	return BATT_AD_DEFAULT_OFFSET;
}

/**
********************************************************************************
* @brief       KR : Battery Driver Init 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : adOffset : AD Offset 값 \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : 처음에 한번 실행\n
*           			현재 Battery Voltage Check용 AD Chip 따로 장착하지 않고\n
*           			L/C용 AD Chip과 같이 사용 (Channel 2)\n
*           			그래서 L/C AD Chip의 Channel 2 Ring Buffer Pointer를 받아옴\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BattDrvInit(INT16U adOffset)
{	
	BattAdRingBufPtr = (&AdDrvRingBufForBatt);
	BattAdFiltValue = 0;
	BattChargeFlag = FALSE;
#ifdef BATT_TYPE_NOT_INSTALLED
	BattPrevVolt = 0;
#else
	BattPrevVolt = BATT_FULL_VOLTAGE_IN_DISCHARGE;
#endif
	BattLowFlag = 0;
	BattResetChargingRefVoltTimer100ms = 0xff;//init state
	BattDrvSetAdOffset(adOffset);
}

/**
********************************************************************************
* @brief       KR : Battery Driver Procedure 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : 일정 시간에 한번씩 계속 실행 되는 함수\n
*           			Battery Ring Buffer 확인 후 Average\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BattDrvProc(INT16U currTimer100ms)
{
	INT8U size;
	INT8U i;
	INT32S avrBattVolt;
	INT16S batt_curr_volt;
	INT16S batt_volt_diff;
	static INT16U prevBattTimer100ms = 0;

	if (prevBattTimer100ms != currTimer100ms)
	{
		prevBattTimer100ms = currTimer100ms;
		//100 ms에 한번씩 실행

		size = RingBufCountSize(BattAdRingBufPtr);
		if (size >= BATT_AD_AVR_SIZE)
		{
			avrBattVolt = 0;
			for (i = 0; i < BATT_AD_AVR_SIZE; i++)
			{
				avrBattVolt = avrBattVolt + RingBufGetInt32s(BattAdRingBufPtr);
			}
			BattAdFiltValue = avrBattVolt / BATT_AD_AVR_SIZE;
			BattAdFiltValue = BattAdFiltValue >> 8;//make 16 bit
			
			//Check charge or discharge for checking battery capacity
			batt_curr_volt = (INT16S)BattDrvReadVolt();
			if ((BattResetChargingRefVoltTimer100ms == 0) || (BattResetChargingRefVoltTimer100ms == 0xff))
			{
				BattResetChargingRefVoltTimer100ms = 0;
				batt_volt_diff = batt_curr_volt - BattPrevVolt;
				if (batt_volt_diff > BATT_VOLTAGE_THRES_FOR_SLOPE)
				{
					BattChargeFlag = TRUE;
					BattPrevVolt = batt_curr_volt;
				}
				else if (batt_volt_diff < (-BATT_VOLTAGE_THRES_FOR_SLOPE))
				{
					BattChargeFlag = FALSE;
					BattPrevVolt = batt_curr_volt;
				}
			}
			else
			{
			 	BattResetChargingRefVoltTimer100ms--;
				BattPrevVolt = batt_curr_volt;
			}	
		}
	}
}

/**
********************************************************************************
* @brief       KR : Battery charge 판정 기준 전압 재셋팅 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : B/L On/Off, Suspend mode On/Off 시 사용\n
*          			 (전류 변화에 의한 배터리 전압 변화 영향 제거 위함)\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BattDrvResetChargingRefVolt(void)
{
	if (BattResetChargingRefVoltTimer100ms != 0xff) //init state
	{
		BattResetChargingRefVoltTimer100ms = 10; //1s
	}
}

/**
********************************************************************************
* @brief       KR : Battery AD offset 범위 check 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : Battery AD 값\n
*                 US : \n
*                 CN : \n
* @return     KR : INT8U (TRUE, FALSE)\n
*                 US : \n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
INT8U BattDrvCheckAdOffsetRange(INT32S adOffset)
{
	if (AbsInt32s(adOffset - BATT_AD_IDEAL_OFFSET) > BATT_AD_OFFSET_ERROR_RANGE)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/**
********************************************************************************
* @brief       KR : Battery offset Ad seting 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : Battery AD 값\n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : battery volt cal. 시 사용\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BattDrvSetAdOffset(INT32S adOffset)
{
	if (!BattDrvCheckAdOffsetRange(adOffset))
	{
		BattAdOffset = BATT_AD_DEFAULT_OFFSET;
	}
	else
	{
		BattAdOffset = (INT32S)adOffset;
	}
}

/**
********************************************************************************
* @brief       KR : Battery Read Ad 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : Battery AD 값\n
*                 US : \n
*                 CN : \n
* @remark   KR : battery volt cal. 시 사용\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
INT32S BattDrvReadAd(void)
{
	return BattAdFiltValue;
}

/**
********************************************************************************
* @brief       KR : Battery Read Voltage 함수\n\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : Battery Voltage 값 (2 byte)\n
*           		     Battery Voltage 값 x.xx 소수점은 2자리 (2 byte)\n
*                 US : \n
*                 CN : \n
* @remark   KR : AD Chip 입력 범위에 맞추어 divide한 저항값에 따라\n
*           		     최종 Votage값으로 변환함\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
BOOLEAN BattChargeCheck(void)
{
	BattDrvReadVolt();
	return BattChargeFlag;
}

/**
********************************************************************************
* @brief       KR : Battery Read Voltage 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : Battery Voltage 값 (2 byte)\n
*			     Battery Voltage 값 x.xx 소수점은 2자리 (2 byte)\n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : AD Chip 입력 범위에 맞추어 divide한 저항값에 따라\n
*           			최종 Votage값으로 변환함\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
INT16U BattDrvReadVolt(void)
{
	INT32S retVol;

	retVol = ((BattAdFiltValue - BattAdOffset) * BATT_AD_MAX_VOLT) / 65535L + BATT_AD_DEFAULT_OFFSET_VOLT;
	return (INT16U)retVol;
}

/**
********************************************************************************
* @brief       KR : Battery Driver Low Voltage Check 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : 1-low volt, 0-high volt\n
*                 US : \n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
BOOLEAN BattDrvCheckLow(void)
{
#ifdef BATT_TYPE_NOT_INSTALLED
	return 0;
#else
	if (BattDrvGetUseablePercent() < BATT_LOW_THRESHOLD_DOWN) 
	{
		BattLowFlag = 1;
	}
	else if (BattDrvGetUseablePercent() >= BATT_LOW_THRESHOLD_UP) 
	{
		BattLowFlag = 0;
	}
	return BattLowFlag;
#endif
}

/**
********************************************************************************
* @brief       KR : Battery Driver Full Voltage Check 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : 1-full volt, 0-no full volt\n
*                 US : \n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
BOOLEAN BattDrvCheckFull(void)
{
#ifdef BATT_TYPE_NOT_INSTALLED
	return 0;
#else
	return 0;
#endif
}

/**
********************************************************************************
* @brief       KR : Battery Driver Cut-off Voltage Check 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : 1-low volt, 0-high volt\n
*                 US : \n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
BOOLEAN BattDrvCheckCutOff(void)
{
#ifdef BATT_TYPE_NOT_INSTALLED
	return 0;
#else
	if (BattDrvReadVolt() < BATT_CUT_OFF_VOLTAGE)
	{
		return 1;
	}
	else
	{
		return 0;
	}
#endif	
}

/**
********************************************************************************
* @brief       KR : Battery 남은 용량 Check 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : 남은 용량 (0 ~ 100 %)\n
*                 US : \n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
INT8U BattDrvGetUseablePercent(void)
{
#ifdef BATT_TYPE_NOT_INSTALLED
	return 0;
#else
	INT8U percent;
	INT16U batt_volt;
	INT16U batt_full_volt;

	batt_volt = BattDrvReadVolt();

	if (BattChargeFlag)
	{
		batt_full_volt = BATT_FULL_VOLTAGE_IN_CHARGE;	
	}
	else
	{
		batt_full_volt = BATT_FULL_VOLTAGE_IN_DISCHARGE;
	}

	if(batt_volt < BATT_CUT_OFF_VOLTAGE)
	{
		batt_volt = BATT_CUT_OFF_VOLTAGE;
	}
	else if(batt_volt > batt_full_volt)
	{
		batt_volt = batt_full_volt;
	}
		
	percent = (batt_volt - BATT_CUT_OFF_VOLTAGE) * 100 / (batt_full_volt - BATT_CUT_OFF_VOLTAGE);
	
	return percent;
#endif	
}
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
#endif//#ifdef BATT_DRV_CS5550
#endif
