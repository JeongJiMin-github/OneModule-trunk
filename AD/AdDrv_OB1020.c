/**
********************************************************************************
* Copyright (c) 2016 CAS
* @brief   AD Driver Source File\n
           Chip : OB1020
* @file    AdDrv_OB1020.c
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
#include "../Common/RingBuf.h"
#include "AdDrv.h"
#include "../Hardware/HardwareConfig.h"
#ifdef AD_DRV_OB1020
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief KR : Loadcell AD 값을 넣을 Ring Buffer\n
		  US : \n
		  CN :									*/
RING_BUF AdDrvRingBuf;

/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief KR : Channel 1(L/C) Ad 값을 넣을 buffer size\n
		  US : \n
		  CN :									*/
#define AD_BUF_SIZE		10


/** @brief KR : CS5550 AD sampling Freq. (Hz)\n
		  US : CS5550 AD sampling Freq. (Hz)\n
		  CN :									*/
#ifdef AD_DRV_FREQ
#define OB1020_AD_FREQ	AD_DRV_FREQ
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
/** @brief KR : Loadcell AD 값을 넣을 Ring Buffer에서 사용할 byte array\n
		  US : \n
		  CN :									*/
INT32U adDrvData[AD_BUF_SIZE];



/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
//static void ads1230Init(void);
//static INT32S ads1230DataRead(void);

static INT32S OB1020DataRead(void);

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
/**
********************************************************************************
* @brief       KR : AD Driver Init 함수, AD Ring Buffer Init, AD Chip Init\n
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
void AdDrvInit(void)
{
	RingBufInit(&AdDrvRingBuf, (char *)(&adDrvData[0]), AD_BUF_SIZE);

#ifdef AD_M0516LDN
	AdDrvCpuAdcInit();
#endif
}
/**
********************************************************************************
* @brief       KR : AD Sampling Rate 반환 함수\n
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
	return (INT16U)OB1020_AD_FREQ;
}

/**
********************************************************************************
* @brief       KR : AD Driver Procedure, AD Chip에서 읽은 값을 Ring Buffer에 저장\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : 일정 시간에 한번씩 계속 실행 되는 함수\n
			     AD Chip의 Sampling 시간 보다는 빨리 실행 되어야 함\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/

void AdDrvProc(void)
{
	INT32S adData;

	OB1020_CLK_LOW;

	if (!OB1020_DOUT)
	{
		adData = OB1020DataRead();
	
		if (adData & 0x800000)       // 참이면 minus
		{  			
			adData = 0;
		}
        RingBufPutInt32sForAdDrv(&AdDrvRingBuf, adData);
	}
	AdDrvCpuAdcProc();
}


/**
********************************************************************************
* @brief       KR : OB1020 AD Chip에 8 bit 보내는 함수\n
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
void delay_clock(unsigned char count)
{
	INT8U  i;

	for(i = 0; i < count; i++)
		;
}

INT32S OB1020DataRead(void)
{
	INT32U bData,i;

	bData = 0x00000000;
    
    //For preventing CLK high time < 50 us
    NVIC_DisableIRQ(TMR2_IRQn);
#ifndef USE_BACK_LIGHT
    NVIC_DisableIRQ(TMR3_IRQn);
#endif
	for (i = 0; i < 24; i++)
	{
        bData <<= 1;

        //For preventing CLK high time < 50 us
        NVIC_DisableIRQ(UART0_IRQn);
    
        OB1020_CLK_HIGH;
#ifdef USE_BACK_LIGHT
        //clk high time > 0.2 us
        //clk low time > 0.2 us
        //2.76 MHz = 0.36 us
        //5.52 MHz = 0.18 us
        delay1Clock();
#endif
        OB1020_CLK_LOW;
#ifdef USE_BACK_LIGHT
        delay1Clock();
#endif
        NVIC_EnableIRQ(UART0_IRQn);
        
        if (OB1020_DOUT) bData++;
	}
    
    //add 1 CLK setting for 10Hz
    //For preventing CLK high time < 50 us
    NVIC_DisableIRQ(UART0_IRQn);
    
    OB1020_CLK_HIGH;
#ifdef USE_BACK_LIGHT
    delay1Clock();
#endif
    OB1020_CLK_LOW;
#ifdef USE_BACK_LIGHT
    delay1Clock();
#endif
    
    NVIC_EnableIRQ(TMR2_IRQn);
#ifndef USE_BACK_LIGHT
    NVIC_EnableIRQ(TMR3_IRQn);
#endif
    NVIC_EnableIRQ(UART0_IRQn);
    
	return (INT32S)(bData);
}

#endif//#ifdef AD_DRV_OB1020
