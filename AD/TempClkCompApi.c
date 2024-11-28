/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Temperature Clk Compensation API Source File\n
* @file    TempClkCompApi.c
* @version 1.0
* @date    2013/01/09
* @author  Deok Hyun Kim (KDH)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/12/05 created by HYP \n
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
#include "TempClkCompApi.h"
#include "../Common/RingBuf.h"
#include "../AD/AdDrv_Efm32g840.h"
#include "../Hardware/HardwareConfig.h"
#include "../Hardware/HardwareDrv.h"
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

/** @brief TEMPERATURECHECK �� Avr size*/
#define TEMPERATURE_AVR_SIZE 3


/*
********************************************************************************
*                       LOCAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert local typedef & structure here */

/** @brief Temperature check ring buffer �ּ� ���� */
static RING_BUF *TemperatureCheckRingBufPtr;
FP32 convertToCelsius(INT32U adcSample);
TEMP_CLK_COMP_STRUCT tempCompensation;
/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/

#ifdef USE_TEMP_CLK_COMPENSATION
/** @brief ��°� ��»����� ���ǰ� ���� ����*/
INT16S spanDiffMidToHigh;

#endif // #ifdef USE_TEMP_CLK_COMPENSATION
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
* @brief    �µ� ���� �ʿ��� ������ �ʱ�ȭ�ϴ� �Լ�\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void TempInitClkVariable(void)
{
#ifdef USE_TEMP_CLK_COMPENSATION

    TemperatureCheckRingBufPtr = (&AdDrvRingBufForTemperatureCheck);
    tempCompensation.tempCompEnable = EfmTuning;
    tempCompensation.tempHighEndterValue = 40;
    tempCompensation.tempHighEndValue = 35;
    tempCompensation.tempLowEndterValue = 0;
    tempCompensation.tempLowEndValue = 5;

#endif//#ifdef USE_TEMP_CLK_COMPENSATION		
}

/**
********************************************************************************
* @brief    �µ� ������ ���� �ϴ� �Լ�\n
* @param    ���� AD ��
* @return   ����� AD��
* @remark   ���� AD���� (re)calibration span, zero������ ������ ������ 
********************************************************************************
*/
INT32S TempClkCompProc(void)
{

#ifdef USE_TEMP_CLK_COMPENSATION	
    INT8U size;
    INT8U i;
    INT32U avrTemperatureCheckVolt;
    INT32U temperatureCurr;
    static FP32 temperatureFiltValue;
    static INT8U currentTempFlag = 1;  // 1 : ��� 2: ��� 0: ���� 
    static INT8U tuningTemp;
    

    
    size = RingBufCountSize(TemperatureCheckRingBufPtr);
	if (size >= TEMPERATURE_AVR_SIZE)
	{
        avrTemperatureCheckVolt = 0;
        for (i = 0; i < TEMPERATURE_AVR_SIZE; i++)
        {
            avrTemperatureCheckVolt = avrTemperatureCheckVolt + RingBufGetInt32s(TemperatureCheckRingBufPtr);
        }
        
        temperatureFiltValue = avrTemperatureCheckVolt / TEMPERATURE_AVR_SIZE;
        temperatureFiltValue = (INT32S)(convertToCelsius(temperatureFiltValue));
        
        if (currentTempFlag == 1)       //��� 
        {
            if (temperatureFiltValue > tempCompensation.tempHighEndterValue)
            {
                currentTempFlag = 2;
                tuningTemp = EfmTuning - 10;
                CMU->HFRCOCTRL = CMU_HFRCOCTRL_BAND_1MHZ | tuningTemp;
            }
            if (temperatureFiltValue < tempCompensation.tempLowEndterValue)
            {
                currentTempFlag = 0;
                tuningTemp = EfmTuning + 10;
                CMU->HFRCOCTRL = CMU_HFRCOCTRL_BAND_1MHZ | tuningTemp;
            }
        }
        else if (currentTempFlag == 2)      // ��� 
        {
            if (temperatureFiltValue < tempCompensation.tempHighEndValue)
            {
                currentTempFlag = 1;
                CMU->HFRCOCTRL = CMU_HFRCOCTRL_BAND_1MHZ | EfmTuning;
            }
        }
        else        // currentTempFlag = 0; �����϶� 
        {
            if (temperatureFiltValue > tempCompensation.tempLowEndValue)
            {
                currentTempFlag = 1;
                CMU->HFRCOCTRL = CMU_HFRCOCTRL_BAND_1MHZ | EfmTuning;
            }
        }

    }
    
    
#endif//#ifdef USE_TEMP_CLK_COMPENSATION
}
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */



FP32 convertToCelsius(INT32U adcSample)
{
  FP32 temp;
  /* Factory calibration temperature from device information page. */
  FP32 cal_temp_0 = (FP32)((DEVINFO->CAL & _DEVINFO_CAL_TEMP_MASK)
                            >> _DEVINFO_CAL_TEMP_SHIFT);

  FP32 cal_value_0 = (FP32)((DEVINFO->ADC0CAL2
                            & _DEVINFO_ADC0CAL2_TEMP1V25_MASK)
                            >> _DEVINFO_ADC0CAL2_TEMP1V25_SHIFT);

  /* Temperature gradient (from datasheet) */
  FP32 t_grad = -6.27;

  temp = (cal_temp_0 - ((cal_value_0 - adcSample)  / t_grad));

  return temp;
}