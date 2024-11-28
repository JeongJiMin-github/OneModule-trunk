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

#ifdef BATT_DRV_CS5550_PB
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

#ifdef BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_PORT
/**
* @brief Dry Battery Cut off Voltage\n
*/
#define BATT_DRY_CUT_OFF_VOLTAGE 420 
#endif

#ifdef BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_VOLT //For ER-jr Dry battery and Pb battery option. 
/**
* @brief Dry Battery Cut off Voltage\n
* 	ER-jr Dry battery type은 전압을 디바이딩하여 확인하는 방식임.
*     모든 전압은 디바이딩 된 전압임.
*/
#define BATT_DRY_CUT_OFF_VOLTAGE 140 //Real Voltage 4.2V
#define BATT_DRY_FULL_VOLATGE 190       //Real Voltage 6V
#define BATT_TYPE_CHECK_VOLTAGE 250  //Real Volatge 7V
#define PB_BATT 0
#define DRY_BATT 1
#ifdef USE_AUTO_POWER_SETTING
#define ADAPTER 2
#endif
/**
* @brief Battery Cut off Voltage\n
*        ex) 560 -> 5.60 Volt
*        SW-WR의 경우 5.3V 이하는 측정값 오차 발생
*        전압을 5V이하로 낮춰도 5V이하 표시 불가
*/
#define BATT_CUT_OFF_VOLTAGE 560 
/**
* @brief Battery Full Voltage (100 %) \n
*        ex) 713 -> 7.13 Volt
*/
#define BATT_FULL_VOLTAGE_IN_CHARGE 705 //충전시 100% 기준 전압
#define BATT_FULL_VOLTAGE_IN_DISCHARGE 655 //방전시 100% 기준 전압
#ifdef USE_AUTO_POWER_SETTING
#define ADAPTER_TYPE_CHECK_VOLTAGE 700 // Adapter 전원인가 했을 때 전압 
#endif
#endif

#ifdef BATT_TYPE_PB_6V
/**
* @brief Battery Cut off Voltage\n
*        ex) 560 -> 5.60 Volt
*        SW-WR의 경우 5.3V 이하는 측정값 오차 발생
*        전압을 5V이하로 낮춰도 5V이하 표시 불가
*/
#define BATT_CUT_OFF_VOLTAGE 560 
/**
* @brief Battery Full Voltage (100 %) \n
*        ex) 713 -> 7.13 Volt
*/
#define BATT_FULL_VOLTAGE_IN_CHARGE 705 //충전시 100% 기준 전압
#define BATT_FULL_VOLTAGE_IN_DISCHARGE 655 //방전시 100% 기준 전압
#endif

#ifdef BATT_TYPE_DRY_9V
/**
* @brief Battery Cut off Voltage\n
*        ex) 540 -> 5.40 Volt
*        PW-II, SW-1S1C의 경우 5.1V 이하는 측정값 오차 발생
*        Backlight사용하거나 LED type인 경우 특성 관찰 필요 
*        전압을 5V이하로 낮춰도 5V이하 표시 불가
*        Dry battery사용시 역삽방지용 다이오드로 인하여 0.3V전압 강하 발생
*        PW-II, SW-1S1C의 경우 실제 컷오프 전압 5.5V
*/
#define BATT_CUT_OFF_VOLTAGE 520
/**
* @brief Battery Full Voltage (100 %) \n
*        ex) 900 -> 9.00 Volt
*/
#define BATT_FULL_VOLTAGE_IN_CHARGE 870
#define BATT_FULL_VOLTAGE_IN_DISCHARGE 870
#endif

#ifdef BATT_TYPE_PB_12V
/**
* @brief Battery Cut off Voltage\n
*        ex) 540 -> 5.40 Volt
*/
#define BATT_CUT_OFF_VOLTAGE 1063
/**
* @brief Battery Full Voltage (100 %) \n
*        ex) 900 -> 9.00 Volt
*/
#define BATT_FULL_VOLTAGE_IN_CHARGE  1313	//충전시  
#define BATT_FULL_VOLTAGE_IN_DISCHARGE 1313 //방전시 //TO DO : check voltage
#endif

/**
* @brief Battery Threshold voltage magnitude for knowing charge or discharge state \n
*        B/L, Suspend mode에 따른 전압 변동값 감안하여야함\n
*        ex) 10 -> 0.10 Volt
*/
#define BATT_VOLTAGE_THRES_FOR_SLOPE	1//0.01

/**
* @brief Battery AD값을 Averaging할 개수 \n
*        BATT_AD_AVR_SIZE < AD_BUF_SIZE_FOR_BATT, check this constant
*/
#define BATT_AD_AVR_SIZE 3
/**
* @brief Battery Low Voltage Check Threshold when go down(단위 : %) \n
*/
#define BATT_LOW_THRESHOLD_DOWN 10
/**
* @brief Battery Low Voltage Check Threshold when go Up(단위 : %) \n
*/
#define BATT_LOW_THRESHOLD_UP	30

/**
* @brief Battery Cut Off Voltage (0 %) \n
*        ex) 560 -> 5.60 Volt
*/

#define BATT_AD_MAX_VOLT	2500L // 25.00
/**
* @brief 정확한 Battery Voltage 측정을 위해 AD Offset 제거하지 위한 Calibration 기준 전압 (단위 : 0.00 V) \n
*/
#define BATT_AD_DEFAULT_OFFSET_VOLT	500L // 5.00 V
/**
* @brief 이상적일 경우(Ad Offset = 0)에 Calibration 기준 전압의 AD값 (16 bit)\n
*/
#define BATT_AD_IDEAL_OFFSET	((BATT_AD_DEFAULT_OFFSET_VOLT * 65535L) / BATT_AD_MAX_VOLT)//13107L	= 500*65535L/2500L
/**
* @brief Calibration하지 않거나 저장된 값이 이상할 경우 사용할 기준 전압의 AD값 (16 bit)\n
*/
#define BATT_AD_DEFAULT_OFFSET	10600L//5.00 Volt일때 측정치 10043,10170,10325,10536,10647 counts (일 반 적 이 지 않 음 )
/**
* @brief Battery의 Calibration offset의 허용 범위 (단위 : AD 값)\n
*/
#define BATT_AD_OFFSET_ERROR_RANGE	6554L//10 %

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
/** @brief AD Chip의 AD값을 가져올 Ring Buffer pointer */
static RING_BUF *BattAdRingBufPtr;
/** @brief AD값의 평균값 */
static INT32S BattAdFiltValue;
/** @brief BATT_AD_DEFAULT_OFFSET_VOLT에서의 AD Offset 값 */
static INT32S BattAdOffset;
/** @brief battery가 charging 중임을 나타내는 Flag */
static BOOLEAN BattChargeFlag;
/** @brief battery voltage 이전 값 */
static INT16S BattPrevVolt;
/** @brief battery low  flag 값 */
static BOOLEAN BattLowFlag;
/** @brief Battery charge 판정 기준 전압 재셋팅 타이머 변수 */
static INT8U BattResetChargingRefVoltTimer100ms;
/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
#ifdef BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_VOLT
/** @brief Battery Type Check function */
BOOLEAN BattTypeCheck(void);
#endif
/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
#ifdef USE_AUTO_POWER_SETTING 
/** @brief Power Type Check function */
INT8U PowerTypeCheck(void);
#endif

/**
********************************************************************************
* @brief    BattAdOffset의Default값 반환 \n
* @param    none
* @return   BattAdOffset (INT16U)
* @remark   
********************************************************************************
*/
INT16U BattDrvGetDefaultAdOffset(void)
{
	return BATT_AD_DEFAULT_OFFSET;
}

/**
********************************************************************************
* @brief    Battery Driver Init 함수\n
* @param    adOffset : AD Offset 값 
* @return   none
* @remark   처음에 한번 실행\n
*           현재 Battery Voltage Check용 AD Chip 따로 장착하지 않고\n
*           L/C용 AD Chip과 같이 사용 (Channel 2)\n
*           그래서 L/C AD Chip의 Channel 2 Ring Buffer Pointer를 받아옴
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
* @brief    Battery Driver Procedure 함수\n
* @param    none
* @return   none
* @remark   일정 시간에 한번씩 계속 실행 되는 함수\n
*           Battery Ring Buffer 확인 후 Average\n
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
* @brief    Battery charge 판정 기준 전압 재셋팅 함수\n
* @param    none
* @return   none
* @remark   B/L On/Off, Suspend mode On/Off 시 사용	\n
*           (전류 변화에 의한 배터리 전압 변화 영향 제거 위함)
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
* @brief    Battery AD offset 범위 check 함수\n
* @param    Battery AD 값
* @return   INT8U (TRUE, FALSE)
* @remark   
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
* @brief    Battery offset Ad seting 함수\n
* @param    Battery AD 값
* @return   none
* @remark   battery volt cal. 시 사용
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
* @brief    Battery Read Ad 함수\n
* @param    none
* @return   Battery AD 값
* @remark   battery volt cal. 시 사용
********************************************************************************
*/
INT32S BattDrvReadAd(void)
{
	return BattAdFiltValue;
}

/**
********************************************************************************
* @brief    Battery Read Voltage 함수\n
* @param    none
* @return   Battery Voltage 값 (2 byte)
*           Battery Voltage 값 x.xx 소수점은 2자리 (2 byte)
* @remark   AD Chip 입력 범위에 맞추어 divide한 저항값에 따라\n
*           최종 Votage값으로 변환함
********************************************************************************
*/
BOOLEAN BattChargeCheck(void)
{
	BattDrvReadVolt();
	return BattChargeFlag;
}

/**
********************************************************************************
* @brief    Battery Read Voltage 함수\n
* @param    none
* @return   Battery Voltage 값 (2 byte)
*           Battery Voltage 값 x.xx 소수점은 2자리 (2 byte)
* @remark   AD Chip 입력 범위에 맞추어 divide한 저항값에 따라\n
*           최종 Votage값으로 변환함
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
* @brief    Battery Driver Low Voltage Check 함수\n
* @param    none
* @return   1-low volt, 0-high volt
* @remark   
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
* @brief    Battery Driver Full Voltage Check 함수\n
* @param    none
* @return   1-full volt, 0-no full volt
* @remark   
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
* @brief    Battery Driver Cut-off Voltage Check 함수\n
* @param    none
* @return   1-low volt, 0-high volt
* @remark   
********************************************************************************
*/
BOOLEAN BattDrvCheckCutOff(void)
{
#if defined(BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_PORT) || defined(BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_VOLT)	
	INT16U cut_off_voltage;
#endif

#ifdef BATT_TYPE_NOT_INSTALLED
	return 0;
#else

#if defined BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_PORT

	cut_off_voltage = BATT_CUT_OFF_VOLTAGE;
	if(BATT_DRY_CHK)
	{
		cut_off_voltage = BATT_DRY_CUT_OFF_VOLTAGE;
	}

	if (BattDrvReadVolt() < cut_off_voltage)
	{
		return 1;
	}
	else
	{
		return 0;
	}

#elif defined BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_VOLT			 
	cut_off_voltage = BATT_CUT_OFF_VOLTAGE;
	if(BattTypeCheck()==DRY_BATT)
	{
		cut_off_voltage = BATT_DRY_CUT_OFF_VOLTAGE;
	}
	else
	{
		cut_off_voltage = BATT_CUT_OFF_VOLTAGE;
	}

	if (BattDrvReadVolt() < cut_off_voltage)
	{
		return 1;
	}
	else
	{
		return 0;
	}

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

#endif	
}

/**
********************************************************************************
* @brief    Battery 남은 용량 Check 함수\n
* @param    none
* @return   남은 용량 (0 ~ 100 %)
* @remark   
********************************************************************************
*/

INT8U BattDrvGetUseablePercent(void)
{
#ifdef BATT_TYPE_NOT_INSTALLED
	return 0;
#else
#if defined(BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_PORT) || defined(BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_VOLT)	
	INT16U cut_off_voltage;
	INT8U batt_type;
#endif
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
	

#if defined BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_PORT
	cut_off_voltage = BATT_CUT_OFF_VOLTAGE;
	if(BATT_DRY_CHK)
	{
		cut_off_voltage = BATT_DRY_CUT_OFF_VOLTAGE;
	}

	if(batt_volt < cut_off_voltage)
	{
		batt_volt = cut_off_voltage;
	}
	else if(batt_volt > batt_full_volt)
	{
		batt_volt = batt_full_volt;
	}
	percent = (batt_volt - cut_off_voltage) * 100 / (batt_full_volt - cut_off_voltage);

#elif defined BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_VOLT		
	batt_type = BattTypeCheck();
	switch(batt_type)
	{
		case PB_BATT:
		cut_off_voltage = BATT_CUT_OFF_VOLTAGE;
		break;

		case DRY_BATT:
		cut_off_voltage = BATT_DRY_CUT_OFF_VOLTAGE;
		batt_full_volt = BATT_DRY_FULL_VOLATGE;	
		break;
	}
			
	if(batt_volt < cut_off_voltage)
	{
		batt_volt = cut_off_voltage;
	}
	else if(batt_volt > batt_full_volt)
	{
		batt_volt = batt_full_volt;
	}
	percent = (batt_volt - cut_off_voltage) * 100 / (batt_full_volt - cut_off_voltage);

#else

	if(batt_volt < BATT_CUT_OFF_VOLTAGE)
	{
		batt_volt = BATT_CUT_OFF_VOLTAGE;
	}
	else if(batt_volt > batt_full_volt)
	{
		batt_volt = batt_full_volt;
	}
	percent = (batt_volt - BATT_CUT_OFF_VOLTAGE) * 100 / (batt_full_volt - BATT_CUT_OFF_VOLTAGE);

#endif
	
	return percent;
#endif	
}

/**
********************************************************************************
* @brief    Battery Type Check 함수\n
* @param    none
* @return   1-Pb_Battery, 0-Dry_Battery
* @remark   
********************************************************************************
*/
#ifdef BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_VOLT
BOOLEAN BattTypeCheck(void)
{
	INT16U cut_off_voltage;

#ifdef BATT_TYPE_NOT_INSTALLED
	return;
#else
	if(BattDrvReadVolt() < BATT_TYPE_CHECK_VOLTAGE)
	{
		return DRY_BATT;
	}
	else
	{
		return PB_BATT;
	}
#endif
}
#endif

/*
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
*/
#ifdef USE_AUTO_POWER_SETTING 

/**
********************************************************************************
* @brief	Power Type Check 함수\n
* @param	none
* @return	2-Adapter, 1-Pb_Battery, 0-Dry_Battery
* @remark	
********************************************************************************
*/
INT8U PowerTypeCheck(void)
{
	if (BattDrvReadVolt() > ADAPTER_TYPE_CHECK_VOLTAGE)
	{
 		return ADAPTER;
	}
	else if(BattDrvReadVolt() < BATT_TYPE_CHECK_VOLTAGE)
	{
		return DRY_BATT;
	}
	else
	{
		return PB_BATT;
	}
}

#endif

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
#endif//#ifdef BATT_DRV_CS5550
