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

#ifndef _HARDWARE_CONFIG_AT89C51ED2_ER_MLED_H
#define _HARDWARE_CONFIG_AT89C51ED2_ER_MLED_H

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
/** @brief Device Option : use TD62783 Display Driver IC for ER LED*/
#define DISP_DRV_74HC573_ER_MLED
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
/** @brief Device Option : use Temperature Sensor TCN75A */
#define TEMP_DRV_TCN75A
/** @brief Device Option : use HT1381 RTC IC */
#define TIME_DRV_HT1381
//#define TIME_DRV_DS1302
/** @brief Device Option : use ER(LED) Key Driver */
#define KEY_DRV_ER_MLED
/** @brief Device Option : use Power is Active low */
#define POWER_ON_ACTIVE_LOW
/** @brief Device Option : use Back Light */
//#define USE_BACK_LIGHT
/** @brief Device Option : Battery type */
//#define SALE_PRINTING
#define BATT_TYPE_PB_6V
/** @brief Device Option : Buzzer is Active low */
#define BUZZER_ON_ACTIVE_LOW
/** @brief Device Option : Back Light is Active low */
#define BACK_LIGHT_ON_ACTIVE_HIGH

/** @brief Device Option : Use DispDrvProcForInterrupt() */
#define USE_DISP_DRV_PROC_FOR_INTERRUPT
/** @brief Device Option : Use POWER DRV for ER-MLED */
#define POWER_DRV_ER_MLED
#define USE_TIMER0_2000Hz
/** @brief Xtal frequency */
#define OSC            11059200L

/** @brief KEY_OUT_PORT for key scan*/
#define LED_SEG_DATA	P0
#define LED_DIGIT_DATA	P0

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
/** @brief BACK_LIGHT, �� P2^4*/
sbit BACK_LIGHT = P2^4;
/** @brief Calibration switch*/
sbit SW_CAL = P1^0;

//sbit BAT_CHG  = P1^7;
//sbit BAT_FULL  = P1^6;
sbit POWER_CHECK = P1^6;
//** @brief DISPLAY PowerON

sbit POWER_DISPLAY_SW = P1^7;
/** @brief LED */
/** @brief LED Segment Latch*/
sbit LED_DIGIT_LATCH	= P3^2;
/** @brief LED Segment Output Enable*/
sbit LED_DIGIT_OUT_ENABLE	= P3^3;
/** @brief LED Digit1~8 Latch*/
sbit LED_SEG1_LATCH	= P2^0;
/** @brief LED Digit9~16 Latch*/
sbit LED_SEG2_LATCH	= P2^1;
/** @brief LED Digit17~20 Latch*/
sbit LED_SEG3_LATCH	= P2^2;
/** @brief LED Digit Output Enable*/
sbit LED_SEG_OUT_ENABLE	= P2^3;

/** @brief LCD_PIN_STATE*/
/** @brief LCD Chip select*/
//sbit LCD_CS = P2^0;
/** @brief LCD Write signal*/
//sbit LCD_WR = P2^2;
/** @brief LCD Data signal*/
//sbit LCD_DATA = P2^1;

/** @brief KEY_STATE*/
/** @brief KEY_OUT_PORT for key scan*/
//#define KEY_OUT_PORT	P0
/** @brief KEY_IN_PORT for key scan*/
#define KEY_IN_PORT		P3
sbit KEY_IN_PORT_0=P3^4;
sbit KEY_IN_PORT_1=P3^5;
sbit KEY_IN_PORT_2=P3^6;
sbit KEY_IN_PORT_3=P3^7;
/** @brief KEY_IN_MASK for key scan*/
#define KEY_IN_MASK		0xf0

/** @brief I2C_PIN_STATE*/
/** @brief I2C Serial Clock*/
sbit I2C_SCLK = P2^6;
/** @brief I2C Serial Data*/
sbit I2C_SDA = P2^5;

/** @brief HT1381*/
/** @brief HT1381 Reset*/
sbit HT1381_RST = P2^7;
/** @brief HT1381 Serial clock, I2C clock�� ����*/
sbit HT1381_SCLK = P2^6;
/** @brief HT1381 Data Input & Output, �� P1^1*/
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
#define delay50Clock(){ _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();\
						_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();\
						_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();\
						_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();\
						_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}

#define delay20Clock(){ _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();\
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


