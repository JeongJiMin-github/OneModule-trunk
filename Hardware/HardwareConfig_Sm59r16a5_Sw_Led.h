/**
********************************************************************************
* Copyright (c) 2014 CAS
* @brief   Hardware Configuraiton Header File\n
           for SW LED
* @file    HardwareConfig_Sm59r16a5_Sw_Led.h
* @version 1.0
* @date    2014/02/24
* @author  Keum Young-Kwang (KYK)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2014/02/24 created by KYK \n
********************************************************************************
*/

#ifndef _HARDWARE_CONFIG_SM59R16A5_SW_LED_H
#define _HARDWARE_CONFIG_SM59R16A5_SW_LED_H

#include <sm59r16a5.h>

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
/** @brief Device Option : use TD62783 Display Driver IC for SW LED */
#define DISP_DRV_TD62783_SWLR
/** @brief Device Option : use SM59R16A5 CPU */
#define CPU_SM59R16A5
/** @brief Device Option : use SM59R16A5 CPU 1T Mode */
//#define X2_MODE
/** @brief Device Option : use SM59R16A5 Timer Mode */
#define TIMER_DRV_SM59R16A5
/** @brief Device Option : use SM59R16A5 eeprom */
#define STORAGE_DRV_SM59R16A5_M24LC64T
/** @brief Device Option : use MSC-51 Serial */
#define COMM_DRV_SM59R16A5
/** @brief Device Option : use Temperature Sensor TCN75A or NONE*/
//#define TEMP_DRV_TCN75A
#define TEMP_DRV_NONE
/** @brief Device Option : use HT1381 RTC IC */
//#define TIME_DRV_HT1381
#define TIME_DRV_NONE
/** @brief Device Option : use SW(LCD) Key Driver */
#define KEY_DRV_SW_LED
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
/** @brief Device Option : Use DispDrvProcForInterrupt() */
#define USE_DISP_DRV_PROC_FOR_INTERRUPT
/** @brief Device Option : Use counting, compare func. */
#define COUNTING_SCALE
/** @brief Device Option : Use POWER DRV for Std */
#define POWER_DRV_STD

/** @brief Xtal frequency */
//#define OSC		11059200L
#define OSC		4000000L

/** @brief KEY_OUT_PORT for key scan*/
#define LED_SEG_DATA	P0


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
sbit BACK_LIGHT = P2^4;
/** @brief Calibration switch*/
sbit SW_CAL = P1^0;
/** @brief Power control*/
sbit POWER_CON = P1^7;
/** @brief Power Key check*/
sbit POWER_KEY_CHK = P1^6;

/** @brief LED_PIN_STATE */
/** @brief LED Segment A*/
sbit LED_SEG_A 			= P0^0;
/** @brief LED Segment B*/
sbit LED_SEG_B 			= P0^1;
/** @brief LED Segment C*/
sbit LED_SEG_C 			= P0^2;
/** @brief LED Segment D*/
sbit LED_SEG_D 			= P0^3;
/** @brief LED Segment E*/
sbit LED_SEG_E 			= P0^4;
/** @brief LED Segment F*/
sbit LED_SEG_F 			= P0^5;
/** @brief LED Segment G*/
sbit LED_SEG_G 			= P0^6;
/** @brief LED Segment DP*/
sbit LED_SEG_DP 			= P0^7;
/** @brief LED Digit 1*/
sbit LED_SEG_DIGIT_1 	= P3^2;
/** @brief LED Digit 2*/
sbit LED_SEG_DIGIT_2 	= P3^3;
/** @brief LED Digit 3*/
sbit LED_SEG_DIGIT_3 	= P3^4;
/** @brief LED Digit 4*/
sbit LED_SEG_DIGIT_4 	= P3^5;
/** @brief LED Digit 5*/
sbit LED_SEG_DIGIT_5 	= P3^6;
/** @brief LED Digit 6*/
sbit LED_SEG_DIGIT_6 	= P3^7;
/** @brief LED Indicator*/
sbit LED_INDICATOR		= P2^3;

/** @brief KEY_PORT */
/** @brief KEY_PORT_TARE*/
sbit KEY_PORT_TARE = P2^1;
/** @brief KEY_PORT_ZERO*/
sbit KEY_PORT_ZERO = P2^2;
/** @brief KEY_PORT_MODE_HOLD*/
sbit KEY_PORT_MODE_HOLD = P2^0;

/** @brief I2C_PIN_STATE */
/** @brief I2C Serial Clock*/
sbit I2C_SCLK = P2^6;
/** @brief I2C Serial Data*/
sbit I2C_SDA = P2^5;

/** @brief HT1381 */
/** @brief HT1381 Reset*/
sbit HT1381_RST = P2^7;
/** @brief HT1381 Serial clock, I2C clock¿Í °øÅë*/
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

