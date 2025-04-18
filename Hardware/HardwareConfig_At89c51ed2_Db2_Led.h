/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Hardware Configuraiton Header File\n
           for SW LCD
* @file    HardwareConfig_At89c51ed2_Db2_Lcd.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _HARDWARE_CONFIG_AT89C51ED2_DB2_LED_H
#define _HARDWARE_CONFIG_AT89C51ED2_DB2_LED_H

#include <reg51xd2.h>

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief Device Option : use CS5550 AD IC for L/C */
#define AD_DRV_CS5550
/** @brief Device Option : use CS5550 AD IC for Battery Voltage */
#define BATT_DRV_CS5550
/** @brief Device Option : use HT1621 Display Driver IC for SW LCD */
//#define DISP_DRV_AS1106_DB2
#define DISP_DRV_TD62783_DB2
/** @brief Device Option : use AT89C51ED2 CPU */
#define CPU_AT89C51ED2
/** @brief Device Option : use AT89C51ED2 CPU X2 Mode */
#define X2_MODE
/** @brief Device Option : use AT89C51ED2 Timer Mode */
#define TIMER_DRV_AT89C51ED2_STD
/** @brief Device Option : use AT89C51ED2 eeprom */
#define STORAGE_DRV_AT89C51ED2
/** @brief Device Option : use MSC-51 Serial */
#define COMM_DRV_C51
/** @brief Device Option : use Temperature Sensor TCN75A or NONE*/
//#define TEMP_DRV_TCN75A
#define TEMP_DRV_NONE
/** @brief Device Option : use HT1381 RTC IC */
#define TIME_DRV_HT1381
/** @brief Device Option : use SW(LCD) Key Driver */
#define KEY_DRV_DB2_LCD
/** @brief Device Option : use Power is Active low */
#define POWER_ON_ACTIVE_LOW
/** @brief Device Option : use Back Light */
//#define USE_BACK_LIGHT
/** @brief Device Option : Battery type */
#define BATT_TYPE_PB_6V
/** @brief Device Option : Buzzer is Active low */
#define BUZZER_ON_ACTIVE_LOW
/** @brief Device Option : Back Light is Active low */
#define BACK_LIGHT_ON_ACTIVE_HIGH
/** @brief Device Option : Use POWER DRV for Std */
#define POWER_DRV_STD
/** @brief Device Option : Use DispDrvProcForInterrupt() */
#define USE_DISP_DRV_PROC_FOR_INTERRUPT


/** @brief Xtal frequency */
#define OSC            11059200L

/** @brief AD_PIN_STATE */
/** @brief AD Chip select, Active Low*/
sbit AD_CS = P1^5;
/** @brief AD Data out*/
sbit AD_DOUT = P1^4;
/** @brief AD Serial clock*/
sbit AD_SCLK = P1^3;
/** @brief AD Data in*/
sbit AD_DIN = P1^2;

/** @brief BUZZER*/
sbit BUZZER = P1^1;
/** @brief BACK_LIGHT*/
//sbit BACK_LIGHT = P1^1;
/** @brief Calibration switch*/
sbit SW_CAL = P1^0;
/** @brief Power control*/
sbit POWER_CON = P1^7;
/** @brief Power Key check*/
sbit POWER_KEY_CHK = P1^6;

#ifdef DISP_DRV_AS1106_DB2
/** @brief LED_Driver PIN_STATE */
/** @brief LED Data signal */
sbit LED_DIN		=	P2^1;
/** @brief LED Clock signal */
sbit LED_CLK		=	P2^2;
/** @brief LED Load signal */
sbit LED_LOAD	=	P2^3;
#else
/** @brief KEY_OUT_PORT for key scan*/
#define LED_SEG_DATA	P0
/** @brief LED_PIN_STATE */
/** @brief LED Indicator*/
sbit LED_INDICATOR		= P0^0;
/** @brief LED Digit 1*/
sbit LED_SEG_DIGIT_1 	= P0^1;
/** @brief LED Digit 2*/
sbit LED_SEG_DIGIT_2 	= P0^2;
/** @brief LED Digit 3*/
sbit LED_SEG_DIGIT_3 	= P0^3;
/** @brief LED Digit 4*/
sbit LED_SEG_DIGIT_4 	= P0^4;
/** @brief LED Digit 5*/
sbit LED_SEG_DIGIT_5 	= P0^5;
/** @brief LED Digit 6*/
sbit LED_SEG_DIGIT_6 	= P0^6;
/** @brief LED_PIN_STATE */
/** @brief LED Common Latch selection */
sbit COM_CON 	= P2^1;
/** @brief LED Digit Latch selection */
sbit DIGIT_CON 	= P2^2;		
#endif

/** @brief KEY_PORT */
/** @brief KEY_PORT_ZERO*/
sbit KEY_PORT_ZERO = P2^0;
/** @brief KEY_PORT_PRINT*/
sbit KEY_PORT_PRINT = P3^2;
/** @brief KEY_PORT_HOLD*/
sbit KEY_PORT_HOLD = P3^3;
/** @brief KEY_PORT_SAMPLE*/
sbit KEY_PORT_SAMPLE = P3^4;
/** @brief KEY_PORT_SET*/
sbit KEY_PORT_SET = P3^5;
/** @brief KEY_PORT_MODE*/
sbit KEY_PORT_MODE = P3^6;
/** @brief KEY_PORT_TARE*/
sbit KEY_PORT_TARE = P3^7;

/** @brief I2C_PIN_STATE */
/** @brief I2C Serial Clock*/
sbit I2C_SCLK = P2^6;
/** @brief I2C Serial Data*/
sbit I2C_SDA = P2^5;

/** @brief HT1381 */
/** @brief HT1381 Reset*/
sbit HT1381_RST = P2^7;
/** @brief HT1381 Serial clock, I2C clock�� ����*/
sbit HT1381_SCLK = P2^6;
/** @brief HT1381 Data Input & Output*/
sbit HT1381_DIO = P2^4;

#include <intrins.h>
/** @brief delay1Clock*/
#define delay1Clock()	{_nop_();}
/** @brief delay2Clock*/
#define	delay2Clock()	{_nop_();_nop_();}
/** @brief delay3Clock*/
#define	delay3Clock()	{_nop_();_nop_();_nop_();}
/** @brief delay4Clock*/
#define	delay4Clock()	{_nop_();_nop_();_nop_();_nop_();}
/** @brief delay5Clock*/
#define	delay5Clock()	{_nop_();_nop_();_nop_();_nop_();_nop_();}
/** @brief delay6Clock*/
#define	delay6Clock()	{_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}
/** @brief delay50Clock*/
#define delay50Clock()	{_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();\
						_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();\
						_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();\
						_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();\
						_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}
/*
*
********************************************************************************
*                       GLOBAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert global typedef & struct here */


/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLE EXTERNALS
********************************************************************************
*/
/* Insert global variable externals here */


/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */

#endif
