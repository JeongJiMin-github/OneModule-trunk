/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Driver Source File\n
* @file    BattDrv_Efm32g840.c
* @version 1.0
* @date    2012/09/19
* @author  Deok Hyun Kim (KDH)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2012/09/19 created by KDH \n
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
#include "../Ad/AdDrv_Efm32g840.h"
#include "BattDrv.h"

#include "../Hardware/HardwareConfig.h"

#ifdef BATT_DRV_EFM32G



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
#ifdef BATT_TYPE_DRY_6V_ONLY
/**
* @brief Battery Cut off Voltage\n
*        ex) 360 -> 3.60 Volt
*        TM AA Size Dry Battery 4�� ���ķ� ����� , ��������� ���̿���� Battery check �޴ܿ� ��ġ�ϹǷ� ���� ����
*/
#define BATT_CUT_OFF_VOLTAGE 360
/**
* @brief Battery Full Voltage (100 %) \n
*        ex) 600 -> 6.00 Volt
*/
#define BATT_FULL_VOLTAGE_IN_CHARGE 600
#define BATT_FULL_VOLTAGE_IN_DISCHARGE 600
#endif

#ifdef BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_PORT
/**
* @brief Dry Battery Cut off Voltage\n
*/
#define BATT_DRY_CUT_OFF_VOLTAGE 420 
#endif

#ifdef BATT_TYPE_PB_6V
/**
* @brief Battery Cut off Voltage\n
*        ex) 560 -> 5.60 Volt
*        SW-WR�� ��� 5.3V ���ϴ� ������ ���� �߻�
*        ������ 5V���Ϸ� ���絵 5V���� ǥ�� �Ұ�
*/
#define BATT_CUT_OFF_VOLTAGE 560 
/**
* @brief Battery Full Voltage (100 %) \n
*        ex) 713 -> 7.13 Volt
*/
#define BATT_FULL_VOLTAGE_IN_CHARGE 705 //������ 100% ���� ����
#define BATT_FULL_VOLTAGE_IN_DISCHARGE 655 //������ 100% ���� ����
#endif

#ifdef BATT_TYPE_DRY_9V
/**
* @brief Battery Cut off Voltage\n
*        ex) 540 -> 5.40 Volt
*        PW-II, SW-1S1C�� ��� 5.1V ���ϴ� ������ ���� �߻�
*        Backlight����ϰų� LED type�� ��� Ư�� ���� �ʿ� 
*        ������ 5V���Ϸ� ���絵 5V���� ǥ�� �Ұ�
*        Dry battery���� ��������� ���̿���� ���Ͽ� 0.3V���� ���� �߻�
*        PW-II, SW-1S1C�� ��� ���� �ƿ��� ���� 5.5V
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
#define BATT_FULL_VOLTAGE_IN_CHARGE  1313	//������  
#define BATT_FULL_VOLTAGE_IN_DISCHARGE 1313 //������ //TO DO : check voltage
#endif

/**
* @brief Battery Threshold voltage magnitude for knowing charge or discharge state \n
*        B/L, Suspend mode�� ���� ���� ������ �����Ͽ�����\n
*        ex) 10 -> 0.10 Volt
*/
#define BATT_VOLTAGE_THRES_FOR_SLOPE	1//0.01

/**
* @brief Battery AD���� Averaging�� ���� \n
*        BATT_AD_AVR_SIZE < AD_BUF_SIZE_FOR_BATT, check this constant
*/
#define BATT_AD_AVR_SIZE 3
/**
* @brief Battery Low Voltage Check Threshold when go down(���� : %) \n
*/
#define BATT_LOW_THRESHOLD_DOWN 10
/**
* @brief Battery Low Voltage Check Threshold when go Up(���� : %) \n
*/
#define BATT_LOW_THRESHOLD_UP	30

/**
* @brief Battery Cut Off Voltage (0 %) \n
*        ex) 560 -> 5.60 Volt
*/
#define BATT_AD_MAX_VOLT	2170L // 25.00
/**
* @brief ��Ȯ�� Battery Voltage ������ ���� AD Offset �������� ���� Calibration ���� ���� (���� : 0.00 V) \n
*/
#define BATT_AD_DEFAULT_OFFSET_VOLT	330L // 5.00 V
/**
* @brief �̻����� ���(Ad Offset = 0)�� Calibration ���� ������ AD�� (16 bit)\n
*/
#define BATT_AD_IDEAL_OFFSET	((BATT_AD_DEFAULT_OFFSET_VOLT * 4096L) / BATT_AD_MAX_VOLT)//13107L	= 500*65535L/2500L
/**
* @brief Calibration���� �ʰų� ����� ���� �̻��� ��� ����� ���� ������ AD�� (16 bit)\n
*/

  #define BATT_AD_DEFAULT_OFFSET	539L//3.30 Volt�϶� ����ġ EFM32G 12bit A/D 

/**
* @brief Battery�� Calibration offset�� ��� ���� (���� : AD ��)\n
*/
//#define BATT_AD_OFFSET_ERROR_RANGE	6554L//10 %
#define BATT_AD_OFFSET_ERROR_RANGE	409L//10 %         // duckspg 12bit A/D 10%

#define REG_VALUE_DIVIDE      7.67        //(100K�� 15K�� 115/15 = 7.66666
#define BATT_RESOLUTION      805.7       // 0.0008057(12bit) * 10^6

#ifdef BATT_DIODE_DROP
#define BATT_DROP_VOLTAGE    30           //0.3V diode drop
#else
#define BATT_DROP_VOLTAGE    0
#endif

#define BATT_DISPLAY_DP      10000
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
/** @brief AD Chip�� AD���� ������ Ring Buffer pointer */
static RING_BUF *BattAdRingBufPtr;
/** @brief AD���� ��հ� */
static INT32S BattAdFiltValue;
/** @brief BATT_AD_DEFAULT_OFFSET_VOLT������ AD Offset �� */
static INT32S BattAdOffset;
/** @brief battery�� charging ������ ��Ÿ���� Flag */
static BOOLEAN BattChargeFlag;
/** @brief battery voltage ���� �� */
static INT16S BattPrevVolt;
/** @brief battery low  flag �� */
static BOOLEAN BattLowFlag;
/** @brief Battery charge ���� ���� ���� ����� Ÿ�̸� ���� */
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
* @brief    BattAdOffset��Default�� ��ȯ \n
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
* @brief    Battery Driver Init �Լ�\n
* @param    adOffset : AD Offset �� 
* @return   none
* @remark   ó���� �ѹ� ����\n
*           ���� Battery Voltage Check�� AD Chip ���� �������� �ʰ�\n
*           L/C�� AD Chip�� ���� ��� (Channel 2)\n
*           �׷��� L/C AD Chip�� Channel 2 Ring Buffer Pointer�� �޾ƿ�
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
* @brief    Battery Driver Procedure �Լ�\n
* @param    none
* @return   none
* @remark   ���� �ð��� �ѹ��� ��� ���� �Ǵ� �Լ�\n
*           Battery Ring Buffer Ȯ�� �� Average\n
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
		//100 ms�� �ѹ��� ����

		size = RingBufCountSize(BattAdRingBufPtr);
		if (size >= BATT_AD_AVR_SIZE)
		{
			avrBattVolt = 0;
			for (i = 0; i < BATT_AD_AVR_SIZE; i++)
			{
				avrBattVolt = avrBattVolt + RingBufGetInt32s(BattAdRingBufPtr);
			}
			BattAdFiltValue = avrBattVolt / BATT_AD_AVR_SIZE;
			BattAdFiltValue = BattAdFiltValue;//make 16 bit // duckspg EFM32 A/D 12bit 
			
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
* @brief    Battery charge ���� ���� ���� ����� �Լ�\n
* @param    none
* @return   none
* @remark   B/L On/Off, Suspend mode On/Off �� ���	\n
*           (���� ��ȭ�� ���� ���͸� ���� ��ȭ ���� ���� ����)
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
* @brief    Battery AD offset ���� check �Լ�\n
* @param    Battery AD ��
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
* @brief    Battery offset Ad seting �Լ�\n
* @param    Battery AD ��
* @return   none
* @remark   battery volt cal. �� ���
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
* @brief    Battery Read Ad �Լ�\n
* @param    none
* @return   Battery AD ��
* @remark   battery volt cal. �� ���
********************************************************************************
*/
INT32S BattDrvReadAd(void)
{
	return BattAdFiltValue;
}

/**
********************************************************************************
* @brief    Battery Read Voltage �Լ�\n
* @param    none
* @return   Battery Voltage �� (2 byte)
*           Battery Voltage �� x.xx �Ҽ����� 2�ڸ� (2 byte)
* @remark   AD Chip �Է� ������ ���߾� divide�� ���װ��� ����\n
*           ���� Votage������ ��ȯ��
********************************************************************************
*/
BOOLEAN BattChargeCheck(void)
{
	BattDrvReadVolt();
	return BattChargeFlag;
}

/**
********************************************************************************
* @brief    Battery Read Voltage �Լ�\n
* @param    none
* @return   Battery Voltage �� (2 byte)
*           Battery Voltage �� x.xx �Ҽ����� 2�ڸ� (2 byte)
* @remark   AD Chip �Է� ������ ���߾� divide�� ���װ��� ����\n
*           ���� Votage������ ��ȯ��
********************************************************************************
*/
INT16U BattDrvReadVolt(void)
{

	INT32S retVol;
    retVol = (INT32S)(REG_VALUE_DIVIDE*(BATT_RESOLUTION * (FP64)(BattAdFiltValue)));
    retVol = (retVol/BATT_DISPLAY_DP)+BATT_DROP_VOLTAGE;
	return (INT16U)retVol;
}

/**
********************************************************************************
* @brief    Battery Driver Low Voltage Check �Լ�\n
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
* @brief    Battery Driver Full Voltage Check �Լ�\n
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
* @brief    Battery Driver Cut-off Voltage Check �Լ�\n
* @param    none
* @return   1-low volt, 0-high volt
* @remark   
********************************************************************************
*/
BOOLEAN BattDrvCheckCutOff(void)
{
#ifdef BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_PORT
	INT16U cut_off_voltage;
#endif

#ifdef BATT_TYPE_NOT_INSTALLED
	return 0;
#else

#ifdef BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_PORT

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
* @brief    Battery ���� �뷮 Check �Լ�\n
* @param    none
* @return   ���� �뷮 (0 ~ 100 %)
* @remark   
********************************************************************************
*/

INT8U BattDrvGetUseablePercent(void)
{
#ifdef BATT_TYPE_NOT_INSTALLED
	return 0;
#else
#ifdef BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_PORT
	INT16U cut_off_voltage;
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

#ifdef BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_PORT 
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

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/

/* Insert local functions here */
#endif//#ifdef BATT_DRV_EFM32G

