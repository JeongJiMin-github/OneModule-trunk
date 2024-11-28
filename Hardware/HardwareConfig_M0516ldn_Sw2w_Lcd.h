/**
********************************************************************************
* Copyright (c) 2018 CAS
* @brief   Hardware Configuraiton Header File\n
           for OMV4 SW2W LCD
* @file    HardwareConfig_M0516ldn_Sw2w_Lcd.h
* @version 1.0
* @date    2018/09/18
* @author  Park Young Min (FDC954)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2018/09/18 created by FDC954 \n
********************************************************************************
*/

#ifndef _HARDWARE_CONFIG_M0516LDN_SW2W_LCD_H
#define _HARDWARE_CONFIG_M0516LDN_SW2W_LCD_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief Device Option : use OB1020 AD IC for L/C */
#define AD_DRV_OB1020
#ifdef USE_CHECK_AD_VENDOR
/** @brief Device Option : use XM24L AD IC for L/C */
    #define AD_DRV_XM24L_3_3V //AD_DRV_XM24L
#endif
/** @brief Device Option : use M0516LDN AD IC for Battery Voltage */
#define BATT_DRV_M0516LDN
/** @brief Device Option : use CH462 Driver IC for M0516LDN SW2W LCD */
#define DISP_DRV_CH462_M0516LDN_SW2W_LCD
/** @brief Device Option : use M0516LDN CPU */
#define CPU_M0516LDN
/** @brief Device Option : use DRV_M0516LDN Timer Mode */
#define TIMER_DRV_M0516LDN
/** @brief Device Option : use AT24C64D eeprom */
#define STORAGE_DRV_AT24C64D
/** @brief Device Option : use M0516LDN Serial */
#define COMM_DRV_M0516LDN
/** @brief Device Option : use Temperature Sensor TCN75A or NONE*/
#define TEMP_DRV_NONE
/** @brief Device Option : use RX8010 RTC IC */
#define TIME_DRV_RX8010
/** @brief Device Option : use Key Driver */
//#define KEY_DRV_SW_LCD
#define KEY_DRV_OMV4_SW2W_LCD
/** @brief Device Option : use M0516LDN MCU AD (POWER CHECK, BATT CHECK, TEMP CHECK) */
#define AD_M0516LDN

/** @brief Device Option : use Back Light */
#define USE_BACK_LIGHT
//#define BATT_DIODE_DROP
/** @brief Device Option : use M0516LDN Buzzer Driver */
#define BUZZER_DRV_M0516LDN
/** @brief Device Option : Battery type */
#define BATT_TYPE_DRY_6V

/** @brief Device Option : Buzzer is Active low */
#define BUZZER_ON_ACTIVE_LOW
/** @brief Device Option : Power is Active low */
#define POWER_ON_ACTIVE_LOW
/** @brief Device Option : Use POWER DRV for Std */
#define POWER_DRV_STD
/** @brief Device Option : AD Driver use non default AD Freq (use when X2 mode is off) */
#define AD_DRV_FREQ		10L         //duckspg 8L -> 10L

/** OMV4 PORT */
#include "M051Series.h"
#include "gpio.h"

/* - a/d module*/
#define OB1020_CLK       P14
#define OB1020_CLK_HIGH  {OB1020_CLK = 1;}
#define OB1020_CLK_LOW   {OB1020_CLK = 0;}
//
#define OB1020_DOUT      P42
#define OB1020_DOUT_HIGH {OB1020_DOUT = 1;}
#define OB1020_DOUT_LOW  {OB1020_DOUT = 0;}
#define OB1020_DOUT_INCHANGE     {GPIO_SetMode(P4, BIT2, GPIO_PMD_INPUT);}
#define OB1020_DOUT_OUTCHANGE    {GPIO_SetMode(P4, BIT2, GPIO_PMD_OUTPUT);}       

#ifdef USE_CHECK_AD_VENDOR
/* PCB_AD_CHK */
#define PCB_AD_CHK       P27
#endif

#ifdef AD_DRV_XM24L_3_3V //AD_DRV_XM24L
/* - XM24L a/d module*/
#define XM24L_CLK       P14
#define XM24L_CLK_HIGH  {XM24L_CLK = 1;}
#define XM24L_CLK_LOW   {XM24L_CLK = 0;}

#define XM24L_DOUT      P42 
#define XM24L_DOUT_HIGH {XM24L_DOUT = 1;}
#define XM24L_DOUT_LOW  {XM24L_DOUT = 0;}
#define XM24L_DOUT_INCHANGE     {GPIO_SetMode(P4, BIT2, GPIO_PMD_INPUT);}
#define XM24L_DOUT_OUTCHANGE    {GPIO_SetMode(P4, BIT2, GPIO_PMD_OUTPUT);}
#endif

/* - buzzer module*/
#define BUZZER          P24
#define BUZZER_SET      {BUZZER = 1;}
#define BUZZER_CLR      {BUZZER = 0;}

/* - power module*/
#define POWER_CON       P23
#define POWER_CON_ON    {POWER_CON = 1;}
#define POWER_CON_OFF   {POWER_CON = 0;}
#define POWER_KEY_CHK   P20

/* - key module*/
#define SW_CAL              P10
#define KEY_PORT_ZERO       P22
#define KEY_PORT_TARE       P21
#define KEY_PORT_MODE_HOLD  P37

/** @brief BACK_LIGHT*/
#define BACK_LIGHT	    P35

/* - display module*/
#define CH462_CS       P00
#define CH462_CS_HIGH  {CH462_CS = 1;}
#define CH462_CS_LOW   {CH462_CS = 0;}

#define CH462_WR       P01
#define CH462_WR_HIGH  {CH462_WR = 1;}
#define CH462_WR_LOW   {CH462_WR = 0;}

#define CH462_SDA_DATA     P02
#define CH462_SDA_HIGH	    {CH462_SDA_DATA = 1;}
#define CH462_SDA_LOW      {CH462_SDA_DATA = 0;}
#define CH462_SDA_INCHANGE     {GPIO_SetMode(P0, BIT2, GPIO_PMD_INPUT);}
#define CH462_SDA_OUTCHANGE    {GPIO_SetMode(P0, BIT2, GPIO_PMD_OUTPUT);}

/* - Enable eeprom module*/
#define EEPROM_CLK      P44
#define EEPROM_CLK_HIGH {EEPROM_CLK = 1;}
#define EEPROM_CLK_LOW  {EEPROM_CLK = 0;}

#define EEPROM_SDA_DATA P45
#define EEPROM_SDA_HIGH {EEPROM_SDA_DATA = 1;}
#define EEPROM_SDA_LOW  {EEPROM_SDA_DATA = 0;}
#define EEPROM_SDA_INCHANGE     {GPIO_SetMode(P4, BIT5, GPIO_PMD_INPUT);}
#define EEPROM_SDA_OUTCHANGE    {GPIO_SetMode(P4, BIT5, GPIO_PMD_OUTPUT);}

/* - Enable eeprom module*/
#define RTC_CLK      P25
#define RTC_CLK_HIGH {RTC_CLK = 1;}
#define RTC_CLK_LOW  {RTC_CLK = 0;}

#define RTC_SDA_DATA P26
#define RTC_SDA_HIGH {RTC_SDA_DATA = 1;}
#define RTC_SDA_LOW  {RTC_SDA_DATA = 0;}
#define RTC_SDA_INCHANGE     {GPIO_SetMode(P2, BIT6, GPIO_PMD_INPUT);}
#define RTC_SDA_OUTCHANGE    {GPIO_SetMode(P2, BIT6, GPIO_PMD_OUTPUT);}

/** @brief delay1Clock*/
#define delay1Clock()	{asm("nop\n");}
/** @brief delay2Clock*/
#define	delay2Clock()	{asm("nop\n");asm("nop\n");}
/** @brief delay3Clock*/
#define	delay3Clock()	{asm("nop\n");asm("nop\n");asm("nop\n");}
/** @brief delay4Clock*/
#define	delay4Clock()	{asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");}
/** @brief delay5Clock*/
#define	delay5Clock()	{asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");}
/** @brief delay6Clock*/
#define	delay6Clock()	{asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");}
/** @brief delay50Clock*/
#define delay15Clock()	{asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");\
			asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");\
                          asm("nop\n");asm("nop\n");asm("nop\n");}
/** @brief delay50Clock*/
#define delay50Clock()	{asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");\
						asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");\
						asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");\
						asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");\
						asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");\
						asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");\
						asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");\
						asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");\
						asm("nop\n");asm("nop\n");}


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

#endif //_HARDWARE_CONFIG_M0516LDN_SW2W_LCD_H
