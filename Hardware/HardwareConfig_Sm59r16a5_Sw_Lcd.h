/**
********************************************************************************
* Copyright (c) 2014 CAS
* @brief   Hardware Configuraiton Header File\n
           for SW LCD
* @file    HardwareConfig_Sm59r16a5_Sw_Lcd.h
* @version 1.0
* @date    2014/02/24
* @author  Keum Young-Kwang (KYK)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2014/02/24 created by KYK \n
********************************************************************************
*/

#ifndef _HARDWARE_CONFIG_SM59R16A5_SW_LCD_H
#define _HARDWARE_CONFIG_SM59R16A5_SW_LCD_H

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
/** @brief Device Option : use HT1621 Display Driver IC for SW LCD */
#define DISP_DRV_HT1621_SW
/** @brief Device Option : use SM59R16A5 CPU */
#define CPU_SM59R16A5
/** @brief Device Option : use SM59R16A5 CPU 1T Mode */
//#define X2_MODE
/** @brief Device Option : use SM59R16A5 Timer Mode */
#define TIMER_DRV_SM59R16A5
/** @brief Device Option : use 24LC64(Microchip)eeprom, External */
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
#define KEY_DRV_SW_LCD
/** @brief Device Option : use Power is Active low */
#define POWER_ON_ACTIVE_LOW
/** @brief Device Option : use Back Light */
#define USE_BACK_LIGHT
/** @brief Device Option : Battery type */
#define BATT_TYPE_DRY_9V
/** @brief Device Option : Buzzer is Active low */
#define BUZZER_ON_ACTIVE_LOW
/** @brief Device Option : Back Light is Active low */
#define BACK_LIGHT_ON_ACTIVE_HIGH
/** @brief Device Option : Use POWER DRV for Std */
#define POWER_DRV_STD
/** @brief Device Option : AD Driver use non default AD Freq (use when X2 mode is off) */
#define AD_DRV_FREQ		8L

/** @brief Xtal frequency */
//#define OSC		11059200L
#define OSC		4000000L

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
sbit POWER_CON = P2^3;
/** @brief Power Key check*/
sbit POWER_KEY_CHK = P2^0;

/** @brief LCD_PIN_STATE */
/** @brief LCD Chip select*/
//sbit LCD_CS = P3^2;
sbit LCD_CS		=	P0^0;
/** @brief LCD Write signal*/
//sbit LCD_WR = P3^3;
sbit LCD_WR		=	P0^1;
/** @brief LCD Data signal*/
//sbit LCD_DATA = P3^4;
sbit LCD_DATA	=	P0^2;

/** @brief KEY_PORT */
/** @brief KEY_PORT_TARE*/
sbit KEY_PORT_TARE = P2^1;
/** @brief KEY_PORT_ZERO*/
sbit KEY_PORT_ZERO = P2^2;

/** @brief KEY_PORT_MODE_HOLD*/
sbit KEY_PORT_MODE_HOLD = P3^7;

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
