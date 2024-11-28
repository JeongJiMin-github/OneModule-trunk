/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Driver Source File\n
           Chip : EMF32G840F128 ( MCU )
* @file    AdDrv_Efm32g840.c
* @version 1.0
* @date    2011/08/29
* @author  Deok Hyun Kim (KDH)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2011/08/29 created by KDH \n
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
#ifdef EFM32G840_AD             // duckspg �� ��� .c�� ��� �����Ͽ��� 
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

/** @brief KR : Channel 2(Battery) Ad ���� ���� buffer size\n
		  US : \n
		  CN :									*/
#define AD_BUF_SIZE_FOR_BATT	10

/** @brief KR : Power Check AD ���� ���� buffer size\n
		  US : \n
		  CN :									*/
#define AD_BUF_SIZE_FOR_POWERCHECK  	10


/** @brief KR : Channel 2(Battery) Ad ���� ���� buffer size\n
		  US : \n
		  CN :									*/


    static INT32U calTemp = 0;
    static INT32U adc0TempRead1v25 = 0;
    static INT32U tempValue = 0;

/*
********************************************************************************
*                       LOCAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert local typedef & structure here */
/** @brief KR : Battery Voltage AD ���� ���� Ring Buffer���� ����� byte array\n
		  US : \n
		  CN :									*/
INT32U adDrvDataForBatt[AD_BUF_SIZE_FOR_BATT];

/** @brief KR : Power Check AD ���� ���� Ring Buffer���� ����� byte array\n
		  US : \n
		  CN :									*/
INT32U adDrvDataForPowerCheck[AD_BUF_SIZE_FOR_POWERCHECK];


/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/

/** @brief KR : Battery Voltage AD ���� ���� Ring Buffer\n
		  US : \n
		  CN :									*/
RING_BUF AdDrvRingBufForBatt;
/** @brief KR : power check AD ���� ���� Ring Buffer\n
		  US : \n
		  CN :									*/
RING_BUF AdDrvRingBufForPowerCheck;


/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
/**
********************************************************************************
* @brief       KR : MCU EFM32G840 AD Driver Init �Լ�, AD Ring Buffer Init, AD Chip Init\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : �ʱ⿡ ���� �Ǵ� �Լ�\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void ADC0_IRQHandler(void)
{
  ADC_IntClear(ADC0, ADC_IF_SINGLE);
}


void AdEfm32g840DrvInit(void)
{
   
    RingBufInit(&AdDrvRingBufForBatt, (INT8U *)(&adDrvDataForBatt[0]), AD_BUF_SIZE_FOR_BATT);
    RingBufInit(&AdDrvRingBufForPowerCheck, (INT8U *)(&adDrvDataForPowerCheck[0]), AD_BUF_SIZE_FOR_POWERCHECK);
    
    
    ADC_Reset(ADC0);
    
    /* Base the ADC configuration on the default setup. */
    ADC_Init_TypeDef       init  = ADC_INIT_DEFAULT;
    
    /* Initialize timebases */
    init.timebase = ADC_TimebaseCalc(0);
    init.prescale = ADC_PrescaleCalc(1000000, 0);
    ADC_Init(ADC0, &init);
    
    /* Set input to temperature sensor. Reference must be 1.25V */
    singleInit.reference = adcRefVDD;
    singleInit.input     = AD_POWERCHECK_CH;
    ADC_InitSingle(ADC0, &singleInit);
    
    /* Setup interrupt generation on completed conversion. */
    ADC_IntEnable(ADC0, ADC_IF_SINGLE);
    NVIC_EnableIRQ(ADC0_IRQn);


    
}





/**
********************************************************************************
* @brief       KR : MCU EFM32G840 AD Driver Proct �Լ�\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : �޹� ���������� AD���� �а� �����͸� ��´� \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/

void AdEfm32g840Proc(void)
{
    static INT8U conversionFlag = 0;
    static INT8U adChSet = AD_BATT_CH;
    INT32S adcDataTemp = 0; 
   
    if (conversionFlag)
    {
        if (!(ADC0->STATUS & ADC_STATUS_SINGLEACT))
        {
            conversionFlag = 0;
            adcDataTemp = ADC_DataSingleGet(ADC0);
            switch(adChSet)
            {
                case AD_BATT_CH:
                    RingBufPutInt32sForAdDrv(&AdDrvRingBufForBatt, adcDataTemp);     // duckspg batt check disable
                    adChSet = AD_POWERCHECK_CH;
                    singleInit.input = adChSet;
                    singleInit.reference = adcRefVDD;
                    break;
                case AD_POWERCHECK_CH:
                    // ������ �Է� �߰� 
                    RingBufPutInt32sForAdDrv(&AdDrvRingBufForPowerCheck, adcDataTemp);
                    adChSet = AD_BATT_CH;
                    singleInit.input = adChSet;
                    singleInit.reference = adcRefVDD;
                    break;
                default:
                    adChSet = AD_BATT_CH;       // duckspg error ���� 
                    break;
            }
        }
    }
    else
    {
        conversionFlag = 1;
        ADC_InitSingle(ADC0, &singleInit);    
        ADC_Start(ADC0, adcStartSingle);
    }
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */


#endif//#ifdef EFM32G840_AD
