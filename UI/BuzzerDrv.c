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
    /** @brief KR : Buzzer On �Լ� \n
			  US : \n
			  CN :									*/
	#define BuzzerOn()                  BUZZER_CLR
	/** @brief KR : Buzzer Off �Լ� \n
			  US : \n
			  CN :									*/
	#define BuzzerOff()                 BUZZER_SET
	#else
    /** @brief KR : Buzzer On �Լ� \n
			  US : \n
			  CN :									*/
	#define BuzzerOn()                  BUZZER_SET
	/** @brief KR : Buzzer Off �Լ� \n
			  US : \n
			  CN :									*/
	#define BuzzerOff()                 BUZZER_CLR
	#endif

#else

#ifndef USE_HT1622_BUZZER_DRV
	#ifdef BUZZER_ON_ACTIVE_LOW
	/** @brief KR : Buzzer On �Լ� \n
			  US : \n
			  CN :									*/
	#define BuzzerOn()		{BUZZER = 0;}
	/** @brief KR : Buzzer Off �Լ� \n
			  US : \n
			  CN :									*/
	#define BuzzerOff()		{BUZZER = 1;}
	#else
	/** @brief KR : Buzzer On �Լ� \n
			  US : \n
			  CN :									*/
	#define BuzzerOn()		{BUZZER = 1;}
	/** @brief KR : Buzzer Off �Լ� \n
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
* @brief       KR : HT1622 IC(Display Driver)�� ����� Buzzer on �Լ�\n
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
* @brief       KR : HT1622 IC(Display Driver)�� ����� Buzzer off �Լ�\n
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
/** @brief KR : Buzzer Ƚ�� ���� \n
		  US : \n
		  CN :									*/
static INT8S buzzerTimes;

/** @brief KR : Buzzer ���� Flag\n
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
* @brief       KR : Buzzer Driver Init �Լ�\n
*           			Buzzer Ƚ�� �ʱ�ȭ\n
*           			Buzzer ��\n
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
void BuzzerDrvInit(void)
{
	buzzerTimes = 0;
	buzzerStatus = OFF;
	BuzzerOff();
}

/**
********************************************************************************
* @brief       KR : Buzzer �︮�� Ƚ�� ����\n
*           			���� Buzzer Driver Procedure�� ȣ��Ǹ� Buzzer �︲\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : Buzzer Driver Procedure�� ���� �ð�(50 ms) ���� ���� �ǹǷ�\n
*          			 ���� ��ü�� Buzzer�� �Ѵ� ������ ���Ƶ� ��\n
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
*           			Buzzer Ƚ�� Check�Ͽ� Buzzer on/off ���� ��Ŵ\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : ���� �ð�(50 ms) ���� ����\n
*           			Timer Interrupt ���� ����\n
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


