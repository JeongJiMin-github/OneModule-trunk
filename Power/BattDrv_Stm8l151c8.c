/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Battery Driver Soruce File\n
           for STM8L151C8
* @file    BattDrv_Stm8l151c8.c
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
#include "../Common/CommonLib.h"
#include "../Common/RingBuf.h"

#include "../Ad/AdDrv.h"
#include "BattDrv.h"

#include "../Hardware/HardwareConfig.h"

#ifdef BATT_DRV_STM8L151C8

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

#ifdef BATT_TYPE_PB_4V
/**
* @brief Battery Cut off Voltage\n
*        ex) 540 -> 5.40 Volt
*/
#define BATT_CUT_OFF_VOLTAGE 355
/**
* @brief Battery Full Voltage (100 %) \n
*        ex) 900 -> 9.00 Volt
*/
#define BATT_FULL_VOLTAGE_IN_CHARGE  430	//충전시  
#define BATT_FULL_VOLTAGE_IN_DISCHARGE 430 //방전시 //TO DO : check voltage
#define BATT_CHARGED_VOLTAGE 420 //방전시 //TO DO : check voltage
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
#define BATT_AD_AVR_SIZE 6
/**
* @brief Battery Low Voltage Check Threshold when go down(단위 : %) \n
*/
#define BATT_LOW_THRESHOLD_DOWN 35
/**
* @brief Battery Low Voltage Check Threshold when go Up(단위 : %) \n
*/
#define BATT_LOW_THRESHOLD_UP	41

/**
* @brief Battery Cut Off Voltage (0 %) \n
*        ex) 560 -> 5.60 Volt
*/
#define BATT_AD_MAX_VOLT	755L // 5.00V
/**
* @brief 정확한 Battery Voltage 측정을 위해 AD Offset 제거하지 위한 Calibration 기준 전압 (단위 : 0.00 V) \n
*/
#define BATT_AD_DEFAULT_OFFSET_VOLT	330L // 3.30 V
/**
* @brief 이상적일 경우(Ad Offset = 0)에 Calibration 기준 전압의 AD값 (16 bit)\n
*/
#define BATT_AD_IDEAL_OFFSET	((BATT_AD_DEFAULT_OFFSET_VOLT * 4096L) / BATT_AD_MAX_VOLT)//13107L	= 500*65535L/2500L
/**
* @brief Calibration하지 않거나 저장된 값이 이상할 경우 사용할 기준 전압의 AD값 (16 bit)\n
*/
//#define BATT_AD_DEFAULT_OFFSET	539L//3.30 Volt일때 측정치 EFM32G 12bit A/D 
#define BATT_AD_DEFAULT_OFFSET	2048L//3.30 Volt일때 측정치 STM8L 12bit A/D   
//#define BATT_AD_DEFAULT_OFFSET	10600L//5.00 Volt일때 측정치 10043,10170,10325,10536,10647 counts (일 반 적 이 지 않 음 )
/**
* @brief Battery의 Calibration offset의 허용 범위 (단위 : AD 값)\n
*/
//#define BATT_AD_OFFSET_ERROR_RANGE	6554L//10 %
#define BATT_AD_OFFSET_ERROR_RANGE	410L//10 %         // duckspg 12bit A/D 10%

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
static BOOLEAN ACFlag;
/** @brief battery가 charging 중임을 나타내는 Flag */
static BOOLEAN BattChargeFlag;
/** @brief battery가 완충임을 나타내는 Flag */
static BOOLEAN BattCHGFULLFlag;
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
/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
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
	BattCHGFULLFlag	 = FALSE;
	BattPrevVolt = BATT_FULL_VOLTAGE_IN_DISCHARGE;
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

		size = RingBufCountSize(&AdDrvRingBufForBatt);
		if (size >= BATT_AD_AVR_SIZE)
		{
			avrBattVolt = 0;
			for (i = 0; i < BATT_AD_AVR_SIZE; i++)
			{
				avrBattVolt = avrBattVolt + RingBufGetInt32s(&AdDrvRingBufForBatt);
			}
			BattAdFiltValue = avrBattVolt / BATT_AD_AVR_SIZE;
			BattAdFiltValue = BattAdFiltValue;//make 16 bit // duckspg EFM32 A/D 12bit 
			
			//Check charge or discharge for checking battery capacity
			batt_curr_volt = (INT16S)BattDrvReadVolt();
			if ((BattResetChargingRefVoltTimer100ms == 0) || (BattResetChargingRefVoltTimer100ms == 0xff))
			{
				BattResetChargingRefVoltTimer100ms = 0;
				batt_volt_diff = batt_curr_volt - BattPrevVolt;				
#ifdef BATT_TYPE_PB_4V
				if (batt_curr_volt < BATT_CHARGED_VOLTAGE)
				{
					BattChargeFlag = TRUE;
					BattPrevVolt = batt_curr_volt;
				}
				else
				{
					if (batt_volt_diff < BATT_VOLTAGE_THRES_FOR_SLOPE)
					{
						BattChargeFlag = FALSE;
						BattPrevVolt = batt_curr_volt;
					}
					else if (batt_volt_diff > BATT_VOLTAGE_THRES_FOR_SLOPE)
					{
						BattChargeFlag = TRUE;
						BattPrevVolt = batt_curr_volt;
					}
				}
#elif
				if (batt_volt_diff > BATT_VOLTAGE_THRES_FOR_SLOPE)
				{
					BattChargeFlag = FALSE;
					BattPrevVolt = batt_curr_volt;
				}
				else if (batt_volt_diff < (-BATT_VOLTAGE_THRES_FOR_SLOPE))
				{
					BattChargeFlag = TRUE;
					BattPrevVolt = batt_curr_volt;
				}
#endif
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

	retVol = ((BattAdFiltValue - BattAdOffset) * BATT_AD_MAX_VOLT) / 4096L + BATT_AD_DEFAULT_OFFSET_VOLT;
	return (INT16U)retVol;
//	return (INT16U)BattAdFiltValue; 
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
	if (BattDrvGetUseablePercent() < BATT_LOW_THRESHOLD_DOWN) 
	{
		BattLowFlag = 1;
	}
	else if (BattDrvGetUseablePercent() >= BATT_LOW_THRESHOLD_UP) 
	{
		BattLowFlag = 0;
	}
	return BattLowFlag;
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
//	return BattCHGFULLFlag;
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
#ifdef BATT_TYPE_NOT_INSTALLED
		return 0;
#endif

	if (BattDrvReadVolt() < BATT_CUT_OFF_VOLTAGE)
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
* @brief    Battery 남은 용량 Check 함수\n
* @param    none
* @return   남은 용량 (0 ~ 100 %)
* @remark   
********************************************************************************
*/

INT8U BattDrvGetUseablePercent(void)
{
	INT8U percent;
	INT16U batt_volt;
	INT16U batt_full_volt;

	batt_volt = BattDrvReadVolt();

	if (ACCHK)// lyh 20130115
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
} 

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/

/* Insert local functions here */
#endif //#ifdef BATT_DRV_STM8L151C8
