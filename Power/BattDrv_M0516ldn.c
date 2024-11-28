/**
********************************************************************************
* Copyright (c) 2016 CAS
* @brief   Battery Driver Soruce File\n
           Chip : M0516LDN
* @file    BattDrv_M0516ldn.c
* @version 1.0
* @date    2016/12/06
* @author  Park Young Min (FDC954)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2016/12/06 created by FDC954 \n
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
#ifdef BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_VOLT
#include "../Power/PowerApi.h"
#endif

#ifdef BATT_DRV_M0516LDN
#include "../Ad/AdDrv_M0516ldn.h"

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

	#define REF_BAT_COMP_VALUE   0.7		//Divide ���� = ??
	
	#define BATT_RESOLUTION      4096		//(12bit)

#endif

#ifdef BATT_TYPE_DRY_6V
	/**
	* @brief Battery Cut off Voltage\n
	*        ex) 370 -> 3.70 Volt
	*        SWII-W�� ��� x.xV ���ϴ� ������ ���� �߻� Ȯ�� �ʿ�
	*        Backlight����ϰų� LED type�� ��� Ư�� ���� �ʿ�
	*        Dry battery���� ��������� ���̿���� ���Ͽ� 0.3V���� ���� �߻�
	*        SWII-W ���� �ƿ��� ���� 3.7V
	*/        
	#define BATT_CUT_OFF_VOLTAGE 340
	/**
	* @brief Battery Full Voltage (100 %) \n
	*        ex) 600 -> 6.00 Volt
	*/
	#define BATT_FULL_VOLTAGE_IN_CHARGE 570
	#define BATT_FULL_VOLTAGE_IN_DISCHARGE 570

	#define REF_BAT_COMP_VALUE   7.76		//(100K�� 15K�� 115/15 = 7.66666 ������, CPU ADC�� ������������ ���� ����ġ�� �ʿ�(0.19 ����)
	
	#define BATT_RESOLUTION      805.7		//(12bit)
#endif

#ifdef BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_PORT
	/**
	* @brief Dry Battery Cut off Voltage\n
	*/
//	#define BATT_DRY_CUT_OFF_VOLTAGE 420
	#define BATT_DRY_CUT_OFF_VOLTAGE 340

//	#define REF_BAT_COMP_VALUE   0.7		//Divide ���� = ??
	#define REF_BAT_COMP_VALUE   7.76		//(100K�� 15K�� 115/15 = 7.66666 ������, CPU ADC�� ������������ ���� ����ġ�� �ʿ�(0.19 ����)
	
    #ifdef AD_OPERATING_VOLTAGE_3_3V
        #define BATT_RESOLUTION       805.7		//(12bit)
    #else
        #define BATT_RESOLUTION      1220		//(12bit)
    #endif
#endif

#ifdef BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_VOLT //For ER-jr Dry battery and Pb battery option. 
    /**
    * @brief Dry Battery Cut off Voltage\n
    * 	ER-jr Dry battery type�� ������ ����̵��Ͽ� Ȯ���ϴ� �����.
    *     ��� ������ ����̵� �� ������.
    */
    #define BATT_DRY_CUT_OFF_VOLTAGE 163 //���� ������ ���� 4.5V�� ���� ��, �¾� ���ַ����� �Է������� ���̿��� ��� ����, 4.2V
    #define BATT_DRY_FULL_VOLATGE 212       //���� ������ ���� 6V�� ���� ��, �¾� ���ַ����� �Է������� ���̿��� ��� ����, 6.0V
    #define BATT_TYPE_CHECK_VOLTAGE 250  //Real Volatge 7V
    #define PB_BATT 0
    #define DRY_BATT 1
#ifdef USE_AUTO_POWER_SETTING
    #define ADAPTER 2
#endif
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
	#define REF_BAT_COMP_VALUE   7.72		//(100K�� 15K�� 115/15 = 7.66666 ������, CPU ADC�� ������������ ���� ����ġ�� �ʿ�(0.09 ����)	
	#define BATT_RESOLUTION      1220		//(12bit)
    
    #define BATT_FULL_VOLTAGE_IN_CHARGE 705 //������ 100% ���� ����
    #define BATT_FULL_VOLTAGE_IN_DISCHARGE 655 //������ 100% ���� ����
#ifdef USE_AUTO_POWER_SETTING
    #define ADAPTER_TYPE_CHECK_VOLTAGE 700 // Adapter �����ΰ� ���� �� ���� 
#endif
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
	#define REF_BAT_COMP_VALUE   7.72		//(100K�� 15K�� 115/15 = 7.66666 ������, CPU ADC�� ������������ ���� ����ġ�� �ʿ�(0.09 ����)	
        
        #ifdef AD_OPERATING_VOLTAGE_3_3V
              #define BATT_RESOLUTION       805.7		//(12bit)
        #else
              #define BATT_RESOLUTION      1220		//(12bit)
        #endif
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
	#define REF_BAT_COMP_VALUE   7.76		//(100K�� 15K�� 115/15 = 7.66666 ������, CPU ADC�� ������������ ���� ����ġ�� �ʿ�(0.19 ����)
	#define BATT_RESOLUTION      805.7		// 0.0008057(12bit) * 10^6
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
	#define REF_BAT_COMP_VALUE   0.7		//Divide ���� = ??	
	#define BATT_RESOLUTION      4096		//(12bit)
#endif

#ifdef BATT_TYPE_NOT_INSTALLED
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
	#define REF_BAT_COMP_VALUE   0.7		//Divide ���� = ??	
	#define BATT_RESOLUTION      4096		//(12bit)
#endif


/**
* @brief Battery Threshold voltage magnitude for knowing charge or discharge state \n
*        B/L, Suspend mode�� ���� ���� ������ �����Ͽ�����\n
*        ex) 10 -> 0.10 Volt
*/
#define BATT_VOLTAGE_THRES_FOR_SLOPE_POSITIVE   10
#define BATT_VOLTAGE_THRES_FOR_SLOPE_NEGATIVE   3

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
#define BATT_DISPLAY_DP      10000
#ifdef BATT_DIODE_DROP
	#define BATT_DROP_VOLTAGE    30           //0.3V diode drop
	#else
	#define BATT_DROP_VOLTAGE    0
#endif

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
			
			//Check charge or discharge for checking battery capacity
			batt_curr_volt = (INT16S)BattDrvReadVolt();
			if ((BattResetChargingRefVoltTimer100ms == 0) || (BattResetChargingRefVoltTimer100ms == 0xff))
			{
				BattResetChargingRefVoltTimer100ms = 0;
				batt_volt_diff = batt_curr_volt - BattPrevVolt;
				if (batt_volt_diff > BATT_VOLTAGE_THRES_FOR_SLOPE_POSITIVE)
				{
					BattChargeFlag = TRUE;          // DUCKSPG
					BattPrevVolt = batt_curr_volt;
				}
				//else if (batt_volt_diff < (-BATT_VOLTAGE_THRES_FOR_SLOPE))
                                else if (batt_volt_diff < (BATT_VOLTAGE_THRES_FOR_SLOPE_NEGATIVE))
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

    //retVol = (INT32S)(REG_VALUE_DIVIDE*(BATT_RESOLUTION * (FP64)(BattAdFiltValue)));
	retVol = (INT32S)(REF_BAT_COMP_VALUE * (BATT_RESOLUTION * (FP64)(BattAdFiltValue)));
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
#ifdef BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_VOLT
    INT8U batt_type;
    batt_type = BattTypeCheck();
    
    if (batt_type == DRY_BATT)
    {
        if (BattDrvGetUseablePercent() < BATT_LOW_THRESHOLD_DOWN)
        {
            BattLowFlag = 1;
            BlDrvSetEnableFlag(DISABLE);
        }
    }
	else 
#endif
    {
        if (BattDrvGetUseablePercent() < BATT_LOW_THRESHOLD_DOWN)
        {
            BattLowFlag = 1;
        }
        else if (BattDrvGetUseablePercent() >= BATT_LOW_THRESHOLD_UP)
        {
            BattLowFlag = 0;
        }
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
			
    if ((batt_type == DRY_BATT) && BattChargeFlag) 
    {
        percent = 0; //omv4 cpu ���� ������ ���� �� �����Ʈ�� �� �� ������ �ⷷ�Ÿ��� ���� ����
        return percent;
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
* @brief    Battery Type Check �Լ�\n
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
* @brief	Power Type Check �Լ�\n
* @param	none
* @return	2-Adapter, 1-Pb_Battery, 0-Dry_Battery
* @remark	
********************************************************************************
*/
INT8U PowerTypeCheck(void)
{
    INT16U batt_check_volt;
    batt_check_volt = BattDrvReadVolt();
    
	if (batt_check_volt > ADAPTER_TYPE_CHECK_VOLTAGE)
	{
 		return ADAPTER;
	}
	else if(batt_check_volt < BATT_TYPE_CHECK_VOLTAGE)
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
#endif//#ifdef BATT_DRV_M0516LDN

