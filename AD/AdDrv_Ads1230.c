/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Driver Source File\n
           Chip : ads1230
* @file    AdDrv_Ads1230.c
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
#include "../Common/RingBuf.h"
#include "AdDrv.h"
#ifdef USE_EFM32G_MCU
#include "AdDrv_Efm32g840.h"
#endif
#ifdef USE_STM8L151C8_MCU
#include "AdDrv_Stm8l151c8.h"
#endif
#include "../Hardware/HardwareConfig.h"
#ifdef AD_DRV_ADS1230
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief KR : Loadcell AD ���� ���� Ring Buffer\n
		  US : \n
		  CN :									*/
RING_BUF AdDrvRingBuf;

/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief KR : Channel 1(L/C) Ad ���� ���� buffer size\n
		  US : \n
		  CN :									*/
#define AD_BUF_SIZE		10

/** @brief KR : CS5550 AD sampling Freq. (Hz)\n
		  US : CS5550 AD sampling Freq. (Hz)\n
		  CN :									*/
#ifdef AD_DRV_FREQ
#define ADS1230_AD_FREQ	AD_DRV_FREQ
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
/** @brief KR : Loadcell AD ���� ���� Ring Buffer���� ����� byte array\n
		  US : \n
		  CN :									*/
INT32U adDrvData[AD_BUF_SIZE];



/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
static void ads1230Init(void);
static INT32S ads1230DataRead(void);

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
/**
********************************************************************************
* @brief       KR : AD Driver Init �Լ�, AD Ring Buffer Init, AD Chip Init\n
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
void AdDrvInit(void)
{
	RingBufInit(&AdDrvRingBuf, (INT8U *)(&adDrvData[0]), AD_BUF_SIZE);
	ads1230Init();
#ifdef USE_EFM32G_MCU
	AdEfm32g840DrvInit();
#endif
#ifdef USE_STM8L151C8_MCU
	AdStm8l151c8DrvInit();
#endif

}
/**
********************************************************************************
* @brief       KR : AD Sampling Rate ��ȯ �Լ�\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : Sampling rate\n
*                 US : Sampling rate\n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
INT16U AdDrvGetSamplingRate(void)
{
	return (INT16U)ADS1230_AD_FREQ;
}

/**
********************************************************************************
* @brief       KR : AD Driver Procedure, AD Chip���� ���� ���� Ring Buffer�� ����\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : ���� �ð��� �ѹ��� ��� ���� �Ǵ� �Լ�\n
			     AD Chip�� Sampling �ð� ���ٴ� ���� ���� �Ǿ�� ��\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/



void AdDrvProc(void)
{
	INT32S adData;


	if (!ADS1230_SDK)
	{ 
		adData = ads1230DataRead();
	
		if (adData & 0x80000)       // ���̸� minus
		{  			
			adData = 0;
		}
        RingBufPutInt32sForAdDrv(&AdDrvRingBuf, adData);            // duckspg AD
	}
#ifdef USE_STM8L151C8_MCU
	AdStm8l151c8Proc();
#endif
	
}
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
/**
********************************************************************************
* @brief       KR : CS5550 AD Chip Init\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : �ʱ⿡ �ѹ� �����ϰų� Chip �ʱ�ȭ �ʿ�� ����\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void ads1230Init(void)
{
    INT8U i;
    static INT8U offsetCalStart = 1;
    if (offsetCalStart)
    {
        ADS1230_POWERON;
        offsetCalStart = 0;
        while(ADS1230_SDK);     // duckspg ���� ������ �Ǽ� ADS1230_SDK�� 0���� �ȶ������� errorǥ�ø� �ؾ��ϴ��� 
        for (i = 0; i < 26; i++)
        {
          ADS1230_CLK_HIGH;
          ADS1230_CLK_LOW;
        }
    }
}

/**
********************************************************************************
* @brief       KR : CS5550 AD Chip�� 8 bit ������ �Լ�\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
INT32S ads1230DataRead(void)
{
  INT32U bData,i;
  bData = 0x00000000;
  for (i = 0; i < 21; i++)
  {
      bData <<= 1;
      ADS1230_CLK_HIGH;
      ADS1230_CLK_LOW;
      if (ADS1230_SDK) 
      {
              bData |= 0x00000001;
      }
      else
      {
              bData &= 0xfffffffe;
      }
  }

  return (INT32S)(bData>>1);
}

#endif//#ifdef AD_DRV_ADS1230
