/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Hardware Configuraiton Header File\n
           for SW LCD
* @file    HardwareConfig_At89c51ed2_PB_Lcd.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _HARDWARE_CONFIG_AT89C51ED2_PB_LCD_H
#define _HARDWARE_CONFIG_AT89C51ED2_PB_LCD_H

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
#define BATT_DRV_CS5550_PB
/** @brief Device Option : use HT1621 Display Driver IC for SW LCD */
//#define DISP_DRV_HT1621_SW
/** @brief Device Option : use HT1621 Display Driver IC for PB LCD */
#define DISP_DRV_HT1621_PB
#define SWAP_DISP_DRIVER_SEG_PIN
/** @brief Device Option : use AT89C51ED2 CPU */
#define CPU_AT89C51ED2
/** @brief Device Option : use AT89C51ED2 CPU X2 Mode */
//#define X2_MODE
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
//#define KEY_DRV_SW_LCD
/** @brief Device Option : use PB(LCD) Key Driver */
#define KEY_DRV_PB_LCD
/** @brief Device Option : use Power is Active low */
//#define POWER_ON_ACTIVE_LOW
/** @brief Device Option : use Back Light */
#define USE_BACK_LIGHT


/** @brief Device Option : Battery type */
#define BATT_TYPE_PB_6V
/** @brief Device Option : Dry Battery with PB Battery type */
#define BATT_TYPE_DRY_6V_PB_6V_WITH_CHECK_PORT
/** @brief Device Option : Battery type */
//#define BATT_TYPE_NOT_INSTALLED  //for test only PCB_rev_A


/** @brief Device Option : Buzzer is Active low */
#define BUZZER_ON_ACTIVE_LOW
/** @brief Device Option : Back Light is Active low */
#define BACK_LIGHT_ON_ACTIVE_HIGH
/** @brief Device Option : Use POWER DRV for Std */
#define POWER_DRV_STD

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
sbit BACK_LIGHT = P3^6;
/** @brief Calibration switch*/
sbit SW_CAL = P1^0;
/** @brief Power control*/
sbit POWER_CON = P2^3;
/** @brief Power Key check*/
sbit POWER_KEY_CHK = P2^0;
/** @brief Dry Battery Install Check*/
sbit BATT_DRY_CHK = P1^6;

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
/** @brief KEY_PORT_ZERO*/
sbit KEY_PORT_ZERO = P2^2;
/** @brief KEY_PORT_PRINT*/
//sbit KEY_PORT_PRINT = P3^2;
/** @brief KEY_PORT_HOLD*/
//sbit KEY_PORT_HOLD = P3^4;
/** @brief KEY_PORT_SAMPLE*/
//sbit KEY_PORT_SAMPLE = P3^3;
/** @brief KEY_PORT_SET*/
//sbit KEY_PORT_SET = P3^7;
/** @brief KEY_PORT_MODE*/
//sbit KEY_PORT_MODE = P3^5;
/** @brief KEY_PORT_TARE*/
sbit KEY_PORT_TARE = P2^1;
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
