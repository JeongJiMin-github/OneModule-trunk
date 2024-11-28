/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Hardware Configuraiton Header File\n
           for RB LCD
* @file    HardwareConfig_At89c51ed2_Rb_Lcd.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _HARDWARE_CONFIG_AT89C51ED2_S2000JR_M_LCD_H
#define _HARDWARE_CONFIG_AT89C51ED2_S2000JR_M_LCD_H

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
/** @brief Device Option : use HT1622 Display Driver IC for RB LCD*/
#define DISP_DRV_HT1622_S2000JR_M
/** @brief Device Option : use AT89C51ED2 CPU */
#define CPU_AT89C51ED2
/** @brief Device Option : use AT89C51ED2 CPU X2 Mode */
#define X2_MODE
/** @brief Device Option : use AT89C51ED2 Timer Mode */
#define TIMER_DRV_AT89C51ED2_STD
/** @brief Device Option : use 24LC64(Microchip)eeprom, External */
#define STORAGE_DRV_M24LC64T
/** @brief Device Option : use MSC-51 Serial */
#define COMM_DRV_C51
/** @brief Device Option : use Temperature Sensor TCN75A or NONE*/
//#define TEMP_DRV_TCN75A
#define TEMP_DRV_NONE
/** @brief Device Option : use HT1381 RTC IC */
#define TIME_DRV_HT1381
/** @brief Device Option : use RB(LCD) Key Driver */
#define KEY_DRV_S2000JR_M_LCD
/** @brief Device Option : use Power is Active low */
#define POWER_ON_ACTIVE_LOW
/** @brief Device Option : use Back Light */
#define USE_BACK_LIGHT
/** @brief Device Option : Battery type */
#define BATT_TYPE_PB_6V
/** @brief Device Option : Buzzer is Active low */
#define BUZZER_ON_ACTIVE_LOW
/** @brief Device Option : Back Light is Active low */
#define BACK_LIGHT_ON_ACTIVE_HIGH
/** @brief Device Option : Use POWER DRV for Std */
#define POWER_DRV_STD

/** @brief Xtal frequency */
#define OSC            11059200L

/** @brief AD_PIN_STATE*/
/** @brief AD Chip select*/
sbit AD_CS = P1^5;     	// Active Low
/** @brief AD Data out*/
sbit AD_DOUT = P1^4;
/** @brief AD Serial clock*/
sbit AD_SCLK = P1^3;
/** @brief AD Data in*/
sbit AD_DIN = P1^2;

/** @brief BUZZER*/
sbit BUZZER = P0^3;
/** @brief BACK_LIGHT, 구 P2^4*/
sbit BACK_LIGHT = P1^1;
/** @brief Calibration switch*/
sbit SW_CAL = P1^0;
/** @brief Power control*/
sbit POWER_CON = P1^7;
/** @brief Power Key check*/
sbit POWER_KEY_CHK = P1^6;

/** @brief LCD_PIN_STATE*/
/** @brief LCD Chip select*/
sbit LCD_CS 		= P2^3;
/** @brief LCD Write signal*/
sbit LCD_WR 		= P2^2;
/** @brief LCD Data signal*/
sbit LCD_DATA 	= P2^1;
/** @brief KEY_IN_PORT_REMAIN1 for key scan*/
sbit KEY_IN_PORT_REMAIN1 = P2^0;
/** @brief KEY_IN_PORT_REMAIN1 for key scan*/
sbit KEY_IN_PORT_REMAIN2 = P0^7;

/** @brief KEY_STATE*/
/** @brief KEY_IN_PORT for key scan*/
#define KEY_IN_PORT		P3
/** @brief KEY_IN_MASK for key scan*/
#define KEY_IN_MASK		0x3f

/** @brief KEY OUT bit 0 for 74HC138*/
sbit KEY_OUT0 	= P0^0;
/** @brief KEY OUT bit 1 for 74HC138*/
sbit KEY_OUT1 	= P0^1;
/** @brief KEY OUT bit 2 for 74HC138*/
sbit KEY_OUT2 	= P0^2;

/** @brief I2C_PIN_STATE*/
/** @brief I2C Serial Clock*/
sbit I2C_SCLK = P2^6;
/** @brief I2C Serial Data*/
sbit I2C_SDA = P2^5;

/** @brief HT1381*/
/** @brief HT1381 Reset*/
sbit HT1381_RST = P2^7;
/** @brief HT1381 Serial clock, I2C clock와 공통*/
sbit HT1381_SCLK = P2^6;
/** @brief HT1381 Data Input & Output, 구 P1^1*/
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

