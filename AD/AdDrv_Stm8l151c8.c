/**
********************************************************************************
* Copyright (c) 2012 CAS
* @brief   AD Driver Source File\n
           Chip : STM8L151C8 ( MCU )
* @file    AdDrv_Stm8l151c8.c
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
#include "../Common/RingBuf.h"
#include "../Hardware/HardwareConfig.h"
#include "AdDrv.h"
#ifdef AD_DRV_STM8L151C8
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/

/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/

/** @brief KR : Battery Ad 값을 넣을 buffer size\n
		  US : \n
		  CN :									*/
#define AD_BUF_SIZE_FOR_BATT	10

    static INT32U calTemp = 0;
    static INT32U adc0TempRead1v25 = 0;
    static INT32U tempValue = 0;

/*
********************************************************************************
*                       LOCAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert local typedef & structure here */
/** @brief KR : Battery Voltage AD 값을 넣을 Ring Buffer에서 사용할 byte array\n
		  US : \n
		  CN :									*/
INT32U adDrvDataForBatt[AD_BUF_SIZE_FOR_BATT];

/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/

/** @brief KR : Battery Voltage AD 값을 넣을 Ring Buffer\n
		  US : \n
		  CN :									*/
RING_BUF AdDrvRingBufForBatt;



/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
/**
********************************************************************************
* @brief       KR : MCU STM8L151C8 AD Driver Init 함수, AD Ring Buffer Init, AD Chip Init\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \nwwwwwwdw
* @remark   KR : 초기에 실행 되는 함수\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
INT32U temperature, read_temperature = 1;
/* Private define ------------------------------------------------------------*/
#define HIGH_THRESHOLD_VOLTAGE ((INT16U)3500) /* 3.500 V */
#define LOW_THRESHOLD_VOLTAGE  ((INT16U)1500) /* 1.500 V */
#define ADC_RATIO              ((INT16U) 806) /*ADC_RATIO = ( 3.3 * 1000 * 1000)/4095 */
#define ASCII_NUM_0            ((INT8U)   48)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
INT16U HighThresholdData = 0;
INT16U LowThresholdData = 0;


void AdStm8l151c8DrvInit(void)
{
    RingBufInit(&AdDrvRingBufForBatt, (INT8U *)(&adDrvDataForBatt[0]), AD_BUF_SIZE_FOR_BATT);
    
	/* Enable ADC1 clock */
	CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);
	
	/* Initialise and configure ADC1 */
	ADC_Init(ADC1, ADC_ConversionMode_Continuous, ADC_Resolution_12Bit, ADC_Prescaler_1);
	ADC_SamplingTimeConfig(ADC1, ADC_Group_SlowChannels, ADC_SamplingTime_16Cycles);
	
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	
	/* Enable ADC1 Channel 3 */
	ADC_ChannelCmd(ADC1, ADC_Channel_23, ENABLE);

//	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

	ADC_SoftwareStartConv(ADC1);
}



/**
********************************************************************************
* @brief       KR : MCU EFM32G840 AD Driver Proct 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : 메번 선택적으로 AD값을 읽고 데이터를 얻는다 \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/

void AdStm8l151c8Proc()
{
	INT16U ADCData = 0;		
	if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)return;
	ADCData = ADC_GetConversionValue(ADC1);
	RingBufPutInt32sForAdDrv(&AdDrvRingBufForBatt, (INT32S)ADCData);
}


/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

#endif//#ifdef AD_DRV_STM8L151C8