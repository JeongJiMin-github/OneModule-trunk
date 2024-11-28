/**
********************************************************************************
* Copyright (c) 2016 CAS
* @brief   AD Driver Source File\n
           Chip : M0516LDN
* @file    AdDrv_M0516ldn.c
* @version 1.0
* @date    2016/12/06
* @author  Keum Young Kwang (KYK)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2016/12/06 created by KYK \n
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

#ifdef AD_M0516LDN
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

/** @brief KR : Channel 2(Battery) Ad 값을 넣을 buffer size\n
		  US : \n
		  CN :									*/
#define AD_BUF_SIZE_FOR_BATT	10

/** @brief KR : Power Check AD 값을 넣을 buffer size\n
		  US : \n
		  CN :									*/
#define AD_BUF_SIZE_FOR_POWERCHECK  	10

#ifdef TEMP_DRV_NTC10K
#define AD_BUF_SIZE_FOR_NTC10K	10
#endif

#ifdef AD_DRV_M0516LDN_MUL_CH
/** @brief KR : AdDrvCpuAdcProc의 state 최대\n
		  US : \n
		  CN :									*/
#define AD_CONV_STATE_MAX	3
#endif

/** @brief KR : Channel 2(Battery) Ad 값을 넣을 buffer size\n
		  US : \n
		  CN :									*/

static INT32U calTemp = 0;
static INT32U adc0TempRead1v25 = 0;
static INT32U tempValue = 0;

#ifdef TEMP_DRV_NTC10K
static INT16U ntc10kAdr[17] = {	738,	895,	1068,	1256,	1455,	1659,	1866,	2070,	2268,	2457,	2635,	2799,	2948,	3084,	3206,	3314,	3410};	// NCU18XH103F60RB 적용
static INT16S ntc10kTemp[17]= {	-200,	-150,	-100,	-50,	0,		50,		100,	150,	200,	250,	300,	350,	400,	450,	500,	550,	600};
static INT16U ntc10kLen = 17;
#endif
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

/** @brief KR : Power Check AD 값을 넣을 Ring Buffer에서 사용할 byte array\n
		  US : \n
		  CN :									*/
INT32U adDrvDataForPowerCheck[AD_BUF_SIZE_FOR_POWERCHECK];

#ifdef TEMP_DRV_NTC10K
INT16U adDrvDataForNtc10k[AD_BUF_SIZE_FOR_NTC10K];

RING_BUF adDrvRingBufForNtc10k;
#endif

/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/

/** @brief KR : Battery Voltage AD 값을 넣을 Ring Buffer\n
		  US : \n
		  CN :									*/
RING_BUF AdDrvRingBufForBatt;
/** @brief KR : power check AD 값을 넣을 Ring Buffer\n
		  US : \n
		  CN :									*/
RING_BUF AdDrvRingBufForPowerCheck;


/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
//ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
/**
********************************************************************************
* @brief       KR : MCU EFM32G840 AD Driver Init 함수, AD Ring Buffer Init, AD Chip Init\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : 초기에 실행 되는 함수\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
//void ADC0_IRQHandler(void)
//{
//  ADC_IntClear(ADC0, ADC_IF_SINGLE);
//}


void AdDrvCpuAdcInit(void)
{
#ifdef AD_DRV_M0516LDN_MUL_CH
	RingBufInit(&AdDrvRingBufForBatt, (char *)(&adDrvDataForBatt[0]), AD_BUF_SIZE_FOR_BATT);
#else
    //INT8U  u8Option;
    //INT32U u32ChannelCount;
    //INT32S  i32ConversionData;

    RingBufInit(&AdDrvRingBufForBatt, (char *)(&adDrvDataForBatt[0]), AD_BUF_SIZE_FOR_BATT);
    //RingBufInit(&AdDrvRingBufForPowerCheck, (INT8U *)(&adDrvDataForPowerCheck[0]), AD_BUF_SIZE_FOR_POWERCHECK);

    /* Set the ADC operation mode as continuous scan, input mode as single-end */
    ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_CONTINUOUS, 0x20);	//0x01 = CH0, 0x02 = CH1, 0x20 = CH5

    /* Power on ADC module */
    ADC_POWER_ON(ADC);

    /* clear the A/D interrupt flag for safe */
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);

    /* start A/D conversion */
    ADC_START_CONV(ADC);
#endif
}

/**
********************************************************************************
* @brief       KR : MCU stm32f030c8 AD Driver Proct 함수\n
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
void AdDrvCpuAdcProc(void)
{
#ifdef AD_DRV_M0516LDN_MUL_CH
	static INT8U conversionState = 0;
	static INT16U adcDataTemp = 0;

	switch (conversionState)
	{
		case 0:
			/* Set the ADC operation mode as single scan, input mode as single-end and
				 enable the analog input channel 6 */
			ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE, 0x20);	//0x20 = CH5
			/* Power on ADC module */
			ADC_POWER_ON(ADC);
			/* clear the A/D interrupt flag for safe */
			ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
			/* start A/D conversion */
			ADC_START_CONV(ADC);
			conversionState++;
			break;

		case 1:
			if (!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT)) return;
			ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
					  adcDataTemp = ADC_GET_CONVERSION_DATA(ADC, 5); //ch5
					  RingBufPutInt32sForAdDrv(&AdDrvRingBufForBatt, adcDataTemp);
					  ADC_POWER_DOWN(ADC);
					  conversionState++;
					  break;
					  
#ifdef TEMP_DRV_NTC10K
				  case 2:
					  /* Set the ADC operation mode as single scan, input mode as single-end and
						   enable the analog input channel 6 */
					  ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE, 0x40);  //0x40 = CH6
					  /* Power on ADC module */
					  ADC_POWER_ON(ADC);
					  /* clear the A/D interrupt flag for safe */
					  ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
					  /* start A/D conversion */
					  ADC_START_CONV(ADC);
					  conversionState++;
					  break;
			
				  case 3:
					  if (!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT)) return;
					  ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
					  adcDataTemp = ADC_GET_CONVERSION_DATA(ADC, 6); //ch6
					  if (adDrvRingBufForNtc10k.Buf)
						  RingBufPutInt16u(&adDrvRingBufForNtc10k, adcDataTemp);
					  ADC_POWER_DOWN(ADC);
					  conversionState++;
					  break;
#endif
				  default:
					  ADC_POWER_DOWN(ADC);
					  if (conversionState > AD_CONV_STATE_MAX)	  // Todo. 코드 추가하여 conversionState 증가시 AD_CONV_STATE_MAX 수정
					  {
						  conversionState = 0;
					  }
					  else
					  {
						  conversionState++;
					  }
			break;
	}
#else
    static INT8U conversionFlag = 0;
    static INT32S adcDataTemp = 0;

    if (conversionFlag)
    {
        /* Wait conversion done */
        while(!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT));

        conversionFlag = 0;

        /* clear the A/D interrupt flag for safe */
        ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);

        adcDataTemp = ADC_GET_CONVERSION_DATA(ADC, 5); //ch5
        RingBufPutInt32sForAdDrv(&AdDrvRingBufForBatt, adcDataTemp);

        /* Wait conversion done */
        while(!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT));

        /* Stop A/D conversion */
        ADC_STOP_CONV(ADC);
    }
    else
    {
        conversionFlag = 1;

        /* start A/D conversion */
        ADC_START_CONV(ADC);
    }
#endif // AD_DRV_M0516LDN_MUL_CH
}

#ifdef TEMP_DRV_NTC10K
BOOLEAN TempDrvInit(void)
{
	RingBufInit(&adDrvRingBufForNtc10k, (char *)(&adDrvDataForNtc10k[0]), AD_BUF_SIZE_FOR_NTC10K);
//	return 0;
}

void TempDrvProc (INT16U currTimer100ms)
{

}

INT16S TempDrvReadData(void)
{
	INT16S adBuf = 0, result;
	INT8U i;
	
	if (adDrvRingBufForNtc10k.Buf)
	{
		for (i = 0 ; i < AD_BUF_SIZE_FOR_NTC10K ; i++)
			adBuf += (INT16S)(RingBufGetInt16u(&adDrvRingBufForNtc10k));
	}
	
	adBuf /= AD_BUF_SIZE_FOR_NTC10K;
	
	if (adBuf < ntc10kAdr[0]) return 200;
	
	for (i = 0 ; i < ntc10kLen - 2 ; i++)
	{
		if (adBuf < ntc10kAdr[i + 1])
		{
			result = (adBuf - ntc10kAdr[i]) * (ntc10kTemp[i + 1] - ntc10kTemp[i]) / (ntc10kAdr[i + 1] - ntc10kAdr[i])  + ntc10kTemp[i];
			return result;
		}
	}
	
	return 200;
}
#endif //#ifdef TEMP_DRV_NTC10K

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */


#endif//#ifdef AD_M0516LDN
