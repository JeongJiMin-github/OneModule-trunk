/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Buzzer Driver Source File\n
* @file    BuzzerDrv.c
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

/*
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
/* Insert include files here */
#include "../Common/Globals.h"
#include "../Hardware/HardwareConfig.h"
#include "BuzzerDrv.h"
#ifdef USE_HT1622_BUZZER_DRV
#include "DispDrv.h"
#endif

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

#if defined(BUZZER_DRV_EFM32G)||defined(USE_BUZZER_DRV_STM8L151C8)||defined(BUZZER_DRV_M0516LDN)
	#ifdef BUZZER_ON_ACTIVE_LOW
    /** @brief KR : Buzzer On 함수 \n
			  US : \n
			  CN :									*/
	#define BuzzerOn()                  BUZZER_CLR
	/** @brief KR : Buzzer Off 함수 \n
			  US : \n
			  CN :									*/
	#define BuzzerOff()                 BUZZER_SET
	#else
    /** @brief KR : Buzzer On 함수 \n
			  US : \n
			  CN :									*/
	#define BuzzerOn()                  BUZZER_SET
	/** @brief KR : Buzzer Off 함수 \n
			  US : \n
			  CN :									*/
	#define BuzzerOff()                 BUZZER_CLR
	#endif

#else

#ifndef USE_HT1622_BUZZER_DRV
	#ifdef BUZZER_ON_ACTIVE_LOW
	/** @brief KR : Buzzer On 함수 \n
			  US : \n
			  CN :									*/
	#define BuzzerOn()		{BUZZER = 0;}
	/** @brief KR : Buzzer Off 함수 \n
			  US : \n
			  CN :									*/
	#define BuzzerOff()		{BUZZER = 1;}
	#else
	/** @brief KR : Buzzer On 함수 \n
			  US : \n
			  CN :									*/
	#define BuzzerOn()		{BUZZER = 1;}
	/** @brief KR : Buzzer Off 함수 \n
			  US : \n
			  CN :									*/
	#define BuzzerOff()		{BUZZER = 0;}
	#endif
#endif

#endif





/*
********************************************************************************
*                       LOCAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert local typedef & structure here */
#ifdef USE_HT1622_BUZZER_DRV
/**
********************************************************************************
* @brief       KR : HT1622 IC(Display Driver)를 사용한 Buzzer on 함수\n
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
void BuzzerOn(void)
{	
	//HT1622_TONE_2K = 0x60
	//HT1622_TONE_4K = 0x40	
	LCD_CS = 0;

	LCD_WR = 0;	
	LCD_DATA = 1;
	delay2Clock();
	LCD_WR = 1;  ///1
	delay2Clock();

	LCD_WR = 0;
	LCD_DATA = 0;
	delay2Clock();
	LCD_WR = 1;  //0
	delay2Clock();

	LCD_WR = 0;
	LCD_DATA = 0;
	delay2Clock();
	LCD_WR = 1;  //0
	delay2Clock();

	ht1622TranCommand(0x40);

	LCD_WR = 0;
	delay2Clock();
	LCD_WR = 1;  
	delay2Clock();
	LCD_CS = 1;
}

/**
********************************************************************************
* @brief       KR : HT1622 IC(Display Driver)를 사용한 Buzzer off 함수\n
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
void BuzzerOff(void)
{
	//HT1622_TONE_OFF = 0x08
	LCD_CS = 0;

	LCD_WR = 0;	
	LCD_DATA = 1;
	delay2Clock();
	LCD_WR = 1;  ///1
	delay2Clock();

	LCD_WR = 0;
	LCD_DATA = 0;
	delay2Clock();
	LCD_WR = 1;  //0
	delay2Clock();

	LCD_WR = 0;
	LCD_DATA = 0;
	delay2Clock();
	LCD_WR = 1;  //0
	delay2Clock();

	ht1622TranCommand(0x08);

	LCD_WR = 0;
	delay2Clock();
	LCD_WR = 1;  
	delay2Clock();
	LCD_CS = 1;
}
#endif

/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/
/* Insert file scope variable & tables here */
/** @brief KR : Buzzer 횟수 변수 \n
		  US : \n
		  CN :									*/
static INT8S buzzerTimes;

/** @brief KR : Buzzer 상태 Flag\n
		  US : \n
		  CN :									*/
static BOOLEAN buzzerStatus;

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
* @brief       KR : Buzzer Driver Init 함수\n
*           			Buzzer 횟수 초기화\n
*           			Buzzer 끔\n
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
void BuzzerDrvInit(void)
{
	buzzerTimes = 0;
	buzzerStatus = OFF;
	BuzzerOff();
}

/**
********************************************************************************
* @brief       KR : Buzzer 울리는 횟수 셋팅\n
*           			그후 Buzzer Driver Procedure가 호출되면 Buzzer 울림\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : Buzzer Driver Procedure는 일정 시간(50 ms) 마다 실행 되므로\n
*          			 셋팅 자체가 Buzzer를 켜는 것으로 보아도 됨\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BuzzerDrvSetOnTimes(INT8S times)
{
	if (buzzerTimes == 1)
	{
		buzzerTimes = buzzerTimes + times;
	}
	else
	{
		buzzerTimes = times;
	}
}

/**
********************************************************************************
* @brief       KR : Buzzer Driver Procedure\n
*           			Buzzer 횟수 Check하여 Buzzer on/off 동작 시킴\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : 일정 시간(50 ms) 마다 실행\n
*           			Timer Interrupt 에서 실행\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void BuzzerDrvProc(void)
{
	if (buzzerTimes > 0 && buzzerStatus == OFF) 
	{
		BuzzerOn();
		buzzerTimes--;
		buzzerStatus = ON;
	}
	else if (buzzerStatus) 
	{
		BuzzerOff();
		buzzerStatus = OFF;
	}

}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */


