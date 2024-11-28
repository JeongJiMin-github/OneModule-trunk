/**
********************************************************************************
* Copyright (c) 2017 CAS
* @brief   AD Driver Source File\n
           Chip : xm24l
* @file    AdDrv_xm24l.c
* @version 1.0
* @date    2017/12/04
* @author  Seong Woong Yang (YSW)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2017/12/04 created by YSW \n
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
#if defined(AD_DRV_XM24L) || defined(AD_DRV_XM24L_3_3V) 
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief KR : Loadcell AD 값을 넣을 Ring Buffer\n
		  US : \n
		  CN :									*/
RING_BUF XM24L_AdDrvRingBuf;

/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief KR : Channel 1(L/C) Ad 값을 넣을 buffer size\n
		  US : \n
		  CN :									*/
#define XM24L_AD_BUF_SIZE		10

#define XM24L_WRITE_MODE_CMD  0x65
#define XM24L_READ_MODE_CMD   0x56 

/** @brief KR : CS5550 AD sampling Freq. (Hz)\n
		  US : CS5550 AD sampling Freq. (Hz)\n
		  CN :									*/
#ifdef AD_DRV_FREQ
#define XM24L_AD_FREQ	AD_DRV_FREQ
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
INT32U XM24L_adDrvData[XM24L_AD_BUF_SIZE];



/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
static void XM24LSendCmd(INT8U data);
static void XM24LInit(void);
static INT32S XM24LDataRead(void);

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
void XM24L_AdDrvInit(void)
{
	RingBufInit(&XM24L_AdDrvRingBuf, (char *)(&XM24L_adDrvData[0]), XM24L_AD_BUF_SIZE);
    //XM24LInit();

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
INT16U XM24L_AdDrvGetSamplingRate(void)
{
	return (INT16U)XM24L_AD_FREQ;
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
void XM24L_AdDrvProc(void)
{
	INT32S adData;

    XM24L_CLK_LOW;
	if (!XM24L_DOUT)
	{ 
		adData = XM24LDataRead();
	
		if (adData & 0x800000)       // 참이면 minus
		{
			adData = 0;
		}
        RingBufPutInt32sForAdDrv(&XM24L_AdDrvRingBuf, adData);
	}
	AdDrvCpuAdcProc();
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
/**
********************************************************************************
* @brief       KR : XM24L Command Mode\n
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
void XM24LCmdMode(INT8U selectCmd)
{
    INT8U i;
    
    for(i = 0; i < 7; i++)
    {
        selectCmd <<= 1;  

        if(selectCmd&0x80)
        {
            XM24L_DOUT_HIGH;
            delay2Clock();
        }
        else
        {
            XM24L_DOUT_LOW;
            delay2Clock();
        }
        
        XM24L_CLK_HIGH;
        delay2Clock();
        XM24L_CLK_LOW;
        delay2Clock();
    }
}

/**
********************************************************************************
* @brief       KR : XM24L Send Command \n
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
void XM24LSendCmd(INT8U command)
{
    INT8U i;
    for(i = 0; i < 8; i++)
    {
        if(command&0x80)
        {
            XM24L_DOUT_HIGH;
            delay2Clock();
        }
        else
        {
            XM24L_DOUT_LOW;
            delay2Clock();
        }
        
        XM24L_CLK_HIGH;
        delay2Clock();
        XM24L_CLK_LOW;
        command <<= 1;
        delay2Clock();

    }
}
/**
********************************************************************************
* @brief       KR : XM 24LL AD Driver Init 함수, AD Ring Buffer Init, AD Chip Init\n
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
void XM24LInit(void)
{
    INT32U bData = 0;
    INT8U i;

    XM24L_CLK_LOW;

    while(XM24L_DOUT); 

    for (i = 0; i < 24; i++)
    {
        XM24L_CLK_HIGH;
        bData << 1;
        if (XM24L_DOUT) bData++;
        XM24L_CLK_LOW;    
    }
    
    for(i = 0; i < 5; i++)
    {
        XM24L_CLK_HIGH; // XM24L_CLK 25~26
        delay2Clock();
        XM24L_CLK_LOW;
        delay2Clock();
    }

    XM24LCmdMode(XM24L_WRITE_MODE_CMD); //  //0x65(write command)= 110 0101    clock : 30~36
    
    XM24L_CLK_HIGH; // XM24L_CLK 37    
    delay2Clock();
    XM24L_CLK_LOW;
    delay2Clock()
    
    XM24LSendCmd(0x0c); //  //0x0c = 0000 1100  AD init setting  clock 38~45

    XM24L_DOUT_INCHANGE;
    
    XM24L_CLK_HIGH; // XM24L_CLK 46
    delay2Clock();
    XM24L_CLK_LOW;
    delay2Clock();
}

/**
********************************************************************************
* @brief       KR : XM24 AD Chip에 8 bit 보내는 함수\n
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
INT32S XM24LDataRead(void)
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

        XM24L_CLK_HIGH;
        //clk high time > 0.45 us
        //clk low time > 0.45 us
        //2.76 MHz = 0.36 us
        //5.52 MHz = 0.18 us
        delay1Clock();
    
        XM24L_CLK_LOW;
        delay1Clock();
      
        NVIC_EnableIRQ(UART0_IRQn);
        if (XM24L_DOUT) bData++;
    }
    NVIC_DisableIRQ(UART0_IRQn);
    
    XM24L_CLK_HIGH;

    delay1Clock();

    XM24L_CLK_LOW;

    delay15Clock();

    XM24L_CLK_HIGH;

    delay1Clock();

    XM24L_CLK_LOW;

    delay15Clock();

    XM24L_CLK_HIGH;

    delay1Clock();

    XM24L_CLK_LOW;

    delay15Clock();

    
    //For preventing CLK high time < 50 us
    NVIC_EnableIRQ(TMR2_IRQn);
#ifndef USE_BACK_LIGHT
    NVIC_EnableIRQ(TMR3_IRQn);
#endif
    NVIC_EnableIRQ(UART0_IRQn);
    
    return (INT32S)(bData);
}

#endif//#if defined(AD_DRV_XM24L) || defined(AD_DRV_XM24L_3_3V) 
