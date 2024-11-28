/**
********************************************************************************
* Copyright (c) 2007 CAS
* @brief   Hardware Configuraiton Header File\n
           for ER-1 VFD
* @file    HardwareConfig_At89c51ed2_Rb_Lcd.h
* @version 1.0
* @date    2007/03/28
* @author  Jae Joon Hong (HJJ)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2007/03/28 created by HJJ \n
********************************************************************************
*/

#ifndef _HARDWARE_CONFIG_AT89C51ED2_AP_VFD_H
#define _HARDWARE_CONFIG_AT89C51ED2_AP_VFD_H

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
/** @brief Device Option : use UPD16310 Display Driver IC for ER-1 VFD */
#define DISP_DRV_UPD16310_AP_VFD
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
/** @brief Device Option : use ER-1 VFD Key Driver */
#define KEY_DRV_AP_VFD
/** @brief Device Option : use Power is Active low */
#define POWER_ON_ACTIVE_LOW
/** @brief Device Option : use Back Light */
//#define USE_BACK_LIGHT
/** @brief Device Option : Battery type */
#define BATT_TYPE_NOT_INSTALLED
/** @brief Device Option : Buzzer is Active low */
#define BUZZER_ON_ACTIVE_LOW
/** @brief Device Option : Back Light is Active low */
#define BACK_LIGHT_ON_ACTIVE_HIGH
/** @brief Device Option : Use DispDrvProcForInterrupt() */
#define USE_DISP_DRV_PROC_FOR_INTERRUPT
/** @brief Device Option : Use POWER DRV for AC */
#define USE_AC_POWER
/** @brief Device Option : AD Driver use non default AD Freq (use when X2 mode is off) */
#define AD_DRV_FREQ		8L


#define USE_TIMER0_2000Hz

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
sbit BUZZER = P1^1;
/** @brief BACK_LIGHT, 구 P2^4*/
//sbit BACK_LIGHT = P2^4;
/** @brief Calibration switch*/
sbit SW_CAL = P1^0;
/** @brief Power control*/
sbit POWER_CON = P1^7;
/** @brief Power Key check*/
sbit POWER_KEY_CHK = P1^6;

/** @brief KEY_STATE*/
/** @brief KEY_IN_PORT for key scan*/
#define KEY_IN_PORT		P2
/** @brief KEY_IN_MASK for key scan*/
#define KEY_IN_MASK		0x0f

/** @brief LCD_PIN_STATE*/
/** @brief VFD Blank Input signal*/
sbit VFD_BLK = P3^2;
/** @brief VFD Latch signal*/
sbit VFD_STRB = P3^3;
/** @brief VFD Data CLK signal*/
sbit VFD_CLK = P3^4;
/** @brief VFD Data signal*/
sbit VFD_DATA = P3^5;
/** @brief VFD power signal*/
sbit VFD_POWER = P3^7;


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
